# Makefile

# Compiler and flags
CC := i686-elf-gcc
AS := i686-elf-as
LD := i686-elf-ld

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -D KERNEL_DEBUG

# Directories
SRC_DIR := src
OBJ_DIR := obj
GRUB_DIR := iso/boot
GRUB_CFG := $(GRUB_DIR)/grub.cfg

# Source files
SRCS := $(shell find $(SRC_DIR) -name '*.s' -o -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS)))

# Target binary
TARGET := iso/boot/makeos.bin

# Rule to build the binary
$(TARGET): $(OBJS)
	$(CC) -T src/boot/linker.ld -o $@ -ffreestanding -O2 -nostdlib $(OBJS) -lgcc
	grub-file --is-x86-multiboot $@ && echo "make success" || echo "make fail"

# Rule to assemble source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create directories
$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

pack: $(TARGET)
	mkdir -p $(GRUB_DIR)
	cp src/boot/grub.cfg $(GRUB_CFG)
	grub-mkrescue -o makeos.iso iso

# Rule to clean the build
clean:
	rm -rf $(OBJ_DIR) $(TARGET) makeos.iso
