#ifndef TERMINAL_H
# define TERMINAL_H

#include "vga.h"
#include "stdint.h"

extern unsigned short current_term;

void title_screen(uint8_t screen);

#endif