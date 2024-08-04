#pragma once
#include <stddef.h>
#include "entry.h"

extern const char * const * query_method;
extern const char * const levenstein_query;
extern const char * const levenstein_caseless_query;

int init_storage(void);
int deinit_storage(void);

int insert_entry(int timestamp, const char * const command);
void query(const char * const string, const size_t limit, const size_t offset);
void requery(void);
entry_t get_entry(void);
