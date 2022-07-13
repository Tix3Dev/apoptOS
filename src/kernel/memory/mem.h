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

#ifndef MEM_H
#define MEM_H

#include <stdbool.h>

#define HIGHER_HALF_DATA	0xFFFF800000000000UL
#define HIGHER_HALF_CODE	0xFFFFFFFF80000000UL
#define HIGHER_HALF_END		0xFFFFFFFFFFFFFFFFUL

#define GiB 0x40000000UL

#define HEAP_MAX_SIZE	(4 * GiB)
#define HEAP_START_ADDR	0xFFFF900000000000

#define PAGE_SIZE 4096

#define KB_TO_PAGES(kb)		    (((kb) * 1024) / PAGE_SIZE)
#define ALIGN_DOWN(address, align)  ((address) & ~((align)-1))
#define ALIGN_UP(address, align)    (((address) + (align)-1) & ~((align)-1))

#define BIT_TO_PAGE(bit)    ((size_t)bit * PAGE_SIZE)
#define PAGE_TO_BIT(page)   ((size_t)page / PAGE_SIZE)

#define PHYS_TO_HIGHER_HALF_DATA(address)   (address + HIGHER_HALF_DATA)
#define PHYS_TO_HIGHER_HALF_CODE(address)   (address + HIGHER_HALF_CODE)
#define HIGHER_HALF_DATA_TO_PHYS(address)   (address - HIGHER_HALF_DATA)
#define HIGHER_HALF_CODE_TO_PHYS(address)   (address - HIGHER_HALF_CODE)

#endif
