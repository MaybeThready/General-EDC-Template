#include "ui.h"
#include <string.h>

UIMenu* ui_main_menu = NULL;
UIWindow* ui_current_window = NULL;
static UIMenu* ui_active_menu = NULL;
static UIMenu* ui_transition_menu = NULL;
static bool ui_menu_transition_active = false;
OLEDFontSizeHalf ui_ascii_size = OLED_6X8_HALF;
OLEDFontSizeFull ui_chinese_size = OLED_8X8_FULL;
uint8_t ui_font_height = 8;
UIWidget ui_cursor = { 0 };
uint32_t ui_update_delta = 0;
uint32_t ui_last_update_tick = 0;

Key* ui_key_left = &keyboard_null_key;
Key* ui_key_up = &keyboard_null_key;
Key* ui_key_right = &keyboard_null_key;
Key* ui_key_down = &keyboard_null_key;
Key* ui_key_enter = &keyboard_null_key;
Key* ui_key_back = &keyboard_null_key;
Key* ui_key_incr = &keyboard_null_key;
Key* ui_key_decr = &keyboard_null_key;
Key* ui_key_scr_up = &keyboard_null_key;
Key* ui_key_scr_down = &keyboard_null_key;
Key* ui_key_0 = &keyboard_null_key;
Key* ui_key_1 = &keyboard_null_key;
Key* ui_key_2 = &keyboard_null_key;
Key* ui_key_3 = &keyboard_null_key;
Key* ui_key_4 = &keyboard_null_key;
Key* ui_key_5 = &keyboard_null_key;
Key* ui_key_6 = &keyboard_null_key;
Key* ui_key_7 = &keyboard_null_key;
Key* ui_key_8 = &keyboard_null_key;
Key* ui_key_9 = &keyboard_null_key;
Key* ui_key_point = &keyboard_null_key;

void init_ui_widget(UIWidget* widget);
static uint16_t ui_text_width(const char* text);
static uint8_t ui_calc_int_length(double display_value);
static double ui_input_box_get_coeff(const UIInputBoxDouble* input_box, uint8_t index);
static int16_t ui_input_box_get_content_x(const UIInputBoxDouble* input_box, uint8_t total_length, uint16_t suffix_length, uint16_t gap_length);
static int16_t ui_input_box_get_unit_x(const UIInputBoxDouble* input_box, int16_t content_x, uint8_t total_length, uint16_t gap_length);
static void ui_input_box_set_unit_widget(UIWidget* widget, int16_t x, int16_t y);
static void ui_input_box_start_unit_slide(UIInputBoxDouble* input_box, uint8_t next_index, bool forward, int16_t unit_x, int16_t content_y);
static void ui_menu_item_render(UIWidget* self);
static void ui_menu_widget_render(UIWidget* self);
static void ui_menu_enter(UIWidget* self);
static void ui_menu_start_transition(UIMenu* from_menu, UIMenu* to_menu, int8_t direction);
static void ui_menu_update_cursor(UIMenu* menu);

/**
 *@brief 在菜单项数组中寻找下一个可选项，所谓可选项是指enter函数指针不为NULL的项
 *
 * @param items 菜单项数组
 * @param item_count 菜单项数量
 * @param selected_index 选中项的索引
 * @param forward 是否向前查找
 * @return true 找到可选项并更新了selected_index
 * @return false 没有找到可选项，selected_index保持不变
 */
static bool find_available_selection(UIWidget** items, uint8_t item_count, int16_t* selected_index, bool forward)
{
    if (items == NULL || item_count == 0 || selected_index == NULL)
    {
        return false;
    }

    int16_t start_index = *selected_index;
    do
    {
        if (forward)
        {
            *selected_index = (*selected_index + 1) % item_count;
        }
        else
        {
            *selected_index = (*selected_index - 1 + item_count) % item_count;
        }

        if (items[*selected_index]->enter != NULL)
        {
            return *selected_index != start_index; // 返回是否找到了不同于起始索引的可选项
        }
    } while (*selected_index != start_index);

    return false;
}

/**
 *@brief 获取数字输入键的瞬时事件
 *
 * @return uint8_t 数字键返回0~9，小数点返回UI_DIGIT_INPUT_POINT，无输入返回UI_NO_DIGIT_INPUT
 */
static uint8_t get_digit_input()
{
    if (ui_key_0->signal_event == KEY_PRESS)
    {
        return 0;
    }
    else if (ui_key_1->signal_event == KEY_PRESS)
    {
        return 1;
    }
    else if (ui_key_2->signal_event == KEY_PRESS)
    {
        return 2;
    }
    else if (ui_key_3->signal_event == KEY_PRESS)
    {
        return 3;
    }
    else if (ui_key_4->signal_event == KEY_PRESS)
    {
        return 4;
    }
    else if (ui_key_5->signal_event == KEY_PRESS)
    {
        return 5;
    }
    else if (ui_key_6->signal_event == KEY_PRESS)
    {
        return 6;
    }
    else if (ui_key_7->signal_event == KEY_PRESS)
    {
        return 7;
    }
    else if (ui_key_8->signal_event == KEY_PRESS)
    {
        return 8;
    }
    else if (ui_key_9->signal_event == KEY_PRESS)
    {
        return 9;
    }
    else if (ui_key_point->signal_event == KEY_PRESS)
    {
        return UI_DIGIT_INPUT_POINT; // 返回10表示小数点被按下
    }
    else
    {
        return UI_NO_DIGIT_INPUT; // 返回255表示没有数字键被按下
    }
}

/**
 *@brief 计算字符串在当前ASCII字体下的显示宽度
 *
 * @param text 文本指针
 * @return uint16_t 显示宽度（像素）
 */
static uint16_t ui_text_width(const char* text)
{
    if (text == NULL)
    {
        return 0;
    }

    return (uint16_t)(strlen(text) * ui_ascii_size);
}

/**
 *@brief 计算数值显示所需的整数部分字符长度（包含负号）
 *
 * @param display_value 显示值
 * @return uint8_t 整数部分字符长度
 */
static uint8_t ui_calc_int_length(double display_value)
{
    double abs_value = fabs(display_value);
    return (uint8_t)((display_value < 0 ? 1 : 0) + (abs_value < 1.0 ? 1 : (int)log10(abs_value) + 1));
}

/**
 *@brief 获取输入框指定单位的显示系数
 *
 * @param input_box 输入框对象指针
 * @param index 单位索引
 * @return double 显示系数
 */
static double ui_input_box_get_coeff(const UIInputBoxDouble* input_box, uint8_t index)
{
    if (input_box->coeffs == NULL || input_box->suffix_count == 0)
    {
        return 1.0;
    }

    if (index >= input_box->suffix_count)
    {
        index = 0;
    }

    return input_box->coeffs[index];
}

/**
 *@brief 计算输入框窗口内数值的居中起始X坐标
 *
 * @param input_box 输入框对象指针
 * @param total_length 数值字符总长度
 * @param suffix_length 单位显示宽度
 * @param gap_length 数值与单位之间的空隙宽度
 * @return int16_t 起始X坐标
 */
