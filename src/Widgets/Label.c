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
            for (int x = start; x <= end; x++)
            {
                uint8_t localX = x - abs_x;
                uint16_t ch = ' ';
                int i = 0;
                while (i <= localX && lbl->text[i] != '\0') {
                    if (i == localX) ch = (unsigned char)lbl->text[i];
                    i++;
                }
                lineBuffer[x] = ch | attr;
            }
        }
    }
}

void TUI_Widgets_Label_setAttr(TUI_Widgets_Label* lbl, uint8_t attr)
{
    assert(lbl);
    lbl->attr = attr;
}

void TUI_Widgets_Label_init(TUI_Widgets_Label* lbl, uint8_t x, uint8_t y, uint8_t w, const char* text, uint8_t attr)
{
    assert(lbl);

    TUI_Widget_init(&lbl->base, x, y, w, 1, false);
    lbl->base.render = Label_Render;

    lbl->text = text;
    lbl->attr = attr;
}
