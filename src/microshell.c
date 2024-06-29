#include "../includes/common.h"
#include "../includes/vga.h"
#include "../includes/keyboard.h"

#define BUFFER_SIZE 256

void read_line(char **buffer) {

	char key = 0;
	int i[3] = {0, 0, 0};

	while (1) {
		key = read_pressed_key();
		if (key == 0) {
			continue;
		}

		switch (key) {
			case BS:
				if (i[screen] > 0) {
					i[screen]--;
					for (int j = i; j < strlen(buffer[screen]); j++) {
						buffer[screen][j] = buffer[screen][j + 1];
					}
					backspace();
					continue;
				}
				continue;
			case DEL:
				//PRINT_STRING(" DEL2 ");
				if (i[screen] > 0) {
					for (int j = i[screen]; j < strlen(buffer[screen]); j++) {
						buffer[screen][j] = buffer[screen][j + 1];
					}
					//i--;
					delete();
					continue;
				}
				continue;
			case LEFT_ARROW:
				if (i[screen] > 0) {
					i[screen]--;
					vga_index[screen]--;
					move_cursor();
					continue;
				}
				continue;;
			case RIGHT_ARROW:
				//printf("%d", LIGHT_RED, vga_index % VGA_WIDTH);
				if (i[screen] < strlen(buffer[screen])) {
					i[screen]++;
					vga_index[screen]++;
					move_cursor();
					continue;
				}
				continue;
			case ENTER:
				//buffer[i] = 0;
				print_string("\n", LIGHT_GRAY);
				return;	
		}
		if (i == BUFFER_SIZE - 1) {
			continue;
		}
		print_char(key, LIGHT_GRAY);
		buffer[screen][i[screen]] = key;
		i[screen]++;
	}
}

void microshell() {

	char buffer[SCREENS][BUFFER_SIZE];

	int i = 0;
	while (1) {

		if (i)
			prompt();
		memset(buffer[screen], 0, BUFFER_SIZE);
		read_line(buffer);

		if (strcmp(buffer[screen], "exit") == 0) {
			break;
		} else if (strcmp(buffer[screen], "clear") == 0) {
			clear_screen();
		} else if (strcmp(buffer[screen], "help") == 0) {
			printf("Commands:\n", BLUE);
			printf("exit: Exit the shell\n", LIGHT_CYAN);
			printf("clear: Clear the screen\n", LIGHT_CYAN);
			printf("help: Display this help message\n", LIGHT_CYAN);
		} else {
			printf("Command not found: %s\n", LIGHT_RED, buffer[screen]);
		}
		i = 1;
	}
}