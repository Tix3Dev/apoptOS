/*
	This file is part of an x86_64 hobbyist operating system called KnutOS
	Everything is openly developed on GitHub: https://github.com/Tix3Dev/KnutOS/

	Copyright (C) 2021-2022  Yves Vollmeier <https://github.com/Tix3Dev>
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

#include <hardware/cpu.h>
#include <hardware/pic/pic.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>

static const char *exceptions[] =
{
    "#DE: Divide Error",
    "#DB: Debug Exception",
    " — : NMI Interrupt",
    "#BP: Breakpoint",
    "#OF: Overflow",
    "#BR: BOUND Range Exceeded",
    "#UD: Invalid Opcode (Undefined Opcode)",
    "#NM: Device Not Available (No Math Coprocessor)",
    "#DF: Double Fault",
    "— : Coprocessor Segment Overrun (reserved)",
    "#TS: Invalid TSS",
    "#NP: Segment Not Present",
    "#SS: Stack-Segment Fault",
    "#GP: General Protection",
    "#PF: Page Fault",
    "— : (Intel reserved. Do not use.)",
    "#MF: x87 FPU Floating-Point Error (Math Fault)",
    "#AC: Alignment Check",
    "#MC: Machine Check",
    "#XM: SIMD Floating-Point Exception",
    "#VE: Virtualization Exception",
    "#CP: Control Protection Exception",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use.",
    "— : Intel reserved. Do not use."
};

static const char *isa_irqs[] =
{
    "Programmable Interrupt Timer",
    "Keyboard",
    "Cascade",
    "COM2",
    "COM1",
    "LPT2",
    "Floppy Disk",
    "LPT1 / Unreliable spurious interrupt",
    "CMOS real-time clock",
    "Free for peripherals / legacy SCSI / NIC",
    "Free for peripherals / SCSI / NIC",
    "Free for peripherals / SCSI / NIC",
    "PS2 Mouse",
    "FPU / Coprocessor / Inter-processor",
    "Primary ATA Hard Disk",
    "Secondary ATA Hard Disk"
};

uint64_t isr_handler(uint64_t rsp)
{
    interrupt_cpu_state_t *cpu = (interrupt_cpu_state_t*)rsp;

    // handle exceptions
    if (cpu->isr_number < 32)
    {
        debug_set_color(TERM_RED);
        debug("\n────────────────────────\n");
        debug("⚠ EXCEPTION OCCURRED! ⚠\n\n");
        debug("⤷ ISR-No. %d: %s\n", cpu->isr_number, exceptions[cpu->isr_number]);
        debug("⤷ Error code: 0x%.16llx\n\n\n", cpu->error_code);
        debug_set_color(TERM_CYAN);
        debug("ℹ Register dump:\n\n");
        debug("⤷ rax: 0x%.16llx, rbx:    0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n"
              "⤷ rsi: 0x%.16llx, rdi:    0x%.16llx, rbp: 0x%.16llx, r8 : 0x%.16llx\n"
              "⤷ r9 : 0x%.16llx, r10:    0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n"
              "⤷ r13: 0x%.16llx, r14:    0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n"
              "⤷ rsp: 0x%.16llx, rflags: 0x%.16llx, cs : 0x%.16llx, rip: 0x%.16llx\n",
              cpu->rax, cpu->rbx,    cpu->rcx, cpu->rdx,
              cpu->rsi, cpu->rdi,    cpu->rbp, cpu->r8,
              cpu->r9,  cpu->r10,    cpu->r11, cpu->r12,
              cpu->r13, cpu->r14,    cpu->r15, cpu->ss,
              cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

        debug_set_color(TERM_COLOR_RESET);

        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }
    // handle ISA IRQ's
    else if (cpu->isr_number >= 32 && cpu->isr_number < 48)
    {
        // check for specific IRQ, like keyboard interrupts
        // otherwise continue here:

        debug_set_color(TERM_RED);
        debug("\n────────────────────────\n");
        debug("⚠ UNKNOWN HARDWARE INTERRUPT OCCURRED! ⚠\n\n");
        debug("⤷ ISR-No. %d: %s\n", cpu->isr_number, isa_irqs[cpu->isr_number - 32]);
        debug_set_color(TERM_CYAN);
        debug("ℹ Register dump:\n\n");
        debug("⤷ rax: 0x%.16llx, rbx:    0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n"
              "⤷ rsi: 0x%.16llx, rdi:    0x%.16llx, rbp: 0x%.16llx, r8 : 0x%.16llx\n"
              "⤷ r9 : 0x%.16llx, r10:    0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n"
              "⤷ r13: 0x%.16llx, r14:    0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n"
              "⤷ rsp: 0x%.16llx, rflags: 0x%.16llx, cs : 0x%.16llx, rip: 0x%.16llx\n",
              cpu->rax, cpu->rbx,    cpu->rcx, cpu->rdx,
              cpu->rsi, cpu->rdi,    cpu->rbp, cpu->r8,
              cpu->r9,  cpu->r10,    cpu->r11, cpu->r12,
              cpu->r13, cpu->r14,    cpu->r15, cpu->ss,
              cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

        debug_set_color(TERM_COLOR_RESET);

        pic_signal_eoi(cpu->isr_number);
    }
    // handle syscalls
    else if (cpu->isr_number == 128)
    {
        // TODO syscalls

        debug_set_color(TERM_RED);
        debug("\n────────────────────────\n");
        debug("⚠ SYSCALL OCCURRED - UNHANDLED FOR NOW! ⚠\n\n");
        debug("⤷ ISR-No. %d\n", cpu->isr_number);
        // debug("⤷ Error code: 0x%.16llx\n\n\n", cpu->error_code);
        debug_set_color(TERM_CYAN);
        debug("ℹ Register dump:\n\n");
        debug("⤷ rax: 0x%.16llx, rbx:    0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n"
              "⤷ rsi: 0x%.16llx, rdi:    0x%.16llx, rbp: 0x%.16llx, r8 : 0x%.16llx\n"
              "⤷ r9 : 0x%.16llx, r10:    0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n"
              "⤷ r13: 0x%.16llx, r14:    0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n"
              "⤷ rsp: 0x%.16llx, rflags: 0x%.16llx, cs : 0x%.16llx, rip: 0x%.16llx\n",
              cpu->rax, cpu->rbx,    cpu->rcx, cpu->rdx,
              cpu->rsi, cpu->rdi,    cpu->rbp, cpu->r8,
              cpu->r9,  cpu->r10,    cpu->r11, cpu->r12,
              cpu->r13, cpu->r14,    cpu->r15, cpu->ss,
              cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

        debug_set_color(TERM_COLOR_RESET);
    }
    // handle unknown interrupts
    else
    {
        debug_set_color(TERM_RED);
        debug("\n────────────────────────\n");
        debug("⚠ UNKNOWN INTERRUPT OCCURRED! ⚠\n\n");
        debug("⤷ ISR-No. %d\n", cpu->isr_number);
        debug_set_color(TERM_CYAN);
        debug("ℹ Register dump:\n\n");
        debug("⤷ rax: 0x%.16llx, rbx:    0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n"
              "⤷ rsi: 0x%.16llx, rdi:    0x%.16llx, rbp: 0x%.16llx, r8 : 0x%.16llx\n"
              "⤷ r9 : 0x%.16llx, r10:    0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n"
              "⤷ r13: 0x%.16llx, r14:    0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n"
              "⤷ rsp: 0x%.16llx, rflags: 0x%.16llx, cs : 0x%.16llx, rip: 0x%.16llx\n",
              cpu->rax, cpu->rbx,    cpu->rcx, cpu->rdx,
              cpu->rsi, cpu->rdi,    cpu->rbp, cpu->r8,
              cpu->r9,  cpu->r10,    cpu->r11, cpu->r12,
              cpu->r13, cpu->r14,    cpu->r15, cpu->ss,
              cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

        debug_set_color(TERM_COLOR_RESET);

        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }

    return rsp;
}
