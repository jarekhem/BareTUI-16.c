#pragma once

#include "TUI/Widget.h"

typedef struct TUI_Containers_Container
{
    TUI_Widget base;
    TUI_Widget* first_child;
    TUI_Widget* focused_child;
} TUI_Containers_Container;

void TUI_Containers_Container_init(TUI_Containers_Container* c, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void TUI_Containers_Container_add(TUI_Containers_Container* c, TUI_Widget* child);
void TUI_Containers_Container_render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX);
bool TUI_Containers_Container_handleEvent(TUI_Widget* self, const TUI_Event* event);
void TUI_Containers_Container_setFocus(TUI_Containers_Container* c, TUI_Widget* child);
