%token HELP VERSION
%token TUI ENABLE
%token LEVENSTEIN CASELESS
%{
    #include "cli.hpp"
    #include "storage.hpp"
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
    | LEVENSTEIN tui_args { is_levenstein = true; }
    | CASELESS   tui_args { is_caseless   = true; }
    ;

%%
