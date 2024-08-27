#include "../includes/interrupts.h"
#include "../includes/kernel.h"
#include "../includes/memory.h"

extern t_interrupt_handler_func_ptr interrupt_handlers[256];

const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(t_registers regs)
{
    if (regs.int_no < 32) { // If the interrupt number is less than 32, it's an exception

        KERNEL_PANIC("Exception: %s (int_no: %d)\n", exception_messages[regs.int_no], regs.int_no);
    } else if (regs.int_no < 256) { // It's an IRQ
        
        if (interrupt_handlers[regs.int_no] != 0) {
            t_interrupt_handler_func_ptr handler = interrupt_handlers[regs.int_no];
            handler(regs);
        } else {
            kernel_log("Unhandled interrupt: %d\n", regs.int_no);
        }
    } else { // It's an invalid interrupt number

        KERNEL_PANIC("Invalid interrupt number: %d\n", regs.int_no);
    }
}