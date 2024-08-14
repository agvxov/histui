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
