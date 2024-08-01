#pragma once

int init_storage(void);
int deinit_storage(void);

int insert_entry(int timestamp, const char * const command);
