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

/*

    Brief file description:
    Set up IDT with proper function address in it. Function addresses are
    stored in _isr_names_asm.
    When an interrupt occurs, the CPU will use the table by finding the
    address and calling it. This will result in the execution of isr_handler.

*/

#include <hardware/pic/pic.h>
#include <libk/serial/log.h>
#include <tables/idt.h>

#include <utility/utils.h>

extern void _load_idt_asm(uint64_t idtr);
extern uintptr_t _isr_vector_asm[];

static idt_descriptor_t idt[256];
static idtr_t idtr;

/* utility function prototypes */

void create_descriptor(uint8_t index, uint8_t type_attributes);

/* core functions */

// create and load IDT
void idt_init(void)
{
    uint16_t i = 0;

    // exceptions
    for (; i < 32; i++)
    {
        create_descriptor(i, INT_GATE);
    }

    // initialize PIC before setting up PIC lines
    pic_remap();

    // standard ISA IRQ's
    for (; i < 48; i++)
    {
        create_descriptor(i, INT_GATE);
    }

    // remaining IRQ's
    for (; i < 256; i++)
    {
        create_descriptor(i, INT_GATE);
    }

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;

    _load_idt_asm((uintptr_t)&idtr);

    asm volatile("sti"); // store interrupt flag -> allow hardware interrupts

    log(INFO, "IDT initialized\n");
}

/* utility functions */

// create an IDT descriptor
void create_descriptor(uint8_t index, uint8_t type_attributes)
{
    uint64_t offset = _isr_vector_asm[index]; // ISR handler address

    idt[index].offset_low	= offset & 0xFFFF;
    idt[index].selector		= 0x08; // kernel code segment
    idt[index].ist		= 0;
    idt[index].type_attributes	= type_attributes;
    idt[index].offset_middle	= (offset >> 16) & 0xFFFF;
    idt[index].offset_high	= (offset >> 32) & 0xFFFFFFFF;
    idt[index].zero		= 0;
}
