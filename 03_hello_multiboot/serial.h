#ifndef SERIAL_H
#define SERIAL_H
#include <stdint.h>
void init_serial();
void putc_serial(char c);
void puts_serial(const char* str) ;
#endif