/*
 * 键盘模块
 * 使用时在主循环的每一帧调用keyboard_update函数来更新键盘状态并检测按键事件。按键事件可以通过访问keyboard_keys数组来获取，每个按键对象包含了当前的按键状态事件和瞬时信号事件。
 * 键盘按键状态事件有两种：KEY_OFF表示按键未按下状态，KEY_ON表示按键按下状态。键盘按键瞬时信号事件有三种：KEY_IDLE表示按键无事件，KEY_PRESS表示按键瞬时被按下事件，KEY_RELEASE表示按键瞬时被松开事件。
 */

#pragma once

#include "../ti_msp_dl_config.h"

// 按键的行列数目
#define KEYBOARD_NUM_H (4)
#define KEYBOARD_NUM_V (4)

// 键盘水平行的GPIO引脚
#define KEYBOARD_H_PINS_VALUES { KEYBOARD_H1_PIN, KEYBOARD_H2_PIN, KEYBOARD_H3_PIN, KEYBOARD_H4_PIN }

// 键盘垂直列的GPIO引脚
#define KEYBOARD_V_PINS_VALUES { KEYBOARD_V1_PIN, KEYBOARD_V2_PIN, KEYBOARD_V3_PIN, KEYBOARD_V4_PIN }

// 键盘行列掩码
#define KEYBOARD_H_PINS_MASK (KEYBOARD_H_PINS[0] | KEYBOARD_H_PINS[1] | KEYBOARD_H_PINS[2] | KEYBOARD_H_PINS[3])
#define KEYBOARD_V_PINS_MASK (KEYBOARD_V_PINS[0] | KEYBOARD_V_PINS[1] | KEYBOARD_V_PINS[2] | KEYBOARD_V_PINS[3])

// 键盘按键映射
#define KEYBOARD_KEY_MAP_VALUES { \
    {'1', '2', '3', 'A'}, \
    {'4', '5', '6', 'B'}, \
    {'7', '8', '9', 'C'}, \
    {'*', '0', '#', 'D'}  \
}

// 按键状态事件枚举
typedef enum
{
    KEY_OFF,  // 按键未按下状态
    KEY_ON    // 按键按下状态
} KeyStateEvent;

// 按键瞬时信号事件枚举
typedef enum
{
    KEY_IDLE,      // 按键无事件
    KEY_PRESS,     // 按键瞬时被按下事件
    KEY_RELEASE    // 按键瞬时被松开事件
} KeySignalEvent;  

// 按键对象结构体
typedef struct
{
    char key_code;                // 键码
    KeyStateEvent state_event;    // 按键状态事件
    KeySignalEvent signal_event;  // 按键瞬时信号事件
    bool is_debouncing;           // 是否正在消抖
    uint32_t debounce_tick;       // 去抖动开始的系统时钟节拍数
} Key;

extern const uint32_t KEYBOARD_H_PINS[KEYBOARD_NUM_H];                 // 键盘水平行的GPIO引脚
extern const uint32_t KEYBOARD_V_PINS[KEYBOARD_NUM_V];                 // 键盘垂直列的GPIO引脚
extern const uint32_t KEYBOARD_DEBOUNCE_DELAY;                         // 键盘去抖动延迟时长
extern const char KEYBOARD_KEY_MAP[KEYBOARD_NUM_H][KEYBOARD_NUM_V];    // 键盘按键映射表

extern Key keyboard_keys[KEYBOARD_NUM_H][KEYBOARD_NUM_V];  // 键盘按键数组

/**
 *@brief 初始化键盘状态，设置按键映射表和初始事件状态
 * 
 */
void init_keyboard();   // 初始化键盘

/**
 *@brief 更新键盘状态，检测按键事件
 * 
 */
void keyboard_update();    // 更新键盘状态，检测按键事件
