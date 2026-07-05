#include "TUI/Screen.h"

#include <assert.h>
#include <stddef.h>

void TUI_Screen_init(TUI_Screen* screen)
{
    assert(screen);
    screen->set_up = NULL;
    screen->tear_down = NULL;
    TUI_Containers_Container_init(&screen->root, 0, 0, 80, 25);
}
