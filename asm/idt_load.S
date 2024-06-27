section .text
global load_idt

extern idtp

load_idt:
    ; Load the IDT pointer using lidt
    lidt [idtp]
    ret
