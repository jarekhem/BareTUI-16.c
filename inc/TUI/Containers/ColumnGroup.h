#pragma once

#include "TUI/Containers/Container.h"
#include <stdint.h>


typedef struct TUI_Containers_ColumnGroup
{
    TUI_Containers_Container base;
    bool is_focused;
} TUI_Containers_ColumnGroup;

void TUI_Containers_ColumnGroup_init(TUI_Containers_ColumnGroup* col, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
bool TUI_Containers_ColumnGroup_addItem(TUI_Containers_ColumnGroup* col, TUI_Widget* item);