static int16_t ui_input_box_get_content_x(const UIInputBoxDouble* input_box, uint8_t total_length, uint16_t suffix_length, uint16_t gap_length)
{
    int16_t window_width = (int16_t)input_box->base.window.base.width;
    if (input_box->base.window.base.target_width > window_width)
    {
        window_width = (int16_t)input_box->base.window.base.target_width;
    }

    return (int16_t)(input_box->base.window.base.x + (window_width - (int16_t)(total_length * ui_ascii_size + gap_length + suffix_length)) / 2);
}

/**
 *@brief 计算单位文本的X坐标
 *
 * @param input_box 输入框对象指针
 * @param content_x 数值起始X坐标
 * @param total_length 数值字符总长度
 * @param gap_length 数值与单位间距
 * @return int16_t 单位X坐标
 */
static int16_t ui_input_box_get_unit_x(const UIInputBoxDouble* input_box, int16_t content_x, uint8_t total_length, uint16_t gap_length)
{
    (void)input_box;
    return (int16_t)(content_x + total_length * ui_ascii_size + gap_length);
}

/**
 *@brief 设置单位文本控件的目标位置
 *
 * @param widget 单位控件指针
 * @param x 目标X坐标
 * @param y 目标Y坐标
 */
static void ui_input_box_set_unit_widget(UIWidget* widget, int16_t x, int16_t y)
{
    widget->target_x = x;
    widget->target_y = y;
    widget->target_width = 0.f;
    widget->target_height = 0.f;
    widget->should_move = true;
}

/**
 *@brief 启动单位文本的竖直滑动动画
 *
 * @param input_box 输入框对象指针
 * @param next_index 目标单位索引
 * @param forward true表示向上切换，false表示向下切换
 */
static void ui_input_box_start_unit_slide(UIInputBoxDouble* input_box, uint8_t next_index, bool forward, int16_t unit_x, int16_t content_y)
{
    int16_t off_top = (int16_t)(input_box->base.window.base.y - ui_font_height - UI_V_MARGIN);
    int16_t off_bottom = (int16_t)(input_box->base.window.base.y + input_box->base.window.base.height + UI_V_MARGIN);

    input_box->unit_animating = true;
    input_box->unit_slide_dir = forward ? 1 : -1;
    input_box->display_suffix_index = input_box->edit_suffix_index;
    input_box->edit_suffix_index = next_index;

    input_box->unit_curr.x = (float)unit_x;
    input_box->unit_curr.y = (float)content_y;
    input_box->unit_next.x = (float)unit_x;

    if (forward)
    {
        ui_input_box_set_unit_widget(&input_box->unit_curr, input_box->unit_curr.x, off_top);
        input_box->unit_next.y = (float)off_bottom;
        ui_input_box_set_unit_widget(&input_box->unit_next, input_box->unit_next.x, content_y);
    }
    else
    {
        ui_input_box_set_unit_widget(&input_box->unit_curr, input_box->unit_curr.x, off_bottom);
        input_box->unit_next.y = (float)off_top;
        ui_input_box_set_unit_widget(&input_box->unit_next, input_box->unit_next.x, content_y);
    }
}

/**
 *@brief UIWidget成员函数：让控件向目标位置和尺寸逐步移动
 *
 * @param widget UIWidget对象指针
 */
void ui_widget_step(UIWidget* widget)
{
    if (widget->should_move)
    {
        if (fabs(widget->x - widget->target_x) +
            fabs(widget->y - widget->target_y) +
            fabs(widget->width - widget->target_width) +
            fabs(widget->height - widget->target_height) < 4 * UI_EPSILON)
        {
            widget->x = widget->target_x;
            widget->y = widget->target_y;
            widget->width = widget->target_width;
            widget->height = widget->target_height;
            widget->should_move = false;
        }
        else
        {
            widget->x += (widget->target_x - widget->x) * UI_MOVE_SPEED_RATIO * ui_update_delta / SYS_TICK_PERIOD;
            widget->y += (widget->target_y - widget->y) * UI_MOVE_SPEED_RATIO * ui_update_delta / SYS_TICK_PERIOD;
            widget->width += (widget->target_width - widget->width) * UI_MOVE_SPEED_RATIO * ui_update_delta / SYS_TICK_PERIOD;
            widget->height += (widget->target_height - widget->height) * UI_MOVE_SPEED_RATIO * ui_update_delta / SYS_TICK_PERIOD;
        }
    }
}

static void ui_menu_item_render(UIWidget* self)
{
    UIMenu* menu = container_of(self, UIMenu, base);
    oled_show_mix_string_area(
        (int16_t)(menu->base.x),
        (int16_t)(menu->base.y),
        (int16_t)(menu->base.width),
        (int16_t)(menu->base.height),
        (int16_t)(menu->base.x),
        (int16_t)(menu->base.y),
        menu->title,
        ui_chinese_size,
        ui_ascii_size
    );
}

static void ui_menu_widget_render(UIWidget* self)
{
    UIMenu* menu = container_of(self, UIMenu, base);
    if (menu == ui_active_menu || menu->anchor.should_move || menu == ui_transition_menu)
    {
        if (menu->render != NULL)
        {
            menu->render(menu);
        }
        return;
    }

    ui_menu_item_render(self);
}

static void ui_menu_start_transition(UIMenu* from_menu, UIMenu* to_menu, int8_t direction)
{
    if (to_menu == NULL || from_menu == to_menu)
    {
        return;
    }

    ui_transition_menu = from_menu;
    ui_active_menu = to_menu;
    ui_menu_transition_active = true;

    from_menu->anchor.target_x = (direction > 0) ? -OLED_WIDTH : OLED_WIDTH;
    from_menu->anchor.target_y = 0.f;
    from_menu->anchor.should_move = true;

    to_menu->anchor.x = (direction > 0) ? OLED_WIDTH : -OLED_WIDTH;
    to_menu->anchor.y = 0.f;
    to_menu->anchor.target_x = 0.f;
    to_menu->anchor.target_y = 0.f;
    to_menu->anchor.should_move = true;

    ui_menu_update_cursor(to_menu);
}

static void ui_menu_enter(UIWidget* self)
{
    UIMenu* menu = container_of(self, UIMenu, base);
    if (ui_active_menu == NULL)
    {
        ui_active_menu = ui_main_menu;
    }

    if (ui_active_menu != NULL && menu != ui_active_menu)
    {
        menu->parent = ui_active_menu;
        if (ui_active_menu->on_exit != NULL)
        {
            ui_active_menu->on_exit(ui_active_menu);
        }
        if (menu->on_enter != NULL)
        {
            menu->on_enter(menu);
        }
        ui_menu_start_transition(ui_active_menu, menu, 1);
    }
}

/**
 *@brief UIMenu成员函数：根据当前选中项更新光标目标区域
 *
 * @param menu UIMenu对象指针
 */
static void ui_menu_update_cursor(UIMenu* menu)
{
    UIWidget* selected_item = menu->items[menu->selected_index];
    float anchor_x = menu->anchor.should_move ? menu->anchor.target_x : menu->anchor.x;
    ui_cursor.target_x = anchor_x;
    ui_cursor.target_y = selected_item->target_y - UI_V_MARGIN / 2;
    ui_cursor.target_width = OLED_WIDTH;
    ui_cursor.target_height = selected_item->target_height + UI_V_MARGIN;
    ui_cursor.should_move = true;
}

/**
 *@brief UIMenu成员函数：渲染菜单及其子项，并处理菜单/弹窗输入
 *
 * @param menu UIMenu对象指针
 */
