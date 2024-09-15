#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define TOTAL_PHYSICAL_PAGES (total_physical_memory / PAGE_SIZE)
#define PAGE_PRESENT 0x1
#define PAGE_WRITE   0x2
#define PAGE_USER    0x4  // User-mode page

// Page directory and page table entry structures
typedef struct {
    uint32_t entries[1024] __attribute__((aligned(4096)));
} page_table_t;

typedef struct {
    uint32_t entries[1024] __attribute__((aligned(4096)));
} page_directory_t;

extern void load_page_directory(uint32_t* page_directory);
extern void enable_paging();

void initialize_memory();
void initialize_paging();

uint32_t allocate_physical_page();
uint32_t allocate_virtual_page();
uint32_t get_physical_address(uint32_t virtual_address);
void unmap_page(uint32_t virtual_address);
void set_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags);
void initialize_paging();
void free_physical_page(uint32_t physical_address);

#endif
