#include "../includes/vga.h"
#include "../includes/terminal.h"
#include "../includes/microshell.h"
#include "../includes/gdt.h"
#include "../includes/idt.h"
#include "../includes/keyboard.h"

void start() {

	init_gdt();
	init_idt();

	init_screens();
	init_terminal();
	
	init_keyboard();

	microshell();
}