#include "../includes/common.h"
#include "../includes/vga.h"
#include "../includes/keyboard.h"
#include "../includes/kernel.h"
#include "../includes/memory.h"

#define BUFFER_SIZE 256

int buff_idx[3] = {0, 0, 0};

void read_line(char buffer[SCREENS][BUFFER_SIZE]) {

	char key = 0;

	while (1) {
		key = read_pressed_key();
		if (key == 0) {
			continue;
		}

		switch (key) {
			case BS:
				if (buff_idx[screen] > 0) {
					buff_idx[screen]--;
					for (int j = buff_idx[screen]; j < strlen(buffer[screen]); j++) {
						buffer[screen][j] = buffer[screen][j + 1];
					}
					backspace();
					continue;
				}
				continue;
			case DEL:
				//PRINT_STRING(" DEL2 ");
				if (buff_idx[screen] > 0) {
					for (int j = buff_idx[screen]; j < strlen(buffer[screen]); j++) {
						buffer[screen][j] = buffer[screen][j + 1];
					}
					//i--;
					delete();
					continue;
				}
				continue;
			case LEFT_ARROW:
				if (buff_idx[screen] > 0) {
					buff_idx[screen]--;
					vga_index[screen]--;
					move_cursor();
					continue;
				}
				continue;;
			case RIGHT_ARROW:
				//printf("%d", LIGHT_RED, vga_index % VGA_WIDTH);
				if (buff_idx[screen] < strlen(buffer[screen])) {
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
		if (buff_idx[screen] == BUFFER_SIZE - 1) {
			continue;
		}
		print_char(key, LIGHT_GRAY);
		buffer[screen][buff_idx[screen]] = key;
		buff_idx[screen]++;
	}
}

void test_memory_allocation() {
    kernel_log("Starting memory allocation test...\n");

    // Test 1: Allocate 4KB (1 page)
    void *ptr1 = malloc(4096);
    if (ptr1 == NULL) {
        kernel_panic("Test 1: malloc failed to allocate 4KB.\n");
    }

    // Log the size of the allocated block
    uint32_t size1 = get_size(ptr1);
    kernel_log("Test 1: Allocated size: %u bytes\n", size1);

    // Write to the memory to check if it's accessible
    *(int *)ptr1 = 42;
    if (*(int *)ptr1 != 42) {
        kernel_panic("Test 1: Memory write/read test failed.\n");
    }

    kernel_log("Test 1: Allocated 4KB and successfully wrote to memory.\n");

    // Test 2: Allocate 8KB (2 pages)
    void *ptr2 = malloc(8192);
    if (ptr2 == NULL) {
        kernel_panic("Test 2: malloc failed to allocate 8KB.\n");
    }

    // Log the size of the allocated block
    uint32_t size2 = get_size(ptr2);
    kernel_log("Test 2: Allocated size: %u bytes\n", size2);

    // Write to the memory to check if it's accessible
    int *int_ptr2 = (int *)ptr2;
    for (int i = 0; i < 1024; i++) {
        int_ptr2[i] = i;  // Write to the entire 8KB region
    }

    for (int i = 0; i < 1024; i++) {
        if (int_ptr2[i] != i) {
            kernel_panic_with_log("Test 2: Memory write/read test failed at index %d.\n", i);
        }
    }

    kernel_log("Test 2: Allocated 8KB and successfully wrote to memory.\n");

    // Free the allocated memory
    free(ptr1);
    free(ptr2);

    kernel_log("Memory allocation tests passed.\n");
}



void microshell() {

	char buffer[SCREENS][BUFFER_SIZE];

	for (int x = 0; x < SCREENS; x++) {
		memset(buffer[x], 0, BUFFER_SIZE);
	}

	int i = 0;
	while (1) {

		if (i)
			prompt();
		buff_idx[screen] = 0;
		read_line(buffer);

		DEBUG_PRINT("Command entered in screen %d: %s\n", screen, buffer[screen]);

		if (strcmp(buffer[screen], "exit") == 0) {
			break;
		} else if (strcmp(buffer[screen], "clear") == 0) {
			clear_screen();
		} else if (strcmp(buffer[screen], "test_memory_allocation") == 0) {
			test_memory_allocation();
		} else if (strcmp(buffer[screen], "help") == 0) {
			printf("Commands:\n", BLUE);
			printf("clear: Clear the screen\n", LIGHT_CYAN);
			printf("exit: Exit the shell\n", LIGHT_CYAN);
			printf("help: Display this help message\n", LIGHT_CYAN);
			printf("\nShortcuts:\n", BLUE);
			printf("F1: Change to screen 1\n", LIGHT_CYAN);
			printf("F2: Change to screen 2\n", LIGHT_CYAN);
			printf("F3: Change to screen 3\n", LIGHT_CYAN);
			printf("CTRL + Shift + D: Toggle debug mode\n", LIGHT_CYAN);
		} else {
			printf("Command not found: %s\n", LIGHT_RED, buffer[screen]);
		}
		i = 1;
	}
}