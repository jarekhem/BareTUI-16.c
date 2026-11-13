#pragma once

#include "TUI/Screen.h"
#include "TUI/Widgets/Label.h"
#include "TUI/Containers/Panel.h"
#include "TUI/Widgets/Checkbox.h"
#include "TUI/Containers/ColumnGroup.h"
#include "TUI/Widgets/RadioButton.h"
#include "TUI/Containers/TabContainer.h"

typedef struct
{
    TUI_Screen base;
    TUI_Containers_Panel panel;

    TUI_Containers_TabContainer tabs;

    TUI_Containers_ColumnGroup tab1;
    TUI_Containers_ColumnGroup tab2;
    TUI_Containers_ColumnGroup tab3;

    int menu_selection;
    TUI_Widgets_RadioButton rb_time;
    TUI_Widgets_RadioButton rb_disks;

    TUI_Widgets_Checkbox chk_uart;
    TUI_Widgets_Checkbox chk_dma;

    TUI_Widgets_Label lbl_info;
} MainScreen;

void MainScreen_Init(MainScreen* screen, bool* en_uart, bool* en_dma);
TUI_Screen* MainScreen_Create(void* heap);
