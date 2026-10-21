# Boot sequence

The boot splash is implemented in freestanding C in `kernel/boot_sequence.c`.
It adapts the restrained dark-screen, centered-mark, progress-stage idea from
[os-ios-macos-alike](https://github.com/red5225/os-ios-macos-alike/blob/main/boot_demo.c).

The source prototype uses SDL2 and runs as a normal desktop application; it
cannot be linked into a freestanding kernel. The kernel version therefore uses
the existing framebuffer/VGA console and only freestanding C plus a short busy
wait. After the sequence, the display clears and the normal terminal starts.

This is a visual startup sequence, not a firmware bootloader. UEFI/GRUB and
`kernel/boot.S` still perform the actual machine boot and CPU setup.
