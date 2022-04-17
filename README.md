<div align="center">

<h3>apoptOS</h3>
<p>A modern x86_64 UNIX-like microkernel written in C and Assembly</p>

<img src="https://github.com/Tix3Dev/apoptOS/tree/main/misc/screenshots/logs.svg"> </a>
</div>

## Features

- Limine bootloader
- Stivale2 boot protocol

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
  - A [blog](), documenting learning process

## Dependencies

**NOTE: Depending on your Linux Distribution the package names might be different (supposing you are using Linux)**

- 

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
