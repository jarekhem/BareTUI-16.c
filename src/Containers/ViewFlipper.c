#include "TUI/Containers/ViewFlipper.h"
#include "TUI/Engine.h"

#include <assert.h>
#include <stddef.h>

static bool ViewFlipper_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);

    TUI_Containers_ViewFlipper* flipper = (TUI_Containers_ViewFlipper*)self;

    if (event->type == TUI_EVENT_FOCUS_GAIN)
    {
        int i = 0;
        TUI_Widget* current = flipper->base.first_child;
        while (current != NULL)
        {
            if (current == flipper->base.focused_child)
            {
                if (flipper->active_index != i)
                {
                    TUI_Widget* c = flipper->base.first_child;
                    while (c != NULL)
                    {
                        c->visible = (c == current);
                        c = c->next_sibling;
                    }
                    flipper->active_index = i;
                    TUI_Widget_invalidate(self);
                }
                break;
            }
            current = current->next_sibling;
            i++;
        }
        return true;
    }

    return false;
}

void TUI_Containers_ViewFlipper_init(TUI_Containers_ViewFlipper* flipper, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    assert(flipper);
    TUI_Containers_Container_init(&flipper->base, x, y, w, h);
    flipper->base.base.focusable = true;
    flipper->base.base.handle_event = ViewFlipper_HandleEvent;
    flipper->active_index = -1;
}

void TUI_Containers_ViewFlipper_setActive(TUI_Containers_ViewFlipper* flipper, int index)
{
    assert(flipper);
    
    if (flipper->active_index == index) return;
    
    int i = 0;
    TUI_Widget* current = flipper->base.first_child;
    TUI_Widget* active_child = NULL;
    
    while (current != NULL)
    {
        current->visible = (i == index);
        if (current->visible)
        {
            active_child = current;
        }
        current = current->next_sibling;
        i++;
    }
    
    flipper->active_index = index;
    if (active_child)
    {
        flipper->base.focused_child = active_child;
        TUI_Engine_setFocus(active_child->focusable ? active_child : (TUI_Widget*)flipper);
    }
    TUI_Widget_invalidate((TUI_Widget*)flipper);
}
