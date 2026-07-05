#pragma once

#include "TUI/Event.h"
#include "TUI/Screen.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*TUI_VramFlushFunc)(const uint16_t* lineBuffer, uint8_t y, uint8_t startX, uint8_t endX);

typedef struct TUI_DirtySpan {
    uint8_t startX;
    uint8_t endX;
} TUI_DirtySpan;

typedef struct TUI_Engine
{
    TUI_VramFlushFunc flushFunc;
    TUI_Screen* activeScreen;
    uint16_t lineBuffer[80];
    TUI_DirtySpan dirtyLines[25];
} TUI_Engine;

extern TUI_Engine tuiEngine;

void TUI_Engine_init(TUI_VramFlushFunc flushFunc);
void TUI_Engine_changeScreen(TUI_Screen* screen);
void TUI_Engine_invalidateArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void TUI_Engine_poll(const TUI_Event* event);
