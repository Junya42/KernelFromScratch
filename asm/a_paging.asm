section .text
global load_page_directory
global enable_paging

; load_page_directory(uint32_t* page_directory)
; Loads the address of the page directory into the CR3 register
load_page_directory:
    mov eax, [esp + 4]  ; Load the argument (pointer to the page directory) from the stack into EAX
    mov cr3, eax        ; Load the page directory address into CR3 register
    ret                 ; Return to the caller

; enable_paging()
; Enables paging by setting the paging bit (bit 31) in the CR0 register
enable_paging:
    mov eax, cr0        ; Get the current value of the CR0 register
    or eax, 0x80000000  ; Set the paging bit (bit 31)
    mov cr0, eax        ; Write the updated value back to CR0
    ret                 ; Return to the caller
