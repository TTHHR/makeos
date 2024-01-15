#include <stdbool.h>
#include <stddef.h>
#include "serial.h"
#include "kernel.h"
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
void delay(int milliseconds) {
    // 假设每个循环迭代大约耗费 200000 个 CPU 周期
    volatile long cycles = milliseconds * 200000;

    // 忙等待，等待一段时间
    while (cycles > 0) {
        cycles--;
    }
}
uint32_t check_mode() {
    uint32_t result;
    __asm__ __volatile__ (
        "movl %%cr0, %0;"   // 将 CR0 寄存器的值加载到 result 变量中
        "andl $1, %0;"       // 使用 AND 指令检查第 0 位
        : "=r" (result)      // 输出操作数，将 result 和 %0 关联
        :                    // 输入操作数为空
        : "memory"           // 告诉编译器内联汇编可能会修改内存
    );
    return result;
}
void kernel_main(void) 
{
    int i=0;
 // 初始化串口
    init_serial();
	while(i<3)
	{
		// 输出日志信息到串口
    	puts_serial("Hello, serial port!\n");
		delay(1000);
        i++;
	}
    uint32_t mode = check_mode();

    if (mode == 0) {
        // 处于实模式
        puts_serial("run on real mode!\n");
    } else {
        // 处于保护模式
        puts_serial("run on protect mode!\n");
    }
    // 无限循环，防止程序退出
    while (1);
}