#include "tui.hpp"

#include <stdio.h>
#include <ncurses.h>

#include "config.hpp"
#include "db.hpp"

static
char * render_entry(const Entry * const entry, const char * const format) {
    return NULL;
}

signed tui_main(int argc, char * * argv) {
    read_config(NULL);
    return 0;
}
