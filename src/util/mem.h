#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

#define KB(n) (n * 1024)
#define MB(n) (n * KB(1024))

// Free list allocator working on a user provided memory buffer

// Provide memory buffer pointer and size
// 1 success, 0 failure
void mem_init(void *, size_t);

// Scans free list first fit. Splits block if necessary
void *mem_alloc(size_t);

// Frees block. Coalesces neighbour blocks if possible
void mem_free(void *);
void *mem_realloc(void *, size_t);

#endif