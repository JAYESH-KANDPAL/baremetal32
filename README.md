# BareMetal32

BareMetal32 is a small educational 32-bit x86 hobby kernel written in C and assembly. It boots through GRUB using the Multiboot v1 protocol and displays diagnostic output in VGA text mode.

The goal is to make the boot path and core code understandable—not to claim this is a production operating system.

## Features

- Multiboot v1 header and assembly entry point
- Freestanding C build targeting ELF32/i386
- Linker script with `.text`, `.rodata`, `.data`, and `.bss`
- Explicit BSS clearing and a 16 KiB bootstrap stack
- VGA text console with a hardware cursor, newline, carriage return, backspace, tabs, and scrolling
- Small `kprintf` implementation supporting `%s`, `%c`, `%d`, `%u`, `%x`, `%p`, and `%%`
- Strict compiler warnings and a Multiboot/ELF validation target
- GRUB ISO and QEMU run targets

## Requirements (Debian/Ubuntu)

```sh
sudo apt update
sudo apt install build-essential gcc-multilib binutils grub-pc-bin xorriso mtools qemu-system-x86
```

## Build and run

```sh
make
make check
make iso
make run
```

`make check` verifies the Multiboot header and prints ELF metadata. `make run` builds a GRUB ISO and launches it in QEMU. Close the QEMU window to exit.

## Project layout

```text
.
├── src/
│   ├── boot.s
│   ├── console.c
│   └── kernel.c
├── include/
│   └── console.h
├── grub/
│   └── grub.cfg
├── linker.ld
├── Makefile
└── README.md
```

## Architecture notes

- `src/boot.s` contains the Multiboot v1 header and `start` entry point. It clears BSS, establishes a bootstrap stack, and calls `kmain`.
- `linker.ld` sets the ELF format, entry symbol, and output-section layout. The 1 MiB address is a conventional educational layout choice; the boot protocol and loader must agree with the image.
- `src/console.c` writes to the legacy VGA text buffer at physical address `0xB8000` and updates the VGA text cursor through I/O ports. This assumes the machine is in a compatible VGA text mode.
- `src/kernel.c` initializes the console, exercises formatted output, and halts in a loop.

## Current limitations

This is a learning project, not a secure or general-purpose OS. It currently assumes a GRUB Multiboot v1 boot path and legacy VGA text mode. It does not yet implement paging, a heap, interrupt handling, keyboard input, a filesystem, userspace, or modern UEFI graphics support.

## Roadmap

- [ ] Add serial logging for reliable automated boot tests
- [ ] Add unit tests for formatting and console logic using a host-side test harness
- [ ] Add CI to build and validate the ELF/ISO
- [ ] Add a GDT and IDT with documented exception handlers
- [ ] Add a physical memory map parser and page-frame allocator
- [ ] Add keyboard input and a tiny shell
- [ ] Document design decisions and known limitations

## Contributing

Issues and pull requests are welcome. For changes, include the motivation, how to reproduce the behavior, and test/build output. Keep changes small and document any hardware assumptions.
