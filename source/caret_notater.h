#ifndef CARET_NOTATER
#define CARET_NOTATER

static inline
int is_caret(char c) {
    return (0 <= c && c <= 31);
}

static inline
char to_caret_char(char c) {
    return c + '@';
}

char * string_to_caret_notation(const char * input, int size, char * output) {
    int output_empty_end = 0;
    for (int i = 0; i < size; i++) {
        if (is_caret(input[i])) {
            output[output_empty_end++] = '^';
            output[output_empty_end++] = to_caret_char(input[i]);
        } else {
            output[output_empty_end++] = input[i];
        }
    }
    output[output_empty_end] = '\0';

    return output;
}

#endif
