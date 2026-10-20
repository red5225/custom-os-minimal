#include "include/bootinfo.h"

/* Walk GRUB's tagged boot-information block safely. */
int bootinfo_find_framebuffer(uint32_t magic, uint32_t address,
                              struct boot_framebuffer *out) {
    if (magic != MULTIBOOT2_BOOT_MAGIC || address == 0 || out == 0) return 0;
    const uint8_t *base = (const uint8_t *)(uintptr_t)address;
    uint32_t total_size = *(const uint32_t *)base;
    if (total_size < 16) return 0;

    for (uint32_t offset = 8; offset + 8 <= total_size;) {
        uint32_t type = *(const uint32_t *)(base + offset);
        uint32_t size = *(const uint32_t *)(base + offset + 4);
        if (size < 8 || offset + size > total_size) break;

        if (type == MULTIBOOT2_TAG_FRAMEBUFFER && size >= 32) {
            out->address = *(const uint64_t *)(base + offset + 8);
            out->pitch = *(const uint32_t *)(base + offset + 16);
            out->width = *(const uint32_t *)(base + offset + 20);
            out->height = *(const uint32_t *)(base + offset + 24);
            out->bits_per_pixel = base[offset + 28];
            out->type = base[offset + 29];
            return out->bits_per_pixel == 32 && out->type == 1 &&
                   out->width > 0 && out->height > 0 &&
                   out->pitch >= out->width * 4;
        }
        if (type == MULTIBOOT2_TAG_END) break;
        offset = (offset + size + 7u) & ~7u;
    }
    return 0;
}
