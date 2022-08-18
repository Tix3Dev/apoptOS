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

/*

    Brief file description:
    Combine slab allocator with pmm allocator for custom sized allocations, making them
    optimized. There are 8 caches for sizes ranging from 4 to 512. 2 isn't used (as it's
    commonly found in other slab allocators), as metadata with size 2 will always be stored.
    Note that it might be better for specific tasks to use the neccessary allocators by hand.

*/

#include <boot/stivale2.h>
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/mem.h>

static slab_cache_t *slab_caches[8];

/* utility function prototypes */

size_t get_slab_cache_index(size_t size);

/* core functions */

// create caches that malloc will be able to use
void malloc_heap_init(void)
{
    slab_caches[0] = slab_cache_create("heap slab size 4", 4, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[1] = slab_cache_create("heap slab size 8", 8, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[2] = slab_cache_create("heap slab size 16", 16, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[3] = slab_cache_create("heap slab size 32", 32, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[4] = slab_cache_create("heap slab size 64", 64, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[5] = slab_cache_create("heap slab size 128", 128, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[6] = slab_cache_create("heap slab size 256", 256, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_caches[7] = slab_cache_create("heap slab size 512", 512, SLAB_PANIC | SLAB_AUTO_GROW);

    log(INFO, "Slab caches for heap initialized\n");
    log(INFO, "Heap fully initialized\n");
}

// allocate memory depending on the size, store metadata for free()
// return a vmm address
void *malloc(size_t size)
{
    void *pointer;

    if (size <= 512)
    {
        size_t new_size = size + sizeof(malloc_metadata_t);

        size_t index = get_slab_cache_index(new_size);
        pointer = slab_cache_alloc(slab_caches[index], SLAB_PANIC);

	if (!pointer)
	{
	    return NULL;
	}

        malloc_metadata_t *metadata = pointer;
        metadata->size = index;
    }
    else
    {
        size_t new_size = ALIGN_UP(size + sizeof(malloc_metadata_t), PAGE_SIZE);

        size_t page_count = new_size / PAGE_SIZE;
        pointer = pmm_allocz(page_count);

	if (!pointer)
	{
	    return NULL;
	}

        malloc_metadata_t *metadata = pointer;
        metadata->size = page_count;

    }

    return pointer + sizeof(malloc_metadata_t) + HEAP_START_ADDR;
}

void *realloc(void *pointer, size_t size)
{
    if (!pointer)
    {
	return malloc(size);
    }

    if (size == 0)
    {
	free(pointer);

	return NULL;
    }

    void *new_pointer = malloc(size);

    if (!new_pointer)
    {
	// `pointer` is not freed, as this is the responsibility of caller
	return NULL;
    }

    // TODO: implement memcpy
    memcpy(new_pointer, pointer, size);
    free(pointer);

    return new_pointer;
}

// free memory depending on the address, extract size from metadata
void free(void *pointer)
{
    if (!pointer)
    {
        return;
    }

    pointer = pointer - HEAP_START_ADDR - sizeof(malloc_metadata_t);

    if (((uint64_t)pointer & 0xFFF) != 0)
    {
        malloc_metadata_t *metadata = pointer;
        size_t index = metadata->size;

        slab_cache_free(slab_caches[index], metadata, SLAB_PANIC);
    }
    else
    {
        malloc_metadata_t *metadata = pointer;
        size_t page_count = metadata->size;

        pmm_free(metadata, page_count);
    }
}

/* utility functions */

// match arbitrary size to specific index for caches
// very efficient (three conditions only) algorithm, compromising
// looks of code
size_t get_slab_cache_index(size_t size)
{
    if (size <= 32)
    {
        if (size <= 8)
        {
            if (size <= 4)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            if (size <= 16)
            {
                return 2;
            }
            else
            {
                return 3;
            }
        }
    }
    else
    {
        if (size <= 128)
        {
            if (size <= 64)
            {
                return 4;
            }
            else
            {
                return 5;
            }
        }
        else
        {
            if (size <= 256)
            {
                return 6;
            }
            else
            {
                return 7;
            }
        }
    }
}
