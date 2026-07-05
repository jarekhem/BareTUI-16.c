#include "TUI/Engine.h"
#include "MainScreen.h"
#include "vga_hal.h"
#include <stddef.h>

bool g_en_uart = false;
bool g_en_dma = true;

/* Explicitly declare your HAL implementation */
extern void vga_draw_char(uint8_t x, uint8_t y, char ch, uint8_t attr);
extern void vga_draw_block(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* buffer);
extern void vga_lock(void);
extern void vga_unlock(void);

static void VramFlush(const uint16_t* lineBuffer, uint8_t y, uint8_t startX, uint8_t endX)
{
    uint8_t width = endX - startX + 1;
    vga_lock();
    vga_draw_block(startX, y, width, 1, lineBuffer + startX);
    vga_unlock();
}

/* Basic primitive cleanup routine (replace with your HAL equivalent) */
static void RawClearScreen(void)
{
    for (int i = 0; i < 80 * 25; i++)
    {
        vga_draw_char(i % 80, i / 80, ' ', 0x07);
    }
}

void app_main(void)
{
    vga_init();
    RawClearScreen();

    /* 2. Init UI Engine and Screens */
    MainScreen main_screen;

    MainScreen_Init(&main_screen, &g_en_uart, &g_en_dma);
    TUI_Engine_init(VramFlush);
    TUI_Engine_changeScreen(&main_screen.base);

    bool is_running = true;
    /* 3. Run Event Loop */
    while (is_running)
    {
        uint16_t key;

        if (vga_get_key(&key))
        {
            if (key == TUI_KEY_ESC)
            {
                is_running = false;
                continue;
            }

            TUI_Event e;
            e.type = TUI_EVENT_KEY;
            e.key.keycode = key;
            TUI_Engine_poll(&e);
        }
        else
        {
            TUI_Engine_poll(NULL);
        }
    }

    /* 4. Teardown & POST continuation */
    RawClearScreen();

    /* Resume native 8086 execution, boot payload, etc. */
}
