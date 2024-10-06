#include "../includes/common.h"
#include "../includes/vga.h"
#include "../includes/keyboard.h"
#include "../includes/kernel.h"
#include "../includes/memory.h"
#include "../includes/microshell.h"
#include "../includes/ksignal.h"

int buff_idx[3] = {0, 0, 0};

void read_line(char buffer[SCREENS][BUFFER_SIZE])
{

	char key = 0;

	while (1)
	{
		key = read_pressed_key();
		if (key == 0)
		{
			continue;
		}

		switch (key)
		{
		case BS:
			if (buff_idx[screen] > 0)
			{
				buff_idx[screen]--;
				for (int j = buff_idx[screen]; j < strlen(buffer[screen]); j++)
				{
					buffer[screen][j] = buffer[screen][j + 1];
				}
				backspace();
				continue;
			}
			continue;
		case DEL:
			// PRINT_STRING(" DEL2 ");
			if (buff_idx[screen] > 0)
			{
				for (int j = buff_idx[screen]; j < strlen(buffer[screen]); j++)
				{
					buffer[screen][j] = buffer[screen][j + 1];
				}
				// i--;
				delete ();
				continue;
			}
			continue;
		case LEFT_ARROW:
			if (buff_idx[screen] > 0)
			{
				buff_idx[screen]--;
				vga_index[screen]--;
				move_cursor();
				continue;
			}
			continue;
			;
		case RIGHT_ARROW:
			// printf("%d", LIGHT_RED, vga_index % VGA_WIDTH);
			if (buff_idx[screen] < strlen(buffer[screen]))
			{
				buff_idx[screen]++;
				vga_index[screen]++;
				move_cursor();
				continue;
			}
			continue;
		case ENTER:
			//buffer[screen][buff_idx[screen]] = '\0';
			print_string("\n", LIGHT_GRAY);
			return;
		}
		if (buff_idx[screen] == BUFFER_SIZE - 1)
		{
			continue;
		}
		print_char(key, LIGHT_GRAY);
		buffer[screen][buff_idx[screen]] = key;
		buff_idx[screen]++;
	}
}

void test_memory_allocation()
{
	kernel_colored_log(YELLOW, "Starting memory allocation test...\n");
	DEBUG_PRINT("Starting memory allocation test...\n");

	// Test 1: Allocate 4KB (1 page)
	DEBUG_PRINT("Test 1: Allocating 4KB...\n");
	kernel_colored_log(YELLOW, "Test 1: Allocating 4KB...\n");
	void *ptr1 = malloc(4096);
	if (ptr1 == NULL)
	{
		DEBUG_PRINT("Test 1: malloc failed to allocate 4KB.\n");
		KERNEL_PANIC("Test 1: malloc failed to allocate 4KB.\n");
	}

	// Log the size of the allocated block
	uint32_t size1 = get_size(ptr1);
	DEBUG_PRINT("Test 1: Allocated size: %u bytes\n", size1);
	kernel_colored_log(LIGHT_GREEN, "Test 1: Allocated size: %u bytes\n", size1);

	// Write to the memory to check if it's accessible
	*(int *)ptr1 = 42;
	if (*(int *)ptr1 != 42)
	{
		DEBUG_PRINT("Test 1: Memory write/read test failed.\n");
		KERNEL_PANIC("Test 1: Memory write/read test failed.\n");
	}

	DEBUG_PRINT("Test 1: Allocated 4KB and successfully wrote to memory.\n");
	kernel_colored_log(LIGHT_GREEN, "Test 1: Allocated 4KB and successfully wrote to memory.\n");

	// Test 2: Allocate 8KB (2 pages)
	DEBUG_PRINT("Test 2: Allocating 8KB...\n");
	kernel_colored_log(YELLOW, "Test 2: Allocating 8KB...\n");
	void *ptr2 = malloc(8192);
	if (ptr2 == NULL)
	{
		DEBUG_PRINT("Test 2: malloc failed to allocate 8KB.\n");
		KERNEL_PANIC("Test 2: malloc failed to allocate 8KB.\n");
	}

	// Log the size of the allocated block
	uint32_t size2 = get_size(ptr2);
	DEBUG_PRINT("Test 2: Allocated size: %u bytes\n", size2);
	kernel_colored_log(LIGHT_GREEN, "Test 2: Allocated size: %u bytes\n", size2);

	// Write to the memory to check if it's accessible
	int *int_ptr2 = (int *)ptr2;
	for (int i = 0; i < 1024; i++)
	{
		int_ptr2[i] = i; // Write to the entire 8KB region
	}

	for (int i = 0; i < 1024; i++)
	{
		if (int_ptr2[i] != i)
		{
			DEBUG_PRINT("Test 2: Memory write/read test failed at index %d.\n", i);
			KERNEL_PANIC("Test 2: Memory write/read test failed at index %d.\n", i);
		}
	}

	DEBUG_PRINT("Test 2: Allocated 8KB and successfully wrote to memory.\n");
	kernel_colored_log(LIGHT_GREEN, "Test 2: Allocated 8KB and successfully wrote to memory.\n");

	// Free the allocated memory
	free(ptr1);
	free(ptr2);

	DEBUG_PRINT("Memory allocation tests completed successfully.\n");
	kernel_colored_log(GREEN, "Memory allocation tests completed successfully.\n");
}

