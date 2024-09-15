#ifndef MICROSHELL_H
# define MICROSHELL_H

#define BUFFER_SIZE 256

#include "symbol.h"
#include "common.h"
#include "vga.h"
#include "keyboard.h"
#include "kernel.h"
#include "memory.h"
#include "microshell.h"

void microshell();
void read_line(char buffer[SCREENS][BUFFER_SIZE]);
void test_memory_allocation();
void print_kernel_stack(uint32_t addr, int limit);
void test_arguments(char *buffer);

#endif