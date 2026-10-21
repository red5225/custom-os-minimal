#include <stddef.h>
#include <stdint.h>

#include "include/console.h"

#define VGA_MEMORY ((volatile uint16_t *)0xB8000)
#define VGA_COLUMNS 80
#define VGA_ROWS 25
#define VGA_ATTRIBUTE 0x0F
#define SHELL_BACKGROUND_RGB 0xFFF28C28u
#define SHELL_TEXT_RGB 0xFF000000u

static struct boot_framebuffer screen;
static int use_framebuffer;
static size_t column;
static size_t row;
static uint32_t background_rgb = SHELL_BACKGROUND_RGB;
static uint32_t text_rgb = SHELL_TEXT_RGB;

/* Simple 5x7 capital-letter font. Lowercase is drawn as capitals for now. */
static const uint8_t letters[26][7] = {
    {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
    {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
    {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
    {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
    {14,4,4,4,4,4,14}, {7,2,2,2,18,18,12},
    {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
    {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17},
    {14,17,17,17,17,17,14}, {30,17,17,30,16,16,16},
    {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
    {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
    {17,17,17,17,17,17,14}, {17,17,17,17,17,10,4},
    {17,27,21,21,17,17,17}, {17,17,10,4,10,17,17},
    {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31}
};

static void put_pixel(uint32_t x, uint32_t y, uint32_t rgb)
{
    if (x < screen.width && y < screen.height) {
        uintptr_t address = (uintptr_t)screen.address +
                            (uintptr_t)y * screen.pitch + x * 4u;
        *(volatile uint32_t *)address = rgb;
    }
}

static void draw_character(char character, uint32_t x, uint32_t y)
{
    char upper = character;
    if (upper >= 'a' && upper <= 'z') {
        upper = (char)(upper - ('a' - 'A'));
    }

    for (uint32_t y_offset = 0; y_offset < 7; ++y_offset) {
        uint8_t bits = 0;
        if (upper >= 'A' && upper <= 'Z') {
            bits = letters[upper - 'A'][y_offset];
        } else if (character == '-') {
            bits = y_offset == 3 ? 31 : 0;
        } else if (character == '>') {
            bits = y_offset == 3 ? 24 :
                   (y_offset == 2 || y_offset == 4 ? 12 :
                   (y_offset == 1 || y_offset == 5 ? 4 : 0));
        } else if (character == '.') {
            bits = y_offset == 6 ? 4 : 0;
        } else if (character == ':') {
            bits = (y_offset == 2 || y_offset == 5) ? 4 : 0;
        }
        for (uint32_t x_offset = 0; x_offset < 5; ++x_offset) {
            uint32_t rgb = (bits & (16u >> x_offset)) ? text_rgb : background_rgb;
            put_pixel(x + x_offset, y + y_offset, rgb);
        }
    }
}

void console_init(const struct boot_framebuffer *framebuffer)
{
    use_framebuffer = framebuffer != 0;
    if (use_framebuffer) {
        screen = *framebuffer;
    }
    console_clear();
}

void console_set_colors(uint32_t background, uint32_t foreground)
{
    background_rgb = background;
    text_rgb = foreground;
}

void console_clear(void)
{
    column = 0;
    row = 0;
    if (use_framebuffer) {
        for (uint32_t y = 0; y < screen.height; ++y) {
            for (uint32_t x = 0; x < screen.width; ++x) {
                put_pixel(x, y, background_rgb);
            }
        }
        return;
    }
    /* VGA text mode has a fixed hardware palette; approximate orange with
       black text on a brown/yellow background attribute. */
    for (size_t i = 0; i < VGA_COLUMNS * VGA_ROWS; ++i) {
        VGA_MEMORY[i] = (0xE0u << 8) | ' ';
    }
}

static void console_scroll_up(void)
{
    if (use_framebuffer) {
        if (screen.height <= 12) { console_clear(); return; }
        for (uint32_t y = 12; y < screen.height; ++y) {
            volatile uint32_t *dst = (volatile uint32_t *)((uintptr_t)screen.address + (uintptr_t)(y - 12) * screen.pitch);
            volatile uint32_t *srcp = (volatile uint32_t *)((uintptr_t)screen.address + (uintptr_t)y * screen.pitch);
            for (uint32_t x = 0; x < screen.pitch / 4u; ++x) dst[x] = srcp[x];
        }
        for (uint32_t y = screen.height - 12; y < screen.height; ++y)
            for (uint32_t x = 0; x < screen.width; ++x) put_pixel(x, y, background_rgb);
        row = screen.height / 12 - 1;
    } else {
        for (size_t y = 1; y < VGA_ROWS; ++y)
            for (size_t x = 0; x < VGA_COLUMNS; ++x)
                VGA_MEMORY[(y - 1) * VGA_COLUMNS + x] = VGA_MEMORY[y * VGA_COLUMNS + x];
        uint8_t attr = background_rgb == SHELL_BACKGROUND_RGB ? 0xE0u : 0x0Fu;
        for (size_t x = 0; x < VGA_COLUMNS; ++x)
            VGA_MEMORY[(VGA_ROWS - 1) * VGA_COLUMNS + x] = ((uint16_t)attr << 8) | ' ';
        row = VGA_ROWS - 1;
    }
}

void console_putc(char character)
{
    if (character == '\r') {
        column = 0;
        return;
    }
    if (character == '\b') {
        if (column > 0) --column;
        character = ' ';
    }
    if (character == '\n') {
        column = 0;
        ++row;
    } else {
        if (use_framebuffer) {
            draw_character(character, (uint32_t)column * 8, (uint32_t)row * 12);
        } else {
            VGA_MEMORY[row * VGA_COLUMNS + column] = (0xE0u << 8) | (uint8_t)character;
        }
        ++column;
        if (column >= (use_framebuffer ? screen.width / 8 : VGA_COLUMNS)) {
            column = 0;
            ++row;
        }
    }
    if (row >= (use_framebuffer ? screen.height / 12 : VGA_ROWS)) {
        console_scroll_up();
    }
}

void console_write(const char *text)
{
    while (*text != '\0') console_putc(*text++);
}
