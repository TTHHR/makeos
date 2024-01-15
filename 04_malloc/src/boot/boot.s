
#include "multiboot.h"

/* Csymbol format. HAVE ASM USCORE is defined by configure. */

/*The size of our stack (16KB). */

.set STACK_SIZE,0x4000

/*The flags for the Multiboot header. */
.set MULTIBOOT_HEADER_MAGIC	,	0x1BADB002
.set MULTIBOOT_HEADER_FLAGS , 0x00000003

.text

.globl start, _start

start:

_start:

        jmp multiboot_entry

            /* Align 32 bits boundary. */

.align 4
/* Multiboot header. */

multiboot_header:

/* magic */

.long MULTIBOOT_HEADER_MAGIC

/* flags */

.long MULTIBOOT_HEADER_FLAGS

/* checksum */

.long -(MULTIBOOT_HEADER_MAGIC +MULTIBOOT_HEADER_FLAGS)

#ifndef __ELF__

/* header addr */

.long multiboot_header

/* load addr */

.long _start

/* load end addr */

.long _edata

/* bss end addr */

.long _end

/* entry addr */

.long multiboot_entry

#endif /* ! ELF */

 

multiboot_entry:

            /* Initialize the stack pointer. */

            movl $(stack_top+STACK_SIZE), %esp

            /* Reset EFLAGS. */

            pushl $0

            popf

            /* Push the pointer to the Multibootinformation structure. */

            pushl %ebx

            /* Push the magic value. */

            pushl %eax

            /* Now enter the C main function... */

            call kernel_main

