#pragma once
extern void    damerau_levenshtein(sqlite3_context *context, int argc, sqlite3_value **argv);
extern void is_damerau_levenshtein(sqlite3_context *context, int argc, sqlite3_value **argv);
extern void damerau_levenshtein_substring(sqlite3_context *context, int argc, sqlite3_value **argv);
