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

#ifndef APIC_H
#define APIC_H

#include <stdbool.h>
#include <stdint.h>

#define LAPIC_EOI_REG	    0x0B0
#define LAPIC_SPURIOUS_REG  0x0F0
#define LAPIC_ICR0_REG	    0x300
#define LAPIC_ICR1_REG	    0x310

#define LAPIC_ENABLE	    (1 << 8)

void apic_init(void);
void lapic_signal_eoi(void);
void lapic_send_ipi(uint32_t lapic_id, uint8_t vector);
void ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq);

#endif
