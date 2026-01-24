%{
    #include "entry.h"
    #include "storage.h"
    long timestamp;

    char * deescape_backslash(char * s) {
        /* NOTE: Fish always escapes backslashes in its history file;
         *        that is, they always come in pairs, i.e. "\\".
         */
        char * si = s;
        for (char * ss = s; *ss != '\0'; si++, ss++) {
            switch (*ss) {
                case '\\': {
                    ++ss;
                } break;
            }
            *si = *ss;
        }
        *si = '\0';

        return s;
    }
%}
%option nodefault
%option noyywrap
%option nounput noinput
%%
-\x20cmd:\x20.* {
    yytext = yytext+sizeof("- cmd: ")-1;
    yytext = deescape_backslash(yytext);

    insert_entry((entry_t){ timestamp, yytext });
}
\x20\x20when:\x20[[:digit:]]+ {
    timestamp = strtoll(yytext+sizeof("  when: ")-1, NULL, 10);
}
\x20\x20paths:\n { ; } /* optional directive, unrelevant to us */
\x20{4}-\x20.*\n { ; } /* list elements belonging to `path:` ^^^ */
\n { ; }
%%
