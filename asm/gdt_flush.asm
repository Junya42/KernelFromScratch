global gdt_flush

gdt_flush:
   mov eax, [esp+4]     ; 1st parameter : pointer to the IDT
   lgdt [eax]
   mov ax, 0x10         ; 0x10 is the offset in the GDT to our data segment
   mov ds, ax           ; Load all data segment selectors
   mov fs, ax
   mov gs, ax
   mov es, ax
   mov ax, 0x18         ; 0x18 is the offset in the GDT to our kernel stack
   mov ss, ax
   jmp 0x08:.flush      ; 0x08 is the offset to our code segment: far jump on it
.flush:
   ret