void ui_menu_render_items(UIMenu* menu)
{
    if (menu->anchor.should_move)
    {
        ui_widget_step(&menu->anchor);
    }

    if (menu->layout != NULL)
    {
        menu->layout(menu);
    }

    if (menu == ui_active_menu && !ui_menu_transition_active && menu->process_input != NULL && ui_current_window == NULL)
    {
        menu->process_input(menu);
    }

    oled_draw_rectangle((int16_t)menu->anchor.x, 0, (int16_t)(ui_ascii_size / 2), ui_font_height, true);
    oled_show_mix_string(
        (int16_t)(menu->anchor.x + ui_ascii_size),
        0,
        menu->title,
        ui_chinese_size,
        ui_ascii_size
    );

    UIWidget* item;
    for (uint8_t i = 0; i < menu->item_count; i++)
    {
        item = menu->items[i];
        if (item->render != NULL)
        {
            item->render(item);
        }
    }

    if (menu == ui_active_menu && ui_current_window != NULL)
    {
        ui_current_window->render(ui_current_window);
        if (ui_current_window->is_exiting)
        {
            ui_menu_update_cursor(menu);
            if (!ui_current_window->base.should_move)
            {
                ui_current_window->is_exiting = false;
                ui_current_window = NULL;
            }
        }
    }

    if (menu == ui_active_menu)
    {
        if (ui_cursor.should_move)
        {
            ui_widget_step(&ui_cursor);
        }
        oled_reverse_area(
            (int16_t)ui_cursor.x,
            (int16_t)ui_cursor.y,
            (int16_t)ui_cursor.width,
            (int16_t)ui_cursor.height
        );
    }
}

/**
 *@brief UIMenu成员函数：渲染菜单标题区域
 *
 * @param self UIWidget对象指针（指向UIMenu.base）
 */
void ui_menu_render_self(UIWidget* self)
{
    UIMenu* menu = container_of(self, UIMenu, base);
    oled_show_mix_string_area(
        (int16_t)(menu->base.x),
        (int16_t)(menu->base.y),
        (int16_t)(menu->base.width),
        (int16_t)(menu->base.height),
        (int16_t)(menu->base.x),
        (int16_t)(menu->base.y),
        menu->title,
        ui_chinese_size,
        ui_ascii_size
    );
}

/**
 *@brief UIMenu成员函数：根据字体大小对菜单项进行布局
 *
 * @param menu UIMenu对象指针
 */
void ui_menu_layout(UIMenu* menu)
{
    int16_t y = (int16_t)(menu->anchor.y + ui_font_height + UI_V_MARGIN); // 从标题下方开始布局
    for (uint8_t i = 0; i < menu->item_count; i++)
    {
        UIWidget* item = menu->items[i];
        item->x = (float)(menu->anchor.x + UI_H_MARGIN);
        item->y = (float)y;
        item->width = (float)(OLED_WIDTH - 2 * UI_H_MARGIN);
        item->height = (float)ui_font_height;
        item->target_x = item->x;
        item->target_y = item->y;
        item->target_width = item->width;
        item->target_height = item->height;
        item->should_move = false;
        y += item->target_height + UI_V_MARGIN;
    }
}

/**
 *@brief UIMenu成员函数：处理菜单输入并更新选中项
 *
 * @param menu UIMenu对象指针
 */
void ui_menu_process_input(UIMenu* menu)
{
    bool selection_changed = false;
    if (ui_key_up->signal_event == KEY_PRESS)
    {
        selection_changed = find_available_selection(menu->items, menu->item_count, &menu->selected_index, false);
    }
    if (ui_key_down->signal_event == KEY_PRESS)
    {
        selection_changed = find_available_selection(menu->items, menu->item_count, &menu->selected_index, true);
    }
    if (ui_key_enter->signal_event == KEY_PRESS)
    {
        UIWidget* selected_item = menu->items[menu->selected_index];
        if (selected_item->enter != NULL)
        {
            selected_item->enter(selected_item);
        }
    }

    if (ui_key_back->signal_event == KEY_PRESS && menu->parent != NULL)
    {
        if (menu->on_exit != NULL)
        {
            menu->on_exit(menu);
        }
        if (menu->parent->on_enter != NULL)
        {
            menu->parent->on_enter(menu->parent);
        }
        ui_menu_start_transition(menu, menu->parent, -1);
        return;
    }

    if (selection_changed)
    {
        ui_menu_update_cursor(menu);
    }
}

/**
 *@brief UIWindow成员函数：渲染窗口自身（清空区域并绘制边框）
 *
 * @param self UIWidget对象指针（指向UIWindow.base）
 */
void ui_window_render_self(UIWidget* self)
{
    UIWindow* window = container_of(self, UIWindow, base);
    if (window->base.should_move)
    {
        window->base.step(&window->base);
    }
    oled_clear_area(
        (int16_t)window->base.x,
        (int16_t)window->base.y,
        (int16_t)window->base.width,
        (int16_t)window->base.height
    );
    oled_draw_rectangle(
        (int16_t)window->base.x,
        (int16_t)window->base.y,
        (int16_t)window->base.width,
        (int16_t)window->base.height,
        false
    );
}

/**
 *@brief UIWindow成员函数：渲染窗口并处理窗口输入
 *
 * @param window UIWindow对象指针
 */
void ui_window_render_items(UIWindow* window)
{
    if (window->layout != NULL)
    {
        window->layout(window);
    }

    if (window->suppress_input_once)
    {
        window->suppress_input_once = false;
    }
    else if (window->process_input != NULL)
    {
        window->process_input(window);
    }

    window->base.render(&window->base);
}

/**
 *@brief UILabel成员函数：渲染标签文本
 *
 * @param self UIWidget对象指针（指向UILabel.base）
 */
void ui_label_render(UIWidget* self)
{
    UILabel* label = container_of(self, UILabel, base);
    oled_show_mix_string_area(
        (int16_t)(label->base.x),
        (int16_t)(label->base.y),
        (int16_t)(label->base.width),
        (int16_t)(label->base.height),
        (int16_t)(label->base.x),
        (int16_t)(label->base.y),
        label->text,
        ui_chinese_size,
        ui_ascii_size
    );
}

/**
 *@brief UICheckbox成员函数：渲染复选框
 *
 * @param self UIWidget对象指针（指向UICheckbox.base）
 */
void ui_checkbox_render(UIWidget* self)
{
    UICheckbox* checkbox = container_of(self, UICheckbox, base);
    int16_t box_size = (int16_t)(ui_font_height - 2);
    if (box_size < 4)
    {
        box_size = 4;
    }

    int16_t box_x = (int16_t)(checkbox->base.x + checkbox->base.width - UI_H_MARGIN - box_size);
    int16_t box_y = (int16_t)(checkbox->base.y + (checkbox->base.height - box_size) / 2);

    oled_show_mix_string_area(
        (int16_t)(checkbox->base.x),
        (int16_t)(checkbox->base.y),
        (int16_t)(checkbox->base.width),
        (int16_t)(checkbox->base.height),
        (int16_t)(checkbox->base.x),
        (int16_t)(checkbox->base.y),
        checkbox->text,
        ui_chinese_size,
        ui_ascii_size
    );

    oled_draw_rectangle(box_x, box_y, box_size, box_size, false);

    if (checkbox->checked)
    {
        int16_t inner_size = (int16_t)(box_size - 4);
        if (inner_size < 2)
        {
            inner_size = 2;
        }
        int16_t inner_x = (int16_t)(box_x + (box_size - inner_size) / 2);
        int16_t inner_y = (int16_t)(box_y + (box_size - inner_size) / 2);
        oled_draw_rectangle(inner_x, inner_y, inner_size, inner_size, true);
    }
}

