%token HELP VERSION EXECUTE
%token TUI ENABLE
%token LEVENSTEIN CASELESS
%{
    #include <stdlib.h>
    #include "cli.h"
    #include "storage.h"
    extern bool do_execute;
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
    | EXECUTE    tui_args { do_execute    = true; }
    | LEVENSTEIN tui_args { is_levenstein = true; }
    | CASELESS   tui_args { is_caseless   = true; }
    ;

%%
