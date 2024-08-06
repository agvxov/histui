#include "storage.hpp"

#include <stddef.h>
#include <sqlite3.h>
#include "damerau_levenshtein.hpp"

/* I would heavily prefer to not have dynamically generated SQL.
 * This might became a scaling issue in the future tho,
 *  but untill then hardcoding will work.
 */

bool is_levenstein = false;
bool is_caseless   = false;

static const char * const insert_entry_sql =
    "INSERT INTO entries (stamp, data) VALUES (?, ?);"
;
static const char * const empty_query =
    "SELECT * FROM entries "
        "ORDER BY stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
static const char * const literal_query =
    "SELECT * FROM entries "
        "WHERE data GLOB CONCAT('*', ?, '*') "
        "GROUP BY data "
        "ORDER BY stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
static const char * const literal_caseless_query =
    "SELECT * FROM entries "
        "WHERE data LIKE CONCAT('%', ?, '%') "
        "GROUP BY data "
        "ORDER BY stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
static const char * const levenstein_query = 
    "SELECT * FROM entries "
        "GROUP BY data "
        "ORDER BY DAMERAU_LEVENSHTEIN_SUBSTRING(data, ?), "
            "stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
static const char * const levenstein_caseless_query =
    "SELECT * FROM entries "
        "GROUP BY data "
        "ORDER BY DAMERAU_LEVENSHTEIN_SUBSTRING(LOWER(data), LOWER(?)), "
            "stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
static const char * const * query_method;

static sqlite3 * db = NULL;

/* These statements must be global so they can be prepared once
 *  upon initialization and finalized on deinit, only rebinding
 *  then during runtime.
 * This should be slightly faster than continously repreparing
 *  and refinalizing.
 */
static sqlite3_stmt * insert_stmt = NULL;
static sqlite3_stmt * query_stmt       = NULL;
static sqlite3_stmt * empty_query_stmt = NULL;
static sqlite3_stmt * * stmt;

int init_storage(void) {
    sqlite3_open(":memory:", &db);
    sqlite3_create_function(db, "damerau_levenshtein_substring", 2, SQLITE_ANY, 0, damerau_levenshtein_substring, NULL, NULL);
    sqlite3_create_function(db,        "is_damerau_levenshtein", 3, SQLITE_ANY, 0,        is_damerau_levenshtein, NULL, NULL);
    sqlite3_create_function(db,           "damerau_levenshtein", 2, SQLITE_ANY, 0,           damerau_levenshtein, NULL, NULL);

    static const char * sql_create_table = "CREATE TABLE entries (stamp INTEGER, data TEXT);";
    sqlite3_exec(db, sql_create_table, 0, 0, 0);

    sqlite3_prepare_v2(db, insert_entry_sql, -1, &insert_stmt, 0);

    if (not is_levenstein) {
        if (not is_caseless) {
            query_method = &literal_query;
        } else {
            query_method = &literal_caseless_query;
        }
    } else {
        if (not is_caseless) {
            query_method = &levenstein_query;
        } else {
            query_method = &levenstein_caseless_query;
        }
    }
    sqlite3_prepare_v2(db, *query_method, -1, &      query_stmt, 0);
    sqlite3_prepare_v2(db,   empty_query, -1, &empty_query_stmt, 0);

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
