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
    blah

*/

// NOTE: goto_address is described here:
// https://github.com/stivale/stivale/blob/master/STIVALE2.md#smp-structure-tag
// ->	even though the arguments for ap_init aren't passed anywhere,
//	stivale2 will load a pointer to the stivale2_smp_info struct into RDI,
//	so they are still present as arguments

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <hardware/cpu.h>
#include <libk/lock/spinlock.h>
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>
#include <libk/testing/assert.h>
#include <memory/physical/pmm.h>
#include <memory/virtual/vmm.h>
#include <memory/mem.h>
#include <proc/smp/smp.h>
#include <tables/gdt.h>
#include <tables/idt.h>






#include <utility/utils.h>


static spinlock_t smp_lock;

cpu_local_t *cpu_locals;
static uint32_t cpus_online = 0;

/* utility function prototypes */

static void bsp_init(struct stivale2_smp_info *smp_entry);
static void ap_init(struct stivale2_smp_info *smp_entry);
static void generic_cpu_local_init(struct stivale2_smp_info *smp_entry);

/* core functions */

void smp_init(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_smp *smp_tag = stivale2_get_tag(stivale2_struct,
            STIVALE2_STRUCT_TAG_SMP_ID);

    log(INFO, "Total CPU count: %d\n", smp_tag->cpu_count);

    cpu_locals = malloc(smp_tag->cpu_count * sizeof(cpu_local_t));

    for (uint64_t i = 0; i < smp_tag->cpu_count; i++)
    {
	spinlock_acquire(&smp_lock);

	smp_tag->smp_info[i].extra_argument = i;

	uint64_t stack = (uintptr_t)pmm_allocz(CPU_LOCALS_STACK_SIZE / PAGE_SIZE);
	assert(stack != 0);
	stack = PHYS_TO_HIGHER_HALF_DATA(stack + CPU_LOCALS_STACK_SIZE);

	smp_tag->smp_info[i].target_stack = stack;

	if (smp_tag->smp_info[i].lapic_id == smp_tag->bsp_lapic_id)
	{
	    bsp_init((void *)&smp_tag->smp_info[i]);

	    spinlock_release(&smp_lock);

	    continue;
	}

	smp_tag->smp_info[i].goto_address = (uint64_t)ap_init;

	spinlock_release(&smp_lock);
    }

    while (cpus_online != smp_tag->cpu_count)
    {
	asm volatile("pause");
    }

    log(INFO, "SMP initialized - All CPUs initialized\n");
}

/* utility functions */

static void bsp_init(struct stivale2_smp_info *smp_entry)
{
    generic_cpu_local_init(smp_entry);

    log(INFO, "CPU No. %ld: BSP fully initialized\n", smp_entry->extra_argument);
    cpus_online++;
}

static void ap_init(struct stivale2_smp_info *smp_entry)
{
    spinlock_acquire(&smp_lock);

    vmm_load_page_table(vmm_get_root_page_table());
    gdt_load();
    idt_load();

    generic_cpu_local_init(smp_entry);

    asm volatile("sti"); // store interrupt flag -> allow hardware interrupts

    log(INFO, "CPU No. %ld: AP fully initialized\n", smp_entry->extra_argument);
    cpus_online++;

    spinlock_release(&smp_lock);
    
    for (;;)
    {
        asm volatile("hlt");
    }
}

static void generic_cpu_local_init(struct stivale2_smp_info *smp_entry)
{
    uint64_t cpu_num = smp_entry->extra_argument;
    uint32_t lapic_id = smp_entry->lapic_id;
    uint64_t stack = smp_entry->target_stack;

    cpu_locals[cpu_num].cpu_number = cpu_num;
    cpu_locals[cpu_num].lapic_id = lapic_id;
    cpu_locals[cpu_num].tss.rsp[0] = stack;

    tss_create_segment(&cpu_locals[cpu_num].tss);
    tss_load();

    // set gsbase
    
    // enable sse
    
    // enable write-combining in the PAT
}
