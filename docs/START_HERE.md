# Start Here: your first steps

Welcome! You do not need to understand the whole operating system at once. Think of it as a tiny machine made of small parts. Each file has one job, and the files call functions in other files to work together.

## What is a kernel?

A kernel is the central part of an operating system. It talks to the CPU and hardware and provides services that other programs can use. This project is a learning kernel, not a daily-use operating system.

## What happens when it boots?

1. The computer firmware starts a bootloader. In our ISO, that bootloader is GRUB.
2. GRUB reads `boot/grub.cfg` and loads `kernel.elf` using the Multiboot2 protocol.
3. `kernel/boot.S` starts in 32-bit mode, creates basic page tables, and switches the CPU to 64-bit mode.
4. Assembly calls `kernel_main()` in `kernel/kernel.c`.
5. The kernel reads boot information, initializes the console and serial logging, then starts the shell.
6. The shell prints `xc>` and checks for keyboard input.

## Your first command

When the terminal is working, type `help` and press Enter. The shell should list the commands it knows. Try `about` next. `clear` clears the screen.

The keyboard support is deliberately basic. If keys do not appear on your physical Chromebook, that may be because it does not provide a legacy PS/2 keyboard controller. It does not necessarily mean the whole kernel failed to boot.

## Learn in small steps

A good first task is to change a message in `kernel/shell.c`, rebuild, and test it in QEMU. Do not start by changing `boot.S`, `linker.ld`, or page tables; mistakes there can prevent the CPU from reaching C code.

Read next:

- [File-by-file guide](FILE_GUIDE.md)
- [Build and test](BUILD_AND_TEST.md)
- [Make safe changes](SAFE_CHANGES.md)
- [Add a feature](ADD_FEATURES.md)
