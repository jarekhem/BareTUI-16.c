#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define KBC_DATA    0x60
#define KBC_CMD     0x64
#define KBC_STATUS  0x64

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

/* Klasyczne inżynierskie opóźnienie szyny ISA.
 * Zapis do portu 0x80 (Port POST) wymusza na procesorze
 * odczekanie cykli sprzętowych szyny. */
static inline void io_wait(void)
{
    __asm__ __volatile__("outb %%al, $0x80" : : "a"(0));
}

/* Oczekiwanie aż bufor wejściowy KBC będzie pusty (można wysyłać) */
static bool kbc_wait_write(void)
{
    uint16_t timeout = 0xFFFF;
    while ((inb(KBC_STATUS) & 0x02) && --timeout) io_wait();
    return timeout > 0;
}

/* Oczekiwanie aż w buforze wyjściowym KBC pojawią się dane (można czytać) */
static bool kbc_wait_read(void)
{
    /* Używamy długiego timeoutu (ponad 300ms na 16MHz), bo test BAT tyle trwa */
    uint32_t timeout = 0x7FFFF;
    while (!(inb(KBC_STATUS) & 0x01) && --timeout) io_wait();
    return timeout > 0;
}

/* Agresywne opróżnianie bufora ze śmieci */
static void kbc_flush(void)
{
    uint16_t timeout = 0x7FFF;
    while (--timeout)
    {
        if (inb(KBC_STATUS) & 0x01)
        {
            inb(KBC_DATA);
        }
        io_wait();
    }
}

void kbd_init(void)
{
    kbc_flush();

    /* --- KROK 1: KBC Self-Test (Magia Kiseleva) --- */
    kbc_wait_write();
    outb(KBC_CMD, 0xAA);
    if (!kbc_wait_read() || inb(KBC_DATA) != 0x55)
        assert(false && "KBC Error");

    /* --- KROK 2: Test Interfejsu Klawiatury --- */
    kbc_wait_write();
    outb(KBC_CMD, 0xAB);
    if (!kbc_wait_read() || inb(KBC_DATA) != 0x00)
        assert(false && "CLK or DATA Line error");


    /* --- KROK 3: Włączenie interfejsu klawiatury w 8042 --- */
    kbc_wait_write();
    outb(KBC_CMD, 0xAE);
    kbc_flush(); /* Czysty start przed resetem fizycznym */

    /* --- KROK 4: Reset Klawiatury (BAT) --- */
    kbc_wait_write();
    outb(KBC_DATA, 0xFF);

    /* Czekamy na potwierdzenie przyjęcia komendy (ACK) */
    if (!kbc_wait_read() || inb(KBC_DATA) != 0xFA)
        assert(false && "KB RESET failed");

    /* Czekamy na wynik testu BAT (W TYM MOMENCIE MRUGAJĄ DIODY) */
    if (!kbc_wait_read() || inb(KBC_DATA) != 0xAA)
        assert(false && "KB TEST failed");

    /* --- KROK 5: Konfiguracja - Wyłączenie Translacji (czysty Set 2) i wyłączenie IRQ --- */
    kbc_wait_write();
    outb(KBC_CMD, 0x20); /* Komenda: Czytaj Command Byte */
    if (!kbc_wait_read())
        assert(false && "KBC CONFIG failed");

    uint8_t cfg = inb(KBC_DATA);

    cfg &= ~0x40; /* Wyczyść Bit 6: Disable Translation (zostawiamy natywny Set 2 AT) */
    cfg &= ~0x01; /* Wyczyść Bit 0: Disable IRQ1 (Działamy w Pollingu) */

    kbc_wait_write();
    outb(KBC_CMD, 0x60); /* Komenda: Zapisz Command Byte */
    kbc_wait_write();
    outb(KBC_DATA, cfg);

    kbc_flush();
}

bool kbd_get_scancode(uint8_t* out_scancode)
{
    assert(out_scancode);

    if (inb(KBC_STATUS) & 0x01)
    {
        *out_scancode = inb(KBC_DATA);
        return true;
    }
    return false;
}
