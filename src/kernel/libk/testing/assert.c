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

*/

#include <libk/serial/log.h>
#include <libk/testing/assert.h>

/* core functions */

void assert_impl(char *file, int line_nr, bool condition, char *condition_str)
{
    if (!condition)
        log(PANIC, "Assert failed at: %s:%d with %s as the condition\n", file, line_nr, condition_str);
}
