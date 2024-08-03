#include <locale.h>
#include "cli.hpp"
#include "bash_history.yy.hpp"
#include "storage.hpp"
#include "tui.hpp"

bool do_run = true;

void init() {
    setlocale(LC_TIME, "C");
    init_storage();
    bash_history_in = fopen("/home/anon/stow/.cache/.bash_history", "r");
    bash_history_lex();
    init_tui();
}

void deinit() {
    deinit_tui();
    deinit_storage();
}

[[ noreturn ]]
void enable() {
    // XXX one day...
    /*
    puts(
    # embed "histui_enable.sh.inc"
    );
    */
    puts(
        R"delim(
function _histui_run() {
    COMMANDFILE="${XDG_CACHE_HOME}/histui_command.txt"
    histui tui 3> "${COMMANDFILE}"
    READLINE_LINE=$(cat "${COMMANDFILE}")
    READLINE_POINT=${#READLINE_LINE}
}

bind -x '"\e[A": _histui_run'
bind -x '"\C-r": _histui_run'
        )delim"
    );
    exit(0);
}

signed main(int argc, char * argv[]) {
    parse_global_options(argc, argv);
    verb_t verb = get_verb(argc, argv);
    switch (verb) {
        case ENABLE: enable();
        case ERROR:  usage(1);
        case TUI:    break;
    }

    init();

    tui_refresh();

    entry_t entry;
    while (do_run) {
        if (is_input_changed) {
            query(rl_line_buffer, entry_lines, selection_offset);
            is_input_changed = false;
        } else {
            requery();
        }
        while (entry = get_entry(), entry.command != NULL) {
            tui_append_back(entry);
        }
        tui_refresh();
        tui_take_input();
    }

    query(rl_line_buffer, 1, selection_offset + selection_relative);
    int fd[2];
    pipe(fd);
    dprintf(3, get_entry().command);
    close(fd[0]);
    close(fd[1]);

    deinit();

    return 0;
}