/**
 *@brief UICheckbox成员函数：切换复选框状态
 *
 * @param self UIWidget对象指针（指向UICheckbox.base）
 */
void ui_checkbox_enter(UIWidget* self)
{
    UICheckbox* checkbox = container_of(self, UICheckbox, base);
    checkbox->checked = !checkbox->checked;
    if (checkbox->on_value_changed != NULL)
    {
        checkbox->on_value_changed(checkbox);
    }
}

/**
 *@brief UIPopupButton成员函数：渲染弹窗按钮的显示文本
 *
 * @param self UIWidget对象指针（指向UIPopupButton.base）
 */
void ui_popup_button_render(UIWidget* self)
{
    UIPopupButton* button = container_of(self, UIPopupButton, base);

    oled_show_mix_string_area(
        (int16_t)(button->base.target_x),
        (int16_t)(button->base.target_y),
        (int16_t)(button->base.target_width),
        (int16_t)(button->base.target_height),
        (int16_t)(button->base.x),
        (int16_t)(button->base.y),
        button->window.title,
        ui_chinese_size,
        ui_ascii_size
    );
}

/**
 *@brief UIPopupButton成员函数：进入弹窗并启动弹窗展开动画
 *
 * @param self UIWidget对象指针（指向UIPopupButton.base）
 */
void ui_popup_button_enter(UIWidget* self)
{
    UIPopupButton* button = container_of(self, UIPopupButton, base);
    ui_current_window = &button->window;
    button->window.suppress_input_once = true;

    button->window.base.target_width = UI_WINDOW_WIDTH;
    button->window.base.target_height = UI_WINDOW_HEIGHT;
    button->window.base.should_move = true;

    ui_cursor.target_width = 0.f;
    ui_cursor.target_height = 0.f;
    ui_cursor.should_move = true;
}

/**
 *@brief UIPopupButton成员函数：渲染弹窗窗口及标题
 *
 * @param window UIWindow对象指针（指向UIPopupButton.window）
 */
void ui_popup_button_window_render_items(UIWindow* window)
{
    ui_window_render_items(window);

    oled_show_mix_string_area(
        (int16_t)(window->base.x),
        (int16_t)(window->base.y),
        (int16_t)(window->base.width),
        (int16_t)(window->base.height),
        (int16_t)(window->base.x + UI_H_MARGIN + 1),  // 加一是因为有边框
        (int16_t)(window->base.y + UI_H_MARGIN + 1),  // 标题行用H_MARGIN
        window->title,
        ui_chinese_size,
        ui_ascii_size
    );
}

/**
 *@brief UIPopupButton成员函数：处理弹窗窗口输入（返回键关闭）
 *
 * @param window UIWindow对象指针（指向UIPopupButton.window）
 */
void ui_popup_button_window_process_input(UIWindow* window)
{
    // 打开弹窗后，键盘事件的处理权限转移到弹窗窗口，弹窗有义务在适当的时候将权限转回主菜单
    UIPopupButton* button = container_of(window, UIPopupButton, window);
    if (ui_key_back->signal_event == KEY_PRESS)
    {
        window->base.target_width = 0.f;
        window->base.target_height = 0.f;
        window->is_exiting = true;
        window->base.should_move = true;
    }
}

/**
 *@brief UIInputBoxDouble成员函数：渲染输入框在菜单中的显示
 *
 * @param self UIWidget对象指针（指向UIInputBoxDouble.base.base）
 */
void ui_input_box_double_render(UIWidget* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base);
    ui_popup_button_render(&input_box->base.base);

    double display_value = input_box->value * ui_input_box_get_coeff(input_box, input_box->selected_suffix_index);
    uint8_t int_length = ui_calc_int_length(display_value);
    uint8_t total_length = int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn; // 数值的总长度（整数部分长度+小数部分长度+小数点+正负号）
    uint16_t suffix_length = 0;

    if (input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        suffix_length = ui_text_width(input_box->suffix[input_box->selected_suffix_index]);
        oled_show_mix_string_area(
            input_box->base.base.x,
            input_box->base.base.y,
            input_box->base.base.width,
            input_box->base.base.height,
            OLED_WIDTH - UI_H_MARGIN - suffix_length,
            input_box->base.base.y,
            input_box->suffix[input_box->selected_suffix_index],
            ui_chinese_size,
            ui_ascii_size
        );
    }

    oled_show_float_num_area(
        input_box->base.base.x,
        input_box->base.base.y,
        input_box->base.base.width,
        input_box->base.base.height,
        OLED_WIDTH - UI_H_MARGIN - 1 - total_length * ui_ascii_size - suffix_length - ui_ascii_size, // 数值右对齐，留出UI_H_MARGIN的右边距
        input_box->base.base.y,
        display_value,
        int_length,
        input_box->frac_length,
        input_box->ignore_positive_sgn,
        ui_ascii_size
    );
}

/**
 *@brief UIInputBoxDouble成员函数：更新输入框的光标位置
 *
 * @param input_box UIInputBoxDouble对象指针
 */
void ui_input_box_double_update_cursor(UIInputBoxDouble* input_box)
{
    double display_value = input_box->edit_value * ui_input_box_get_coeff(input_box, input_box->edit_suffix_index);
    uint8_t int_length = ui_calc_int_length(display_value);
    uint8_t total_length = int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn;
    uint16_t suffix_length = 0;
    uint16_t gap_length = 0;
    int16_t content_x;
    int16_t unit_x;

    if (input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        suffix_length = ui_text_width(input_box->suffix[input_box->edit_suffix_index]);
        gap_length = ui_ascii_size;
    }

    content_x = ui_input_box_get_content_x(input_box, total_length, suffix_length, gap_length);
    unit_x = ui_input_box_get_unit_x(input_box, content_x, total_length, gap_length);

    if (input_box->cursor_on_unit && suffix_length > 0)
    {
        ui_cursor.target_x = unit_x - UI_H_MARGIN;
        ui_cursor.target_width = (int16_t)suffix_length + UI_H_MARGIN * 2;
    }
    else
    {
        ui_cursor.target_x = content_x - UI_H_MARGIN;
        ui_cursor.target_width = (int16_t)(total_length * ui_ascii_size) + UI_H_MARGIN * 2; // 光标宽度略大于数值显示区域，留出一些余量
    }
    ui_cursor.target_y = input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN / 2;
    ui_cursor.target_height = (int16_t)ui_font_height + UI_V_MARGIN;
    ui_cursor.should_move = true;
}

/**
 *@brief UIInputBoxDouble成员函数：进入输入框并初始化编辑状态
 *
 * @param self UIWidget对象指针（指向UIInputBoxDouble.base.base）
 */
