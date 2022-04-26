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
    Initialize global descriptor table and the task state segment.

*/

#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <tables/gdt.h>

extern uint8_t stack[16384];

static tss_t tss;
static gdt_t gdt;
static gdt_pointer_t gdt_pointer;

/* utility function prototypes */

extern void _load_gdt_and_tss_asm(uint64_t gdt_ptr);

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
    gdt.tss_descriptor.length		= 104;
    gdt.tss_descriptor.base_low    	= 0;
    gdt.tss_descriptor.base_middle	= 0;
    gdt.tss_descriptor.flags1		= 0b10001001;
    gdt.tss_descriptor.flags2	    	= 0;
    gdt.tss_descriptor.base_high   	= 0;
    gdt.tss_descriptor.base_upper  	= 0;
    gdt.tss_descriptor.reserved    	= 0;

    memset(&tss, 0, sizeof(tss));

    tss.rsp[0]	    = (uintptr_t)stack + sizeof(stack);
    tss.iopb_offset = sizeof(tss);

    gdt_pointer.limit    = sizeof(gdt) - 1;
    gdt_pointer.base	    = (uint64_t)&gdt;

    _load_gdt_and_tss_asm((uintptr_t)&gdt_pointer);
    
    log(INFO, "GDT initialized\n");
}
