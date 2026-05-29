/**
 *@file oled.h
 * @author Thready
 * @brief OLED软件模块。注意所有屏幕操作函数只对显存进行操作，需要手动refresh才能更新到屏幕上
 * @version 0.1
 * @date 2026-05-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "oled_driver.h"
#include "oled_fonts.h"

typedef enum
{
    OLED_10X20_HALF = 10,	// 半角字符，宽10像素，高20像素
    OLED_8X16_HALF = 8,		// 半角字符，宽8像素，高16像素
    OLED_7X12_HALF = 7,		// 半角字符，宽7像素，高12像素
    OLED_6X8_HALF = 6,		// 半角字符，宽6像素，高8像素
} OLEDFontSizeHalf;

typedef enum
{
    OLED_8X8_FULL = 8,		// 全角字符，宽8像素，高8像素
    OLED_12X12_FULL = 12,	// 全角字符，宽12像素，高12像素
    OLED_16X16_FULL = 16,	// 全角字符，宽16像素，高16像素
    OLED_20X20_FULL = 20,	// 全角字符，宽20像素，高20像素
} OLEDFontSizeFull;

/**
 * @brief 清空屏幕
 *
 */
void oled_clear();

/**
 *@brief 清空指定区域
 * 
 * @param x 区域左上角x坐标
 * @param y 区域左上角y坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void oled_clear_area(int16_t x, int16_t y, int16_t width, int16_t height);

/**
 *@brief 屏幕反色
 * 
 */
void oled_reverse();

/**
 *@brief 指定区域反色
 * 
 * @param x 区域左上角x坐标
 * @param y 区域左上角y坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void oled_reverse_area(int16_t x, int16_t y, int16_t width, int16_t height);

/**
 *@brief 显示图像
 * 
 * @param x 图像左上角x坐标
 * @param y 图像左上角y坐标
 * @param width 图像宽度
 * @param height 图像高度
 * @param image 图像数据指针
 */
void oled_show_image(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t* image);

/**
 *@brief 显示ASCII字符
 * 
 * @param x 字符左上角x坐标
 * @param y 字符左上角y坐标
 * @param chr 要显示的ASCII字符
 * @param size 字符大小，只能为半角字符大小
 */
void oled_show_char(int16_t x, int16_t y, char chr, OLEDFontSizeHalf size);

/**
 *@brief 显示无符号整数
 * 
 * @param x 数字左上角x坐标
 * @param y 数字左上角y坐标
 * @param num 要显示的无符号整数
 * @param length 数字的长度，即显示多少位
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_num(int16_t x, int16_t y, uint32_t num, uint8_t length, OLEDFontSizeHalf size);

/**
 *@brief 显示有符号整数
 * 
 * @param x 数字左上角x坐标
 * @param y 数字左上角y坐标
 * @param num 要显示的有符号整数
 * @param length 数字的长度，即显示多少位（不包括符号位）
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_signed_num(int16_t x, int16_t y, int32_t num, uint8_t length, OLEDFontSizeHalf size);

/**
 *@brief 显示十六进制数
 * 
 * @param x 数字左上角x坐标
 * @param y 数字左上角y坐标
 * @param num 要显示的数字
 * @param length 数字的长度，即显示多少位
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_hex_num(int16_t x, int16_t y, int32_t num, uint8_t length, OLEDFontSizeHalf size);

/**
 *@brief 显示二进制数
 * 
 * @param x 数字左上角x坐标
 * @param y 数字左上角y坐标
 * @param num 要显示的数字
 * @param length 数字的长度，即显示多少位
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_bin_num(int16_t x, int16_t y, int32_t num, uint8_t length, OLEDFontSizeHalf size);

/**
 *@brief 显示浮点数
 * 
 * @param x 数字左上角x坐标
 * @param y 数字左上角y坐标
 * @param num 要显示的浮点数
 * @param int_length 整数部分的长度
 * @param frac_length 小数部分的长度
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_float_num(int16_t x, int16_t y, double num, uint8_t int_length, uint8_t frac_length, OLEDFontSizeHalf size);

/**
 *@brief 显示字符串
 * 
 * @param x 字符串左上角x坐标
 * @param y 字符串左上角y坐标
 * @param str 要显示的字符串
 * @param size 字符大小，只能为半角字符大小
 */
void oled_show_string(int16_t x, int16_t y, const char* str, OLEDFontSizeHalf size);

/**
 *@brief 显示混合字符串（中英文）
 * 
 * @param x 字符串左上角x坐标
 * @param y 字符串左上角y坐标
 * @param str 要显示的字符串
 * @param chinese_size 中文字符大小，只能为全角字符大小
 * @param ascii_size ASCII字符大小，只能为半角字符大小
 */
void oled_show_mix_string(int16_t x, int16_t y, const char* str, OLEDFontSizeFull chinese_size, OLEDFontSizeHalf ascii_size);

/**
 *@brief 显示中文字符串
 * 
 * @param x 字符串左上角x坐标
 * @param y 字符串左上角y坐标
 * @param str 要显示的中文字符串
 * @param size 字符大小，只能为全角字符大小
 */
