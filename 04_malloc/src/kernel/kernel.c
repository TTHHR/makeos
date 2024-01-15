#include <stdbool.h>
#include <stddef.h>
#include "../lib/stdio.h"
#include "../lib/serial.h"
#include "../lib/malloc.h"
#include "../boot/multiboot.h"
#include "kernel.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

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
MemoryManager mm;
void kernel_main(unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;
    
    unsigned long total_memory_kb = 0;
    // 初始化串口
    init_serial();
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {

        printf("Invalid magic number:%d\n", (unsigned)magic);
        return;
    }
    mbi = (multiboot_info_t *)addr;
    if (CHECK_FLAG(mbi->flags, 0))
    {
        total_memory_kb = mbi->mem_lower + mbi->mem_upper;
        if (total_memory_kb >= 1024)
        {
            printf("Memory size: %u MB\n", total_memory_kb / 1024);
        }
        else
        {
            printf("Memory size: %u KB\n", total_memory_kb);
        }
    }
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
    unsigned long base_addr = (unsigned long)mbi->mmap->base_addr_low | ((unsigned long)mbi->mmap->base_addr_high << 32);
    unsigned long mem_length = (unsigned long)mbi->mmap->length_low | ((unsigned long)mbi->mmap->length_high << 32);
    printf("base_addr %x length %d!\n",base_addr,mem_length);
    init_memory_manager(&mm, (void *)base_addr,mem_length);

    // 无限循环，防止程序退出
    while (1)
        ;
}