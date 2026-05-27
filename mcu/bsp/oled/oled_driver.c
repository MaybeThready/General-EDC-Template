#include "oled_driver.h"
#include "../msp_sys.h"
#include <string.h>

uint8_t oled_display_buffer[OLED_HEIGHT / 8][OLED_WIDTH];  // 显存

/**
 *@brief 向OLED写入一个字节
 * 
 * @param data 要写入的数据
 * @param cmd 指示是命令还是数据，非零表示数据，零表示命令
 */
void oled_write_byte(uint8_t data, uint8_t cmd)
{
    if (cmd)
    {
        oled_set_dc();
    }
    else
    {
        oled_clear_dc();
    }
    oled_clear_cs();
    while (DL_SPI_isBusy(OLED_SPI_INST)) { __NOP(); }
    DL_SPI_transmitData8(OLED_SPI_INST, data);
    while (DL_SPI_isBusy(OLED_SPI_INST)) { __NOP(); }
    oled_set_cs();
}

/**
 *@brief 向OLED写入一个字节数组
 * 
 * @param arr 要写入的数据数组
 * @param cnt 要写入的数据字节数
 */
void oled_write_arr(uint8_t* arr, uint8_t cnt)
{
    for (uint8_t i = 0; i < cnt; i++)
    {
        oled_write_byte(arr[i], OLED_DATA);
    }
}

/**
 *@brief OLED颜色反转
 * 
 * @param i 颜色反转开关，0表示关闭，1表示开启
 */
void oled_color_turn(uint8_t i)
{
    if (i == 0)
    {
        oled_write_byte(0xA6, OLED_CMD);
    }
    if (i == 1)
    {
        oled_write_byte(0xA7, OLED_CMD);
    }
}

/**
 *@brief OLED显示翻转180度
 * 
 * @param i 显示翻转开关，0表示关闭，1表示开启
 */
void oled_display_turn(uint8_t i)
{
    if (i == 0)
    {
        oled_write_byte(0xC8, OLED_CMD);
        oled_write_byte(0xA1, OLED_CMD);
    }
    if (i == 1)
    {
        oled_write_byte(0xC0, OLED_CMD);
        oled_write_byte(0xA0, OLED_CMD);
    }
}

/**
 *@brief 打开OLED显示
 *
 */
void oled_display_on()
{
    oled_write_byte(0X8D, OLED_CMD);
    oled_write_byte(0X14, OLED_CMD);
    oled_write_byte(0XAF, OLED_CMD);
}

/**
 *@brief 关闭OLED显示
 * 
 */
void oled_display_off()
{
    oled_write_byte(0X8D, OLED_CMD);
    oled_write_byte(0X10, OLED_CMD);
    oled_write_byte(0XAE, OLED_CMD);
}

/**
 *@brief 设置游标位置
 * 
 * @param page 游标所在页，范围0~7
 * @param x 游标所在列，范围0~127
 */
void oled_set_cursor(uint8_t page, uint8_t x)
{
    x += 2;  // SH1106需要偏移2个像素
    oled_write_byte(0xB0 | page, OLED_CMD);
    oled_write_byte(0x10 | ((x & 0xF0) >> 4), OLED_CMD);
    oled_write_byte(0x00 | (x & 0x0F), OLED_CMD);
}

void oled_refresh()
{
    for (uint8_t j = 0; j < 8; j++)
    {
        oled_set_cursor(j, 0);
        oled_write_arr(oled_display_buffer[j], OLED_WIDTH);
    }
}

void oled_refresh_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    if (x > OLED_WIDTH - 1 || y > OLED_HEIGHT - 1)
    {
        return;
    }

    if (x + width > OLED_WIDTH)
    {
        width = OLED_WIDTH - x;
    }

    if (y + height > OLED_HEIGHT)
    {
        height = OLED_HEIGHT - y;
    }

    for (uint8_t j = y / 8; j < (y + height - 1) / 8 + 1; j++)
    {
        oled_set_cursor(j, x);
        oled_write_arr(&oled_display_buffer[j][x], width);
    }
}

void init_oled()
{
    oled_clear_res();
    delay_ms(200);
    oled_set_res();

    oled_write_byte(0xAE, OLED_CMD);
    oled_write_byte(0x02, OLED_CMD);
    oled_write_byte(0x10, OLED_CMD);
    oled_write_byte(0x40, OLED_CMD);
    oled_write_byte(0xB0, OLED_CMD);
    oled_write_byte(0x81, OLED_CMD);
    oled_write_byte(0xFF, OLED_CMD);
    oled_write_byte(0xA1, OLED_CMD);
    oled_write_byte(0xA6, OLED_CMD);
    oled_write_byte(0xA8, OLED_CMD);
    oled_write_byte(0x3F, OLED_CMD);
    oled_write_byte(0xAD, OLED_CMD);
    oled_write_byte(0x8B, OLED_CMD);
    oled_write_byte(0x32, OLED_CMD);
    oled_write_byte(0xC8, OLED_CMD);
    oled_write_byte(0xD3, OLED_CMD);
    oled_write_byte(0x00, OLED_CMD);
    oled_write_byte(0xD5, OLED_CMD);
    oled_write_byte(0x80, OLED_CMD);
    oled_write_byte(0xD9, OLED_CMD);
    oled_write_byte(0x22, OLED_CMD);
    oled_write_byte(0xDA, OLED_CMD);
    oled_write_byte(0x12, OLED_CMD);
    oled_write_byte(0xDB, OLED_CMD);
    oled_write_byte(0x35, OLED_CMD);

	memset(oled_display_buffer, 0x00, sizeof(oled_display_buffer));
    oled_refresh();

    oled_write_byte(0xAF, OLED_CMD);
    delay_ms(200);
}
