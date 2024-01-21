#include"mm_malloc.h"
// 内存池的起始地址和大小
static MemManager manager;
// 从内存池中分配内存块
void *mm_malloc(size_t size)
{
    if (size == 0|| size> (manager.totalSize-manager.usedSize))
    {
        printf("mm_malloc fail s %d t %d u %d\n", size,manager.totalSize, manager.usedSize);
        return NULL;
    }

    mem_block *current = manager.freeList;
    mem_block *prev;
    while (current != NULL)//遍历链表
    {
        printf("current %x %u %u\n",current, current->used, current->size);
        if(current->used==0&&current->size>size)//如果当前的block已经够了
        {
            break;
        }
        prev = current;
        current = current->next;
    }
    if(current==NULL)//没有发现可用block
    {
        printf("mm_malloc fail no block\n");
        return NULL;
    }
    prev = current + sizeof(mem_block);//指向下一个block
    prev->size = current->size - size;
    prev->next = NULL;
    prev->used = 0;
    prev->addr = current->addr + size;

    printf("next %x %u\n",prev->addr,prev->size);

    current->used = 1;
    current->size = size;
    current->next = prev;

    void *block = current->addr;

    manager.usedSize += size;

    printf("mm_malloc t %d u %d c 0x%x b 0x%x\n", manager.totalSize, manager.usedSize,current->addr,prev->addr);

    return block;
}

// 初始化内存池
void init_memory_pool(void *start, size_t size)
{
    //起始地址预留
    if(size<MEM_LIST_MEM)
    {
        printf("!!!!!!!memory too lite!!!!!!!!!");
    }
    manager.totalSize = size-MEM_LIST_MEM;
    manager.start = start;
    manager.usedSize = 0;

    mem_block *initial_block = (mem_block *)(start);
    initial_block->size = manager.totalSize;
    initial_block->used = 0;
    initial_block->next = NULL;
    initial_block->addr = start + MEM_LIST_MEM;
    manager.freeList = initial_block;
}
// 伙伴系统的内存释放
void mm_free(void *ptr)
{
    if (!ptr)
    {
        return;
    }
    mem_block *block = ptr;
    block->used = 0;
    //todo 合并内存
}

// 获取总内存大小
size_t mm_getTotalMemSize()
{
    return manager.totalSize;
}

// 获取剩余内存大小
size_t mm_getRemainingMemSize()
{
    return manager.totalSize - manager.usedSize;
}