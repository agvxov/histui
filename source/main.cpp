#include <locale.h>
#include "cli.hpp"
#include "bash_history.yy.hpp"
#include "storage.hpp"
#include "tui.hpp"

bool do_run = true;

void init(void);
void deinit(void);

void init(void) {
    setlocale(LC_TIME, "C");
    init_storage();

    char * history_file_path = getenv("HISTFILE");
    if (!history_file_path) {
        fputs("$HISTFILE is not set, try exporting it.\n", stderr);
        deinit();
        exit(1);
    }

    bash_history_in = fopen(history_file_path, "r");
    if (!bash_history_in) {
        fputs("Failed to open history file.\n", stderr);
        deinit();
        exit(1);
    }

    bash_history_lex();

    init_tui();
}

void deinit(void) {
    deinit_tui();
    deinit_storage();
}

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
    export HISTFILE histui tui 3> "${COMMANDFILE}"
    READLINE_LINE=$(cat "${COMMANDFILE}")
    READLINE_POINT=${#READLINE_LINE}
}

bind -x '"\e[A": _histui_run'
bind -x '"\C-r": _histui_run'
        )delim"
    );
}

void export_result(const char * const result) {
    int fd[2];
    pipe(fd);
    dprintf(3, result);
    close(fd[0]);
    close(fd[1]);
}

signed main(int argc, char * argv[]) {
    // NOTE: never returns on error
    parse_arguments(argc, argv);

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
    export_result(get_entry().command);

    deinit();

    return 0;
}
