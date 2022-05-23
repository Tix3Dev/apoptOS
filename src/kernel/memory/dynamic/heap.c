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
    blah

*/

#include <boot/stivale2.h>
#include <libk/serial/log.h>
#include <memory/dynamic/heap.h>
#include <memory/virtual/vmm.h>
#include <memory/mem.h>

/* core functions */

void heap_init(void)
{
    vmm_map_range(vmm_get_root_page_table(), 0, HEAP_ADDR_END, HEAP_ADDR_START, KERNEL_READ_WRITE);

    log(INFO, "Initialized heap virtual memory region\n");

    // create system slab
    
    log(INFO, "Initialized general purpose system slab cache\n");

    log(INFO, "Initialized heap\n");
}

void *heap_alloc(size_t size)
{
    //
}

void heap_free(void *pointer)
{
    //
}