#define KERNEL_STACK_SIZE_32 4096

extern unsigned char stack_space;

unsigned long get_stack_pointer(void) {
    unsigned long sp;
    asm volatile("mov %%esp, %0" : "=r"(sp));
    return sp;
}

#define MAX_STACK_DEPTH 20


void	hex_to_string(unsigned int addr, char *result, int size)
{
	int	len;
	char	base_str[16] = "0123456789abcdef";

	len = size - 1;
	memset(result, '0', size);
	result[size - 1] = 0;
	while (addr != 0)
	{
		result[--len] = base_str[addr % 16];
		addr = addr / 16;
	}
}

void write_to_memory(uint32_t addr, char *message) {

	char *c = (char *)addr;

    int i;
    for (i = 0; message[i] != '\0' && i < 16; i++) {
        c[i] = message[i];
    }

    // Optionally, fill the rest of the 16-byte line with zeros
    for (; i < 16; i++) {
        c[i] = 0x00;
    }
}

void    kdump(uint32_t addr, int limit)
{
    char *c = (char *)addr;

    char str_addr[9];
    int i;
    uint32_t previous;

    if (limit <= 0)
        return;
    for (i = 0; i < limit; i++)
    {
        if ((i % 16) == 0) // 16 = size line
        {
            if (i != 0)
            {
                previous = addr - 16;
                while (previous < addr)
                {
                    if (*(char *)previous <= 32)
                    {
                        kernel_colored_log(LIGHT_GRAY, "%c", '.');
                    }
                    else
                    {
                        kernel_colored_log(LIGHT_GRAY, "%c", *(char *)previous);
                    }
                    previous++;
                }
                PRINT_CHAR('\n');
            }
            if ((uint32_t)0x00000800 == addr)
			{}
            else
			{}
            kernel_colored_log(LIGHT_BLUE, "%p: ", addr);
        }
        hex_to_string((uint32_t)c[i], str_addr, 3);
        if ((uint32_t)c[i] == 0) // == 00
		{}
        else
		{}
        kernel_colored_log(LIGHT_BLUE, "%s ", str_addr);
        addr++;
    }
    for (i = 0; i < ((limit % 16) * 3); i++) // last line
        PRINT_CHAR(' ');
    if ((limit % 16) == 0)
        previous = addr - 16;
    else
        previous = addr - (limit % 16);
    while (previous < addr)
    {
        if (*(char *)previous <= 32)
            PRINT_CHAR('.');
        else
			PRINT_CHAR(*(char *)previous);
        previous++;
    }
    PRINT_CHAR('\n');
}

