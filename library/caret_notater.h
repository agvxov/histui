#ifndef CARET_NOTATER_H
#define CARET_NOTATER_H
/* caret_notater.h converts strings containing
 *  unprintable characters to equivalent strings
 *  in caret notation.
 */

#if !defined(CARET_NOTATER_ALLOC) && defined(__STDC__) && __STDC__ == 1
# define CARET_NOTATER_ALLOC 1
#endif

static inline int  is_caretable(const char c);
static inline char to_caret_char(const char c);
// NOTE: these functions return their `output` argument
char * char_to_caret_notation(const char c, char output[3]);
char * string_to_caret_notation(const char * const input, const unsigned long size, char * const output);
#if CARET_NOTATER_ALLOC == 1
static inline char * alloc_char_to_caret_notation(const char c);
static inline char * alloc_string_to_caret_notation(const char * const input, const unsigned long size);
#endif

// --- Implementation

static inline
int is_caretable(const char c) {
    return ((0 <= c && c <= 31) || c == 127);
}

static inline
char to_caret_char(const char c) {
    return c ^ 0x40;
}

char * char_to_caret_notation(const char c, char output[3]) {
    if (is_caretable(c)) {
        output[0] = '^';
        output[1] = to_caret_char(c);
        output[2] = '\0';
    } else {
        output[0] = '\0';
    }
    return output;
}

/* NOTE: the size must be passed because `'\0' -> "^@"` must be a valid transformation
 */
char * string_to_caret_notation(const char * const input, const unsigned long size, char * const output) {
    int output_empty_end = 0;
    for (unsigned long i = 0; i < size; i++) {
        if (is_caretable(input[i])) {
            output[output_empty_end++] = '^';
            output[output_empty_end++] = to_caret_char(input[i]);
        } else {
            output[output_empty_end++] = input[i];
        }
    }
    output[output_empty_end] = '\0';

    return output;
}

#if CARET_NOTATER_ALLOC == 1
  #include <stdlib.h>

  static inline
  char * alloc_char_to_caret_notation(const char c) {
      char * const r = malloc(3);
      return char_to_caret_notation(c, r);
  }
  
  static inline
  char * alloc_string_to_caret_notation(const char * const input, const unsigned long size) {
      char * const r = malloc(size*2);
      return string_to_caret_notation(input, size, r);
  }
#endif

#endif
/* --- LICENSE
 * This software is in the Public Domain;
 *  and if (You) dare to say this notice is not legally valid,
 *  I will sue you.
 */
