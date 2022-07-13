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

#ifndef SDT_H
#define SDT_H

#include <stdint.h>

typedef struct __attribute__((__packed__))
{
    char	signature[4];
    uint32_t	length;
    uint8_t	revision;
    uint8_t	checksum;
    char	oem_string[6];
    char	oem_table_id[8];
    uint32_t	oem_revision;
    char	creator_id[4];
    uint32_t	creator_revision;
} sdt_t;

#endif
