#include "../includes/paging.h"
#include "../includes/kernel.h"
#include "../includes/stddef.h"
#include "../includes/common.h"
#include "../includes/multiboot.h"
#include "../includes/vga.h"

extern multiboot_info_t *multiboot_info_ptr;

uint32_t total_physical_pages;
uint32_t *physical_memory_bitmap;


#define MAX_PHYSICAL_MEMORY (1024 * 1024 * 1024) // 1 GB max
#define MAX_PHYSICAL_PAGES (MAX_PHYSICAL_MEMORY / PAGE_SIZE)
#define BITMAP_SIZE (MAX_PHYSICAL_PAGES / 32)

static uint32_t static_memory_bitmap[BITMAP_SIZE];

#define MULTIBOOT_INFO_MEMORY 0x00000001
#define KERNEL_RESERVED_PAGES 32

void initialize_memory() {
    if (!(multiboot_info_ptr->flags & MULTIBOOT_INFO_MEMORY)) {
        KERNEL_PANIC("No memory information provided by bootloader.\n");
    }

    uint32_t total_memory = ((uint64_t)multiboot_info_ptr->mem_upper + (uint64_t)multiboot_info_ptr->mem_lower) * 1024;
    total_physical_pages = total_memory / PAGE_SIZE;

    if (total_physical_pages > MAX_PHYSICAL_PAGES) {
        KERNEL_PANIC("More physical memory (%d MB) than supported (%d MB).\n", 
                     total_memory / (1024 * 1024), MAX_PHYSICAL_MEMORY / (1024 * 1024));
    }

    uint32_t bitmap_size = (total_physical_pages + 31) / 32; // Round up to nearest 32
    physical_memory_bitmap = static_memory_bitmap;

    memset(physical_memory_bitmap, 0, bitmap_size * sizeof(uint32_t));

    for (uint32_t i = 0; i < KERNEL_RESERVED_PAGES; i++) {
        uint32_t bitmap_index = i / 32;
        uint32_t bit_index = i % 32;
        physical_memory_bitmap[bitmap_index] |= (1 << bit_index);
    }
    // Initialize bitmap to mark all pages as used

    kernel_log("Total physical memory: %u bytes (%u MB)\n", total_memory, total_memory / (1024 * 1024));
    DEBUG_PRINT("Total physical memory: %u bytes (%u MB)\n", total_memory, total_memory / (1024 * 1024));

    kernel_log("Total physical pages: %u\n", total_physical_pages);
    DEBUG_PRINT("Total physical pages: %u\n", total_physical_pages);
}

page_directory_t page_directory __attribute__((aligned(4096)));
page_table_t first_page_table __attribute__((aligned(4096)));

void free_physical_page(uint32_t physical_address) {
    uint32_t page_index = physical_address / PAGE_SIZE;
    
    uint32_t bitmap_index = page_index / 32;
    uint32_t bit_index = page_index % 32;

    physical_memory_bitmap[bitmap_index] &= ~(1 << bit_index);

    DEBUG_PRINT("Freed physical page: %x\n", physical_address);
    kernel_colored_log(LIGHT_GRAY, "Freed physical page: %x\n", physical_address);
}

uint32_t allocate_physical_page() {
    for (uint32_t i = 0; i < total_physical_pages; i++) {
        uint32_t bitmap_index = i / 32;
        uint32_t bit_index = i % 32;
        
        // DEBUG_PRINT("Checking page %u, bitmap_index: %u, bit_index: %u, bitmap value: %x\n", 
        //             i, bitmap_index, bit_index, physical_memory_bitmap[bitmap_index]);
        
        if (!(physical_memory_bitmap[bitmap_index] & (1 << bit_index))) {
            physical_memory_bitmap[bitmap_index] |= (1 << bit_index);
            uint32_t physical_address = i * PAGE_SIZE;
            DEBUG_PRINT("Allocated physical page: %x\n", physical_address);
            kernel_colored_log(LIGHT_GRAY, "Allocated physical page: %x\n", physical_address);
            return physical_address;
        }
    }
    DEBUG_PRINT("Out of physical memory\n");
    KERNEL_PANIC("Out of physical memory\n");
    return 0;
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

    page_table_t *first_page_table = (page_table_t *)allocate_physical_page();
    for (int i = 0; i < 1024; i++) {
        first_page_table->entries[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;
    }

    page_directory.entries[0] = (uint32_t)first_page_table | PAGE_PRESENT | PAGE_WRITE;

    load_page_directory((uint32_t*)&page_directory);
    enable_paging();
}

// void unmap_page(uint32_t virtual_address) {
//     DEBUG_PRINT("Unmapping page: %x\n", virtual_address);
//     uint32_t page_directory_index = virtual_address >> 22;
//     uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

//     uint32_t *page_table = (uint32_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
//     if (page_table) {

//         uint32_t physical_address = get_physical_address(virtual_address);
//         page_table[page_table_index] = 0;
        
//         // Invalidate the TLB (Translation Lookaside Buffer) for this virtual address
//         asm volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
//         DEBUG_PRINT("Page unmapped: %p -> %x\n", virtual_address, physical_address);
//     } else {
//         DEBUG_PRINT("Page not mapped: %p\n", virtual_address);
//     }
// }

void unmap_page(uint32_t virtual_address) {
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    uint32_t *page_table = (uint32_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
    if (page_table) {
        page_table[page_table_index] = 0;
        
        // Check if the page table is now empty
        uint8_t page_table_empty = 1;
        for (int i = 0; i < 1024; i++) {
            if (page_table[i] != 0) {
                page_table_empty = 0;
                break;
            }
        }
        
        // If the page table is empty, free it
        if (page_table_empty) {
            free_physical_page((uint32_t)page_table);
            page_directory.entries[page_directory_index] = 0;
        }

        asm volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
    }
}


// void set_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags) {
//     uint32_t page_directory_index = virtual_address >> 22;
//     uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

//     page_table_t *table = (page_table_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
//     if (!table) {
//         table = (page_table_t *)allocate_physical_page();
//         page_directory.entries[page_directory_index] = ((uint32_t)table) | PAGE_PRESENT | PAGE_WRITE;
//     }

//     table->entries[page_table_index] = (physical_address & ~0xFFF) | (flags & 0xFFF);
// }

void set_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags) {
    DEBUG_PRINT("Setting page: %x -> %x\n", virtual_address, physical_address);
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    page_table_t *table = (page_table_t *)(page_directory.entries[page_directory_index] & ~0xFFF);
    if (!table) {
        DEBUG_PRINT("Allocating new page table\n");
        table = (page_table_t *)allocate_physical_page();
        for (int i = 0; i < 1024; i++) {
            table->entries[i] = 0; // Clear the new page table
        }
        page_directory.entries[page_directory_index] = ((uint32_t)table) | PAGE_PRESENT | PAGE_WRITE;
    }

    table->entries[page_table_index] = (physical_address & ~0xFFF) | (flags & 0xFFF);
    
    // Invalidate the TLB for this virtual address
    asm volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
    DEBUG_PRINT("Page set: %x -> %x\n", virtual_address, physical_address);
}
