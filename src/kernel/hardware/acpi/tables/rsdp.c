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

#include <hardware/acpi/tables/rsdp.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>

static rsdp_struct_t *rsdp;
static bool has_xsdt_var = false;

/* utility function prototypes */

void rsdp_verify_checksum(uint64_t rsdp_address);

/* core functions */

void rsdp_init(uint64_t rsdp_address)
{
    rsdp_verify_checksum(rsdp_address);

    // debug_set_color(TERM_PURPLE);

    // if (rsdp->revision >= 2)
    // {
    //     has_xsdt_var = true;

    //     debug(INFO, "ACPI Version 2.0 or above is used\n");
    // }
    // else
    // {
    //     debug(INFO, "ACPI Version 1.0 is used\n");
    // }

    // debug_set_color(TERM_COLOR_RESET);
}

rsdp_struct_t *get_rsdp_struct(void)
{
    return rsdp;
}

bool has_xsdt(void)
{
    return has_xsdt_var;
}

/* utility functions */

void rsdp_verify_checksum(uint64_t rsdp_address)
{
    uint8_t checksum = 0;
    uint8_t *ptr = (uint8_t *)rsdp_address;
    uint8_t current_byte;

    log(INFO, "Verifying RSDP checksum:\n");

    debug_set_color(TERM_PURPLE);

    debug("20 first bytes are being checked: ");

    for (uint8_t i = 0; i < 20; i++)
    {
        current_byte = ptr[i];
        debug("%x ", current_byte);

        checksum += current_byte;
    }

    debug("\n");

    debug_set_color(TERM_COLOR_RESET);

    if ((checksum & 0xFF) == 0x00)
    {
        log(INFO, "RSDP checksum is verified\n");
    }
    else
    {
        log(PANIC, "RSDP checksum isn't 0! Checksum: 0x%x\n", checksum & 0xFF);
    }
}
