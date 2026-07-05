#include "TUI/Containers/ViewFlipper.h"
#include <assert.h>
#include <stddef.h>

void TUI_Containers_ViewFlipper_init(TUI_Containers_ViewFlipper* flipper, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    assert(flipper);
    TUI_Containers_Container_init(&flipper->base, x, y, w, h);
    flipper->active_index = -1;
}

void TUI_Containers_ViewFlipper_setActive(TUI_Containers_ViewFlipper* flipper, int index)
{
    assert(flipper);
    
    if (flipper->active_index == index) return;
    
    int i = 0;
    TUI_Widget* current = flipper->base.first_child;
    
    while (current != NULL)
    {
        current->visible = (i == index);
        if (current->visible && current->focusable)
        {
            flipper->base.focused_child = current;
        }
        current = current->next_sibling;
        i++;
    }
    
    flipper->active_index = index;
    TUI_Widget_invalidate((TUI_Widget*)flipper);
}
