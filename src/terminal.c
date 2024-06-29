#include "../includes/terminal.h"

unsigned short current_term = 0;

void title_screen(uint8_t screen) {

    print_string("\t\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    printf("\t\xBA\t _     _ _______ _______\t\tScreen %d\t\t\xBA\n", YELLOW, screen);
    print_string("\t\xBA\t |____/  |______ |______\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t |    \\_ |       ______|\t \t\t\t\t   \xBA\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);

    print_string("\t\xBA\t\t\t\t\t\t", YELLOW);print_string("42 - anremiki & cmarouf ", BROWN);print_string("\t\xBA\n", YELLOW);
    
    print_string("\t\xBA\t\t\t\t\t\t\t\t\t\t\t\t\t\xBA\n", YELLOW);
    print_string("\t\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n\n", YELLOW);
    
}