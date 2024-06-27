#include "kernel.h"
#include "idt.h"

unsigned short *terminal_buffer = (unsigned short *)VGA_ADDRESS;
unsigned int vga_index = 0;

void titlescreen() {

//    print_string("hello world!\n", CYAN);
    print_string("\t\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t _     _ _______ _______\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t |____/  |______ |______\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t |    \\_ |       ______|\t \t\t\t\t   \xBA\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);

    print_string("\t\xBA\t\t\t\t\t\t", YELLOW);print_string("42 - anremiki & cmarouf ", BROWN);print_string("\t\xBA\n", YELLOW);
    
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n\n", YELLOW);
    
    prompt();
}

void start() {

    init_idt();
    
    enable_irq(1);

    asm volatile("sti");

    clear_screen();

    titlescreen();

    while (1) {
    }

}