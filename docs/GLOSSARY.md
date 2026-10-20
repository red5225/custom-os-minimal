# Beginner glossary

- **Assembly (`.S`)**: instructions close to what the CPU executes. Small syntax mistakes can stop booting.
- **Bootloader**: program that loads the kernel, such as GRUB.
- **C source (`.c`)**: human-readable code compiled into machine instructions.
- **Compiler**: program that turns C source into object files and reports mistakes.
- **Freestanding**: code built without assuming a normal operating system or C runtime is present.
- **Framebuffer**: a region of memory representing screen pixels.
- **GRUB**: a bootloader that can load kernels through protocols such as Multiboot2.
- **Header (`.h`)**: declares shared C functions and data types.
- **ISO**: disk-image file commonly used for bootable media.
- **Kernel**: central software that manages CPU and hardware resources.
- **Linker**: combines object files and assigns addresses to program sections.
- **Multiboot2**: a protocol GRUB can use to pass a kernel and boot information.
- **Object file (`.o`)**: compiled piece of a program, not yet the final kernel.
- **QEMU**: a virtual machine/emulator used to test operating-system images.
- **Serial console**: a simple text channel useful for early boot debugging.
- **UEFI**: modern firmware interface used to start bootloaders.
- **Virtual machine (VM)**: an emulated computer that lets you test without replacing your real OS.
