#include <stddef.h>
#include <stdint.h>

#define VGA ((volatile uint16_t *)0xB8000)
#define VGA_W 80
#define VGA_H 25
#define COLOR 0x0F

static size_t row, col;
static uint8_t color = COLOR;

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void serial_init(void) {
    outb(0x3F9, 0x00); outb(0x3FB, 0x80);
    outb(0x3F8, 0x03); outb(0x3F9, 0x00);
    outb(0x3FB, 0x03); outb(0x3FA, 0xC7); outb(0x3FC, 0x0B);
}
static void serial_char(char c) {
    for (unsigned int i = 0; i < 1000000; ++i) if (inb(0x3FD) & 0x20) break;
    outb(0x3F8, (uint8_t)c);
}
static void serial_write(const char *s) { while (*s) serial_char(*s++); }

static void scroll(void) {
    for (size_t y = 1; y < VGA_H; ++y)
        for (size_t x = 0; x < VGA_W; ++x) VGA[(y - 1) * VGA_W + x] = VGA[y * VGA_W + x];
    for (size_t x = 0; x < VGA_W; ++x) VGA[(VGA_H - 1) * VGA_W + x] = ((uint16_t)color << 8) | ' ';
    row = VGA_H - 1;
}
static void putc_term(char c) {
    if (c == '\r') { col = 0; return; }
    if (c == '\n') { col = 0; if (++row >= VGA_H) scroll(); return; }
    if (c == '\b') { if (col) { --col; VGA[row * VGA_W + col] = ((uint16_t)color << 8) | ' '; } return; }
    VGA[row * VGA_W + col] = ((uint16_t)color << 8) | (uint8_t)c;
    if (++col >= VGA_W) { col = 0; if (++row >= VGA_H) scroll(); }
}
static void write(const char *s) { while (*s) { putc_term(*s); serial_char(*s++); } }
static void clear_screen(void) {
    for (size_t i = 0; i < VGA_W * VGA_H; ++i) VGA[i] = ((uint16_t)color << 8) | ' ';
    row = col = 0;
}
static void prompt(void) { write("\nxc> "); }

static char keymap[128] = {
    [0x02]='1',[0x03]='2',[0x04]='3',[0x05]='4',[0x06]='5',[0x07]='6',[0x08]='7',[0x09]='8',[0x0A]='9',[0x0B]='0',
    [0x0C]='-',[0x0D]='=',[0x0E]='\b',[0x0F]='\t',[0x10]='q',[0x11]='w',[0x12]='e',[0x13]='r',[0x14]='t',[0x15]='y',[0x16]='u',[0x17]='i',[0x18]='o',[0x19]='p',
    [0x1A]='[',[0x1B]=']',[0x1C]='\n',[0x1E]='a',[0x1F]='s',[0x20]='d',[0x21]='f',[0x22]='g',[0x23]='h',[0x24]='j',[0x25]='k',[0x26]='l',[0x27]=';',[0x28]='\'',[0x29]='`',
    [0x2B]='\\',[0x2C]='z',[0x2D]='x',[0x2E]='c',[0x2F]='v',[0x30]='b',[0x31]='n',[0x32]='m',[0x33]=',[',[0x34]='.',[0x35]='/',[0x39]=' '
};

static void execute(char *cmd) {
    if (!cmd[0]) return;
    if (cmd[0]=='h' && cmd[1]=='e' && cmd[2]=='l' && cmd[3]=='p' && !cmd[4]) write("\nCommands: help, about, clear, reboot\n");
    else if (cmd[0]=='a' && cmd[1]=='b' && cmd[2]=='o' && cmd[3]=='u' && cmd[4]=='t' && !cmd[5]) write("\nCustom OS Minimal: freestanding x86_64 kernel. No filesystem or network yet.\n");
    else if (cmd[0]=='c' && cmd[1]=='l' && cmd[2]=='e' && cmd[3]=='a' && cmd[4]=='r' && !cmd[5]) { clear_screen(); serial_write("\n[screen cleared]\n"); }
    else if (cmd[0]=='r' && cmd[1]=='e' && cmd[2]=='b' && cmd[3]=='o' && cmd[4]=='o' && cmd[5]=='t' && !cmd[6]) {
        write("\nReboot requested. Use Ctrl+Alt+Delete in the emulator if this platform blocks reboot.\n");
    } else write("\nUnknown command. Type help.\n");
}

void kernel_main(void) {
    serial_init();
    clear_screen();
    write("CUSTOM OS MINIMAL\n64-bit kernel | text terminal\nType help for commands.\n");
    serial_write("\nKERNEL_READY\n");
    prompt();
    char command[80]; size_t len = 0;
    for (;;) {
        if (!(inb(0x64) & 1)) { __asm__ volatile ("pause"); continue; }
        uint8_t scan = inb(0x60);
        if (scan & 0x80) continue;
        if (scan == 0x1C) {
            command[len] = 0;
            execute(command);
            len = 0;
            prompt();
        } else if (scan == 0x0E) {
            if (len) { --len; putc_term('\b'); serial_write("\b \b"); }
        } else if (scan < 128 && keymap[scan] && keymap[scan] != '\t') {
            if (len < sizeof(command) - 1) { command[len++] = keymap[scan]; putc_term(keymap[scan]); serial_char(keymap[scan]); }
        }
    }
}