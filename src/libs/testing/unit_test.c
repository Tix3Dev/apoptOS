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
    Provides all needed (helper) implementations for basic unit testing -> Framework
    Is used in tests/unit_test_list.c

*/

#include <libs/serial/debug.h>
#include <libs/serial/log.h>
#include <libs/testing/unit_test.h>

unit_test_t tests[MAX_TESTS];
int test_count = 0;

failed_assert_t fails[MAX_FAIL_COUNT];
int fail_count = 0;

/* core functions */

void unit_assert_impl(char *file, int line_nr, bool condition, char *condition_str)
{
    if (condition)
        return;

    if (fail_count >= MAX_FAIL_COUNT)
    {
        log(WARNING, "Unit test - Maximal assert count exceeded!\n");
        return;
    }

    fails[fail_count++] = (failed_assert_t)
    {
        .file_name = file,
        .line_number = line_nr,
        .condition_string = condition_str
    };
}

void unit_test_add(unit_test_t test)
{
    tests[test_count++] = test;
}

void unit_test_run_all(void)
{
    for (int i = 0; i < test_count; i++)
    {
        unit_test_t current_test = tests[i];
        (*current_test.func_ptr)();

        if (fail_count == 0)
            log(SUCCESS, "Unit test for %s passed\n", current_test.func_name);

        log(FAIL, "Unit test for %s failed\n", current_test.func_name);
        debug("Failed asserts:\n");

        for (int j = 0; j < fail_count; j++)
        {
            failed_assert_t current_fail = fails[j];

            debug("	In %s:%d with condition %s\n",
                  current_fail.file_name,
                  current_fail.line_number,
                  current_fail.condition_string
                 );
        }

        // reset fails array by allowing overwriting from the start on
        fail_count = 0;
    }
}
