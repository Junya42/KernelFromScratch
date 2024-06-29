#include "../includes/vga.h"
#include "../includes/terminal.h"
#include "../includes/microshell.h"
#include "../includes/gdt.h"
#include "../includes/idt.h"
#include "../includes/keyboard.h"

void start() {

	clear_screen();
	init_gdt();
	init_idt();
	title_screen();
	init_keyboard();

	microshell();
}