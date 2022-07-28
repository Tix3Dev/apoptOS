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

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>

#include <utility/utils.h>

#include <libk/serial/debug.h>
typedef struct
{
    char lock;
    bool interrupts;
} spinlock_t;

static inline void spinlock_acquire(spinlock_t *spinlock)
{
    spinlock->interrupts = asm_get_interrupt_flag();
    asm volatile("cli");

    while (__atomic_test_and_set(&spinlock->lock, __ATOMIC_ACQUIRE))
    {
	// debug("bruh\n");
	asm volatile("pause");
    }
}

static inline void spinlock_release(spinlock_t *spinlock)
{
    __atomic_clear(&spinlock->lock, __ATOMIC_RELEASE);

    if (spinlock->interrupts)
    {
	asm volatile("sti");
    }
    else
    {
	asm volatile("cli");
    }
}

#endif
