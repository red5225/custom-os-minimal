#ifndef BOOTINFO_H
#define BOOTINFO_H
#include <stdint.h>

/* Values passed by GRUB's Multiboot2 loader. */
#define MULTIBOOT2_BOOT_MAGIC 0x36d76289u
#define MULTIBOOT2_TAG_END 0u
#define MULTIBOOT2_TAG_FRAMEBUFFER 8u

struct boot_framebuffer {
    uint64_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bits_per_pixel;
    uint8_t type;
};

int bootinfo_find_framebuffer(uint32_t magic, uint32_t info_address,
                              struct boot_framebuffer *out);
#endif
