#include <stddef.h>
#include "include/console.h"
#include "include/keyboard.h"

static char line[80];
static size_t length;
static int equals(const char *a, const char *b) {
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == 0 && *b == 0;
}
static void print_prompt(void) { console_write("\nXC> "); }
static void run_command(void) {
    line[length] = 0;
    if (equals(line, "help")) console_write("\nCommands: help, about, clear\n");
    else if (equals(line, "about")) console_write("\nMinimal x86-64 kernel and terminal.\n");
    else if (equals(line, "clear")) console_clear();
    else if (length != 0) console_write("\nUnknown command. Type help.\n");
    length = 0;
    print_prompt();
}
void shell_run(void) {
    console_write("CUSTOM OS MINIMAL\n64-BIT KERNEL / TEXT SHELL\nType help for commands.\n");
    print_prompt();
    for (;;) {
        char c = keyboard_read_char();
        if (!c) { __asm__ volatile ("pause"); continue; }
        if (c == '\n') { run_command(); continue; }
        if (c == '\b') {
            if (length) { --length; console_write("\b \b"); }
            continue;
        }
        if (length < sizeof(line)-1) { line[length++] = c; console_putc(c); }
    }
}
