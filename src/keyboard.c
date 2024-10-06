#include "../includes/stdint.h"
#include "../includes/keyboard.h"
#include "../includes/vga.h"
#include "../includes/interrupts.h"
#include "../includes/io.h"
#include "../includes/common.h"
#include "../includes/terminal.h"
#include "../includes/ksignal.h"

uint8_t shift;
uint8_t ctrl;
uint8_t pressedkeys[256];
uint8_t lastpressedkey = 0;


// Scancode -> ASCII
const uint8_t lower_ascii_codes[256] = {
    0x00,  0x00,  '1',  '2',     /* 0x00 */
     '3',  '4',  '5',  '6',     /* 0x04 */
     '7',  '8',  '9',  '0',     /* 0x08 */
     '-',  '=',   0x00, 0x00,     /* 0x0C */
     'q',  'w',  'e',  'r',     /* 0x10 */
     't',  'y',  'u',  'i',     /* 0x14 */
     'o',  'p',  '[',  ']',     /* 0x18 */
    '\n', 0x00,  'a',  's',     /* 0x1C */
     'd',  'f',  'g',  'h',     /* 0x20 */
     'j',  'k',  'l',  ';',     /* 0x24 */
    '\'',  '`', 0x00, '\\',     /* 0x28 */
     'z',  'x',  'c',  'v',     /* 0x2C */
     'b',  'n',  'm',  ',',     /* 0x30 */
     '.',  '/', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '-',  '4',     /* 0x48 */
     '5',  '6',  '+',  '1',     /* 0x4C */
     '2',  '3',  '0',  '.',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

// Scancode -> ASCII
const uint8_t upper_ascii_codes[256] = {
    0x00,  0x00,  '!',  '@',     /* 0x00 */
     '#',  '$',  '%',  '^',     /* 0x04 */
     '&',  '*',  '(',  ')',     /* 0x08 */
     '_',  '+',   0x00, '\t',     /* 0x0C */
     'Q',  'W',  'E',  'R',     /* 0x10 */
     'T',  'Y',  'U',  'I',     /* 0x14 */
     'O',  'P',  '{',  '}',     /* 0x18 */
    '\n', 0x00,  'A',  'S',     /* 0x1C */
     'D',  'F',  'G',  'H',     /* 0x20 */
     'J',  'K',  'L',  ':',     /* 0x24 */
     '"',  '~', 0x00,  '|',     /* 0x28 */
     'Z',  'X',  'C',  'V',     /* 0x2C */
     'B',  'N',  'M',  '<',     /* 0x30 */
     '>',  '?', 0x00,  '*',     /* 0x34 */
    0x00,  ' ', 0x00, 0x00,     /* 0x38 */
    0x00, 0x00, 0x00, 0x00,     /* 0x3C */
    0x00, 0x00, 0x00, 0x00,     /* 0x40 */
    0x00, 0x00, 0x00,  '7',     /* 0x44 */
     '8',  '9',  '-',  '4',     /* 0x48 */
     '5',  '6',  '+',  '1',     /* 0x4C */
     '2',  '3',  '0',  '.',     /* 0x50 */
    0x00, 0x00, 0x00, 0x00,     /* 0x54 */
    0x00, 0x00, 0x00, 0x00      /* 0x58 */
};

uint8_t ctrl_macros(uint8_t scancode) {

	switch (scancode) {
		case 0x20: /* ctrl + d */
			break;
		default:
			break;
	}
	return 0;
}

uint8_t ctrl_shift_macros(uint8_t scancode) {

	switch (scancode) {
		case 0x20: /* ctrl + shift + d */
			toggle_debug();
			break;
		default:
			break;
	}
	return 0;
}

char read_pressed_key() {
	char key = lastpressedkey;

	lastpressedkey = 0;
	return key;
}

int is_defined_key(uint8_t scancode) {

	// replace the if with a switch statement and printf the corresponding key name
	switch (scancode) {
		case ESC:
			//PRINT_STRING(" ESC ");
			break;
		case BS:
			//PRINT_STRING(" BS ");
			break;
		case EOT:
			//PRINT_STRING(" EOT ");
			break;
		case DEL:
			//PRINT_STRING(" DEL ");
			break;
		case TAB:
			//PRINT_STRING(" TAB ");
			return '\n';
			break;
		case SHIFT:
			//PRINT_STRING(" SHIFT ");
			break;
		case CAPSLOCK:
			//PRINT_STRING(" CAPSLOCK ");
			break;
		case CTRL:
			//PRINT_STRING(" CTRL ");
			break;
		case ALT:
			//PRINT_STRING(" ALT ");
			break;
		case ENTER:
			//PRINT_STRING(" ENTER " );
			break;
		case LEFT_ARROW:
			//PRINT_STRING(" LEFT_ARROW ");
			break;
		case RIGHT_ARROW:
			//PRINT_STRING(" RIGHT_ARROW ");
			break;
		default:
			return 0;
	}
	return scancode;
}

char handle_key_input() { // Needs to be static ?

	uint8_t scancode = inb(0x60);

	if (scancode == 0)
		return 0;

	/* Key released */
	if (scancode & 0x80) {

		uint8_t released_key = scancode & 0x7F;

		switch (released_key) {
			case 0x2A:
				shift = 0;
				break;
			case 0x1D:
				ctrl = 0;
				break;
		}

		pressedkeys[released_key] = 0;
		return 0;
	}

	/* Key pressed */
	if (pressedkeys[scancode] > 0 && pressedkeys[scancode] < 10) { /* if key is already pressed */

		pressedkeys[scancode]++;
		return 0;
	}
	pressedkeys[scancode]++;
	outb(0x60, 0);

	//printf("scancode %d -> ", YELLOW, scancode);
	switch (scancode) {
		case SHIFT:
			shift = 1;
			break;
		case CTRL:
			ctrl = 1;
			break;
		case CAPSLOCK:
			shift = shift ? 0 : 1;
			break;
		case F1:
			change_screen(0);
			break;
		case F2:
			change_screen(1);
			break;
		case F3:
			change_screen(2);
			break;
		case 0x2E:
			if (ctrl) {
				trigger_signal(SIG_INTERRUPT);
				return 0;
			}
		default:

			if (scancode > 128)
				return 0;

			if (ctrl && shift)
				return ctrl_shift_macros(scancode);
				
			if (ctrl)
				return ctrl_macros(scancode);

			lastpressedkey = shift ? upper_ascii_codes[scancode] : lower_ascii_codes[scancode];

			//printf("lastpressekey %d -> ", YELLOW, lastpressedkey);
			if (is_defined_key(scancode)) {
				//printf("defined key ->", GREEN);
				lastpressedkey = scancode;
			} else {
				//printf("ascii code %c -> ", CYAN, lastpressedkey);
			}
			//PRINT_CHAR('\n');

			return lastpressedkey;
	}
	return 0;
}

static void keyboard_handler(t_registers regs) {

	(void)regs;
	handle_key_input();

}

void init_keyboard(void) {
	shift = 0;
	ctrl = 0;
	memset(pressedkeys, 0, 256);

	register_interrupt_handler(IRQ1, &keyboard_handler);
	register_interrupt_handler(IRQ0, &scheduler_tick);
}
