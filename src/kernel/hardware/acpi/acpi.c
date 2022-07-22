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
    Provide utilities to interact with ACPI tables. Initialize most
    important APCI tables: RSDT/XSDT and MADT.

*/

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <hardware/acpi/tables/madt.h>
#include <hardware/acpi/tables/rsdp.h>
#include <hardware/acpi/tables/rsdt.h>
#include <hardware/acpi/acpi.h>
#include <libk/serial/log.h>
#include <libk/string/string.h>
#include <memory/mem.h>

static rsdt_t *rsdt;
static xsdt_t *xsdt;

/* utility function prototypes */

bool acpi_verify_sdt_checksum(sdt_t *sdt, const char *signature);

/* core functions */

// get RSDT entries and use it to initialize MADT
void acpi_init(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_rsdp *rsdp_tag = stivale2_get_tag(stivale2_struct,
            STIVALE2_STRUCT_TAG_RSDP_ID);

    rsdp_init(rsdp_tag->rsdp);

    if (has_xsdt())
    {
        xsdt = (xsdt_t *)PHYS_TO_HIGHER_HALF_DATA(get_rsdp_struct()->xsdt_address);

        // last and final check to verify ACPI is supported
        if (!acpi_verify_sdt(&xsdt->header, "XSDT"))
        {
            log(PANIC, "No ACPI was found on this computer!\n");
        }
    }
    else
    {
        rsdt = (rsdt_t *)PHYS_TO_HIGHER_HALF_DATA(get_rsdp_struct()->rsdt_address);

        // last and final check to verify ACPI is supported
        if (!acpi_verify_sdt(&rsdt->header, "RSDT"))
        {
            log(PANIC, "No ACPI was found on this computer!\n");
        }
    }

    madt_init();

    log(INFO, "ACPI initialized\n");
}

// compare passed signature with desired signature and sum bytes up for checksum
bool acpi_verify_sdt(sdt_t *sdt, const char *signature)
{
    return (memcmp(sdt->signature, signature, 4) == 0) &&
           acpi_verify_sdt_checksum(sdt, signature);
}

// search array of entries in RSDT for SDT header with the desired signature
sdt_t *acpi_find_sdt(const char *signature)
{
    size_t entry_count;

    if (has_xsdt())
    {
        entry_count = (xsdt->header.length - sizeof(xsdt->header)) / 8;
    }
    else
    {
        entry_count = (rsdt->header.length - sizeof(rsdt->header)) / 4;
    }

    sdt_t *current_entry;

    for (size_t i = 0; i < entry_count; i++)
    {
        current_entry = (sdt_t *)(has_xsdt() ? xsdt->entries[i] : rsdt->entries[i]);

        if (acpi_verify_sdt(current_entry, signature))
        {
            return (sdt_t *)PHYS_TO_HIGHER_HALF_DATA((uintptr_t)current_entry);
        }
    }

    log(PANIC, "Could not find SDT with signature '%s'!\n", signature);

    return NULL;
}

/* utility functions */

// sum up first bytes according to length, if zero verification was successful
bool acpi_verify_sdt_checksum(sdt_t *sdt, const char *signature)
{
    uint8_t checksum = 0;
    uint8_t *ptr = (uint8_t *)sdt;

    for (uint8_t i = 0; i < sdt->length; i++)
    {
        checksum += ptr[i];
    }

    checksum = checksum & 0xFF;

    if (checksum == 0)
    {
        log(INFO, "%s checksum is verified\n", signature);

        return true;
    }
    else
    {
        log(PANIC, "%s checksum isn't 0! Checksum: 0x%x\n", signature, checksum);

        return false;
    }
}
