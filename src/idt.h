#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Define the IDT entry structure
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;        // Our kernel segment goes here!
    uint8_t always0;     // This will ALWAYS be set to 0!
    uint8_t flags;       // More flags
    uint16_t base_hi;
} __attribute__((packed));

// Define a pointer structure for the IDT
struct idt_ptr {
    uint16_t limit;
    uint32_t base;       // The address of the first element in our idt_entry_t array
} __attribute__((packed));

// Declare an IDT of 256 entries
extern struct idt_entry idt[256];
extern struct idt_ptr idtp;

// Functions to set up the IDT
void set_idt_entry(int num, uint32_t base, uint16_t sel, uint8_t flags);
void init_idt();

// Function to load the IDT
extern void load_idt();

// Function to enable an IRQ
void enable_irq(int irq_num);

// Inline functions to read and write ports
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

#endif // IDT_H
