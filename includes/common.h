#ifndef COMMON_H
# define COMMON_H

#include "stddef.h"
#include "stdint.h"

void *memset(void *p, int c, size_t count);
void *memcpy(void *dest, const void *src, size_t n);
int strcmp(const char *str1, const char *str2);
size_t strlen(const char *str);
int to_upper(int c);
int to_lower(int c);
char **ft_split(char const *s, char c);
int	ft_strncmp(const char *s1, const char *s2, size_t n);
void free_array(char **array);
char	*ft_strjoin(char *s1, char *s2);
char	*ft_strdup(const char *s);

#endif