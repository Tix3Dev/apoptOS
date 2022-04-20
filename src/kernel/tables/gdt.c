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
    Initialize global descriptor table and it's task state segment entry.

*/

#include <libk/serial/log.h>
#include <tables/gdt.h>

static gdt_t gdt;
static gdt_descriptor_t gdt_descriptor;

/* utility function prototypes */

extern void _load_gdt_and_tss_asm(uint64_t gdt_descriptor_ptr);

/* core functions */

// set up segments of GDT and load the necessary ones
void gdt_init(void)
{
    // segment 0x00 - null descriptor
    gdt.entries[GDT_NULL_DESCRIPTOR].limit_low		    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].base_low		    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].base_middle	    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].access		    = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].limit_high_and_flags   = 0;
    gdt.entries[GDT_NULL_DESCRIPTOR].base_high		    = 0;

    // segment 0x08 - kernel code
    gdt.entries[GDT_KERNEL_CODE].limit_low		= 0;
    gdt.entries[GDT_KERNEL_CODE].base_low		= 0;
    gdt.entries[GDT_KERNEL_CODE].base_middle		= 0;
    gdt.entries[GDT_KERNEL_CODE].access			= 0b10011010;
    gdt.entries[GDT_KERNEL_CODE].limit_high_and_flags	= 0b00100000;
    gdt.entries[GDT_KERNEL_CODE].base_high		= 0;

    // segment 0x10 - kernel data
    gdt.entries[GDT_KERNEL_DATA].limit_low		= 0;
    gdt.entries[GDT_KERNEL_DATA].base_low		= 0;
    gdt.entries[GDT_KERNEL_DATA].base_middle		= 0;
    gdt.entries[GDT_KERNEL_DATA].access			= 0b10010010;
    gdt.entries[GDT_KERNEL_DATA].limit_high_and_flags	= 0;
    gdt.entries[GDT_KERNEL_DATA].base_high		= 0;

    // segment 0x18 - user data
    gdt.entries[GDT_USER_DATA].limit_low		= 0;
    gdt.entries[GDT_USER_DATA].base_low			= 0;
    gdt.entries[GDT_USER_DATA].base_middle		= 0;
    gdt.entries[GDT_USER_DATA].access			= 0b11110010;
    gdt.entries[GDT_USER_DATA].limit_high_and_flags	= 0;
    gdt.entries[GDT_USER_DATA].base_high		= 0;

    // segment 0x20 - user code
    gdt.entries[GDT_USER_CODE].limit_low	    = 0;
    gdt.entries[GDT_USER_CODE].base_low		    = 0;
    gdt.entries[GDT_USER_CODE].base_middle	    = 0;
    gdt.entries[GDT_USER_CODE].access		    = 0b11111010;
    gdt.entries[GDT_USER_CODE].limit_high_and_flags = 0b00100000;
    gdt.entries[GDT_USER_CODE].base_high	    = 0;

    // segment 0x28 - tss entry
    gdt.gdt_tss.length	    = 104;
    gdt.gdt_tss.base_low    = 0;
    gdt.gdt_tss.base_mid    = 0;
    gdt.gdt_tss.flags1	    = 0b10001001;
    gdt.gdt_tss.flags2	    = 0;
    gdt.gdt_tss.base_high   = 0;
    gdt.gdt_tss.base_upper  = 0;
    gdt.gdt_tss.reserved    = 0;

    gdt_descriptor.limit    = sizeof(gdt) - 1;
    gdt_descriptor.base	    = (uint64_t)&gdt;

    _load_gdt_and_tss_asm((uintptr_t)&gdt_descriptor);
    
    log(INFO, "GDT initialized\n");
}

// return pointer to GDT for external use
gdt_t *gdt_get(void)
{
    return &gdt;
}

// return pointer to GDT descriptor for external use
gdt_descriptor_t *gdt_descriptor_get(void)
{
    return &gdt_descriptor;
}
