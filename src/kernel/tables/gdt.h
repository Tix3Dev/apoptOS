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

#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_NULL_DESCRIPTOR 0
#define GDT_KERNEL_CODE	    1
#define GDT_KERNEL_DATA	    2
#define GDT_USER_DATA	    3
#define GDT_USER_CODE	    4

#define GDT_ENTRY_COUNT	    5

typedef struct __attribute__((__packed__))
{
    uint32_t	reserved0;
    uint64_t	rsp[3];
    uint64_t	reserved1;
    uint64_t	ist[7];
    uint32_t	reserved2;
    uint32_t	reserved3;
    uint16_t	reserved4;
    uint16_t	iopb_offset;
} tss_t;


typedef struct __attribute__((__packed__))
{
    uint16_t	limit_low;		// bits 0-15
    uint16_t	base_low;	    	// bits 0-15
    uint8_t	base_middle;	    	// bits 16-23
    uint8_t	access;		    	// access
    uint8_t	limit_high_and_flags;	// bits 16-19 and flags
    uint8_t	base_high;		// bits 24-31
} gdt_descriptor_t;

typedef struct __attribute__((__packed__))
{
    uint16_t	length;		// bits 0-15 
    uint16_t	base_low;	// bits 0-15
    uint8_t	base_middle;	// bits 16-23
    uint8_t	flags1;		// flags
    uint8_t	flags2;	    	// flags 
    uint8_t	base_high;  	// bits 24-31
    uint32_t	base_upper; 	// bits 32-63
    uint32_t	reserved;   	// reserved
} tss_descriptor_t;

typedef struct __attribute__((__packed__))
{
    gdt_descriptor_t entries[GDT_ENTRY_COUNT];
    tss_descriptor_t tss_descriptor;
} gdt_t;


typedef struct __attribute__((__packed__))
{
    uint16_t limit; // equivalent to size
    uint64_t base;  // equivalent to address
} gdt_pointer_t;

void gdt_init();

#endif
