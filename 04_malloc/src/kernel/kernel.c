#include <stdbool.h>
#include <stddef.h>
#include "../lib/stdio.h"
#include "../lib/serial.h"
#include "../lib/mm_malloc.h"
#include "../boot/multiboot.h"
#include "kernel.h"

void delay(int milliseconds)
{
    // 假设每个循环迭代大约耗费 200000 个 CPU 周期
    volatile long cycles = milliseconds * 200000;

    // 忙等待，等待一段时间
    while (cycles > 0)
    {
        cycles--;
    }
}
uint32_t check_mode()
{
    uint32_t result;
    __asm__ __volatile__(
        "movl %%cr0, %0;" // 将 CR0 寄存器的值加载到 result 变量中
        "andl $1, %0;"    // 使用 AND 指令检查第 0 位
        : "=r"(result)    // 输出操作数，将 result 和 %0 关联
        :                 // 输入操作数为空
        : "memory"        // 告诉编译器内联汇编可能会修改内存
    );
    return result;
}
#ifdef KERNEL_DEBUG
void printMultiBootInfo(multiboot_info_t *mbi)
{
    uint64_t tmp64;
    if (mbi == NULL) {
        printf("Multiboot info pointer is NULL.\n");
        return;
    }

    printf("Multiboot Information:\n");
    printf("  Flags: 0x%x\n", mbi->flags);
    printf("  Memory Lower: %u KB\n", mbi->mem_lower);
    printf("  Memory Upper: %u KB\n", mbi->mem_upper);
    printf("  Boot Device: 0x%x\n", mbi->boot_device);
    printf("  Command Line: %s\n", mbi->cmdline);

    printf("  Modules:\n");
    printf("    Count: %u\n", mbi->mods_count);
    if (mbi->mods_count > 0) {
        module_t *mods = (module_t *)mbi->mods_addr;
        for (unsigned long i = 0; i < mbi->mods_count; ++i) {
            printf("    Module %u:\n", i + 1);
            printf("      Start Address: 0x%x\n", mods[i].mod_start);
            printf("      End Address: 0x%x\n", mods[i].mod_end);
            printf("      String: 0x%x\n", mods[i].string);
            printf("      Reserved: 0x%x\n", mods[i].reserved);
        }
    }

    printf("  Kernel Information:\n");
    if (CHECK_FLAG(mbi->flags, MBF_AOUT)) {
        printf("    AOUT Symbol Table:\n");
        printf("      Tabsize: %u\n", mbi->u.aout_sym.tabsize);
        printf("      Strsize: %u\n", mbi->u.aout_sym.strsize);
        printf("      Addr: 0x%x\n", mbi->u.aout_sym.addr);
        printf("      Reserved: 0x%x\n", mbi->u.aout_sym.reserved);
    } else if (CHECK_FLAG(mbi->flags, MBF_ELF)) {
        printf("    ELF Section Header Table:\n");
        printf("      Num: %u\n", mbi->u.elf_sec.num);
        printf("      Size: %u\n", mbi->u.elf_sec.size);
        printf("      Addr: 0x%x\n", mbi->u.elf_sec.addr);
        printf("      Shndx: %u\n", mbi->u.elf_sec.shndx);
    }

    printf("  Memory Maps:\n");
    printf("    Length: %u\n", mbi->mmap_length);
    if (mbi->mmap_length > 0) {
        memory_map_t *mmap = (memory_map_t *)mbi->mmap;
        while ((unsigned long)mmap < mbi->mmap + mbi->mmap_length) {
            if (mmap->type == 1 || mmap->type == 3 || mmap->type == 4 || mmap->type == 5)
            {
            printf("    Entry:\n");
            printf("      Size: %u\n", mmap->size);
            tmp64=(mmap->base_addr_high<<32)+mmap->base_addr_low;
            printf("      Base Address: 0x%x%x 0x%x\n", mmap->base_addr_high, mmap->base_addr_low,tmp64/1024);
            tmp64=(mmap->length_high<<32)+mmap->length_low;
            printf("      Length: 0x%x%x Bytes = %d KB\n", mmap->length_high, mmap->length_low,tmp64/1024);
            printf("      Type: %u\n", mmap->type);
            }
            mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size));
        }
    }
}
#endif
void select_memory_region(multiboot_info_t* mbi, void** start, size_t* size) {
    uint64_t tmp64;
    if (mbi->mmap_length > 0) {
        memory_map_t *mmap = (memory_map_t *)mbi->mmap;
        while ((unsigned long)mmap < mbi->mmap + mbi->mmap_length) {
            if (mmap->type == 1 || mmap->type == 3 || mmap->type == 4 || mmap->type == 5)
            {
            
            tmp64=(mmap->length_high<<32)+mmap->length_low;
                if(tmp64>mbi->mem_lower*1024)//大于第一块内存就用它
                {
                    *size=tmp64;
                    tmp64=(mmap->base_addr_high<<32)+mmap->base_addr_low;
                    *start=tmp64;
                    #ifdef KERNEL_DEBUG
                    tmp64=*size>1024*1024*1024?*size/1024/1024/1024:*size>1024*1024?*size/1024/1024:*size;
                    printf("use mbi memory addr:0x%x \nuse mbi memory size: %d ",*start,tmp64);
                    printf("%s \n",*size>1024*1024*1024?"GB":*size>1024*1024?"MB":"KB");
                    #endif
                    return;
                }
            }
            mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size));
        }
    }
    else
    {
        printf("no memory info!\n");
    }
}

