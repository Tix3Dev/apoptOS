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

#ifndef VMM_H
#define VMM_H

void vmm_init(void);
void vmm_map_page(uint64_t *page_table, uint64_t phys_page, uint64_t virt_page, uint64_t flags);
void vmm_unmap_page(uint64_t *page_table, uint64_t virt_page);
void vmm_map_range(uint64_t *page_table, uint64_t start, uint64_t end, uint64_t offset, uint64_t flags);

#endif
