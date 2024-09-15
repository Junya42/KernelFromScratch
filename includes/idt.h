#ifndef IDT_H
# define IDT_H

#include "stdint.h"

typedef struct __attribute__((packed))  idt_entry
{
    uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
    uint16_t sel;                 // Kernel segment selector.
    uint8_t  always0;             // This must always be zero.
    uint8_t  flags;               // More flags. See documentation.
    uint16_t base_hi;             // The upper 16 bits of the address to jump to.
}                                       t_idt_entry;

typedef struct __attribute__((packed))  idt_ptr
{
    uint16_t limit;
    uint32_t base;                // The address of the first element in our idt_entry_t array.
}                                       t_idt_ptr;

extern void idt_flush(uint32_t idt_ptr);

void init_idt(void);

#endif
