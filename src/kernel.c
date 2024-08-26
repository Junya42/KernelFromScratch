#include "../includes/vga.h"
#include "../includes/terminal.h"
#include "../includes/microshell.h"
#include "../includes/gdt.h"
#include "../includes/idt.h"
#include "../includes/keyboard.h"
#include "../includes/stdarg.h"

void kernel_log(char *format, ...) {
    va_list args;
    va_start(args, format);

    printf(format, LIGHT_RED, args);

    va_end(args);
}

void kernel_panic(char *message) {
    kernel_log("Kernel Panic: %s\n", message);
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}

void kernel_panic_with_log(char *format, ...) {
	va_list args;
	va_start(args, format);

	kernel_log(format, args);

	va_end(args);

	asm volatile("cli");
	while (1) {
		asm volatile("hlt");
	}
}

void start() {

	init_gdt();
	init_idt();

	init_screens();
	init_terminal();

	init_keyboard();

	microshell();
}