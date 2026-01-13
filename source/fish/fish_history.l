%{
    #include "entry.h"
    #include "storage.h"
    long timestamp;
%}
%option nodefault
%option noyywrap
%option nounput noinput
%%
-\x20cmd:\x20.* {
    insert_entry((entry_t){ timestamp, yytext+sizeof("- cmd: ")-1 });
}
\x20\x20when:\x20[[:digit:]]+ {
    timestamp = strtoll(yytext+sizeof("  when: ")-1, NULL, 10);
}
\x20\x20paths:\n { ; } /* optional directive, unrelevant to us */
\x20{4}-\x20.*\n { ; } /* list elements belonging to `path:` ^^^ */
\n { ; }
%%
