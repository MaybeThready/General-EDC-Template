#include "ui.h"
#include <string.h>

UIMenu* ui_main_menu = NULL;
UIWindow* ui_current_window = NULL;
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
bool find_available_selection(UIWidget** items, uint8_t item_count, int16_t* selected_index, bool forward)
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

uint8_t get_digit_input()
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

/**
 *@brief UIMenu成员函数：根据当前选中项更新光标目标区域
 *
 * @param menu UIMenu对象指针
 */
void ui_menu_update_cursor(UIMenu* menu)
{
    UIWidget* selected_item = menu->items[menu->selected_index];
    ui_cursor.target_x = 0;
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
    if (menu->layout != NULL)
    {
        menu->layout(menu);
    }

    if (menu->process_input != NULL && ui_current_window == NULL) // 只有在没有弹窗时才处理菜单输入
    {
        menu->process_input(menu);
    }

    oled_draw_rectangle(0, 0, (int16_t)(ui_ascii_size / 2), ui_font_height, true);
    oled_show_mix_string(
        ui_ascii_size,
        0,
        menu->title,
        ui_chinese_size,
        ui_ascii_size
    );

    UIWidget* item;
    for (uint8_t i = 0; i < menu->item_count; i++)
    {
        item = menu->items[i];
        if (item->should_move)
        {
            item->step(item);
        }

        if (item->render != NULL)
        {
            item->render(item);
        }
    }

    if (ui_current_window != NULL)
    {
        ui_current_window->render(ui_current_window);
        if (ui_current_window->is_exiting)
        {
            ui_menu_update_cursor(menu); // 更新菜单光标位置，确保光标在正确的选项上显示

            if (!ui_current_window->base.should_move)  // 只有在窗口完全停止移动后才销毁窗口并返回菜单
            {
                ui_current_window->is_exiting = false;
                ui_current_window = NULL;
            }
        }
    }

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
    int16_t y = ui_font_height + UI_V_MARGIN; // 从标题下方开始布局
    for (uint8_t i = 0; i < menu->item_count; i++)
    {
        UIWidget* item = menu->items[i];
        item->target_x = UI_H_MARGIN;
        item->target_y = y;
        item->target_width = OLED_WIDTH - 2 * UI_H_MARGIN;
        item->target_height = ui_font_height;
        item->should_move = true;
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

void ui_input_box_double_render(UIWidget* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base);
    ui_popup_button_render(&input_box->base.base);

    double display_value = input_box->value * input_box->coeff;
    double abs_value = fabs(display_value);
    uint8_t int_length = (display_value < 0 ? 1 : 0) + (abs_value < 1.0 ? 1 : (int)log10(abs_value) + 1);
    uint8_t total_length = int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn; // 数值的总长度（整数部分长度+小数部分长度+小数点+正负号）
    uint8_t suffix_length = 0;

    if (input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        suffix_length = strlen(input_box->suffix[input_box->selected_suffix_index]);
        oled_show_mix_string_area(
            input_box->base.base.x,
            input_box->base.base.y,
            input_box->base.base.width,
            input_box->base.base.height,
            OLED_WIDTH - UI_H_MARGIN - suffix_length * ui_ascii_size,
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
        OLED_WIDTH - UI_H_MARGIN - 1 - total_length * ui_ascii_size - suffix_length * ui_ascii_size - ui_ascii_size, // 数值右对齐，留出UI_H_MARGIN的右边距
        input_box->base.base.y,
        display_value,
        int_length,
        input_box->frac_length,
        input_box->ignore_positive_sgn,
        ui_ascii_size
    );
}

void ui_input_box_double_update_cursor(UIInputBoxDouble* input_box)
{
    double display_value = input_box->edit_value * input_box->coeff;
    double abs_value = fabs(display_value);
    uint8_t int_length = (display_value < 0 ? 1 : 0) + (abs_value < 1.0 ? 1 : (int)log10(abs_value) + 1);

    ui_cursor.target_x = input_box->base.window.base.x + UI_H_MARGIN + 1;
    ui_cursor.target_y = input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN / 2;
    ui_cursor.target_width = (int16_t)((int_length + input_box->frac_length + 1 + !input_box->ignore_positive_sgn) * ui_ascii_size) + UI_H_MARGIN; // 光标宽度略大于数值显示区域，留出一些余量
    ui_cursor.target_height = (int16_t)ui_font_height + UI_V_MARGIN;
    ui_cursor.should_move = true;
}

void ui_input_box_double_enter(UIWidget* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base);
    ui_popup_button_enter(&input_box->base.base);

    input_box->edit_value = input_box->value;
    input_box->frac_pos = 0;
    ui_input_box_double_update_cursor(input_box);
    input_box->state = UI_INPUT_BOX_IDLE; // 进入编辑整数部分状态
}

void ui_input_box_double_window_render_items(UIWindow* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base.window);

    ui_popup_button_window_render_items(&input_box->base.window);
    double display_value = input_box->edit_value * input_box->coeff;
    double abs_value = fabs(display_value);
    uint8_t int_length = (display_value < 0 ? 1 : 0) + (abs_value < 1.0 ? 1 : (int)log10(abs_value) + 1);

    oled_show_float_num_area(
        input_box->base.window.base.x,
        input_box->base.window.base.y,
        input_box->base.window.base.width,
        input_box->base.window.base.height,
        (int16_t)(input_box->base.window.base.x + UI_H_MARGIN + 1),
        (int16_t)(input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN), // 数值行在标题下方，间隔UI_V_MARGIN
        display_value,
        int_length,
        input_box->frac_length,
        input_box->ignore_positive_sgn,
        ui_ascii_size
    );

    if (input_box->suffix != NULL && input_box->suffix_count > 0)
    {
        oled_show_mix_string_area(
            input_box->base.window.base.x,
            input_box->base.window.base.y,
            input_box->base.window.base.width,
            input_box->base.window.base.height,
            (int16_t)(input_box->base.window.base.x + UI_H_MARGIN + 1 + (int_length + input_box->frac_length + 2 + !input_box->ignore_positive_sgn) * ui_ascii_size), // 后缀紧跟在数值后面
            (int16_t)(input_box->base.window.base.y + UI_H_MARGIN + 1 + ui_font_height + UI_V_MARGIN), // 数值行在标题下方，间隔UI_V_MARGIN
            input_box->suffix[input_box->selected_suffix_index],
            ui_chinese_size,
            ui_ascii_size
        );
    }
}

