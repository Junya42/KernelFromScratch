#include "keyboard.h"
#include "screen.h"
#include "idt.h"

static unsigned char input_table[256] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, // Left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, // Right shift
    '*',
    0, // Alt
    ' ',
    0, // Caps Lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 - F10 Keys
    0, // Num Lock
    0, // Scroll Lock
    0, // Home Key
    0, // Up Arrow
    0, // Page Up
    '-',
    0, // Left Arrow
    0,
    0, // Right Arrow
    '+',
    0, // End Key
    0, // Down Arrow
    0, // Page Down
    0, // Insert Key
    0, // Delete Key
    0, 0, 0, 0, 0, 0, 0, 0, 0, // F11 - F12 keys
    0, // Other non-mapped keys
};


static char input_table_shift[256] = {
        0, 27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, //* Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, /* Right shift */
    '*',
    0, /* Alt */
    ' ',
    0, /* Caps Lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 - F10 Keys */
    0, /* Num Lock */
    0, /* Scroll Lock */
    0, /* Home Key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
    '+',
    0, /* End Key */
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0, 0, 0, 0, 0, 0, 0, /* F11 - F12 keys */
    0 /* Other non mapped keys */
};

int ctrlshift = 0;

unsigned char get_input_bytes() {
    return inb(0x60);
}

char test_inputb_to_char(unsigned char inputb) {
    // Add your scan code to character conversion logic here
    // Example for a few keys:
    switch(inputb) {
        case 0x1E: return 'a';
        case 0x30: return 'b';
        case 0x2E: return 'c';
        // Add more scan codes as needed
        default: return 0;
    }
}

char inputb_to_char(uint8_t inputb) {
    return ctrlshift < 2 ? input_table[inputb] : input_table_shift[inputb];
}

void ctrl_macros(unsigned char inputb) {
    switch (inputb)
    {
        case 0x26:
            clear_screen();
            break; 
        default:
            print_string("CTRL MACROS", LIGHT_BLUE);
            break;
    }

}

void ctrl_shift_macros(unsigned char inputb) {
    switch (inputb)
    {
        case 0x26:
            clear_screen();
            break; 
        default:
            print_string("CTRL SHIFT MACROS", LIGHT_MAGENTA);
            break;
    }
}

void keyboard_handler() {
    
    unsigned char inputb;

    inputb = get_input_bytes();

    // Debugging: Print the input byte
    print_string("Input: ", LIGHT_BLUE);
    print_number(inputb, LIGHT_BLUE);
    print_string("\n", LIGHT_BLUE);
    
    if (inputb == 0x1D) { // Ctrl pressed
        ctrlshift++;
    } else if (inputb == 0x9D) { // Ctrl released
        ctrlshift--;
    } else if (inputb == 0x2A) { // Shift pressed
        ctrlshift += 2;
    } else if (inputb == 0xAA) { // Shift released
        ctrlshift -= 2;
    }

    if (1) { // check if it is a key press event ( not a release event )

        char character = inputb_to_char(inputb);

        switch (ctrlshift)
        {
            case 1:
                ctrl_macros(inputb);
                break;
            case 3:
                ctrl_shift_macros(inputb);
                break;
            default:
                if (inputb == 0x1C) { // Enter key
                    prompt();
                }
                else if (character != 0) {
                    PRINT_CHAR(character);
                    PRINT_CHAR('_');
                }
                break;
        }
    }
}