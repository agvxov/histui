#include "tui.hpp"

#include <time.h>
#include <ncurses.h>
#include <readline/readline.h>

extern bool do_execute;
size_t entry_lines;
bool is_input_changed = true;

static WINDOW * main_window;
static WINDOW * entry_window;
static WINDOW * input_window;
static WINDOW * version_window;

static int input_available = false;
static int input;

size_t selection_offset   = 0;
size_t selection_relative = 0;
static size_t entry_line_index   = 0;

static char version_string[] =
#  include "version.inc"
;

static void refresh_input(void);

int init_tui(void) {
    // Ncurses
	initscr();
    nonl();
    halfdelay(1);
	noecho();
	curs_set(0);
    keypad(stdscr, TRUE);

    entry_lines = LINES-3;

	main_window    = newwin(LINES-1, COLS, 0, 0);
    entry_window   = subwin(main_window, entry_lines, COLS-2, 1, 1);
    version_window = subwin(main_window, 1, strlen(version_string), 0, 5);
    input_window   = newwin(1, COLS, LINES-1, 0);
    refresh();

    box(main_window, 0, 0);

    // XXX
    waddstr(version_window, version_string);
    wrefresh(version_window);

	// Readline
	rl_bind_key('\t', rl_insert);
	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	rl_prep_term_function = NULL;
	rl_deprep_term_function = NULL;
	rl_change_environment = 0;

	rl_getc_function = []([[maybe_unused]] FILE* ignore){
		input_available = false;
		return (int)input;
	};
	rl_input_available_hook = []{
		return input_available;
	};
	rl_redisplay_function = refresh_input;
    /* We must specify an input handler or readline chimps out,
     *  but we dont want the line to be actually submittable,
     *  (search is continous and that would delete what the user
     *   has typedso far)
     *  so we also override enter to do nothing.
     */
	rl_callback_handler_install("", []([[maybe_unused]] char *line){ ; });
    rl_bind_key('\n', []([[maybe_unused]] int i, [[maybe_unused]] int h){ return 0; });

    return 0;
}

int deinit_tui(void) {
	endwin();
    return 0;
}

void tui_append_back(const entry_t entry) {
    struct tm *tm_info = localtime((time_t*)&entry.timestamp);
    const int TIME_BUFFER_SIZE = 30;
    char time_buffer[TIME_BUFFER_SIZE];
    const int time_len = 19;
    strftime(time_buffer, TIME_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", tm_info);

    if (entry_line_index == selection_relative) {
        wattron(entry_window, A_REVERSE);
    }
    mvwprintw(entry_window, (entry_lines-1)-entry_line_index, 0,
                    "%s  %.*s",
                        time_buffer,
                        COLS-2-time_len-2, // XXX: this is horrible
                        entry.command
                );
    if (entry_line_index == selection_relative) {
        wattroff(entry_window, A_REVERSE);
    }

    ++entry_line_index;
}

static void refresh_input(void) {
    entry_line_index = 0;
	wmove(input_window, 0, 0);
	wclrtoeol(input_window);
	waddstr(input_window, "$ ");
	waddstr(input_window, rl_line_buffer);
    waddch(input_window, ACS_BLOCK);
	wrefresh(input_window);
}

void tui_refresh(void) {
    wmove(entry_window, 0, 0);
    wrefresh(entry_window);
    wrefresh(main_window);
    refresh_input();

    wclear(entry_window);
}


void tui_take_input(void) {
    extern bool do_run;
    const size_t paging_size = entry_lines / 2;

	input = wgetch(stdscr);
    switch (input) {
        case KEY_UP:
        case CTRL('p'):
        case CTRL('k'): {
            if (selection_relative != entry_lines-1) {
                ++selection_relative;
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
            } else
            if (selection_offset > paging_size) {
                selection_offset -= paging_size;
            } else {
                selection_offset = 0;
            }
            is_input_changed = true;
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
}
