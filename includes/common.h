#ifndef COMMON_H
# define COMMON_H

#include "stddef.h"
#include "stdint.h"

void *memset(void *p, int c, size_t count);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *str);
int to_upper(int c);
int to_lower(int c);

#endif