void oled_show_chinese(int16_t x, int16_t y, const char* str, OLEDFontSizeFull size);

/**
 *@brief 在指定区域内显示图像
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param width 显示内容的宽度
 * @param height 显示内容的高度
 * @param image 显示内容的图像数据指针
 */
void oled_show_image_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t* image);

/**
 *@brief 在指定区域内显示ASCII字符
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param chr 要显示的ASCII字符
 * @param size 字符大小，只能为半角字符大小
 */
void oled_show_char_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, char chr, OLEDFontSizeHalf size);

/**
 *@brief 在指定区域内显示字符串
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param string 要显示的字符串
 * @param size 字符大小，只能为半角字符大小
 */
void oled_show_string_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, const char* string, OLEDFontSizeHalf size);

/**
 *@brief 在指定区域内显示中文字符串
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param chinese 要显示的中文字符串
 * @param size 字符大小，只能为全角字符大小
 */
void oled_show_chinese_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, const char* chinese, OLEDFontSizeFull size);

/**
 *@brief 在指定区域内显示混合字符串（中英文）
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param string 要显示的字符串
 * @param chinese_size 中文字符大小，只能为全角字符大小
 * @param ascii_size ASCII字符大小，只能为半角字符大小
 */
void oled_show_mix_string_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, const char* string, OLEDFontSizeFull chinese_size, OLEDFontSizeHalf ascii_size);

/**
 *@brief 在指定区域内显示无符号整数
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param num 要显示的无符号整数
 * @param length 数字的长度，即显示多少位
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_num_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, uint32_t num, uint8_t length, OLEDFontSizeHalf size);

/**
 *@brief 在指定区域内显示浮点数
 * 
 * @param area_x 区域左上角x坐标
 * @param area_y 区域左上角y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param x 显示内容的左上角x坐标
 * @param y 显示内容的左上角y坐标
 * @param num 要显示的浮点数
 * @param int_length 整数部分的长度
 * @param frac_length 小数部分的长度
 * @param size 数字大小，只能为半角字符大小
 */
void oled_show_float_num_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, double num, uint8_t int_length, uint8_t frac_length, bool ignore_positive_sgn, OLEDFontSizeHalf size);

/**
 *@brief 绘制点
 * 
 * @param x 点的x坐标
 * @param y 点的y坐标
 */
void oled_draw_point(int16_t x, int16_t y);

/**
 *@brief 获取点的状态（0或1）
 * 
 * @param x 点的x坐标
 * @param y 点的y坐标
 * @return bool 点的状态，true表示点亮，false表示熄灭
 */
bool oled_get_point(uint8_t x, uint8_t y);

/**
 *@brief 绘制线段
 * 
 * @param x0 线段起点x坐标
 * @param y0 线段起点y坐标
 * @param x1 线段终点x坐标
 * @param y1 线段终点y坐标
 */
void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

/**
 *@brief 绘制矩形
 * 
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 * @param is_filled 是否填充，true表示填充，false表示仅绘制边框
 */
void oled_draw_rectangle(int16_t x, int16_t y, int16_t width, int16_t height, bool is_filled);

/**
 *@brief 绘制三角形
 * 
 * @param x0 三角形第一个顶点x坐标
 * @param y0 三角形第一个顶点y坐标
 * @param x1 三角形第二个顶点x坐标
 * @param y1 三角形第二个顶点y坐标
 * @param x2 三角形第三个顶点x坐标
 * @param y2 三角形第三个顶点y坐标
 * @param is_filled 是否填充，true表示填充，false表示仅绘制边框
 */
void oled_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool is_filled);

/**
 *@brief 绘制圆形
 * 
 * @param x 圆心x坐标
 * @param y 圆心y坐标
 * @param radius 圆半径
 * @param is_filled 是否填充，true表示填充，false表示仅绘制边框
 */
void oled_draw_circle(int16_t x, int16_t y, int16_t radius, bool is_filled);

/**
 *@brief 绘制椭圆
 * 
 * @param x 圆心x坐标
 * @param y 圆心y坐标
 * @param a 椭圆水平半轴长度
 * @param b 椭圆垂直半轴长度
 * @param is_filled 是否填充，true表示填充，false表示仅绘制边框
 */
void oled_draw_ellipse(int16_t x, int16_t y, int16_t a, int16_t b, bool is_filled);

/**
 *@brief 绘制弧线
 * 
 * @param x 圆心x坐标
 * @param y 圆心y坐标
 * @param radius 圆半径
 * @param start_angle 起始角度，单位为度，0度表示水平向右，顺时针方向为正
 * @param end_angle 终止角度，单位为度，0度表示水平向右，顺时针方向为正
 * @param is_filled 是否填充，true表示填充，false表示仅绘制边框
 */
void oled_draw_arc(int16_t x, int16_t y, int16_t radius, int16_t start_angle, int16_t end_angle, bool is_filled);

/**
 *@brief 绘制圆角矩形
 * 
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 * @param radius 圆角半径
 * @param is_filled 是否填充，true表示填充，false表示仅绘制边框
 */
void oled_draw_rounded_rectangle(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius, bool is_filled);

