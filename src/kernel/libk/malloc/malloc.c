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
#include <libk/string/string.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/mem.h>





#include <libk/serial/debug.h>




static slab_cache_t *slab_caches[8];

/* utility function prototypes */

size_t round_alloc_size(size_t size);
uint32_t next_power_of_two(uint32_t n);
int64_t size_to_slab_cache_index(size_t size);
int64_t slab_cache_index_to_size(size_t index);

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

    // log(WARNING, "realloc test\n");

    // debug("size to index(%d): %d\n", -23, size_to_slab_cache_index(-23));
    // debug("size to index(%d): %d\n", 0, size_to_slab_cache_index(0));
    // debug("size to index(%d): %d\n", 4, size_to_slab_cache_index(4));
    // debug("size to index(%d): %d\n", 5, size_to_slab_cache_index(5));
    // debug("size to index(%d): %d\n", 16, size_to_slab_cache_index(16));
    // debug("size to index(%d): %d\n", 32, size_to_slab_cache_index(32));
    // debug("size to index(%d): %d\n", 64, size_to_slab_cache_index(64));
    // debug("size to index(%d): %d\n", 128, size_to_slab_cache_index(128));
    // debug("size to index(%d): %d\n", 129, size_to_slab_cache_index(129));
    // debug("size to index(%d): %d\n", 512, size_to_slab_cache_index(512));
    // 
    // for (int i = -3; i < 10; i++)
    // {
    //     debug("index to size(%d): %d\n", i, slab_cache_index_to_size(i));
    // }

    // debug("round_alloc_size(%d): %d\n", 3, round_alloc_size(3));

    // for (int i = 0; i < 130; i++)
    // {
    //     debug("round_alloc_size(%d): %d\n", i, round_alloc_size(i));
    // }
    // 
    // for (int i = 0; i < PAGE_SIZE * 100; i += 23)
    // {
    //     debug("round_alloc_size(%d): %x\n", i, round_alloc_size(i));
    // }
}

// allocate memory depending on the size, store metadata for realloc() or free()
// return a vmm address - not guaranteed that everything set to zero
void *malloc(size_t size)
{
    size_t new_size = round_alloc_size(size);
    void *pointer;

    if (size <= 512)
    {
        int64_t index = size_to_slab_cache_index(new_size);

	if (index == -1)
	{
	    return NULL;
	}

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
        int64_t page_count = new_size / PAGE_SIZE;

	if (page_count == -1)
	{
	    return NULL;
	}

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

void *realloc(void *old_pointer, size_t new_size)
{
    if (!old_pointer)
    {
	return malloc(new_size);
    }

    if (!new_size)
    {
	free(old_pointer);

	return NULL;
    }

    new_size = round_alloc_size(new_size);
    size_t old_size = 0;

    if (((uint64_t)old_pointer & 0xFFF) != 0)
    {
        malloc_metadata_t *metadata = old_pointer;
        size_t index = metadata->size;

	int64_t old_size_temp = slab_cache_index_to_size(index);

	if (old_size_temp == -1)
	{
	    return NULL;
	}

	old_size = (size_t)old_size_temp;
    }
    else
    {
        malloc_metadata_t *metadata = old_pointer;
        size_t page_count = metadata->size;

	old_size = page_count / PAGE_SIZE;
    }

    if (old_size == new_size)
    {
	return old_pointer;
    }

    void *new_pointer = malloc(new_size);

    if (!new_pointer)
    {
	return NULL;
    }

    if (old_size > new_size)
    {
	memcpy(new_pointer, old_pointer, new_size);
    }
    else
    {
	memcpy(new_pointer, old_pointer, old_size);
    }

    free(old_pointer);

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

// round size so that it can be used properly without
// having to care about rounding
size_t round_alloc_size(size_t size)
{
    if (size <= 512)
    {
	return next_power_of_two(size);
    }
    else
    {
	return ALIGN_UP(size, PAGE_SIZE);
    }
}

// round number to next biggest power of two
uint32_t next_power_of_two(uint32_t n)
{
    if (n < 2)
    {
	return 2;
    }

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

// match size to specific index for caches
int64_t size_to_slab_cache_index(size_t size)
{
    switch (size)
    {
	case 4:
	    return 0;
	
	case 8:
	    return 1;

	case 16:
	    return 2;

	case 32:
	    return 3;

	case 64:
	    return 4;
	
	case 128:
	    return 5;
	
	case 256:
	    return 6;
	
	case 512:
	    return 7;
	
	default:
	    return -1;
    }
}

// math cache index to specific size
int64_t slab_cache_index_to_size(size_t index)
{
    switch (index)
    {
	case 0:
	    return 4;
	
	case 1:
	    return 8;

	case 2:
	    return 16;

	case 3:
	    return 32;

	case 4:
	    return 64;
	
	case 5:
	    return 128;
	
	case 6:
	    return 256;
	
	case 7:
	    return 512;
	
	default:
	    return -1;
    }
}
