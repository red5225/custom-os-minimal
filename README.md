# Custom OS Minimal

A small educational x86-64 operating-system kernel and command terminal, built from freestanding C and assembly. It does not use a Linux kernel or another full operating-system base.

## Important: choose the right download

- **For a USB stick intended to boot through UEFI, use \`custom-os-minimal.img\`.** It is a raw GPT disk image with a FAT32 EFI System Partition containing \`EFI/BOOT/BOOTX64.EFI\` and \`/boot/kernel.elf\`.
- \`custom-os-minimal.iso\` is the optical/virtual-machine ISO. A successful ISO smoke test does not prove that writing the ISO to a USB stick exposes a firmware-readable EFI System Partition.
- GitHub Actions artifact downloads are ZIP archives. Extract the artifact first, then select the \`.img\` file inside it in balenaEtcher. Flashing the image erases the selected USB drive, so double-check the target drive.

## Current status

GitHub Actions builds the kernel, ISO, and GPT/EFI raw disk image. CI tests the kernel handoff in BIOS QEMU for the ISO and in UEFI QEMU for both the raw disk image and ISO. These tests do not prove that a particular physical Chromebook will boot or that its keyboard, trackpad, Wi-Fi, audio, or storage devices are supported.

## Included

- Multiboot2 kernel entry that switches the CPU into 64-bit mode.
- Small freestanding C kernel, framebuffer/VGA console, serial diagnostics, legacy PS/2 keyboard polling, and a simple shell.
- GRUB configuration and a standalone x86-64 UEFI GRUB application for removable media.
- GitHub Actions that build, smoke-test, and package outputs with SHA-256 checksums.

## Not included yet

No filesystem, USB input stack, Chromebook EC/trackpad driver, Wi-Fi/network stack, audio, multitasking, user accounts, or persistent settings. The legacy PS/2 keyboard reader is unlikely to provide built-in Chromebook keyboard/trackpad support by itself.

## Safety

Test in QEMU first. Do not flash this image to your Chromebook's internal storage, change firmware write protection, or disable device protections just to test the kernel. A successful CI build is not a guarantee of physical-device compatibility.
