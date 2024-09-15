#include "../includes/paging.h"
#include "../includes/stddef.h"
#include "../includes/vga.h"
#include "../includes/kernel.h"

#define KERNEL_HEAP_START 0x10000000  // Arbitrary start address for kernel heap
#define KERNEL_HEAP_END   0x40000000  // Arbitrary end address for kernel heap

static uint32_t next_virtual_address = KERNEL_HEAP_START;

void *allocate_virtual_address(uint32_t size) {
    DEBUG_PRINT("Allocating virtual address: %x\n", next_virtual_address);
    DEBUG_PRINT("Getting aligned size for: %u\n", size);
    uint32_t aligned_size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    DEBUG_PRINT("Aligned size: %u\n", aligned_size);
    
    if (next_virtual_address + aligned_size >= KERNEL_HEAP_END) {
        DEBUG_PRINT("Out of virtual memory: next_virtual_address: %u + aligned_size %u = %u\n", 
                    next_virtual_address, aligned_size, next_virtual_address + aligned_size);
        return NULL;
    }
    
    void *allocated_address = (void *)next_virtual_address;
    next_virtual_address += aligned_size;
    
    return allocated_address;
}

typedef struct {
    uint32_t size;
} allocation_header_t;


void *malloc(uint32_t size) {
    DEBUG_PRINT("Entering malloc, requested size: %u\n", size);

    uint32_t total_size = size + sizeof(allocation_header_t);
    DEBUG_PRINT("Total size (including header): %u\n", total_size);
    
    void *virtual_address = allocate_virtual_address(total_size);
    DEBUG_PRINT("Virtual address allocated: %p\n", virtual_address);
    if (virtual_address == NULL) {
        DEBUG_PRINT("Virtual address allocation failed\n");
        return NULL;
    }
    
    uint32_t num_pages = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;
    DEBUG_PRINT("Number of pages needed: %u\n", num_pages);
    
    DEBUG_PRINT("Starting page mapping loop\n");
    for (uint32_t i = 0; i < num_pages; i++) {
        DEBUG_PRINT("Mapping page %u of %u\n", i + 1, num_pages);
        uint32_t physical_page = allocate_physical_page();
        if (physical_page == 0) {
            DEBUG_PRINT("Physical page allocation failed for page %x\n", i + 1);
            // Cleanup on failure
            for (uint32_t j = 0; j < i; j++) {
                DEBUG_PRINT("Unmapping page %x\n", j + 1);
                unmap_page((uint32_t)virtual_address + j * PAGE_SIZE);
            }
            return NULL;
        }
        
        DEBUG_PRINT("Setting page: virtual %p, physical %x\n", (void *)((uint32_t)virtual_address + i * PAGE_SIZE), physical_page);
        kernel_colored_log(BROWN, "Setting page: virtual %p, physical %x\n", (void *)((uint32_t)virtual_address + i * PAGE_SIZE), physical_page);
        set_page((uint32_t)virtual_address + i * PAGE_SIZE, physical_page, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    }
    DEBUG_PRINT("Page mapping completed successfully\n");
    
    DEBUG_PRINT("Setting allocation header\n");
    allocation_header_t *header = (allocation_header_t *)virtual_address;
    DEBUG_PRINT("Header address: %p\n", (void *)header);
    header->size = size;
    DEBUG_PRINT("Header size set to: %u\n", header->size);
    
    void *user_address = (void *)((char *)virtual_address + sizeof(allocation_header_t));
    DEBUG_PRINT("User address calculated: %p\n", user_address);
    
    DEBUG_PRINT("Malloc completed successfully\n");
    return user_address;
}

uint32_t get_size(void *ptr) {
    allocation_header_t *header = (allocation_header_t *)((char *)ptr - sizeof(allocation_header_t));
    return header->size;
}

void free(void *ptr) {
    if (ptr == NULL) {
        DEBUG_PRINT("Attempted to free NULL pointer\n");
        return; 
    }

    allocation_header_t *header = (allocation_header_t *)((char *)ptr - sizeof(allocation_header_t));
    
    uint32_t size = header->size;
    uint32_t total_size = size + sizeof(allocation_header_t);
    
    // Calculate the start of the first page containing this allocation
    uint32_t start_address = (uint32_t)header;
    
    uint32_t num_pages = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;

    DEBUG_PRINT("Freeing allocation: size=%u, total_size=%u, num_pages=%u\n", size, total_size, num_pages);

    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t virtual_address = start_address + i * PAGE_SIZE;
        uint32_t physical_address = get_physical_address(virtual_address);

        if (physical_address) {
            DEBUG_PRINT("Unmapping and freeing page: virtual=%x, physical=%x\n", virtual_address, physical_address);
            unmap_page(virtual_address);
            free_physical_page(physical_address);
        } else {
            DEBUG_PRINT("No physical address found for virtual address: %x\n", virtual_address);
        }
    }

    DEBUG_PRINT("Free completed successfully\n");	
}