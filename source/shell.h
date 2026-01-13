#define BASH 1
#define FISH 2

// Passed by the build-system and requires expansion
#define TARGET_SHELL SHELL

#if TARGET_SHELL == bash
    static const char enable_string[] = {
        #embed "bash/enable.sh.inc"
        , '\0'
    };
#else
    #error
#endif
