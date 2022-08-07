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

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define INT_GATE 0x8E

typedef struct __attribute__((__packed__))
{
    uint16_t	offset_low;
    uint16_t	selector;
    uint8_t	ist;
    uint8_t	type_attributes;
    uint16_t	offset_middle;
    uint32_t	offset_high;
    uint32_t	zero;
} idt_descriptor_t;

typedef struct __attribute__((__packed__))
{
    uint16_t limit;
    uint64_t base;
} idtr_t;

void idt_init(void);
void idt_load(void);

#endif
