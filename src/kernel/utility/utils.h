/*
	This file is part of a modern x86_64 UNIX-like microkernel-based
	operating system which is called apoptOS
	Everything is openly developed on GitHub: https://github.com/Tix3Dev/apoptOS

	Copyright (C) 2022  Yves Vollmeier <https://github.com/Tix3Dev>
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>

// read the value a custom CR register contains
#define asm_read_cr(reg)				    \
({							    \
    uint64_t value = 0;					    \
    asm volatile("mov %%cr" #reg ", %0" : "=r" (value));    \
    value;						    \
})

// write a value to a custom CR register
#define asm_write_cr(reg, value)			\
({							\
    asm volatile("mov %0, %%cr" #reg : : "r" (value));	\
})

// read from a model-specific register
static inline uint64_t asm_rdmsr(uint32_t msr) {
    uint64_t rax;
    uint64_t rdx;
    asm volatile ("rdmsr" : "=a"(rax), "=d"(rdx) : "c"(msr) : "memory");
    return (rdx << 32) | rax;
}

// write to a model-specific register
static inline void asm_wrmsr(uint32_t msr, uint64_t value) {
    uint64_t rax = (uint32_t)value;
    uint64_t rdx = value >> 32;
    asm volatile ("wrmsr" :: "a"(rax), "d"(rdx), "c"(msr));
}

// send data to a IO port
static inline void asm_io_outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// get data from a IO port
static inline uint8_t asm_io_inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// wait for one IO cycle
static inline void asm_io_wait(void)
{
    asm_io_inb(0x80);
}

// load an address into the translation lookaside buffer
static inline void asm_invlpg(uint64_t *address)
{
    asm volatile("invlpg (%0)" : : "r" (address));
}

// get the state (sti=1 and cli=0) of the interrupt flag in rflags
static inline bool asm_get_interrupt_flag() {
    uint64_t rflags = 0;
    asm volatile ("pushfq; pop %0" : "=r"(rflags));
    return (rflags >> 9) & 1;
}

#endif
