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
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>
#include <libk/testing/assert.h>
#include <memory/mem.h>
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

    /* malloc test start */

    // slab allocations

    void *ptr1 = malloc(30);
    debug("ptr1: %p\n", ptr1);

    void *ptr2 = malloc(30);
    debug("ptr2: %p\n", ptr2);

    free(ptr1);
    free(ptr2);

    void *ptr3 = malloc(30);
    debug("ptr3: %p\n", ptr3);
    void *ptr4 = malloc(30);
    debug("ptr4: %p\n", ptr4);

    // pmm allocations

    void *ptr5 = malloc(5111);
    debug("ptr5: %p\n", ptr5);
    void *ptr6 = malloc(5111);
    debug("ptr6: %p\n", ptr6);

    free(ptr5);
    free(ptr6);

    void *ptr7 = malloc(5111);
    debug("ptr7: %p\n", ptr7);
    void *ptr8 = malloc(5111);
    debug("ptr8: %p\n", ptr8);

    debug("\n");

    void *lol1 = pmm_alloc(2);
    debug("lol1: %p\n", lol1);
    void *lol2 = pmm_alloc(2);
    debug("lol2: %p\n", lol2);

    pmm_free(lol1, 2);
    pmm_free(lol2, 2);

    void *lol3 = pmm_alloc(2);
    debug("lol3: %p\n", lol3);
    void *lol4 = pmm_alloc(2);
    debug("lol4: %p\n", lol4);


    void *bee = malloc(6);
    debug("bee:  %p\n", bee);

    int *dummy = (int *)malloc(sizeof(int));
    *dummy = 5;
    debug("dummy %p: %d\n", dummy, *dummy);

    int *dummy1 = (int *)malloc(12011);
    *dummy1 = 5;
    debug("dummy %p: %d\n", dummy1, *dummy1);

    /* malloc test end */

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

    malloc_heap_init();
}
