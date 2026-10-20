#include <stdint.h>
#include "include/serial.h"

/* COM1 is useful for QEMU logs; real laptops may not expose serial hardware. */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}
void serial_init(void) {
    outb(0x3F9, 0); outb(0x3FB, 0x80); outb(0x3F8, 3);
    outb(0x3F9, 0); outb(0x3FB, 3); outb(0x3FA, 0xC7); outb(0x3FC, 0x0B);
}
void serial_write(const char *text) {
    while (*text) {
        for (unsigned i = 0; i < 1000000; ++i) if (inb(0x3FD) & 0x20) break;
        outb(0x3F8, (uint8_t)*text++);
    }
}
