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
    one LAPIC (thus LAPIC functions will be executed by the LAPIC of the calling CPU)
    and there is normally one global IOAPIC. As there are two types of APIC, utilites
    are split up too. Also provides LAPIC timer for per-CPU time management.

*/

#include <hardware/acpi/tables/madt.h>
#include <hardware/apic/apic.h>
#include <hardware/hpet/hpet.h>
#include <hardware/pic/pic.h>
#include <hardware/cpu.h>
#include <libk/data_structs/bitmap.h>
#include <libk/malloc/malloc.h>
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <memory/mem.h>
#include <tables/isr.h>

static uintptr_t lapic_address;
static uint32_t lapic_timer_freq = 0; // TODO: remove

/* utility function prototypes */

bool apic_is_available(void);

uint32_t lapic_read_reg(uint32_t reg);
void lapic_write_reg(uint32_t reg, uint32_t data);
uint32_t lapic_timer_calibrate(uint32_t ms);

uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg_offset);
void ioapic_write_reg(size_t ioapic_i, uint8_t reg_offset, uint32_t data);
uint32_t ioapic_get_max_redirect(size_t ioapic_i);
size_t ioapic_i_from_gsi(uint32_t gsi);
void ioapic_set_gsi_redirect(uint32_t lapic_id, uint8_t vector, uint32_t gsi, uint16_t flags, bool mask);

/* core functions */

// disable PIC, enable LAPIC, mask all (legacy) ISA IRQ's and start LAPIC timer
void apic_init(void)
{
    if (!apic_is_available())
    {
	log(PANIC, "No APIC was found on this computer!\n");
    }

    lapic_address = PHYS_TO_HIGHER_HALF_DATA(madt->lapic_address);

    pic_disable();
    lapic_enable();

    bitmap_t irq_bitmap;
    irq_bitmap.size = 16;
    irq_bitmap.map = (uint8_t *)malloc(irq_bitmap.size);
    memset((void *)irq_bitmap.map, 0, irq_bitmap.size);

    for (uint32_t i = 0; i < 16; i++)
    {
	if (!bitmap_check_bit(&irq_bitmap, i))
	{
	    uint32_t irq = ioapic_set_irq_redirect(lapic_get_id(), i + 32, i, true);
	    bitmap_set_bit(&irq_bitmap, irq);
	}
    }

    hpet_init();
    lapic_timer_init();
    
    log(INFO, "APIC initialized\n");
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

// signal an end of interrupt
void lapic_signal_eoi(void)
{
    lapic_write_reg(LAPIC_EOI_REG, 0);
}

// send an interrupt to another LAPIC by using interprocessor interrupts
void lapic_send_ipi(uint32_t lapic_id, uint8_t vector) // TODO: test this
{
    lapic_write_reg(LAPIC_ICR1_REG, lapic_id << 24);
    lapic_write_reg(LAPIC_ICR0_REG, vector);
}

// globally set the lapic timer frequency through calibration
void lapic_timer_init(void)
{
    // frequency = ticks per second -> calibrate 1ms -> multiply by 1000 to get one second
    lapic_timer_freq = lapic_timer_calibrate(1000) * 1000; // TODO: this_cpu()->lapic_timer_freq

    log(INFO, "LAPIC timer initialized - Oneshot's possible from now on\n");
}

// send an interrupt vector (through the LVT and not IOAPIC redirection table) after
// the specified amount of microseconds, where interrupt vector = LAPIC_TIMER_INT
void lapic_timer_oneshot(uint32_t us)
{
    uint32_t ticks = us * (lapic_timer_freq / 1000000); // TODO: this_cpu()->lapic_timer_freq 

    lapic_write_reg(LAPIC_TIMER_REG, LAPIC_TIMER_INT);
    lapic_write_reg(LAPIC_TIMER_DIV_REG, 3);
    lapic_write_reg(LAPIC_TIMER_INITCNT_REG, ticks);
}

// tell the IOAPIC to always redirect the specified IRQ (pin) to a specified LAPIC,
// which then always should use a specified vector - flags are set according to ISO's
uint32_t ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq, bool mask)
{
    for (size_t isos_i = 0; isos_i < madt_isos_i; isos_i++)
    {
        if (madt_isos[isos_i]->irq_source == irq)
        {
            log(INFO, "Resolving ISO -> GSI: %d | IRQ: %d\n", madt_isos[isos_i]->gsi, irq);

            ioapic_set_gsi_redirect(lapic_id, vector, madt_isos[isos_i]->gsi,
        	    madt_isos[isos_i]->flags, mask);

            return madt_isos[isos_i]->gsi;
        }
    }
    
    ioapic_set_gsi_redirect(lapic_id, vector, irq, 0, mask);

    return irq;
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

// return how many ticks it took the LAPIC timer for the specified amount of microseconds
uint32_t lapic_timer_calibrate(uint32_t us)
{
    lapic_write_reg(LAPIC_TIMER_DIV_REG, 3);

    uint32_t initial_count = 0xFFFFFFFF;
    lapic_write_reg(LAPIC_TIMER_INITCNT_REG, initial_count);

    hpet_usleep(us);

    lapic_write_reg(LAPIC_TIMER_REG, LAPIC_TIMER_DISABLE_BIT);
    uint32_t final_count = lapic_read_reg(LAPIC_TIMER_CURCNT_REG);

    uint32_t total_ticks = initial_count - final_count;
    return total_ticks;
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
