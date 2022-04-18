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

#include <stdbool.h>

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#define MAX_TESTS 1024
#define MAX_FAIL_COUNT 1024

typedef struct
{
    char *func_name;
    void (*func_ptr)(void);
} unit_test_t;

typedef struct 
{
    char *file_name;
    int line_number;
    char *condition_string;
} failed_assert_t;

void unit_assert_impl(char *file, int line_nr, bool condition, char *condition_str);
void unit_test_add(unit_test_t test);
void unit_test_run_all(void);

// Inspired by:
// https://github.com/Abb1x/tisix/blob/e86e7a836dddb38443499e2257a1077ebf1b9449/src/lib/tisix/test.hpp#L17
// Under following license:
// https://github.com/Abb1x/tisix/blob/f79de11dae657b2b5e082c4a0057192763d6c15a/LICENSE
// Explanation: The constructor attribute will run unit_test_constructor<whatever the function name is>
// automatically before kmain

#define UNIT_TEST(func_name)						    \
    void unit_test_##func_name(void);					    \
    __attribute__((constructor))					    \
    static inline void unit_test_constructor_##func_name(void)		    \
    {									    \
	unit_test_add((unit_test_t){#func_name, unit_test_##func_name});    \
    }									    \
    void unit_test_##func_name(void)

#define UNIT_ASSERT(condition) unit_assert_impl(__FILE__, __LINE__, condition, #condition)

// Example: Expansion of this macro for
/*
UNIT_TEST(malloc)
{
    void *ptr = malloc(1337);

    some_assert(ptr != NULL);
}
*/
// will result in the following:
/*
void unit_test_malloc(void);

__attribute__((__constructor__))
static inline void unit_test_constructor_malloc(void)
{
    unit_test_add((unit_test_t){"malloc", unit_test_malloc});
} 

void unit_test_malloc(void)
{
    void *ptr = malloc(1337);

    some_assert(ptr != NULL);
}
*/

#endif
