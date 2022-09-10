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

#ifndef SLAB_H
#define SLAB_H

#include <stdbool.h>
#include <stddef.h>

typedef struct __attribute__((__packed__)) slab_bufctl
{
    struct slab_bufctl *next;

    uint16_t index;
} slab_bufctl_t;

typedef struct slab
{
    struct slab *next;

    size_t bufctl_count;

    slab_bufctl_t *freelist_head;
    slab_bufctl_t *freelist;
} slab_t;

typedef struct
{
    const char *name;
    size_t slab_size;
    size_t bufctl_count_max;

    slab_t *slabs_head;
    slab_t *slabs;
} slab_cache_t;

typedef enum
{
    SLAB_PANIC	    = (1 << 0),
    SLAB_AUTO_GROW  = (1 << 1)
} slab_flags_t;

slab_cache_t *slab_cache_create(const char *name, size_t slab_size, slab_flags_t flags);
void slab_cache_destroy(slab_cache_t *cache, slab_flags_t flags);
void *slab_cache_alloc(slab_cache_t *cache, slab_flags_t flags);
void slab_cache_free(slab_cache_t *cache, void *pointer, slab_flags_t flags);
void slab_cache_grow(slab_cache_t *cache, size_t count, slab_flags_t flags);
void slab_cache_reap(slab_cache_t *cache, slab_flags_t flags);
void slab_cache_dump(slab_cache_t *cache, slab_flags_t flags);

#endif