void test_arguments(char *buffer) {

	printf("Usage: test_arguments <string1> <string2> ...\n", LIGHT_RED);
			kernel_colored_log(YELLOW, "Starting arguments test...\n");
			kernel_colored_log(YELLOW, "Running ft_split...\n");

			char **args = ft_split(buffer, ' ');
			if (args == NULL)
			{
				kernel_colored_log(RED, "ft_split failed\n");
				return;
			}
			kernel_colored_log(GREEN, "ft_split successful\n");
			kernel_colored_log(YELLOW, "Printing arguments...\n");
			for (int i = 1; args[i] != NULL; i++)
			{
				printf("Argument %d: %s\n", LIGHT_BLUE, i, args[i]);
			}

			int size = 0;
			while (args[size] != NULL)
			{
				size++;
			}

			if (size > 2)
			{
				kernel_colored_log(YELLOW, "Running strjoin...\n");

				char *str = ft_strdup(args[1]);
				for (int i = 2; args[i] != NULL; i++)
				{
					str = ft_strjoin(str, args[i]);
					if (str == NULL)
					{
						kernel_colored_log(RED, "strjoin failed\n");
						continue;
					}
					kernel_colored_log(GREEN, "strjoin successful\n");
					kernel_colored_log(LIGHT_BLUE, "Joined string: %s\n", str);
				}
				free(str);
				kernel_colored_log(LIGHT_MAGENTA, "Freeing arguments...\n");
				for (int x = 0; args[x] != NULL; x++)
				{
					kernel_colored_log(LIGHT_RED, "Freeing argument %d: %s\n", x, args[x]);
					free(args[x]);
				}
				kernel_colored_log(LIGHT_MAGENTA, "Freeing array...\n");
				free(args);
				kernel_colored_log(GREEN, "Arguments test completed successfully.\n");
			} else{
				kernel_colored_log(RED, "Not enough arguments to run strjoin\n");
				kernel_colored_log(RED, "Usage: test_arguments <string1> <string2> ...\n");
				free_array(args);
			}
}

// Returns a pointer to the first occurrence of 'c' in 'str', or NULL if not found.
char* find_char(char *str, char c) {
    while (*str) {
        if (*str == c)
            return str;
        str++;
    }
    return NULL;
}

// Trims leading and trailing spaces from 'str'.
// Modifies the string in place and returns a pointer to the trimmed string.
// Ensure trim_spaces accepts a char * (already the case)
char* trim_spaces(char *str) {
    char *end;

    // Trim leading spaces
    while (*str == ' ' || *str == '\t') str++;

    if (*str == 0)  // All spaces
        return str;

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
        end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}


// Converts a hexadecimal string to a uint32_t.
// Returns 0 on success, -1 on failure.
int hex_to_uint32(const char *hex_str, uint32_t *value) {
    uint32_t result = 0;
    int i = 0;

    // Check for "0x" or "0X" prefix
    if (hex_str[0] == '0' && (hex_str[1] == 'x' || hex_str[1] == 'X')) {
        i = 2;
    }

    for (; hex_str[i] != '\0'; i++) {
        char c = hex_str[i];
        uint32_t digit;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        }
        else if (c >= 'a' && c <= 'f') {
            digit = 10 + (c - 'a');
        }
        else if (c >= 'A' && c <= 'F') {
            digit = 10 + (c - 'A');
        }
        else {
            // Invalid character for hex
            return -1;
        }

        result = (result << 4) | digit;
    }

    *value = result;
    return 0;
}

int parse_and_write(char *buffer, size_t buffer_length) {
    char local_buffer[buffer_length + 1];
    char *colon_ptr;
    char *address_str;
    char *message_str;
    uint32_t address;

    // Copy buffer to local_buffer to ensure null-termination
    if (buffer_length >= sizeof(local_buffer))
        return -1; // Buffer too large

    for (int i = 0; i < buffer_length; i++) {
        local_buffer[i] = buffer[i];
    }
    local_buffer[buffer_length] = '\0';

    // Find the colon ':' separator
    colon_ptr = find_char(local_buffer, ':');
    if (!colon_ptr)
        return -1; // Invalid format

    // Split the string into address and message
    *colon_ptr = '\0';
    address_str = trim_spaces(local_buffer);
    message_str = trim_spaces(colon_ptr + 1);

    // Convert address string to uint32_t
    if (hex_to_uint32(address_str, &address) != 0)
        return -1; // Invalid address format

    // Write the message to the specified memory address
    write_to_memory(address, message_str);

    return 0;
}

