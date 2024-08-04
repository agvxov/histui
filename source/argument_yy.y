%token HELP VERSION
%token TUI ENABLE
%token LEVENSTEIN
%{
    #include "stdio.h"
    extern void usage(void);
    extern void version(void);
    extern void enable(void);
    extern void argument_yy_error(const char * const s);
    extern int argument_yy_lex(void);
%}
%%
histui_args: global_args verb_and_args
    ;

global_args: %empty
    | HELP              { usage();   exit(0); }
    | VERSION           { version(); exit(0); }
    ;

verb_and_args: ENABLE   { enable(); exit(0); }
    | TUI tui_args      { ; }
    ;

tui_args: %empty
    | LEVENSTEIN        { ; }
    ;

%%
