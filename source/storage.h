#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <stddef.h>
#include "entry.h"

extern bool is_fuzzy;
extern bool is_caseless;
extern bool is_grouped;

extern int init_storage(void);
extern int deinit_storage(void);

extern int insert_entry(const entry_t entry);
extern void query(const char * const string, const size_t limit, const size_t offset);
extern void requery(void);
extern void cancel_all_queries(void);
extern entry_t get_entry(void);

#endif
