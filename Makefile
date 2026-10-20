BUILD := build
CC := gcc
LD := ld

CFLAGS := -m64 -std=gnu11 -ffreestanding -fno-stack-protector \
          -fno-pic -fno-pie -mno-red-zone -mgeneral-regs-only \
          -mcmodel=small -Wall -Wextra -O2 -Ikernel
ASFLAGS := -m64 -ffreestanding -fno-pic -fno-pie
LDFLAGS := -m elf_x86_64 -T linker.ld -nostdlib

C_SOURCES := kernel/kernel.c kernel/bootinfo.c kernel/console.c \
             kernel/serial.c kernel/keyboard.c kernel/commands.c \
             kernel/shell.c
OBJECTS := $(BUILD)/boot.o $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SOURCES))

DISK_IMAGE := $(BUILD)/custom-os-minimal.img
ESP_IMAGE := $(BUILD)/esp.img
ESP_OFFSET_SECTORS := 2048
ESP_SECTORS := 126976

.PHONY: all clean check iso image run
all: iso image

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

iso: $(BUILD)/custom-os-minimal.iso

$(BUILD)/custom-os-minimal.iso: $(BUILD)/kernel.elf boot/grub.cfg
	rm -rf $(BUILD)/iso-root
	mkdir -p $(BUILD)/iso-root/boot/grub
	cp $(BUILD)/kernel.elf $(BUILD)/iso-root/boot/kernel.elf
	cp boot/grub.cfg $(BUILD)/iso-root/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(BUILD)/iso-root
	test -s $@

$(BUILD)/BOOTX64.EFI: boot/grub.cfg | $(BUILD)
	grub-mkstandalone -O x86_64-efi -o $@ \
	  --modules="part_gpt part_msdos fat iso9660 multiboot2 normal configfile search search_fs_file search_fs_uuid echo terminal" \
	  "boot/grub/grub.cfg=boot/grub.cfg"
	file $@ | grep -Eqi 'PE32\+.*EFI application|PE32\+ executable.*EFI'

$(ESP_IMAGE): $(BUILD)/BOOTX64.EFI $(BUILD)/kernel.elf
	rm -f $@
	dd if=/dev/zero of=$@ bs=1M count=64 status=none
	printf 'label: gpt\nunit: sectors\nsector-size: 512\nstart=$(ESP_OFFSET_SECTORS),size=$(ESP_SECTORS),type=uefi,name="Custom OS EFI"\n' | sfdisk $@
	mkfs.fat -F 32 $@
	mmd -i $@ ::/EFI ::/EFI/BOOT ::/boot
	mcopy -i $@ $(BUILD)/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $@ $(BUILD)/kernel.elf ::/boot/kernel.elf
	test -s $@

image: $(DISK_IMAGE)

$(DISK_IMAGE): $(ESP_IMAGE)
	cp $(ESP_IMAGE) $@

run: iso
	qemu-system-x86_64 -machine q35 -m 256M -cdrom $(BUILD)/custom-os-minimal.iso -serial stdio

clean:
	rm -rf $(BUILD)
