#include "../includes/terminal.h"
#include "../includes/vga.h"

void title_screen(void) {

    print_string("\t\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    printf("\t\xBA\t _     _ _______ _______\t\tScreen %d\t\t\xBA\n", YELLOW, screen + 1);
    print_string("\t\xBA\t |____/  |______ |______\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t |    \\_ |       ______|\t \t\t\t\t   \xBA\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);

    print_string("\t\xBA\t\t\t\t\t\t", YELLOW);print_string("42 - anremiki & cmarouf ", BROWN);print_string("\t\xBA\n", YELLOW);
    
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n\n", YELLOW);
    
}

void init_terminal(void) {

    for (int i = 0; i < SCREENS; i++) {
        clear_screen();
        title_screen();
        prompt();
        change_screen(i + 1);
    }
    change_screen(0);
}