#include "include/bootinfo.h"

/* Read the framebuffer details that GRUB passes to the kernel. */
int bootinfo_find_framebuffer(uint32_t magic,
                              uint32_t info_address,
                              struct boot_framebuffer *out)
{
    if (magic != MULTIBOOT2_BOOT_MAGIC || info_address == 0 || out == 0) {
        return 0;
    }

    const uint8_t *base = (const uint8_t *)(uintptr_t)info_address;
    uint32_t total_size = *(const uint32_t *)base;

    if (total_size < 16) {
        return 0;
    }

    uint32_t offset = 8;
    while (offset + 8 <= total_size) {
        uint32_t type = *(const uint32_t *)(base + offset);
        uint32_t size = *(const uint32_t *)(base + offset + 4);

        if (size < 8 || offset + size > total_size) {
            break;
        }

        if (type == MULTIBOOT2_TAG_FRAMEBUFFER && size >= 32) {
            out->address = *(const uint64_t *)(base + offset + 8);
            out->pitch = *(const uint32_t *)(base + offset + 16);
            out->width = *(const uint32_t *)(base + offset + 20);
            out->height = *(const uint32_t *)(base + offset + 24);
            out->bits_per_pixel = base[offset + 28];
            out->type = base[offset + 29];

            return out->bits_per_pixel == 32 &&
                   out->type == 1 &&
                   out->width > 0 &&
                   out->height > 0 &&
                   out->pitch >= out->width * 4;
        }

        if (type == MULTIBOOT2_TAG_END) {
            break;
        }

        /* Multiboot2 tag sizes are rounded up to an eight-byte boundary. */
        offset = (offset + size + 7u) & ~7u;
    }

    return 0;
}
