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

#include <stdint.h>

#ifndef PIC_H
#define PIC_H

#define PIC1		0x20	    // I/O base address for master PIC
#define PIC2		0xA0	    // I/O base address for slave PIC
#define PIC1_COMMAND	PIC1	    // master PIC command port
#define PIC1_DATA	(PIC1+1)    // master PIC data port
#define PIC2_COMMAND	PIC2	    // slave PIC command port
#define PIC2_DATA	(PIC2+1)    // slave PIC data port
#define ICW4_8086	0x01	    // 8086/88 (MCS-80/85) mode

void pic_disable(void);
void pic_remap(void);
void pic_set_mask(unsigned char irq_line);
void pic_clear_mask(unsigned char irq_line);
void pic_signal_EOI(uint64_t isr_number);

#endif
