#include "kernel.h"

unsigned short *terminal_buffer = (unsigned short *)VGA_ADDRESS;
unsigned int vga_index = 0;

void start() {

    clear_screen();
    PRINT_STRING("Hello 42!\n");
    print_string("Red color test!\n", RED);
    print_string("Blue no new line", BLUE);
    print_string("and now yellow", YELLOW);

    while (1) {
        keyboard_handler();
    }

}