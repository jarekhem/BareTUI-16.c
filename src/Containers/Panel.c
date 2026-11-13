#include "TUI/Containers/Panel.h"

#include <assert.h>

#include "TUI/Containers/Container.h"
#include "TUI/Widget.h"

static void Panel_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    assert(self);

    TUI_Containers_Panel* panel = (TUI_Containers_Panel*)self;

    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);
    uint8_t abs_y = TUI_Widget_getAbsoluteY(self);
    uint8_t local_y = y - abs_y;
    
    uint16_t bg_color = (uint16_t)panel->bg_attr << 8;
    uint16_t border_color = (uint16_t)panel->border_attr << 8;
    
    uint8_t panelEndX = abs_x + self->width - 1;
    
    uint8_t start = (abs_x > clipStartX) ? abs_x : clipStartX;
    uint8_t end = (panelEndX < clipEndX) ? panelEndX : clipEndX;
    
    if (start <= end)
    {
        for (int x = start; x <= end; x++)
        {
            uint16_t ch = ' ' | bg_color;
            if (local_y == 0)
            {
                if (x == abs_x) ch = 0xDA | border_color;
                else if (x == panelEndX) ch = 0xBF | border_color;
                else ch = 0xC4 | border_color;
            }
            else if (local_y == self->height - 1)
            {
                if (x == abs_x) ch = 0xC0 | border_color;
                else if (x == panelEndX) ch = 0xD9 | border_color;
                else ch = 0xC4 | border_color;
            }
            else
            {
                if (x == abs_x || x == panelEndX) ch = 0xB3 | border_color;
            }
            lineBuffer[x] = ch;
        }
    }

    TUI_Containers_Container_render(self, lineBuffer, y, clipStartX, clipEndX);
}

void TUI_Containers_Panel_init(TUI_Containers_Panel* panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t bg_attr, uint8_t border_attr)
{
    assert(panel);

    TUI_Containers_Container_init(&panel->base, x, y, width, height);

    panel->base.base.render = Panel_Render;

    panel->bg_attr = bg_attr;
    panel->border_attr = border_attr;
}