BUILD := build
ISO_ROOT := $(BUILD)/iso-root
CC := gcc
AS := gcc
CFLAGS := -m64 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -mno-red-zone -mcmodel=small -Wall -Wextra -Werror -O2 -Ikernel
ASFLAGS := -m64 -ffreestanding -fno-pic -fno-pie -Ikernel
LDFLAGS := -m elf_x86_64 -T linker.ld -nostdlib

.PHONY: all clean iso run check
all: $(BUILD)/kernel.elf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: kernel/boot.S | $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(BUILD)/boot.o $(BUILD)/kernel.o linker.ld
	ld $(LDFLAGS) $(BUILD)/boot.o $(BUILD)/kernel.o -o $@

check: $(BUILD)/kernel.elf
	grub-file --is-x86-multiboot2 $(BUILD)/kernel.elf

iso: check
	mkdir -p $(ISO_ROOT)/boot/grub
	cp $(BUILD)/kernel.elf $(ISO_ROOT)/boot/kernel.elf
	cp boot/grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD)/custom-os-minimal.iso $(ISO_ROOT)

run: iso
	qemu-system-x86_64 -m 256M -cdrom $(BUILD)/custom-os-minimal.iso -serial stdio

clean:
	rm -rf $(BUILD)