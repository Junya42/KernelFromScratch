#include "../includes/gdt.h"

t_gdt_entry     gdt_entries[GDT_ENTRIES];
t_gdt_ptr       *gdt_ptr = (t_gdt_ptr *)GDT_ADDRESS;

static void set_gdt_entry_value(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
   gdt_entries[num].base_low = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high = (base >> 24) & 0xFF;
   gdt_entries[num].limit_low = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;
   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access = access;
}

void init_gdt()
{
    gdt_ptr->limit = (sizeof(t_gdt_entry) * GDT_ENTRIES) - 1;
    gdt_ptr->base = ((uint32_t)(&gdt_entries));

    set_gdt_entry_value(0, 0, 0, 0, 0);                // Null segment
    set_gdt_entry_value(1, 0, 0xFFFFFFFF, (uint8_t)GDT_CODE_PL0, 0xCF); // Kernel code
    set_gdt_entry_value(2, 0, 0xFFFFFFFF, (uint8_t)GDT_DATA_PL0, 0xCF); // Kernel data
    set_gdt_entry_value(3, 0, 0xFFFFFFFF, (uint8_t)GDT_STACK_PL0, 0xCF); // Kernel stack

    set_gdt_entry_value(4, 0, 0xFFFFFFFF, (uint8_t)GDT_CODE_PL3, 0xCF); // User code
    set_gdt_entry_value(5, 0, 0xFFFFFFFF, (uint8_t)GDT_DATA_PL3, 0xCF); // User data
    set_gdt_entry_value(6, 0, 0xFFFFFFFF, (uint8_t)GDT_STACK_PL3, 0xCF); // User stack

    gdt_flush(((uint32_t)(gdt_ptr)));
}
