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

#ifndef MADT_H
#define MADT_H

typedef struct __attribute__((__packed__))
{
    uint8_t type;
    uint8_t length;
} madt_header_t;

typedef struct __attribute__((__packed__))
{
    madt_header_t header;
    uint8_t acpi_processor_id;
    uint8_t acpi_id;
    uint32_t flags;
} madt_lapic_t;

typedef struct __attribute__((__packed__))
{
    madt_header_t header;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint8_t ioapic_address;
    uint8_t gsi_base;
} madt_ioapic_t;

typedef struct __attribute__((__packed__))
{
    madt_header_t header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} madt_iso_t;

typedef struct __attribute__((__packed__))
{
    madt_header_t header;
    uint8_t acpi_processor_id;
    uint16_t flags;
    uint8_t lint;
} madt_lapic_nmi_t;

typedef struct __attribute__((__packed__))
{
    sdt_t header;
    uint32_t lapic_address;
    uint32_t flags;
    madt_entries_t entries[];
} madt_t;

void madt_init(void);

#endif
