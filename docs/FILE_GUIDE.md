# File-by-file guide

This guide explains the purpose of each important file, what it does, and what is safe for a beginner to edit.

## `kernel/boot.S` — CPU startup

**Job:** This is assembly, not C. It contains the Multiboot2 header, creates initial page tables, enables CPU features needed for 64-bit mode, sets up a stack, and calls `kernel_main`.

**Safe edits:** At first, only read comments. Later, edit comments or ask for a review before changing instructions.

**Risk:** A wrong address, register, page-table entry, or CPU mode transition can make the kernel stop before it can print anything. Test every boot-code change in QEMU.

## `kernel/kernel.c` — the conductor

**Job:** `kernel_main()` is the first C function. It initializes serial logging, checks bootloader framebuffer information, initializes the console, prints a ready marker, and starts the shell.

**Safe edits:** Change the startup messages. Keep initialization before functions that depend on it. The ready marker is used by the CI smoke test, so do not remove `KERNEL_READY` or `CUSTOM OS MINIMAL` unless you also update the test intentionally.

## `kernel/bootinfo.c` and `kernel/include/bootinfo.h`

**Job:** Parse the Multiboot2 information block and look for a framebuffer tag. The `.c` file contains the work; the `.h` file describes the structure and function signature for other files.

**Safe edits:** Comments and explanatory names. Keep the tag layout and bounds checks together. The information comes from the bootloader, so never assume every tag exists.

## `kernel/console.c` and `kernel/include/console.h`

**Job:** Print characters and text. It uses a simple built-in bitmap font for a framebuffer and has a legacy VGA text fallback.

**Safe edits:** Startup text and, after learning arrays, the font table. Test newline and screen clearing after console changes.

**Risk:** Writing to the wrong framebuffer address can crash or draw nothing. Do not remove bounds checks.

## `kernel/serial.c` and `kernel/include/serial.h`

**Job:** Send diagnostics through COM1, commonly captured by QEMU's serial console. It helps debug a blank screen.

**Safe edits:** Add short diagnostic messages. Keep serial output independent from screen output so early boot errors can still be found.

## `kernel/keyboard.c` and `kernel/include/keyboard.h`

**Job:** Poll a legacy PS/2 keyboard controller and convert a small set of scancodes into characters.

**Safe edits:** Add comments or extend the key map carefully. The current map is incomplete: it does not properly handle Shift, Caps Lock, or all punctuation.

**Limit:** Modern laptops and Chromebooks may use an internal keyboard connected through a different controller. This code does not include USB HID or I2C-HID support.

## `kernel/shell.c`

**Job:** Keep a short command line, echo keys, and choose a command when Enter is pressed. Current commands are `help`, `about`, and `clear`.

**Best beginner file:** Start here. To add a command, read [Adding features](ADD_FEATURES.md). Keep the input buffer size check; it prevents writing past the end of the array.

## `kernel/include/*.h`

Header files tell C files which functions and data structures exist. If a function is shared by two files, declare it in a header and include that header in both places. Do not put normal function bodies or duplicate global variables in headers unless you know why.

## `linker.ld` — memory layout

**Job:** Tells the linker the kernel entry point and where sections such as code, data and BSS should be placed. BSS holds zero-initialized variables and the early stack/page tables.

**Safe edits:** Comments only until you understand ELF sections and virtual/physical addresses.

## `boot/grub.cfg` — boot menu

**Job:** Configures the GRUB menu, asks for a suitable display mode, and loads the kernel through Multiboot2.

**Safe edits:** Menu label and timeout. Keep the `multiboot2 /boot/kernel.elf` line unless you also change the ISO layout and loader protocol.

## `Makefile` — build recipe

**Job:** Compiles each C file separately, assembles `boot.S`, links the object files, checks the Multiboot2 header, and creates the ISO.

**Safe edits:** Add a new `.c` file to `C_SOURCES` when you create a module. Keep recipe lines indented with a real tab; spaces can break Makefiles.

## `.github/workflows/build.yml` — automatic builder

**Job:** Installs tools, builds the ISO, boots it in QEMU, checks serial markers, and uploads the ZIP and ISO as artifacts.

**Safe edits:** Change labels or documentation. Keep the build, smoke test, and artifact checks in order. A green build means the automated checks passed, not that physical Chromebook hardware is fully supported.

## `docs/`

These guides are the manual. Update the relevant guide when you change a file's behavior, add a new module, or change the build process.
