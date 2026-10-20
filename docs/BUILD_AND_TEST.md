# Build and test the OS

## Recommended: use GitHub Actions first

1. Open the [Actions page](https://github.com/red5225/custom-os-minimal/actions).
2. Open the newest run on `main`.
3. Wait until it finishes. Do not download an artifact from a failed or still-running run.
4. If green, scroll to **Artifacts** and download `custom-os-minimal-zip` or `custom-os-minimal-iso`.
5. The ZIP includes the ISO and a SHA-256 checksum file.

## Build on a Debian/Ubuntu Linux computer

Install the toolchain once:

```sh
sudo apt-get update
sudo apt-get install -y build-essential grub-pc-bin grub-efi-amd64-bin xorriso mtools qemu-system-x86 zip
```

Then from the repository folder:

```sh
make clean
make iso
```

Expected files after a successful build:

- `build/kernel.elf` — the linked kernel.
- `build/custom-os-minimal.iso` — bootable ISO image.

If `grub-file --is-x86-multiboot2 build/kernel.elf` fails, the boot header or link layout is wrong. Read the first actual error above the final `make: ***` line; the last line usually only says that a previous command failed.

## Test in QEMU before physical hardware

```sh
qemu-system-x86_64 -machine q35 -m 256M -cdrom build/custom-os-minimal.iso -serial stdio
```

Close QEMU to stop the VM. You should see the GRUB menu, then the kernel terminal. Use `help`, `about`, and `clear` if keyboard input is available.

For a headless serial test:

```sh
timeout 15s qemu-system-x86_64 -machine q35 -m 256M -cdrom build/custom-os-minimal.iso -display none -serial stdio -monitor none -no-reboot
```

A timeout can be expected because a successful kernel shell intentionally runs forever. What matters is that the serial output contains `KERNEL_READY` and `CUSTOM OS MINIMAL`.

## macOS note

This Makefile expects GNU/Linux build tools. On an Intel Mac running macOS Monterey, the simplest reliable path is GitHub Actions or a Linux VM. Do not assume Homebrew's GRUB and `grub-mkrescue` setup is identical.

## Physical Chromebook testing

Test only after QEMU passes. Confirm that the Chromebook can boot the chosen external media and that you understand the device's firmware policy. Do not disable security features, remove write protection, or flash firmware just to test this ISO. Keep your existing OS and recovery media available. This project currently lacks modern Chromebook keyboard drivers, so a blank terminal or no keyboard response can be a hardware-driver limitation.
