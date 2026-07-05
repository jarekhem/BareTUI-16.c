#pragma once

#include <stdint.h>

#include "TUI/Widget.h"

typedef struct TUI_Widgets_HLine
{
    TUI_Widget base;
    unsigned char left;
    unsigned char middle;
    unsigned char right;
    uint8_t attr;
} TUI_Widgets_HLine;

void TUI_Widgets_HLine_init(TUI_Widgets_HLine* vline, uint8_t x, uint8_t y, uint8_t w, unsigned char left, unsigned char middle, unsigned char right, uint8_t attr);
