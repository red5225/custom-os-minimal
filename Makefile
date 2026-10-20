BUILD := build
CC := gcc
LD := ld

CFLAGS := -m64 -std=gnu11 -ffreestanding -fno-stack-protector \
          -fno-pic -fno-pie -mno-red-zone -mgeneral-regs-only \
          -mcmodel=small -Wall -Wextra -O2 -Ikernel
ASFLAGS := -m64 -ffreestanding -fno-pic -fno-pie
LDFLAGS := -m elf_x86_64 -T linker.ld -nostdlib

C_SOURCES := kernel/kernel.c kernel/bootinfo.c kernel/console.c \
             kernel/serial.c kernel/keyboard.c kernel/shell.c
OBJECTS := $(BUILD)/boot.o $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SOURCES))

.PHONY: all clean check iso run
all: $(BUILD)/kernel.elf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: kernel/boot.S | $(BUILD)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

check: $(BUILD)/kernel.elf
	grub-file --is-x86-multiboot2 $(BUILD)/kernel.elf

iso: check
	mkdir -p $(BUILD)/iso-root/boot/grub
	cp $(BUILD)/kernel.elf $(BUILD)/iso-root/boot/kernel.elf
	cp boot/grub.cfg $(BUILD)/iso-root/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD)/custom-os-minimal.iso $(BUILD)/iso-root

run: iso
	qemu-system-x86_64 -machine q35 -m 256M -cdrom $(BUILD)/custom-os-minimal.iso -serial stdio

clean:
	rm -rf $(BUILD)
