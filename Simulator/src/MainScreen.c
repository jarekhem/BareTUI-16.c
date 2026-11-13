#include "MainScreen.h"

#include <assert.h>
#include <stddef.h>

extern bool g_en_uart;
extern bool g_en_dma;

static void set_up(TUI_Screen* s) { (void)s; }
static void tear_down(TUI_Screen* s) { (void)s; }

void MainScreen_Init(MainScreen* screen, bool* en_uart, bool* en_dma)
{
    /* Color Palette */
    uint8_t attr_text = 0x1F; // Bright White on Blue
    uint8_t attr_panel_bg = 0x18; // Dark Grey on Blue
    uint8_t attr_border = 0x1F; // Bright White on Blue
    uint8_t attr_focus = 0x70; // Black on Light Grey
    
    /* AMI BIOS style tab colors */
    uint8_t attr_tab_header = 0x70; // Black on Light Grey
    uint8_t attr_tab_active = 0x1F; // Bright White on Blue

    TUI_Screen_init(&screen->base);
    screen->base.set_up = set_up;
    screen->base.tear_down = tear_down;

    /* Main background panel */
    TUI_Containers_Panel_init(&screen->panel, 0, 0, 80, 25, attr_panel_bg, attr_border);

    /* Tab Container setup - spans across the panel */
    TUI_Containers_TabContainer_init(&screen->tabs, 1, 1, 78, 23, attr_tab_header, attr_tab_active);

    /* Tabs initialization */
    /* Give tabs an offset so they don't overlap with the top navigation bar of TabContainer */
    TUI_Containers_ColumnGroup_init(&screen->tab1, 2, 2, 70, 20);
    TUI_Containers_ColumnGroup_init(&screen->tab2, 2, 2, 70, 20);
    TUI_Containers_ColumnGroup_init(&screen->tab3, 2, 2, 70, 20);

    /* Tab 1: Main Content */
    TUI_Widgets_Checkbox_init(&screen->chk_uart, 0, 0, 30, "Enable V50 UART", en_uart, attr_text, attr_focus);
    TUI_Widgets_Checkbox_init(&screen->chk_dma, 0, 1, 30, "Enable Main DMA", en_dma, attr_text, attr_focus);
    TUI_Containers_ColumnGroup_addItem(&screen->tab1, (TUI_Widget*)&screen->chk_uart);
    TUI_Containers_ColumnGroup_addItem(&screen->tab1, (TUI_Widget*)&screen->chk_dma);

    /* Tab 2: Advanced Content */
    screen->menu_selection = 0; /* Default selection */
    TUI_Widgets_RadioButton_init(&screen->rb_time, 0, 0, 20, "Time / Date", &screen->menu_selection, 0, attr_text, attr_focus);
    TUI_Widgets_RadioButton_init(&screen->rb_disks, 0, 1, 20, "Disks", &screen->menu_selection, 1, attr_text, attr_focus);
    TUI_Containers_ColumnGroup_addItem(&screen->tab2, (TUI_Widget*)&screen->rb_time);
    TUI_Containers_ColumnGroup_addItem(&screen->tab2, (TUI_Widget*)&screen->rb_disks);

    /* Tab 3: Security Content */
    TUI_Widgets_Label_init(&screen->lbl_info, 0, 0, 40, "No advanced options available.", attr_text);
    TUI_Containers_ColumnGroup_addItem(&screen->tab3, (TUI_Widget*)&screen->lbl_info);

    /* Add tabs to TabContainer */
    TUI_Containers_TabContainer_addTab(&screen->tabs, "Main", (TUI_Widget*)&screen->tab1);
    TUI_Containers_TabContainer_addTab(&screen->tabs, "Advanced", (TUI_Widget*)&screen->tab2);
    TUI_Containers_TabContainer_addTab(&screen->tabs, "Security", (TUI_Widget*)&screen->tab3);

    /* Build the UI tree */
    TUI_Containers_Container_add((TUI_Containers_Container*)&screen->panel, (TUI_Widget*)&screen->tabs);
    TUI_Containers_Container_add((TUI_Containers_Container*)screen, (TUI_Widget*)&screen->panel);
}

TUI_Screen* MainScreen_Create(void* heap)
{
    assert(heap != NULL);
    MainScreen* screen = (MainScreen*)heap;
    MainScreen_Init(screen, &g_en_uart, &g_en_dma);
    return &screen->base;
}
