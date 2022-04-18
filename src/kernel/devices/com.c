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
    Provides basic buffer based communication via COM.

*/

#include <kernel/assembly/utils.h>
#include <kernel/devices/com.h>

/* utility function prototypes */

bool com_is_transmit_empty(com_port_t port);
bool com_is_received(com_port_t port);

/* core functions */

// set basic values for a given COM port
void com_init(com_port_t port)
{
    io_outb(port + 1, 0x00);
    io_outb(port + 3, 0x80);
    io_outb(port + 0, 0x03);
    io_outb(port + 1, 0x00);
    io_outb(port + 3, 0x03);
    io_outb(port + 2, 0xC7);
    io_outb(port + 4, 0x0B);
}

// send data (char) to a given COM port
void com_send_char(com_port_t port, char c)
{
    while (!com_is_transmit_empty(port));

    io_outb(port, c);
}

// send data (string) to a given COM port
void com_send_string(com_port_t port, char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        com_send_char(port, str[i]);
}

// get information from a given COM port
char com_recv(com_port_t port)
{
    while (!com_is_received(port));

    return io_inb(port);
}

/* utility functions */

// check if transmission buffer of a given COM port is empty
bool com_is_transmit_empty(com_port_t port)
{
    return io_inb(port + 5) & 0x20;
}

// check if transmission buffer of a given COM port is not empty
bool com_is_received(com_port_t port)
{
    return io_inb(port + 5) & 1;
}
