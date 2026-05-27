/**
 *@file oled_fonts.h
 * @author Thready
 * @brief OLED预制点阵
 * @version 0.1
 * @date 2026-05-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <stdint.h>
#define OLED_CHN_CHAR_WIDTH (2)  // OLED中文字符宽度，单位为字节

typedef struct 
{
	char index[OLED_CHN_CHAR_WIDTH + 1];  // 中文字符索引
	uint8_t data[60];                    // 字模数据
} ChineseCell20x20;

typedef struct
{
	char index[OLED_CHN_CHAR_WIDTH + 1];
	uint8_t data[32];
} ChineseCell16x16;

typedef struct
{
    char index[OLED_CHN_CHAR_WIDTH + 1];
    uint8_t data[24];
} ChineseCell12x12;

typedef struct 
{
    char index[OLED_CHN_CHAR_WIDTH + 1];
    uint8_t data[8];
} ChineseCell8x8;

// ASCII字符点阵
extern const uint8_t OLED_F10x20[][30];
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F7x12[][14];
extern const uint8_t OLED_F6x8[][6];

// 汉字点阵
extern const ChineseCell20x20 OLED_CF20x20[];
extern const ChineseCell16x16 OLED_CF16x16[];
extern const ChineseCell12x12 OLED_CF12x12[];
extern const ChineseCell8x8 OLED_CF8x8[];

// 其他图标点阵

