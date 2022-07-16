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
    Iterate through all Multiple APIC Description Table entries and 
    make them global to access.

*/

#include <boot/stivale2.h>
#include <hardware/acpi/tables/madt.h>
#include <hardware/acpi/acpi.h>
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>

madt_t *madt;

madt_lapic_t	    **madt_lapics;
madt_ioapic_t	    **madt_ioapics;
madt_iso_t	    **madt_isos;
madt_lapic_nmi_t    **madt_lapic_nmis;

size_t madt_lapics_i	    = 0;
size_t madt_ioapics_i	    = 0;
size_t madt_isos_i	    = 0;
size_t madt_lapic_nmis_i    = 0;

/* core functions */

// get + store madt and it's entries
void madt_init(void)
{
    madt = (madt_t *)(uintptr_t)acpi_find_sdt("APIC");

    if (madt == NULL)
    {
	log(PANIC, "No MADT was found on this computer!\n");
    }

    madt_lapics	    = malloc(256);
    madt_ioapics    = malloc(256);
    madt_isos	    = malloc(256);
    madt_lapic_nmis = malloc(256);

    uint8_t *start = (uint8_t *)&madt->entries;
    size_t end = (size_t)&madt->header + madt->header.length;

    for (uint8_t *entry_ptr = start; (size_t)entry_ptr < end; entry_ptr += *(entry_ptr + 1))
    {
	switch (*entry_ptr)
	{
	    case 0:
		log(INFO, "madt_init(): Found LAPIC at 0x%llx\n", entry_ptr);
		madt_lapics[madt_lapics_i++] = (madt_lapic_t *)entry_ptr;
		break;

	    case 1:
		log(INFO, "madt_init(): Found IOAPIC at 0x%llx\n", entry_ptr);
		madt_ioapics[madt_ioapics_i++] = (madt_ioapic_t *)entry_ptr;
		break;

	    case 2:
		log(INFO, "madt_init(): Found Interrupt Source Override at 0x%llx\n", entry_ptr);
		madt_isos[madt_isos_i++] = (madt_iso_t *)entry_ptr;
		break;

	    case 4:
		log(INFO, "madt_init(): Found Non Maskable Interrupt at 0x%llx\n", entry_ptr);
		madt_lapic_nmis[madt_lapic_nmis_i++] = (madt_lapic_nmi_t *)entry_ptr;
		break;
	}
    }

    log(INFO, "MADT initialized\n");
}
