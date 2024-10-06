#include "multiboot.h"

__attribute__((section(".multiboot")))
struct multiboot_header header = {
    .magic = MULTIBOOT_MAGIC,
    .flags = MULTIBOOT_FLAG,
    .checksum = CHECKSUM
};

