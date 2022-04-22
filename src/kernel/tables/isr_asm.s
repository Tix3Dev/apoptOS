;   This file is part of a modern x86_64 UNIX-like microkernel which is called apoptOS
;   Everything is openly developed on GitHub: https://github.com/Tix3Dev/apoptOS
;   
;   Copyright (C) 2022  Yves Vollmeier <https://github.com/Tix3Dev>
;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <https://www.gnu.org/licenses/>.

%include "src/kernel/assembly/definitions.inc"

extern isr_handler

global _isr_handler_asm
global _isr_names_asm

_isr_handler_asm:
    cld			; clear DF on function entry
    pusha64		; push all, customly defined in definitions.inc
    
    mov	    rdi, rsp	; pass the stack pointer as an argument to the c function
    call    isr_handler	; call the c function
    mov	    rsp, rax    ; get the new stack pointer returned
   
    popa64		; pop all, customly defined in definitions.inc
    add	    rsp, 16	; pop error code and int number
    
    iretq		; return

%macro ISR_NAME 1
dq _ISR%1
%endmacro

%macro ISR_ERR 1
_ISR%1:
	    ; error code is being pushed automatically by CPU (because "ERR")
    push %1 ; push isr number
    jmp _isr_handler_asm
%endmacro

%macro ISR_NO_ERR 1
_ISR%1:
    push 0  ; push error code by hand as not automatically done by CPU (because "NO_ERR")
    push %1 ; push isr number
    jmp _isr_handler_asm
%endmacro

ISR_NO_ERR  0
ISR_NO_ERR  1
ISR_NO_ERR  2
ISR_NO_ERR  3
ISR_NO_ERR  4
ISR_NO_ERR  5
ISR_NO_ERR  6
ISR_NO_ERR  7
ISR_ERR	    8
ISR_NO_ERR  9
ISR_ERR	    10
ISR_ERR     11
ISR_ERR     12
ISR_ERR     13
ISR_ERR     14
ISR_NO_ERR  15
ISR_NO_ERR  16
ISR_ERR	    17
ISR_NO_ERR  18
ISR_NO_ERR  19
ISR_NO_ERR  20
ISR_ERR	    21
ISR_NO_ERR  22
ISR_NO_ERR  23
ISR_NO_ERR  24
ISR_NO_ERR  25
ISR_NO_ERR  26
ISR_NO_ERR  27
ISR_NO_ERR  28
ISR_NO_ERR  29
ISR_NO_ERR  30
ISR_NO_ERR  31

ISR_NO_ERR  32
ISR_NO_ERR  33
ISR_NO_ERR  34
ISR_NO_ERR  35
ISR_NO_ERR  36
ISR_NO_ERR  37
ISR_NO_ERR  38
ISR_NO_ERR  39
ISR_NO_ERR  40
ISR_NO_ERR  41
ISR_NO_ERR  42
ISR_NO_ERR  43
ISR_NO_ERR  44
ISR_NO_ERR  45
ISR_NO_ERR  46
ISR_NO_ERR  47

; ISR_NO_ERR   128 <- TODO: this will be needed for syscalls

_isr_names_asm:
    ISR_NAME 0
    ISR_NAME 1
    ISR_NAME 2
    ISR_NAME 3
    ISR_NAME 4
    ISR_NAME 5
    ISR_NAME 6
    ISR_NAME 7
    ISR_NAME 8
    ISR_NAME 9
    ISR_NAME 10
    ISR_NAME 11
    ISR_NAME 12
    ISR_NAME 13
    ISR_NAME 14
    ISR_NAME 15
    ISR_NAME 16
    ISR_NAME 17
    ISR_NAME 18
    ISR_NAME 19
    ISR_NAME 20
    ISR_NAME 21
    ISR_NAME 22
    ISR_NAME 23
    ISR_NAME 24
    ISR_NAME 25
    ISR_NAME 26
    ISR_NAME 27
    ISR_NAME 28
    ISR_NAME 29
    ISR_NAME 30
    ISR_NAME 31
    
    ISR_NAME 32
    ISR_NAME 33
    ISR_NAME 34
    ISR_NAME 35
    ISR_NAME 36
    ISR_NAME 37
    ISR_NAME 38
    ISR_NAME 39
    ISR_NAME 40
    ISR_NAME 41
    ISR_NAME 42
    ISR_NAME 43
    ISR_NAME 44
    ISR_NAME 45
    ISR_NAME 46
    ISR_NAME 47

    ; ISR_NAME	128 <- TODO: this will be needed for syscalls
