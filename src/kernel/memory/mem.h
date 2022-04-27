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

#include <stdbool.h>

#include <assembly/utils.h>

#ifndef MEM_H
#define MEM_H

#define HIGHER_HALF_DATA_LV5	0xFF00000000000000UL
#define HIGHER_HALF_DATA_LV4	0xFFFF800000000000UL
#define HIGHER_HALF_CODE	0xFFFFFFFF80000000UL

#define PAGE_SIZE		    4096
#define TABLES_PER_DIRECTORY	    512
#define PAGES_PER_TABLE		    512	// TODO: do I need this???

#define KB_TO_PAGES(kb)		    (((kb) * 1024) / PAGE_SIZE)
#define ALIGN_DOWN(addr, align)	    ((addr) & ~((align)-1))
#define ALIGN_UP(addr, align)	    (((addr) + (align)-1) & ~((align)-1))

#define IS_PAGE_ALIGNED(num)	    ((num % PAGE_SIZE) == 0)

#define BIT_TO_PAGE(bit)    ((size_t)bit * PAGE_SIZE)
#define PAGE_TO_BIT(page)   ((size_t)page / PAGE_SIZE)

// get cr4 and return la57 = bit 12
static inline bool is_la57_enabled(void)
{
    uint64_t cr4 = asm_read_cr(4);

    return (cr4 >> 12) & 1;
}

/* those functions are important, as KnutOS is a higher half kernel */

static inline uintptr_t phys_to_higher_half_data(uintptr_t address)
{
    if (is_la57_enabled())
	return HIGHER_HALF_DATA_LV5 + address;

    return HIGHER_HALF_DATA_LV4 + address;
}

static inline uintptr_t phys_to_higher_half_code(uintptr_t address)
{
    return HIGHER_HALF_CODE + address;
}

static inline uintptr_t higher_half_data_to_phys(uintptr_t address)
{
    if (is_la57_enabled())
	return address - HIGHER_HALF_DATA_LV5;

    return address - HIGHER_HALF_DATA_LV4;
}

static inline uintptr_t higher_half_code_to_phys(uintptr_t address)
{
    return address - HIGHER_HALF_CODE;
}

#endif
