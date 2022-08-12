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
    cpuid() is a modified version of this implementation:
	https://github.com/qword-os/qword/blob/2e7899093d597dc55d8ed0d101f5c0509293d62f/src/sys/cpu.h#L67-L79

    Under the following license:
	https://github.com/qword-os/qword/blob/master/LICENSE
*/

#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include <tables/gdt.h>
#include <libk/lock/spinlock.h>
#include <libk/malloc/malloc.h>
#include <libk/printf/printf.h>
#include <utility/utils.h>

typedef struct
{
    uint64_t	cpu_number;
    uint32_t	lapic_id;
    uint32_t	lapic_timer_freq;
    tss_t	tss;
} cpu_local_t;

typedef struct
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    
    uint64_t isr_number;
    uint64_t error_code;
    
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} cpu_interrupt_state_t;

typedef struct
{
    uint32_t leaf;
    uint32_t subleaf;

    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_registers_t;

enum
{
    CPUID_GET_VENDOR_STRING,
    CPUID_GET_FEATURES
};

enum
{
    CPUID_FEAT_ECX_SSE3		= 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL	= 1 << 1,
    CPUID_FEAT_ECX_DTES64	= 1 << 2,
    CPUID_FEAT_ECX_MONITOR	= 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL	= 1 << 4,  
    CPUID_FEAT_ECX_VMX		= 1 << 5,  
    CPUID_FEAT_ECX_SMX		= 1 << 6,  
    CPUID_FEAT_ECX_EST		= 1 << 7,  
    CPUID_FEAT_ECX_TM2		= 1 << 8,  
    CPUID_FEAT_ECX_SSSE3	= 1 << 9,  
    CPUID_FEAT_ECX_CID		= 1 << 10,
    CPUID_FEAT_ECX_SDBG		= 1 << 11,
    CPUID_FEAT_ECX_FMA		= 1 << 12,
    CPUID_FEAT_ECX_CX16		= 1 << 13, 
    CPUID_FEAT_ECX_XTPR		= 1 << 14, 
    CPUID_FEAT_ECX_PDCM		= 1 << 15, 
    CPUID_FEAT_ECX_PCID		= 1 << 17, 
    CPUID_FEAT_ECX_DCA		= 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1	= 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2	= 1 << 20, 
    CPUID_FEAT_ECX_X2APIC	= 1 << 21, 
    CPUID_FEAT_ECX_MOVBE	= 1 << 22, 
    CPUID_FEAT_ECX_POPCNT	= 1 << 23, 
    CPUID_FEAT_ECX_TSC		= 1 << 24, 
    CPUID_FEAT_ECX_AES		= 1 << 25, 
    CPUID_FEAT_ECX_XSAVE	= 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE	= 1 << 27, 
    CPUID_FEAT_ECX_AVX		= 1 << 28,
    CPUID_FEAT_ECX_F16C		= 1 << 29,
    CPUID_FEAT_ECX_RDRAND	= 1 << 30,
    CPUID_FEAT_ECX_HYPERVISOR	= 1 << 31,
    
    CPUID_FEAT_EDX_FPU		= 1 << 0,  
    CPUID_FEAT_EDX_VME		= 1 << 1,  
    CPUID_FEAT_EDX_DE		= 1 << 2,  
    CPUID_FEAT_EDX_PSE		= 1 << 3,  
    CPUID_FEAT_EDX_TSC		= 1 << 4,  
    CPUID_FEAT_EDX_MSR		= 1 << 5,  
    CPUID_FEAT_EDX_PAE		= 1 << 6,  
    CPUID_FEAT_EDX_MCE		= 1 << 7,  
    CPUID_FEAT_EDX_CX8		= 1 << 8,  
    CPUID_FEAT_EDX_APIC		= 1 << 9,  
    CPUID_FEAT_EDX_SEP	    	= 1 << 11, 
    CPUID_FEAT_EDX_MTRR	    	= 1 << 12, 
    CPUID_FEAT_EDX_PGE	    	= 1 << 13, 
    CPUID_FEAT_EDX_MCA	    	= 1 << 14, 
    CPUID_FEAT_EDX_CMO	    	= 1 << 15, 
    CPUID_FEAT_EDX_PAT	    	= 1 << 16, 
    CPUID_FEAT_EDX_PSE36	= 1 << 17, 
    CPUID_FEAT_EDX_PSN		= 1 << 18, 
    CPUID_FEAT_EDX_CLFLUSH	= 1 << 19, 
    CPUID_FEAT_EDX_DS		= 1 << 21, 
    CPUID_FEAT_EDX_ACPI		= 1 << 22, 
    CPUID_FEAT_EDX_MMX		= 1 << 23, 
    CPUID_FEAT_EDX_FXSR		= 1 << 24, 
    CPUID_FEAT_EDX_SSE		= 1 << 25, 
    CPUID_FEAT_EDX_SSE2		= 1 << 26, 
    CPUID_FEAT_EDX_SS		= 1 << 27, 
    CPUID_FEAT_EDX_HTT		= 1 << 28, 
    CPUID_FEAT_EDX_TM		= 1 << 29, 
    CPUID_FEAT_EDX_IA64		= 1 << 30,
    CPUID_FEAT_EDX_PBE		= 1 << 31
};

typedef enum
{
    PAT_UNCACHEABLE	= 0,
    PAT_WRITE_COMBINING = 1,
    PAT_WRITE_THROUGH   = 4,
    PAT_WRITE_PROTECTED = 5,
    PAT_WRITE_BACK	= 6,
    PAT_UNCACHED	= 7
} pat_cache_t;

// retrieve information about cpu through cpuid instruction
static inline int cpuid(cpuid_registers_t *registers)
{
    uint32_t cpuid_max;

    asm volatile("cpuid"
	    : "=a" (cpuid_max)
	    : "a" (registers->leaf & 0x80000000)
	    : "rbx", "rcx", "rdx");

    if (registers->leaf > cpuid_max)
	return 0;

    asm volatile("cpuid"
	    : "=a" (registers->eax), "=b" (registers->ebx), "=c" (registers->ecx), "=d" (registers->edx)
	    : "a" (registers->leaf), "c" (registers->subleaf));

    return 1;
}

// get cpu vendor id string by using cpuid
static inline char *cpu_get_vendor_id_string(void)
{
    cpuid_registers_t *regs = &(cpuid_registers_t)
    {
        .leaf = CPUID_GET_VENDOR_STRING,
        .subleaf = 0,

        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0
    };

    cpuid(regs);

    char *vendor_string = malloc(13);
    snprintf(vendor_string, 13, "%.4s%.4s%.4s",
	    (char *)&regs->ebx, (char *)&regs->edx, (char *)&regs->ecx);

    return vendor_string;
}

static inline void enable_pat(void)
{

    uint64_t uncacheable	= PAT_UNCACHEABLE;
    uint64_t write_combining	= PAT_WRITE_COMBINING;
    uint64_t write_through	= PAT_WRITE_THROUGH;
    uint64_t write_protected    = PAT_WRITE_PROTECTED;
    uint64_t write_back		= PAT_WRITE_BACK;
    uint64_t uncached		= PAT_UNCACHED;

    uint64_t custom_pat_config = uncacheable | (write_combining << 8) | (write_through << 32) |
				(write_protected << 40) | (write_back << 48) | (uncached << 56);

    asm_wrmsr(0x277, custom_pat_config);
}

static inline void enable_sse(void)
{
    uint64_t cr0 = asm_read_cr(0);
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);
    asm_write_cr(0, cr0);

    uint64_t cr4 = asm_read_cr(4);
    cr4 |= (1 << 9);
    cr4 |= (1 << 10);
    asm_write_cr(4, cr4);
}

#endif
