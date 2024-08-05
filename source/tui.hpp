#ifndef TUI_HPP
#define TUI_HPP

#include <stddef.h>
#include "entry.h"

extern "C" char * rl_line_buffer;

extern size_t entry_lines;
extern size_t selection_offset;
extern size_t selection_relative;
extern bool   is_input_changed;

extern int init_tui(void);
extern int deinit_tui(void);
extern void tui_refresh(void);
extern void tui_take_input(void);

extern void tui_append_back(const entry_t entry);

#endif
