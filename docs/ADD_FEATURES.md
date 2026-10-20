# How to add features

This guide shows the usual path for extending the OS without needing to understand every file.

## Add a shell command (best first feature)

The shell is in `kernel/shell.c`. Find `run_command()`. It compares the text the user typed with command names.

For example, to add a `hello` command, add a branch before the `Unknown command` case:

```c
} else if (text_equals(command_line, "hello")) {
    console_write("\nHello from your kernel!\n");
```

Then update the command list in the `help` branch so it includes `hello`. Save, commit, build, and test `hello` in QEMU.

## Add a new C module

Only create a new module when the feature is big enough to deserve its own file.

1. Create `kernel/example.c` with a clear purpose and comments.
2. If other modules need to call it, create `kernel/include/example.h` with include guards and function declarations.
3. Include that header in the C files that use it.
4. Add `kernel/example.c` to `C_SOURCES` in the `Makefile`.
5. If the new module needs initialization, call its init function from `kernel/kernel.c` in the correct order.
6. Add a section to `docs/FILE_GUIDE.md` and update this guide.
7. Build, run QEMU, check serial output, and only then merge the change.

## How to choose where code belongs

- CPU startup and mode changes: `boot.S` (advanced; ask for review first).
- Bootloader information: `bootinfo.c` and `bootinfo.h`.
- Drawing or printing text: `console.c` and `console.h`.
- Keyboard scancodes: `keyboard.c` and `keyboard.h`.
- Typed commands and command behavior: `shell.c`.
- Early startup and initialization order: `kernel.c`.
- Diagnostic messages: `serial.c`.
- Build source list and commands: `Makefile`.
- Boot menu and loader configuration: `boot/grub.cfg`.
- Link addresses and ELF sections: `linker.ld` (advanced).

## Add a console function

Put the function implementation in `kernel/console.c`, declare it in `kernel/include/console.h`, then include the header in callers. Use existing functions as patterns. Keep the implementation small and test newline, backspace and screen clearing behavior.

## Add a keyboard key

The table in `kernel/keyboard.c` maps PS/2 set-1 scan codes to characters. Add a mapping only if you know the scan code. This map does not implement Shift/Caps Lock and may not work with a modern Chromebook's internal keyboard. Do not assume the keyboard is broken just because the PS/2 port returns no data.

## Before calling a feature finished

- It builds with no errors.
- QEMU reaches the shell.
- The feature works in QEMU.
- The code has comments explaining non-obvious parts.
- The relevant documentation is updated.
- The CI artifact is downloaded only from a successful run.
- Hardware limitations are written down honestly.
