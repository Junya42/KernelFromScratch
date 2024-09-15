#include "../includes/vga.h"
#include "../includes/terminal.h"
#include "../includes/microshell.h"
#include "../includes/gdt.h"
#include "../includes/idt.h"
#include "../includes/keyboard.h"
#include "../includes/stdarg.h"
#include "../includes/multiboot.h"
#include "../includes/paging.h"
#include "../includes/symbol.h"

void kernel_log(char *format, ...) {
    va_list args;
    va_start(args, format);

    printf(format, LIGHT_RED, args);

    va_end(args);
}

void kernel_colored_log(unsigned char color, char *format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, color, args);

    va_end(args);
}

void kernel_panic(const char *function, const char *file, int line, char *format, ...) {
	va_list args;
	va_start(args, format);

	kernel_log("\n\033[32mFunction: %s\033[0m, File: %s, Line: %d\n", function, file, line);
	kernel_log(format, args);

	va_end(args);

	asm volatile("cli");
	while (1) {
		asm volatile("hlt");
	}
}

multiboot_info_t* multiboot_info_ptr;

void start(unsigned long magic, unsigned long addr) {

	init_gdt();
	init_idt();

	toggle_debug();
	DEBUG_PRINT("magic %d -> addr %d", magic, addr);

	multiboot_info_ptr = (multiboot_info_t *)addr;

	initialize_memory();
	initialize_paging();

	init_symbols();

	init_screens();
	init_terminal();

	init_keyboard();

	microshell(); // kernel heap

	listclear(kernel_symbols);
}