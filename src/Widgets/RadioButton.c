#include "TUI/Widgets/RadioButton.h"

#include <assert.h>
#include <stddef.h>


static void RadioButton_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    TUI_Widgets_RadioButton* rb = (TUI_Widgets_RadioButton*)self;
    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);
    (void)y;

    uint8_t widgetEndX = abs_x + self->width - 1;
    uint8_t start = (abs_x > clipStartX) ? abs_x : clipStartX;
    uint8_t end = (widgetEndX < clipEndX) ? widgetEndX : clipEndX;

    if (start <= end)
    {
        uint8_t attr = rb->is_focused ? rb->focused_attr : rb->normal_attr;
        uint16_t color = (uint16_t)attr << 8;
        bool is_checked = (rb->shared_value != NULL && *(rb->shared_value) == rb->my_value);

        for (int x = start; x <= end; x++)
        {
            uint8_t localX = x - abs_x;
            uint16_t ch = ' ';

            if (localX == 0) ch = '(';
            else if (localX == 1) ch = (is_checked ? '*' : ' ');
            else if (localX == 2) ch = ')';
            else if (localX == 3) ch = ' ';
            else if (rb->text)
            {
                int textIdx = localX - 4;
                int i = 0;
                while (i <= textIdx && rb->text[i] != '\0') {
                    if (i == textIdx) ch = rb->text[i];
                    i++;
                }
            }

            lineBuffer[x] = ch | color;
        }
    }
}

static bool RadioButton_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);

    TUI_Widgets_RadioButton* rb = (TUI_Widgets_RadioButton*)self;

    if (event->type == TUI_EVENT_FOCUS_GAIN || event->type == TUI_EVENT_FOCUS_LOST)
    {
        rb->is_focused = (event->type == TUI_EVENT_FOCUS_GAIN);
        TUI_Widget_invalidate(self);
        return true;
    }

    if (event->type == TUI_EVENT_KEY)
    {
        uint16_t key = event->key.keycode;
        if (key == TUI_KEY_ENTER || key == TUI_KEY_SPACE || (key & 0xFF) == ' ' || (key & 0xFF) == '\r')
        {
            if (rb->shared_value)
            {
                *(rb->shared_value) = rb->my_value;
                TUI_Widget_invalidate(self);
                if (self->parent)
                    TUI_Widget_invalidate(self->parent);
            }
            return true;
        }
    }
    return false;
}

void TUI_Widgets_RadioButton_init(TUI_Widgets_RadioButton* rb, int16_t x, int16_t y, uint16_t w, const char* text, int* shared_value, int my_value, uint8_t normal_attr, uint8_t focused_attr)
{
    assert(rb);

    TUI_Widget_init(&rb->base, x, y, w, 1, true);
    rb->base.render = RadioButton_Render;
    rb->base.handle_event = RadioButton_HandleEvent;

    rb->text = text;
    rb->shared_value = shared_value;
    rb->my_value = my_value;
    rb->normal_attr = normal_attr;
    rb->focused_attr = focused_attr;
    rb->is_focused = false;
}
