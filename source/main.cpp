#include <locale.h>
#include "storage.hpp"
#include "tui.hpp"
#include "bash_history.yy.hpp"

void init() {
    setlocale(LC_TIME, "C");
    init_storage();
    init_tui();
}

void deinit() {
    deinit_storage();
    deinit_tui();
}

signed main(int argc, char * argv[]) {
    // TODO cli stuff

    init();

    bash_history_in = fopen("/home/anon/stow/.cache/.bash_history", "r");
    bash_history_lex();

    tui_refresh();

    entry_t entry;
    while (true) {
        query(rl_line_buffer);
        while (entry = get_entry(), entry.command != NULL) {
            tui_append_back(entry);
        }
        tui_take_input();
        tui_refresh();
    }

    deinit();

    return 0;
}