void ui_input_box_double_enter(UIWidget* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base);
    ui_popup_button_enter(&input_box->base.base);

    input_box->edit_value = input_box->value;
    input_box->edit_suffix_index = input_box->selected_suffix_index;
    input_box->display_suffix_index = input_box->selected_suffix_index;
    input_box->unit_animating = false;
    input_box->frac_pos = 0;
    input_box->cursor_on_unit = false;
    ui_input_box_double_update_cursor(input_box);
    input_box->state = UI_INPUT_BOX_IDLE; // 进入编辑整数部分状态

    init_ui_widget(&input_box->unit_curr);
    init_ui_widget(&input_box->unit_next);
}

/**
 *@brief UIInputBoxDouble成员函数：渲染输入框窗口内容
 *
 * @param self UIWindow对象指针（指向UIInputBoxDouble.base.window）
 */
void ui_input_box_double_window_render_items(UIWindow* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base.window);

    ui_popup_button_window_render_items(&input_box->base.window);
    double display_value = input_box->edit_value * ui_input_box_get_coeff(input_box, input_box->edit_suffix_index);
    uint8_t int_length = ui_calc_int_length(display_value);
    uint8_t total_length = int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn;
    uint16_t suffix_length = 0;
    uint16_t gap_length = 0;
    int16_t content_x;
    int16_t unit_x;
    int16_t content_y;

    if (input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        suffix_length = ui_text_width(input_box->suffix[input_box->edit_suffix_index]);
        gap_length = ui_ascii_size;

    }

    content_x = ui_input_box_get_content_x(input_box, total_length, suffix_length, gap_length);
    unit_x = ui_input_box_get_unit_x(input_box, content_x, total_length, gap_length);
    content_y = (int16_t)(input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN);

    oled_show_float_num_area(
        input_box->base.window.base.x,
        input_box->base.window.base.y,
        input_box->base.window.base.width,
        input_box->base.window.base.height,
        content_x,
        content_y, // 数值行在标题下方，间隔UI_V_MARGIN
        display_value,
        int_length,
        input_box->frac_length,
        input_box->ignore_positive_sgn,
        ui_ascii_size
    );

    if (input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        if (input_box->unit_curr.should_move)
        {
            input_box->unit_curr.step(&input_box->unit_curr);
        }
        if (input_box->unit_next.should_move)
        {
            input_box->unit_next.step(&input_box->unit_next);
        }

        if (!input_box->unit_animating)
        {
            input_box->unit_curr.x = (float)unit_x;
            input_box->unit_curr.y = (float)content_y;
        }
        else
        {
            input_box->unit_curr.x = (float)unit_x;
            input_box->unit_next.x = (float)unit_x;
        }

        oled_show_mix_string_area(
            input_box->base.window.base.x,
            input_box->base.window.base.y,
            input_box->base.window.base.width,
            input_box->base.window.base.height,
            (int16_t)(input_box->unit_curr.x),
            (int16_t)(input_box->unit_curr.y),
            input_box->suffix[input_box->display_suffix_index],
            ui_chinese_size,
            ui_ascii_size
        );

        if (input_box->unit_animating)
        {
            oled_show_mix_string_area(
                input_box->base.window.base.x,
                input_box->base.window.base.y,
                input_box->base.window.base.width,
                input_box->base.window.base.height,
                (int16_t)(input_box->unit_next.x),
                (int16_t)(input_box->unit_next.y),
                input_box->suffix[input_box->edit_suffix_index],
                ui_chinese_size,
                ui_ascii_size
            );

            if (!input_box->unit_curr.should_move && !input_box->unit_next.should_move)
            {
                input_box->unit_animating = false;
                input_box->display_suffix_index = input_box->edit_suffix_index;
                input_box->unit_curr = input_box->unit_next;
            }
        }
    }
}

/**
 *@brief UIInputBoxDouble成员函数：处理输入框窗口输入
 *
 * @param self UIWindow对象指针（指向UIInputBoxDouble.base.window）
 */
void ui_input_box_double_window_process_input(UIWindow* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base.window);
    if (ui_key_back->signal_event == KEY_PRESS)
    {
        input_box->edit_value = input_box->value;
        input_box->edit_suffix_index = input_box->selected_suffix_index;
        input_box->display_suffix_index = input_box->selected_suffix_index;
        input_box->unit_animating = false;
        input_box->frac_pos = 0;
        input_box->cursor_on_unit = false;
        input_box->state = UI_INPUT_BOX_IDLE;
        ui_popup_button_window_process_input(&input_box->base.window);
        return;
    }

    ui_popup_button_window_process_input(&input_box->base.window);

    uint8_t digit = get_digit_input();
    if (!input_box->unit_animating && input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        if (ui_key_scr_up->signal_event == KEY_PRESS)
        {
            uint8_t next_index = (uint8_t)((input_box->edit_suffix_index + 1) % input_box->suffix_count);
            double display_value = input_box->edit_value * ui_input_box_get_coeff(input_box, input_box->edit_suffix_index);
            input_box->edit_value = display_value / ui_input_box_get_coeff(input_box, next_index);
            {
                uint8_t int_length = ui_calc_int_length(display_value);
                uint8_t total_length = int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn;
                uint16_t suffix_length = ui_text_width(input_box->suffix[next_index]);
                uint16_t gap_length = ui_ascii_size;
                int16_t content_x = ui_input_box_get_content_x(input_box, total_length, suffix_length, gap_length);
                int16_t unit_x = ui_input_box_get_unit_x(input_box, content_x, total_length, gap_length);
                int16_t content_y = (int16_t)(input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN);
                ui_input_box_start_unit_slide(input_box, next_index, true, unit_x, content_y);
            }
            input_box->cursor_on_unit = true;
            ui_input_box_double_update_cursor(input_box);
        }
        else if (ui_key_scr_down->signal_event == KEY_PRESS)
        {
            uint8_t next_index = (uint8_t)((input_box->edit_suffix_index + input_box->suffix_count - 1) % input_box->suffix_count);
            double display_value = input_box->edit_value * ui_input_box_get_coeff(input_box, input_box->edit_suffix_index);
            input_box->edit_value = display_value / ui_input_box_get_coeff(input_box, next_index);
            {
                uint8_t int_length = ui_calc_int_length(display_value);
                uint8_t total_length = int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn;
                uint16_t suffix_length = ui_text_width(input_box->suffix[next_index]);
                uint16_t gap_length = ui_ascii_size;
                int16_t content_x = ui_input_box_get_content_x(input_box, total_length, suffix_length, gap_length);
                int16_t unit_x = ui_input_box_get_unit_x(input_box, content_x, total_length, gap_length);
                int16_t content_y = (int16_t)(input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN);
                ui_input_box_start_unit_slide(input_box, next_index, false, unit_x, content_y);
            }
            input_box->cursor_on_unit = true;
            ui_input_box_double_update_cursor(input_box);
        }
    }

    switch (input_box->state)
    {
    case UI_INPUT_BOX_IDLE:
        if (digit != UI_NO_DIGIT_INPUT)
        {
            input_box->edit_value = (double)((digit == UI_DIGIT_INPUT_POINT) ? 0.0 : digit) / ui_input_box_get_coeff(input_box, input_box->edit_suffix_index); // 将输入的数字转换为实际值
            if (digit == UI_DIGIT_INPUT_POINT)
            {
                input_box->frac_pos = 0;
                input_box->state = UI_INPUT_BOX_EDITING_FRAC;
            }
            else
            {
                input_box->frac_pos = 0;
                input_box->state = UI_INPUT_BOX_EDITING_INT;
            }
            input_box->cursor_on_unit = false;
            ui_input_box_double_update_cursor(input_box);
        }
        break;

    case UI_INPUT_BOX_EDITING_INT:
        if (digit != UI_NO_DIGIT_INPUT && digit != UI_DIGIT_INPUT_POINT)
        {
            input_box->edit_value = input_box->edit_value * 10 + (double)digit / ui_input_box_get_coeff(input_box, input_box->edit_suffix_index); // 将输入的数字转换为实际值并添加到整数部分
            input_box->cursor_on_unit = false;
            ui_input_box_double_update_cursor(input_box);
        }
        else if (digit == UI_DIGIT_INPUT_POINT)
        {
            input_box->frac_pos = 0;
            input_box->state = UI_INPUT_BOX_EDITING_FRAC;
            input_box->cursor_on_unit = false;
            ui_input_box_double_update_cursor(input_box);
        }
        break;

    case UI_INPUT_BOX_EDITING_FRAC:
        if (digit != UI_NO_DIGIT_INPUT && digit != UI_DIGIT_INPUT_POINT)
        {
            if (input_box->frac_pos < input_box->frac_length)
            {
                input_box->frac_pos++;
                input_box->edit_value += (double)digit / ui_input_box_get_coeff(input_box, input_box->edit_suffix_index) * pow(10.0, -(double)input_box->frac_pos);
                input_box->cursor_on_unit = false;
                ui_input_box_double_update_cursor(input_box);
            }
        }
        break;
    }

    if (ui_key_enter->signal_event == KEY_PRESS)
    {
        input_box->value = input_box->edit_value;
        input_box->selected_suffix_index = input_box->edit_suffix_index;
        input_box->display_suffix_index = input_box->edit_suffix_index;
        input_box->unit_animating = false;
        input_box->frac_pos = 0;
        input_box->cursor_on_unit = false;
        if (input_box->on_value_changed != NULL)
        {
            input_box->on_value_changed(input_box);
        }
        input_box->state = UI_INPUT_BOX_IDLE;
        input_box->base.window.base.target_width = 0.f;
        input_box->base.window.base.target_height = 0.f;
        input_box->base.window.is_exiting = true;
        input_box->base.window.base.should_move = true;
    }
}

