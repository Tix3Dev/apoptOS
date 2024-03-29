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
    Virtual memory management through 4 level paging.

*/

#include <utility/utils.h>
#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <hardware/cpu.h>
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <libk/testing/assert.h>
#include <memory/mem.h>
#include <memory/physical/pmm.h>
#include <memory/virtual/vmm.h>

static uint64_t *root_page_table;

/* utility function prototypes */

uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags);
void vmm_set_pt_value(uint64_t *page_table, uint64_t virt_page, uint64_t pt_value,
                      uint64_t flags, pat_cache_t pat_type);
uint64_t vmm_pat_cache_to_flags(pat_cache_t type);
void vmm_flush_tlb(void *address);

/* core functions */

// enable PAT for caching, create root page table and map important physical memory regions
void vmm_init(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_memmap *memory_map = stivale2_get_tag(stivale2_struct,
            STIVALE2_STRUCT_TAG_MEMMAP_ID);
    struct stivale2_mmap_entry *current_entry;

    enable_pat();

    root_page_table = PHYS_TO_HIGHER_HALF_DATA(pmm_allocz(1));
    assert(root_page_table != NULL);

    // identity map 0x0 - 0x100000000
    vmm_map_range(root_page_table, 0, 4 * GiB, 0, KERNEL_READ_WRITE, PAT_UNCACHEABLE);

    // map 0xFFFF800000000000 - 0xFFFF800100000000 to 0x0 - 0x100000000
    vmm_map_range(root_page_table, 0, 4 * GiB, HIGHER_HALF_DATA, KERNEL_READ_WRITE, PAT_UNCACHEABLE);

    // map 0xFFFF900000000000 - 0xFFFF900100000000 to 0x0 - 0x100000000
    vmm_map_range(root_page_table, 0, HEAP_MAX_SIZE, HEAP_START_ADDR, KERNEL_READ_WRITE, PAT_UNCACHEABLE);

    // map 0xFFFFFFFF80000000 - 0x0001000000000000 0x0 - 0x80000000
    vmm_map_range(root_page_table, 0, 2 * GiB, HIGHER_HALF_CODE, KERNEL_READ, PAT_UNCACHEABLE);

    // map at 0xFFFF800000000000 to all entries in memory map
    for (uint64_t i = 0; i < memory_map->entries; i++)
    {
        current_entry = &memory_map->memmap[i];

        vmm_map_range(root_page_table, 0, current_entry->length, HIGHER_HALF_DATA, KERNEL_READ_WRITE,
                      PAT_UNCACHEABLE);
    }

    log(INFO, "Replaced bootloader page table at 0x%.16llx\n", asm_read_cr(3));
    vmm_load_page_table(root_page_table);
    log(INFO, "Now using kernel page table at 0x%.16llx\n", asm_read_cr(3));

    log(INFO, "VMM initialized\n");
}

// set a page table entry for a new virtual memory address, which will be mapped to a physical frame
void vmm_map_page(uint64_t *page_table, uint64_t phys_page, uint64_t virt_page,
                  uint64_t flags, pat_cache_t pat_type)
{
    uint64_t pt_value = phys_page;
    vmm_set_pt_value(page_table, ALIGN_DOWN(virt_page, PAGE_SIZE), pt_value, flags, pat_type);
}

// set a page table entry to zero, in order to "forget" a virtual memory address
void vmm_unmap_page(uint64_t *page_table, uint64_t virt_page)
{
    uint64_t pt_value = 0;
    vmm_set_pt_value(page_table, ALIGN_DOWN(virt_page, PAGE_SIZE), pt_value, 0, 0);
}

// map a whole physical memory region with custom offset
void vmm_map_range(uint64_t *page_table, uint64_t start, uint64_t end, uint64_t offset,
                   uint64_t flags, pat_cache_t pat_type)
{
    for (uint64_t i = ALIGN_DOWN(start, PAGE_SIZE); i < ALIGN_UP(end, PAGE_SIZE); i += PAGE_SIZE)
    {
        vmm_map_page(page_table, i, i + offset, flags, pat_type);
    }
}

// unmap a whole physical memory region
void vmm_unmap_range(uint64_t *page_table, uint64_t start, uint64_t end)
{
    for (uint64_t i = ALIGN_DOWN(start, PAGE_SIZE); i < ALIGN_UP(end, PAGE_SIZE); i += PAGE_SIZE)
    {
        vmm_unmap_page(page_table, i);
    }
}

// load a page table into cr3 to be used
void vmm_load_page_table(uint64_t *page_table)
{
    asm_write_cr(3, HIGHER_HALF_DATA_TO_PHYS((uint64_t)page_table));
}

// return the root, aka kernel, page table
uint64_t *vmm_get_root_page_table(void)
{
    return root_page_table;
}

/* utility functions */

// make use (and if needed alloacte for that) a custom page map level
uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags)
{
    // check present flag
    if (!(pml[pml_index] & 1))
    {
        pml[pml_index] = (uint64_t)pmm_allocz(1) | flags;
    }

    return (uint64_t *)(pml[pml_index] & ~(511));
}

// set a value in a page table entry and flush translation lookaside buffer
void vmm_set_pt_value(uint64_t *page_table, uint64_t virt_page, uint64_t pt_value,
                      uint64_t flags, pat_cache_t pat_type)
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
    pt[pt_index]    = pt_value | flags | vmm_pat_cache_to_flags(pat_type);

    // for changes to apply, the translation lookaside buffers need to be flushed
    vmm_flush_tlb((void *)virt_page);
}

// combine PAT, PCD and PWT according to custom_pat_config (in PAT MSR),
// so it can be used for PT flag (only PT because the layout is different
// for different page map levels)
uint64_t vmm_pat_cache_to_flags(pat_cache_t type)
{
    /*
        Layout that needs to be used according to custom_pat_config:
        Uncachable:     PAT0:  PAT = 0, PCD = 0, PWT = 0
        WriteCombining: PAT1:  PAT = 0, PCD = 0, PWT = 1
        WriteThrough:   PAT4:  PAT = 1, PCD = 0, PWT = 0
        WriteProtected: PAT5:  PAT = 1, PCD = 0, PWT = 1
        WriteBack:      PAT6:  PAT = 1, PCD = 1, PWT = 0
        Uncached:       PAT7:  PAT = 1, PCD = 1, PWT = 1
    */

    uint64_t flags = 0;

    switch (type)
    {
        case PAT_UNCACHEABLE:
            break;

        case PAT_WRITE_COMBINING:
            flags = PTE_WRITE_THROUGH;
            break;

        case PAT_WRITE_THROUGH:
            flags = PTE_PAT;
            break;

        case PAT_WRITE_PROTECTED:
            flags = PTE_PAT | PTE_WRITE_THROUGH;
            break;

        case PAT_WRITE_BACK:
            flags = PTE_PAT | PTE_CACHE_DISABLED;
            break;

        case PAT_UNCACHED:
            flags = PTE_PAT | PTE_CACHE_DISABLED | PTE_WRITE_THROUGH;
            break;
    }

    return flags;
}

// flush/reload translation lookaside buffer
void vmm_flush_tlb(void *address)
{
    asm_invlpg(address);
}
