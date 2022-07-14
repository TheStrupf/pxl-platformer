#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_USE_STDALLOC 1

#define ALIGN(s) ((s + ALIGNMENT - 1) & -ALIGNMENT)

typedef struct block {
        size_t size;
        struct block *next;
} block;

enum {
        ALIGNMENT = sizeof(void *),
        HEADER = ALIGN(sizeof(block)),
        SPLIT_THRESH = HEADER + ALIGNMENT,
};

static block *b_free = NULL;

void mem_init(void *memory, size_t memory_size)
{
        b_free = memory;
        b_free->size = memory_size;
        b_free->next = NULL;
}

void *mem_alloc(size_t s)
{
#if MEM_USE_STDALLOC == 1
        return malloc(s);
#endif
        if (s == 0)
                return NULL;
        const size_t ALLOC_SIZE = ALIGN(s) + HEADER;
        for (block *b = b_free, *p = NULL; b; p = b, b = b->next) {
                if (b->size >= ALLOC_SIZE) {
                        size_t excess = b->size - ALLOC_SIZE;
                        block *tmp = b->next;
                        if (excess >= SPLIT_THRESH) {
                                // split blocks
                                b->size = ALLOC_SIZE;
                                tmp = (block *)((char *)b + ALLOC_SIZE);
                                tmp->size = excess;
                                tmp->next = b->next;
                        }
                        // re-link free list
                        if (p)
                                p->next = tmp;
                        else
                                b_free = tmp;
                        return (void *)((char *)b + HEADER);
                }
        }
        printf("+++ ERR: Couldn't alloc memory\n");
        return NULL;
}

void mem_free(void *ptr)
{
#if MEM_USE_STDALLOC == 1
        free(ptr);
        return;
#endif
        if (!ptr)
                return;
        block *b = (block *)((char *)ptr - HEADER);
        block *prev = b->next = NULL;
        const char *b_ptr = (char *)b;

        // iterate free list to insert b (memory sorted)
        for (block *it = b_free; it; prev = it, it = it->next) {
                if (b_ptr < (char *)it) {
                        // insert b before it
                        if (b_ptr + b->size == (char *)it) {
                                // end of block b ends at it: merge blocks
                                b->size += it->size;
                                b->next = it->next;
                        } else
                                b->next = it;
                        break;
                }
        }

        if (prev) {
                if ((char *)prev + prev->size == (char *)b) {
                        // previous block p ends at b: merge blocks
                        prev->size += b->size;
                        prev->next = b->next;
                } else
                        prev->next = b;
        } else
                b_free = b;
}

void *mem_realloc(void *p, size_t s)
{
#if MEM_USE_STDALLOC == 1
        return realloc(p, s);
#endif
        if (!p)
                return mem_alloc(s);
        const block *b = (block *)((char *)p - HEADER);
        const size_t usersize = b->size - HEADER;
        if (usersize < s) {
                void *newblock = mem_alloc(s);
                if (newblock) {
                        memcpy(newblock, p, usersize);
                        mem_free(p);
                        return newblock;
                }
                return NULL;
        }
        return p;
}