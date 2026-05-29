/**
 *@file ui.h
 * @author Thready
 * @brief UI模块
 * @version 0.1
 * @date 2026-05-27
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "../bsp/oled/oled.h"
#include "../bsp/keyboard.h"
#include "../bsp/msp_sys.h"
#include <stdint.h>

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#define UI_MOVE_SPEED_RATIO (2000.f)
#define UI_V_MARGIN (4)
#define UI_H_MARGIN (2)
#define UI_WINDOW_WIDTH (OLED_WIDTH - 4)
#define UI_WINDOW_HEIGHT (OLED_HEIGHT - ui_font_height - 8)
#define UI_EPSILON (0.5f)

typedef void (*VoidCallbackFunc)(void);

// UI控件基类
typedef struct UIWidget
{
    float x;
    float y;
    float width;
    float height;

    float target_x;
    float target_y;
    float target_width;
    float target_height;

    bool should_move;

    void (*enter)(struct UIWidget* widget);
    void (*render)(struct UIWidget* widget);
    void (*step)(struct UIWidget* widget);
} UIWidget;

// 窗口
typedef struct UIWindow
{
    UIWidget base;
    const char* title;
    bool is_exiting;
    void (*render)(struct UIWindow* window);
    void (*layout)(struct UIWindow* window);
    void (*process_input)(struct UIWindow* window);
} UIWindow;

// 菜单
typedef struct UIMenu
{
    UIWidget base;
    const char* title;
    UIWidget** items;
    uint8_t item_count;
    int16_t selected_index;
    void (*render)(struct UIMenu* widget);
    void (*layout)(struct UIMenu* menu);
    void (*process_input)(struct UIMenu* menu);
} UIMenu;

// 标签
typedef struct UILabel
{
    UIWidget base;
    const char* text;
} UILabel;

typedef struct UIPopupButton
{
    UIWidget base;
    UIWindow window;
} UIPopupButton;

/**
 * @brief 初始化菜单
 *
 * @param menu 菜单对象指针
 * @param title 菜单标题
 * @param items 菜单项数组
 * @param item_count 菜单项数量
 */
void init_ui_menu(UIMenu* menu, const char* title, UIWidget** items, uint8_t item_count);

/**
 *@brief 初始化标签
 * 
 * @param label 标签对象指针
 * @param text 标签文本
 */
void init_ui_label(UILabel* label, const char* text);

/**
 *@brief 初始化弹窗按钮
 * 
 * @param button 弹窗按钮对象指针
 * @param text 按钮文本（也是弹窗标题）
 */
void init_ui_popup_button(UIPopupButton* button, const char* text);

extern UIMenu* ui_main_menu;  // 主菜单，程序启动后显示的默认菜单
extern UIWindow* ui_current_window;  // 当前活动的窗口，如果不为NULL则显示该窗口并优先处理其输入，直到窗口退出后才会继续显示菜单
extern OLEDFontSizeHalf ui_ascii_size;  // 当前ASCII字符字体大小，单位为像素宽度，必须是OLEDFontSizeHalf枚举值之一
extern OLEDFontSizeFull ui_chinese_size;  // 当前中文字符字体大小，单位为像素宽度，必须是OLEDFontSizeFull枚举值之一
extern uint8_t ui_font_height;  // 当前字体高度，单位为像素，根据ui_ascii_size和ui_chinese_size计算得出
extern UIWidget ui_cursor;  // 菜单光标

extern Key* ui_key_left;  // 菜单左移键
extern Key* ui_key_up;  // 菜单上移键
extern Key* ui_key_right;  // 菜单右移键
extern Key* ui_key_down;  // 菜单下移键
extern Key* ui_key_enter;  // 菜单确认键
extern Key* ui_key_back;  // 菜单返回键
extern Key* ui_key_incr;  // 菜单增大选择键
extern Key* ui_key_decr;  // 菜单减小选择键
extern Key* ui_key_scr_up;  // 菜单滚屏向上键
extern Key* ui_key_scr_down;  // 菜单滚屏向下键

extern uint32_t ui_update_delta;  // 上次更新到现在的时间差，单位为系统时钟节拍数
extern uint32_t ui_last_update_tick;  // 上次更新的系统时钟节拍数

/**
 *@brief 更新UI显示，应该在主循环中定期调用以刷新UI界面
 * 
 */
void ui_update();

/**
 *@brief 更改UI字体大小，调用后会立即生效并更新界面布局
 * 
 * @param ascii_size 新的ASCII字符字体大小，单位为像素宽度，必须是OLEDFontSizeHalf枚举值之一
 * @param chinese_size 新的中文字符字体大小，单位为像素宽度，必须是OLEDFontSizeFull枚举值之一
 */
void ui_change_font_size(OLEDFontSizeHalf ascii_size, OLEDFontSizeFull chinese_size);
