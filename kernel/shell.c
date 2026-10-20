#include <stddef.h>

#include "include/console.h"
#include "include/keyboard.h"

#define COMMAND_CAPACITY 80

static char command_line[COMMAND_CAPACITY];
static size_t command_length;

static int text_equals(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0' && *left == *right) {
        ++left;
        ++right;
    }

    return *left == '\0' && *right == '\0';
}

static void show_prompt(void)
{
    console_write("\nxc> ");
}

static void run_command(void)
{
    command_line[command_length] = '\0';

    if (text_equals(command_line, "help")) {
        console_write("\nCommands: help, about, clear\n");
    } else if (text_equals(command_line, "about")) {
        console_write("\nCustom OS Minimal: small x86-64 kernel and terminal.\n");
    } else if (text_equals(command_line, "clear")) {
        console_clear();
    } else if (command_length > 0) {
        console_write("\nThe shell is very limited sooooooo try another command or retry :|\n");
    }

    command_length = 0;
    show_prompt();
}

void shell_run(void)
{
    console_write("Welcome to my OS thingy\n");
    console_write("64-bit kernel / text shell\n");
    console_write("Type help for commands.\n");
    show_prompt();

    for (;;) {
        char key = keyboard_read_char();

        if (key == 0) {
            __asm__ volatile ("pause");
            continue;
        }

        if (key == '\n') {
            run_command();
            continue;
        }

        if (key == '\b') {
            if (command_length > 0) {
                --command_length;
                console_write("\b \b");
            }
            continue;
        }

        if (command_length < sizeof(command_line) - 1) {
            command_line[command_length++] = key;
            console_putc(key);
        }
    }
}
