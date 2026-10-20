# Custom OS Minimal

A small freestanding x86-64 kernel with a simple text shell. GitHub Actions builds an ISO and packages it in a ZIP.

## Source layout

The kernel is split into small files so each part has one job:

- `kernel/boot.S` — CPU startup, switches to 64-bit mode, passes GRUB boot information to C.
- `kernel/kernel.c` — main entry point; connects boot info, screen, serial output and shell.
- `kernel/bootinfo.c` — reads GRUB's Multiboot2 framebuffer information.
- `kernel/console.c` — writes characters to the framebuffer when provided, otherwise tries VGA text memory.
- `kernel/keyboard.c` — basic US keyboard mapping using legacy PS/2 scancodes.
- `kernel/serial.c` — COM1 output used by QEMU's automated smoke test.
- `kernel/shell.c` — command prompt and command handling.
- `kernel/include/` — small header files that describe module interfaces.
- `boot/grub.cfg` — bootloader menu and text-mode preference.
- `Makefile` — build steps and ISO packaging.

## Build and test

GitHub Actions runs `make iso`, then starts the image in QEMU and checks for the kernel startup message. Download `custom-os-minimal-zip` from the [Actions page](https://github.com/red5225/custom-os-minimal/actions); the ZIP contains the ISO and SHA-256 checksum.

## Important limitations

- A successful QEMU smoke test does not prove the ISO boots on the Acer Chromebook CB314-3H-C4V5.
- The framebuffer is only used if GRUB supplies a compatible 32-bit framebuffer. VGA is a fallback, but modern UEFI systems may not expose legacy VGA text memory.
- Keyboard input currently requires a legacy PS/2-compatible controller. Some Chromebook firmware/hardware configurations do not expose one to the kernel.
- There is no filesystem, USB driver, network stack, or persistent storage yet.
- The shell currently supports `help`, `about`, and `clear`.

The project is intentionally minimal and should be extended one hardware interface at a time, with each change tested in QEMU before testing physical hardware.
