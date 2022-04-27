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

*/

#include <assembly/utils.h>
#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <libk/testing/assert.h>
#include <memory/mem.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

static uint64_t *root_page_table; 

/* utility function prototypes */

uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags);
void vmm_set_pt_value(uint64_t *page_table, uint64_t virt_page, uint64_t flags, uint64_t value);
void vmm_flush_tlb(void *address);
void vmm_load_page_table(uint64_t *page_table);

/* core functions */

#define GB 0x40000000UL

#define PTE_PRESENT	    1
#define PTE_READ_WRITE	    2
#define PTE_USER_SUPERVISOR 4
#define PTE_WRITE_THROUGH   8
#define PTE_CHACHE_DISABLED 16
#define PTE_ACCESSED	    32
#define PTE_DIRTY	    64
#define PTE_PAT		    128
#define PTE_GLOBAL	    256

void vmm_init(void)
{
    root_page_table = pmm_alloc(1);
    assert(root_page_table != NULL);
    memset(root_page_table, 0, PAGE_SIZE);

    for (uint64_t i = 0; i < 4 * GB; i += PAGE_SIZE)
        vmm_map_page(root_page_table, i, i, PTE_PRESENT | PTE_READ_WRITE);

    // vmm_map_range(root_page_table, 0, 4 * GB, 0, PTE_PRESENT | PTE_READ_WRITE);
    // vmm_map_range(root_page_table, 0, 4 * GB, HIGHER_HALF_DATA_LV4, PTE_PRESENT | PTE_READ_WRITE);
    // vmm_map_range(root_page_table, 0, 2 * GB, HIGHER_HALF_DATA_LV4, PTE_PRESENT);

    // vmm_load_page_table(root_page_table);

    log(INFO, "VMM initialized\n");
}

void vmm_map_page(uint64_t *page_table, uint64_t phys_page, uint64_t virt_page, uint64_t flags)
{
    // // index for page mapping level 4
    // size_t pml4_index	= (virt_page & ((uintptr_t)0x1ff << 39)) >> 39;
    // // index for page directory table
    // size_t pdpt_index	= (virt_page & ((uintptr_t)0x1ff << 30)) >> 30;
    // // index for page directory
    // size_t pd_index	= (virt_page & ((uintptr_t)0x1ff << 21)) >> 21;
    // // index for page table
    // size_t pt_index	= (virt_page & ((uintptr_t)0x1ff << 12)) >> 12;

    // // page mapping level 4 = pml4
    // uint64_t *pml4  = page_table;
    // // page directory table = pml3
    // uint64_t *pdpt  = vmm_get_or_create_pml(pml4, pml4_index, flags);
    // // page directory	    = pml2
    // uint64_t *pd    = vmm_get_or_create_pml(pdpt, pdpt_index, flags);	// <---
    // // page table	    = pml1
    // uint64_t *pt    = vmm_get_or_create_pml(pd, pd_index, flags);

    // // actual mapped, physical frame
    // pt[pt_index]    = phys_page | flags;

    // // for changes to apply, the translation lookaside buffers need to be flushed
    // vmm_flush_tlb((void *)virt_page);

    uint64_t pt_value = phys_page | flags;
    vmm_set_pt_value(page_table, virt_page, flags, pt_value);
}

// void vmm_unmap_page(uint64_t *page_table, uint64_t virt_page)
// {
//     // // index for page mapping level 4
//     // size_t pml4_index	= (virt_page & ((uintptr_t)0x1ff << 39)) >> 39;
//     // // index for page directory table
//     // size_t pdpt_index	= (virt_page & ((uintptr_t)0x1ff << 30)) >> 30;
//     // // index for page directory
//     // size_t pd_index	= (virt_page & ((uintptr_t)0x1ff << 21)) >> 21;
//     // // index for page table
//     // size_t pt_index	= (virt_page & ((uintptr_t)0x1ff << 12)) >> 12;
// 
//     // // page mapping level 4 = pml4
//     // uint64_t *pml4  = page_table;
//     // // page directory table = pml3
//     // uint64_t *pdpt  = vmm_get_or_create_pml(pml4, pml4_index, 0);
//     // // page directory	    = pml2
//     // uint64_t *pd    = vmm_get_or_create_pml(pdpt, pdpt_index, 0);
//     // // page table	    = pml1
//     // uint64_t *pt    = vmm_get_or_create_pml(pd, pd_index, 0);
// 
//     // // unset mapped, physical frame
//     // pt[pt_index]    = 0;
// 
//     // // for changes to apply, the translation lookaside buffers need to be flushed
//     // vmm_flush_tlb((void *)virt_page);
// 
//     uint64_t pt_value = 0;
//     vmm_set_pt_value(page_table, virt_page, 0, pt_value);
// }

void vmm_map_range(uint64_t *page_table, uint64_t start, uint64_t end, uint64_t offset, uint64_t flags)
{
    assert(IS_PAGE_ALIGNED(start));
    assert(IS_PAGE_ALIGNED(end));

    for (uint64_t i = start; i < end; i += PAGE_SIZE)
	vmm_map_page(page_table, i, i + offset, flags);
}

/* utility functions */

uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags)
{
    // check present flag
    if (!(pml[pml_index] & 1))
        pml[pml_index] = phys_to_higher_half_data((uint64_t)pmm_alloc(1)) | flags;

    return (uint64_t *)(pml[pml_index] & ~(511));
}

void vmm_set_pt_value(uint64_t *page_table, uint64_t virt_page, uint64_t flags, uint64_t value)
{
    // index for page mapping level 4
    size_t pml4_index	= (virt_page & ((uintptr_t)0x1ff << 39)) >> 39;
    // index for page directory table
    size_t pdpt_index	= (virt_page & ((uintptr_t)0x1ff << 30)) >> 30;
    // index for page directory
    size_t pd_index	= (virt_page & ((uintptr_t)0x1ff << 21)) >> 21;
    // index for page table
    size_t pt_index	= (virt_page & ((uintptr_t)0x1ff << 12)) >> 12;

    // page mapping level 4 = pml4
    uint64_t *pml4  = page_table;
    // page directory table = pml3
    uint64_t *pdpt  = vmm_get_or_create_pml(pml4, pml4_index, flags);
    // page directory	    = pml2
    uint64_t *pd    = vmm_get_or_create_pml(pdpt, pdpt_index, flags);
    // page table	    = pml1
    uint64_t *pt    = vmm_get_or_create_pml(pd, pd_index, flags);

    // actual mapped value (either physical frame address or 0)
    pt[pt_index]    = value;

    // for changes to apply, the translation lookaside buffers need to be flushed
    vmm_flush_tlb((void *)virt_page);
}

void vmm_flush_tlb(void *address)
{
    asm_invlpg(address);
}

void vmm_load_page_table(uint64_t *page_table)
{
    asm_write_cr(3, higher_half_data_to_phys((uint64_t)page_table));
}
