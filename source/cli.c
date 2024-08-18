#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argument_yy.tab.h"

bool is_expecting_argument = false;

static int yy_argc;
static const char * const * yy_argv;
static int yy_i;

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
        "    HISTFILE=$HISTFILE ${HISTUICMD} --input \"${READLINE_LINE}\" 3> \"${COMMANDFILE}\"\n"
        "    READLINE_LINE=$(cat \"${COMMANDFILE}\")\n"
        "    READLINE_POINT=${#READLINE_LINE}\n"
        "}\n"
        "\n"
        "bind -x '\"\\e[A\": _histui_run'\n"
        "bind -x '\"\\C-r\": _histui_run'\n"
    );
}

/* Lexical analysis of a poor man.
 * Using Flex would be problematic because our input
 *  is not stored in an actual buffer (not in a defined
 *  behaviour way anyways).
 */
#define YIELD(v) ++yy_i; return v
int argument_yy_lex(void) {
    for (; yy_i < yy_argc;) {
        if (is_expecting_argument) {
            is_expecting_argument = false;
            argument_yy_lval.strval = yy_argv[yy_i];
            YIELD(ARGUMENT);
        }

        if (!strcmp(yy_argv[yy_i], "--help")
        ||  !strcmp(yy_argv[yy_i], "-h")) {
            YIELD(HELP);
        } else
        if (!strcmp(yy_argv[yy_i], "--version")
        ||  !strcmp(yy_argv[yy_i], "-v")) {
            YIELD(VERSION);
        } else
        if (!strcmp(yy_argv[yy_i], "tui")) {
            YIELD(TUI);
        } else
        if (!strcmp(yy_argv[yy_i], "enable")) {
            YIELD(ENABLE);
        } else
        if (!strcmp(yy_argv[yy_i], "--execute")) {
            YIELD(EXECUTE);
        } else
        if (!strcmp(yy_argv[yy_i], "--fuzzy")) {
            YIELD(FUZZY);
        } else
        if (!strcmp(yy_argv[yy_i], "--caseless")) {
            YIELD(CASELESS);
        } else
        if (!strcmp(yy_argv[yy_i], "--group")) {
            YIELD(GROUP);
        } else
        if (!strcmp(yy_argv[yy_i], "--input")) {
            YIELD(INPUT);
        } else {
            YIELD(YYUNDEF);
        }
    }

    YIELD(YYEOF);
}

void argument_yy_error([[ maybe_unused ]] const char * const s) {
    fputs("Argument parsing failed.\n", stderr);
    usage();
    exit(2);
}

void parse_arguments(const int argc, const char * const * const argv) {
  #if 0
    argument_yy_debug = 1;
  #endif

    if (argc < 2) {
        usage();
        exit(1);
    }

    yy_argc = argc - 1;
    yy_argv = argv + 1;

    argument_yy_parse();
}
