#include "TUI/Widget.h"

#include <assert.h>
#include <stddef.h>

void TUI_Widget_init(TUI_Widget* d, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool focusable)
{
    assert(d);

    d->x = x;
    d->y = y;
    d->width = w;
    d->height = h;
    d->dirty = true;
    d->focusable = focusable;
    d->interactive = focusable; // Default behavior
    d->visible = true;

    d->parent = NULL;
    d->next_sibling = NULL;

    d->handle_event = NULL;
}

#include "TUI/Engine.h"

void TUI_Widget_invalidate(TUI_Widget* d)
{
    assert(d);

    d->dirty = true;
    TUI_Engine_invalidateArea(TUI_Widget_getAbsoluteX(d), TUI_Widget_getAbsoluteY(d), d->width, d->height);
}

uint8_t TUI_Widget_getAbsoluteX(const TUI_Widget* d)
{
    uint8_t abs_x = 0;
    while (d != NULL)
    {
        abs_x += d->x;
        d = d->parent;
    }
    return abs_x;
}

uint8_t TUI_Widget_getAbsoluteY(const TUI_Widget* d)
{
    uint8_t abs_y = 0;
    while (d != NULL)
    {
        abs_y += d->y;
        d = d->parent;
    }
    return abs_y;
}
