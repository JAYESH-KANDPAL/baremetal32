#include "console.h"

void kmain(void)
{
    console_init();

    console_set_color(11, 0); /* Light cyan on black. */
    kprintf("BareMetal32");
    console_set_color(7, 0);
    kprintf(" - a tiny x86 hobby kernel\n");
    kprintf("----------------------------------------\n");
    kprintf("Boot status : OK\n");
    kprintf("Architecture: 32-bit x86\n");
    kprintf("Console     : VGA text mode\n");
    kprintf("Hex test    : 0x%x\n", 0xC0FFEEu);
    kprintf("Signed test : %d\n", -32);
    kprintf("Unsigned    : %u\n", 2026u);
    kprintf("\nHello from kmain(). The kernel is alive.\n");
    kprintf("Next: add tests, serial logging, and memory management.\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
