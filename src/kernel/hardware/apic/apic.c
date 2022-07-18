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
uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg_offset);
void ioapic_write_reg(size_t ioapic_i, uint8_t reg_offset, uint32_t data);

/* core functions */

// make lapic_address global, disable PIC to enable LAPIC
void apic_init(void)
{
    if (!apic_is_available())
    {
	log(PANIC, "No APIC was found on this computer!\n");
    }

    lapic_address = PHYS_TO_HIGHER_HALF_DATA(madt->lapic_address);

    pic_disable();
    lapic_enable();

    log(INFO, "APIC initialized\n");
}

// signal an end of interrupt
void lapic_signal_eoi(void) // TODO: test this
{
    lapic_write_reg(LAPIC_EOI_REG, 0);
}

// send an interrupt to another LAPIC by using interprocessor interrupts
void lapic_send_ipi(uint32_t lapic_id, uint8_t vector) // TODO: test this
{
    lapic_write_reg(LAPIC_ICR1_REG, lapic_id << 24);
    lapic_write_reg(LAPIC_ICR0_REG, vector);
}

void ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq)
{
    //
}

/* utility functions */

// check CPUID status over availability of APIC
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

// get data from a LAPIC register
uint32_t lapic_read_reg(uint32_t reg)
{
    return *((volatile uint32_t *)(lapic_address + reg));
}

// write data to a LAPIC register
void lapic_write_reg(uint32_t reg, uint32_t data)
{
    *((volatile uint32_t *)(lapic_address + reg)) = data;
}

// enable LAPIC by setting 'default interrupt' (spurious interrupt) to IRQ 255
void lapic_enable(void)
{
    lapic_write_reg(LAPIC_SPURIOUS_REG,
	    lapic_read_reg(LAPIC_SPURIOUS_REG) | LAPIC_ENABLE | SPURIOUS_INT);
}

// read data from a IOAPIC register - IOAPIC is custom
uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg_offset)
{
    uint32_t volatile *current_io_apic_base = (uint32_t volatile *)madt_ioapics[ioapic_i];

    *current_io_apic_base = reg_offset;

    return *(current_io_apic_base + 0x10);
}

// write data to a IOAPIC register - IOAPIC is custom
void ioapic_write_reg(size_t ioapic_i, uint8_t reg_offset, uint32_t data)
{
    uint32_t volatile *current_ioapic_address = (uint32_t volatile *)madt_ioapics[ioapic_i];

    *current_ioapic_address = reg_offset;

    *(current_ioapic_address + 0x10) = data;
}
