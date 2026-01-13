#define bash 1
#define fish 2

// Passed by the build-system and requires expansion
#define TARGET_SHELL SHELL

#if TARGET_SHELL == bash
    #include "bash_history.yy.h"
    #define history_in  bash_history_in
    #define history_lex bash_history_lex
    static const char enable_string[] = {
        #embed "bash/enable.sh.inc"
        , '\0'
    };
#elif TARGET_SHELL == fish
    #include "fish_history.yy.h"
    #define history_in  fish_history_in
    #define history_lex fish_history_lex
    static const char enable_string[] = {
        #embed "fish/enable.fish.inc"
        , '\0'
    };
#else
    #error
#endif
