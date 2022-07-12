/*
	This file is part of a modern x86_64 UNIX-like microkernel which is called apoptOS
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

#include <boot/stivale2.h>
#include <boot/stivale2_boot.h>
#include <hardware/acpi/tables/rsdp.h>
#include <hardware/acpi/tables/rsdt.h>
#include <hardware/acpi/acpi.h>
#include <libk/serial/log.h>
#include <memory/mem.h>

static rsdt_structure_t *rsdt;

/* utility function prototypes */

bool acpi_verify_sdt_checksum(sdt_t *sdt, const char signature);

/* core functions */

void acpi_init(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_rsdp *rsdp_tag = stivale2_get_tag(stivale2_struct,
            STIVALE2_STRUCT_TAG_RSDP_ID);

    rsdp_init(rsdp_tag->rsdp);

    // rsdt = (rsdt_structure_t *)PHYS_TO_HIGHER_HALF_DATA((uintptr_t)get_rsdp_struct()->rsdt_address);

    // having a RSDT is equivalent to having ACPI supported
    // if (acpi_verify_sdt(&rsdt->header, "RSDT") != 0)
    //     log(PANIC, "No ACPI was found on this computer!\n");
    
    // madt_init();

    log(INFO, "ACPI initialized\n");
}

bool acpi_verify_sdt(sdt_t *sdt, const char *signature)
{
    //
}

sdt_t *acpi_find_sdt(const char *signature)
{
    //
}

/* utility functions */

bool acpi_verify_sdt_checksum(sdt_t *sdt, const char signature)
{
    //
}
