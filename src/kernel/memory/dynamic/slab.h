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

#include <stddef.h>

#include <libk/data_structs/linked_list.h>

#ifndef SLAB_H
#define SLAB_H

typedef void (*cache_ctor_t)(size_t);
typedef void (*cache_dtor_t)(size_t);

// TODO: flags

typedef struct
{
    slist_t next;

    void *pointer;
} slab_bufctl_t;

typedef struct
{
    slist_t next;
    slist_t prev;

    slab_bufctl_t *freelist;
} slab_t;

typedef struct
{
    const char *name;
    size_t slab_size;

    cache_ctor_t constructor;
    cache_dtor_t deconstructor;

    slab_t *slabs;
} slab_cache_t;

#endif
