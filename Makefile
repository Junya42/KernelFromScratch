CC = gcc
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -Wall -Wextra
LDFLAGS = -T linker.ld -m elf_i386

SRC_DIR = src
HEADER_DIR = includes
ASM_DIR = asm
OBJ_DIR = obj

SRC_FILES = common.c io.c isr_handler.c gdt.c idt.c interrupts_handlers.c irq.c \
			kernel.c keyboard.c microshell.c terminal.c vga.c paging.c memory.c \
			chained_list.c ksignal.c sighandler.c

HEAD_FILES = common.h io.h gdt.h idt.h interrupts.h keyboard.h stdarg.h stddef.h \
			 stdint.h terminal.h vga.h paging.h kernel.h memory.h chained_list.h \
			 ksignal.h sighandler.h
ASM_FILES = boot.asm gdt_flush.asm idt_flush.asm interrupts.asm isr.asm a_paging.asm

SOURCES = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
ASM_SOURCES = $(addprefix $(ASM_DIR)/, $(ASM_FILES))
HEADERS = $(addprefix $(HEADER_DIR)/, $(HEAD_FILES))

# Convert .c and .S source files to object files
CODE_OBJECTS = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))
ASM_OBJECTS = $(addprefix $(OBJ_DIR)/, $(ASM_FILES:.asm=.o))

KERNEL_BIN = kernel.bin
ISO = kfs.iso

CCYAN=\033[36m
CRED=\033[31m
CGREEN=\033[32m
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

$(KERNEL_BIN): $(ASM_OBJECTS) $(CODE_OBJECTS)
	@echo "$(CCYAN)Linking kernel...$(CEND)"
	@echo "$(CCYAN)--- $^ ---$(CEND)"
	ld $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(ASM_DIR)/%.asm | $(OBJ_DIR)
	@echo "$(CCYAN)Assembling $<...$(CEND)"
	nasm -f elf32 -g -F dwarf $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	@echo "$(CCYAN)Compiling $<...$(CEND)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@echo "$(CCYAN)Creating directory $(OBJ_DIR)...$(CEND)"
	mkdir -p $(OBJ_DIR)

boot.o: boot.asm
	@echo "$(CCYAN)Assembling bootloader...$(CEND)"
	nasm -f elf32 -g -F dwarf $< -o $@

# Multiboot check rule
multiboot-check: $(KERNEL_BIN)
	@if grub-file --is-x86-multiboot $(KERNEL_BIN); then \
		echo "$(CGREEN)multiboot confirmed$(CEND)"; \
	else \
		echo "$(CRED)the file is not multiboot$(CEND)"; \
	fi

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
	qemu-system-i386 -cdrom $(ISO) -serial mon:stdio

log:
	@echo "$(CCYAN)Running iso in QEMU...$(CEND)"
	qemu-system-i386 -cdrom $(ISO) -d int,cpu_reset -no-reboot

force:
	qemu-system-i386 -no-reboot -no-shutdown -s -cdrom $(ISO)

debug:
	qemu-system-i386 -s -S -cdrom $(ISO)

print-headers:
	@echo $(HEADERS)

print-srcs:
	@echo $(SOURCES)

.PHONY: all build clean fclean init run log force debug print-headers print-srcs
