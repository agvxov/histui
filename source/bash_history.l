%{
    #include "storage.hpp"
    long timestamp;
%}
%option noyywrap
%%
\#[[:digit:]]+  {
    timestamp = strtoll(yytext+1, NULL, 10);
}
[^\#\n].*  {
    insert_entry(timestamp, yytext);
}
\n  { ; }
%%
