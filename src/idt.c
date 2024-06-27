#include "idt.h"
#include "keyboard.h"

// Define the IDT array and the pointer to the IDT
struct idt_entry idt[256];
struct idt_ptr idtp;

// Function to set an IDT entry
void set_idt_entry(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags | 0x60;
}

// Function to initialize the IDT
void init_idt() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear out the entire IDT, initializing it to 0s
    for (int i = 0; i < 256; i++) {
        set_idt_entry(i, 0, 0, 0);
    }

    // Set the entry for IRQ1 (keyboard interrupt) to point to the keyboard handler
    set_idt_entry(33, (uint32_t)keyboard_handler, 0x08, 0x8E);

    // Load the IDT
    load_idt();
}

// Function to initialize the IRQs
void init_irq() {
    // Initialize the Programmable Interrupt Controller (PIC) here
    // Enable IRQ1 (keyboard interrupt)
    enable_irq(1);
}

// Function to load the IDT
extern void load_idt();

// Function to enable an IRQ
void enable_irq(int irq_num) {
    // Add your code to enable the specific IRQ here
    // Example for PIC:
    uint8_t mask = inb(0x21);
    mask &= ~(1 << irq_num);
    outb(0x21, mask);
}
