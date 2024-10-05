#include "../includes/interrupts.h"
#include "../includes/kernel.h"
#include "../includes/memory.h"
#include "../includes/vga.h"
#include "../includes/microshell.h"

extern t_interrupt_handler_func_ptr interrupt_handlers[256];

const char *exception_messages[] = {
    "Division By Zero", // 0
    "Debug", // 1
    "Non Maskable Interrupt", // 2
    "Breakpoint", // 3
    "Into Detected Overflow", // 4
    "Out of Bounds", // 5
    "Invalid Opcode", // 6
    "No Coprocessor", // 7
    "Double Fault", // 8
    "Coprocessor Segment Overrun", // 9
    "Bad TSS", // 10
    "Segment Not Present", // 11
    "Stack Fault", // 12
    "General Protection Fault", // 13
    "Page Fault", // 14
    "Unknown Interrupt", // 15
    "Coprocessor Fault", // 16
    "Alignment Check", // 17
    "Machine Check", // 18
    "Reserved", // 19
    "Reserved", // 20
    "Reserved", // 21
    "Reserved", // 22
    "Reserved", // 23
    "Reserved", // 24
    "Reserved", // 25
    "Reserved", // 26
    "Reserved", // 27
    "Reserved", // 28
    "Reserved", // 29
    "Reserved", // 30
    "Reserved" // 31
};



void isr_handler(t_registers regs)
{
    if (regs.int_no < 32) { // If the interrupt number is less than 32, it's an exception

        DEBUG_PRINT("Exception: %s (int_no: %d)\n", exception_messages[regs.int_no], regs.int_no);
        print_kernel_stack(STACK_BASE, 200);
        KERNEL_PANIC("Exception: %s (int_no: %d)\n", exception_messages[regs.int_no], regs.int_no);
    }else if (regs.int_no < 256) { // It's an IRQ
        
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