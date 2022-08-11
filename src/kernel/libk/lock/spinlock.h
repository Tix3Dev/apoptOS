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
    Spinlock implementation.

    The basic concept is to "spin" (i.e. wait) until a custom lock isn't locked (i.e. set) anymore.

    But because when an interrupt occurs while in code which is locked by foo, the interrupt handler
    could also try to access foo, thus creating a deadlock. To prevent this, interrupts are disabled
    for locked code parts (and must not be changed!), but after the code is released, the old interrupt
    state is restored.
*/




#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>

#include <utility/utils.h>

typedef struct
{
    char lock;
    bool interrupts;
} spinlock_t;

static inline void spinlock_acquire(spinlock_t *spinlock)
{
    while (__atomic_test_and_set(&spinlock->lock, __ATOMIC_ACQUIRE))
    {
	asm volatile("pause");
    }

    spinlock->interrupts = asm_get_interrupt_flag();

    asm volatile("cli");
}

static inline void spinlock_release(spinlock_t *spinlock)
{
    if (spinlock->interrupts)
    {
	asm volatile("sti");
    }
    else
    {
	asm volatile("cli");
    }

    __atomic_clear(spinlock, __ATOMIC_RELEASE);
}

#endif
