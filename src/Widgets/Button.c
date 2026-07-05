#include "TUI/Widgets/Button.h"

#include <assert.h>


void TUI_Widgets_Button_setActive(TUI_Widgets_Button* button, bool active)
{
    assert(button);
    button->active = active;
    if (button->focused)
        TUI_Widgets_Label_setAttr(&button->base, button->focus_attr);
    else if (button->active)
        TUI_Widgets_Label_setAttr(&button->base, button->active_attr);
    else
        TUI_Widgets_Label_setAttr(&button->base, button->attr);

    TUI_Widget_invalidate((TUI_Widget*)button);
}

static void TUI_Button_SetFocus(TUI_Widgets_Button* button, bool focused)
{
    assert(button);
    if (focused)
        TUI_Widgets_Label_setAttr(&button->base, button->focus_attr);
    else if (button->active)
        TUI_Widgets_Label_setAttr(&button->base, button->active_attr);
    else
        TUI_Widgets_Label_setAttr(&button->base, button->attr);

    button->focused = focused;
    TUI_Widget_invalidate((TUI_Widget*)button);
}

static bool TUI_Button_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);
    TUI_Widgets_Button* button = (TUI_Widgets_Button*)self;

    switch (event->type)
    {
    case TUI_EVENT_FOCUS_GAIN:
        TUI_Button_SetFocus(button, true);
        return true;
    case TUI_EVENT_FOCUS_LOST:
        TUI_Button_SetFocus(button, false);
        return true;
    case TUI_EVENT_KEY:
        if (button->focused && (event->key.keycode == TUI_KEY_ENTER || event->key.keycode == TUI_KEY_SPACE || (event->key.keycode & 0xFF) == ' ' || (event->key.keycode & 0xFF) == '\r'))
        {
            if (button->action) button->action();
            return true;
        }
        return false;
    default:
        return false;
    }
}

void TUI_Widgets_Button_init(TUI_Widgets_Button* button,
                     uint8_t x,
                     uint8_t y,
                     uint8_t w,
                     const char* text,

                     uint8_t attr,
                     uint8_t focus_attr,
                     uint8_t active_attr,
                     void (*action)())
{
    assert(button);
    TUI_Widgets_Label_init(&button->base, x, y, w, text, attr);
    button->base.base.focusable = true;
    button->attr = attr;
    button->focus_attr = focus_attr;
    button->active_attr = active_attr;
    button->action = action;
    button->focused = false;
    button->active = false;

    button->base.base.handle_event = TUI_Button_HandleEvent;
}
