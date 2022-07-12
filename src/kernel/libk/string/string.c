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
    Custom implementations for string.h standard library.

*/

#include <libk/string/string.h>

/* core functions */

// fill a block of memory with a certain value
void *memset(void *pointer, uint32_t value, size_t size)
{
    uint8_t *buffer_pointer = (uint8_t *)pointer;

    for (size_t i = 0; i < size; i++)
        buffer_pointer[i] = value;

    return pointer;
}

// compare first n bytes at pointer1 with first n bytes at pointer2
// return value is based of fist non-matching character (e.g. abc aBc -> b/B) and
// < 0 when string1 is less than string2
// > 0 when string1 is greater than string2
// = 0 when string1 is equal to string2
int memcmp(const void *pointer1, const void *pointer2, size_t n)
{
    const unsigned char *ptr1 = pointer1;
    const unsigned char *ptr2 = pointer2;

    for (size_t i = 0; i < n; i++, ptr1++, ptr2++)
    {
        if (*ptr1 != *ptr2)
            return (*ptr1 - *ptr2);
    }

    return 0;
}
