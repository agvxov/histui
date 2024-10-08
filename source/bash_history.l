%{
    #include "entry.h"
    #include "storage.h"
    long timestamp;
%}
%option nodefault
%option noyywrap
%option nounput noinput
%%
\#[[:digit:]]+  { timestamp = strtoll(yytext+1, NULL, 10);
}
.*              { insert_entry((entry_t){ timestamp, yytext }); }
\n              { ; }
%%