void kernel_main(unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;
    uint64_t memory_addr;
    uint64_t total_memory;
    // 初始化串口
    init_serial();
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Invalid magic number:%d\n", (unsigned)magic);
        return;
    }
    mbi = (multiboot_info_t *)addr;
#ifdef KERNEL_DEBUG
    printMultiBootInfo(mbi);
#endif
    //获取内存大小，但是这种方式可能有问题
    // if (CHECK_FLAG(mbi->flags, 0))
    // {
    //     total_memory = mbi->mem_lower + mbi->mem_upper;
    //     if (total_memory >= 1024)
    //     {
    //         printf("Memory size: %u MB\n", total_memory / 1024);
    //     }
    //     else
    //     {
    //         printf("Memory size: %u KB\n", total_memory);
    //     }
    // }
    uint32_t mode = check_mode();

    if (mode == 0)
    {
        // 处于实模式
        printf("run on real mode!\n");
    }
    else
    {
        // 处于保护模式
        printf("run on protect mode!\n");
    }
    select_memory_region(mbi,&memory_addr,&total_memory);
    init_memory_pool((void *)memory_addr, total_memory);
#ifdef KERNEL_DEBUG
    uint8_t checkStatus = 0;
    #define memLength  (1024*512)
    checkStatus = 0;
    printf("total size %u remain size %u \n", mm_getTotalMemSize(), mm_getRemainingMemSize());
    uint8_t *requestMem = mm_malloc(memLength);
    printf("get mem addr:%x endaddr:%x\n", requestMem,requestMem+memLength);
    if (requestMem != NULL)
    {
        for (uint64_t i = 0; i < memLength; i++)
        {
            ((uint8_t *)requestMem)[i] = (i & 0xFF);
        }
        printf("write mem done\n");
        for (uint64_t i = 0; i < memLength; i++)
        {
            if (requestMem[i] != (i & 0xFF))
            {
                printf("mem error at %x\n", &requestMem[i]);
                checkStatus = 1;
                break;
            }
        }
        printf("mem check %s \n", checkStatus ? "fail" : "ok");
        mm_free(requestMem);
    }
    else
    {
        printf("malloc mem fail!\n");
    }
    delay(1000);
    size_t *memPtr[8196];
    for (uint32_t block = 0; block < sizeof(memPtr); block++)
    {
        delay(500);
        printf("Remainning mem size %u\n", mm_getRemainingMemSize());
        memPtr[block] = mm_malloc(memLength);
        if(memPtr[block]==NULL)
        break;
        printf("malloc addr 0x%x\n",memPtr[block]);
    }
    for (uint32_t block = 0; block < sizeof(memPtr); block++)
    {
        if (memPtr[block] == 0)
        {
            printf("total get mem block %d\n", block);
            break;
        }
        mm_free(memPtr[block]);
        printf("Remainning mem size %u\n", mm_getRemainingMemSize());
    }
#endif
    // 无限循环，防止程序退出
    while (1)
        ;
}