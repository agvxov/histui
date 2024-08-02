#pragma once
#include "entry.h"

int init_storage(void);
int deinit_storage(void);

int insert_entry(int timestamp, const char * const command);
void query(const char * const string);
entry_t get_entry(void);
