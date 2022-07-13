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

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
 
ENTRY(kmain)
 
PHDRS
{
    null    PT_NULL    FLAGS(0) ;                  
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ;
    rodata  PT_LOAD    FLAGS((1 << 2)) ;           
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ;
    dynamic PT_DYNAMIC FLAGS((1 << 1) | (1 << 2)) ;
}
 
SECTIONS
{
    . = 0xffffffff80200000;
 
    .text : {
        *(.text*)
    } :text
 
    . += 0x1000;
 
    .stivale2hdr : {
        KEEP(*(.stivale2hdr))
    } :rodata
 
    .rodata : {
        *(.rodata*)
    } :rodata
 
    . += 0x1000;
 
    .data : {
        *(.data*)
    } :data
 
    .dynamic : {
        *(.dynamic)
    } :data :dynamic
 
    .bss : {
        *(COMMON)
        *(.bss*)
    } :data
}
