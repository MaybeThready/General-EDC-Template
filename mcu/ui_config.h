#include "ui/ui.h"
#include "bsp/keyboard.h"
#include "bsp/msp_sys.h"

UIPopupButton test_button_1;
UILabel test_label_1;
UIPopupButton test_button_2;

UIMenu main_menu;
UIWidget* main_menu_items[] = { &test_button_1.base, &test_label_1.base, &test_button_2.base };

void init_ui()
{
    ui_key_left = &keyboard_keys[1][0];
    ui_key_up = &keyboard_keys[0][1];
    ui_key_right = &keyboard_keys[1][2];
    ui_key_down = &keyboard_keys[2][1];
    ui_key_enter = &keyboard_keys[3][3];
    ui_key_back = &keyboard_keys[0][3];
    ui_key_incr = ui_key_right;
    ui_key_decr = ui_key_left;
    ui_key_scr_up = &keyboard_keys[1][3];
    ui_key_scr_down = &keyboard_keys[2][3];

    init_ui_label(&test_label_1, "Label 1");
    init_ui_popup_button(&test_button_1, "Button 1");
    init_ui_popup_button(&test_button_2, "Button 2");
    init_ui_menu(&main_menu, "Main Menu", main_menu_items, 3);
    ui_main_menu = &main_menu;

    ui_update_delta = sys_tick;
    ui_last_update_tick = sys_tick;
}
