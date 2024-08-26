#include "../includes/paging.h"
#include "../includes/kernel.h"
#include "../includes/memory.h"
#include "../includes/stddef.h"

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
} multiboot_info_t;

extern multiboot_info_t *multiboot_info_ptr;

uint32_t total_physical_pages;
uint32_t *physical_memory_bitmap;

void initialize_memory() {
    if (multiboot_info_ptr->flags & 0x1) {  // Check if memory information is available
        uint32_t total_memory = (multiboot_info_ptr->mem_lower + multiboot_info_ptr->mem_upper) * 1024;
        
        total_physical_pages = total_memory / PAGE_SIZE;

        uint32_t bitmap_size = total_physical_pages / 32;
        physical_memory_bitmap = (uint32_t *)malloc(bitmap_size * sizeof(uint32_t));
        if (physical_memory_bitmap == NULL) {
            kernel_panic("Failed to allocate physical memory bitmap.\n");
        }

        for (uint32_t i = 0; i < bitmap_size; i++) {
            physical_memory_bitmap[i] = 0;
        }

        kernel_log("Total physical memory: %u bytes\n", total_memory);
        kernel_log("Total physical pages: %u\n", total_physical_pages);
    } else {
        kernel_panic("No memory information provided by bootloader.\n");
    }
}

page_directory_t page_directory __attribute__((aligned(4096)));
page_table_t first_page_table __attribute__((aligned(4096)));

void free_physical_page(uint32_t physical_address) {
    uint32_t page_index = physical_address / PAGE_SIZE;
    
    uint32_t bitmap_index = page_index / 32;
    uint32_t bit_index = page_index % 32;

    physical_memory_bitmap[bitmap_index] &= ~(1 << bit_index);

    kernel_log("Freed physical page: %x\n", physical_address);
}


uint32_t allocate_physical_page() {
    static uint32_t next_free_page = 0x100000;
    uint32_t allocated_page = next_free_page;
    next_free_page += PAGE_SIZE;
    return allocated_page;
}

uint32_t allocate_virtual_page() {
    static uint32_t next_virtual_page = 0x400000;
    uint32_t allocated_page = next_virtual_page;
    next_virtual_page += PAGE_SIZE;
    return allocated_page;
}


uint32_t get_physical_address(uint32_t virtual_address) {
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    page_table_t *table = (page_table_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
    if (!table) {
        return 0;
    }

    return table->entries[page_table_index] & ~0xFFF;
}

void initialize_paging() {
    for (int i = 0; i < 1024; i++) {
        page_directory.entries[i] = 0x00000002;
    }

    for (int i = 0; i < 1024; i++) {
        first_page_table.entries[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;
    }

    page_directory.entries[0] = (uint32_t)&first_page_table | PAGE_PRESENT | PAGE_WRITE;

    load_page_directory((uint32_t*)&page_directory);
    enable_paging();
}

void unmap_page(uint32_t virtual_address) {
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    uint32_t *page_table = (uint32_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
    if (page_table) {
        page_table[page_table_index] = 0;
        
        // Invalidate the TLB (Translation Lookaside Buffer) for this virtual address
        asm volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
    }
}


void set_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags) {
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    page_table_t *table = (page_table_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
    if (!table) {
        table = (page_table_t *)allocate_physical_page();
        page_directory.entries[page_directory_index] = ((uint32_t)table) | PAGE_PRESENT | PAGE_WRITE;
    }

    table->entries[page_table_index] = (physical_address & ~0xFFF) | (flags & 0xFFF);
}
