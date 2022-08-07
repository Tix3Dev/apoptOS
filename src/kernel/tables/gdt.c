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
    Initialize global descriptor table and the task state segment.

*/

#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <tables/gdt.h>

extern uint8_t stack[16384];

extern void _gdt_load_asm(uint64_t gdt_ptr);
extern void _tss_load_asm(void);

static gdt_t gdt;
static gdtr_t gdtr;

/* core functions */

// set up segments of GDT and load the necessary ones
void gdt_init(void)
{
    // segment 0x00 - null descriptor segment
    gdt.entries[GDT_NULL_DESCRIPTOR].limit_low		    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].base_low		    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].base_middle	    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].access		    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].limit_high_and_flags   = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].base_high		    = 0;

    // segment 0x08 - kernel code segment
    gdt.entries[GDT_KERNEL_CODE].limit_low		= 0;
    gdt.entries[GDT_KERNEL_CODE].base_low		= 0;
    gdt.entries[GDT_KERNEL_CODE].base_middle		= 0;
    gdt.entries[GDT_KERNEL_CODE].access			= 0b10011010;
    gdt.entries[GDT_KERNEL_CODE].limit_high_and_flags	= 0b00100000;
    gdt.entries[GDT_KERNEL_CODE].base_high		= 0;

    // segment 0x10 - kernel data segment
    gdt.entries[GDT_KERNEL_DATA].limit_low		= 0;
    gdt.entries[GDT_KERNEL_DATA].base_low		= 0;
    gdt.entries[GDT_KERNEL_DATA].base_middle		= 0;
    gdt.entries[GDT_KERNEL_DATA].access			= 0b10010010;
    gdt.entries[GDT_KERNEL_DATA].limit_high_and_flags	= 0;
    gdt.entries[GDT_KERNEL_DATA].base_high		= 0;

    // segment 0x18 - user data segment
    gdt.entries[GDT_USER_DATA].limit_low		= 0;
    gdt.entries[GDT_USER_DATA].base_low			= 0;
    gdt.entries[GDT_USER_DATA].base_middle		= 0;
    gdt.entries[GDT_USER_DATA].access			= 0b11110010;
    gdt.entries[GDT_USER_DATA].limit_high_and_flags	= 0;
    gdt.entries[GDT_USER_DATA].base_high		= 0;

    // segment 0x20 - user code segment
    gdt.entries[GDT_USER_CODE].limit_low	    = 0;
    gdt.entries[GDT_USER_CODE].base_low		    = 0;
    gdt.entries[GDT_USER_CODE].base_middle	    = 0;
    gdt.entries[GDT_USER_CODE].access		    = 0b11111010;
    gdt.entries[GDT_USER_CODE].limit_high_and_flags = 0b00100000;
    gdt.entries[GDT_USER_CODE].base_high	    = 0;

    // segment 0x28 - tss segment
    // address passed is just 0 for now, SMP CPU startup will do it
    // properly with the right address and tss_load will be called
    tss_create_segment(0);

    gdtr.limit	= sizeof(gdt) - 1;
    gdtr.base	= (uint64_t)&gdt;

    gdt_load();

    log(INFO, "GDT initialized\n");
}

// assembly interface - load GDT via GDTR into calling CPU
void gdt_load(void)
{
    _gdt_load_asm((uintptr_t)&gdtr);
}

// assembly interface - load TSS into calling CPU
void tss_load(void)
{
    _tss_load_asm();
}

// create a TSS segment which can be loaded
void tss_create_segment(tss_t *tss)
{
    uintptr_t addr = (uintptr_t)tss;

    gdt.tss_descriptor.length		= 104;
    gdt.tss_descriptor.base_low    	= (uint16_t)addr;
    gdt.tss_descriptor.base_middle	= (uint8_t)(addr >> 16);
    gdt.tss_descriptor.flags1		= 0b10001001;
    gdt.tss_descriptor.flags2	    	= 0;
    gdt.tss_descriptor.base_high   	= (uint8_t)(addr >> 24);
    gdt.tss_descriptor.base_upper  	= (uint32_t)(addr >> 32);
    gdt.tss_descriptor.reserved    	= 0;
}
