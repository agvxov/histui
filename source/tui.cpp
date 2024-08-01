#include "tui.hpp"

#include <ncurses.h>

WINDOW * main_window;
WINDOW * input_window;

int init_tui(void) {
	initscr();
	noecho();
	curs_set(0);
	main_window = newwin(COLS-1, LINES, 0, 0);
    box(main_window, 0, 0);
    return 0;
}

int deinit_tui(void) {
	endwin();
    return 0;
}

void tui_append_back(int timestamp, const unsigned char * const text) {
    mvprintw(1, 1, "%d, %s\n", timestamp, text);
}

void tui_refresh(void) {
    wrefresh(main_window);
    refresh();
}
