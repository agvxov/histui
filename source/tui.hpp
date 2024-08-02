#pragma once
#include "entry.h"

extern "C" char * rl_line_buffer;

int init_tui(void);
int deinit_tui(void);
void tui_refresh(void);
void tui_take_input(void);

void tui_append_back(const entry_t entry);
