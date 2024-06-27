CC = gcc
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -Wall -Wextra -Werror
LDFLAGS = -T linker.ld -m elf_i386

SRC_DIR = src
ASM_DIR = asm
OBJ_DIR = obj

SRC_FILES = kernel.c keyboard.c screen.c idt.c
ASM_FILES = idt_load.S
HEAD_FILES = kernel.h keyboard.h globals.h idt.h

SOURCES = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
ASM_SOURCES = $(addprefix $(SRC_DIR)/, $(ASM_FILES))
HEADERS = $(addprefix $(SRC_DIR)/, $(HEAD_FILES))

# Convert .c and .S source files to object files
CODE_OBJECTS = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))
ASM_OBJECTS = $(addprefix $(OBJ_DIR)/, $(ASM_FILES:.S=.o))

KERNEL_BIN = kernel.bin
ISO = kfs.iso

CCYAN=\033[36m
CEND=\033[0m

all: fclean build run

build: $(ISO)

$(ISO): $(KERNEL_BIN) grub.cfg
	@echo "$(CCYAN)Creating ISO image...$(CEND)"
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/$(KERNEL_BIN)
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir
	rm -rf isodir

$(KERNEL_BIN): boot.o $(ASM_OBJECTS) $(CODE_OBJECTS)
	@echo "$(CCYAN)Linking kernel...$(CEND)"
	ld $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(ASM_DIR)/%.S | $(OBJ_DIR)
	@echo "$(CCYAN)Assembling $<...$(CEND)"
	nasm -f elf $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	@echo "$(CCYAN)Compiling $<...$(CEND)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@echo "$(CCYAN)Creating directory $(OBJ_DIR)...$(CEND)"
	mkdir -p $(OBJ_DIR)

boot.o: boot.asm
	@echo "$(CCYAN)Assembling bootloader...$(CEND)"
	nasm -f elf32 -g -F dwarf $< -o $@

clean:
	@echo "$(CCYAN)Cleaning up object files...$(CEND)"
	rm -f $(OBJ_DIR)/*.o boot.o
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(CCYAN)Cleaning up kernel binary...$(CEND)"
	rm -f $(KERNEL_BIN)
	rm -f $(ISO)

init:
	@echo "$(CCYAN)Installing dependencies...$(CEND)"
	sudo apt-get update
	sudo apt-get install -y gcc nasm qemu-system-x86 grub-pc-bin grub-common xorriso mtools

run:
	@echo "$(CCYAN)Running iso in QEMU...$(CEND)"
	qemu-system-i386 -cdrom $(ISO) -d int,cpu_reset -no-reboot

force:
	qemu-system-i386 -no-reboot -no-shutdown -s -cdrom $(ISO)

debug:
	qemu-system-i386 -s -S -cdrom kfs.iso

.PHONY: all build clean fclean init run
