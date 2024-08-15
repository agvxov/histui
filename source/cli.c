#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argument_yy.tab.h"

int * arg_tokens;

void version() {
    puts(
    # include "version.inc"
    );
}

void usage(void) {
    puts(
        "histui [options] <verb>\n"
        "\tOptions:\n"
        "\t\t-v --version\n"
        "\t\t-h --help\n"
        "\tVerbs:\n"
        "\t\tenable            : print a bash script to enable histui in the current shell\n"
        "\t\ttui [tui-options] : run histui normally\n"
        "\t\t\t--caseless : ignore case while searching\n"
        "\t\t\t--fuzzy    : employ fuzzy searching\n"
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
        "function _histui_run() {\n"
        "    COMMANDFILE=\"${XDG_CACHE_HOME}/histui_command.txt\"\n"
        "    if ! [ -v HISTUICMD ]; then\n"
        "        HISTUICMD=\"histui tui\"\n"
        "    fi\n"
        "    HISTFILE=$HISTFILE ${HISTUICMD} 3> \"${COMMANDFILE}\"\n"
        "    READLINE_LINE=$(cat \"${COMMANDFILE}\")\n"
        "    READLINE_POINT=${#READLINE_LINE}\n"
        "}\n"
        "\n"
        "bind -x '\"\\e[A\": _histui_run'\n"
        "bind -x '\"\\C-r\": _histui_run'\n"
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
        if (!strcmp(argv[i], "--execute")) {
            tokens[token_empty_head++] = EXECUTE;
        } else
        if (!strcmp(argv[i], "--fuzzy")) {
            tokens[token_empty_head++] = FUZZY;
        } else
        if (!strcmp(argv[i], "--caseless")) {
            tokens[token_empty_head++] = CASELESS;
        } else
        if (!strcmp(argv[i], "--group")) {
            tokens[token_empty_head++] = GROUP;
        } else {
            tokens[token_empty_head++] = YYUNDEF;
        }
    }
    tokens[token_empty_head++] = YYEOF;

    arg_tokens = tokens;

    argument_yy_parse();
}
