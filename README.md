# Custom OS Minimal

A tiny freestanding x86-64 kernel with a text terminal. GitHub Actions builds a bootable ISO and packages it in a ZIP.

## Build output

The workflow builds `custom-os-minimal.iso` and `custom-os-minimal.zip`. The ISO is generated with GRUB's BIOS and x86-64 EFI modules where supported by the installed GRUB tools. The GRUB config prefers a text console and does not require a graphics mode.

Download the ZIP from the **custom-os-minimal-zip** artifact on the [Actions page](https://github.com/red5225/custom-os-minimal/actions). It contains the ISO and a SHA-256 checksum file.

## Important hardware notes

- The kernel is a minimal Multiboot2 kernel, not a complete general-purpose operating system.
- The terminal currently relies on legacy VGA text output and PS/2 keyboard input; UEFI firmware and modern Chromebook hardware may not expose these interfaces in the same way.
- A successful QEMU smoke test proves the tested emulator path only. It does **not** prove that the ISO boots on the Acer Chromebook CB314-3H-C4V5.
- For a Chromebook, boot the USB using the firmware's **UEFI** entry if available. The firmware must permit external boot, and Secure Boot may need to be disabled for an unsigned custom image.
- The `no suitable video mode found` message is a GRUB graphics-mode issue. This configuration explicitly selects the console/text path, but hardware-specific display output still needs testing on the device.

## Terminal commands

`help`, `about`, `clear`, and `reboot`.

No filesystem, networking, USB stack, or persistent storage is implemented yet.
