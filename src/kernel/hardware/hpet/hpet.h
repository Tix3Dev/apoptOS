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

#ifndef HPET_H
#define HPET_H

#include <stddef.h>
#include <stdint.h>

#include <hardware/acpi/tables/sdt.h>

typedef struct __attribute__((__packed__))
{
    sdt_t header;
    uint8_t hardware_rev_id;
    uint8_t info; 
    uint16_t pci_id;
    uint8_t address_space_id;
    uint8_t register_width;
    uint8_t register_offset;
    uint8_t reserved;
    uint64_t address;
    uint8_t hpet_num;
    uint16_t minimum_ticks;
    uint8_t page_protection;
} hpet_t;

typedef struct __attribute__((packed))
{
    uint64_t capabilities;
    uint64_t reserved0;
    uint64_t general_config;
    uint64_t reserved1;
    uint64_t int_status;
    uint64_t reserved2;
    uint64_t reserved3[24];
    volatile uint64_t counter_value;
    uint64_t unused4;
} hpet_regs_t;

void hpet_init(void);
void hpet_usleep(uint64_t us);
uint64_t hpet_get_counter_value(void);

#endif
