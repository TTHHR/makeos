# Makefile

# Compiler and flags
CC := i686-elf-gcc
AS := i686-elf-as
LD := i686-elf-ld

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Source files
SRCS := boot.s kernel.c serial.c stdio.c
OBJS := $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))

# Target binary
TARGET := makeos.bin

# Rule to build the binary
$(TARGET): $(OBJS)
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(OBJS) -lgcc
	grub-file --is-x86-multiboot $@ && echo "make success" || echo "make fail"

# Rule to assemble source files
%.o: %.s
	$(AS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

pack: $(TARGET)
	rm -rf iso/
	mkdir -p $(GRUB_DIR)
	cp $(TARGET) $(GRUB_DIR)/
	cp grub.cfg $(GRUB_CFG)
	grub-mkrescue -o makeos.iso iso

# Rule to clean the build
clean:
	rm -f $(OBJS) $(TARGET) makeos.iso
