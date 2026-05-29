#include "keyboard.h"
#include "msp_sys.h"

const uint32_t KEYBOARD_H_PINS[KEYBOARD_NUM_H] = KEYBOARD_H_PINS_VALUES;

const uint32_t KEYBOARD_V_PINS[KEYBOARD_NUM_V] = KEYBOARD_V_PINS_VALUES;

const uint32_t KEYBOARD_DEBOUNCE_DELAY = TIME_MS(20);

const char KEYBOARD_KEY_MAP[KEYBOARD_NUM_H][KEYBOARD_NUM_V] = KEYBOARD_KEY_MAP_VALUES;

Key keyboard_keys[KEYBOARD_NUM_H][KEYBOARD_NUM_V];
Key keyboard_null_key = { .state_event = KEY_OFF, .signal_event = KEY_IDLE, .debounce_target = KEY_DEBOUNCE_PRESS, .debounce_tick = 0, .key_code = '\0', .is_debouncing = false };

void init_keyboard()
{
    for (int i = 0; i < KEYBOARD_NUM_H; i++)
    {
        for (int j = 0; j < KEYBOARD_NUM_V; j++)
        {
            keyboard_keys[i][j].key_code = KEYBOARD_KEY_MAP[i][j];
            keyboard_keys[i][j].state_event = KEY_OFF;
            keyboard_keys[i][j].signal_event = KEY_IDLE;
            keyboard_keys[i][j].is_debouncing = false;
            keyboard_keys[i][j].debounce_target = KEY_DEBOUNCE_PRESS;
            keyboard_keys[i][j].debounce_tick = 0;
        }
    }
}

void keyboard_update()
{
    DL_GPIO_setPins(KEYBOARD_PORT, KEYBOARD_H_PINS_MASK);

    for (int i = 0; i < KEYBOARD_NUM_H; i++)
    {
        DL_GPIO_clearPins(KEYBOARD_PORT, KEYBOARD_H_PINS[i]);
        for (int j = 0; j < KEYBOARD_NUM_V; j++)
        {
            if (DL_GPIO_readPins(KEYBOARD_PORT, KEYBOARD_V_PINS[j]) == 0)  // 如果按键处于按下的状态
            {
                if (keyboard_keys[i][j].is_debouncing)  // 如果当前按键正在消抖
                {
                    if (keyboard_keys[i][j].debounce_target == KEY_DEBOUNCE_PRESS)
                    {
                        if ((sys_tick - keyboard_keys[i][j].debounce_tick) * SYS_TICK_PERIOD >= KEYBOARD_DEBOUNCE_DELAY)
                        {
                            // 消抖完成，确认按键事件
                            keyboard_keys[i][j].state_event = KEY_ON;
                            keyboard_keys[i][j].signal_event = KEY_PRESS;
                            keyboard_keys[i][j].is_debouncing = false;  // 重置消抖状态
                        }
                    }
                    else
                    {
                        // 消抖目标为松开，但读到了按下，取消消抖
                        keyboard_keys[i][j].is_debouncing = false;
                    }
                }
                else  // 如果当前按键没有在消抖
                {
                    if (keyboard_keys[i][j].state_event == KEY_OFF)  // 如果按键之前是未按下状态
                    {
                        // 开始消抖
                        keyboard_keys[i][j].is_debouncing = true;
                        keyboard_keys[i][j].debounce_target = KEY_DEBOUNCE_PRESS;
                        keyboard_keys[i][j].debounce_tick = sys_tick;
                    }
                    else
                    {
                        // 按键持续被按下
                        keyboard_keys[i][j].state_event = KEY_ON;
                        keyboard_keys[i][j].signal_event = KEY_IDLE;
                    }
                }
            }
            else // 如果按键处于未按下的状态
            {
                if (keyboard_keys[i][j].is_debouncing)  // 如果当前按键正在消抖
                {
                    if (keyboard_keys[i][j].debounce_target == KEY_DEBOUNCE_RELEASE)
                    {
                        if ((sys_tick - keyboard_keys[i][j].debounce_tick) * SYS_TICK_PERIOD >= KEYBOARD_DEBOUNCE_DELAY)
                        {
                            // 消抖完成，确认松开事件
                            keyboard_keys[i][j].state_event = KEY_OFF;
                            keyboard_keys[i][j].signal_event = KEY_RELEASE;
                            keyboard_keys[i][j].is_debouncing = false;  // 重置消抖状态
                        }
                    }
                    else
                    {
                        // 消抖目标为按下，但读到了松开，取消消抖
                        keyboard_keys[i][j].is_debouncing = false;
                    }
                }
                else
                {
                    if (keyboard_keys[i][j].state_event == KEY_ON)  // 如果按键之前是按下状态
                    {
                        // 开始消抖
                        keyboard_keys[i][j].is_debouncing = true;
                        keyboard_keys[i][j].debounce_target = KEY_DEBOUNCE_RELEASE;
                        keyboard_keys[i][j].debounce_tick = sys_tick;
                    }
                    else
                    {
                        // 按键持续未被按下
                        keyboard_keys[i][j].state_event = KEY_OFF;
                        keyboard_keys[i][j].signal_event = KEY_IDLE;
                    }
                }
            }
        }
        DL_GPIO_setPins(KEYBOARD_PORT, KEYBOARD_H_PINS[i]);
    }
}
