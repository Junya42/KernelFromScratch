#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#define MULTIBOOT_MAGIC 0x1BADB002
#define MULTIBOOT_FLAG 0x00010003
#define CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAG)

struct multiboot_header {
    unsigned int magic;
    unsigned int flags;
    unsigned int checksum;
} __attribute__((packed));

#endif

