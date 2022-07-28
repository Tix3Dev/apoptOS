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

#define LAPIC_ID_REG		0x020
#define LAPIC_EOI_REG		0x0B0
#define LAPIC_SPURIOUS_REG	0x0F0
#define LAPIC_ICR0_REG		0x300
#define LAPIC_ICR1_REG		0x310
#define LAPIC_TIMER_REG		0x320
#define LAPIC_TIMER_INITCNT_REG	0x380
#define LAPIC_TIMER_CURCNT_REG	0x390
#define LAPIC_TIMER_DIV_REG	0x3E0

#define LAPIC_ENABLE_BIT	(1 << 8)
#define LAPIC_TIMER_DISABLE_BIT	(1 << 16)

#define IOREGSEL    0
#define IOWIN	    0x10

#define IOAPICID_REG	0
#define IOAPICVER_REG	1

#define IOAPIC_PINPOL_BIT   (1 << 13)
#define IOAPIC_TRIGMODE_BIT (1 << 15)
#define IOAPIC_MASK_BIT	    (1 << 16)

#define IRQ_TO_IOREDTBL_REG(irq) (((irq) * 2) + 0x10)

void apic_init(void);
uint8_t lapic_get_id(void);
void lapic_signal_eoi(void);
void lapic_send_ipi(uint32_t lapic_id, uint8_t vector);
void lapic_timer_oneshot(uint32_t us);
uint32_t ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq, bool mask);

#endif
