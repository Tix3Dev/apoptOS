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

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <hardware/hpet/hpet.h>
#include <hardware/cpu.h>
#include <libk/lock/spinlock.h>
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>
#include <memory/physical/pmm.h>
#include <memory/mem.h>
#include <proc/smp/smp.h>

spinlock_t smp_lock;

cpu_local_t *cpu_locals;
static uint32_t cpu_count = 0;
static uint32_t bsp_lapic_id = 0;

/* utility function prototypes */

static void cpu_init(struct stivale2_smp_info *smp_info);

/* core functions */

void smp_init(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_smp *smp_tag = stivale2_get_tag(stivale2_struct,
            STIVALE2_STRUCT_TAG_SMP_ID);

    log(INFO, "Total CPU count: %d\n", smp_tag->cpu_count);

    cpu_locals = malloc(smp_tag->cpu_count * sizeof(cpu_local_t));
    bsp_lapic_id = smp_tag->bsp_lapic_id;
    log(INFO, "bsp lapic id: %d\n", bsp_lapic_id);
    log(INFO, "cpu count: %d\n", cpu_count);

    for (size_t i = 0; i < smp_tag->cpu_count; i++)
    {
	smp_tag->smp_info[i].extra_argument = (uint64_t)&cpu_locals[i];
	
	uint64_t stack = (uintptr_t)pmm_allocz(CPU_LOCALS_STACK_SIZE / PAGE_SIZE);
	stack += CPU_LOCALS_STACK_SIZE; // stack grows downwards
	stack = PHYS_TO_HIGHER_HALF_DATA(stack);

	uint64_t scheduler_stack = (uintptr_t)pmm_allocz(1);
	scheduler_stack += PAGE_SIZE; // stack grows downwards
	scheduler_stack = PHYS_TO_HIGHER_HALF_DATA(stack);

	cpu_locals[i].tss.rsp[0] = stack;
	cpu_locals[i].tss.ist[0] = scheduler_stack;

	if (smp_tag->smp_info[i].lapic_id == bsp_lapic_id)
	{
	    log(INFO, "this is the bsp\n");
	    cpu_init((void *)&smp_tag->smp_info[i]);
	    log(INFO, "---\n");
	    
	    continue;
	}

	cpu_locals[i].cpu_number = i;
	log(INFO, "cpu_locals[i].cpu_number: %d\n", cpu_locals[i].cpu_number);

	spinlock_acquire(smp_lock);
	smp_tag->smp_info[i].target_stack = stack;
	smp_tag->smp_info[i].goto_address = (uint64_t)cpu_init;
	spinlock_release(smp_lock);

	log(INFO, "waiting\n");
	hpet_usleep(100 * 1000);
	log(INFO, "done waiting\n");

	log(INFO, "---\n");
    }

    while (cpu_count != smp_tag->cpu_count);

    log(INFO, "SMP initialized - All CPUs initialized\n");
}

/* utility functions */

static void cpu_init(struct stivale2_smp_info *smp_info)
{
    spinlock_acquire(smp_lock);
    log(INFO, "cpu initializing - lapic id: %d\n", smp_info->lapic_id);
    log(INFO, "cpu_count: %d\n", cpu_count++);
    spinlock_release(smp_lock);
}
