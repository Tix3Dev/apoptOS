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

#include <memory/dynamic/slab.h>

/* utility function prototypes */

// 

/* core functions */

void slab_cache_create(const char *name, size_t size,
	cache_ctor_t constructor, cache_dtor_t deconstructor)
{
    //
}

void slab_cache_destroy(void)
{
    //
}

void slab_cache_alloc(void)
{
    //
}

void slab_cache_free(void)
{
    //
}

/* utility functions */

void slab_cache_grow(void)
{
    //
}

void slab_cache_reap(void)
{
    //
}
