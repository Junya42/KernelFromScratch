#ifndef VGA_H
# define VGA_H

extern unsigned short *vga_buffer; // VGA buffer
extern unsigned int vga_index[3]; // VGA buffer index
extern unsigned short screen;

#define VGA_ADDRESS 0xB8000

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define BYTES_PER_CHAR 2
#define SCREENSIZE VGA_HEIGHT * VGA_WIDTH * BYTES_PER_CHAR
#define SCREENS 3

#define VGA_BUFFER_SIZE (VGA_HEIGHT * VGA_WIDTH)

#define BLACK 0x00
#define BLUE 0x01
#define GREEN 0x02
#define CYAN 0x03
#define RED 0x04
#define MAGENTA 0x05
#define BROWN 0x06
#define LIGHT_GRAY 0x07
#define DARK_GRAY 0x08
#define LIGHT_BLUE 0x09
#define LIGHT_GREEN 0x0A
#define LIGHT_CYAN 0x0B
#define LIGHT_RED 0x0C
#define LIGHT_MAGENTA 0x0D
#define YELLOW 0x0E
#define WHITE 0x0F

void toggle_debug();
void init_screens(void);
void clear_screen(void);
void backspace(void);
void delete(void);
void print_string(char *str, unsigned char color);
void print_char(char c, unsigned char color);
void print_number(int n, unsigned char color);
void change_screen(unsigned short new_screen);
void move_cursor(void);
void prompt(void);
void printf(char *format, unsigned char color, ...);
void serial_printf(const char *format, ...);
void debug_print(const char* function, const char* file, int line, const char* param_format, ...);

#define DEBUG_PRINT(param_format, ...) \
    debug_print(__FUNCTION__, __FILE__, __LINE__, param_format, ##__VA_ARGS__)

#define PRINT_CHAR(c) print_char(c, LIGHT_GRAY)
#define PRINT_STRING(str) print_string(str, LIGHT_GRAY)
#define PRINT_NUMBER(n) print_number(n, LIGHT_GRAY)

#endif