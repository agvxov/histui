#ifndef CLI_HPP
#define CLI_HPP

extern void parse_arguments(const int argc, const char * const * const argv);

extern void usage(void);
extern void version(void);
extern void enable(void);

extern void argument_yy_error(const char * const s);
extern int argument_yy_lex(void);

#endif
