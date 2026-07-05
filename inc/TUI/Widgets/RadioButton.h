#pragma once

#include "TUI/Widget.h"

#include <stdint.h>

typedef struct TUI_Widgets_RadioButton
{
    TUI_Widget base;
    const char* text;
    int* shared_value;
    int my_value;
    uint8_t normal_attr;
    uint8_t focused_attr;
    bool is_focused;
} TUI_Widgets_RadioButton;


void TUI_Widgets_RadioButton_init(TUI_Widgets_RadioButton* rb,
                      int16_t x,
                      int16_t y,
                      uint16_t w,
                      const char* text,
                      int* shared_value,
                      int my_value,
                      uint8_t normal_attr,
                      uint8_t focused_attr);
