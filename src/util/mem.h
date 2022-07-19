#ifndef MEM_H
#define MEM_H

#include "shared.h"
#include <stdbool.h>
#include <stdlib.h>

#define KB(n) (n * 1024)
#define MB(n) (n * KB(1024))

// Free list allocator working on a user provided memory buffer

// Provide memory buffer pointer and size
// 1 success, 0 failure

size_t mem_align(size_t);
void mem_init(void *mem, size_t stack_size, size_t dynamic_size);

// Scans free list first fit. Splits block if necessary
void *mem_alloc(size_t);

// Frees block. Coalesces neighbour blocks if possible
void mem_free(void *);
void *mem_realloc(void *, size_t);

void *mem_stackalloc(size_t);
void mem_stackpop();
void mem_stackpop_incl(void *);

#endif