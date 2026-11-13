#include "TUI/Containers/TabContainer.h"
#include "TUI/Engine.h"

#include <assert.h>
#include <string.h>

#define TITLE_MAX_LEN   16

static void TabContainer_Render(TUI_Widget* self, uint16_t* lineBuffer, uint8_t y, uint8_t clipStartX, uint8_t clipEndX)
{
    assert(self);
    TUI_Containers_TabContainer* tab = (TUI_Containers_TabContainer*)self;

    uint8_t abs_y = TUI_Widget_getAbsoluteY(self);
    uint8_t abs_x = TUI_Widget_getAbsoluteX(self);
    uint8_t local_y = y - abs_y;

    if (local_y == 0)
    {
        uint16_t bg_color = (uint16_t)tab->header_attr << 8;
        uint8_t end_x = abs_x + self->width - 1;

        // Draw background for header row
        uint8_t start_fill = (abs_x > clipStartX) ? abs_x : clipStartX;
        uint8_t end_fill = (end_x < clipEndX) ? end_x : clipEndX;
        for (int x = start_fill; x <= end_fill; x++)
        {
            lineBuffer[x] = ' ' | bg_color;
        }

        int current_x = abs_x + 1; // start drawing tabs with a little left padding
        for (int i = 0; i < tab->tab_count; i++)
        {
            const char* title = tab->tab_titles[i];
            int len = (int)strnlen(title, TITLE_MAX_LEN);
            uint16_t attr = (i == tab->active_tab) ? ((uint16_t)tab->active_attr << 8) : bg_color;

            // Draw padding before title
            if (current_x >= clipStartX && current_x <= clipEndX)
            {
                lineBuffer[current_x] = ' ' | attr;
            }
            current_x++;

            // Draw title characters
            for (int c = 0; c < len; c++)
            {
                if (current_x >= clipStartX && current_x <= clipEndX)
                {
                    lineBuffer[current_x] = (uint8_t)title[c] | attr;
                }
                current_x++;
            }

            // Draw padding after title
            if (current_x >= clipStartX && current_x <= clipEndX)
            {
                lineBuffer[current_x] = ' ' | attr;
            }
            current_x++;

            // Extra spacing between tabs
            if (i < tab->tab_count - 1)
            {
                current_x++; // empty space
            }
        }
    }

    // Call parent render for children
    TUI_Containers_Container_render(self, lineBuffer, y, clipStartX, clipEndX);
}

static bool TabContainer_HandleEvent(TUI_Widget* self, const TUI_Event* event)
{
    assert(self);
    assert(event);
    TUI_Containers_TabContainer* tab = (TUI_Containers_TabContainer*)self;

    /* Synchronize active tab if focus was restored directly to a child inside a specific tab */
    if (event->type == TUI_EVENT_FOCUS_GAIN)
    {
        for (int i = 0; i < tab->tab_count; i++)
        {
            if (tab->tab_contents[i] == tab->base.focused_child)
            {
                if (tab->active_tab != i)
                {
                    if (tab->active_tab >= 0 && tab->active_tab < tab->tab_count)
                    {
                        tab->tab_contents[tab->active_tab]->visible = false;
                    }
                    tab->active_tab = i;
                    tab->tab_contents[i]->visible = true;
                    TUI_Widget_invalidate(self);
                }
                break;
            }
        }
        return true;
    }

    /* Events arrive here via bottom-up bubbling only if unhandled by focused child inside the tab */
    if (event->type == TUI_EVENT_KEY)
    {
        if (event->key.keycode == TUI_KEY_LEFT)
        {
            if (tab->active_tab > 0)
            {
                TUI_Containers_TabContainer_setActiveTab(tab, tab->active_tab - 1);
                return true;
            }
        }
        else if (event->key.keycode == TUI_KEY_RIGHT)
        {
            if (tab->active_tab < tab->tab_count - 1)
            {
                TUI_Containers_TabContainer_setActiveTab(tab, tab->active_tab + 1);
                return true;
            }
        }
    }

    return false;
}

void TUI_Containers_TabContainer_init(TUI_Containers_TabContainer* tab, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t header_attr, uint8_t active_attr)
{
    assert(tab);
    TUI_Containers_Container_init(&tab->base, x, y, w, h);

    // TabContainer is inherently focusable as it manages child tabs
    tab->base.base.focusable = true;

    tab->base.base.render = TabContainer_Render;
    tab->base.base.handle_event = TabContainer_HandleEvent;

    tab->tab_count = 0;
    tab->active_tab = -1;
    tab->header_attr = header_attr;
    tab->active_attr = active_attr;
}

void TUI_Containers_TabContainer_addTab(TUI_Containers_TabContainer* tab, const char* title, TUI_Widget* content)
{
    assert(tab);
    assert(title);
    assert(content);

    if (tab->tab_count < MAX_TABS)
    {
        tab->tab_titles[tab->tab_count] = title;
        tab->tab_contents[tab->tab_count] = content;

        content->visible = (tab->tab_count == 0);

        TUI_Containers_Container_add(&tab->base, content);

        if (tab->tab_count == 0)
        {
            tab->active_tab = 0;
            if (content->focusable)
            {
                tab->base.focused_child = content;
            }
        }

        tab->tab_count++;
    }
}

void TUI_Containers_TabContainer_setActiveTab(TUI_Containers_TabContainer* tab, int index)
{
    assert(tab);
    if (index >= 0 && index < tab->tab_count && index != tab->active_tab)
    {
        if (tab->active_tab >= 0)
        {
            tab->tab_contents[tab->active_tab]->visible = false;
        }
        tab->active_tab = index;
        TUI_Widget* active_content = tab->tab_contents[tab->active_tab];
        active_content->visible = true;
        tab->base.focused_child = active_content;

        TUI_Engine_setFocus(active_content->focusable ? active_content : (TUI_Widget*)tab);

        TUI_Widget_invalidate((TUI_Widget*)tab);
    }
}
