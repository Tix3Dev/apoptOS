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
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <libk/testing/assert.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/mem.h>


// TODO: add flags!!!


/* utility function prototypes */

slab_bufctl_t *slab_create_bufctl(void);
void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl);
void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index);
bool is_power_of_two(int num);

/* core functions */

slab_cache_t *slab_cache_create(const char *name, size_t slab_size, slab_flags_t flags)
{
    assert(slab_size <= 512); // only support small slab sizes (PAGE_SIZE / 8)
    assert(is_power_of_two(slab_size));

    slab_cache_t *cache = (slab_cache_t *)pmm_alloc(1);

    if (!cache && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache create ('%s'): Couldn't allocate memory\n", name);

    if (!cache)
	return NULL;

    cache->name = name;
    cache->slab_size = slab_size;
    cache->bufctl_count_max = (PAGE_SIZE - sizeof(slab_cache_t)) / cache->slab_size;

    cache->slabs = NULL;

    slab_cache_grow(cache, 1, flags);

    return cache;
}

void slab_cache_destroy(slab_cache_t *cache, slab_flags_t flags)
{
    if (!cache && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache destroy (name missing): Cache doesn't exist\n");

    if (!cache)
	return;

    cache->slabs = cache->slabs_head;

    for (;;)
    {
	if (!cache->slabs)
	    break;

	if ((cache->slabs->bufctl_count != cache->bufctl_count_max) && (flags & SLAB_PANIC))
	    log(PANIC, "Slab cache destroy ('%s'): A slab wasn't compeltely free\n", cache->name);

	if (cache->slabs->bufctl_count != cache->bufctl_count_max)
	    return;

	pmm_free((void *)cache->slabs->freelist_head, 1);

	cache->slabs = cache->slabs->next;
    }

    memset(cache, 0, sizeof(slab_cache_t));
    pmm_free((void *)cache, 1);
}

void slab_cache_grow(slab_cache_t *cache, size_t count, slab_flags_t flags)
{
    if (!cache && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache grow (name missing): Cache doesn't exist\n");

    if (!cache)
	return;

    cache->slabs = cache->slabs_head;

    for (size_t i = 0; i < count; i++)
    {
        slab_bufctl_t *bufctl = slab_create_bufctl();

        if (!bufctl && (flags & SLAB_PANIC))
	    log(PANIC, "Slab cache grow ('%s'): Couldn't create bufctl\n", cache->name);

        if (!bufctl)
            return;
        
        slab_create_slab(cache, bufctl);
        
        for (size_t j = 0; j < cache->bufctl_count_max; j++)
            slab_init_bufctls(cache, bufctl, j);
    }
}

void slab_cache_reap(slab_cache_t *cache, slab_flags_t flags)
{
    if (!cache && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache reap (name missing): Cache doesn't exist\n");

    if (!cache)
	return;

    cache->slabs = cache->slabs_head;

    slab_t *prev = cache->slabs_head;

    for (;;)
    {
	if (!cache->slabs)
	{
	    cache->slabs = cache->slabs_head;
	    return;
	}

	if (cache->slabs->bufctl_count == cache->bufctl_count_max)
	{
	    if (cache->slabs == cache->slabs_head)
		cache->slabs_head = cache->slabs->next;

	    prev->next = cache->slabs->next;

	    pmm_free((void *)cache->slabs, 1);

	    cache->slabs = prev->next;

	    continue;
	}

	prev = cache->slabs;
	cache->slabs = cache->slabs->next;
    }
}

void *slab_cache_alloc(slab_cache_t *cache, slab_flags_t flags)
{
    if (!cache && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache alloc (name missing): Cache doesn't exist\n");

    if (!cache)
	return NULL;
    
    cache->slabs = cache->slabs_head;

    for (;;)
    {
	if (!cache->slabs && (flags & SLAB_AUTO_GROW))
	{
	    slab_cache_grow(cache, 1, flags);

	    return slab_cache_alloc(cache, flags);
	}

	if (!cache->slabs && (flags & SLAB_PANIC))
	    log(PANIC, "Slab cache alloc ('%s'): Couldn't find allocatable memory\n", cache->name);

	if (!cache->slabs)
	    return NULL;

	if (cache->slabs->freelist_head)
	    break;

	cache->slabs = cache->slabs->next;
    }

    void *pointer = cache->slabs->freelist_head;

    cache->slabs->freelist_head = cache->slabs->freelist_head->next;
    cache->slabs->bufctl_count--;

    return pointer;
}

void slab_cache_free(slab_cache_t *cache, void *pointer, slab_flags_t flags)
{
    if (!cache && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache free (name missing): Cache doesn't exist\n");

    if (!cache)
	return;

    cache->slabs = cache->slabs_head;

    for (;;)
    {
	if (!cache->slabs && (flags & SLAB_PANIC))
	    log(PANIC, "Slab cache free ('%s'): Couldn't find a slab for the to be freed pointer\n", cache->name);

	if (!cache->slabs)
	    return;

	if (cache->slabs->bufctl_count < cache->bufctl_count_max)
	    break;
	
	cache->slabs = cache->slabs->next;
    }

    slab_bufctl_t *new_bufctl = (slab_bufctl_t *)pointer;

    new_bufctl->next = cache->slabs->freelist_head;
    new_bufctl->pointer = (void *)new_bufctl; // just for sanity (would work without)

    cache->slabs->freelist_head = new_bufctl;
    cache->slabs->bufctl_count++;
}

void slab_cache_dump(slab_cache_t *cache, slab_flags_t flags)
{
    if (!cache->slabs && (flags & SLAB_PANIC))
	log(PANIC, "Slab cache dump (name missing): Slabs don't exist\n");

    if (!cache->slabs)
	return;

    debug("Dump for cache with name '%s'\n", cache->name);

    cache->slabs = cache->slabs_head;

    for (int slab_count = 0;; slab_count++)
    {
	if (!cache->slabs)
	    break;

	cache->slabs->freelist = cache->slabs->freelist_head;

	debug("\tSlab no. %d is at 0x%p\n", slab_count, cache->slabs);

	for (int bufctl_count = 0;; bufctl_count++)
	{
	    if (!cache->slabs->freelist)
		goto done;

	    // debug("\t\tBufctl no. %d\t at \t0x%p\n", bufctl_count, cache->slabs->freelist);
	    // debug("\t\t\tHas pointer: \t0x%p\n", cache->slabs->freelist->pointer);
	    debug("\t\tBufctl no. %d\t has pointer 0x%p\n", bufctl_count, cache->slabs->freelist->pointer);

	    cache->slabs->freelist = cache->slabs->freelist->next;
	}
done:
	cache->slabs = cache->slabs->next;
    }
}

/* utility functions */

slab_bufctl_t *slab_create_bufctl(void)
{
    slab_bufctl_t *bufctl = (slab_bufctl_t *)pmm_alloc(1);

    if (!bufctl)
	return NULL;

    bufctl->next = NULL;
    
    return bufctl;
}

void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl)
{
    slab_t *slab = (slab_t *)(((uintptr_t)bufctl + PAGE_SIZE) - sizeof(slab_t));

    slab->next = NULL;

    slab->bufctl_count = cache->bufctl_count_max;

    slab->freelist_head = NULL;
    slab->freelist = NULL;

    if (!cache->slabs)
    {
	debug("\nomg first time\n\n");
	cache->slabs_head = slab;
	cache->slabs = slab;
    }
    else
    {
	cache->slabs->next = slab;
	cache->slabs = cache->slabs->next;
    }
}

void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index)
{
    slab_bufctl_t *new_bufctl = (slab_bufctl_t *)((uintptr_t)bufctl + cache->slab_size * index);
    new_bufctl->pointer = new_bufctl;

    if (!cache->slabs->freelist)
    {
	cache->slabs->freelist_head = new_bufctl;
	cache->slabs->freelist = new_bufctl;
    }
    else
    {
        cache->slabs->freelist->next = new_bufctl;
        cache->slabs->freelist = cache->slabs->freelist->next;
    }
}

bool is_power_of_two(int num)
{
    return (num > 0) && ((num & (num - 1)) == 0);
}
