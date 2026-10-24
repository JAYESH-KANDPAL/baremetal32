CC      := gcc
LD      := ld
CFLAGS  := -m32 -std=gnu11 -O2 -Wall -Wextra -Werror \
           -ffreestanding -fno-pie -fno-stack-protector \
           -fno-builtin -fno-asynchronous-unwind-tables \
           -fno-unwind-tables -Iinclude
ASFLAGS := -m32 -ffreestanding -fno-pie
LDFLAGS := -m elf_i386 -T linker.ld -nostdlib -z max-page-size=0x1000

BUILD   := build
OBJECTS := $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/console.o

.PHONY: all check iso run clean

all: $(BUILD)/kernel.elf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: src/boot.s | $(BUILD)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/kernel.o: src/kernel.c include/console.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/console.o: src/console.c include/console.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

check: $(BUILD)/kernel.elf
	grub-file --is-x86-multiboot $(BUILD)/kernel.elf
	readelf -h $(BUILD)/kernel.elf
	@echo "ELF and Multiboot checks passed."

iso: check
	mkdir -p $(BUILD)/isodir/boot/grub
	cp $(BUILD)/kernel.elf $(BUILD)/isodir/boot/kernel.elf
	cp grub/grub.cfg $(BUILD)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD)/baremetal32.iso $(BUILD)/isodir

run: iso
	qemu-system-i386 -cdrom $(BUILD)/baremetal32.iso -m 128M

clean:
	rm -rf $(BUILD)
