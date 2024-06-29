#include "../includes/common.h"
#include "../includes/vga.h"
#include "../includes/keyboard.h"

#define BUFFER_SIZE 256

void read_line(char *buffer) {

	char key = 0;
	int i = 0;

	while (1) {
		key = read_pressed_key();
		if (key == 0) {
			continue;
		}

		switch (key) {
			case BS:
				if (i > 0) {
					i--;
					backspace();
					buffer[i] = 0;
					continue;
				}
				continue;
			case DEL:
				//PRINT_STRING(" DEL2 ");
				if (i > 0) {
					for (int j = i; j < strlen(buffer); j++) {
						buffer[j] = buffer[j + 1];
					}
					//i--;
					delete();
					continue;
				}
				continue;
			case LEFT_ARROW:
				if (i > 0) {
					vga_index--;
					move_cursor();
					continue;
				}
				continue;;
			case RIGHT_ARROW:
				if (i < strlen(buffer)) {
					i++;
					vga_index++;
					move_cursor();
					continue;
				}
				continue;
			case ENTER:
				buffer[i] = 0;
				print_string("\n", LIGHT_GRAY);
				return;	
		}
		if (i == BUFFER_SIZE - 1) {
			continue;
		}
		print_char(key, LIGHT_GRAY);
		buffer[i] = key;
		i++;
	}
}

void microshell() {

	char buffer[BUFFER_SIZE];
	while (1) {

		prompt();
		memset(buffer, 0, BUFFER_SIZE);
		read_line(buffer);
	}
}