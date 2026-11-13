#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "Event.h"

typedef struct TUI_Widget TUI_Widget;

typedef struct TUI_Widget
{
    uint8_t x, y;
    uint8_t width, height;
    bool focusable;
    bool is_container;
    bool visible;

    TUI_Widget* parent;
    TUI_Widget* next_sibling;

    /* Render directly to the off-screen line buffer. */
    void (*render)(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX);
    bool (*handle_event)(TUI_Widget* self, const TUI_Event* event);
} TUI_Widget;

/* Universal API for each element */
void TUI_Widget_init(TUI_Widget* d, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool focusable);
void TUI_Widget_invalidate(TUI_Widget* d);
uint8_t TUI_Widget_getAbsoluteX(const TUI_Widget* d);
uint8_t TUI_Widget_getAbsoluteY(const TUI_Widget* d);