static const char* ui_choose_box_get_text(UIChooseBox* choose_box, uint8_t index)
{
    if (choose_box->options == NULL || choose_box->option_count == 0)
    {
        return "";
    }

    if (index >= choose_box->option_count)
    {
        index = 0;
    }

    return choose_box->options[index] != NULL ? choose_box->options[index] : "";
}

static int16_t ui_choose_box_get_content_y(UIWindow* window)
{
    return (int16_t)(window->base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN);
}

static int16_t ui_choose_box_get_effective_width(UIWindow* window)
{
    float width = window->base.width;
    if (window->base.target_width > width)
    {
        width = window->base.target_width;
    }

    return (int16_t)width;
}

static int16_t ui_choose_box_get_center_x(UIWindow* window, uint16_t text_width)
{
    int16_t width = ui_choose_box_get_effective_width(window);
    return (int16_t)(window->base.x + (width - (int16_t)text_width) / 2);
}

static void ui_choose_box_set_widget_text(UIWidget* widget, int16_t x, int16_t y, uint16_t width)
{
    widget->target_x = x;
    widget->target_y = y;
    widget->target_width = width;
    widget->target_height = ui_font_height;
    widget->should_move = true;
}

static void ui_choose_box_start_slide(UIChooseBox* choose_box, int8_t dir, uint8_t next_index)
{
    UIWindow* window = &choose_box->base.window;
    const char* next_text = ui_choose_box_get_text(choose_box, next_index);
    uint16_t next_width = ui_text_width(next_text);
    int16_t content_y = ui_choose_box_get_content_y(window);
    int16_t center_x = ui_choose_box_get_center_x(window, next_width);
    int16_t width = ui_choose_box_get_effective_width(window);

    int16_t off_left = (int16_t)(window->base.x - (int16_t)next_width - UI_H_MARGIN);
    int16_t off_right = (int16_t)(window->base.x + width + UI_H_MARGIN);
    int16_t start_x = (dir > 0) ? off_right : off_left;
    int16_t exit_x = (dir > 0) ? off_left : off_right;

    choose_box->slide_dir = dir;
    choose_box->is_animating = true;

    choose_box->option_next.x = (float)start_x;
    choose_box->option_next.y = (float)content_y;
    choose_box->option_next.width = (float)next_width;
    choose_box->option_next.height = (float)ui_font_height;

    ui_choose_box_set_widget_text(&choose_box->option_next, center_x, content_y, next_width);
    ui_choose_box_set_widget_text(&choose_box->option_curr, exit_x, content_y, (uint16_t)choose_box->option_curr.width);

    ui_cursor.target_x = center_x - UI_H_MARGIN;
    ui_cursor.target_y = content_y - UI_V_MARGIN / 2;
    ui_cursor.target_width = next_width + UI_H_MARGIN * 2;
    ui_cursor.target_height = ui_font_height + UI_V_MARGIN;
    ui_cursor.should_move = true;
}

/**
 *@brief UIChooseBox成员函数：渲染选择框在菜单中的显示
 *
 * @param self UIWidget对象指针（指向UIChooseBox.base.base）
 */
void ui_choose_box_render(UIWidget* self)
{
    UIChooseBox* choose_box = container_of(self, UIChooseBox, base);
    const char* value_text = ui_choose_box_get_text(choose_box, choose_box->selected_index);
    uint16_t value_width = ui_text_width(value_text);

    oled_show_mix_string_area(
        (int16_t)(choose_box->base.base.target_x),
        (int16_t)(choose_box->base.base.target_y),
        (int16_t)(choose_box->base.base.target_width),
        (int16_t)(choose_box->base.base.target_height),
        (int16_t)(choose_box->base.base.x),
        (int16_t)(choose_box->base.base.y),
        choose_box->base.window.title,
        ui_chinese_size,
        ui_ascii_size
    );

    oled_show_mix_string_area(
        (int16_t)(choose_box->base.base.target_x),
        (int16_t)(choose_box->base.base.target_y),
        (int16_t)(choose_box->base.base.target_width),
        (int16_t)(choose_box->base.base.target_height),
        (int16_t)(choose_box->base.base.x + choose_box->base.base.width - UI_H_MARGIN - value_width),
        (int16_t)(choose_box->base.base.y),
        value_text,
        ui_chinese_size,
        ui_ascii_size
    );
}

/**
 *@brief UIChooseBox成员函数：进入选择框并初始化编辑状态
 *
 * @param self UIWidget对象指针（指向UIChooseBox.base.base）
 */
