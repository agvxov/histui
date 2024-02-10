#include "tui.hpp"

#include <stdio.h>
#include <ncurses.h>

#include "config.hpp"

signed tui_main(int argc, char * * argv) {
    read_config(NULL);
    return 0;
}
