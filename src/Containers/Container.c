#include "TUI/Containers/Container.h"
#include "TUI/Engine.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

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

void TUI_Containers_Container_init(TUI_Containers_Container* c, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    assert(c);

    TUI_Widget_init(&c->base, x, y, w, h, false);
    c->base.is_container = true;
    c->base.render = TUI_Containers_Container_render;
    c->base.handle_event = NULL;
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

    if (child->focusable)
    {
        TUI_Widget* curr = child;
        TUI_Widget* parent = curr->parent;
        while (parent != NULL)
        {
            parent->focusable = true;
            if (parent->is_container)
            {
                TUI_Containers_Container* pc = (TUI_Containers_Container*)parent;
                if (pc->focused_child == NULL && curr->visible)
                {
                    pc->focused_child = curr;
                }
            }
            curr = parent;
            parent = parent->parent;
        }
    }
}

void TUI_Containers_Container_setFocus(TUI_Containers_Container* c, TUI_Widget* child)
{
    assert(c);
    c->focused_child = child;
    TUI_Engine_setFocus(child != NULL ? child : (TUI_Widget*)c);
}
