#include "tui.hpp"

#include <time.h>
#include <locale.h>
#include <ncurses.h>
#include <readline/readline.h>

// XXX
#include <cstdlib>

static WINDOW * main_window;
static WINDOW * entry_window;
static WINDOW * input_window;

static int input_available = false;
static char input;

static void refresh_input(void);

int init_tui(void) {
    // Ncurses
	initscr();
	noecho();
	curs_set(0);

	main_window   = newwin(LINES-1, COLS, 0, 0);
    entry_window  = subwin(main_window, LINES-3, COLS-2, 1, 1);
    input_window  = newwin(1, COLS, LINES-1, 0);
    refresh();

    box(main_window, 0, 0);

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
    strftime(time_buffer, TIME_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", tm_info);

    wprintw(entry_window, "%s  %s\n",
                            time_buffer,
                            entry.command
                );
}

static void refresh_input(void) {
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
}


void tui_take_input(void) {
	input = wgetch(stdscr);
	input_available = true;
	rl_callback_read_char();
}
