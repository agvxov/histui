#include "cli.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

[[ noreturn ]]
void version() {
    puts(
    # include "version.inc"
    );

    exit(0);
}

[[ noreturn ]]
void usage(int exit_value) {
    // TODO
    puts(
        "histui [options] <verb>\n"
        "\tOptions:\n"
        "\t\t-v --version\n"
        "\t\t-h --help\n"
        "\tVerbs:\n"
        "\t\tenable : print a bash script to enable histui in the current shell\n"
        "\t\ttui    : run histui normally\n"
    );
    exit(exit_value);
}

void parse_global_options(const int argc, const char * const * const argv) {
    for(int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            return;
        }
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

verb_t get_verb(const int argc, const char * const * const argv) {
    for(int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            continue;
        }
        if (not strcmp(argv[i], "tui")) {
            return TUI;
        }
        if (not strcmp(argv[i], "enable")) {
            return ENABLE;
        }
        return ERROR;
    }
    return ERROR;
}
