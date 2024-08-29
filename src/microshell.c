#include "../includes/common.h"
#include "../includes/vga.h"
#include "../includes/keyboard.h"
#include "../includes/kernel.h"
#include "../includes/memory.h"
#include "../includes/microshell.h"

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
			buffer[screen][buff_idx[screen]] = '\0';
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

	DEBUG_PRINT("Memory allocation tests passed.\n");
	kernel_colored_log(GREEN, "Memory allocation tests passed.\n");
}

#define MAX_STACK_DEPTH 20

void print_kernel_stack(void) {
    uint32_t *ebp;
    uint32_t *eip;
    int frame;

    // Get the current base pointer
    asm volatile ("mov %%ebp, %0" : "=r" (ebp));

    printf("Kernel stack trace:\n", LIGHT_MAGENTA);

    for (frame = 0; frame < MAX_STACK_DEPTH && ebp != 0; frame++) {
        // The return address is typically stored at ebp+1
        eip = (uint32_t *)(*(ebp + 1));
        
        // Use your lookup_symbol function to get the function name
        const char* func_name = lookup_symbol((void*)eip);
        
        if (func_name) {
            printf("[%08x] EIP: 0x%x (%s), EBP: 0x%x\n", LIGHT_BLUE, frame, (unsigned int)eip, func_name, (unsigned int)ebp);
        } else {
            printf("[%08x] EIP: 0x%x (Unknown), EBP: 0x%x\n", LIGHT_GRAY, frame, (unsigned int)eip, (unsigned int)ebp);
        }

        // Move to the next stack frame
        ebp = (uint32_t *)(*ebp);
    }
}

void print_user_stack(void) {
    uint32_t *ebp;
    uint32_t *eip;
    int frame;

    // Get the current base pointer
    asm volatile ("mov %%ebp, %0" : "=r" (ebp));

    printf("User stack trace:\n", LIGHT_MAGENTA);

    for (frame = 0; frame < MAX_STACK_DEPTH && ebp != 0; frame++) {
        // The return address is typically stored at ebp+1
        eip = (uint32_t *)(*(ebp + 1));
        
        // Use your lookup_symbol function to get the function name
        const char* func_name = lookup_symbol((void*)eip);
        
        if (func_name) {
            printf("[%08x] EIP: 0x%x (%s), EBP: 0x%x\n", LIGHT_BLUE, frame, (unsigned int)eip, func_name, (unsigned int)ebp);
        } else {
            printf("[%08x] EIP: 0x%x (Unknown), EBP: 0x%x\n", LIGHT_GRAY, frame, (unsigned int)eip, (unsigned int)ebp);
        }

        // Move to the next stack frame
        ebp = (uint32_t *)(*ebp);
    }
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

void microshell()
{

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
			print_kernel_stack();
		}
		else if (strcmp(buffer[screen], "ustack") == 0)
		{
			print_user_stack();
		}
		else if (strcmp(buffer[screen], "help") == 0)
		{
			printf("Commands:\n", BLUE);
			printf("clear: Clear the screen\n", LIGHT_CYAN);
			printf("exit: Exit the shell\n", LIGHT_CYAN);
			printf("help: Display this help message\n", LIGHT_CYAN);
			printf("kstack: Print the kernel stack trace\n", LIGHT_CYAN);
			printf("ustack: Print the user stack trace\n", LIGHT_CYAN);
			printf("test_memory_allocation: Run memory allocation tests\n", LIGHT_MAGENTA);
			printf("test_arguments: Run ft_split and strjoin test\n", LIGHT_MAGENTA);
			printf("\nShortcuts:\n", BLUE);
			printf("F1: Change to screen 1\n", LIGHT_CYAN);
			printf("F2: Change to screen 2\n", LIGHT_CYAN);
			printf("F3: Change to screen 3\n", LIGHT_CYAN);
			printf("CTRL + Shift + D: Toggle debug mode\n", LIGHT_CYAN);
		}
		else if (ft_strncmp(buffer[screen], "test_arguments", 14) == 0)
		{
			test_arguments(buffer[screen]);
		}
		else
		{
			printf("Command not found: %s\n", LIGHT_RED, buffer[screen]);
		}
		i = 1;
	}
}