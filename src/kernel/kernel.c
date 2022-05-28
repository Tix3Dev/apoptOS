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
    C code entry point of whole kernel, even OS itself.

*/

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <libk/serial/log.h>
#include <libk/testing/assert.h>
#include <memory/mem.h>
#include <memory/dynamic/heap.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/virtual/vmm.h>
#include <tables/gdt.h>
#include <tables/idt.h>





#include <libk/serial/debug.h>




/* utility function prototypes */

void kinit_all(struct stivale2_struct *stivale2_struct);

/* core functions */

// kernel entry point - call initializations
void kmain(struct stivale2_struct *stivale2_struct)
{
    log(INFO, "Kernel started\n");

    kinit_all(stivale2_struct);

    log(INFO, "All kernels parts initialized\n");


    /* heap testing start */

    slab_cache_t *dummy1 = slab_cache_create("test filesystem cache", 128);
    // slab_cache_t *dummy2 = slab_cache_create("test heap cache", 256);
    
    slab_cache_dump(dummy1);
    // slab_cache_dump(dummy2);

    debug("\nAllocation tests:\n\n");

//     for (int i = 0; i < 31; i++)
// 	slab_cache_alloc(dummy1);

    void *d1ptr1 = slab_cache_alloc(dummy1);
    void *d1ptr2 = slab_cache_alloc(dummy1);
    void *d1ptr3 = slab_cache_alloc(dummy1);

    debug("d1ptr1: %p\n", d1ptr1);
    debug("d1ptr2: %p\n", d1ptr2);
    debug("d1ptr3: %p\n", d1ptr3);

    // void *d2ptr1 = slab_cache_alloc(dummy2);
    // void *d2ptr2 = slab_cache_alloc(dummy2);
    // void *d2ptr3 = slab_cache_alloc(dummy2);

    // debug("d2ptr1: %p\n", d2ptr1);
    // debug("d2ptr2: %p\n", d2ptr2);
    // debug("d2ptr3: %p\n", d2ptr3);

    slab_cache_dump(dummy1);
    // slab_cache_dump(dummy2);

    slab_cache_free(dummy1, d1ptr1);
    slab_cache_free(dummy1, d1ptr2);
    slab_cache_free(dummy1, d1ptr3);

    slab_cache_dump(dummy1);

    /* heap testing end */


    for (;;)
        asm volatile("hlt");
}

/* utility functions */

// initialize all kernel parts
void kinit_all(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_hhdm *hhdm = stivale2_get_tag(stivale2_struct,
                                            STIVALE2_STRUCT_TAG_HHDM_ID);
    log(INFO, "Verified HHDM address 0x%.16llx\n", hhdm->addr);
    assert(hhdm->addr == HIGHER_HALF_DATA);

    pmm_init(stivale2_struct);
    vmm_init(stivale2_struct);

    gdt_init();
    idt_init();

    // heap_init();
}
