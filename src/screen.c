#include "kernel.h"
#include "globals.h"

void clear_screen(void) {

    int i = 0;

    while (i < LINES * COLUMNS * 2) {
        terminal_buffer[i] = ' ';
        i += 2;
    }
}

void print_string(char *str, unsigned char color) {

    int i = 0;
    while (str[i]) {

        if (str[i] == '\n')
            vga_index = (vga_index / COLUMNS + 1) * COLUMNS;
        else
            terminal_buffer[vga_index++] = (unsigned short)str[i] | (unsigned short)color << 8;
        i++;
    }
}

void print_char(char c, unsigned char color) {

    if (c == '\n')
        vga_index = (vga_index / COLUMNS + 1) * COLUMNS;
    else
        terminal_buffer[vga_index++] = c | (unsigned short)color << 8;
}