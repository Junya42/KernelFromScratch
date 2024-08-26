#include "../includes/paging.h"
#include "../includes/stddef.h"

#define KERNEL_HEAP_START 0x10000000  // Arbitrary start address for kernel heap
#define KERNEL_HEAP_END   0x40000000  // Arbitrary end address for kernel heap

static uint32_t next_virtual_address = KERNEL_HEAP_START;

void *allocate_virtual_address(uint32_t size) {
    uint32_t aligned_size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    if (next_virtual_address + aligned_size >= KERNEL_HEAP_END) {
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
    uint32_t total_size = size + sizeof(allocation_header_t);
    
    void *virtual_address = allocate_virtual_address(total_size);
    if (virtual_address == NULL) {
        return NULL;
    }
    
    allocation_header_t *header = (allocation_header_t *)virtual_address;
    header->size = size;

    void *user_address = (void *)((char *)virtual_address + sizeof(allocation_header_t));

    uint32_t num_pages = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t physical_page = allocate_physical_page();
        if (physical_page == 0) {
            // Cleanup on failure
            for (uint32_t j = 0; j < i; j++) {
                unmap_page((uint32_t)virtual_address + j * PAGE_SIZE);
            }
            return NULL;
        }
        
        set_page((uint32_t)virtual_address + i * PAGE_SIZE, physical_page, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    }
    
    return user_address;
}

uint32_t get_size(void *ptr) {
    allocation_header_t *header = (allocation_header_t *)((char *)ptr - sizeof(allocation_header_t));
    return header->size;
}

void free(void *ptr) {
    if (ptr == NULL) {
        return; 
    }

    allocation_header_t *header = (allocation_header_t *)((char *)ptr - sizeof(allocation_header_t));
    
    uint32_t size = header->size;
    
    uint32_t num_pages = (size + sizeof(allocation_header_t) + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t virtual_address = (uint32_t)header + i * PAGE_SIZE;
        uint32_t physical_address = get_physical_address(virtual_address);

        if (physical_address) {
            unmap_page(virtual_address);
            free_physical_page(physical_address);
        }
    }
}