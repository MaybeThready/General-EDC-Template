#pragma once

#include "../ti_msp_dl_config.h"

// LED数量
#define NUM_LEDS 8

// LED状态位码
#define LED_1 0x01
#define LED_2 0x02
#define LED_3 0x04
#define LED_4 0x08
#define LED_5 0x10
#define LED_6 0x20
#define LED_7 0x40
#define LED_8 0x80
#define LED_ALL 0xFF

// LED端口定义
#define LED_PORTS_VALUES { LED_L1_PORT, LED_L2_PORT, LED_L3_PORT, LED_L4_PORT, LED_L5_PORT, LED_L6_PORT, LED_L7_PORT, LED_L8_PORT }

// LED引脚定义
#define LED_PINS_VALUES { LED_L1_PIN, LED_L2_PIN, LED_L3_PIN, LED_L4_PIN, LED_L5_PIN, LED_L6_PIN, LED_L7_PIN, LED_L8_PIN }

extern GPIO_Regs* LED_PORTS[NUM_LEDS];
extern const uint32_t LED_PINS[NUM_LEDS];

extern uint8_t led_states;

/**
 *@brief 设置LED状态
 * 
 * @param leds LED位码，可以是LED_1、LED_2、...、LED_8的组合
 * @param state LED状态，true表示点亮，false表示熄灭
 */
void set_led_state(uint8_t leds, bool state);

/**
 *@brief 切换LED状态
 * 
 * @param leds LED位码，可以是LED_1、LED_2、...、LED_8的组合
 */
void toggle_led(uint8_t leds);
