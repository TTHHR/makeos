#include "serial.h"
uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}

void init_serial() {
    // 初始化串口，设置波特率等
    outb(0x3F8 + 1, 0x00);  // Disable all interrupts
    outb(0x3F8 + 3, 0x80);  // Enable DLAB (set baud rate divisor)
    outb(0x3F8 + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
    outb(0x3F8 + 1, 0x00);  //                  (hi byte)
    outb(0x3F8 + 3, 0x03);  // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    outb(0x3F8 + 4, 0x0B);  // IRQs enabled, RTS/DSR set
}
void putc_serial(char c) {
    // 等待串口可以发送
    while ((inb(0x3F8 + 5) & 0x20) == 0);

    // 发送字符到串口
    outb(0x3F8, c);
}

void puts_serial(const char* str) {
    // 逐字符发送字符串
    while (*str) {
        putc_serial(*str);
        str++;
    }
}