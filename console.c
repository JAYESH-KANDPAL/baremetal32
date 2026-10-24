#include "console.h"

#define VGA_WIDTH  80u
#define VGA_HEIGHT 25u
#define VGA_MEMORY ((volatile unsigned short *)0xB8000u)

static unsigned int cursor_row;
static unsigned int cursor_column;
static unsigned char text_color = 0x07;

static unsigned char make_color(unsigned char foreground,
                                unsigned char background)
{
    return (unsigned char)((background << 4) | (foreground & 0x0Fu));
}

static unsigned short make_entry(char c, unsigned char color)
{
    return (unsigned short)((unsigned char)c | ((unsigned short)color << 8));
}

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void update_hardware_cursor(void)
{
    unsigned short position =
        (unsigned short)(cursor_row * VGA_WIDTH + cursor_column);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

static void scroll_if_needed(void)
{
    unsigned int row;
    unsigned int column;

    if (cursor_row < VGA_HEIGHT)
        return;

    for (row = 1; row < VGA_HEIGHT; ++row) {
        for (column = 0; column < VGA_WIDTH; ++column) {
            VGA_MEMORY[(row - 1) * VGA_WIDTH + column] =
                VGA_MEMORY[row * VGA_WIDTH + column];
        }
    }

    for (column = 0; column < VGA_WIDTH; ++column) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + column] =
            make_entry(' ', text_color);
    }

    cursor_row = VGA_HEIGHT - 1;
}

void console_set_color(unsigned char foreground, unsigned char background)
{
    text_color = make_color(foreground, background);
}

void console_clear(void)
{
    unsigned int i;

    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i)
        VGA_MEMORY[i] = make_entry(' ', text_color);

    cursor_row = 0;
    cursor_column = 0;
    update_hardware_cursor();
}

void console_init(void)
{
    text_color = make_color(7, 0); /* Light gray on black. */
    console_clear();
}

void console_putc(char c)
{
    switch (c) {
    case '\n':
        cursor_column = 0;
        ++cursor_row;
        break;
    case '\r':
        cursor_column = 0;
        break;
    case '\b':
        if (cursor_column > 0) {
            --cursor_column;
            VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_column] =
                make_entry(' ', text_color);
        }
        break;
    case '\t':
        do {
            console_putc(' ');
        } while ((cursor_column % 4u) != 0u);
        return;
    default:
        VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_column] =
            make_entry(c, text_color);
        ++cursor_column;
        if (cursor_column >= VGA_WIDTH) {
            cursor_column = 0;
            ++cursor_row;
        }
        break;
    }

    scroll_if_needed();
    update_hardware_cursor();

}

void console_write(const char *text)
{
    if (text == 0)
        return;

    while (*text != '\0') {
        console_putc(*text);
        ++text;
    }
}

static void print_unsigned(unsigned int value, unsigned int base)
{
    char buffer[32];
    unsigned int length = 0;

    if (value == 0) {
        console_putc('0');
        return;
    }

    while (value != 0 && length < sizeof(buffer)) {
        unsigned int digit = value % base;
        buffer[length++] = (char)(digit < 10 ? '0' + digit : 'a' + digit - 10);
        value /= base;
    }

    while (length > 0)
        console_putc(buffer[--length]);
}

void kprintf(const char *format, ...)
{
    va_list args;

    va_start(args, format);

    while (*format != '\0') {
        if (*format != '%') {
            console_putc(*format++);
            continue;
        }

        ++format;
        if (*format == '\0')
            break;

        switch (*format) {
        case '%':
            console_putc('%');
            break;
        case 'c':
            console_putc((char)va_arg(args, int));
            break;
        case 's': {
            const char *s = va_arg(args, const char *);
            console_write(s != 0 ? s : "(null)");
            break;
        }
        case 'd': {
            int value = va_arg(args, int);
            unsigned int magnitude;

            if (value < 0) {
                console_putc('-');
                magnitude = 0u - (unsigned int)value;
            } else {
                magnitude = (unsigned int)value;
            }
            print_unsigned(magnitude, 10);
            break;
        }
        case 'u':
            print_unsigned(va_arg(args, unsigned int), 10);
            break;
        case 'x':
            print_unsigned(va_arg(args, unsigned int), 16);
            break;
        case 'p':
            console_write("0x");
            print_unsigned((unsigned int)va_arg(args, void *), 16);
            break;
        default:
            console_putc('%');
            console_putc(*format);
            break;
        }

        ++format;
    }

    va_end(args);
}
