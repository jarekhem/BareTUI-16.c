#pragma once

#include <stdint.h>

#include "TUI/Widget.h"

typedef struct TUI_Widgets_Label
{
    TUI_Widget base;
    const char* text;
    uint8_t attr;
} TUI_Widgets_Label;

void TUI_Widgets_Label_init(TUI_Widgets_Label* lbl, uint8_t x, uint8_t y, uint8_t w, const char* text, uint8_t attr);
void TUI_Widgets_Label_setAttr(TUI_Widgets_Label* lbl, uint8_t attr);
