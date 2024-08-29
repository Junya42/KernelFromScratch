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

typedef struct s_symbol {
	void *address;
	const char *name;
} t_symbol;

t_list *kernel_symbols;

t_symbol *create_symbol(void *address, const char *name);
const char *lookup_symbol(void *address);
void init_symbols();

#endif