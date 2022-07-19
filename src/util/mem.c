#include "mem.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN(s) ((s + ALIGNMENT - 1) & -ALIGNMENT)

typedef struct block {
        size_t size;
        struct block *next;
} block;

typedef struct stack_header {
        size_t size;
        struct stack_header *prev;
} stack_header;

static block *b_free;
static char *stackptr;
static size_t stackremaining;
static stack_header *stacktop;

enum {
        ALIGNMENT = sizeof(void *),
        HEADER = ALIGN(sizeof(block)),
        STACKHEADER = ALIGN(sizeof(stack_header)),
        SPLIT_THRESH = HEADER + ALIGNMENT,
};

size_t mem_align(size_t s)
{
        return ALIGN(s);
}

void mem_init(void *mem, size_t stack_size, size_t dynamic_size)
{
        stackptr = (char *)mem;
        stackremaining = stack_size;
        stacktop = NULL;

        b_free = (block *)((char *)mem + stack_size);
        b_free->size = dynamic_size;
        b_free->next = NULL;
}

void *mem_alloc(size_t s)
{
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

void *mem_stackalloc(size_t s)
{
        size_t size = ALIGN(s);
        if (stackremaining >= STACKHEADER + size) {
                stack_header *h = (stack_header *)stackptr;
                h->size = STACKHEADER + size;
                h->prev = stacktop;
                stacktop = h;
                stackptr += STACKHEADER;
                void *m = stackptr;
                stackptr += size;
                stackremaining -= STACKHEADER + size;
                return m;
        }
        return NULL;
}

void mem_stackpop()
{
        if (stacktop) {
                stackptr -= stacktop->size;
                stackremaining += stacktop->size;
                stacktop = stacktop->prev;
        }
}

void mem_stackpop_incl(void *p)
{
        while (stacktop) {
                void *cur = (char *)stacktop + STACKHEADER;
                mem_stackpop();
                if (cur == p)
                        return;
        }
}