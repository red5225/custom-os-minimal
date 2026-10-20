#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdint.h>
#include "bootinfo.h"

void console_init(const struct boot_framebuffer *framebuffer);
void console_clear(void);
void console_write(const char *text);
void console_putc(char c);
#endif
