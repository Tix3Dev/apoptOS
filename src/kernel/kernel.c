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

    slab_cache_t *dummy1 = slab_cache_create("test filesystem cache", 64, NULL, NULL);
    
    // slab_cache_t *dummy1 = (slab_cache_t *)pmm_alloc(1);
    // dummy1->name = "fs";
    // dummy1->slab_size = 128;
    // dummy1->constructor = NULL;
    // dummy1->deconstructor = NULL;


    // slab_bufctl_t freelist4_2 = {NULL, (void *)0x012};
    // slab_bufctl_t freelist4_1 = {&freelist4_2, (void *)0x012};
    // slab_bufctl_t freelist4_0 = {&freelist4_1, (void *)0x012};
    // slab_t slab4 = {NULL, &freelist4_0};

    // slab_bufctl_t freelist3_2 = {NULL, (void *)0x012};
    // slab_bufctl_t freelist3_1 = {&freelist3_2, (void *)0x012};
    // slab_bufctl_t freelist3_0 = {&freelist3_1, (void *)0x012};
    // slab_t slab3 = {&slab4, &freelist3_0};

    // slab_bufctl_t freelist2_2 = {NULL, (void *)0x012};
    // slab_bufctl_t freelist2_1 = {&freelist2_2, (void *)0x012};
    // slab_bufctl_t freelist2_0 = {&freelist2_1, (void *)0x012};
    // slab_t slab2 = {&slab3, &freelist2_0};

    // slab_bufctl_t freelist1_2 = {NULL, (void *)0x012};
    // slab_bufctl_t freelist1_1 = {&freelist1_2, (void *)0x012};
    // slab_bufctl_t freelist1_0 = {&freelist1_1, (void *)0x012};
    // slab_t slab1 = {&slab2, &freelist1_0};

    // dummy1->slabs = &slab1;

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
