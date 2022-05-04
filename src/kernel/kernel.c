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
#include <libk/serial/debug.h>
#include <libk/serial/log.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <tables/gdt.h>
#include <tables/idt.h>

/* utility function prototypes */

void kinit_all(struct stivale2_struct *stivale2_struct);

/* core functions */

void kmain(struct stivale2_struct *stivale2_struct)
{
    log(INFO, "Kernel started\n");
    
    kinit_all(stivale2_struct);

    // debug_set_color(TERM_PURPLE);

    // void *ptr1 = pmm_alloc(1);
    // debug("1: ptr1 allocated at: 0x%x\n", ptr1);

    // void *ptr2 = pmm_alloc(1);
    // debug("1: ptr2 allocated at: 0x%x\n", ptr2);

    // void *ptr3 = pmm_alloc(1);
    // debug("1: ptr3 allocated at: 0x%x\n", ptr3);

    // void *ptr4 = pmm_alloc(2);
    // debug("2: ptr4 allocated at: 0x%x\n", ptr4);

    // void *ptr5 = pmm_alloc(3);
    // debug("3: ptr5 allocated at: 0x%x\n", ptr5);

    // void *ptr6 = pmm_alloc(8);
    // debug("8: ptr6 allocated at: 0x%x\n", ptr6);

    // void *ptr7 = pmm_alloc(1);
    // debug("1: ptr7 allocated at: 0x%x\n", ptr7);

    // pmm_free(ptr1, 1);
    // pmm_free(ptr2, 1);
    // pmm_free(ptr3, 1);
    // pmm_free(ptr4, 2);
    // pmm_free(ptr5, 3);
    // pmm_free(ptr6, 8);
    // pmm_free(ptr7, 7);

    // void *ptr8 = pmm_alloc(1);
    // debug("1: ptr8 allocated at: 0x%x\n", ptr8);

    // debug_set_color(TERM_PURPLE);

    // for (;;)
    // {
    //     void *ptr9 = pmm_alloc(10000);
    //     debug("1: ptr9 allocated at: 0x%x\n", ptr9);
    // }

    log(INFO, "All kernels parts initialized\n");

    for (;;)
        asm volatile("hlt");
}

/* utility functions */

void kinit_all(struct stivale2_struct *stivale2_struct)
{
    pmm_init(stivale2_struct);
    //vmm_init(stivale2_struct);
    gdt_init();
    idt_init();
}
