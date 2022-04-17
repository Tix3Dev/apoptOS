#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
 
void kmain(struct stivale2_struct *stivale2_struct)
{
    for (;;)
	asm volatile("hlt");
}
