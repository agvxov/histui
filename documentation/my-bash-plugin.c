// @BAKE gcc -fPIC -shared -o $*.so $@

/* This is a Bash plugin.
 * It creates a builtin command that will make Bash execute
 *  the current contents of the users input line.
 * It however has multiple compatibility issues:
 *  * its accessing internals; `rl_done = 1` could technically stop working tomorrow
 *  * while every bash binary i could find supports plugins,
 *     technically speaking its an optional feature
 *  * while bash usually is built with plugin support,
 *     the headers required to compile a plugin are almost never present on the system
 *     (thats why this example has all the relevant library code copy+pasted in)
 * Consequently, no-shit nobody uses builtins.
 */

// ------------------- GNU ---------------------
// ---------------------------------------------
// ---------------------------------------------
/* A structure which represents a word. */
typedef struct word_desc {
  char *word;           /* Zero terminated string. */
  int flags;            /* Flags associated with this word. */
} WORD_DESC;

/* A linked list of words. */
typedef struct word_list {
  struct word_list *next;
  WORD_DESC *word;
} WORD_LIST;

typedef int sh_builtin_func_t (WORD_LIST *);

struct builtin {
  char *name;			/* The name that the user types. */
  sh_builtin_func_t *function;	/* The address of the invoked function. */
  int flags;			/* One of the #defines above. */
  char * const *long_doc;	/* NULL terminated array of strings. */
  const char *short_doc;	/* Short version of documentation. */
  char *handle;			/* for future use */
};

#define BUILTIN_ENABLED 0x01    /* This builtin is enabled. */
#define EXECUTION_SUCCESS 0
// ---------------------------------------------
// ---------------------------------------------
// ---------------------------------------------
// ---------------------------------------------
// ---------------------------------------------

#include <stddef.h>

extern int rl_done;

int
my_accept_current_line_builtin(WORD_LIST *list)
{
    rl_done = 1;
    return EXECUTION_SUCCESS;
}

char *my_accept_current_line_doc[] = {
    "Set readline done flag.",
    (char *)NULL
};

struct builtin my_accept_current_line_struct = {
    "my_accept_current_line",
    my_accept_current_line_builtin,
    BUILTIN_ENABLED,
    my_accept_current_line_doc,
    "my_accept_current_line",
    0
};
