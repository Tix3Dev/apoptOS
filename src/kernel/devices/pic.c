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
    Basic interaction with the programmable interrupt controller.

*/

#include <assembly/utils.h>
#include <devices/pic.h>

/* core functions */

// disable PIC e.g. to use LAPIC and IOAPIC
void pic_disable(void)
{
    io_outb(PIC2_DATA, 0xFF);
    io_outb(PIC1_DATA, 0xFF);
}

// remap/initialize the PIC
void pic_remap(void)
{
    uint8_t mask1 = io_inb(PIC1_DATA);
    uint8_t mask2 = io_inb(PIC2_DATA);

    io_outb(PIC1_COMMAND, 0x11);
    io_outb(PIC2_COMMAND, 0x11);
    io_wait();

    io_outb(PIC1_DATA, 0x20);
    io_outb(PIC2_DATA, 0x28);
    io_wait();

    io_outb(PIC1_DATA, 0x04);
    io_outb(PIC2_DATA, 0x02);
    io_wait();

    io_outb(PIC1_DATA, 0x01);
    io_outb(PIC2_DATA, 0x01);
    io_wait();

    io_outb(PIC1_DATA, 0x00);
    io_outb(PIC2_DATA, 0x00);
    io_wait();

    io_outb(PIC1_DATA, ICW4_8086);
    io_wait();
    io_outb(PIC2_DATA, ICW4_8086);
    io_wait();

    io_outb(PIC1_DATA, mask1);
    io_outb(PIC2_DATA, mask2);
}

// set interrupt mask register to ignore incoming IRQ's
void pic_set_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8)
	port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = io_inb(port) | (1 << irq_line);
    io_outb(port, value);
}

// "undo" pic_set_mask
void pic_clear_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = io_inb(port) & ~(1 << irq_line);
    io_outb(port, value);
}

// signal an end of interrupt
void pic_signal_EOI(uint64_t isr_number)
{
    // check if IRQ comes from slave PIC
    if (isr_number >= 40)
        io_outb(PIC2_COMMAND, 0x20);

    // IRQ comes from master (and from slave) PIC
    io_outb(PIC1_COMMAND, 0x20);
}
