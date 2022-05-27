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
slab_t *slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl);

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

    slab_cache_grow(cache, 1);

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
    debug("slab cache dump for '%s'\n", cache->name);

    slab_t *head = cache->slabs;

    for (;;)
    {
	if (!head)
	    break;

	for (;;)
	{
	    if (!head->freelist)
		continue;

	    debug("slab ptr: %p | bufctl ptr: %p\n", head->freelist);

	    head->freelist = head->freelist->next;
	}

	head = head->next;
    }
}

/* utility functions */

bool slab_cache_grow(slab_cache_t *cache, size_t count)
{
    // allocate whole page for bufctl
    //
    // put slab at end of allocated bufctl (ptr + PAGE_SIZE - sizeof slab)
    //
    // bufctl_count = (PAGE_SIZE-1) / cache->slab_size
    //
    // for i in bufctl_count
    //	create_bufctl
    //
    // insert slab into list
    //
    // create_bufctl()
    //	insert bufctl into list


    for (size_t i = 0; i < count; i++)
    {
	slab_bufctl_t *bufctl = slab_create_bufctl();
	
	slab_t *slab = slab_create_slab(cache, bufctl);
	
	size_t bufctl_count = (PAGE_SIZE - sizeof(slab_cache_t)) / cache->slab_size;

	for (size_t j = 0; j < bufctl_count; j++)
	    slab_init_bufctls(cache, bufctl, j);
    }
}

slab_bufctl_t *slab_create_bufctl(void)
{
    slab_bufctl_t *bufctl = (slab_bufctl_t *)pmm_alloc(1);

    if (!bufctl)
	return false;

    bufctl->next = NULL;
    
    return bufctl;
}

slab_t *slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl)
{
    slab_t *slab = (slab_t *)(((uintptr_t)bufctl - PAGE_SIZE) - sizeof(slab_t));

    slab->next = NULL;

    slab->freelist = bufctl;

    if (!cache->slabs)
	cache->slabs = slab;
    else
	cache->slabs->next = slab;
}

void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index)
{
    slab_bufctl_t *new_bufctl = (uintptr_t)bufctl + cache->size * i;
    new_bufctl->pointer = *new_bufctl; // TODO

    if (!cache->slabs->freelist)
	cache->slabs->freelist = new_bufctl;
    // else


}

// void slab_cache_reap(void)
// {
//     //
// }

bool is_power_of_two(int num)
{
    return (num > 0) && ((num & (num - 1)) == 0);
}
