#include"mm_malloc.h"
static MemoryManager *memoryManager;
// 初始化内存管理器
void init_memory_manager(MemoryManager *mm, void *start, size_t size) {
    memoryManager=mm;
    // 计算内存池的起始地址和大小，你可能需要根据实际需求修改这部分
    void *memory_pool_start = start;
    size_t memory_pool_size = size;

    // 可以根据实际需求进行一些调整，例如减去一定的边界空间
    memory_pool_size -= 1024*1024 /* 预留1M */;

    // 创建初始的内存块
    Block *initial_block = (Block *)memory_pool_start;
    initial_block->size = memory_pool_size;
    initial_block->next = NULL;

    // 将初始的内存块添加到内存管理器的空闲列表中
    memoryManager->free_list = initial_block;
    memoryManager->free_list = (Block *)start;
}

// 简单的内存分配函数
void *mm_malloc(size_t size) {
    
    // 简单的首次适应算法
    Block *current = memoryManager->free_list;
    Block *prev = NULL;

    while (current) {
        if (current->size >= size) {
            // 找到足够大的内存块
            if (current->size > size) {
                // 将剩余的部分作为新的空闲块
                Block *new_block = (Block *)((char *)current + size);
                new_block->size = current->size - size;
                new_block->next = current->next;
                current->size = size;
                current->next = new_block;
            }

            // 更新空闲块链表
            if (prev) {
                prev->next = current->next;
            } else {
                memoryManager->free_list = current->next;
            }

            return (void *)((char *)current + sizeof(Block));
        }

        prev = current;
        current = current->next;
    }

    // 没有足够大的空闲块
    return NULL;
}

// 简单的内存释放函数
void mm_free(void *ptr) {
    // 将块放回空闲块链表
    Block *block = (Block *)((char *)ptr - sizeof(Block));
    block->next = memoryManager->free_list;
    memoryManager->free_list = block;
}

uint64_t mm_getTotalMemSize()
{
    return memoryManager->free_list->size;
}
