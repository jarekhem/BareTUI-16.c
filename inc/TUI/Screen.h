#pragma once

#include "TUI/Containers/Container.h"

typedef struct TUI_Screen TUI_Screen;

typedef struct TUI_Screen
{
    TUI_Containers_Container root;
    void (*set_up)(TUI_Screen*);
    void (*tear_down)(TUI_Screen*);
} TUI_Screen;

void TUI_Screen_init(TUI_Screen* screen);
void TUI_Screen_handleEvent(TUI_Screen* screen, const TUI_Event* event);
