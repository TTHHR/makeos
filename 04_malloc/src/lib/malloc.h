#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
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
void *malloc(size_t size);
void free(void *ptr);

#endif // MEMORY_MALLOC_H
