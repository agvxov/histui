#include "tui.h"

#include <time.h>
#include <ncurses.h>
#include <readline/readline.h>

#include "caret_notater.h"

/* I fucking hate readline.
 * Apparently the only way to set an initial value is using a hook.
 * What makes this extra painful is that readline cannot be explicitly
 *  initialized nor is it documented clearly that shit will segfault
 *  otherwise.
 * XXX: If I ever find out what is a sensible alternative im ditching it forever.
 */
const char * initial_text;

const char * get_input_line(void) { return rl_line_buffer; }

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
static inline void update_input(void);
static void full_redraw(void);
static void resize(void);
static inline void init_windows();
static inline void deinit_windows();

static bool do_fullredraw = true;

int init_tui(void) {
    // Ncurses
    initscr();
    nonl();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    init_windows();

    // Readline
    rl_bind_key('\t', rl_insert);
    rl_catch_signals        = 0;
    rl_catch_sigwinch       = 0;
    rl_change_environment   = 0;
    rl_prep_term_function   = NULL;
    rl_deprep_term_function = NULL;

    int getc_function([[maybe_unused]] FILE* ignore) { input_available = false; return (int)(input); }
    int return_input_available(void) { return input_available; }
    int initializer(void) { rl_insert_text(initial_text); return 0; }
    rl_getc_function        = getc_function;
    rl_input_available_hook = return_input_available;
    rl_startup_hook         = initializer;
    /* Due to this bug: https://mail.gnu.org/archive/html/bug-readline/2013-09/msg00021.html ;
     *  we cannot null this function.
     * Im seriously questioning why readline is still the """default""" library in the wild
     *  and whether i should participate.
     */
    void redisplay_nop(void) { return; }
    rl_redisplay_function   = redisplay_nop;
    /* We must specify an input handler or readline chimps out,
     *  but we dont want the line to be actually submittable,
     *  (search is continous and that would delete what the user
     *   has typed so far)
     *  so we also override enter to do nothing.
     */
    void no_op_handler([[maybe_unused]] char *line) { return; }
    int  no_op_bind([[maybe_unused]] int i, [[maybe_unused]] int h) { return 0; }
	rl_callback_handler_install("", no_op_handler);
    rl_bind_key('\n', no_op_bind);

    return 0;
}

int deinit_tui(void) {
    deinit_windows();
	endwin();
    return 0;
}

void tui_append_back(const entry_t entry) {
    const int TIME_SIZE = 19 + 1;
    char time_buffer[TIME_SIZE];
    strftime(time_buffer, TIME_SIZE, "%Y-%m-%d %H:%M:%S", localtime((time_t*)&entry.timestamp));
    char caret_notation_buffer[(strlen(entry.command)*2)+1];
    const size_t current_line_y = (entry_lines-1)-entry_line_index;

    if (entry_line_index == selection_relative) {
        wattron(entry_window, A_REVERSE);
    }
    mvwprintw(entry_window, current_line_y, 0,
                    "%s  %.*s",
                        time_buffer,
                        getmaxx(entry_window) - (TIME_SIZE-1) - /*space-padding*/2,
                        string_to_caret_notation(entry.command,
                                                    strlen(entry.command),
                                                    caret_notation_buffer
                                                )
                );
    if (entry_line_index == selection_relative) {
        wattroff(entry_window, A_REVERSE);
    }

    /* Only delete to the end of the line if the cursor did not overflow
     *  to the line below from a long entry.
     */
    if ((size_t)getcury(entry_window) == current_line_y) {
        wclrtoeol(entry_window);
    }

    ++entry_line_index;
}

static inline
void init_windows() {
    entry_lines = LINES-3;

	main_window    = newwin(LINES-1, COLS,       0, 0);
    input_window   = newwin(      1, COLS, LINES-1, 0);
    entry_window   = subwin(main_window, entry_lines,                 COLS-2, 1, 1);
    version_window = subwin(main_window,           1, strlen(version_string), 0, 5);
    refresh();
}

static inline
void deinit_windows() {
    delwin(entry_window);
    delwin(version_window);
    delwin(main_window);
    delwin(input_window);
}

static
void full_redraw(void) {
    box(main_window, 0, 0);

    waddstr(version_window, version_string);

    update_input();

    wnoutrefresh(version_window);
    doupdate();
}

static
void resize(void) {
    deinit_windows();
    init_windows();
    full_redraw();
}

static inline
void update_input() {
    const char * const prompt = "$ ";

    wmove(input_window, 0, 0);
    waddstr(input_window, prompt);

    waddnstr(input_window, rl_line_buffer, rl_point);
    wattron(input_window, A_REVERSE);
    if (rl_point == rl_end) {
        waddch(input_window, ' ');
        wattroff(input_window, A_REVERSE);
    } else {
        waddch(input_window, rl_line_buffer[rl_point]);
        wattroff(input_window, A_REVERSE);
        waddnstr(input_window, rl_line_buffer + rl_point + 1, rl_end - rl_point);
    }

    wclrtoeol(input_window);

    wnoutrefresh(input_window);
}

void tui_rearm() {
    entry_line_index = 0;
}

void tui_refresh(void) {
    // XXX: this is dirty
    if (selection_relative > last_entry_line_index-1) {
        selection_relative = last_entry_line_index-2;
        do_redisplay = true;
        return;
    }

    if (do_fullredraw) {
        do_fullredraw = false;
        full_redraw();
        return;
    }

    last_entry_line_index = entry_line_index;
    while (entry_line_index < entry_lines) {
        wmove(entry_window, (entry_lines-1)-entry_line_index++, 0);
        wclrtoeol(entry_window);
    }
    entry_line_index = 0;

    update_input();

    wnoutrefresh(entry_window);
    wnoutrefresh(main_window);
    doupdate();
}

#include <stdlib.h>

void tui_take_input(void) {
    extern bool do_run;
    extern bool do_execute;
    extern bool do_select;
    const size_t paging_size = entry_lines / 2;

	input = wgetch(stdscr);
    switch (input) {
        // select previous history entry
        case KEY_UP:
        case CTRL('p'):
        case CTRL('k'): {
            if (selection_relative == last_entry_line_index-1
            &&  entry_lines != last_entry_line_index) {
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
        // select next history entry
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
        // jump half a page up
        case KEY_PPAGE:
        case CTRL('u'): {
            if (last_entry_line_index < entry_lines) {
                break;
            }
            selection_offset += paging_size;
            is_input_changed = true;
        } break;
        // jump half a page down
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
        // quit and run current selection
        case '\r': {
            do_run = false;
        } break;
        // quit and insert current selection
        case '\t': {
            do_execute = false;
            do_run     = false;
        } break;
        // quit and preserve input
        case ESC: {
            do_select  = false;
            do_execute = false;
            do_run     = false;
        } break;
        // edit cursor left
        case KEY_LEFT: {
            if (rl_point != 0) {
                --rl_point;
                is_input_changed = true;
            }
        } break;
        // edit cursor right
        case KEY_RIGHT: {
            if (rl_point != rl_end) {
                ++rl_point;
                is_input_changed = true;
            }
        } break;
        // resize
        case KEY_RESIZE: {
            //flushinp();
            resize();
        } break;
        // ignore errors
        case ERR: break;
        // insert regular char
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
