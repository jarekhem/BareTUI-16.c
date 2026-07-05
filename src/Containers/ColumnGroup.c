#include "TUI/Containers/ColumnGroup.h"
#include <assert.h>
#include <stddef.h>

#include "TUI/Containers/Container.h"

static bool ColumnGroup_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);

    TUI_Containers_ColumnGroup* col = (TUI_Containers_ColumnGroup*)self;
    TUI_Containers_Container* container = &col->base;

    if (container->first_child == NULL)
        return false;

    /* Forward Focus changes directly to the currently active child */
    if (event->type == TUI_EVENT_FOCUS_GAIN || event->type == TUI_EVENT_FOCUS_LOST)
    {
        col->is_focused = (event->type == TUI_EVENT_FOCUS_GAIN);
        if (container->focused_child && container->focused_child->handle_event)
        {
            TUI_Event focus_event;
            focus_event.type = event->type;
            container->focused_child->handle_event(container->focused_child, &focus_event);
        }
        TUI_Widget_invalidate(self);
        return true;
    }

    if (event->type == TUI_EVENT_KEY)
    {
        uint16_t key = event->key.keycode;
        bool is_up = (key == TUI_KEY_UP);
        bool is_down = (key == TUI_KEY_DOWN);
        bool is_tab = (key == TUI_KEY_TAB);

        if (is_up)
        {
            TUI_Widget* prev = NULL;
            TUI_Widget* current = container->first_child;
            TUI_Widget* last_focusable = NULL;

            while (current != NULL)
            {
                if (current->focusable && current->visible)
                {
                    if (current == container->focused_child)
                        prev = last_focusable;
                    last_focusable = current;
                }
                current = current->next_sibling;
            }

            if (prev == NULL) prev = last_focusable;

            if (prev)
                TUI_Containers_Container_setFocus(container, prev);

            return true;
        }

        if (is_down || is_tab)
        {
            TUI_Widget* next = NULL;
            TUI_Widget* current = container->first_child;
            bool found_focused = false;

            while (current != NULL)
            {
                if (current->focusable && current->visible)
                {
                    if (found_focused)
                    {
                        next = current;
                        break;
                    }
                    if (current == container->focused_child)
                        found_focused = true;
                }
                current = current->next_sibling;
            }

            if (next == NULL)
            {
                current = container->first_child;
                while (current != NULL)
                {
                    if (current->focusable && current->visible)
                    {
                        next = current;
                        break;
                    }
                    current = current->next_sibling;
                }
            }

            if (next)
                TUI_Containers_Container_setFocus(container, next);

            return true;
        }
    }

    /* Forward other events directly to the currently active child */
    if (container->focused_child && container->focused_child->handle_event)
    {
        return container->focused_child->handle_event(container->focused_child, event);
    }

    return false;
}

void TUI_Containers_ColumnGroup_init(TUI_Containers_ColumnGroup* col, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    assert(col);

    TUI_Containers_Container_init(&col->base, x, y, w, h);
    col->base.base.focusable = true;
    col->base.base.handle_event = ColumnGroup_HandleEvent;
    col->is_focused = false;
}

bool TUI_Containers_ColumnGroup_addItem(TUI_Containers_ColumnGroup* col, TUI_Widget* item)
{
    assert(col);
    assert(item);

    item->parent = (TUI_Widget*)col;
    item->next_sibling = NULL;

    if (col->base.first_child == NULL)
    {
        col->base.first_child = item;
        if (item->focusable)
            col->base.focused_child = item;
    }
    else
    {
        TUI_Widget* current = col->base.first_child;
        while (current->next_sibling != NULL)
            current = current->next_sibling;

        current->next_sibling = item;

        if (item->focusable && col->base.focused_child == NULL)
            col->base.focused_child = item;
    }

    return true;
}
