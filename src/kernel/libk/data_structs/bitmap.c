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
    Interaction with the custom bitmap data structure.

*/

#include <libk/data_structs/bitmap.h>

/* core functions */

// set exactly one bit to 1 in the bitmap
void bitmap_set_bit(bitmap_t *bitmap, int bit)
{
    bitmap->map[bit / 8] |= (1 << (bit % 8));
}

// set exactly one bit to 0 in the bitmap
void bitmap_unset_bit(bitmap_t *bitmap, int bit)
{
    bitmap->map[bit / 8] &= ~(1 << (bit % 8));
}

// return specific bit in bitmap (either 0 or 1)
uint8_t bitmap_check_bit(bitmap_t *bitmap, int bit)
{
    return bitmap->map[bit / 8] & (1 << (bit % 8));
}
