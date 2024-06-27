CC = gcc
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -Wall -Wextra -Werror
LDFLAGS = -T linker.ld -m elf_i386

SRC_DIR = src
OBJ_DIR = obj

SRC_FILES = kernel.c keyboard.c screen.c
HEAD_FILES = kernel.h keyboard.h globals.h

SOURCES = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
HEADERS = $(addprefix $(SRC_DIR)/, $(HEAD_FILES))

OBJECTS = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:%.c=%.o))

KERNEL_BIN = kernel.bin

ISO = kfs.iso

all: $(ISO)

$(ISO): $(KERNEL_BIN) grub.cfg
	@echo "Creating ISO image..."
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/grub/$(KERNEL_BIN)
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir
	rm -rf isodir

$(KERNEL_BIN): boot.o $(OBJECTS)
	@echo "Linking kernel..."
	ld $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c  $(HEADERS) | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@echo "Creating directory $(OBJ_DIR)..."
	mkdir -p $(OBJ_DIR)

boot.o: boot.asm
	@echo "Assembling bootloader..."
	nasm -f elf32 $< -o $@

clean:
	@echo "Cleaning up object files..."
	rm -f $(OBJ_DIR)/*.o boot.o
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning up kernel binary..."
	rm -f $(KERNEL_BIN)
	rm -f $(ISO)

init:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y gcc nasm qemu-system-x86 grub-pc-bin grub-common xorriso mtools

run:
	@echo "Running iso in QEMU..."
	qemu-system-i386 -no-reboot -no-shutdown -s -cdrom $(ISO)

.PHONY: all clean fclean init run