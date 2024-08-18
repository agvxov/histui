%token HELP VERSION EXECUTE
%token TUI ENABLE
%token FUZZY CASELESS GROUP INPUT
%{
    #include <stdlib.h>
    #include "cli.h"
    #include "tui.h"
    #include "storage.h"
    extern bool do_execute;
    extern bool is_expecting_argument;
%}
%union { const char * strval; }
%token<strval> ARGUMENT
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
    | EXECUTE  tui_args { do_execute  = true; }
    | FUZZY    tui_args { is_fuzzy    = true; }
    | CASELESS tui_args { is_caseless = true; }
    | GROUP    tui_args { is_grouped  = true; }
    | input ARGUMENT tui_args { initial_text = $2; }
    ;

input: INPUT { is_expecting_argument = true; }
%%
