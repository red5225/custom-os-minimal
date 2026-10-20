# Architecture overview

## Big picture

```text
UEFI firmware
    |
    v
GRUB bootloader (from the ISO)
    |
    | Multiboot2 loads kernel.elf and passes boot information
    v
kernel/boot.S  -- set stack, page tables, CPU long mode
    |
    v
kernel/kernel.c -- kernel_main()
    |-- serial.c    diagnostics
    |-- bootinfo.c  framebuffer details from GRUB
    |-- console.c   text output
    |-- shell.c     command prompt
    `-- keyboard.c  basic input polling
```

## Why assembly and C are both used

Assembly is used for the earliest CPU setup because C assumes that a stack and a suitable CPU execution mode already exist. Most later logic is written in C because functions, structs, arrays and comments make it easier to organize and understand.

## UEFI versus BIOS

UEFI is the computer's firmware interface. GRUB can be packaged with UEFI boot files on the ISO, but the kernel itself is loaded using the Multiboot2 protocol. The CI must create a valid hybrid/UEFI-capable ISO and the firmware must support the boot path. A GRUB configuration entry alone does not prove the ISO boots on a particular Chromebook.

## The terminal path

The shell asks the keyboard module for one character at a time. It stores characters in a fixed-size buffer. When Enter arrives, it compares the buffer with known command names and calls console functions to display the result. The fixed-size check is important: without it, typing too much could overwrite nearby kernel memory.

## Current limitations

This is a tiny learning kernel. It does not yet have interrupt handling, a scheduler, virtual memory management beyond early identity mapping, a filesystem, disk or USB drivers, Wi-Fi, modern Chromebook keyboard support, or a secure multi-user model. Add these in separate, tested steps rather than treating the current kernel as a general-purpose OS.