int split_and_write(char *buffer) {

	if (strlen(buffer) < 16) {
		DEBUG_PRINT("Invalid buffer length\n");
		kernel_colored_log(RED, "Invalid buffer length\n");
		return -1;
	}

	char **args = ft_split(buffer + 16, ':');

	uint32_t address;

	if (args[0] == NULL || args[1] == NULL)
		return -1;

	if (hex_to_uint32(args[0], &address) != 0) {
		DEBUG_PRINT("Invalid address format %s\n", args[0]);
		kernel_colored_log(RED, "Invalid address format\n");
		free_array(args);
		return -1;
	}

	DEBUG_PRINT("-------------------Writing to memory: %p: %s\n", address, args[1]);
	write_to_memory(address, args[1]);
	free_array(args);
	return 0;
}

void microshell()
{

	//print_symbols();
	char buffer[SCREENS][BUFFER_SIZE];

	for (int x = 0; x < SCREENS; x++)
	{
		memset(buffer[x], 0, BUFFER_SIZE);
	}

	int i = 0;
	while (1)
	{

		if (i)
			prompt();
		buff_idx[screen] = 0;
		read_line(buffer);
		buffer[screen][strlen(buffer[screen])] = 0;

		DEBUG_PRINT("Command entered in screen %d: %s\n", screen, buffer[screen]);

		if (strcmp(buffer[screen], "exit") == 0)
		{
			break;
		}
		else if (strcmp(buffer[screen], "clear") == 0)
		{
			clear_screen();
		}
		else if (strcmp(buffer[screen], "test_memory_allocation") == 0)
		{
			test_memory_allocation();
		}
		else if (strcmp(buffer[screen], "kstack") == 0)
		{
			//kdump(STACK_BASE, STACK_TOP - STACK_BASE);
			kdump(STACK_BASE, 150);
		}
		else if (strcmp(buffer[screen], "help") == 0)
		{
			printf("Commands:\n", BLUE);
			printf("clear: Clear the screen\n", LIGHT_CYAN);
			printf("exit: Exit the shell\n", LIGHT_CYAN);
			printf("help: Display this help message\n", LIGHT_CYAN);
			printf("kstack: Print the kernel stack trace\n\n", LIGHT_CYAN);
			printf("test_memory_allocation: Run memory allocation tests\n", LIGHT_MAGENTA);
			printf("test_arguments: Run ft_split and strjoin test\n", LIGHT_MAGENTA);
			printf("write_to_memory ADDRESS: message -> write_to_memory 0x10B000: Hello world!\n", LIGHT_MAGENTA);
			printf("crash: crash the system to test out kernel panic\n", LIGHT_MAGENTA);
			printf("\nShortcuts:\n", BLUE);
			printf("F1: Change to screen 1\n", LIGHT_CYAN);
			printf("F2: Change to screen 2\n", LIGHT_CYAN);
			printf("F3: Change to screen 3\n", LIGHT_CYAN);
			printf("CTRL + Shift + D: Toggle debug mode\n", LIGHT_CYAN);
		}
		else if (ft_strncmp(buffer[screen], "write_to_memory", 14) == 0) {
			DEBUG_PRINT("write_to_memory: %s\n", buffer[screen]);
			split_and_write(buffer[screen]);
		}
		else if (ft_strncmp(buffer[screen], "test_arguments", 14) == 0)
		{
			test_arguments(buffer[screen]);
		}
		else if (strcmp(buffer[screen], "crash") == 0) {
			char *test = malloc(4);
			
			free(test);
			
			for (int crash = 0; crash < 10; crash++) {
				test[crash] = 'X';
				free(test);
			}
		}
		else if (strcmp(buffer[screen], "sigint") == 0)
			trigger_signal(SIG_INTERRUPT);
		else
		{
			printf("Command not found: %s\n", LIGHT_RED, buffer[screen]);
		}
		i = 1;
		memset(buffer[screen], 0, BUFFER_SIZE);
		scheduler_tick();
	}
}

