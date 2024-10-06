#ifndef KERNEL_H
# define KERNEL_H

# include "microshell.h"

void kernel_log(char *format, ...);
void kernel_colored_log(unsigned char color, char *format, ...);
void kernel_panic(const char *function, const char *file, int line, char *format, ...);

#define DUMP_STACK() \
	kdump(STACK_BASE, 200);

#define KERNEL_PANIC(format, ...) \
	DUMP_STACK(); \
	kernel_panic(__FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__);

#endif
