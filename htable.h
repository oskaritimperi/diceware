#ifndef HTABLE_H
#define HTABLE_H

#include <stdlib.h>

struct htable_entry
{
    char *id;
    char *word;
};

struct htable
{
    size_t size;
    size_t max;
    struct htable_entry **elems;
};


int htable_put(struct htable *htable, struct htable_entry *entry);
struct htable_entry *htable_get(struct htable *htable, const char *id);
void htable_init(struct htable *htable, size_t max);
void htable_deinit(struct htable *htable);

#endif
