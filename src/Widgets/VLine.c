#include "TUI/Widgets/VLine.h"
#include <assert.h>

static void TUI_VLine_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    TUI_Widgets_VLine* vline = (TUI_Widgets_VLine*)self;
    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);
    uint8_t abs_y = TUI_Widget_getAbsoluteY(self);

    if (abs_x >= clipStartX && abs_x <= clipEndX)
    {
        uint16_t color = (uint16_t)vline->attr << 8;
        uint16_t ch = vline->middle;
        
        if (y == abs_y) ch = vline->top;
        else if (y == abs_y + self->height - 1) ch = vline->bottom;
        
        lineBuffer[abs_x] = ch | color;
    }
}

void TUI_Widgets_VLine_init(TUI_Widgets_VLine* vline, uint8_t x, uint8_t y, uint8_t h, unsigned char top, unsigned char middle, unsigned char bottom, uint8_t attr)
{
    assert(vline);
    TUI_Widget_init(&vline->base, x, y, 1, h, false);
    vline->base.render = TUI_VLine_Render;


    vline->top = top;
    vline->middle = middle;
    vline->bottom = bottom;
    vline->attr = attr;
}
