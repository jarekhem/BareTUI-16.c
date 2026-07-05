
#pragma once

#include <stdint.h>

#include "TUI/Widget.h"


typedef struct TUI_Widgets_Checkbox
{
    TUI_Widget base; /* MUST be first! */
    const char* text;
    bool* value_ref; /* Pointer to a physical variable in RAM */
    bool is_focused; /* Highlight state */
    uint8_t normal_attr;
    uint8_t focused_attr;
} TUI_Widgets_Checkbox;

void TUI_Widgets_Checkbox_init(TUI_Widgets_Checkbox* cb, int16_t x, int16_t y, uint16_t w, const char* text, bool* value_ref, uint8_t normal_attr, uint8_t focused_attr);
