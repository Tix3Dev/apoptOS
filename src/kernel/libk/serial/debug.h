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

#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

// bash color codes
#define TERM_BLACK	    "\e[0;30m"
#define TERM_RED	    "\e[0;31m"
#define TERM_GREEN	    "\e[0;32m"
#define TERM_YELLOW	    "\e[0;33m"
#define TERM_BLUE	    "\e[0;34m"
#define TERM_PURPLE	    "\e[0;35m"
#define TERM_CYAN	    "\e[0;36m"
#define TERM_WHITE	    "\e[0;37m"
#define TERM_COLOR_RESET    "\e[0m"

void debug(char *fmt, ...);
void debug_set_color(char *color_code);

#endif
