#include <assert.h>

#include "../../Simulator/inc/vga_hal.h"
#include <stddef.h>
#include <stdlib.h>

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

/* Natywne kody AT (Scancode Set 2) */
#define AT_MAKE_ESC     0x76
#define AT_MAKE_ENTER   0x5A
#define AT_MAKE_UP      0x75
#define AT_MAKE_DOWN    0x72
#define AT_MAKE_LEFT    0x6B
#define AT_MAKE_RIGHT   0x74
#define AT_MAKE_SPACE   0x29

bool kbd_init(void);
bool kbd_get_scancode(uint8_t* out_scancode);

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
    kbd_init();
    /* Point the global buffer directly to the physical VGA text mode memory */
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

bool vga_get_key(uint16_t* out_key)
{
    /* Zmienna statyczna pamięta stan pomiędzy wywołaniami pętli */
    static bool expect_break = false;
    uint8_t scancode;

    if (kbd_get_scancode(&scancode))
    {
        /* 1. Prefix 0xE0 oznacza klawisz rozszerzony (np. szare strzałki).
         * Ignorujemy go, bo prawdziwy kod przyjdzie w następnym bajcie. */
        if (scancode == 0xE0)
            return false;

        /* 2. Prefix 0xF0 oznacza, że NASTĘPNY odebrany bajt to puszczenie klawisza. */
        if (scancode == 0xF0)
        {
            expect_break = true;
            return false;
        }

        /* 3. Jeśli poprzednio dostaliśmy 0xF0, to ten bajt jest kodem
         * zwalnianego klawisza. Ignorujemy go i resetujemy flagę. */
        if (expect_break)
        {
            expect_break = false;
            return false;
        }

        /* 4. Skoro dotarliśmy tutaj, mamy czysty, wciśnięty klawisz (Make Code)! */
        switch (scancode)
        {
        case AT_MAKE_UP: *out_key = TUI_KEY_UP;
            return true;
        case AT_MAKE_DOWN: *out_key = TUI_KEY_DOWN;
            return true;
        case AT_MAKE_LEFT: *out_key = TUI_KEY_LEFT;
            return true;
        case AT_MAKE_RIGHT: *out_key = TUI_KEY_RIGHT;
            return true;
        case AT_MAKE_ENTER: *out_key = TUI_KEY_ENTER;
            return true;
        case AT_MAKE_ESC: *out_key = TUI_KEY_ESC;
            return true;
        case AT_MAKE_SPACE: *out_key = TUI_KEY_SPACE;
            return true;
        }
    }
    return false;
}

void vga_set_cursor(uint8_t x, uint8_t y)
{
    /* Zabezpieczenie przed wyjściem poza ekran */
    if (x >= VGA_COLS)
        x = VGA_COLS - 1;
    if (y >= VGA_ROWS)
        y = VGA_ROWS - 1;

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

static uint8_t printText(uint8_t x, uint8_t y, const char* str, uint8_t attr)
{
    assert(str);

    while (*str != '\0' && x < 80)
        vga_draw_char(x++, y, *str++, attr);

    return x;
}


static void clearScreen(uint8_t attr)
{
    vga_set_cursor(0, 0);
    for (int i = 0; i < 80 * 25; i++)
        vga_draw_char(i % 80, i / 80, ' ', attr);
}

void __attribute__((noreturn)) __assert_func(const char* file, int line)
{
    /* 0x4F = White text on Red background for a panic screen */
    uint8_t attr = 0x4F;
    clearScreen(attr);

    printText(2, 1, "=== ASSERTION FAILED ===", attr);

    uint8_t x = 2;
    x = printText(x, 3, "File : ", attr);
    x = printText(x, 3, file ? file : "?", attr);
    x = printText(x, 3, " : ", attr);

    char line_buf[8];
    itoa(line, line_buf, 10);
    printText(x, 3, line_buf, attr);

    asm volatile (
        "xchg %bx, %bx\n\t"
    );

    while (1);
}
