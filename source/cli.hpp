#pragma once

typedef enum {
    TUI,
    ENABLE,
    ERROR,
} verb_t;

extern void parse_global_options(const int argc, const char * const * const argv);
extern verb_t get_verb(const int argc, const char * const * const argv);
[[ noreturn ]] void usage(int exit_value = 0);
