/**
 * @file oled_driver.h
 * @author Thready
 * @brief OLED硬件驱动模块
 * @version 0.1
 * @date 2026-05-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "../../ti_msp_dl_config.h"

// 定义引脚
#define OLED_SPI_INST (SPI_0_INST)
#define oled_set_res() DL_GPIO_setPins(OLED_RES_PORT, OLED_RES_PIN)
#define oled_clear_res() DL_GPIO_clearPins(OLED_RES_PORT, OLED_RES_PIN)
#define oled_set_dc() DL_GPIO_setPins(OLED_DC_PORT, OLED_DC_PIN)
#define oled_clear_dc() DL_GPIO_clearPins(OLED_DC_PORT, OLED_DC_PIN)
#define oled_set_cs() DL_GPIO_setPins(OLED_CS_PORT, OLED_CS_PIN)
#define oled_clear_cs() DL_GPIO_clearPins(OLED_CS_PORT, OLED_CS_PIN)

#define OLED_CMD (0)
#define OLED_DATA (1)

#define OLED_WIDTH (128)
#define OLED_HEIGHT (64)

extern uint8_t oled_display_buffer[OLED_HEIGHT / 8][OLED_WIDTH];  // 显存

/**
 *@brief 初始化OLED硬件
 * 
 */
void init_oled();

/**
 *@brief 刷新OLED
 * 
 */
void oled_refresh();

/**
 *@brief 仅刷新area所在的Page
 * 
 * @param x Area左上角的横坐标，范围0~127
 * @param y Area左上角的纵坐标，范围0~63
 * @param width Area的宽度，范围0~128
 * @param height Area的高度，范围0~64
 */
void oled_refresh_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
