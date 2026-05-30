#include "ui/ui.h"
#include "bsp/keyboard.h"
#include "bsp/msp_sys.h"

UIMenu main_menu;
UIChooseBox test_choose_box;

UIWidget* main_menu_items[] = { &test_choose_box.base.base };

const char* test_choose_box_options[] = { "Option 1", "Option 2", "Option 3" };

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

    init_ui_choose_box(&test_choose_box, "Mode", test_choose_box_options, 3, 0);
    init_ui_menu(&main_menu, "Main Menu", main_menu_items, 1);
    ui_main_menu = &main_menu;

    ui_update_delta = sys_tick;
    ui_last_update_tick = sys_tick;
}
