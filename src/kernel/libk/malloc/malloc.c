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

#include <libk/malloc/malloc.h>
#include <memory/dynamic/slab.h>

/* core functions */

/*
slab_cache_t *slab_cache_create(const char *name, size_t slab_size, slab_flags_t flags);
void slab_cache_destroy(slab_cache_t *cache, slab_flags_t flags);
void *slab_cache_alloc(slab_cache_t *cache, slab_flags_t flags);
void slab_cache_free(slab_cache_t *cache, void *pointer, slab_flags_t flags);
void slab_cache_grow(slab_cache_t *cache, size_t count, slab_flags_t flags);
void slab_cache_reap(slab_cache_t *cache, slab_flags_t flags);
void slab_cache_dump(slab_cache_t *cache, slab_flags_t flags);
*/

void malloc_heap_init(void)
{
    slab_cache_create("heap slab size 2", 2, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 4", 4, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 8", 8, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 16", 16, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 32", 32, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 64", 64, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 128", 128, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 256", 256, SLAB_PANIC | SLAB_AUTO_GROW);
    slab_cache_create("heap slab size 512", 512, SLAB_PANIC | SLAB_AUTO_GROW);
}

void malloc(size_t size)
{
    //
}

void free(void *pointer)
{
    //
}
