bits 32

section .multiboot
        dd 0x1BADB002               ; Magic number for bootloader
        dd 0x0                      ; Flags
        dd - (0x1BADB002 + 0x0)     ; Checksum

section .text
global _start
extern start                         ; Main
_start:
    cli                             ; Clear interrupts
    sti
    xor ebp, ebp
    mov esp, stack_space            ; Stack pointer
    push ebx
    call start                       ; Main call
    jmp .1
.1:
    hlt
    ; hlt                           ; Halt CPU

section .bss
    ; resb 8192                     ; stack size : 8KB
    resb 4096                       ; stack size : 4KB
stack_space: