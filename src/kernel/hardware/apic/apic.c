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
    Utilities to make switching from PIC to APIC possible and thus allowing
    for a sophisticated multiprocessor interrupt management. Each processor has
    one LAPIC and there is normally one global IOAPIC. As there are two types of
    APIC, utilites are split up too.

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
uint8_t lapic_get_id(void);

uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg_offset);
void ioapic_write_reg(size_t ioapic_i, uint8_t reg_offset, uint32_t data);
uint32_t ioapic_get_max_redirect(size_t ioapic_i);
size_t ioapic_i_from_gsi(uint32_t gsi);
void ioapic_set_gsi_redirect(uint32_t lapic_id, uint8_t vector, uint32_t gsi, uint16_t flags, bool mask);
void ioapic_redirect_all_isa_irqs(void);

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

    ioapic_redirect_all_isa_irqs();
    
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

// tell the IOAPIC to always redirect the specified IRQ (pin) to a specified LAPIC,
// which then always should use a specified vector - flags are set according to ISO's
void ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq, bool mask)
{
    for (size_t isos_i = 0; isos_i < madt_isos_i; isos_i++)
    {
        if (madt_isos[isos_i]->irq_source == irq)
        {
            log(INFO, "Resolving ISO -> GSI: %d | IRQ: %d\n", madt_isos[isos_i]->gsi, irq);

            ioapic_set_gsi_redirect(lapic_id, vector, madt_isos[isos_i]->gsi,
        	    madt_isos[isos_i]->flags, mask);

            return;
        }
    }
    
    ioapic_set_gsi_redirect(lapic_id, vector, irq, 0, mask);
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
	    lapic_read_reg(LAPIC_SPURIOUS_REG) | LAPIC_ENABLE_BIT | SPURIOUS_INT);
}

// get the LAPIC ID of the current CPU
uint8_t lapic_get_id(void)
{
    return (uint8_t)(lapic_read_reg(LAPIC_ID_REG) >> 24);
}

// read data from a IOAPIC register - IOAPIC is custom
uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg)
{
    size_t ioapic_i_addr = PHYS_TO_HIGHER_HALF_DATA(madt_ioapics[ioapic_i]->ioapic_address);

    *((volatile uint32_t *)(ioapic_i_addr + IOREGSEL)) = reg;
    return *((volatile uint32_t *)(ioapic_i_addr + IOWIN));
}

// write data to a IOAPIC register - IOAPIC is custom
void ioapic_write_reg(size_t ioapic_i, uint8_t reg, uint32_t data)
{
    size_t ioapic_i_addr = PHYS_TO_HIGHER_HALF_DATA(madt_ioapics[ioapic_i]->ioapic_address);

    *((volatile uint32_t *)(ioapic_i_addr + IOREGSEL)) = reg;
    *((volatile uint32_t *)(ioapic_i_addr + IOWIN)) = data;
}

// get how any IRQ's (i.e. redirects) this IOAPIC can handle
uint32_t ioapic_get_max_redirect(size_t ioapic_i)
{
    return (ioapic_read_reg(ioapic_i, IOAPICVER_REG) & 0xFF0000) >> 16;
}

// find IOREDTBL entry according to GSI, retrieve vector from it
uint8_t ioapic_get_vector_from_gsi(uint32_t gsi)
{
    size_t ioapic_i = ioapic_i_from_gsi(gsi);
    uint32_t ioredtbl = IRQ_TO_IOREDTBL_REG(gsi - madt_ioapics[ioapic_i]->gsi_base);
    
    uint64_t entry = ioapic_read_reg(ioapic_i, ioredtbl) |
	((uint64_t)ioapic_read_reg(ioapic_i, ioredtbl + 1) << 32);

    return (uint8_t)(entry & 0xFF);
}

// iterate through all IOAPIC's and check if the GSI is in current IOAPIC's range
size_t ioapic_i_from_gsi(uint32_t gsi)
{
    uint32_t start_gsi;
    uint32_t end_gsi;

    for (size_t ioapic_i = 0; ioapic_i < madt_ioapics_i; ioapic_i++)
    {
	start_gsi = madt_ioapics[ioapic_i]->gsi_base;
	end_gsi = start_gsi + ioapic_get_max_redirect(ioapic_i);

	if (gsi >= start_gsi && gsi < end_gsi)
	{
	    return ioapic_i;
	}
    }

    log(PANIC, "Couldn't find an IOAPIC for GSI %d!\n", gsi);
    return 0;
}

// set fields in redirection entry according to MADT ISO struct and save entry in IOREDTBL register
void ioapic_set_gsi_redirect(uint32_t lapic_id, uint8_t vector, uint32_t gsi, uint16_t flags, bool mask)
{
    uint64_t redirect_entry = vector;

    // if flags.pin_polarity is active low (else active high)
    if (flags & 2)
    {
	redirect_entry |= IOAPIC_PINPOL_BIT;
    }

    // if flags.trigger_mode is level triggered (else edge triggered)
    if (flags & 8)
    {
	redirect_entry |= IOAPIC_TRIGMODE_BIT;
    }

    if (mask)
    {
        redirect_entry |= IOAPIC_MASK_BIT; // set bit -> mask
    }
    else
    {
        redirect_entry &= ~IOAPIC_MASK_BIT; // clear bit -> unmask
    }

    // set destination
    redirect_entry |= (uint64_t)lapic_id << 56;

    size_t ioapic_i = ioapic_i_from_gsi(gsi);
    uint32_t ioredtbl = IRQ_TO_IOREDTBL_REG(gsi - madt_ioapics[ioapic_i]->gsi_base);

    ioapic_write_reg(ioapic_i, ioredtbl, (uint32_t)redirect_entry);
    ioapic_write_reg(ioapic_i, ioredtbl + 1, (uint32_t)(redirect_entry >> 32));
}

// redirect if necessary and mask all ISA IRQ's
void ioapic_redirect_all_isa_irqs(void)
{
    // first redirect all ISO's
    for (size_t isos_i = 0; isos_i < madt_isos_i; isos_i++)
    {
	ioapic_set_gsi_redirect(lapic_get_id(), madt_isos[isos_i]->irq_source + 32,
		madt_isos[isos_i]->gsi, madt_isos[isos_i]->flags, true);
    }

    // and then redirect the rest of ISA IRQ's, skipping the already set ISO's
    uint8_t vector = 0;
    for (uint8_t gsi = 0; gsi < 16; gsi++, vector++)
    {
	// check if current GSI is already set, if yes decrease vector
	// so next iteration it's still the same
	if (ioapic_get_vector_from_gsi(gsi) >= 32)
	{
	    vector--;
	    continue;
	}

retry:
	// check if current vector is from a ISO, if yes retry
	for (size_t isos_i = 0; isos_i < madt_isos_i; isos_i++)
	{
	    if (madt_isos[isos_i]->irq_source == vector)
	    {
		vector++;
		goto retry;
	    }
	}

	ioapic_set_gsi_redirect(lapic_get_id(), vector + 32, gsi, 0, true);
    }	
}
