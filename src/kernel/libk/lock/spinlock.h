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

    As we can never be sure if an IRQ occurs while in a spinlock, we have to clear the
    interrupt flag, so all interrupts are ignored and dropped. Before that, the interrupt
    flag state has to be saved in order to go back to the old state once the spinlock is released.

    The rest of the concept is just to "spin" (i.e. wait) until the lock isn't set anymore.
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
    spinlock->interrupts = asm_get_interrupt_flag();
    asm volatile("cli");

    while (__atomic_test_and_set(&spinlock->lock, __ATOMIC_ACQUIRE))
    {
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
