#ifndef SYMBOL_H
# define SYMBOL_H

#include "stdint.h"
#include "chained_list.h"
#include "stddef.h"
#include "common.h"
#include "gdt.h"
#include "idt.h"
#include "interrupts.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "microshell.h"
#include "multiboot.h"
#include "paging.h"
#include "terminal.h"
#include "vga.h"
#include "kernel.h"

#define STACK_BASE 0x10B000
#define STACK_TOP 0x10C000

typedef struct s_symbol {
	void *address;
	unsigned long stack_address;
	const char *name;
} t_symbol;

t_list *kernel_symbols;

t_symbol *create_symbol(void *address, const char *name);
const char *lookup_symbol(unsigned long stack_address);
void init_symbols();
void print_symbols();

#endif