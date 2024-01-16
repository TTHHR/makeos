#ifndef STDIO_H
#define STDIO_H
#include <stddef.h>
#include <stdint.h>
void printf(const char *format, ...);
// 设置内存块的内容为指定值
void *memset(void *dest, int value, size_t count);

// 复制内存块的内容
void *memcpy(void *dest, const void *src, size_t count);

#endif