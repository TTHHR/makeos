#ifndef MM_MALLOC_H
#define MM_MALLOC_H

#include <stddef.h>
#include <stdint.h>
// 简单的内存块结构
typedef struct {
    size_t size;
    struct Block *next;
} Block;

// 简单的内存管理结构
typedef struct {
    Block *free_list;
} MemoryManager;

void init_memory_manager(MemoryManager *mm, void *start, size_t size);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
uint64_t mm_getTotalMemSize();

#endif // MEMORY_MALLOC_H
