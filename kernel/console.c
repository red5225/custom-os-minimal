#include <stddef.h>
#include <stdint.h>
#include "include/console.h"

#define VGA_MEMORY ((volatile uint16_t *)0xB8000)
#define VGA_COLUMNS 80
#define VGA_ROWS 25
#define VGA_ATTRIBUTE 0x0F

static struct boot_framebuffer screen;
static int use_framebuffer;
static size_t column, row;

/* A deliberately small, readable 5x7 font for the banner and shell prompt. */
static const uint8_t letters[26][7] = {
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
 {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
 {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
 {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
 {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31}
};
static void pixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if (x < screen.width && y < screen.height)
        *(volatile uint32_t *)(uintptr_t)(screen.address + (uint64_t)y * screen.pitch + x * 4u) = rgb;
}
static void draw_char(char c, uint32_t x, uint32_t y) {
    char upper = c;
    if (upper >= 'a' && upper <= 'z') upper -= 'a' - 'A';
    for (uint32_t yy = 0; yy < 7; ++yy) {
        uint8_t bits = (upper >= 'A' && upper <= 'Z') ? letters[upper-'A'][yy] : 0;
        if (c == '-') bits = (yy == 3) ? 31 : 0;
        if (c == '>') bits = (yy == 3) ? 24 : ((yy == 2 || yy == 4) ? 12 : ((yy == 1 || yy == 5) ? 4 : 0));
        if (c == '.') bits = (yy == 6) ? 4 : 0;
        if (c == ':') bits = (yy == 2 || yy == 5) ? 4 : 0;
        if (c == ' ') bits = 0;
        for (uint32_t xx = 0; xx < 5; ++xx)
            pixel(x + xx, y + yy, (bits & (16u >> xx)) ? 0x00FFFFFF : 0x00101828);
    }
}
void console_init(const struct boot_framebuffer *fb) {
    use_framebuffer = fb != 0;
    if (use_framebuffer) screen = *fb;
    console_clear();
}
void console_clear(void) {
    column = row = 0;
    if (use_framebuffer) {
        for (uint32_t y=0; y<screen.height; ++y)
            for (uint32_t x=0; x<screen.width; ++x) pixel(x,y,0x00101828);
    } else {
        for (size_t i=0; i<VGA_COLUMNS*VGA_ROWS; ++i) VGA_MEMORY[i] = (VGA_ATTRIBUTE << 8) | ' ';
    }
}
void console_putc(char c) {
    if (c == '\r') { column = 0; return; }
    if (c == '\n') { column = 0; ++row; }
    else {
        if (use_framebuffer) draw_char(c, (uint32_t)column*8, (uint32_t)row*12);
        else VGA_MEMORY[row*VGA_COLUMNS+column] = (VGA_ATTRIBUTE << 8) | (uint8_t)c;
        ++column;
        if (column >= (use_framebuffer ? screen.width/8 : VGA_COLUMNS)) { column=0; ++row; }
    }
    if (row >= (use_framebuffer ? screen.height/12 : VGA_ROWS)) console_clear();
}
void console_write(const char *text) { while (*text) console_putc(*text++); }
