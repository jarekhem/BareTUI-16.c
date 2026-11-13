#include "TUI/Widgets/Label.h"

#include <assert.h>

static void Label_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    assert(self);
    (void)y;

    TUI_Widgets_Label* lbl = (TUI_Widgets_Label*)self;
    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);

    if (lbl->text)
    {
        uint8_t widgetEndX = abs_x + self->width - 1;
        uint8_t start = (abs_x > clipStartX) ? abs_x : clipStartX;
        uint8_t end = (widgetEndX < clipEndX) ? widgetEndX : clipEndX;

        if (start <= end)
        {
            uint16_t attr = (uint16_t)lbl->attr << 8;
            const char* p = lbl->text;
            uint8_t skip = start - abs_x;
            while (skip > 0 && *p != '\0')
            {
                p++;
                skip--;
            }

            for (uint8_t x = start; x <= end; x++)
            {
                uint8_t ch = ' ';
                if (*p != '\0')
                {
                    ch = (uint8_t)*p++;
                }
                lineBuffer[x] = (uint16_t)ch | attr;
            }
        }
    }
}

void TUI_Widgets_Label_setAttr(TUI_Widgets_Label* lbl, uint8_t attr)
{
    assert(lbl);
    if (lbl->attr != attr)
    {
        lbl->attr = attr;
        TUI_Widget_invalidate(&lbl->base);
    }
}

void TUI_Widgets_Label_init(TUI_Widgets_Label* lbl, uint8_t x, uint8_t y, uint8_t w, const char* text, uint8_t attr)
{
    assert(lbl);

    TUI_Widget_init(&lbl->base, x, y, w, 1, false);
    lbl->base.render = Label_Render;

    lbl->text = text;
    lbl->attr = attr;
}
