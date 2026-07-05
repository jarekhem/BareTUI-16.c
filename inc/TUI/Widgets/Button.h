#pragma once


#include "TUI/Widget.h"
#include "TUI/Widgets/Label.h"

typedef struct TUI_Widgets_Button
{
    TUI_Widgets_Label base;
    uint8_t attr;
    uint8_t focus_attr;
    uint8_t active_attr;
    void (*action)();

    bool focused;
    bool active;
} TUI_Widgets_Button;


void TUI_Widgets_Button_init(TUI_Widgets_Button* button,
                     uint8_t x,
                     uint8_t y,
                     uint8_t w,
                     const char* text,

                     uint8_t attr,
                     uint8_t focus_attr,
                     uint8_t active_attr,
                     void (*action)());

void TUI_Widgets_Button_setActive(TUI_Widgets_Button* button, bool active);