void ui_input_box_double_window_process_input(UIWindow* self)
{
    UIInputBoxDouble* input_box = container_of(self, UIInputBoxDouble, base.window);
    if (ui_key_back->signal_event == KEY_PRESS)
    {
        input_box->edit_value = input_box->value;
        input_box->frac_pos = 0;
        input_box->state = UI_INPUT_BOX_IDLE;
        ui_popup_button_window_process_input(&input_box->base.window);
        return;
    }

    ui_popup_button_window_process_input(&input_box->base.window);

    uint8_t digit = get_digit_input();
    switch (input_box->state)
    {
    case UI_INPUT_BOX_IDLE:
        if (digit != UI_NO_DIGIT_INPUT)
        {
            input_box->edit_value = (double)((digit == UI_DIGIT_INPUT_POINT) ? 0.0 : digit) / input_box->coeff; // 将输入的数字转换为实际值
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
            ui_input_box_double_update_cursor(input_box);
        }
        break;

    case UI_INPUT_BOX_EDITING_INT:
        if (digit != UI_NO_DIGIT_INPUT && digit != UI_DIGIT_INPUT_POINT)
        {
            input_box->edit_value = input_box->edit_value * 10 + (double)digit / input_box->coeff; // 将输入的数字转换为实际值并添加到整数部分
            ui_input_box_double_update_cursor(input_box);
        }
        else if (digit == UI_DIGIT_INPUT_POINT)
        {
            input_box->frac_pos = 0;
            input_box->state = UI_INPUT_BOX_EDITING_FRAC;
            ui_input_box_double_update_cursor(input_box);
        }
        break;

    case UI_INPUT_BOX_EDITING_FRAC:
        if (digit != UI_NO_DIGIT_INPUT && digit != UI_DIGIT_INPUT_POINT)
        {
            if (input_box->frac_pos < input_box->frac_length)
            {
                input_box->frac_pos++;
                input_box->edit_value += (double)digit / input_box->coeff * pow(10.0, -(double)input_box->frac_pos);
                ui_input_box_double_update_cursor(input_box);
            }
        }
        break;
    }

    if (ui_key_enter->signal_event == KEY_PRESS)
    {
        input_box->value = input_box->edit_value;
        input_box->frac_pos = 0;
        if (input_box->on_value_changed != NULL)
        {
            input_box->on_value_changed(input_box->value);
        }
        input_box->state = UI_INPUT_BOX_IDLE;
        input_box->base.window.base.target_width = 0.f;
        input_box->base.window.base.target_height = 0.f;
        input_box->base.window.is_exiting = true;
        input_box->base.window.base.should_move = true;
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
    menu->base.render = ui_menu_render_self;

    menu->title = title;
    menu->items = items;
    menu->item_count = item_count;
    menu->selected_index = 0;
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

void init_ui_input_box_double(UIInputBoxDouble* input_box, const char* title, double initial_value, const char** suffix, uint8_t suffix_count, uint8_t frac_length, bool ignore_positive_sgn, DoubleChangeCallbackFunc on_value_changed)
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
    input_box->suffix = suffix;
    input_box->suffix_count = suffix_count;
    input_box->coeff = 1.0;
    input_box->selected_suffix_index = 0;
    input_box->frac_length = frac_length;
    input_box->ignore_positive_sgn = ignore_positive_sgn;
    input_box->state = UI_INPUT_BOX_IDLE;
    input_box->on_value_changed = on_value_changed;
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
    if (ui_main_menu != NULL && ui_main_menu->render != NULL)
    {
        ui_main_menu->render(ui_main_menu);
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
