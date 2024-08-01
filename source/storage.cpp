#include "storage.hpp"

#include <stddef.h>
#include <sqlite3.h>
//#include "damerau_levenshtein.h"
#include "damerau_levenshtein2.h"

sqlite3 * db;
sqlite3_stmt * stmt;

int init_storage(void) {
    sqlite3_open(":memory:", &db);
    sqlite3_create_function(db, "is_damerau_levenshtein", 3, SQLITE_ANY, 0, is_damerau_levenshtein, NULL, NULL);
    sqlite3_create_function(db,    "damerau_levenshtein", 2, SQLITE_ANY, 0,    damerau_levenshtein, NULL, NULL);

    static const char * sql_create_table = "CREATE TABLE test (stamp INTEGER, data TEXT);";
    sqlite3_exec(db, sql_create_table, 0, 0, 0);

    return 0;
}

int deinit_storage(void) {
    sqlite3_close(db);
    return 0;
}

int insert_entry(int timestamp, const char * const command) {
    static const char * sql_insert = "INSERT INTO test (stamp, data) VALUES (?, ?);";
    sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);

    sqlite3_bind_int64(stmt, 1, timestamp);
    sqlite3_bind_text(stmt, 2, command, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return 0;
}
