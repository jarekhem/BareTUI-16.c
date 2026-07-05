#include "TUI/Containers/Container.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

static void SwitchFocus(TUI_Containers_Container* c, TUI_Widget* new_focus)
{
    assert(c);

    if (c->focused_child && c->focused_child->handle_event)
    {
        TUI_Event lost_event;
        lost_event.type = TUI_EVENT_FOCUS_LOST;
        c->focused_child->handle_event(c->focused_child, &lost_event);
    }
    c->focused_child = new_focus;
    if (c->focused_child && c->focused_child->handle_event)
    {
        TUI_Event gain_event;
        gain_event.type = TUI_EVENT_FOCUS_GAIN;
        c->focused_child->handle_event(c->focused_child, &gain_event);
    }
}

void TUI_Containers_Container_render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    assert(self);

    TUI_Containers_Container* c = (TUI_Containers_Container*)self;

    TUI_Widget* current = c->first_child;
    while (current != NULL)
    {
        uint8_t absY = TUI_Widget_getAbsoluteY(current);
        
        if (current->visible && y >= absY && y < absY + current->height)
        {
            uint8_t absX = TUI_Widget_getAbsoluteX(current);
            uint8_t childEndX = absX + current->width - 1;

            if (absX <= clipEndX && childEndX >= clipStartX)
            {
                uint8_t childClipStart = (absX > clipStartX) ? absX : clipStartX;
                uint8_t childClipEnd = (childEndX < clipEndX) ? childEndX : clipEndX;

                if (current->render)
                    current->render(current, lineBuffer, y, childClipStart, childClipEnd);
            }
        }
        current = current->next_sibling;
    }
}

static void Container_Render_Default(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    assert(self);

    TUI_Containers_Container_render(self, lineBuffer, y, clipStartX, clipEndX);
}

bool TUI_Containers_Container_handleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);

    TUI_Containers_Container* c = (TUI_Containers_Container*)self;

    if (c->focused_child && c->focused_child->handle_event)
    {
        if (c->focused_child->handle_event(c->focused_child, event))
            return true;
    }
    return false;
}

void TUI_Containers_Container_init(TUI_Containers_Container* c, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    assert(c);

    TUI_Widget_init(&c->base, x, y, w, h, false);
    c->base.render = Container_Render_Default;
    c->base.handle_event = TUI_Containers_Container_handleEvent;
    c->first_child = NULL;
    c->focused_child = NULL;
}

void TUI_Containers_Container_add(TUI_Containers_Container* c, TUI_Widget* child)
{
    assert(c);
    assert(child);

    child->parent = (TUI_Widget*)c;
    child->next_sibling = NULL;

    if (c->first_child == NULL)
    {
        c->first_child = child;
    }
    else
    {
        TUI_Widget* current = c->first_child;
        while (current->next_sibling != NULL)
        {
            current = current->next_sibling;
        }
        current->next_sibling = child;
    }

    if (child->focusable && c->focused_child == NULL)
        c->focused_child = child;
}


void TUI_Containers_Container_setFocus(TUI_Containers_Container* c, TUI_Widget* child)
{
    /* Forward the manual focus request to the internal static handler */
    SwitchFocus(c, child);
}
