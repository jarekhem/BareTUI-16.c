#ifndef PANEL_H
#define PANEL_H

#include "Container.h"

typedef struct TUI_Containers_Panel
{
    TUI_Containers_Container base;
    uint8_t bg_attr;
    uint8_t border_attr;
} TUI_Containers_Panel;

void TUI_Containers_Panel_init(TUI_Containers_Panel* panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t bg_attr, uint8_t border_attr);

#endif /* PANEL_H */
