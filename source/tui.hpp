#pragma once

int init_tui(void);
int deinit_tui(void);
void tui_refresh(void);

void tui_append_back(int timestamp, const unsigned char * const text);
