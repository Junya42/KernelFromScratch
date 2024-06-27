#include "kernel.h"
#include "globals.h"

void clear_screen() {
    for (int i = 0; i < VGA_BUFFER_SIZE; i++) {
        terminal_buffer[i] = (unsigned short)' ' | (unsigned short)LIGHT_GRAY << 8; // LIGHT GRAY is the default color
    }
    vga_index = 0;
}

void vga_bounds_check(unsigned char color) {

    if (vga_index >= VGA_BUFFER_SIZE) {

            // Scroll the screen up if we reach the end
            for (int j = 0; j < VGA_BUFFER_SIZE - COLUMNS; j++)
                terminal_buffer[j] = terminal_buffer[j + COLUMNS];

            // Clear the last line
            for (int j = VGA_BUFFER_SIZE - COLUMNS; j < VGA_BUFFER_SIZE; j++)
                terminal_buffer[j] = (unsigned short)' ' | (unsigned short)color << 8;

            // Set vga_index to the start of the last line
            vga_index = VGA_BUFFER_SIZE - COLUMNS;
    }
}

void    print_number(int n, unsigned char color)
{
	if (n >= 0)
		n = -n;
	else
		print_char('-', color);
	if (n < -9)
		print_number(-n / 10, color);
	print_char(-(n % 10) + '0', color);
}

void prompt() {
    PRINT_CHAR('\n');
    print_string("[ aremiki ]: ", LIGHT_GREEN);
    PRINT_CHAR('_');
}

void print_string(char *str, unsigned char color) {

    int i = 0;
    while (str[i]) {
        
        unsigned char character = str[i];

        if (str[i] == '\n') {
            vga_index = (vga_index / COLUMNS + 1) * COLUMNS;
        }
        else {
            if (str[i] == '\t')
                print_string("    ", color);
            else
                terminal_buffer[vga_index++] = (unsigned short)character | (unsigned short)color << 8;
        }

        vga_bounds_check(color);
        i++;
    }
}

void print_char(char c, unsigned char color) {

    if (c == '\n')
        vga_index = (vga_index / COLUMNS + 1) * COLUMNS;
    else if (c == '\t')
        print_string("    ", color);
    else
        terminal_buffer[vga_index++] = (unsigned char)c | (unsigned short)color << 8;
}