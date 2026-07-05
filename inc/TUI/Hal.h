#pragma once

#include <stdint.h>

typedef struct
{
    void (*lock)(void);
    void (*unlock)(void);
    void (*draw_char)(uint8_t x, uint8_t y, char ch, uint8_t attr);
    void (*set_cursor)(uint8_t x, uint8_t y);
    void (*draw_block)(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* buffer);
} TUI_Hal;
