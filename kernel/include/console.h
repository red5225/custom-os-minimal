#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdint.h>
#include "bootinfo.h"

void console_init(const struct boot_framebuffer *framebuffer);
void console_set_colors(uint32_t background_rgb, uint32_t text_rgb);
void console_clear(void);
void console_write(const char *text);
void console_putc(char c);
#endif
