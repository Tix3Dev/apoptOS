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

#include <stdint.h>

#ifndef TSS_H
#define TSS_H

typedef struct __attribute__((__packed__))
{
    uint16_t	length;	    // bits 0-15 
    uint16_t	base_low;   // bits 0-15
    uint8_t	base_mid;   // bits 16-23
    uint8_t	flags1;	    // flags
    uint8_t	flags2;	    // flags 
    uint8_t	base_high;  // bits 24-31
    uint32_t	base_upper; // bits 32-63
    uint32_t	reserved;   // reserved
} tss_entry_t;

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

void tss_init(void);
tss_t *tss_get(void);

#endif
