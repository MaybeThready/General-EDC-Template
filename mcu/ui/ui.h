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
#define UI_NO_DIGIT_INPUT (255)
#define UI_DIGIT_INPUT_POINT (10)

struct UIMenu;
struct UICheckbox;
struct UIInputBoxDouble;
struct UIChooseBox;

typedef void (*MenuCallbackFunc)(struct UIMenu* menu);
typedef void (*DoubleChangeCallbackFunc)(struct UIInputBoxDouble* input_box);
typedef void (*ChooseChangeCallbackFunc)(struct UIChooseBox* choose_box);
typedef void (*CheckboxChangeCallbackFunc)(struct UICheckbox* checkbox);

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
    bool suppress_input_once;
    void (*render)(struct UIWindow* window);
    void (*layout)(struct UIWindow* window);
    void (*process_input)(struct UIWindow* window);
} UIWindow;

// 菜单
typedef struct UIMenu
{
    UIWidget base;
    UIWidget anchor;  // 菜单内容的参考点，用于菜单整体移动
    const char* title;
    UIWidget** items;
    uint8_t item_count;
    int16_t selected_index;
    struct UIMenu* parent;
    MenuCallbackFunc on_enter;
    MenuCallbackFunc on_exit;
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

typedef struct UICheckbox
{
    UIWidget base;
    const char* text;
    bool checked;
    CheckboxChangeCallbackFunc on_value_changed;
} UICheckbox;

typedef struct UIPopupButton
{
    UIWidget base;
    UIWindow window;
} UIPopupButton;

typedef enum
{
    UI_INPUT_BOX_IDLE,
    UI_INPUT_BOX_EDITING_INT,
    UI_INPUT_BOX_EDITING_FRAC,
} UIInputBoxState;

typedef struct UIInputBoxDouble
{
    UIPopupButton base;
    double value;
    double edit_value;
    uint8_t frac_pos;
    bool cursor_on_unit;
    const double* coeffs;  // 不同单位的显示系数数组：显示值 = 实际值 * coeffs[index]
    const char** suffix;
    uint8_t suffix_count;
    uint8_t selected_suffix_index;
    uint8_t edit_suffix_index;
    uint8_t display_suffix_index;
    bool unit_animating;
    int8_t unit_slide_dir;
    UIWidget unit_curr;
    UIWidget unit_next;
    uint32_t frac_length;
    bool ignore_positive_sgn;  // 是否在显示正数时忽略+号
    UIInputBoxState state;
    DoubleChangeCallbackFunc on_value_changed;
} UIInputBoxDouble;

typedef struct UIChooseBox
{
    UIPopupButton base;
    const char** options;
    uint8_t option_count;
    uint8_t selected_index;
    uint8_t edit_index;
    uint8_t display_index;
    bool is_animating;
    int8_t slide_dir;
    UIWidget option_curr;
    UIWidget option_next;
    ChooseChangeCallbackFunc on_value_changed;
} UIChooseBox;

/**
 * @brief 初始化菜单
 *
 * @param menu 菜单对象指针
 * @param title 菜单标题
 * @param items 菜单项数组
 * @param item_count 菜单项数量
 * @note 传入的items需要在整个菜单生命周期内保持有效
 * @note 仅enter不为NULL的项可被选择
 * @note items中若包含菜单对象，按enter会进入子菜单
 * @note 在子菜单按back会回退到parent菜单（如已设置）
 * @note 可通过menu->on_enter/on_exit注册进出回调
 */
void init_ui_menu(UIMenu* menu, const char* title, UIWidget** items, uint8_t item_count);

/**
 *@brief 初始化标签
 *
 * @param label 标签对象指针
 * @param text 标签文本
 * @note text需在标签生命周期内保持有效
 */
void init_ui_label(UILabel* label, const char* text);

/**
 *@brief 设置标签文本
 *
 * @param label 标签对象指针
 * @param text 新的标签文本
 * @note text需在标签生命周期内保持有效
 */
void ui_label_set_text(UILabel* label, const char* text);

/**
 *@brief 初始化复选框
 *
 * @param checkbox 复选框对象指针
 * @param text 左侧显示文本
 * @param initial_checked 初始选中状态
 * @note 复选框无弹窗，按确认键切换状态
 * @note text需在复选框生命周期内保持有效
 */
void init_ui_checkbox(UICheckbox* checkbox, const char* text, bool initial_checked);

/**
 *@brief 初始化弹窗按钮
 *
 * @param button 弹窗按钮对象指针
 * @param text 按钮文本（也是弹窗标题）
 * @note 按下enter后会弹出窗口，back关闭窗口
 * @note text需在弹窗生命周期内保持有效
 */
void init_ui_popup_button(UIPopupButton* button, const char* text);

/**
 *@brief 初始化双精度输入框
 *
 * @param input_box 输入框对象指针
 * @param title 输入框标题
 * @param initial_value 初始值
 * @param coeffs 单位系数数组，长度需与suffix_count一致
 * @param suffix 后缀字符串数组
 * @param suffix_count 后缀数量
 * @param frac_length 小数位数
 * @param ignore_positive_sgn 是否忽略正号显示
 * @note 输入过程中修改的是临时值，只有按确认键才写回
 * @note 单位可通过scr_up/scr_down切换，显示值保持不变，仅单位变化
 * @note 单位切换后的实际值仅在确认键按下后写回
 * @note suffix与coeffs数组及其内容需在输入框生命周期内保持有效
 */
void init_ui_input_box_double(UIInputBoxDouble* input_box, const char* title, double initial_value, const double* coeffs, const char** suffix, uint8_t suffix_count, uint8_t frac_length, bool ignore_positive_sgn);

/**
 *@brief 初始化选择框
 *
 * @param choose_box 选择框对象指针
 * @param title 选择框标题
 * @param options 选项字符串数组
 * @param option_count 选项数量
 * @param initial_index 初始选中索引
 * @note 左右键切换选项，确认键保存，返回键取消
 * @note options数组及字符串需在选择框生命周期内保持有效
 */
void init_ui_choose_box(UIChooseBox* choose_box, const char* title, const char** options, uint8_t option_count, uint8_t initial_index);

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
extern Key* ui_key_0;
extern Key* ui_key_1;
extern Key* ui_key_2;
extern Key* ui_key_3;
extern Key* ui_key_4;
extern Key* ui_key_5;
extern Key* ui_key_6;
extern Key* ui_key_7;
extern Key* ui_key_8;
extern Key* ui_key_9;
extern Key* ui_key_point;

extern uint32_t ui_update_delta;  // 上次更新到现在的时间差，单位为系统时钟节拍数
extern uint32_t ui_last_update_tick;  // 上次更新的系统时钟节拍数

/**
 *@brief 更新UI显示，应该在主循环中定期调用以刷新UI界面
 *
 * @note 在调用前应确保已设置ui_main_menu及键位映射
 */
void ui_update();

/**
 *@brief 更改UI字体大小，调用后会立即生效并更新界面布局
 *
 * @param ascii_size 新的ASCII字符字体大小，单位为像素宽度，必须是OLEDFontSizeHalf枚举值之一
 * @param chinese_size 新的中文字符字体大小，单位为像素宽度，必须是OLEDFontSizeFull枚举值之一
 * @note 改变字体后需重新布局菜单与窗口
 */
void ui_change_font_size(OLEDFontSizeHalf ascii_size, OLEDFontSizeFull chinese_size);
