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
    blah

*/

#include <hardware/acpi/tables/madt.h>
#include <hardware/apic/apic.h>
#include <hardware/pic/pic.h>
#include <hardware/cpu.h>
#include <libk/serial/log.h>
#include <memory/mem.h>
#include <tables/isr.h>

static uintptr_t lapic_address;

/* utility function prototypes */

bool apic_is_available(void);
uint32_t lapic_read_reg(uint32_t reg);
void lapic_write_reg(uint32_t reg, uint32_t data);
void lapic_enable(void);
void ioapic_set_gsi_redirect(void);

/* core functions */

void apic_init(void)
{
    if (!apic_is_available())
    {
	log(PANIC, "No APIC was found on this computer!\n");
    }

    lapic_address = PHYS_TO_HIGHER_HALF_DATA(madt->lapic_address);

    log(WARNING, "lapic_address: 0x%llx\n", lapic_address);

    pic_disable();

    lapic_enable();

    log(WARNING, "lapic spurious reg: 0x%llx\n", lapic_read_reg(LAPIC_SPURIOUS_REG));
    log(WARNING, "lapic revision: %d\n", lapic_read_reg(0x30));

    log(INFO, "APIC initialized\n");
}

void lapic_signal_eoi(void)
{
    //
}

void lapic_send_ipi(void) // TODO: uint8_t lapic_id, uint8_t vector
{
    //
}

void ioapic_set_irq_redirect(void) // TODO: uint8_t lapic_id, uint8_t vector, uint8_t irq
{
    //
}

/* utility functions */

bool apic_is_available(void)
{

    cpuid_registers_t *regs = &(cpuid_registers_t)
    {
        .leaf = CPUID_GET_FEATURES,
        .subleaf = 0,

        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0
    };

    cpuid(regs);

    if (regs->edx & CPUID_FEAT_EDX_APIC)
        return true;

    return false;
}

uint32_t lapic_read_reg(uint32_t reg)
{
    return *((volatile uint32_t *)lapic_address + reg);
}

void lapic_write_reg(uint32_t reg, uint32_t data)
{
    *((volatile uint32_t *)(lapic_address + reg)) = data;
}

void lapic_enable(void)
{
    lapic_write_reg(LAPIC_SPURIOUS_REG,
	    lapic_read_reg(LAPIC_SPURIOUS_REG) | LAPIC_ENABLE | SPURIOUS_INT);
}
