#pragma once
#include <stddef.h>
#include "entry.h"

extern "C" char * rl_line_buffer;

extern size_t entry_lines;
extern size_t selection_offset;
extern size_t selection_relative;
extern bool   is_input_changed;

int init_tui(void);
int deinit_tui(void);
void tui_refresh(void);
void tui_take_input(void);

void tui_append_back(const entry_t entry);
