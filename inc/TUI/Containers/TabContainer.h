#pragma once

#include "TUI/Containers/Container.h"

#define MAX_TABS 10

typedef struct TUI_Containers_TabContainer
{
    TUI_Containers_Container base;
    
    const char* tab_titles[MAX_TABS];
    TUI_Widget* tab_contents[MAX_TABS];
    
    int tab_count;
    int active_tab;

    /* Colors for the header row */
    uint8_t header_attr;
    uint8_t active_attr;
} TUI_Containers_TabContainer;

void TUI_Containers_TabContainer_init(TUI_Containers_TabContainer* tab, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t header_attr, uint8_t active_attr);
void TUI_Containers_TabContainer_addTab(TUI_Containers_TabContainer* tab, const char* title, TUI_Widget* content);
void TUI_Containers_TabContainer_setActiveTab(TUI_Containers_TabContainer* tab, int index);
