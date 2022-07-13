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

#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint8_t	*map;
    size_t	size;
} bitmap_t;

void bitmap_set_bit(bitmap_t *bitmap, int bit);
void bitmap_unset_bit(bitmap_t *bitmap, int bit);
uint8_t bitmap_check_bit(bitmap_t *bitmap, int bit);

#endif
