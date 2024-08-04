#include "cli.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argument_yy.tab.hpp"

int * arg_tokens;

void version() {
    puts(
    # include "version.inc"
    );
}

void usage(void) {
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
}

void enable(void) {
    // XXX one day...
    /*
    puts(
    # embed "histui_enable.sh.inc"
    );
    */
    puts(
        R"delim(
function _histui_run() {
    COMMANDFILE="${XDG_CACHE_HOME}/histui_command.txt"
    export HISTFILE histui tui 3> "${COMMANDFILE}"
    READLINE_LINE=$(cat "${COMMANDFILE}")
    READLINE_POINT=${#READLINE_LINE}
}

bind -x '"\e[A": _histui_run'
bind -x '"\C-r": _histui_run'
        )delim"
    );
}

int argument_yy_lex(void) {
    static int i = 0;
    return arg_tokens[i++];
}

void argument_yy_error([[ maybe_unused ]] const char * const s) {
    fputs("Argument parsing failed.\n", stderr);
    usage();
    exit(2);
}

void parse_arguments(const int argc, const char * const * const argv) {
    if (argc < 2) {
        usage();
        exit(1);
    }

    /* Lexical analysis of a poor man.
     * Using Flex would be problematic because our input
     *  is not stored in an actual buffer (not in a defined
     *  behaviour way anyways).
     */
    int tokens[argc];
    int token_empty_head = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")
        ||  !strcmp(argv[i], "-h")) {
            tokens[token_empty_head++] = HELP;
        } else
        if (!strcmp(argv[i], "--version")
        ||  !strcmp(argv[i], "-v")) {
            tokens[token_empty_head++] = VERSION;
        } else
        if (!strcmp(argv[i], "tui")) {
            tokens[token_empty_head++] = TUI;
        } else
        if (!strcmp(argv[i], "enable")) {
            tokens[token_empty_head++] = ENABLE;
        } else
        if (!strcmp(argv[i], "--levenstein")) {
            tokens[token_empty_head++] = LEVENSTEIN;
        } else {
            tokens[token_empty_head++] = YYUNDEF;
        }
    }
    tokens[token_empty_head++] = YYEOF;

    arg_tokens = tokens;

    argument_yy_parse();
}
