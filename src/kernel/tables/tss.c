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

*/

#include <libk/serial/log.h>
#include <tables/tss.h>

static tss_t tss;

/* core functions */

void tss_init(void)
{
    tss.reserved0 = 0;
    tss.rsp[0] = 0;
    tss.rsp[1] = 0;
    tss.rsp[2] = 0;
    tss.reserved1 = 0;
    tss.ist[0] = 0;
    tss.ist[1] = 0;
    tss.ist[2] = 0;
    tss.ist[3] = 0;
    tss.ist[4] = 0;
    tss.ist[5] = 0;
    tss.ist[6] = 0;
    tss.reserved2 = 0;
    tss.reserved3 = 0;
    tss.reserved4 = 0;
    tss.iopb_offset = 0;

    // TSS is not yet fully initialized, as it needs to be loaded, which happens in gdt.c
}

tss_t *tss_get(void)
{
    return &tss;
}
