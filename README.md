<div align="center">

<h3>apoptOS</h3>
<p>A modern x86_64 UNIX-like microkernel-based operating system written in C and Assembly</p>

<img src="https://github.com/Tix3Dev/apoptOS/blob/main/misc/screenshots/logs.png">

</div>

## Features

- Higher half kernel
- Limine bootloader
- Stivale2 boot protocol
- Serial communication
- GDT and TSS
- IDT and ISR's
- Physical memory management -> Bitmap based allocator & Slab allocator
- Virtual memory management -> 4-level paging support
- Heap memory -> `malloc` and `free`
- Basic kernel library
- ACPI table parsing
- LAPIC and IOAPIC
- LAPIC timer and HPET
- SMP

## Goals

- Understand:
  - Microkernels
  - UNIX-like systems
  - Scheduling
  - Messaging (IPC)
  - Syscalls
  - Userland
  - ELF
  - Filesystems

- Write:
  - Clean code
  - A [blog](https://tix3dev.github.io/apoptos-series-1), documenting learning process

## Dependencies

**NOTE: Depending on your Linux Distribution the package names might be different (supposing you are using Linux)**

- x86_64-elf-binutils
- x86_64-elf-gcc
- nasm-git
- make
- xorriso
- qemu

## Installation

- Install the dependencies
- Get the repository
  - `git clone https://github.com/Tix3Dev/apoptOS --recursive`
  - `cd apoptOS`
- Build it
  - `make all` (release build, for debug build use `make all_dbg`)
- Run it
  - `make run` (release QEMU version, for debug QEMU version use `make run_dbg`)

## Contributing

For those interested in contributing, please refer to the [contributing guidelines](https://github.com/Tix3Dev/apoptOS/blob/main/misc/CONTRIBUTING.md).
