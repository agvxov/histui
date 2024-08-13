#include "tui.h"

#include <time.h>
#include <ncurses.h>
#include <readline/readline.h>

#include "caret_notater.h"

extern bool do_execute;

/* "Cursor" position; the entry selected by the user
 */
size_t selection_relative = 0;
size_t selection_offset   = 0;
size_t entry_lines;

bool is_input_changed = true;
bool do_redisplay     = true;

/* Counter to signal which line to print to,
 *  and saved counterpart which is used to
 *  determine how many lines were printed.
 */
static size_t entry_line_index      = 0;
static size_t last_entry_line_index = 0;

static char version_string[] =
#  include "version.inc"
;

// Ncurses fun
static WINDOW * main_window;
static WINDOW * entry_window;
static WINDOW * input_window;
static WINDOW * version_window;

// Readline requirements
static int input_available = false;
static int input;

//
static void refresh_input(void);
static void full_redraw(void);

static bool do_fullredraw = true;

int init_tui(void) {
    // Ncurses
	initscr();
    nonl();
    cbreak();
	noecho();
	curs_set(0);
    keypad(stdscr, TRUE);

    entry_lines = LINES-3;

	main_window    = newwin(LINES-1, COLS,       0, 0);
    input_window   = newwin(      1, COLS, LINES-1, 0);
    entry_window   = subwin(main_window, entry_lines,                 COLS-2, 1, 1);
    version_window = subwin(main_window,           1, strlen(version_string), 0, 5);
    refresh();

	// Readline
	rl_bind_key('\t', rl_insert);
	rl_catch_signals        = 0;
	rl_catch_sigwinch       = 0;
	rl_change_environment   = 0;
	rl_prep_term_function   = NULL;
	rl_deprep_term_function = NULL;

    int getc_function([[maybe_unused]] FILE* ignore) { input_available = false; return (int)(input); }
    int return_input_available(void) { return input_available; }
	rl_getc_function        = getc_function;
	rl_input_available_hook = return_input_available;
	rl_redisplay_function   = refresh_input;
    /* We must specify an input handler or readline chimps out,
     *  but we dont want the line to be actually submittable,
     *  (search is continous and that would delete what the user
     *   has typedso far)
     *  so we also override enter to do nothing.
     */
    void no_op_handler([[maybe_unused]] char *line) { ; }
    int  no_op_bind([[maybe_unused]] int i, [[maybe_unused]] int h) { return 0; }
	rl_callback_handler_install("", no_op_handler);
    rl_bind_key('\n', no_op_bind);

    return 0;
}

int deinit_tui(void) {
	endwin();
    return 0;
}

void tui_append_back(const entry_t entry) {
    struct tm *tm_info = localtime((time_t*)&entry.timestamp);
    const int TIME_SIZE = 20;
    char time_buffer[TIME_SIZE];
    strftime(time_buffer, TIME_SIZE, "%Y-%m-%d %H:%M:%S", tm_info);
    const size_t entry_len = strlen(entry.command);
    char caret_notation_buffer[entry_len*2];

    if (entry_line_index == selection_relative) {
        wattron(entry_window, A_REVERSE);
    }
    mvwprintw(entry_window, (entry_lines-1)-entry_line_index, 0,
                    "%s  %.*s",
                        time_buffer,
                        COLS-2-(TIME_SIZE-1)-2, // XXX: this is horrible
                        string_to_caret_notation(entry.command,
                                                    entry_len,
                                                    caret_notation_buffer
                                                )
                );
    if (entry_line_index == selection_relative) {
        wattroff(entry_window, A_REVERSE);
    }

    if (getcury(entry_window) == (entry_lines-1)-entry_line_index) {
        wclrtoeol(entry_window);
    }

    ++entry_line_index;
}

static
void full_redraw(void) {
    box(main_window, 0, 0);

    waddstr(version_window, version_string);
    wrefresh(version_window);

    refresh_input();
}

static
void refresh_input(void) {
	wmove(input_window, 0, 0);
	wclrtoeol(input_window);
	waddstr(input_window, "$ ");
	waddstr(input_window, rl_line_buffer);
    waddch(input_window, ACS_BLOCK);
	wrefresh(input_window);
}

void tui_refresh(void) {
    if (do_fullredraw) {
        do_fullredraw = false;
        full_redraw();
        return;
    }

    last_entry_line_index = entry_line_index;
    if (entry_line_index+1 < entry_lines) {
        while (entry_line_index != entry_lines) {
            wmove(entry_window, entry_lines-entry_line_index++, 0);
            wclrtoeol(entry_window);
        }
        wmove(entry_window, entry_lines-entry_line_index, 0);
        wclrtoeol(entry_window);
    }
    entry_line_index = 0;

    refresh_input();

    wrefresh(entry_window);
    wmove(entry_window, 0, 0);
    wrefresh(main_window);
}


void tui_take_input(void) {
    extern bool do_run;
    const size_t paging_size = entry_lines / 2;

	input = wgetch(stdscr);
    switch (input) {
        case KEY_UP:
        case CTRL('p'):
        case CTRL('k'): {
            if (selection_relative == last_entry_line_index-2) {
                break;
            }
            if (selection_relative != entry_lines-1) {
                ++selection_relative;
                do_redisplay = true;
            } else {
                ++selection_offset;
                is_input_changed = true;
            }
        } break;
        case KEY_DOWN:
        case CTRL('n'):
        case CTRL('j'): {
            if (selection_relative != 0) {
                --selection_relative;
                do_redisplay = true;
            } else {
                if (selection_offset != 0) {
                    --selection_offset;
                    is_input_changed = true;
                }
            }
        } break;
        case KEY_PPAGE:
        case CTRL('u'): {
            selection_offset += paging_size;
            is_input_changed = true;
        } break;
        case KEY_NPAGE:
        case CTRL('d'): {
            if (selection_offset == 0) {
                selection_relative = 0;
                do_redisplay = true;
            } else
            if (selection_offset > paging_size) {
                selection_offset -= paging_size;
                is_input_changed = true;
            } else {
                selection_offset = 0;
                is_input_changed = true;
            }
        } break;
        case CTRL('q'): {
            do_execute = false;
            do_run = false;
        } break;
        case '\r': {
            do_run = false;
        } break;
        case ERR: break;
        default: {
            input_available = true;
            rl_callback_read_char();
            is_input_changed = true;
        } break;
    }

    if (is_input_changed) {
        do_redisplay = true;
    }
}
