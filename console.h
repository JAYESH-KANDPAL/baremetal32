#ifndef BAREMETAL32_CONSOLE_H
#define BAREMETAL32_CONSOLE_H

#include <stdarg.h>

void console_init(void);
void console_clear(void);
void console_set_color(unsigned char foreground, unsigned char background);
void console_putc(char c);
void console_write(const char *text);
void kprintf(const char *format, ...);

#endif
