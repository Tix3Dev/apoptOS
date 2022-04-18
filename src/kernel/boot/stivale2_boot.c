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

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>

static uint8_t stack[4096];

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr =
{
    .entry_point    = 0,
    .stack	    = (uintptr_t)stack + sizeof(stack),
    .flags	    = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags	    = 0
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;

    for (;;)
    {
        if (current_tag == NULL)
            return NULL;

        if (current_tag->identifier == id)
            return current_tag;

        current_tag = (void *)current_tag->next;
    }
}
