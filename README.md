# Custom OS Minimal

A tiny x86-64 freestanding kernel with a text-mode terminal. This repository is deliberately separate from `red5225/custom-os`.

## What it is

- A small kernel written in C plus a 32-bit-to-64-bit assembly boot trampoline.
- GRUB/Multiboot2 boot path.
- VGA text output and basic PS/2 keyboard input for a simple command prompt.
- COM1 serial logging for automated QEMU smoke tests.
- GitHub Actions builds `custom-os-minimal.iso`, validates the kernel's Multiboot2 header, runs a QEMU smoke test, and packages the ISO in `custom-os-minimal.zip`.

## Build locally (Debian/Ubuntu)

Install dependencies: `sudo apt-get install build-essential gcc-multilib grub-pc-bin grub-efi-amd64-bin xorriso mtools qemu-system-x86`

Then run `make iso` and test with `make run`.

## Terminal commands

The initial prompt supports `help`, `clear`, `about`, and `reboot` (reboot depends on emulator/firmware support). It is an early kernel, not a full OS: there is no filesystem, networking, USB stack, or persistent storage yet. Keyboard input currently uses the legacy PS/2 controller, which may not be exposed by every Chromebook firmware.

## Download

Open the repository's **Actions** tab, select the latest successful **Build ISO and ZIP** run, and download the `custom-os-minimal-zip` artifact. The ZIP contains the bootable ISO and a SHA-256 checksum file.

A successful CI run verifies compilation, ISO creation, and the serial-output smoke test in QEMU. It does not prove compatibility with every physical Chromebook; test in QEMU first and keep recovery firmware/media available before attempting real hardware boot.