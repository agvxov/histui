#include "storage.hpp"
#include "tui.hpp"
#include "bash_history.yy.hpp"

// XXX
#include <sqlite3.h>

void init() {
    init_storage();
    init_tui();
}

void deinit() {
    deinit_storage();
    deinit_tui();
}

signed main(int argc, char * argv[]) {
    // XXX
    extern sqlite3 * db;
    extern sqlite3_stmt * stmt;
    // TODO cli stuff

    init();

    bash_history_in = fopen("/home/anon/stow/.cache/.bash_history", "r");
    bash_history_lex();

    // XXX
    while (true) {
        sqlite3_prepare_v2(db, "SELECT * FROM test WHERE DAMERAU_LEVENSHTEIN(data, 'a');", -1, &stmt, 0);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            tui_append_back(sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1));
        }
        sqlite3_finalize(stmt);
        tui_refresh();
    }

    deinit();

    return 0;
}
