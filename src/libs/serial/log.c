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

#include <kprintf/kprintf.h>
#include <serial/debug.h>
#include <serial/log.h>

const char log_buffer[5120];

/* core functions */

void log_impl(char *file, int line_nr, log_status_t status, char *fmt, ...)
{
    va_list ptr;
    va_start(ptr, fmt);
    vsnprintf((char *)&log_buffer, -1, fmt, ptr);

    if (status == INFO)
    {
        debug_set_color(TERM_CYAN);
        debug("[INFO]    | %s:%d ─→ %s", file, line_nr, (char *)log_buffer);
    }
    else if (status == WARNING)
    {
        debug_set_color(TERM_YELLOW);
        debug("[WARNING] | %s:%d ─→ %s", file, line_nr, (char *)log_buffer);
    }
    else if (status == PANIC)
    {
        debug_set_color(TERM_RED);
        debug("[PANIC]   | %s:%d ─→ %s", file, line_nr, (char *)log_buffer);

	for (;;)
	    asm volatile("cli; hlt");
    }

    debug_set_color(TERM_COLOR_RESET);
}