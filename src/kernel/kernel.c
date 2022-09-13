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
    C code entry point of whole kernel, even OS itself.

*/

#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <hardware/acpi/acpi.h>
#include <hardware/apic/apic.h>
#include <hardware/cpu.h>
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>
#include <libk/testing/assert.h>
#include <memory/mem.h>
#include <memory/dynamic/slab.h>
#include <memory/physical/pmm.h>
#include <memory/virtual/vmm.h>
#include <proc/smp/smp.h>
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

    log(INFO, "All kernel parts initialized\n");

    for (;;)
    {
        asm volatile("hlt");
    }
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

    // log(INFO, "CPU vendor id string: '%s'\n", cpu_get_vendor_id_string());

    // acpi_init(stivale2_struct);
    // apic_init();

    /* realloc (and helpers) test start */

    debug("sizeof(int): %d\n", sizeof(int));

    int *ptr;

    ptr = malloc(10 * sizeof(int));

    debug("ptr at  0x%p\n", ptr);

    ptr = realloc(ptr, 40 * sizeof(int));
    if(ptr == NULL) {
    	log(WARNING, "returned NULL\n"); 
    }

    ptr[100] = -120;

    debug("%d at 0x%p\n", ptr[100], &ptr[3]);


    int *ptr1;

    ptr1 = malloc(10 * sizeof(int));
    debug("ptr1 at 0x%p\n", ptr1);

    ptr1[3] = -120;

    ptr1 = realloc(ptr1, 4 * sizeof(int));
    if(ptr1 == NULL) {
	log(WARNING, "returned NULL too\n");
    }
    debug("%d at 0x%p\n", ptr1[3], &ptr1[3]);


    int *ptr2;

    ptr2 = malloc(10 * sizeof(int));
    debug("ptr2 at 0x%p\n", ptr2);
    ptr2[3] = -120;

    ptr2 = realloc(ptr2, 10 * sizeof(int));
    if(ptr2 == NULL) {
	log(WARNING, "returned NULL too\n");
    }
    debug("%d at 0x%p\n", ptr2[3], &ptr2[3]);

    /* realloc (and helpers) test end */

    // smp_init(stivale2_struct);
}
