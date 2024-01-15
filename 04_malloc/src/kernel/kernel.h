#ifndef KERNEL_H
#define KERNEL_H
/*Check if the bit BIT in FLAGS is set. */

#define CHECK_FLAG(flags,bit) ((flags)& (1 << (bit)))

void delay(int milliseconds) ;
uint32_t check_mode();
#endif