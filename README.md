# Custom OS Minimal

A tiny educational x86-64 operating-system kernel with a text terminal. It is designed to be read and changed by a beginner learning C. The kernel is freestanding: it does not use Linux or another operating-system kernel.

> **Status:** experimental learning project. A successful GitHub build is not proof that every physical Chromebook will boot it. Back up important data and do not change firmware or disable device protections just to test this project.

## Start here

1. Read [`docs/START_HERE.md`](docs/START_HERE.md).
2. Read [`docs/FILE_GUIDE.md`](docs/FILE_GUIDE.md) before editing files.
3. Follow [`docs/BUILD_AND_TEST.md`](docs/BUILD_AND_TEST.md) to build and test in a virtual machine first.
4. Use [`docs/SAFE_CHANGES.md`](docs/SAFE_CHANGES.md) for the safe change-and-undo workflow.
5. Use [`docs/ADD_FEATURES.md`](docs/ADD_FEATURES.md) when adding commands or kernel features.

## What is included

- Multiboot2 entry code that switches the CPU into 64-bit mode.
- A small C kernel, a text console, serial diagnostics, a basic PS/2 keyboard reader and a simple command shell.
- A GRUB boot menu and a GitHub Actions workflow that builds an ISO, smoke-tests it in QEMU, and packages the ISO plus SHA-256 checksum in a ZIP.
- Beginner documentation in `docs/` and code comments in the kernel modules.

## What is not included yet

No filesystem, disk driver, USB driver, Wi-Fi/network stack, audio, multitasking, users, permissions, package manager, or persistent settings. The keyboard reader uses legacy PS/2 ports, which may not be exposed on a modern Chromebook. The framebuffer console is intentionally small and supports only a few punctuation characters.

## Repository map

- `kernel/boot.S`: CPU startup and transition to 64-bit mode.
- `kernel/kernel.c`: kernel entry point and startup sequence.
- `kernel/bootinfo.c`: reads bootloader-provided framebuffer details.
- `kernel/console.c`: writes text to the display.
- `kernel/serial.c`: sends diagnostic text to the emulated serial port.
- `kernel/keyboard.c`: polls a basic PS/2 keyboard.
- `kernel/shell.c`: handles commands typed at the prompt.
- `kernel/include/`: declarations shared between modules.
- `boot/grub.cfg`: GRUB menu and Multiboot2 loader command.
- `linker.ld`: tells the linker where kernel sections belong in memory.
- `Makefile`: repeatable build steps.
- `.github/workflows/build.yml`: automated build, QEMU smoke test and ZIP artifacts.
- `docs/`: beginner guides and detailed file documentation.

## Build output

The workflow uploads two artifacts when all required checks pass: `custom-os-minimal-zip` and `custom-os-minimal-iso`. Download them from the successful run's **Artifacts** section. The ZIP contains `custom-os-minimal.iso` and `SHA256SUMS.txt`.

## Safety rules

- Build and test in QEMU before testing hardware.
- Do not flash an ISO to your computer's internal drive.
- Do not change Chromebook firmware, write protection, or security settings as part of normal code debugging.
- Change one small thing at a time and keep commits so you can revert.
- Read the workflow result; never assume an artifact exists just because a workflow started.
