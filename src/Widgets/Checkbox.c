#include "TUI/Widgets/Checkbox.h"

#include <assert.h>

static void Checkbox_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    TUI_Widgets_Checkbox* cb = (TUI_Widgets_Checkbox*)self;
    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);
    (void)y;

    uint8_t widgetEndX = abs_x + self->width - 1;
    uint8_t start = (abs_x > clipStartX) ? abs_x : clipStartX;
    uint8_t end = (widgetEndX < clipEndX) ? widgetEndX : clipEndX;

    if (start <= end)
    {
        uint8_t attr = cb->is_focused ? cb->focused_attr : cb->normal_attr;
        uint16_t color = (uint16_t)attr << 8;

        const char* p = cb->text;
        if (p && start > abs_x + 4)
        {
            uint8_t skip = start - (abs_x + 4);
            while (skip > 0 && *p != '\0')
            {
                p++;
                skip--;
            }
        }

        for (uint8_t x = start; x <= end; x++)
        {
            uint8_t localX = x - abs_x;
            uint8_t ch = ' ';

            if (localX == 0) ch = '[';
            else if (localX == 1) ch = (*cb->value_ref ? 'X' : ' ');
            else if (localX == 2) ch = ']';
            else if (localX >= 4 && p && *p != '\0')
            {
                ch = (uint8_t)*p++;
            }

            lineBuffer[x] = (uint16_t)ch | color;
        }
    }
}

static bool Checkbox_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);

    TUI_Widgets_Checkbox* cb = (TUI_Widgets_Checkbox*)self;

    if (event->type == TUI_EVENT_FOCUS_GAIN || event->type == TUI_EVENT_FOCUS_LOST)
    {
        cb->is_focused = (event->type == TUI_EVENT_FOCUS_GAIN);
        TUI_Widget_invalidate(self);
        return true;
    }

    if (event->type == TUI_EVENT_KEY)
    {
        uint16_t key = event->key.keycode;
        if (key == TUI_KEY_ENTER || key == TUI_KEY_SPACE || (key & 0xFF) == ' ' || (key & 0xFF) == '\r')
        {
            if (cb->value_ref)
            {
                *(cb->value_ref) = !*(cb->value_ref);
                TUI_Widget_invalidate(self);
            }
            return true;
        }
    }
    return false;
}

void TUI_Widgets_Checkbox_init(TUI_Widgets_Checkbox* cb, int16_t x, int16_t y, uint16_t w, const char* text, bool* value_ref, uint8_t normal_attr, uint8_t focused_attr)
{
    assert(cb);

    TUI_Widget_init(&cb->base, x, y, w, 1, true); /* Checkboxes are interactive */
    cb->base.render = Checkbox_Render;
    cb->base.handle_event = Checkbox_HandleEvent;

    cb->text = text;
    cb->value_ref = value_ref;
    cb->normal_attr = normal_attr;
    cb->focused_attr = focused_attr;
    cb->is_focused = false;
}
