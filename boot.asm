BITS 32
extern start

section .text
global _start
_start:
    cli                                 ; Clear interrupts
    mov esp, stack_space + stack_size   ; Set up stack
    call start                          ; Call the C kernel entry function
    hlt                                 ; Halt the CPU

section .bss
align 4
stack_space:
    resb 8192                           ; Reserve 8 KB for the stack
stack_size equ $ - stack_space