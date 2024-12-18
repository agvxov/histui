#include "storage.h"

#include <stddef.h>
#include <iso646.h>
#include <string.h>
#include <sqlite3.h>

#define SELECT       "SELECT * FROM entries "
#define GROUP_SELECT "SELECT MAX(stamp), data FROM entries "
#define ORDERING "ORDER BY stamp DESC "
#define PAGING   "LIMIT ? OFFSET ? "

bool is_fuzzy    = false;
bool is_caseless = false;
bool is_grouped  = false;

static sqlite3 * db = NULL;

/* These statements must be global so they can be prepared once
 *  upon initialization and finalized on deinit, only rebinding
 *  then during runtime.
 * This should be slightly faster than continously repreparing
 *  and refinalizing.
 */
static sqlite3_stmt * insert_stmt      = NULL;
static sqlite3_stmt * query_stmt       = NULL;
static sqlite3_stmt * empty_query_stmt = NULL;
static sqlite3_stmt * * stmt;

void braindamaged_fuzzy_search(sqlite3_context * context, [[maybe_unused]] int argc, sqlite3_value * * argv);

int init_storage(void) {
    // Create database
    sqlite3_open(":memory:", &db);
    sqlite3_create_function(db, "braindamaged_fuzzy_search", 2, SQLITE_ANY, 0, braindamaged_fuzzy_search, NULL, NULL);

    const char * const sql_create_table = "CREATE TABLE entries (stamp INTEGER, data TEXT);";
    sqlite3_exec(db, sql_create_table, 0, 0, 0);

    // Prepare inserts
    const char * const insert_entry_sql = "INSERT INTO entries (stamp, data) VALUES (?, ?);";
    sqlite3_prepare_v2(db, insert_entry_sql, -1, &insert_stmt, 0);

    // Prepare queries
    char filtered_query[256] = "\0";
    char empty_query[256]    = "\0";

    if (not is_grouped) {
        strcat(filtered_query, SELECT);
        strcat(empty_query,    SELECT);
    } else {
        strcat(filtered_query, GROUP_SELECT);
        strcat(empty_query,    GROUP_SELECT);
    }

    char * sql_mid[2][2] = {
        {
            "WHERE data GLOB CONCAT('*', ?, '*') ",
            "WHERE data LIKE CONCAT('%', ?, '%') ",
        },
        {
            "WHERE BRAINDAMAGED_FUZZY_SEARCH(data, ?) ",
            "WHERE BRAINDAMAGED_FUZZY_SEARCH(LOWER(data), LOWER(?)) ",
        },
    };
    strcat(filtered_query, sql_mid[is_fuzzy][is_caseless]);

    if (is_grouped) {
        strcat(filtered_query, "GROUP BY data ");
        strcat(empty_query,    "GROUP BY data ");
    }

    strcat(filtered_query, ORDERING);
    strcat(empty_query,    ORDERING);
    strcat(filtered_query, PAGING);
    strcat(empty_query,    PAGING);
    strcat(filtered_query, ";");
    strcat(empty_query,    ";");

    sqlite3_prepare_v2(db, filtered_query, -1, &      query_stmt, 0);
    sqlite3_prepare_v2(db,    empty_query, -1, &empty_query_stmt, 0);

    return 0;
}

int deinit_storage(void) {
    sqlite3_finalize(insert_stmt);
    sqlite3_finalize(query_stmt);
    sqlite3_finalize(empty_query_stmt);
    sqlite3_close(db);
    return 0;
}

int insert_entry(const entry_t entry) {
    sqlite3_reset(insert_stmt);
    sqlite3_clear_bindings(insert_stmt);
    sqlite3_bind_int64(insert_stmt, 1, entry.timestamp);
    sqlite3_bind_text(insert_stmt, 2, entry.command, -1, SQLITE_STATIC);
    sqlite3_step(insert_stmt);

    return 0;
}

void query(const char * const string, const size_t limit, const size_t offset) {
    if (string[0] != '\0') {
        sqlite3_reset(query_stmt);
        sqlite3_clear_bindings(query_stmt);
        sqlite3_bind_text(query_stmt, 1, string, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(query_stmt, 2, (long)limit);
        sqlite3_bind_int64(query_stmt, 3, (long)offset);
        stmt = &query_stmt;
    } else {
        sqlite3_reset(empty_query_stmt);
        sqlite3_clear_bindings(empty_query_stmt);
        sqlite3_bind_int64(empty_query_stmt, 1, (long)limit);
        sqlite3_bind_int64(empty_query_stmt, 2, (long)offset);
        stmt = &empty_query_stmt;
    }
}

void requery(void) {
    sqlite3_reset(*stmt);
}

void cancel_all_queries(void) {
    sqlite3_interrupt(db);
}

entry_t get_entry(void) {
    if (sqlite3_step(*stmt) != SQLITE_ROW) {
        return (entry_t){
            .timestamp = 0,
            .command   = NULL,
        };
    }
    return  (entry_t){
        .timestamp = sqlite3_column_int(*stmt, 0),
        .command   = (char*)sqlite3_column_text(*stmt, 1),
    };
}

void braindamaged_fuzzy_search(sqlite3_context * context, [[maybe_unused]] int argc, sqlite3_value * * argv) {
    const char * const db_text    = (const char *)sqlite3_value_text(argv[0]);
    const char * const user_query = (const char *)sqlite3_value_text(argv[1]);

    char mutable_user_query[strlen(user_query)+1];
    strcpy(mutable_user_query, user_query);
    const char* delim = " \t";
    char * save;
    char * s = strtok_r(mutable_user_query, delim, &save);

    if (!s) { goto end; }

    do {
        if (!strstr(db_text, s)) {
            sqlite3_result_int(context, 0);
            return;
        }
    } while((s = strtok_r(NULL, delim, &save), s));

  end:
    sqlite3_result_int(context, 1);
}
