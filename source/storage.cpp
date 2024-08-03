#include "storage.hpp"

#include <stddef.h>
#include <sqlite3.h>
#include "damerau_levenshtein.hpp"

static sqlite3 * db;
static sqlite3_stmt * stmt = NULL;

int init_storage(void) {
    sqlite3_open(":memory:", &db);
    sqlite3_create_function(db, "damerau_levenshtein_substring", 2, SQLITE_ANY, 0, damerau_levenshtein_substring, NULL, NULL);
    sqlite3_create_function(db, "is_damerau_levenshtein", 3, SQLITE_ANY, 0, is_damerau_levenshtein, NULL, NULL);
    sqlite3_create_function(db,    "damerau_levenshtein", 2, SQLITE_ANY, 0,    damerau_levenshtein, NULL, NULL);

    static const char * sql_create_table = "CREATE TABLE entries (stamp INTEGER, data TEXT);";
    sqlite3_exec(db, sql_create_table, 0, 0, 0);

    return 0;
}

int deinit_storage(void) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int insert_entry(int timestamp, const char * const command) {
    static const char * sql_insert = "INSERT INTO entries (stamp, data) VALUES (?, ?);";
    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);

    sqlite3_bind_int64(stmt, 1, timestamp);
    sqlite3_bind_text(stmt, 2, command, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return 0;
}

void query(const char * const string, const size_t limit, const size_t offset) {
    sqlite3_finalize(stmt);
    const char * sql_query;
    if (string[0] != '\0') {
        sql_query = "SELECT * FROM entries "
                        "ORDER BY DAMERAU_LEVENSHTEIN_SUBSTRING(LOWER(data), LOWER(?)) "
                        "LIMIT ? "
                        "OFFSET ?;"
                    ;
        sqlite3_prepare_v2(db, sql_query, -1, &stmt, 0);
        sqlite3_bind_text(stmt, 1, string, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 2, (long)limit);
        sqlite3_bind_int64(stmt, 3, (long)offset);
    } else {
        sql_query = "SELECT * FROM entries "
                        "ORDER BY stamp DESC "
                        "LIMIT ? "
                        "OFFSET ?;";
        sqlite3_prepare_v2(db, sql_query, -1, &stmt, 0);
        sqlite3_bind_int64(stmt, 1, (long)limit);
        sqlite3_bind_int64(stmt, 2, (long)offset);
    }
}

void requery(void) {
    sqlite3_reset(stmt);
}

entry_t get_entry(void) {
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        return (entry_t){
            .timestamp = 0,
            .command   = NULL
        };
    }
    return  (entry_t){
        .timestamp = sqlite3_column_int(stmt, 0),
        .command   = (char*)sqlite3_column_text(stmt, 1)
    };
}
