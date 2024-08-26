#ifndef KERNEL_H
# define KERNEL_H

void kernel_log(const char *format, ...);
void kernel_panic(const char *message);
void kernel_panic_with_log(char *format, ...);

#endif