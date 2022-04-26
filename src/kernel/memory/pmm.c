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
    Physical memory management through bitmap based allocator. Basic concept:
    Each bit in the bitmap corresponds to a page (block of memory) through a
    mapping system. A bit only says if the corresponding page is free or used.

*/

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <libk/data_structs/bitmap.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <memory/mem.h>
#include <memory/pmm.h>

bitmap_t pmm_bitmap;
static size_t highest_page_top = 0;
static size_t used_pages_count = 0;

/* utility function prototypes */

const char *get_memmap_entry_type_string(uint32_t type);
void *pmm_find_first_free_page_range(size_t page_count);

/* core functions */

// handle memory map passed by stivale2 and host bitmap for allocator
void pmm_init(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_memmap *memory_map = stivale2_get_tag(stivale2_struct,
	    STIVALE2_STRUCT_TAG_MEMMAP_ID);
    struct stivale2_mmap_entry *current_entry;

    size_t current_page_top = 0;

    /* handle memory map passed by stivale2 */
    
    log(INFO, "Memory map layout:\n");

    for (uint64_t i = 0; i < memory_map->entries; i++)
    {
	current_entry = &memory_map->memmap[i];

	debug_set_color(TERM_PURPLE);
        debug("Memory map entry No. %.16d: Base: 0x%.16llx | Length: 0x%.16llx | Type: %s\n",
              i, current_entry->base, current_entry->length, get_memmap_entry_type_string(current_entry->type));
	debug_set_color(TERM_COLOR_RESET);

	if (current_entry->type != STIVALE2_MMAP_USABLE &&
		current_entry->type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE &&
		current_entry->type != STIVALE2_MMAP_KERNEL_AND_MODULES)
	    continue;

	current_page_top = current_entry->base + current_entry->length;

	if (current_page_top > highest_page_top)
	    highest_page_top = current_page_top;
    }

    used_pages_count = KB_TO_PAGES(highest_page_top);

    pmm_bitmap.size = ALIGN_UP(ALIGN_DOWN(highest_page_top, PAGE_SIZE) / PAGE_SIZE / 8, PAGE_SIZE);

    /* host bitmap for allocator */

    for (uint64_t i = 0; i < memory_map->entries; i++)
    {
	current_entry = &memory_map->memmap[i];

	if (current_entry->type != STIVALE2_MMAP_USABLE)
	    continue;

	if (current_entry->length >= pmm_bitmap.size)
	{
	    debug_set_color(TERM_PURPLE);
	    debug("Found big memory map entry to host the PMM bitmap\n");
            debug("PMM bitmap stored between 0x%.8lx and 0x%.8lx\n",
		    current_entry->base, current_entry->base + current_entry->length - 1);
	    debug_set_color(TERM_COLOR_RESET);

	    pmm_bitmap.map = (uint8_t *)(phys_to_higher_half_data(current_entry->base));

	    current_entry->base += pmm_bitmap.size;
	    current_entry->length -= pmm_bitmap.size;

	    break;
	}
    }

    memset((void *)pmm_bitmap.map, 0xFF, pmm_bitmap.size);

    for (uint64_t i = 0; i < memory_map->entries; i++)
    {
	current_entry = &memory_map->memmap[i];

	if (current_entry->type == STIVALE2_MMAP_USABLE)
	    pmm_free((void *)current_entry->base, current_entry->length / PAGE_SIZE);
    }

    bitmap_set_bit(&pmm_bitmap, 0);

    log(INFO, "PMM initialized\n");
}

// set free memory range to used and return base pointer
void *pmm_alloc(size_t page_count)
{
    if (used_pages_count <= 0)
        return NULL;

    void *pointer = pmm_find_first_free_page_range(page_count);

    if (pointer == NULL)
        return NULL;

    uint64_t index = (uint64_t)pointer / PAGE_SIZE;

    for (size_t i = 0; i < page_count; i++)
        bitmap_set_bit(&pmm_bitmap, index + i);

    used_pages_count += page_count;

    return (void *)BIT_TO_PAGE(index);
}

// set status of n pages to unused
void pmm_free(void *pointer, size_t page_count)
{
    uint64_t index = PAGE_TO_BIT(pointer);

    for (size_t i = 0; i < page_count; i++)
        bitmap_unset_bit(&pmm_bitmap, index + i);

    used_pages_count -= page_count;
}

/* utility functions */

// convert a stivale2 memory map entry type to a string
const char *get_memmap_entry_type_string(uint32_t type)
{
    switch (type)
    {
        case STIVALE2_MMAP_USABLE:
            return "Usable";
        case STIVALE2_MMAP_RESERVED:
            return "Reserved";
        case STIVALE2_MMAP_ACPI_RECLAIMABLE:
            return "ACPI Reclaimable";
        case STIVALE2_MMAP_ACPI_NVS:
            return "ACPI Non Volatile Storage";
        case STIVALE2_MMAP_BAD_MEMORY:
            return "Bad Memory";
        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader Reclaimable";
        case STIVALE2_MMAP_KERNEL_AND_MODULES:
            return "Kernel And Modules";
        case STIVALE2_MMAP_FRAMEBUFFER:
            return "Framebuffer";
        default:
            return "Unknown";
    }
}

// search bitmap for contiguous unused bits -> free pages
void *pmm_find_first_free_page_range(size_t page_count)
{
    for (size_t all_bits_i = 0; all_bits_i < PAGE_TO_BIT(highest_page_top); all_bits_i++)
    {
	for (size_t page_count_i = 0; page_count_i < page_count; page_count_i++)
	{
	    if (bitmap_check_bit(&pmm_bitmap, all_bits_i + PAGE_TO_BIT(page_count_i)))
		break;

	    if (page_count_i == page_count - 1)
		return (void *)BIT_TO_PAGE(all_bits_i);
	}
    }

    return NULL;
}
