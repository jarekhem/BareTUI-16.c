#pragma once

#include "TUI/Containers/Container.h"

typedef struct TUI_Containers_ViewFlipper
{
    TUI_Containers_Container base;
    int active_index;
} TUI_Containers_ViewFlipper;

void TUI_Containers_ViewFlipper_init(TUI_Containers_ViewFlipper* flipper, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void TUI_Containers_ViewFlipper_setActive(TUI_Containers_ViewFlipper* flipper, int index);
