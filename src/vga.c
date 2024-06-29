#include "../includes/vga.h"
#include "../includes/stdarg.h"
#include "../includes/stdint.h"
#include "../includes/terminal.h"

unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;
unsigned short screen_buffer[SCREENS][SCREENSIZE];
unsigned int vga_index[SCREENS];
unsigned short screen = 0;

void init_screens() {

    for (int i = 0; i < SCREENS; i++) {
        vga_buffer[i] = (unsigned short *)VGA_ADDRESS + SCREENSIZE * i;
        vga_index[i] = 0;
    }
}

void clear_screen() {
    for (int i = 0; i < VGA_BUFFER_SIZE; i++) {
        vga_buffer[i] = (unsigned short)' ' | (unsigned short)LIGHT_GRAY << 8; // LIGHT GRAY is the default color
        screen_buffer[screen][i] = vga_buffer[i];
    }
    *vga_index = 0;
}

void delete() {
    if (vga_index[screen] < VGA_BUFFER_SIZE) {
        for (int i = vga_index[screen]; i < VGA_BUFFER_SIZE - 1; i++) {
            vga_buffer[i] = vga_buffer[i + 1];
            screen_buffer[screen][i] = vga_buffer[i];
        }
        vga_buffer[VGA_BUFFER_SIZE - 1] = (unsigned short)' ' | (unsigned short)LIGHT_GRAY << 8;
        screen_buffer[screen][VGA_BUFFER_SIZE - 1] = vga_buffer[VGA_BUFFER_SIZE - 1];
    }
}

void backspace() {
    if (vga_index[screen] > 0) {
        vga_index[screen]--;
        delete();
        move_cursor();
    }
}

void scroll() {
    // Move each line up
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
            screen_buffer[screen][y * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }

    // Clear the last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (LIGHT_GRAY << 8) | ' ';
        screen_buffer[screen][(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x];
    }
}

void moveCursor(int row, int col) {
    // ANSI escape code to move the cursor
    printf("\033[%d;%dH", row, col);
}

void vga_bounds_check(unsigned char color) {

    if (vga_index[screen] >= VGA_BUFFER_SIZE) {

            scroll();
           /*
           // Scroll the screen up if we reach the end
            for (int j = 0; j < VGA_BUFFER_SIZE - VGA_WIDTH; j++)
                vga_buffer[j] = vga_buffer[j + VGA_WIDTH];

            // Clear the last line
            for (int j = VGA_BUFFER_SIZE - VGA_WIDTH; j < VGA_BUFFER_SIZE; j++)
                vga_buffer[j] = (unsigned short)' ' | (unsigned short)color << 8;

            // Set vga_index to the start of the last line
            */
            vga_index[screen] = VGA_BUFFER_SIZE - VGA_WIDTH;
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


void print_string(char *str, unsigned char color) {

    int i = 0;
    while (str[i]) {
        
        unsigned char character = str[i];

        if (str[i] == '\n') {
            vga_index[screen] = (vga_index[screen] / VGA_WIDTH + 1) * VGA_WIDTH;
        }
        else {
            if (str[i] == '\t')
                print_string("    ", color);
            else {
                vga_buffer[vga_index[screen]] = (unsigned short)character | (unsigned short)color << 8;
                screen_buffer[screen][vga_index[screen]] = vga_buffer[vga_index[screen]];
                vga_index[screen]++;
            }
        }

        vga_bounds_check(color);
        i++;
    }
    move_cursor();
}

void print_char(char c, unsigned char color) {

    if (c == '\n')
        vga_index[screen] = (vga_index[screen] / VGA_WIDTH + 1) * VGA_WIDTH;
    else if (c == '\t')
        print_string("    ", color);
    else {
        vga_buffer[vga_index[screen]] = (unsigned char)c | (unsigned short)color << 8;
        screen_buffer[screen][vga_index[screen]] = vga_buffer[vga_index[screen]];
        vga_index[screen]++;
    }
    vga_bounds_check(color);
    move_cursor();
}

void printf(char *format, unsigned char color, ...) {
    va_list args;
    va_start(args, color);
    
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'c':
                    print_char((char)va_arg(args, int), color);
                    break;
                case 's':
                    print_string(va_arg(args, char *), color);
                    break;
                case 'd':
                    print_number(va_arg(args, int), color);
                    break;
                default:
                    print_char(format[i], color);
                    break;
            }
        } else {
            print_char(format[i], color);
        }
    }
    
    va_end(args);
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void change_screen(unsigned short new_screen) {
    screen = new_screen;
    memcpy(vga_buffer, screen_buffer[screen], SCREENSIZE);
    move_cursor();
}

void move_cursor() {
    int x = vga_index[screen] % VGA_WIDTH;
    int y = vga_index[screen] / VGA_WIDTH;
    uint16_t position = y * VGA_WIDTH + x;

    // Send the high byte of the cursor position
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)(position >> 8));
    // Send the low byte of the cursor position
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
}

void prompt() {

    printf("\n[ Root ]: ", LIGHT_GREEN);
}
