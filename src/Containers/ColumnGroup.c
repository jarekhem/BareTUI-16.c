#include "TUI/Containers/ColumnGroup.h"
#include "TUI/Containers/Container.h"
#include "TUI/Engine.h"

#include <assert.h>
#include <stddef.h>

static bool ColumnGroup_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);

    TUI_Containers_ColumnGroup* col = (TUI_Containers_ColumnGroup*)self;
    TUI_Containers_Container* container = &col->base;

    if (event->type == TUI_EVENT_FOCUS_GAIN || event->type == TUI_EVENT_FOCUS_LOST)
    {
        col->is_focused = (event->type == TUI_EVENT_FOCUS_GAIN);
        TUI_Widget_invalidate(self);
        return true;
    }

    if (container->first_child == NULL)
        return false;

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
                TUI_Engine_setFocus(prev);

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
                TUI_Engine_setFocus(next);

            return true;
        }
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

    TUI_Containers_Container_add(&col->base, item);
    return true;
}
