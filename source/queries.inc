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
static const char * const fuzzy_query = 
    "SELECT * FROM entries "
        "WHERE BRAINDAMAGED_FUZZY_SEARCH(data, ?) "
        "GROUP BY data "
        "ORDER BY stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
static const char * const fuzzy_caseless_query =
    "SELECT * FROM entries "
        "WHERE BRAINDAMAGED_FUZZY_SEARCH(LOWER(data), LOWER(?)) "
        "GROUP BY data "
        "ORDER BY stamp DESC "
        "LIMIT ? "
        "OFFSET ?;"
;
