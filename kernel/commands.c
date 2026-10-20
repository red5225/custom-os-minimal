#include <stddef.h>

#include "include/commands.h"
#include "include/console.h"

/*
 * Add shell commands here.
 *
 * To add a command:
 *   1. Write a handler function taking const char *args.
 *   2. Add { "name", "short description", handler } to command_table.
 *
 * The help command lists this table automatically, so it only needs to be
 * registered once. Handlers must not depend on hosted C library functions.
 */

typedef void (*command_handler)(const char *args);

struct shell_command {
    const char *name;
    const char *description;
    command_handler run;
};

static int text_equals(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0' && *left == *right) {
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

static const char *skip_spaces(const char *text)
{
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    return text;
}

static void command_about(const char *args)
{
    (void)args;
    console_write("\nSmall os.\n");
    console_write("\nA small freestanding x86-64 kernel and command shell.\n");
    console_write("\nHad AI start this so I can edit it to how I like.\n");
}

static void command_clear(const char *args)
{
    (void)args;
    console_clear();
}

static void command_hi(const char *args)
{
    (void)args;
    console_write("\nHi!\n");
}

static void command_version(const char *args)
{
    (void)args;
    console_write("\nCustom OS Minimal version 0.1.0\n");
    console_write("\nArchitecture: x86_64\n");
}

static void command_echo(const char *args)
{
    console_write("\n");
    console_write(args);
    console_write("\n");
}

static void command_help(const char *args);

static const struct shell_command command_table[] = {
    { "help",    "List available commands", command_help },
    { "about",   "Show what this OS is", command_about },
    { "version", "Show OS version and architecture", command_version },
    { "clear",   "Clear the screen", command_clear },
    { "hi",      "Print a greeting", command_hi },
    { "echo",    "Print text", command_echo }
};

#define COMMAND_COUNT (sizeof(command_table) / sizeof(command_table[0]))

static void command_help(const char *args)
{
    size_t i;
    (void)args;

    console_write("\nAvailable commands:\n");
    for (i = 0; i < COMMAND_COUNT; ++i) {
        console_write("  ");
        console_write(command_table[i].name);
        console_write(" - ");
        console_write(command_table[i].description);
        console_write("\n");
    }
}

int commands_execute(char *line)
{
    char *name;
    char *args;
    size_t i;

    if (line == NULL) {
        return 0;
    }

    name = (char *)skip_spaces(line);
    if (*name == '\0') {
        return 1;
    }

    /* Split the command name from its arguments in-place. */
    args = name;
    while (*args != '\0' && *args != ' ' && *args != '\t') {
        ++args;
    }
    if (*args != '\0') {
        *args++ = '\0';
        args = (char *)skip_spaces(args);
    }

    for (i = 0; i < COMMAND_COUNT; ++i) {
        if (text_equals(name, command_table[i].name)) {
            command_table[i].run(args);
            return 1;
        }
    }

    return 0;
}
