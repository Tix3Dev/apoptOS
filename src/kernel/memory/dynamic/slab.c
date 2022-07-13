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
    Memory allocator mainly intended for heap memory management.
    Based on some of the principles of the slab allocator, e.g. implemented by
    Jeff Bonwick (https://people.eecs.berkeley.edu/~kubitron/courses/cs194-24-S14/hand-outs/bonwick_slab.pdf).
    Though this allocator does only work for small slab sizes (<= 512), as this is the best for keeping
    the same slab layout for every size. It also doesn't make use of slab states (free, used and partial)
    for the sake of simplicity.

*/

#include <boot/stivale2.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <libk/testing/assert.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/mem.h>

/* utility function prototypes */

slab_bufctl_t *slab_create_bufctl_buffer(void);
void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl);
void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index);
bool is_power_of_two(int num);

/* core functions */

// create a cache and grow one slab
slab_cache_t *slab_cache_create(const char *name, size_t slab_size, slab_flags_t flags)
{
    assert(slab_size <= 512); // only support small slab sizes (PAGE_SIZE / 8)
    assert(is_power_of_two(slab_size));

    slab_cache_t *cache = (slab_cache_t *)pmm_allocz(1);

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

// try to delete all slabs in a cache + delete cache itself
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

// allocate one page, put bufctls + slab into it (per count)
void slab_cache_grow(slab_cache_t *cache, size_t count, slab_flags_t flags)
{
    if (!cache && (flags & SLAB_PANIC))
        log(PANIC, "Slab cache grow (name missing): Cache doesn't exist\n");

    if (!cache)
        return;

    cache->slabs = cache->slabs_head;

    for (size_t i = 0; i < count; i++)
    {
        slab_bufctl_t *bufctl = slab_create_bufctl_buffer();

        if (!bufctl && (flags & SLAB_PANIC))
            log(PANIC, "Slab cache grow ('%s'): Couldn't create bufctl\n", cache->name);

        if (!bufctl)
            return;

        slab_create_slab(cache, bufctl);

        for (size_t j = 0; j < cache->bufctl_count_max; j++)
            slab_init_bufctls(cache, bufctl, j);
    }
}

// find and delete all unused slabs
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
            return;

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

// find a slab with a freelist, remove bufctl from freelist, return address
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

// find slab with open spot (slab->bufctl_count < bufctl_count_max), insert new
// bufctl at beginning of freelist
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

// print hierarchy of cache (including slabs + bufctls + it's addresses)
void slab_cache_dump(slab_cache_t *cache, slab_flags_t flags)
{
    cache->slabs = cache->slabs_head;

    if (!cache->slabs && (flags & SLAB_PANIC))
        log(PANIC, "Slab cache dump (name missing): Slabs don't exist\n");

    if (!cache->slabs)
        return;

    debug("Dump for cache with name '%s'\n", cache->name);

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

            debug("\t\tBufctl no. %d\t has pointer 0x%p\n", bufctl_count, cache->slabs->freelist->pointer);

            cache->slabs->freelist = cache->slabs->freelist->next;
        }

done:
        cache->slabs = cache->slabs->next;
    }
}

/* utility functions */

// allocate one page for all bufctls in that slab + slab structure itself
slab_bufctl_t *slab_create_bufctl_buffer(void)
{
    slab_bufctl_t *bufctl = (slab_bufctl_t *)pmm_allocz(1);

    if (!bufctl)
        return NULL;

    bufctl->next = NULL;

    return bufctl;
}

// put slab structure at end of bufctl buffer, add to linked list of slabs
void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl)
{
    slab_t *slab = (slab_t *)(((uintptr_t)bufctl + PAGE_SIZE) - sizeof(slab_t));

    slab->next = NULL;

    slab->bufctl_count = cache->bufctl_count_max;

    slab->freelist_head = NULL;
    slab->freelist = NULL;

    if (!cache->slabs)
    {
        cache->slabs_head = slab;
        cache->slabs = slab;
    }
    else
    {
        cache->slabs->next = slab;
        cache->slabs = cache->slabs->next;
    }
}

// position bufctl at index in bufctl buffer, add it to freelist
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

// return if num is power of two
bool is_power_of_two(int num)
{
    return (num > 0) && ((num & (num - 1)) == 0);
}
