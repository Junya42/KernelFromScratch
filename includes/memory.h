#ifndef MEMORY_H
# define MEMORY_H

#include "stdint.h"

void *malloc(uint32_t size);
void free(void *ptr);
uint32_t get_size(void *ptr);

#endif