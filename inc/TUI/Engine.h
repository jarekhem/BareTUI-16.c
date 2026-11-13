#pragma once

#include "TUI/Event.h"
#include "TUI/Screen.h"
#include <stdbool.h>
#include <stdint.h>

#define TUI_MAX_NAV_DEPTH 4

typedef void (*TUI_VramFlushFunc)(const uint16_t* lineBuffer, uint8_t y, uint8_t startX, uint8_t endX);

typedef struct TUI_DirtySpan {
    uint8_t startX;
    uint8_t endX;
} TUI_DirtySpan;

typedef struct TUI_NavEntry {
    TUI_ScreenFactory factory;
    TUI_Widget* focused_widget;
} TUI_NavEntry;

typedef struct TUI_Engine
{
    TUI_VramFlushFunc flushFunc;
    void* frontendHeap;
    TUI_Screen* activeScreen;
    TUI_Widget* focusedWidget;

    TUI_NavEntry navStack[TUI_MAX_NAV_DEPTH];
    uint8_t navDepth;
    bool pendingTransition;

    uint16_t lineBuffer[80];
    TUI_DirtySpan dirtyLines[25];
} TUI_Engine;

extern TUI_Engine tuiEngine;

void TUI_Engine_init(TUI_VramFlushFunc flushFunc, void* frontendHeap);
void TUI_Engine_changeScreen(TUI_ScreenFactory factory);
void TUI_Engine_pushScreen(TUI_ScreenFactory factory);
bool TUI_Engine_popScreen(void);
void TUI_Engine_setFocus(TUI_Widget* widget);
TUI_Widget* TUI_Engine_getFocus(void);
void TUI_Engine_invalidateArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void TUI_Engine_poll(const TUI_Event* event);
