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

#ifndef BITMAP_H
#define BITMAP_H

#define BIT_TO_PAGE(bit)    ((size_t)bit * 0x1000)
#define PAGE_TO_BIT(page)   ((size_t)page / 0x1000)

typedef struct
{
    uint8_t	*map;
    size_t	size;
} BITMAP_t;

void bitmap_set_bit(BITMAP_t *bitmap, int bit);
void bitmap_unset_bit(BITMAP_t *bitmap, int bit);
uint8_t bitmap_check_bit(BITMAP_t *bitmap, int bit);

#endif
