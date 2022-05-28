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
#include <libk/serial/debug.h>
#include <libk/testing/assert.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/mem.h>


// TODO: add flags!!!


/* utility function prototypes */

bool slab_cache_grow(slab_cache_t *cache, size_t count);
void slab_cache_reap(void);
bool is_power_of_two(int num);
slab_bufctl_t *slab_create_bufctl(void);
void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl);
void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index);

/* core functions */

slab_cache_t *slab_cache_create(const char *name, size_t slab_size,
                       cache_ctor_t constructor, cache_dtor_t deconstructor)
{
    assert(slab_size <= 512); // only support small slab sizes (PAGE_SIZE / 8)
    assert(is_power_of_two(slab_size));

    slab_cache_t *cache = (slab_cache_t *)pmm_alloc(1);

    if (!cache)
	return NULL;

    cache->name = name;
    cache->slab_size = slab_size;
    cache->constructor = constructor;
    cache->deconstructor = deconstructor;

    cache->slabs = NULL;

    slab_cache_grow(cache, 3);

    return cache;
}

// void slab_cache_destroy(void)
// {
//     //
// }
// 
// void *slab_cache_alloc(slab_cache_t *cache)
// {
//     // check if cache exists
//     //
//     // check if slabs exist
//     //
//     // iterate over all slabs, check if freelist exists - if yes:
//     //	remove bufctl from freelist
//     //	return it's address
//     // if no:
//     //	continue
//     //
//     // grow cache if allowed (flags)
// }
// 
// void slab_cache_free(slab_cache_t *cache, void *pointer)
// {
//     // check if cache exists
//     //
//     // check if pointer exists
//     // 
//     // iterate over slabs, check if freelist exists - if yes:
//     //	add ptr to freelist
// }

void slab_cache_dump(slab_cache_t *cache)
{
    debug("Dump for cache with name '%s'\n", cache->name);

    cache->slabs = cache->slabs_head;

    for (int slab_count = 0;; slab_count++)
    {
	if (!cache->slabs)
	    break;

	cache->slabs->freelist = cache->slabs->freelist_head;

	debug("\tSlab no. %d is at %p\n", slab_count, cache->slabs);

	for (int bufctl_count = 0;; bufctl_count++)
	{
	    if (!cache->slabs->freelist)
		goto done;

	    debug("\t\tBufctl no. %d\t has pointer: %p\n", bufctl_count, cache->slabs->freelist);

	    cache->slabs->freelist = cache->slabs->freelist->next;
	}
done:
	cache->slabs = cache->slabs->next;
    }
}

/* utility functions */

// okay - flags instead of bool
bool slab_cache_grow(slab_cache_t *cache, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
	slab_bufctl_t *bufctl = slab_create_bufctl();

	if (!bufctl)
	    return false;
	
	slab_create_slab(cache, bufctl);
	
	size_t bufctl_count = (PAGE_SIZE - sizeof(slab_cache_t)) / cache->slab_size;

	for (size_t j = 0; j < bufctl_count; j++)
	    slab_init_bufctls(cache, bufctl, j);
    }

    return true;
}

// okay
slab_bufctl_t *slab_create_bufctl(void)
{
    slab_bufctl_t *bufctl = (slab_bufctl_t *)pmm_alloc(1);

    if (!bufctl)
	return NULL;

    bufctl->next = NULL;
    
    return bufctl;
}

// okay
void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl)
{
    slab_t *slab = (slab_t *)(((uintptr_t)bufctl + PAGE_SIZE) - sizeof(slab_t));

    slab->next = NULL;

    slab->freelist = NULL;

    if (!cache->slabs)
    {
	debug("Cache with name %s: First slab created - head set\n", cache->name);
	cache->slabs_head = slab;
	cache->slabs = slab;
    }
    else
    {
	cache->slabs->next = slab;
	cache->slabs = cache->slabs->next;
    }
}

// okay
void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index)
{
    slab_bufctl_t *new_bufctl = (slab_bufctl_t *)((uintptr_t)bufctl + cache->slab_size * index);
    new_bufctl->pointer = new_bufctl; // TODO

    if (!cache->slabs->freelist)
    {
	debug("Cache with name %s: First bufctl in current slab created - head set\n", cache->name);
	cache->slabs->freelist_head = new_bufctl;
	cache->slabs->freelist = new_bufctl;
    }
    else
    {
        cache->slabs->freelist->next = new_bufctl;
        cache->slabs->freelist = cache->slabs->freelist->next;
    }
}

void slab_cache_reap(void)
{
    //
}

bool is_power_of_two(int num)
{
    return (num > 0) && ((num & (num - 1)) == 0);
}
