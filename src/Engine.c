#include "TUI/Engine.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

TUI_Engine tuiEngine;

void TUI_Engine_init(TUI_VramFlushFunc flushFunc)
{
    assert(flushFunc != NULL);
    tuiEngine.flushFunc = flushFunc;
    tuiEngine.activeScreen = NULL;

    for (int i = 0; i < 25; i++)
    {
        tuiEngine.dirtyLines[i].startX = 0xFF;
        tuiEngine.dirtyLines[i].endX = 0xFF;
    }
}

void TUI_Engine_changeScreen(TUI_Screen* new_screen)
{
    assert(new_screen != NULL);

    if (tuiEngine.activeScreen && tuiEngine.activeScreen->tear_down)
        tuiEngine.activeScreen->tear_down(tuiEngine.activeScreen);

    tuiEngine.activeScreen = new_screen;

    if (new_screen->set_up)
        new_screen->set_up(new_screen);

    TUI_Widget* drawable = (TUI_Widget*)new_screen;
    drawable->dirty = true;

    TUI_Engine_invalidateArea(0, 0, 80, 25);
}

void TUI_Engine_invalidateArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t endY = y + height;
    if (endY > 25) endY = 25;

    if (width == 0) return;
    uint8_t endX = x + width - 1;
    if (endX >= 80) endX = 79;

    for (uint8_t i = y; i < endY; i++)
    {
        if (tuiEngine.dirtyLines[i].startX == 0xFF)
        {
            tuiEngine.dirtyLines[i].startX = x;
            tuiEngine.dirtyLines[i].endX = endX;
        }
        else
        {
            if (x < tuiEngine.dirtyLines[i].startX) tuiEngine.dirtyLines[i].startX = x;
            if (endX > tuiEngine.dirtyLines[i].endX) tuiEngine.dirtyLines[i].endX = endX;
        }
    }
}

static void TUI_Engine_renderScreen(void)
{
    if (!tuiEngine.activeScreen) return;

    TUI_Widget* drawable = (TUI_Widget*)tuiEngine.activeScreen;

    for (uint8_t y = 0; y < 25; y++)
    {
        uint8_t startX = tuiEngine.dirtyLines[y].startX;
        uint8_t endX = tuiEngine.dirtyLines[y].endX;

        if (startX != 0xFF)
        {
            for (uint8_t x = startX; x <= endX; x++)
            {
                tuiEngine.lineBuffer[x] = (uint16_t)' ' | 0x0700;
            }

            if (drawable->render)
            {
                drawable->render(drawable, tuiEngine.lineBuffer, y, startX, endX);
            }

            tuiEngine.flushFunc(tuiEngine.lineBuffer, y, startX, endX);

            tuiEngine.dirtyLines[y].startX = 0xFF;
        }
    }
}

void TUI_Engine_poll(const TUI_Event* event)
{
    if (event && event->type != TUI_EVENT_NONE && tuiEngine.activeScreen)
    {
        TUI_Widget* drawable = (TUI_Widget*)tuiEngine.activeScreen;
        if (drawable->handle_event)
        {
            drawable->handle_event(drawable, event);
        }
    }

    TUI_Engine_renderScreen();
}
