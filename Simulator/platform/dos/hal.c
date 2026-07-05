#include "../../Simulator/inc/vga_hal.h"
#include <assert.h>

#define NASM(code) ".intel_syntax noprefix\n\t" code "\n\t.att_syntax prefix"

uint16_t vga_segment;

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__("inb %%dx, %%al" : "=a"(ret) : "d"(port));
    return ret;
}


static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ __volatile__("outb %%al, %%dx" : : "a"(val), "d"(port));
}

void vga_enable_16color_bg(void)
{
    __asm__ volatile (
        "mov $0x1003, %%ax\n"
        "mov $0x00, %%bl\n"
        "int $0x10\n"
        : /* no outputs */
        : /* no inputs */
        : "ax", "bx" /* clobbered registers */
    );
}


void vga_draw_char(uint8_t x, uint8_t y, char ch, uint8_t attr)
{
    assert(x < 80 && y < 25);

    uint16_t offset = (y * 80) + x;
    uint16_t val = (uint16_t)(((uint16_t)attr << 8) | (uint8_t)ch);

    /* Safely write to B800:offset using ES segment override */
    __asm__ __volatile__(
        "push %%es\n\t"
        "movw %0, %%es\n\t"
        "movw %%ax, %%es:(%%bx)\n\t"
        "pop %%es"
        :
        : "r" (vga_segment), "a" (val), "b" (offset * 2)
        : "memory"
    );
}


void vga_draw_block(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* buffer)
{
    for (uint8_t row = 0; row < h; row++)
    {
        for (uint8_t col = 0; col < w; col++)
        {
            uint16_t cell = buffer[(row * w) + col];
            char ch = (char)(cell & 0xFF);
            uint8_t attr = (uint8_t)(cell >> 8);
            vga_draw_char(x + col, y + row, ch, attr);
        }
    }
}

bool vga_init(void)
{
    vga_segment = 0xB800;
    outb(0x3D4, 0x0A); // Wybierz rejestr 0x0A (Cursor Start Register)
    outb(0x3D5, 0x20); // Ustaw 5. bit na '1' (Disable Cursor)
    vga_enable_16color_bg();
    return true;
}

void vga_lock(void)
{
    if (0)
    {
        /* Czekamy aż bit 0 (Display Enable) będzie jedynką.
         * Oznacza to bezpieczny moment na zapis (Retrace). */
        while ((inb(0x3DA) & 0x01) == 0);
    }
}

void vga_unlock(void)
{
}

void vga_set_cursor(uint8_t x, uint8_t y)
{
    /* Zabezpieczenie przed wyjściem poza ekran */
    if (x >= VGA_COLS) x = VGA_COLS - 1;
    if (y >= VGA_ROWS) y = VGA_ROWS - 1;

    /* Obliczenie liniowego adresu w VRAM (0 - 1999) */
    uint16_t pos = y * VGA_COLS + x;

    /* 1. WŁĄCZENIE KURSORA (Kształt: standardowa linia na dole) */
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x0E); /* Start skanowania na linii 14 (zeruje bit 5 - disable) */

    outb(0x3D4, 0x0B);
    outb(0x3D5, 0x0F); /* Koniec skanowania na linii 15 */

    /* 2. USTAWIENIE POZYCJI KURSORA */
    /* Zapisz młodszy bajt (Low Byte) do rejestru 0x0F */
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    /* Zapisz starszy bajt (High Byte) do rejestru 0x0E */
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_shutdown(void)
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x0E); // Włącz kursor, start na linii skanowania 14
    outb(0x3D4, 0x0B);
    outb(0x3D5, 0x0F); // Koniec na linii skanowania 15
}

bool vga_get_key(uint16_t* keycode)
{
    uint16_t ax_out, flags_out;

    /* Check keyboard buffer via BIOS INT 16h */
    __asm__ volatile (
        NASM(
            "int 0x16\n\t"
            "pushf\n\t"
            "pop dx"
        )
        : "=a"(ax_out), "=d"(flags_out)
        : "a"(0x0100)
    );

    if ((flags_out & 0x40) == 0)
    {
        /* Buffer has data, read it */
        __asm__ volatile ("int $0x16" : "=a"(ax_out) : "a"(0x0000));
        *keycode = ax_out;
        return true;
    }
    return false;
}

int main(void)
{
    app_main();
    return 0;
}
