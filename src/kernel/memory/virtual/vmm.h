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

#ifndef VMM_H
#define VMM_H

#include <stddef.h>
#include <stdint.h>

// privilege of a page table entry (PTE)
#define PTE_PRESENT	    (1 << 0)
#define PTE_READ_WRITE	    (1 << 1)
#define PTE_USER_SUPERVISOR (1 << 2)
#define PTE_WRITE_THROUGH   (1 << 3)
#define PTE_CHACHE_DISABLED (1 << 4)
#define PTE_ACCESSED	    (1 << 5)
#define PTE_DIRTY	    (1 << 6)
#define PTE_PAT		    (1 << 7)
#define PTE_GLOBAL	    (1 << 8)

// types of virtual memory mapping privileges
typedef enum
{
    KERNEL_READ		= PTE_PRESENT,
    KERNEL_READ_WRITE	= PTE_PRESENT | PTE_READ_WRITE,
    USER_READ		= PTE_PRESENT | PTE_USER_SUPERVISOR,
    USER_READ_WRITE	= PTE_PRESENT | PTE_READ_WRITE | PTE_USER_SUPERVISOR
} vmm_map_privilege_t;

void vmm_init(struct stivale2_struct *stivale2_struct);
void vmm_map_page(uint64_t *page_table, uint64_t phys_page, uint64_t virt_page, uint64_t flags);
void vmm_unmap_page(uint64_t *page_table, uint64_t virt_page);
void vmm_map_range(uint64_t *page_table, uint64_t start, uint64_t end, uint64_t offset, uint64_t flags);
void vmm_unmap_range(uint64_t *page_table, uint64_t start, uint64_t end);
void vmm_load_page_table(uint64_t *page_table);
uint64_t *vmm_get_root_page_table(void);

#endif