void ui_choose_box_enter(UIWidget* self)
{
    UIChooseBox* choose_box = container_of(self, UIChooseBox, base);
    ui_popup_button_enter(&choose_box->base.base);

    choose_box->edit_index = choose_box->selected_index;
    choose_box->display_index = choose_box->selected_index;
    choose_box->is_animating = false;

    const char* text = ui_choose_box_get_text(choose_box, choose_box->display_index);
    uint16_t width = ui_text_width(text);
    int16_t content_y = ui_choose_box_get_content_y(&choose_box->base.window);
    int16_t center_x = ui_choose_box_get_center_x(&choose_box->base.window, width);

    init_ui_widget(&choose_box->option_curr);
    init_ui_widget(&choose_box->option_next);
    choose_box->option_curr.x = (float)center_x;
    choose_box->option_curr.y = (float)content_y;
    choose_box->option_curr.width = (float)width;
    choose_box->option_curr.height = (float)ui_font_height;

    ui_choose_box_set_widget_text(&choose_box->option_curr, center_x, content_y, width);

    ui_cursor.target_x = center_x - UI_H_MARGIN;
    ui_cursor.target_y = content_y - UI_V_MARGIN / 2;
    ui_cursor.target_width = width + UI_H_MARGIN * 2;
    ui_cursor.target_height = ui_font_height + UI_V_MARGIN;
    ui_cursor.should_move = true;
}

/**
 *@brief UIChooseBox成员函数：渲染选择框窗口内容
 *
 * @param self UIWindow对象指针（指向UIChooseBox.base.window）
 */
void ui_choose_box_window_render_items(UIWindow* self)
{
    UIChooseBox* choose_box = container_of(self, UIChooseBox, base.window);
    ui_popup_button_window_render_items(&choose_box->base.window);

    if (choose_box->option_curr.should_move)
    {
        choose_box->option_curr.step(&choose_box->option_curr);
    }
    if (choose_box->option_next.should_move)
    {
        choose_box->option_next.step(&choose_box->option_next);
    }

    const char* curr_text = ui_choose_box_get_text(choose_box, choose_box->display_index);
    oled_show_mix_string_area(
        (int16_t)(choose_box->base.window.base.x),
        (int16_t)(choose_box->base.window.base.y),
        (int16_t)(choose_box->base.window.base.width),
        (int16_t)(choose_box->base.window.base.height),
        (int16_t)(choose_box->option_curr.x),
        (int16_t)(choose_box->option_curr.y),
        curr_text,
        ui_chinese_size,
        ui_ascii_size
    );

    if (choose_box->is_animating)
    {
        const char* next_text = ui_choose_box_get_text(choose_box, choose_box->edit_index);
        oled_show_mix_string_area(
            (int16_t)(choose_box->base.window.base.x),
            (int16_t)(choose_box->base.window.base.y),
            (int16_t)(choose_box->base.window.base.width),
            (int16_t)(choose_box->base.window.base.height),
            (int16_t)(choose_box->option_next.x),
            (int16_t)(choose_box->option_next.y),
            next_text,
            ui_chinese_size,
            ui_ascii_size
        );

        if (!choose_box->option_curr.should_move && !choose_box->option_next.should_move)
        {
            choose_box->is_animating = false;
            choose_box->option_curr = choose_box->option_next;
            choose_box->display_index = choose_box->edit_index;
        }
    }
}

/**
 *@brief UIChooseBox成员函数：处理选择框窗口输入
 *
 * @param self UIWindow对象指针（指向UIChooseBox.base.window）
 */
void ui_choose_box_window_process_input(UIWindow* self)
{
    UIChooseBox* choose_box = container_of(self, UIChooseBox, base.window);

    if (ui_key_back->signal_event == KEY_PRESS)
    {
        choose_box->edit_index = choose_box->selected_index;
        ui_popup_button_window_process_input(&choose_box->base.window);
        return;
    }

    ui_popup_button_window_process_input(&choose_box->base.window);

    if (!choose_box->is_animating && choose_box->option_count > 0)
    {
        if (ui_key_left->signal_event == KEY_PRESS)
        {
            uint8_t next_index = (uint8_t)((choose_box->edit_index + choose_box->option_count - 1) % choose_box->option_count);
            ui_choose_box_start_slide(choose_box, -1, next_index);
            choose_box->edit_index = next_index;
        }
        else if (ui_key_right->signal_event == KEY_PRESS)
        {
            uint8_t next_index = (uint8_t)((choose_box->edit_index + 1) % choose_box->option_count);
            ui_choose_box_start_slide(choose_box, 1, next_index);
            choose_box->edit_index = next_index;
        }
    }

    if (ui_key_enter->signal_event == KEY_PRESS)
    {
        choose_box->selected_index = choose_box->edit_index;
        if (choose_box->on_value_changed != NULL)
        {
            choose_box->on_value_changed(choose_box);
        }
        choose_box->base.window.base.target_width = 0.f;
        choose_box->base.window.base.target_height = 0.f;
        choose_box->base.window.is_exiting = true;
        choose_box->base.window.base.should_move = true;
    }
}

/**
 *@brief UIWidget成员函数：初始化控件基类
 *
 * @param widget UIWidget对象指针
 */
void init_ui_widget(UIWidget* widget)
{
    widget->x = 0.f;
    widget->y = 0.f;
    widget->width = 0.f;
    widget->height = 0.f;
    widget->target_x = 0.f;
    widget->target_y = 0.f;
    widget->target_width = 0.f;
    widget->target_height = 0.f;
    widget->should_move = false;
    widget->enter = NULL;
    widget->render = NULL;
    widget->step = ui_widget_step;
}

/**
 *@brief UIWindow成员函数：初始化窗口对象
 *
 * @param window UIWindow对象指针
 * @param title 窗口标题
 */
void init_ui_window(UIWindow* window, const char* title)
{
    init_ui_widget(&window->base);
    window->base.x = (OLED_WIDTH - UI_WINDOW_WIDTH) / 2;
    window->base.y = (OLED_HEIGHT - ui_font_height - UI_WINDOW_HEIGHT) / 2 + ui_font_height; // 考虑标题高度，使窗口垂直居中
    window->base.target_x = window->base.x;
    window->base.target_y = window->base.y;
    window->base.render = ui_window_render_self;
    window->base.enter = NULL;
    window->title = title;
    window->is_exiting = false;
    window->suppress_input_once = false;
    window->render = ui_window_render_items;
    window->layout = NULL; // 需要自定义布局函数
    window->process_input = NULL; // 需要自定义输入处理函数
}

/**
 *@brief UIMenu成员函数：初始化菜单
 *
 * @param menu UIMenu对象指针
 * @param title 菜单标题
 * @param items 菜单项数组
 * @param item_count 菜单项数量
 */
void init_ui_menu(UIMenu* menu, const char* title, UIWidget** items, uint8_t item_count)
{
    init_ui_widget(&menu->base);
    menu->base.render = ui_menu_widget_render;
    menu->base.enter = ui_menu_enter;

    init_ui_widget(&menu->anchor);
    menu->anchor.x = 0.f;
    menu->anchor.y = 0.f;
    menu->anchor.target_x = 0.f;
    menu->anchor.target_y = 0.f;
    menu->anchor.should_move = false;

    menu->title = title;
    menu->items = items;
    menu->item_count = item_count;
    menu->selected_index = 0;
    menu->parent = NULL;
    menu->on_enter = NULL;
    menu->on_exit = NULL;
    menu->render = ui_menu_render_items;
    menu->layout = ui_menu_layout;
    menu->process_input = ui_menu_process_input;

    menu->layout(menu);

    if (item_count > 0 && items[menu->selected_index]->enter != NULL)
    {
        ui_menu_update_cursor(menu);
    }
    else if (find_available_selection(items, item_count, &menu->selected_index, true))
    {
        ui_menu_update_cursor(menu);
    }
}

/**
 *@brief UILabel成员函数：初始化标签
 *
 * @param label UILabel对象指针
 * @param text 标签文本
 */
