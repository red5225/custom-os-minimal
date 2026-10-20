#include <stddef.h>

#include "include/commands.h"
#include "include/console.h"
#include "include/keyboard.h"

#define COMMAND_CAPACITY 128u

static char command_line[COMMAND_CAPACITY];
static size_t command_length;

static void show_prompt(void)
{
    console_write("\nUser> ");
}

static void submit_command(void)
{
    command_line[command_length] = '\0';

    if (!commands_execute(command_line)) {
        console_write("\nUnknown command. Type help to see available commands.\n");
    }

    command_length = 0;
    command_line[0] = '\0';
    show_prompt();
}

void shell_run(void)
{
    console_write("\nWelcome to Custom OS Minimal\n");
    console_write("64-bit kernel command shell. Type help for commands.\n");
    show_prompt();

    for (;;) {
        char key = keyboard_read_char();

        if (key == 0) {
            __asm__ volatile ("pause");
            continue;
        }

        if (key == '\n' || key == '\r') {
            submit_command();
            continue;
        }

        if (key == '\b') {
            if (command_length > 0) {
                --command_length;
                command_line[command_length] = '\0';
                console_write("\b");
            }
            continue;
        }

        /* Only accept printable ASCII characters in the command line. */
        if (key >= 32 && key <= 126 &&
            command_length < sizeof(command_line) - 1) {
            command_line[command_length++] = key;
            command_line[command_length] = '\0';
            console_putc(key);
        }
    }
}
