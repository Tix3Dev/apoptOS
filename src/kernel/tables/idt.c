/*
	This file is part of a modern x86_64 UNIX-like microkernel which is called apoptOS
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

/*

    Brief file description:
    Set up IDT with proper function address in it. Function addresses are
    stored in _isr_names_asm.
    When an interrupt occurs, the CPU will use the table by finding the
    address and calling it. This will result in the execution of isr_handler.

*/

#include <libk/serial/log.h>
#include <tables/idt.h>

#include <assembly/utils.h>

extern void _load_idt_asm(uint64_t idt_ptr);
extern uintptr_t _isr_names_asm[];

static idt_descriptor_t idt[256];
static idt_pointer_t idt_pointer;

/* utility function prototypes */

void create_descriptor(uint8_t index, uint8_t type_attributes);

/* core functions */

// create and load IDT
void idt_init(void)
{
    // 32 exceptions
    for (uint8_t i = 0; i < 32; i++)
	create_descriptor(i, INT_GATE);

    // pic_remap();

    // 16 standard ISA IRQ's
    for (uint8_t i = 32; i < 48; i++)
	create_descriptor(i, INT_GATE);
    
    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.base = (uint64_t)&idt;

    log(WARNING, "idtptr limit: %p\n", idt_pointer.limit);
    log(WARNING, "idtptr base: %p\n", idt_pointer.base);
    _load_idt_asm((uintptr_t)&idt_pointer);

    asm volatile("sti"); // store interrupt flag -> allow hardware interrupts

    log(INFO, "IDT initialized\n");
}

/* utility functions */

// create an IDT descriptor
void create_descriptor(uint8_t index, uint8_t type_attributes)
{
    uint64_t offset = _isr_names_asm[index];

    idt[index].offset_low	= offset & 0xFFFF;
    idt[index].selector		= 0x08; // kernel code segment
    idt[index].ist		= 0;
    idt[index].type_attributes	= type_attributes;
    idt[index].offset_middle	= (offset >> 16) & 0xFFFF;
    idt[index].offset_high	= (offset >> 32) & 0xFFFFFFFF;
    idt[index].zero		= 0;
}
