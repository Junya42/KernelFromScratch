#include "../includes/common.h"
#include "../includes/memory.h"

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


int	ft_haystack(char s, char c)
{
	if (s == c)
		return (1);
	return (0);
}

int	ft_get_size(char const *s, char c)
{
	int	size;

	size = 0;
	if (!s)
		return (0);
	while (*s)
	{
		if (!ft_haystack(*s, c))
		{
			size++;
			while (*s && !ft_haystack(*s, c))
				s++;
		}
		else
			s++;
	}
	return (size);
}

void	ft_memorycpy(char *memory, char const *s, char *res)
{
	while (memory < s)
		*(res++) = *(memory++);
	*res = '\0';
}

char	**ft_secure(char const *s, char c, char **res, char *memory)
{
	int	i;

	i = 0;
	while (*s)
	{
		if (!ft_haystack(*s, c))
		{
			memory = (char *)s;
			while (*s && !ft_haystack(*s, c))
				s++;
			res[i] = (char *)malloc(sizeof(char) * (s - memory + 1));
			if (!res[i]) {
                i--;
                while (i >= 0) {
                    free(res[i]);
                    i--;
                }
                free(res);
				return (NULL);
            }
			ft_memorycpy(memory, s, res[i]);
			i++;
		}
		else
			s++;
	}
	res[i] = 0;
	return (res);
}

char	**ft_split(char const *s, char c)
{
	char	**res;
	char	*memory;

	memory = (char *)s;
	res = (char **)malloc(sizeof(char *) * (ft_get_size(s, c) + 1));
	if (!res || !s)
		return (NULL);
	return (ft_secure(s, c, res, memory));
}

void free_array(char **array) {
    if (array == NULL) return;

    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
    }

    free(array);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (NULL);
	str = malloc(sizeof(char) * (strlen(s1) + strlen(s2)));
	if (str == NULL)
		return (NULL);
	while (i < strlen(s1))
	{
		str[i] = s1[i];
		i++;
	}
	while (j < strlen(s2))
	{
		str[i + j] = s2[j];
		j++;
	}
	str[i + j] = '\0';
	free(s1);
	return (str);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (!n)
		return (0);
	while ((s1[i] == s2[i] && (s1[i] && s2[i])) && i < n - 1)
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
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

char	*ft_strdup(const char *s)
{
	char	*str;
	int		i;

	i = -1;
	str = (char *)malloc(sizeof(char) * (strlen(s) + 1));
	if (str == NULL)
		return (NULL);
	while (s[++i])
		str[i] = s[i];
	str[i] = '\0';
	return (str);
}