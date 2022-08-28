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





#include <libk/malloc/malloc.h>
#include <libk/serial/debug.h>
#include <libk/string/string.h>




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

    // NOTE: memdump
    // qemu monitor
    // pmemsave addr size file
    //
    // heap: 0xb000 0xfffff000
    // -> pmemsave 0xb000 0xfffff000 memdump.txt

    // memcpy
    // int *mem_block1 = malloc(5 * sizeof(int));
    // mem_block1[0] = 43;
    // mem_block1[1] = 37;
    // mem_block1[2] = 763;
    // mem_block1[3] = 944;
    // mem_block1[4] = 1220;

    // debug("mem_block1[0]: %d at %x\n", mem_block1[0], &mem_block1[0]);
    // debug("mem_block1[1]: %d at %x\n", mem_block1[1], &mem_block1[1]);
    // debug("mem_block1[2]: %d at %x\n", mem_block1[2], &mem_block1[2]);
    // debug("mem_block1[3]: %d at %x\n", mem_block1[3], &mem_block1[3]);
    // debug("mem_block1[4]: %d at %x\n", mem_block1[4], &mem_block1[4]);

    // int *mem_block2 = malloc(5 * sizeof(int));
    // memcpy(mem_block2, mem_block1, 5 * sizeof(int));

    // debug("mem_block2[0]: %d at %x\n", mem_block2[0], &mem_block2[0]);
    // debug("mem_block2[1]: %d at %x\n", mem_block2[1], &mem_block2[1]);
    // debug("mem_block2[2]: %d at %x\n", mem_block2[2], &mem_block2[2]);
    // debug("mem_block2[3]: %d at %x\n", mem_block2[3], &mem_block2[3]);
    // debug("mem_block2[4]: %d at %x\n", mem_block2[4], &mem_block2[4]);

    // char str1[] = "Hello";
    // char str2[] = "HELLO";

    // debug("str1 before: %s\n", str1);
    // debug("str2 before: %s\n", str2);

    // memcpy(str1, str2, sizeof(str2));

    // debug("str1 after: %s\n", str1);
    // debug("str2 after: %s\n", str2);

    // round_alloc_size
    // in malloc.c - works

    // next_power_of_two
    // in malloc.c - works

    // size_to_slab_cache_index
    // in malloc.c - works

    // slab_cache_index_to_size
    // in malloc.c - works

    // malloc (if it still works, can reuse tests)
    // /* malloc test start */

    // // slab allocations

    // void *ptr1 = malloc(30);
    // debug("ptr1: %p\n", ptr1);

    // void *ptr2 = malloc(30);
    // debug("ptr2: %p\n", ptr2);

    // free(ptr1);
    // free(ptr2);

    // void *ptr3 = malloc(30);
    // debug("ptr3: %p\n", ptr3);
    // void *ptr4 = malloc(30);
    // debug("ptr4: %p\n", ptr4);

    // // pmm allocations

    // void *ptr5 = malloc(5111);
    // debug("ptr5: %p\n", ptr5);
    // void *ptr6 = malloc(5111);
    // debug("ptr6: %p\n", ptr6);

    // free(ptr5);
    // free(ptr6);

    // void *ptr7 = malloc(5111);
    // debug("ptr7: %p\n", ptr7);
    // void *ptr8 = malloc(5111);
    // debug("ptr8: %p\n", ptr8);

    // debug("\n");

    // void *lol1 = pmm_alloc(2);
    // debug("lol1: %p\n", lol1);
    // void *lol2 = pmm_alloc(2);
    // debug("lol2: %p\n", lol2);

    // pmm_free(lol1, 2);
    // pmm_free(lol2, 2);

    // void *lol3 = pmm_alloc(2);
    // debug("lol3: %p\n", lol3);
    // void *lol4 = pmm_alloc(2);
    // debug("lol4: %p\n", lol4);


    // void *bee = malloc(6);
    // debug("bee:  %p\n", bee);

    // int *dummy = (int *)malloc(sizeof(int));
    // *dummy = 5;
    // debug("dummy %p: %d\n", dummy, *dummy);

    // int *dummy1 = (int *)malloc(12011);
    // *dummy1 = 5;
    // debug("dummy %p: %d\n", dummy1, *dummy1);

    // /* malloc test end */

    // realloc (use qemu monitor with command `pmemsave`)
    //
    // int *mem_block1 = malloc(sizeof(int));

    // free(mem_block1);
    
    void *ptr1 = malloc(2);
    void *ptr2 = malloc(5121);

    free(ptr1);
    free(ptr2);

    // mem_block1[0] = 43;
    // mem_block1[1] = 37;
    // mem_block1[2] = 763;
    // mem_block1[3] = 944;
    // mem_block1[4] = 1220;

    // debug("mem_block1[0]: %d at %x\n", mem_block1[0], &mem_block1[0]);
    // debug("mem_block1[1]: %d at %x\n", mem_block1[1], &mem_block1[1]);
    // debug("mem_block1[2]: %d at %x\n", mem_block1[2], &mem_block1[2]);
    // debug("mem_block1[3]: %d at %x\n", mem_block1[3], &mem_block1[3]);
    // debug("mem_block1[4]: %d at %x\n", mem_block1[4], &mem_block1[4]);

    // mem_block1 = realloc(mem_block1, 3 * sizeof(int));

//     if (mem_block1 == NULL)
// 	debug("no\n");

    // debug("mem_block1[0]: %d at %x\n", mem_block1[0], &mem_block1[0]);
    // debug("mem_block1[1]: %d at %x\n", mem_block1[1], &mem_block1[1]);
    // debug("mem_block1[2]: %d at %x\n", mem_block1[2], &mem_block1[2]);
    // debug("mem_block1[3]: %d at %x\n", mem_block1[3], &mem_block1[3]);
    // debug("mem_block1[4]: %d at %x\n", mem_block1[4], &mem_block1[4]);

    /* realloc (and helpers) test end */

    // smp_init(stivale2_struct);
}
