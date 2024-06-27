#ifndef SCREEN_H
# define SCREEN_H

#define VGA_ADDRESS 0xB8000

#define ROWS 25
#define COLUMNS 80

#define VGA_BUFFER_SIZE (ROWS * COLUMNS)

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


void clear_screen(void);
void print_string(char *str, unsigned char color);
void print_char(char c, unsigned char color);
void print_number(int n, unsigned char color);
void prompt(void);

#define PRINT_CHAR(c) print_char(c, LIGHT_GRAY)
#define PRINT_STRING(str) print_string(str, LIGHT_GRAY)
#define PRINT_NUMBER(n) print_number(n, LIGHT_GRAY)

#endif