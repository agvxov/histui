#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>

using namespace std;

[[ noreturn ]]
void version() {
    puts("Histui "
    #include "version.inc"
    );

    exit(0);
}

[[ noreturn ]]
void usage(int exit_value = 0) {
    // TODO
    exit(exit_value);
}

void global_options(const int argc, const char * const * const argv) {
    for(int i = 0; i < argc; i++) {
        if (not strcmp(argv[i], "-v")
        ||  not strcmp(argv[i], "--version")) {
            version();
        }
        if (not strcmp(argv[i], "-h")
        ||  not strcmp(argv[i], "--help")) {
            usage();
        }
    }
}

typedef signed (*mainlike_t)(int argc, char * * argv);
map<const char*, mainlike_t> verb_table = {
    {"tui", tui_main},
    {"import", import_main},
    {"export", export_main},
};
