#ifndef MM_MALLOC_H
#define MM_MALLOC_H

#include <stddef.h>
#include <stdint.h>
#define MEM_LIST_MEM 2048

typedef struct mem_block
{
    size_t size;
    uint8_t used;
    void *addr;
    struct mem_block *next;
} mem_block;
typedef struct MemManager
{
    size_t *start;
    size_t totalSize;
    size_t usedSize;
    mem_block *freeList;
} MemManager;

void init_memory_pool(void *start, size_t size);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
size_t mm_getTotalMemSize();
size_t mm_getRemainingMemSize();

#endif // MEMORY_MALLOC_H
