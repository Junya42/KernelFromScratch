#include "../includes/interrupts.h"

extern t_interrupt_handler_func_ptr interrupt_handlers[256];

// This gets called from our ASM interrupt handler stub.
void isr_handler(t_registers regs)
{
    if (interrupt_handlers[regs.int_no] != 0)
    {
        t_interrupt_handler_func_ptr handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}