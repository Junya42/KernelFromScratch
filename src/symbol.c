#include "../includes/symbol.h"

t_symbol *create_symbol(void *address, const char *name)
{
	t_symbol *symbol = malloc(sizeof(t_symbol));
	if (!symbol)
		return NULL;

	symbol->address = address;
	symbol->stack_address = STACK_BASE + ((unsigned long)address % (STACK_TOP - STACK_BASE));
	symbol->name = name;

	return symbol;
}

const char *lookup_symbol(unsigned long stack_address) {
	t_list *current = kernel_symbols;
	while (current) {
		t_symbol *symbol = (t_symbol *)current->content;
		if (symbol->stack_address == stack_address) {
			return symbol->name;
		}
		current = current->next;
	}
	return NULL;
}

void init_symbols() {

	kernel_symbols = create_list(create_symbol(&memset, "memset"));
	listadd_back(kernel_symbols, create_symbol(&memcpy, "memcpy"));
	listadd_back(kernel_symbols, create_symbol(&strcmp, "strcmp"));
	listadd_back(kernel_symbols, create_symbol(&strlen, "strlen"));
	listadd_back(kernel_symbols, create_symbol(&to_upper, "to_upper"));
	listadd_back(kernel_symbols, create_symbol(&to_lower, "to_lower"));
	listadd_back(kernel_symbols, create_symbol(&ft_split, "ft_split"));
	listadd_back(kernel_symbols, create_symbol(&ft_strncmp, "ft_strncmp"));
	listadd_back(kernel_symbols, create_symbol(&free_array, "free_array"));
	listadd_back(kernel_symbols, create_symbol(&ft_strjoin, "ft_strjoin"));
	listadd_back(kernel_symbols, create_symbol(&ft_strdup, "ft_strdup"));
	listadd_back(kernel_symbols, create_symbol(&kernel_log, "kernel_log"));
	listadd_back(kernel_symbols, create_symbol(&kernel_colored_log, "kernel_colored_log"));
	listadd_back(kernel_symbols, create_symbol(&handle_key_input, "handle_key_input"));
	listadd_back(kernel_symbols, create_symbol(&read_pressed_key, "read_pressed_key"));
	listadd_back(kernel_symbols, create_symbol(&init_keyboard, "init_keyboard"));
	listadd_back(kernel_symbols, create_symbol(&malloc, "malloc"));
	listadd_back(kernel_symbols, create_symbol(&free, "free"));
	listadd_back(kernel_symbols, create_symbol(&get_size, "get_size"));
	listadd_back(kernel_symbols, create_symbol(&allocate_physical_page, "allocate_physical_page"));
	listadd_back(kernel_symbols, create_symbol(&free_physical_page, "free_physical_page"));
	listadd_back(kernel_symbols, create_symbol(&allocate_virtual_page, "allocate_virtual_page"));
	listadd_back(kernel_symbols, create_symbol(&get_physical_address, "get_physical_address"));
	listadd_back(kernel_symbols, create_symbol(&set_page, "set_page"));
	listadd_back(kernel_symbols, create_symbol(&unmap_page, "unmap_page"));
	listadd_back(kernel_symbols, create_symbol(&title_screen, "title_screen"));
	listadd_back(kernel_symbols, create_symbol(&init_terminal, "init_terminal"));
	listadd_back(kernel_symbols, create_symbol(&toggle_debug, "toggle_debug"));
	listadd_back(kernel_symbols, create_symbol(&init_screens, "init_screens"));
	listadd_back(kernel_symbols, create_symbol(&clear_screen, "clear_screen"));
	listadd_back(kernel_symbols, create_symbol(&backspace, "backspace"));
	listadd_back(kernel_symbols, create_symbol(&delete, "delete"));
	listadd_back(kernel_symbols, create_symbol(&print_string, "print_string"));
	listadd_back(kernel_symbols, create_symbol(&print_char, "print_char"));
	listadd_back(kernel_symbols, create_symbol(&print_number, "print_number"));
	listadd_back(kernel_symbols, create_symbol(&change_screen, "change_screen"));
	listadd_back(kernel_symbols, create_symbol(&move_cursor, "move_cursor"));
	listadd_back(kernel_symbols, create_symbol(&prompt, "prompt"));
	listadd_back(kernel_symbols, create_symbol(&vprintf, "vprintf"));
	listadd_back(kernel_symbols, create_symbol(&printf, "printf"));
	listadd_back(kernel_symbols, create_symbol(&serial_printf, "serial_printf"));
	listadd_back(kernel_symbols, create_symbol(&debug_print, "debug_print"));
	listadd_back(kernel_symbols, create_symbol(&microshell, "microshell"));
	listadd_back(kernel_symbols, create_symbol(&read_line, "read_line"));
	listadd_back(kernel_symbols, create_symbol(&test_memory_allocation, "test_memory_allocation"));
	listadd_back(kernel_symbols, create_symbol(&kdump, "kdump"));
	listadd_back(kernel_symbols, create_symbol(&test_arguments, "test_arguments"));
	listadd_back(kernel_symbols, create_symbol(&lookup_symbol, "lookup_symbol"));
}


void print_symbols() {

	t_list *current = kernel_symbols;
	while (current) {
		t_symbol *symbol = (t_symbol *)current->content;
		serial_printf("Symbol: %s -> %p\n", symbol->name, symbol->address);
		current = current->next;
	}
}