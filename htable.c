#include "htable.h"

#include <assert.h>
#include <string.h>

/*

    Simple hash table implementation. Uses the numeric id of the words as the
    key.

*/

void htable_grow(struct htable *htable, size_t newmax);

int htable_put(struct htable *htable, struct htable_entry *entry)
{
    unsigned long i, key;

    assert(htable != NULL);
    assert(entry != NULL);
    assert(htable->max > 0);

    if ((htable->size * 100) / (htable->max) >= 70)
    {
        htable_grow(htable, htable->max * 1.5);
    }

    key = strtol(entry->id, NULL, 10);

    for (i = 0; 1; ++i)
    {
        unsigned long hash = (key + i) % htable->max;

        if (!htable->elems[hash])
        {
            htable->elems[hash] = entry;
            ++htable->size;
            return 0;
        }
    }

    return -1;
}

struct htable_entry *htable_get(struct htable *htable, const char *id)
{
    unsigned long i, key;

    assert(htable != NULL);
    assert(id != NULL);

    if (htable->size == 0)
        return NULL;

    key = strtol(id, NULL, 10);

    for (i = 0; 1; ++i)
    {
        unsigned long hash = (key + i) % htable->max;

        if (!htable->elems[hash])
            return NULL;

        if (strcmp(htable->elems[hash]->id, id) == 0)
            return htable->elems[hash];
    }

    return NULL;
}

void htable_init(struct htable *htable, size_t max)
{
    assert(htable != NULL);
    assert(max > 0);

    htable->size = 0;
    htable->max = max;
    htable->elems = (struct htable_entry **) malloc(sizeof(struct htable_entry *) * max);
    memset(htable->elems, 0, sizeof(struct htable_entry *) * max);
}

void htable_deinit(struct htable *htable)
{
    assert(htable != NULL);

    if (htable->size > 0)
    {
        size_t i;

        for (i = 0; i < htable->max; ++i)
        {
            if (htable->elems[i])
            {
                free(htable->elems[i]->id);
                free(htable->elems[i]->word);
                free(htable->elems[i]);
            }
        }
    }

    htable->size = 0;
    htable->max = 0;

    free(htable->elems);
}

void htable_grow(struct htable *htable, size_t newmax)
{
    struct htable tmp;
    size_t i;

    assert(htable != NULL);
    assert(htable->max > 0);
    assert(newmax > htable->max);

    htable_init(&tmp, newmax);

    for (i = 0; i < htable->max; ++i)
    {
        if (htable->elems[i])
        {
            htable_put(&tmp, htable->elems[i]);
        }
    }

    htable->size = 0;
    htable_deinit(htable);

    htable->size = tmp.size;
    htable->max = tmp.max;
    htable->elems = tmp.elems;
}
