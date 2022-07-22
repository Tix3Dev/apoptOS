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
    Provides a sleep function and function to get the counter to measure time between events
    using the HPET.

*/

#include <boot/stivale2.h>
#include <hardware/hpet/hpet.h>
#include <hardware/acpi/acpi.h>
#include <memory/mem.h>

static volatile hpet_t *hpet;
static volatile hpet_regs_t *hpet_regs;

/* core functions */

// get HPET and set necessary variables
void hpet_init(void)
{
    hpet = (hpet_t *)(uintptr_t)acpi_find_sdt("HPET");
    hpet_regs = (hpet_regs_t *)(PHYS_TO_HIGHER_HALF_DATA(hpet->address));

    hpet_regs->counter_value = 0;
    hpet_regs->general_config = 1;
}

// wait for us nanoseconds
void hpet_usleep(size_t us)
{
    uint32_t clock_period = hpet_regs->capabilities >> 32;

    volatile size_t target_value = hpet_regs->counter_value + (us * (1000000000 / clock_period));

    while (hpet_regs->counter_value < target_value)
    {
	asm volatile("pause");
    }
}

// return the current counter value
uint64_t hpet_get_counter_value(void)
{
    return hpet_regs->counter_value;
}
