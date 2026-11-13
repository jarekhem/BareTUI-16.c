#include "TUI/Engine.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

TUI_Engine tuiEngine;

void TUI_Engine_init(TUI_VramFlushFunc flushFunc, void* frontendHeap)
{
    assert(flushFunc != NULL);
    assert(frontendHeap != NULL);

    tuiEngine.flushFunc = flushFunc;
    tuiEngine.frontendHeap = frontendHeap;
    tuiEngine.activeScreen = NULL;
    tuiEngine.focusedWidget = NULL;
    tuiEngine.navDepth = 0;
    tuiEngine.pendingTransition = false;

    for (int i = 0; i < 25; i++)
    {
        tuiEngine.dirtyLines[i].startX = 0xFF;
        tuiEngine.dirtyLines[i].endX = 0xFF;
    }
}

void TUI_Engine_changeScreen(TUI_ScreenFactory factory)
{
    assert(factory != NULL);

    if (tuiEngine.navDepth == 0)
    {
        tuiEngine.navDepth = 1;
    }

    tuiEngine.navStack[tuiEngine.navDepth - 1].factory = factory;
    tuiEngine.navStack[tuiEngine.navDepth - 1].focused_widget = NULL;
    tuiEngine.pendingTransition = true;
}

void TUI_Engine_pushScreen(TUI_ScreenFactory factory)
{
    assert(factory != NULL);
    assert(tuiEngine.navDepth < TUI_MAX_NAV_DEPTH);

    if (tuiEngine.navDepth > 0)
    {
        tuiEngine.navStack[tuiEngine.navDepth - 1].focused_widget = tuiEngine.focusedWidget;
    }

    tuiEngine.navStack[tuiEngine.navDepth].factory = factory;
    tuiEngine.navStack[tuiEngine.navDepth].focused_widget = NULL;
    tuiEngine.navDepth++;
    tuiEngine.pendingTransition = true;
}

bool TUI_Engine_popScreen(void)
{
    if (tuiEngine.navDepth <= 1)
    {
        return false;
    }

    tuiEngine.navDepth--;
    tuiEngine.pendingTransition = true;
    return true;
}

TUI_Widget* TUI_Engine_getFocus(void)
{
    return tuiEngine.focusedWidget;
}

void TUI_Engine_setFocus(TUI_Widget* widget)
{
    if (widget == NULL)
    {
        return;
    }

    /* 1. Update focused_child references along the parent chain up to the root */
    TUI_Widget* child = widget;
    TUI_Widget* parent = child->parent;
    while (parent != NULL)
    {
        if (parent->is_container)
        {
            ((TUI_Containers_Container*)parent)->focused_child = child;
        }
        child = parent;
        parent = parent->parent;
    }

    /* 2. If a container was passed, drill down to the deepest focusable leaf */
    TUI_Widget* target = widget;
    while (target != NULL && target->is_container)
    {
        TUI_Containers_Container* c = (TUI_Containers_Container*)target;
        TUI_Widget* next = NULL;

        if (c->focused_child != NULL && c->focused_child->visible && c->focused_child->focusable)
        {
            next = c->focused_child;
        }
        else
        {
            TUI_Widget* curr = c->first_child;
            while (curr != NULL)
            {
                if (curr->visible && curr->focusable)
                {
                    next = curr;
                    break;
                }
                curr = curr->next_sibling;
            }
        }

        if (next != NULL)
        {
            c->focused_child = next;
            target = next;
        }
        else
        {
            break;
        }
    }

    if (tuiEngine.focusedWidget == target)
    {
        return;
    }

    /* 3. Notify old focus chain of FOCUS_LOST */
    if (tuiEngine.focusedWidget != NULL)
    {
        TUI_Event lost_event;
        lost_event.type = TUI_EVENT_FOCUS_LOST;

        TUI_Widget* curr = tuiEngine.focusedWidget;
        while (curr != NULL)
        {
            if (curr->handle_event)
            {
                curr->handle_event(curr, &lost_event);
            }
            curr = curr->parent;
        }
    }

    tuiEngine.focusedWidget = target;

    /* 4. Notify new focus chain of FOCUS_GAIN */
    if (target != NULL)
    {
        TUI_Event gain_event;
        gain_event.type = TUI_EVENT_FOCUS_GAIN;

        TUI_Widget* curr = target;
        while (curr != NULL)
        {
            if (curr->handle_event)
            {
                curr->handle_event(curr, &gain_event);
            }
            curr = curr->parent;
        }
    }
}

static void TUI_Engine_processTransition(void)
{
    tuiEngine.pendingTransition = false;

    if (tuiEngine.activeScreen && tuiEngine.activeScreen->tear_down)
    {
        tuiEngine.activeScreen->tear_down(tuiEngine.activeScreen);
    }

    tuiEngine.activeScreen = NULL;
    tuiEngine.focusedWidget = NULL;

    if (tuiEngine.navDepth == 0)
    {
        return;
    }

    TUI_NavEntry* entry = &tuiEngine.navStack[tuiEngine.navDepth - 1];
    assert(entry->factory != NULL);

    TUI_Screen* new_screen = entry->factory(tuiEngine.frontendHeap);
    assert(new_screen != NULL);

    tuiEngine.activeScreen = new_screen;

    if (new_screen->set_up)
    {
        new_screen->set_up(new_screen);
    }

    if (entry->focused_widget != NULL)
    {
        TUI_Engine_setFocus(entry->focused_widget);
    }
    else
    {
        TUI_Engine_setFocus((TUI_Widget*)new_screen);
    }

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
    if (tuiEngine.pendingTransition)
    {
        TUI_Engine_processTransition();
    }

    if (event && event->type != TUI_EVENT_NONE && tuiEngine.activeScreen)
    {
        TUI_Widget* target = tuiEngine.focusedWidget;
        if (target == NULL)
        {
            target = (TUI_Widget*)tuiEngine.activeScreen;
        }

        /* Bottom-up event bubbling along the parent chain (LVGL style) */
        while (target != NULL)
        {
            if (target->handle_event && target->handle_event(target, event))
            {
                break;
            }
            target = target->parent;
        }

        if (tuiEngine.pendingTransition)
        {
            TUI_Engine_processTransition();
        }
    }

    TUI_Engine_renderScreen();
}
