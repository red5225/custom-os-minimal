# How to add features

This guide shows where to add features without needing to understand every file.

## Add a shell command

Shell commands are kept separate from keyboard input:

- `kernel/shell.c` reads keys, edits the current line, and submits it.
- `kernel/commands.c` contains the command handlers and the central command table.
- `kernel/include/commands.h` declares the command dispatcher.

To add a command, edit only `kernel/commands.c` for the usual case.

1. Write a handler that accepts `const char *args`.
2. Add one entry to `command_table`.
3. Build and run the OS, then test the command.

Example handler:

```c
static void command_hello(const char *args)
{
    (void)args;
    console_write("\\nHello from Custom OS!\\n");
}
```

Register it in `command_table`:

```c
{ "hello", "Print a greeting", command_hello }
```

The `help` command reads this table automatically, so you do not need to edit a separate help list. Commands can read their text after the command name through `args`; for example, `echo hello`.

## Add a new C module

Only create a new module when the feature is big enough to deserve its own file.

1. Create `kernel/example.c` with a clear purpose and comments.
2. If other modules need to call it, create `kernel/include/example.h`.
3. Include that header in the C files that use it.
4. Add `kernel/example.c` to `C_SOURCES` in the root `Makefile`.
5. If initialization is needed, call it from `kernel/kernel.c` in the correct order.
6. Update `docs/FILE_GUIDE.md` if the new module changes the project structure.
7. Build and test in QEMU before treating the change as finished.

## Where code belongs

- CPU startup and mode changes: `kernel/boot.S` (advanced; review carefully).
- Bootloader information: `kernel/bootinfo.c` and `kernel/include/bootinfo.h`.
- Drawing and printing text: `kernel/console.c` and `kernel/include/console.h`.
- Keyboard scan codes: `kernel/keyboard.c` and `kernel/include/keyboard.h`.
- Command definitions and behavior: `kernel/commands.c`.
- Keyboard input loop: `kernel/shell.c`.
- Early startup and initialization order: `kernel/kernel.c`.
- Build source list and boot images: `Makefile`.
- Boot menu and loader configuration: `boot/grub.cfg`.

## Before calling a feature finished

- It builds with no errors.
- QEMU reaches the shell.
- The feature works in QEMU.
- The code has comments explaining non-obvious parts.
- Relevant documentation is updated.
- Hardware limitations are documented honestly.
