#include "../includes/common.h"

void *memset(void *p, int c, size_t count)
{
    for(size_t i = 0; i < count; i++)
    {
        ((char *)p)[i] = c;
    }
    return p;
}

void *memcpy(void *dest, const void *src, size_t n) {

    char *cdest = dest;
    const char *csrc = src;

    size_t i;
    for(i = 0; i < n; i++) {
        cdest[i] = csrc[i];
    }

    return dest;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}


size_t strlen(const char *str) {
    const char *cstr;

    if (str == NULL) return 0;

    // Handle unaligned start
    for (cstr = str; ((uint32_t)cstr & 3) != 0; cstr++) {
        if (*cstr == '\0') {
            return cstr - str;
        }
    }

    size_t len = cstr - str;
    uint32_t n;

    // Main loop handling 4 bytes at a time
    while (1) {
        n = *((uint32_t *)&str[len]);

        if ((n - 0x01010101UL) & (~n) & 0x80808080UL) {
            break;
        }

        len += 4;
    }

    // Handle remaining bytes
    str = &str[len];
    if (str[0] == '\0') return len;
    if (str[1] == '\0') return len + 1;
    if (str[2] == '\0') return len + 2;
    if (str[3] == '\0') return len + 3;

    // Unreachable
    return (size_t)-1;
}


int to_upper(int c) {
	if (c >= 'a' && c <= 'z') {
		return c - 32;
	}
	return c;
}

int to_lower(int c) {
	if (c >= 'A' && c <= 'Z') {
		return c + 32;
	}
	return c;
}