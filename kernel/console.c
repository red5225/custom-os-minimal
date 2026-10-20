#include <stddef.h>
#include <stdint.h>

#include "include/console.h"

#define VGA_MEMORY ((volatile uint16_t *)0xB8000)
#define VGA_COLUMNS 80
#define VGA_ROWS 25
#define VGA_ATTRIBUTE 0x0F
#define BACKGROUND_RGB 0xFF7D5C00
#define TEXT_RGB 0xFF000000

static struct boot_framebuffer screen;
static int use_framebuffer;
static size_t column;
static size_t row;

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
            uint32_t rgb = (bits & (16u >> x_offset)) ? TEXT_RGB : BACKGROUND_RGB;
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

void console_clear(void)
{
    column = 0;
    row = 0;

    if (use_framebuffer) {
        for (uint32_t y = 0; y < screen.height; ++y) {
            for (uint32_t x = 0; x < screen.width; ++x) {
                put_pixel(x, y, BACKGROUND_RGB);
            }
        }
        return;
    }

    for (size_t i = 0; i < VGA_COLUMNS * VGA_ROWS; ++i) {
        VGA_MEMORY[i] = (VGA_ATTRIBUTE << 8) | ' ';
    }
}

void console_putc(char character)
{
    if (character == '\r') {
        column = 0;
        return;
    }

    if (character == '\b') {
        if (column > 0) {
            --column;
        }
        character = ' ';
    }

    if (character == '\n') {
        column = 0;
        ++row;
    } else {
        if (use_framebuffer) {
            draw_character(character, (uint32_t)column * 8,
                           (uint32_t)row * 12);
        } else {
            VGA_MEMORY[row * VGA_COLUMNS + column] =
                (VGA_ATTRIBUTE << 8) | (uint8_t)character;
        }

        ++column;
        if (column >= (use_framebuffer ? screen.width / 8 : VGA_COLUMNS)) {
            column = 0;
            ++row;
        }
    }

    if (row >= (use_framebuffer ? screen.height / 12 : VGA_ROWS)) {
        console_clear();
    }
}

void console_write(const char *text)
{
    while (*text != '\0') {
        console_putc(*text++);
    }
}
