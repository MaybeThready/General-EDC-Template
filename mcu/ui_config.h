#include "ui/ui.h"
#include "bsp/keyboard.h"
#include "bsp/msp_sys.h"

UIPopupButton test_button_1;
UILabel test_label_1;
UIInputBoxDouble test_input_box_1;

const char* test_input_box_suffixes[] = { "mV", "V" };

UIMenu main_menu;
UIWidget* main_menu_items[] = { &test_button_1.base, &test_label_1.base, &test_input_box_1.base.base };

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
    ui_key_0 = &keyboard_keys[3][1];
    ui_key_1 = &keyboard_keys[0][0];
    ui_key_2 = &keyboard_keys[0][1];
    ui_key_3 = &keyboard_keys[0][2];
    ui_key_4 = &keyboard_keys[1][0];
    ui_key_5 = &keyboard_keys[1][1];
    ui_key_6 = &keyboard_keys[1][2];
    ui_key_7 = &keyboard_keys[2][0];
    ui_key_8 = &keyboard_keys[2][1];
    ui_key_9 = &keyboard_keys[2][2];
    ui_key_point = &keyboard_keys[3][0];

    init_ui_label(&test_label_1, "Label 1");
    init_ui_popup_button(&test_button_1, "Button 1");
    init_ui_input_box_double(&test_input_box_1, "Vol", 1234.567, test_input_box_suffixes, 2, 3, true, NULL);
    init_ui_menu(&main_menu, "Main Menu", main_menu_items, 3);
    ui_main_menu = &main_menu;

    ui_update_delta = sys_tick;
    ui_last_update_tick = sys_tick;
}