void init_ui_label(UILabel* label, const char* text)
{
    init_ui_widget(&label->base);
    label->base.render = ui_label_render;

    label->text = text;
}

/**
 *@brief UILabel成员函数：设置标签文本
 *
 * @param label UILabel对象指针
 * @param text 新的标签文本
 */
void ui_label_set_text(UILabel* label, const char* text)
{
    if (label == NULL)
    {
        return;
    }

    label->text = text;
}

/**
 *@brief UICheckbox成员函数：初始化复选框
 *
 * @param checkbox UICheckbox对象指针
 * @param text 左侧显示文本
 * @param initial_checked 初始选中状态
 * @param on_value_changed 状态改变回调函数，参数为UICheckbox对象指针
 */
void init_ui_checkbox(UICheckbox* checkbox, const char* text, bool initial_checked)
{
    init_ui_widget(&checkbox->base);
    checkbox->base.render = ui_checkbox_render;
    checkbox->base.enter = ui_checkbox_enter;

    checkbox->text = text;
    checkbox->checked = initial_checked;
    checkbox->on_value_changed = NULL;
}

/**
 *@brief UIPopupButton成员函数：初始化弹窗按钮
 *
 * @param button UIPopupButton对象指针
 * @param text 按钮文本（也是弹窗标题）
 */
void init_ui_popup_button(UIPopupButton* button, const char* text)
{
    init_ui_widget(&button->base);
    init_ui_window(&button->window, text);
    button->window.render = ui_popup_button_window_render_items;
    button->window.process_input = ui_popup_button_window_process_input;

    button->base.render = ui_popup_button_render;
    button->base.enter = ui_popup_button_enter;
}

/**
 *@brief UIChooseBox成员函数：初始化选择框
 *
 * @param choose_box UIChooseBox对象指针
 * @param title 选择框标题
 * @param options 选项字符串数组
 * @param option_count 选项数量
 * @param initial_index 初始选中索引
 * @param on_value_changed 确认选择后的回调函数，参数为UIChooseBox对象指针
 */
void init_ui_choose_box(UIChooseBox* choose_box, const char* title, const char** options, uint8_t option_count, uint8_t initial_index)
{
    init_ui_popup_button(&choose_box->base, title);
    choose_box->base.base.enter = ui_choose_box_enter;
    choose_box->base.base.render = ui_choose_box_render;
    init_ui_window(&choose_box->base.window, title);
    choose_box->base.window.render = ui_choose_box_window_render_items;
    choose_box->base.window.process_input = ui_choose_box_window_process_input;

    choose_box->options = options;
    choose_box->option_count = option_count;
    choose_box->selected_index = (option_count > 0 && initial_index < option_count) ? initial_index : 0;
    choose_box->edit_index = choose_box->selected_index;
    choose_box->display_index = choose_box->selected_index;
    choose_box->is_animating = false;
    choose_box->slide_dir = 0;
    init_ui_widget(&choose_box->option_curr);
    init_ui_widget(&choose_box->option_next);
    choose_box->on_value_changed = NULL;
}

/**
 *@brief UIInputBoxDouble成员函数：初始化双精度输入框
 *
 * @param input_box UIInputBoxDouble对象指针
 * @param title 输入框标题
 * @param initial_value 初始值
 * @param suffix 后缀字符串数组
 * @param suffix_count 后缀数量
 * @param frac_length 小数位数
 * @param ignore_positive_sgn 是否忽略正号显示
 * @param on_value_changed 数值确认后的回调函数，参数为UIInputBoxDouble对象指针
 */
void init_ui_input_box_double(UIInputBoxDouble* input_box, const char* title, double initial_value, const double* coeffs, const char** suffix, uint8_t suffix_count, uint8_t frac_length, bool ignore_positive_sgn)
{
    init_ui_popup_button(&input_box->base, title);
    input_box->base.base.enter = ui_input_box_double_enter;
    input_box->base.base.render = ui_input_box_double_render;
    init_ui_window(&input_box->base.window, title);
    input_box->base.window.render = ui_input_box_double_window_render_items;
    input_box->base.window.process_input = ui_input_box_double_window_process_input;

    input_box->value = initial_value;
    input_box->edit_value = initial_value;
    input_box->frac_pos = 0;
    input_box->cursor_on_unit = false;
    input_box->coeffs = coeffs;
    input_box->suffix = suffix;
    input_box->suffix_count = suffix_count;
    input_box->selected_suffix_index = 0;
    input_box->edit_suffix_index = 0;
    input_box->display_suffix_index = 0;
    input_box->unit_animating = false;
    input_box->unit_slide_dir = 0;
    init_ui_widget(&input_box->unit_curr);
    init_ui_widget(&input_box->unit_next);
    input_box->frac_length = frac_length;
    input_box->ignore_positive_sgn = ignore_positive_sgn;
    input_box->state = UI_INPUT_BOX_IDLE;
    input_box->on_value_changed = NULL;
}

/**
 *@brief 更新UI显示，应该在主循环中定期调用以刷新UI界面
 *
 */
void ui_update()
{
    ui_update_delta = sys_tick - ui_last_update_tick;
    ui_last_update_tick = sys_tick;
    oled_clear();
    if (ui_active_menu == NULL)
    {
        ui_active_menu = ui_main_menu;
    }

    if (ui_main_menu != NULL && ui_main_menu->render != NULL)
    {
        ui_main_menu->render(ui_main_menu);
    }

    if (ui_menu_transition_active && ui_transition_menu != NULL && ui_active_menu != NULL)
    {
        if (!ui_transition_menu->anchor.should_move && !ui_active_menu->anchor.should_move)
        {
            ui_menu_transition_active = false;
            ui_transition_menu = NULL;
            ui_menu_update_cursor(ui_active_menu);
        }
    }
    oled_refresh();
}

/**
 *@brief 更改UI字体大小，调用后会立即生效并更新界面布局
 *
 * @param ascii_size 新的ASCII字符字体大小，单位为像素宽度，必须是OLEDFontSizeHalf枚举值之一
 * @param chinese_size 新的中文字符字体大小，单位为像素宽度，必须是OLEDFontSizeFull枚举值之一
 */
void ui_change_font_size(OLEDFontSizeHalf ascii_size, OLEDFontSizeFull chinese_size)
{
    ui_ascii_size = ascii_size;
    ui_chinese_size = chinese_size;
    uint8_t ascii_height, chinese_height;

    switch (ascii_size)
    {
    case OLED_6X8_HALF:
        ascii_height = 8;
        break;
    case OLED_7X12_HALF:
        ascii_height = 12;
        break;
    case OLED_8X16_HALF:
        ascii_height = 16;
        break;
    case OLED_10X20_HALF:
        ascii_height = 20;
        break;
    default:
        ascii_height = 8;
        break;
    }

    switch (chinese_size)
    {
    case OLED_8X8_FULL:
        chinese_height = 8;
        break;
    case OLED_12X12_FULL:
        chinese_height = 12;
        break;
    case OLED_16X16_FULL:
        chinese_height = 16;
        break;
    case OLED_20X20_FULL:
        chinese_height = 20;
        break;
    default:
        chinese_height = 8;
        break;
    }

    ui_font_height = (ascii_height > chinese_height) ? ascii_height : chinese_height;
}
