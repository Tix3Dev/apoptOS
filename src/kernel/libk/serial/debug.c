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
    Print debugging information to serial_console via COM.

*/

#include <kernel/devices/com.h>
#include <libs/serial/debug.h>
#include <libs/kprintf/kprintf.h>

const char debug_buffer[5120];

/* core functions */

// variadic function for format specifiers to print debugging information to the serial console
void debug(char *fmt, ...)
{
    va_list ptr;
    va_start(ptr, fmt);
    vsnprintf((char *)&debug_buffer, -1, fmt, ptr);

    com_send_string(COM1, (char *)debug_buffer);

    va_end(ptr);
}

// set bash color code
void debug_set_color(char *color_code)
{
    if (color_code[0] != '\e' || color_code[1] != '[')
        return;

    com_send_string(COM1, color_code);
}
