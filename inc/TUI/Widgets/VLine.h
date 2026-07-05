#pragma once

#include <stdint.h>

#include "TUI/Widget.h"

typedef struct TUI_Widgets_VLine
{
    TUI_Widget base;
    unsigned char top;
    unsigned char middle;
    unsigned char bottom;
    uint8_t attr;
} TUI_Widgets_VLine;

void TUI_Widgets_VLine_init(TUI_Widgets_VLine* vline, uint8_t x, uint8_t y, uint8_t h, unsigned char top, unsigned char middle, unsigned char bottom, uint8_t attr);
