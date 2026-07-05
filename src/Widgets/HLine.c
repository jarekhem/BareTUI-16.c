#include "TUI/Widgets/HLine.h"
#include <assert.h>

static void TUI_HLine_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    TUI_Widgets_HLine* hline = (TUI_Widgets_HLine*)self;
    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);
    (void)y;

    uint16_t color = (uint16_t)hline->attr << 8;
    
    uint8_t widgetEndX = abs_x + self->width - 1;
    uint8_t start = (abs_x > clipStartX) ? abs_x : clipStartX;
    uint8_t end = (widgetEndX < clipEndX) ? widgetEndX : clipEndX;

    if (start <= end)
    {
        for (int x = start; x <= end; x++)
        {
            uint16_t ch = hline->middle;
            if (x == abs_x) ch = hline->left;
            else if (x == widgetEndX) ch = hline->right;
            lineBuffer[x] = ch | color;
        }
    }
}

void TUI_Widgets_HLine_init(TUI_Widgets_HLine* hline, uint8_t x, uint8_t y, uint8_t w, unsigned char left, unsigned char middle, unsigned char right, uint8_t attr)
{
    assert(hline);
    TUI_Widget_init(&hline->base, x, y, w, 1, false);
    hline->base.render = TUI_HLine_Render;


    hline->left = left;
    hline->middle = middle;
    hline->right = right;
    hline->attr = attr;
}
