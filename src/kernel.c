#include "../includes/vga.h"
#include "../includes/terminal.h"
#include "../includes/microshell.h"
#include "../includes/gdt.h"
#include "../includes/idt.h"
#include "../includes/keyboard.h"
#include "../includes/stdarg.h"
#include "../includes/multiboot.h"
#include "../includes/paging.h"
#include "../includes/ksignal.h"
#include "../includes/sighandler.h"
//#include "../includes/symbol.h"

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

void kernel_halt(void) {
	asm volatile ("hlt");
}

void kernel_clear_registers(void) {
	asm volatile ("mov $0, %%eax" ::: "eax");
    asm volatile ("mov $0, %%ebx" ::: "ebx");
    asm volatile ("mov $0, %%ecx" ::: "ecx");
    asm volatile ("mov $0, %%edx" ::: "edx");
	kernel_colored_log(YELLOW, "All registers have been cleared.\n");
}

void kernel_panic(const char *function, const char *file, int line, char *format, ...) {
	va_list args;
	va_start(args, format);

	kernel_log("\n\033[32mFunction: %s\033[0m, File: %s, Line: %d\n", function, file, line);
	kernel_log(format, args);

	va_end(args);

	asm volatile("cli");
	kernel_clear_registers();
	while (1) {
		kernel_halt();
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

	//init_symbols();

	init_signals();
	init_screens();
	init_terminal();

	init_keyboard();

	signal(SIG_INTERRUPT, handle_sigint);
	signal(SIG_ALARM, handle_sigalarm);
	schedule_repeat_signal(SIG_ALARM, (uint64_t)5000, -1);
	microshell(); // kernel heap

	//listclear(kernel_symbols);
}
