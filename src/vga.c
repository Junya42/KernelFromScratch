#include "../includes/vga.h"
#include "../includes/stdarg.h"
#include "../includes/stdint.h"
#include "../includes/terminal.h"
#include "../includes/common.h"
#include "../includes/io.h"

unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;
unsigned short screen_buffer[SCREENS][SCREENSIZE];
unsigned int vga_index[SCREENS];
unsigned short screen = 0;

#define COM1_PORT 0x3F8 // The standard I/O port address for COM1

static inline void serial_outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void init_serial() {
    outb(COM1_PORT + 1, 0x00);    // Disable all interrupts
    outb(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1_PORT + 1, 0x00);    //                  (hi byte)
    outb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_received() {
    return inb(COM1_PORT + 5) & 1;
}

char serial_read() {
    while (serial_received() == 0);
    return inb(COM1_PORT);
}

int is_transmit_empty() {
    return inb(COM1_PORT + 5) & 0x20;
}

void serial_putc(char c) {
    while (is_transmit_empty() == 0);
    serial_outb(COM1_PORT, c);
}

void serial_puts(const char *str) {
    while (*str) {
        serial_putc(*str++);
    }
}

void serial_print_number(int n) {
    if (n < 0) {
        serial_putc('-');
        n = -n;
    }
    if (n == 0) {
        serial_putc('0');
        return;
    }
    char buffer[10];
    int i = 0;
    while (n > 0) {
        buffer[i++] = (n % 10) + '0';
        n /= 10;
    }
    while (i > 0) {
        serial_putc(buffer[--i]);
    }
}

void serial_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'c':
                    serial_putc((char)va_arg(args, int));
                    break;
                case 's':
                    serial_puts(va_arg(args, char *));
                    break;
                case 'd':
                    serial_print_number(va_arg(args, int));
                    break;
                default:
                    serial_putc(format[i]);
                    break;
            }
        } else {
            serial_putc(format[i]);
        }
    }

    va_end(args);
}

uint8_t t_debug = 0;

void toggle_debug() {
    t_debug = !t_debug;
    serial_printf("\n\033[33mDebug mode: %s\033[0m\n", t_debug ? "on" : "off");
}

void debug_print(const char* function, const char* file, int line, const char* param_format, ...) {

    if (!t_debug) {
        return;
    }
    
    va_list args;
    va_start(args, param_format);

    serial_printf("\n\033[32mFunction: %s\033[0m, File: %s, Line: %d\n", function, file, line);

    serial_printf("\033[33mParameters:\033[0m ");

    int count = 0;
    for (int i = 0; param_format[i] != '\0'; i++) {
        if (param_format[i] == '%') {
            count++;
            i++;
            serial_printf("\033[31m");
            switch (param_format[i]) {
                case 'c':
                    serial_printf("%c ", va_arg(args, int));
                    break;
                case 'd':
                    serial_printf("%d ", va_arg(args, int));
                    break;
                case 's':
                    serial_printf("%s ", va_arg(args, char *));
                    break;
                default:
                    serial_printf("[Unsupported format specifier]");
                    break;
            }
            serial_printf("\033[0m");
        } else {
            serial_putc(param_format[i]);
        }
    }
    if (!count) {
        serial_printf("[No parameters]");
    }
    serial_printf("\n\n");
    
    va_end(args);
}

void init_screens() {

    for (int i = 0; i < SCREENS; i++) {
        vga_buffer[i] = *(unsigned short *)((unsigned short *)VGA_ADDRESS + SCREENSIZE * i);
        vga_index[i] = 0;
    }
}

void clear_screen() {
    for (int i = 0; i < VGA_BUFFER_SIZE; i++) {
        vga_buffer[i] = (unsigned short)' ' | (unsigned short)LIGHT_GRAY << 8; // LIGHT GRAY is the default color
        screen_buffer[screen][i] = vga_buffer[i];
    }
    vga_index[screen] = 0;
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

    (void)color;
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
