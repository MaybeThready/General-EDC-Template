#pragma once

#include "../ti_msp_dl_config.h"

#define OLED_CMD 0
#define OLED_DATA 1
#define OLED_MODE 0

#define OLED_SIZE 12
#define X_LEVEL_L 0x02
#define X_LEVEL_H 0x10
#define MAX_COLUMN 128
#define MAX_ROW 64
#define BRIGHTNESS 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64

#if (OLED_SIZE == 16)
#define YOFFSET 1
#else
#define YOFFSET 0
#endif

#define OLED_SPI_INST SPI_0_INST

extern uint8_t oled_gram[130][8];

/**
 *@brief 初始化OLED显示屏，设置显示参数并清屏
 * 
 */
void init_oled();

/**
 * @brief 向OLED写入一个字节
 *
 * @param data 要写入的数据
 * @param cmd 命令或数据标志，非零表示数据，零表示命令
 */
void oled_write_byte(uint8_t data, uint8_t cmd);

/**
 *@brief 打开OLED显示
 * 
 */
void oled_display_on();

/**
 *@brief 关闭OLED显示
 * 
 */
void oled_display_off();

/**
 *@brief 清屏函数，将OLED显示内容清空
 * 
 */
void oled_clear();

/**
 *@brief 在指定坐标绘制一个点
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~7
 * @param t 点的状态，非零表示点亮，零表示熄灭
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t t);

/**
 *@brief 填充函数。不建议使用该函数
 * 
 * @param x1 起始横坐标，范围0~127
 * @param y1 起始纵坐标，范围0~7
 * @param x2 结束横坐标，范围0~127
 * @param y2 结束纵坐标，范围0~7
 * @param dot 填充状态，非零表示填充，零表示清除
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);

/**
 *@brief 显示一个字符
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~7
 * @param chr 要显示的字符
 */
void oled_show_char(uint8_t x, uint8_t y, char chr);

/**
 *@brief 显示一个数字
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~7
 * @param num 要显示的数字
 * @param len 数字的长度
 * @param size2 字符大小
 */
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2);

/**
 *@brief 显示一个有符号数字
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~7
 * @param num 要显示的有符号数字
 * @param len 数字的长度
 * @param size2 字符大小
 */
void oled_show_signed_num(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size2);

/**
 *@brief 显示一个字符串
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~7
 * @param p 要显示的字符串指针
 */
void oled_show_string(uint8_t x, uint8_t y, char* p);

/**
 *@brief 设置显示位置
 * 
 * @param x 横坐标，范围0~127
 * @param y 纵坐标，范围0~7
 */
void oled_set_pos(uint8_t x, uint8_t y);

/**
 *@brief 显示一个中文字符
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~7
 * @param no 中文字符编号
 */
void oled_show_chinese(uint8_t x, uint8_t y, uint8_t no);

/**
 *@brief 绘制一个位图
 * 
 * @param x0 起始横坐标，范围0~127
 * @param y0 起始纵坐标，范围0~7
 * @param x1 结束横坐标，范围0~127
 * @param y1 结束纵坐标，范围0~7
 * @param BMP 位图数据指针
 */
void oled_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);

/**
 *@brief OLED颜色反转
 * 
 * @param i 颜色反转开关，0表示关闭，1表示开启
 */
void oled_color_turn(uint8_t i);

/**
 *@brief OLED显示翻转
 * 
 * @param i 显示翻转开关，0表示正常显示，1表示翻转显示
 */
void oled_display_turn(uint8_t i);

/**
 *@brief 刷新OLED显示，将GRAM内容更新到屏幕上
 * 
 */
void oled_refresh();

/**
 *@brief 画点函数
 * 
 * @param x 显示的横坐标，范围0~127
 * @param y 显示的纵坐标，范围0~63
 * @param t
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t t);

/**
 *@brief 画线函数
 * 
 * @param x1 起始横坐标，范围0~127
 * @param y1 起始纵坐标，范围0~7
 * @param x2 结束横坐标，范围0~127
 * @param y2 结束纵坐标，范围0~7
 * @param mode
 */
void oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);

/**
 *@brief 画圆函数
 * 
 * @param x 圆心横坐标，范围0~127
 * @param y 圆心纵坐标，范围0~7
 * @param r 圆的半径
 */
void oled_draw_circle(uint8_t x, uint8_t y, uint8_t r);

extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];
extern const unsigned char Hzk[][32];
