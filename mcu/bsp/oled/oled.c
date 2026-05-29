#include "oled.h"
#include <string.h>

/**
 *@brief 计算m的n次幂
 *
 * @param m
 * @param n
 * @return uint32_t
 */
uint32_t oled_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
	{
		result *= m;
	}
	return result;
}

/**
 * @brief 判断指定点是否在指定多边形内部
 *
 * @param nvert 多边形的顶点数
 * @param vertx 包含多边形顶点的x坐标的数组
 * @param verty 包含多边形顶点的y坐标的数组
 * @param testx 测试点的x坐标
 * @param testy 测试点的y坐标
 * @return bool 如果测试点在多边形内部返回true，否则返回false
 */
bool oled_pnpoly(uint8_t nvert, int16_t* vertx, int16_t* verty, int16_t testx, int16_t testy)
{
	int16_t i = 0;
	int16_t j = 0;
	bool c = false;
	/*此算法由W. Randolph Franklin提出*/
	/*参考链接：https://wrfranklin.org/Research/Short_Notes/pnpoly.html*/
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

/**
 * @brief 判断指定点是否在指定角度内部
 *
 * @param x 指定点的x坐标
 * @param y 指定点的y坐标
 * @param start_angle 起始角度，范围：-180~180，水平向右为0度，水平向左为180度或-180度，下方为正数，上方为负数，顺时针旋转
 * @param end_angle 终止角度，范围：-180~180，水平向右为0度，水平向左为180度或-180度，下方为正数，上方为负数，顺时针旋转
 * @return true 指定点在指定角度内部
 * @return false 指定点不在指定角度内部
 */
bool oled_is_in_angle(int16_t x, int16_t y, int16_t start_angle, int16_t end_angle)
{
	int16_t point_angle;
	point_angle = atan2(y, x) / 3.14 * 180;	//计算指定点的弧度，并转换为角度表示
	if (start_angle < end_angle)	//起始角度小于终止角度的情况
	{
		/*如果指定角度在起始终止角度之间，则判定指定点在指定角度*/
		if (point_angle >= start_angle && point_angle <= end_angle)
		{
			return true;
		}
	}
	else			//起始角度大于于终止角度的情况
	{
		/*如果指定角度大于起始角度或者小于终止角度，则判定指定点在指定角度*/
		if (point_angle >= start_angle || point_angle <= end_angle)
		{
			return true;
		}
	}
	return false;		//不满足以上条件，则判断判定指定点不在指定角度
}

void oled_clear()
{
	memset(oled_display_buffer, 0x00, sizeof(oled_display_buffer));
}

void oled_clear_area(int16_t x, int16_t y, int16_t width, int16_t height)
{
	int16_t x_start;
	int16_t y_start;
	int16_t x_end;
	int16_t y_end;
	int16_t i;
	int16_t j;

	if (width <= 0 || height <= 0)
	{
		return;
	}

	// 计算X方向的起始和结束位置
	x_start = (x < 0) ? 0 : x;
	x_end = x + width;
	if (x_end > OLED_WIDTH)
	{
		x_end = OLED_WIDTH;
	}
	if (x_start >= x_end)
	{
		return;
	}

	// 计算Y方向的起始和结束位置
	y_start = (y < 0) ? 0 : y;
	y_end = y + height;
	if (y_end > OLED_HEIGHT)
	{
		y_end = OLED_HEIGHT;
	}
	if (y_start >= y_end)
	{
		return;
	}

	// 调整Width和Height为实际需要清除的区域
	width = x_end - x_start;
	height = y_end - y_start;

	for (j = y_start; j < y_end; j++)
	{
		for (i = x_start; i < x_end; i++)
		{
			oled_display_buffer[j / 8][i] &= ~(0x01 << (j % 8));
		}
	}
}

void oled_reverse(void)
{
	uint8_t i;
	uint8_t j;
	for (i = 0; i < OLED_HEIGHT / 8; i++)
	{
		for (j = 0; j < OLED_WIDTH; j++)
		{
			oled_display_buffer[i][j] ^= 0xFF;	//将显存数组数据全部取反
		}
	}
}

void oled_reverse_area(int16_t x, int16_t y, int16_t width, int16_t height)
{
	int16_t i;
	int16_t j;
	if (width <= 0 || height <= 0)
	{
		return;
	}
	/*参数检查，保证指定区域不会超出屏幕范围*/
	if (x > OLED_WIDTH - 1)
	{
		return;
	}
	if (y > OLED_HEIGHT - 1)
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
	if (x + width < 0)
	{
		return;
	}
	if (y + height < 0)
	{
		return;
	}
	if (x < 0)
	{
		x = 0;
	}
	if (y < 0)
	{
		y = 0;
	}

	for (i = y; i < y + height; i++)		//遍历指定页
	{
		for (j = x; j < x + width; j++)	//遍历指定列
		{
			oled_display_buffer[i / 8][j] ^= 0x01 << (i % 8);	//将显存数组指定数据取反
		}
	}
}

void oled_show_image(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t* image)
{
	uint8_t i;
	uint8_t j;

	/* 参数检查，保证指定图像不会超出屏幕范围 */
	if (width == 0 || height == 0)
	{
		return; // 如果宽度或高度为0，直接返回
	}

	if (x > OLED_WIDTH - 1)
	{
		return; // X 超出右边界，直接返回
	}
	if (y > OLED_HEIGHT - 1)
	{
		return; // Y 超出下边界，直接返回
	}

	/* 将图像所在区域清空 */
	uint8_t start_x = (x < 0) ? 0 : x; // 计算实际起始显示位置的 X 坐标
	uint8_t start_y = (y < 0) ? 0 : y; // 计算实际起始显示位置的 Y 坐标
	uint8_t end_x = (x + width - 1 > OLED_WIDTH - 1) ? OLED_WIDTH - 1 : x + width - 1; // 计算实际结束显示位置的 X 坐标
	uint8_t end_y = (y + height - 1 > OLED_HEIGHT - 1) ? OLED_HEIGHT - 1 : y + height - 1; // 计算实际结束显示位置的 Y 坐标

	oled_clear_area(start_x, start_y, end_x - start_x + 1, end_y - start_y + 1);

	/* 遍历指定图像涉及的相关页 */
	for (j = 0; j < (height - 1) / 8 + 1; j++)
	{
		/* 遍历指定图像涉及的相关列 */
		for (i = 0; i < width; i++)
		{
			int16_t curr_x = x + i;
			int16_t curr_y = y + j * 8;

			/* 超出边界，则跳过显示 */
			if (curr_x < 0 || curr_x > OLED_WIDTH - 1 || curr_y < 0 || curr_y > OLED_HEIGHT - 1)
			{
				continue;
			}
			/* 显示图像在当前页的内容 */
			oled_display_buffer[curr_y / 8][curr_x] |= image[j * width + i] << (curr_y % 8);
			/* 当前页下一页 */
			if (curr_y + 8 <= OLED_HEIGHT - 1)
			{
				oled_display_buffer[curr_y / 8 + 1][curr_x] |= image[j * width + i] >> (8 - curr_y % 8);
			}
		}
	}
	if (y < 0)
	{
		for (i = 0; i < width; i++)
		{
			int16_t curr_x = x + i;
			if (curr_x < 0 || curr_x > OLED_WIDTH - 1)
			{
				continue;
			}
			oled_display_buffer[0][curr_x] |= image[-y / 8 * width + i] >> -y % 8;
		}
	}
}

void oled_show_char(int16_t x, int16_t y, char chr, OLEDFontSizeHalf size)
{
	if (size == OLED_8X16_HALF)		//字体为宽8像素，高16像素
	{
		/*将ASCII字模库OLED_F8x16的指定数据以8*16的图像格式显示*/
		oled_show_image(x, y, 8, 16, OLED_F8x16[chr - ' ']);
	}
	else if (size == OLED_6X8_HALF)	//字体为宽6像素，高8像素
	{
		/*将ASCII字模库OLED_F6x8的指定数据以6*8的图像格式显示*/
		oled_show_image(x, y, 6, 8, OLED_F6x8[chr - ' ']);
	}
	else if (size == OLED_7X12_HALF)	//字体为宽6像素，高8像素
	{
		/*将ASCII字模库OLED_F6x8的指定数据以6*8的图像格式显示*/
		oled_show_image(x, y, 7, 12, OLED_F7x12[chr - ' ']);
	}
	else if (size == OLED_10X20_HALF)
	{
		/*将ASCII字模库OLED_F10X20的指定数据以10x20的图像格式显示*/
		oled_show_image(x, y, 10, 20, OLED_F10x20[chr - ' ']);
	}
}

void oled_show_num(int16_t x, int16_t y, uint32_t num, uint8_t length, OLEDFontSizeHalf size)
{
	uint8_t i;
	for (i = 0; i < length; i++)		//遍历数字的每一位
	{
		oled_show_char(x + i * size, y, num / oled_pow(10, length - i - 1) % 10 + '0', size);
	}
}

void oled_show_signed_num(int16_t x, int16_t y, int32_t num, uint8_t length, OLEDFontSizeHalf size)
{
	uint8_t i;
	uint32_t unsigned_num;

	if (num >= 0)						//数字大于等于0
	{
		oled_show_char(x, y, '+', size);	//显示+号
		unsigned_num = num;					//unsigned_num直接等于num
	}
	else									//数字小于0
	{
		oled_show_char(x, y, '-', size);	//显示-号
		unsigned_num = -num;					//unsigned_num等于num取负
	}

	for (i = 0; i < length; i++)			//遍历数字的每一位
	{
		oled_show_char(x + (i + 1) * size, y, unsigned_num / oled_pow(10, length - i - 1) % 10 + '0', size);
	}
}

void oled_show_hex_num(int16_t x, int16_t y, int32_t num, uint8_t length, OLEDFontSizeHalf size)
{
	uint8_t i;
	uint8_t single_num;
	for (i = 0; i < length; i++)		//遍历数字的每一位
	{
		/*以十六进制提取数字的每一位*/
		single_num = num / oled_pow(16, length - i - 1) % 16;

		if (single_num < 10)			//单个数字小于10
		{
			oled_show_char(x + i * size, y, single_num + '0', size);
		}
		else							//单个数字大于10
		{
			oled_show_char(x + i * size, y, single_num - 10 + 'A', size);
		}
	}
}

void oled_show_bin_num(int16_t x, int16_t y, int32_t num, uint8_t length, OLEDFontSizeHalf size)
{
	uint8_t i;
	for (i = 0; i < length; i++)		//遍历数字的每一位
	{
		oled_show_char(x + i * size, y, num / oled_pow(2, length - i - 1) % 2 + '0', size);
	}
}

void oled_show_float_num(int16_t x, int16_t y, double num, uint8_t int_length, uint8_t frac_length, OLEDFontSizeHalf size)
{
	uint32_t pow_num;
	uint32_t int_num;
	uint32_t frac_num;

	if (num >= 0)						//数字大于等于0
	{
		oled_show_char(x, y, '+', size);	//显示+号
	}
	else									//数字小于0
	{
		oled_show_char(x, y, '-', size);	//显示-号
		num = -num;					//num取负
	}

	/*提取整数部分和小数部分*/
	int_num = num;						//直接赋值给整型变量，提取整数
	num -= int_num;						//将num的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	pow_num = oled_pow(10, frac_length);		//根据指定小数的位数，确定乘数
	frac_num = round(num * pow_num);		//将小数乘到整数，同时四舍五入，避免显示误差
	int_num += frac_num / pow_num;				//若四舍五入造成了进位，则需要再加给整数

	/*显示整数部分*/
	oled_show_num(x + size, y, int_num, int_length, size);

	/*显示小数点*/
	oled_show_char(x + (int_length + 1) * size, y, '.', size);

	/*显示小数部分*/
	oled_show_num(x + (int_length + 2) * size, y, frac_num, frac_length, size);
}

void oled_show_string(int16_t x, int16_t y, const char* str, OLEDFontSizeHalf size)
{
	uint8_t i;
	for (i = 0; str[i] != '\0'; i++)		//遍历字符串的每个字符
	{
		oled_show_char(x + i * size, y, str[i], size);

	}
}

void oled_show_chinese(int16_t x, int16_t y, const char* str, OLEDFontSizeFull size)
{
	uint8_t p_chinese = 0;
	uint8_t p_index;
	uint8_t i;
	char single_chinese[OLED_CHN_CHAR_WIDTH + 1] = { 0 };

	for (i = 0; str[i] != '\0'; i++)    // 遍历汉字串
	{
		single_chinese[p_chinese] = str[i];    // 提取汉字串数据到单个汉字数组
		p_chinese++;                            // 计次自增

		if (p_chinese >= OLED_CHN_CHAR_WIDTH)    // 提取到了一个完整的汉字
		{
			p_chinese = 0;    // 计次归零
			const void* font_array;
			if (size == OLED_8X8_FULL)
			{
				font_array = (const void*)OLED_CF8x8;
			}
			else if (size == OLED_12X12_FULL)
			{
				font_array = (const void*)OLED_CF12x12;
			}
			else if (size == OLED_16X16_FULL)
			{
				font_array = (const void*)OLED_CF16x16;
			}
			else if (size == OLED_20X20_FULL)
			{
				font_array = (const void*)OLED_CF20x20;
			}

			if (size == OLED_8X8_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell8x8*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell8x8*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image(x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_8X8_FULL, y, OLED_8X8_FULL, OLED_8X8_FULL, ((const ChineseCell8x8*)font_array)[p_index].data);
			}
			else if (size == OLED_12X12_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell12x12*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell12x12*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image(x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_12X12_FULL, y, OLED_12X12_FULL, OLED_12X12_FULL, ((const ChineseCell12x12*)font_array)[p_index].data);
			}
			else if (size == OLED_16X16_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell16x16*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell16x16*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image(x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_16X16_FULL, y, OLED_16X16_FULL, OLED_16X16_FULL, ((const ChineseCell16x16*)font_array)[p_index].data);
			}
			else if (size == OLED_20X20_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell20x20*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell20x20*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image(x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_20X20_FULL, y, OLED_20X20_FULL, OLED_20X20_FULL, ((const ChineseCell20x20*)font_array)[p_index].data);
			}
		}
	}
}

void oled_show_mix_string(int16_t x, int16_t y, const char* str, OLEDFontSizeFull chinese_size, OLEDFontSizeHalf ascii_size)
{
	while (*str != '\0')
	{
		if (*str & 0x80)
		{
			// 判断是否是中文字符 (最高位为1表示中文字符)
			char chinese[OLED_CHN_CHAR_WIDTH + 1];
			for (uint8_t i = 0; i < OLED_CHN_CHAR_WIDTH; i++)
			{
				chinese[i] = *(str + i);
			}
			chinese[OLED_CHN_CHAR_WIDTH] = '\0';
			oled_show_chinese(x, y, chinese, chinese_size);
			x += chinese_size;  // 中文字符宽度
			str += OLED_CHN_CHAR_WIDTH;  // 跳过当前的两个字节的中文字符
		}
		else
		{
			// 如果是ASCII字符
			oled_show_char(x, y, *str, ascii_size);
			x += ascii_size; // ASCII字符宽度
			str++; // 指向下一个字符
		}
	}
}

void oled_show_image_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t* image)
{
	if (width == 0 || height == 0 || area_width == 0 || area_height == 0 || x > OLED_WIDTH - 1 || area_x > OLED_WIDTH - 1 || y > OLED_HEIGHT - 1 || area_y > OLED_HEIGHT - 1)
	{
		return;
	}
	int16_t start_x = (x < area_x) ? area_x : x;
	int16_t end_x = ((area_x + area_width - 1) < (x + width - 1)) ? (area_x + area_width - 1) : (x + width - 1);
	int16_t start_y = (y < area_y) ? area_y : y;
	int16_t end_y = ((area_y + area_height - 1) < (y + height - 1)) ? (area_y + area_height - 1) : (y + height - 1);
	end_x = (end_x > OLED_WIDTH - 1) ? OLED_WIDTH - 1 : end_x;
	end_y = (end_y > OLED_HEIGHT - 1) ? OLED_HEIGHT - 1 : end_y;
	if (start_x > end_x || start_y > end_y)
	{
		return;
	}
	for (uint8_t j = 0; j <= (height - 1) / 8; j++)
	{
		for (uint8_t i = 0; i < width; i++)
		{
			uint8_t curr_x = x + i;
			if (curr_x < start_x || curr_x > end_x)
			{
				continue;
			}
			for (uint8_t bit = 0; bit < 8; bit++)
			{
				uint8_t curr_y = y + j * 8 + bit;
				if (curr_y < start_y || curr_y > end_y)
				{
					continue;
				}
				uint8_t page = curr_y / 8;
				uint8_t bit_pos = curr_y % 8;
				uint8_t data = image[j * width + i];
				if (data & (1 << bit))
				{
					oled_display_buffer[page][curr_x] |= (1 << bit_pos);
				}
			}
		}
	}
}

void oled_show_char_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, char chr, OLEDFontSizeHalf size)
{

	if (size == OLED_8X16_HALF)		//字体为宽8像素，高16像素
	{
		/*将ASCII字模库OLED_F8x16的指定数据以8*16的图像格式显示*/
		oled_show_image_area(area_x, area_y, area_width, area_height, x, y, 8, 16, OLED_F8x16[chr - ' ']);
	}
	else if (size == OLED_6X8_HALF)	//字体为宽6像素，高8像素
	{
		/*将ASCII字模库OLED_F6x8的指定数据以6*8的图像格式显示*/
		oled_show_image_area(area_x, area_y, area_width, area_height, x, y, 6, 8, OLED_F6x8[chr - ' ']);
	}
	else if (size == OLED_7X12_HALF)	//字体为宽6像素，高8像素
	{
		/*将ASCII字模库OLED_F7X12的指定数据以6*8的图像格式显示*/
		oled_show_image_area(area_x, area_y, area_width, area_height, x, y, 7, 12, OLED_F7x12[chr - ' ']);
	}
	else if (size == OLED_10X20_HALF)
	{
		/*将ASCII字模库OLED_F10x20的指定数据以6*8的图像格式显示*/
		oled_show_image_area(area_x, area_y, area_width, area_height, x, y, 10, 20, OLED_F10x20[chr - ' ']);
	}
}

void oled_show_string_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, const char* string, uint8_t size)
{
	/*由于有可能显示极长的字符串，所以uint16_t*/
	uint16_t i;
	for (i = 0; string[i] != '\0'; i++)		//遍历字符串的每个字符
	{
		/*调用OLED_ShowCharArea函数，依次显示每个字符*/
		oled_show_char_area(area_x, area_y, area_width, area_height, x + i * size, y, string[i], size);
	}
}

void oled_show_chinese_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, const char* chinese, OLEDFontSizeFull size)
{
	uint8_t p_chinese = 0;
	uint8_t p_index;
	uint8_t i;
	char single_chinese[OLED_CHN_CHAR_WIDTH + 1] = { 0 };
	for (i = 0; chinese[i] != '\0'; i++)    // 遍历汉字串
	{
		single_chinese[p_chinese] = chinese[i];    // 提取汉字串数据到单个汉字数组
		p_chinese++;                            // 计次自增

		if (p_chinese >= OLED_CHN_CHAR_WIDTH)    // 提取到了一个完整的汉字
		{
			p_chinese = 0;    // 计次归零
			const void* font_array;
			if (size == OLED_8X8_FULL)
			{
				font_array = (const void*)OLED_CF8x8;
			}
			else if (size == OLED_12X12_FULL)
			{
				font_array = (const void*)OLED_CF12x12;
			}
			else if (size == OLED_16X16_FULL)
			{
				font_array = (const void*)OLED_CF16x16;
			}
			else if (size == OLED_20X20_FULL)
			{
				font_array = (const void*)OLED_CF20x20;
			}

			if (size == OLED_8X8_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell8x8*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell8x8*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image_area(area_x, area_y, area_width, area_height, x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_8X8_FULL, y, OLED_8X8_FULL, OLED_8X8_FULL, ((const ChineseCell8x8*)font_array)[p_index].data);
			}
			else if (size == OLED_12X12_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell12x12*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell12x12*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image_area(area_x, area_y, area_width, area_height, x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_12X12_FULL, y, OLED_12X12_FULL, OLED_12X12_FULL, ((const ChineseCell12x12*)font_array)[p_index].data);
			}
			else if (size == OLED_16X16_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell16x16*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell16x16*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image_area(area_x, area_y, area_width, area_height, x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_16X16_FULL, y, OLED_16X16_FULL, OLED_16X16_FULL, ((const ChineseCell16x16*)font_array)[p_index].data);
			}
			else if (size == OLED_20X20_FULL)
			{
				for (p_index = 0; strcmp(((const ChineseCell20x20*)font_array)[p_index].index, "") != 0; p_index++)
				{
					if (strcmp(((const ChineseCell20x20*)font_array)[p_index].index, single_chinese) == 0)
					{
						break;
					}
				}
				oled_show_image_area(area_x, area_y, area_width, area_height, x + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * OLED_20X20_FULL, y, OLED_20X20_FULL, OLED_20X20_FULL, ((const ChineseCell20x20*)font_array)[p_index].data);
			}
		}
	}
}

void oled_show_mix_string_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, const char* string, OLEDFontSizeFull chinese_size, OLEDFontSizeHalf ascii_size)
{
	while (*string != '\0')
	{
		if (*string & 0x80)
		{
			// 判断中文字符（最高位为1）
			char chinese[OLED_CHN_CHAR_WIDTH + 1]; // 根据编码长度动态调整数组
			for (uint8_t i = 0; i < OLED_CHN_CHAR_WIDTH; i++)
			{
				chinese[i] = *(string + i); // 连续拷贝字符编码
			}
			chinese[OLED_CHN_CHAR_WIDTH] = '\0'; // 添加字符串结束符
			oled_show_chinese_area(area_x, area_y, area_width, area_height, x, y, chinese, chinese_size);
			x += chinese_size; // 更新X坐标
			string += OLED_CHN_CHAR_WIDTH; // 跳过已处理的中文字符
		}
		else
		{
			// ASCII字符处理
			oled_show_char_area(area_x, area_y, area_width, area_height, x, y, *string, ascii_size);
			x += ascii_size; // 更新X坐标
			string++; // 处理下一个字符
		}
	}
}

void oled_show_num_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, uint32_t num, uint8_t length, OLEDFontSizeHalf size)
{
	uint8_t i;
	for (i = 0; i < length; i++)		//遍历数字的每一位
	{
		oled_show_char_area(area_x, area_y, area_width, area_height, x + i * size, y, num / oled_pow(10, length - i - 1) % 10 + '0', size);
	}
}

void oled_show_float_num_area(int16_t area_x, int16_t area_y, int16_t area_width, int16_t area_height, int16_t x, int16_t y, double num, uint8_t int_length, uint8_t frac_length, bool ignore_positive_sgn, OLEDFontSizeHalf size)
{
	uint32_t pow_num;
	uint32_t int_num;
	uint32_t frac_num;

	if (num >= 0)						//数字大于等于0
	{
		if (!ignore_positive_sgn)
		{
			oled_show_char_area(area_x, area_y, area_width, area_height, x, y, '+', size);	//显示+号
		}
		else
		{
			x -= size;	//不显示+号时，数字整体左移一个字符的位置，以保持和负数的对齐
		}
	}
	else									//数字小于0
	{
		oled_show_char_area(area_x, area_y, area_width, area_height, x, y, '-', size);	//显示-号
		num = -num;					//num取负
	}

	/*提取整数部分和小数部分*/
	int_num = num;						//直接赋值给整型变量，提取整数
	num -= int_num;						//将num的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	pow_num = oled_pow(10, frac_length);		//根据指定小数的位数，确定乘数
	frac_num = round(num * pow_num);		//将小数乘到整数，同时四舍五入，避免显示误差
	int_num += frac_num / pow_num;				//若四舍五入造成了进位，则需要再加给整数

	/*显示整数部分*/
	oled_show_num_area(area_x, area_y, area_width, area_height, x + size, y, int_num, int_length, size);

	/*显示小数点*/
	oled_show_char_area(area_x, area_y, area_width, area_height, x + (int_length + 1) * size, y, '.', size);

	/*显示小数部分*/
	oled_show_num_area(area_x, area_y, area_width, area_height, x + (int_length + 2) * size, y, frac_num, frac_length, size);
}

void oled_draw_point(int16_t x, int16_t y)
{
	/*参数检查，保证指定位置不会超出屏幕范围*/
	if (x < 0 || y < 0 || x > OLED_WIDTH - 1 || y > OLED_HEIGHT - 1)
	{
		return;
	}

	/*将显存数组指定位置的一个Bit数据置1*/
	oled_display_buffer[y / 8][x] |= 0x01 << (y % 8);
}

bool oled_get_point(uint8_t x, uint8_t y)
{
	/*参数检查，保证指定位置不会超出屏幕范围*/
	if (x > OLED_WIDTH - 1)
	{
		return 0;
	}
	if (y > OLED_HEIGHT - 1)
	{
		return 0;
	}

	/*判断指定位置的数据*/
	if (oled_display_buffer[y / 8][x] & 0x01 << (y % 8))
	{
		return true;	//为1，返回1
	}
	return false;		//否则，返回0
}

/**
  * 函    数：OLED画线
  * 参    数：X0 指定一个端点的横坐标，范围：0~127
  * 参    数：Y0 指定一个端点的纵坐标，范围：0~63
  * 参    数：X1 指定另一个端点的横坐标，范围：0~127
  * 参    数：Y1 指定另一个端点的纵坐标，范围：0~63
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	int16_t x;
	int16_t y;
	int16_t dx;
	int16_t dy;
	int16_t d;
	int16_t incr_e;
	int16_t incr_ne;
	int16_t temp;
	uint8_t y_flag = 0;
	uint8_t xy_flag = 0;

	if (y0 == y1)		//横线单独处理
	{
		/*0号点X坐标大于1号点X坐标，则交换两点X坐标*/
		if (x0 > x1)
		{
			temp = x0;
			x0 = x1;
			x1 = temp;
		}

		/*遍历X坐标*/
		for (x = x0; x <= x1; x++)
		{
			oled_draw_point(x, y0);	//依次画点
		}
	}
	else if (x0 == x1)	//竖线单独处理
	{
		/*0号点Y坐标大于1号点Y坐标，则交换两点Y坐标*/
		if (y0 > y1)
		{
			temp = y0;
			y0 = y1;
			y1 = temp;
		}

		/*遍历Y坐标*/
		for (y = y0; y <= y1; y++)
		{
			oled_draw_point(x0, y);	//依次画点
		}
	}
	else				//斜线
	{
		/*使用Bresenham算法画直线，可以避免耗时的浮点运算，效率更高*/
		/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
		/*参考教程：https://www.bilibili.com/video/BV1364y1d7Lo*/

		if (x0 > x1)	//0号点X坐标大于1号点X坐标
		{
			/*交换两点坐标*/
			/*交换后不影响画线，但是画线方向由第一、二、三、四象限变为第一、四象限*/
			temp = x0;
			x0 = x1;
			x1 = temp;
			temp = y0;
			y0 = y1;
			y1 = temp;
		}

		if (y0 > y1)	//0号点Y坐标大于1号点Y坐标
		{
			/*将Y坐标取负*/
			/*取负后影响画线，但是画线方向由第一、四象限变为第一象限*/
			y0 = -y0;
			y1 = -y1;

			/*置标志位yflag，记住当前变换，在后续实际画线时，再将坐标换回来*/
			y_flag = 1;
		}

		if (y1 - y0 > x1 - x0)	//画线斜率大于1
		{
			/*将X坐标与Y坐标互换*/
			/*互换后影响画线，但是画线方向由第一象限0~90度范围变为第一象限0~45度范围*/
			temp = x0;
			x0 = y0;
			y0 = temp;
			temp = x1;
			x1 = y1;
			y1 = temp;

			/*置标志位xyflag，记住当前变换，在后续实际画线时，再将坐标换回来*/
			xy_flag = 1;
		}

		/*以下为Bresenham算法画直线*/
		/*算法要求，画线方向必须为第一象限0~45度范围*/
		dx = x1 - x0;
		dy = y1 - y0;
		incr_e = 2 * dy;
		incr_ne = 2 * (dy - dx);
		d = 2 * dy - dx;
		x = x0;
		y = y0;

		/*画起始点，同时判断标志位，将坐标换回来*/
		if (y_flag && xy_flag)
		{
			oled_draw_point(y, -x);
		}
		else if (y_flag)
		{
			oled_draw_point(x, -y);
		}
		else if (xy_flag)
		{
			oled_draw_point(y, x);
		}
		else
		{
			oled_draw_point(x, y);
		}

		while (x < x1)		//遍历X轴的每个点
		{
			x++;
			if (d < 0)		//下一个点在当前点东方
			{
				d += incr_e;
			}
			else			//下一个点在当前点东北方
			{
				y++;
				d += incr_ne;
			}

			/*画每一个点，同时判断标志位，将坐标换回来*/
			if (y_flag && xy_flag)
			{
				oled_draw_point(y, -x);
			}
			else if (y_flag)
			{
				oled_draw_point(x, -y);
			}
			else if (xy_flag)
			{
				oled_draw_point(y, x);
			}
			else
			{
				oled_draw_point(x, y);
			}
		}
	}
}

void oled_draw_rectangle(int16_t x, int16_t y, int16_t width, int16_t height, bool is_filled)
{
	if (width == 0 || height == 0)
	{
		return; // 宽度或高度为0，不绘制
	}

	int16_t x_start = x;
	int16_t x_end = x + width - 1;
	int16_t y_start = y;
	int16_t y_end = y + height - 1;

	// 限制坐标在屏幕范围内
	if (x_start < 0)
	{
		x_start = 0;
	}
	if (x_end >= OLED_WIDTH)
	{
		x_end = OLED_WIDTH - 1;
	}
	if (y_start < 0)
	{
		y_start = 0;
	}
	if (y_end >= OLED_HEIGHT)
	{
		y_end = OLED_HEIGHT - 1;
	}

	// 计算有效宽度和高度
	int16_t valid_width = x_end - x_start + 1;
	int16_t valid_height = y_end - y_start + 1;
	if (valid_width <= 0 || valid_height <= 0)
	{
		return;
	}

	if (!is_filled)
	{
		// 绘制矩形边框
		for (int16_t i = x_start; i <= x_end; i++)
		{
			oled_draw_point(i, y_start);
			oled_draw_point(i, y_end);
		}
		for (int16_t i = y_start; i <= y_end; i++)
		{
			oled_draw_point(x_start, i);
			oled_draw_point(x_end, i);
		}
	}
	else
	{
		// 计算起始和结束页
		int16_t start_page = y_start / 8;
		int16_t end_page = y_end / 8;

		// 计算每页的掩码
		uint8_t start_mask = 0xFF << (y_start % 8);
		uint8_t end_mask = 0xFF >> (7 - (y_end % 8));

		// 遍历每一列，应用掩码
		for (int16_t x_pos = x_start; x_pos <= x_end; x_pos++)
		{
			for (int16_t page = start_page; page <= end_page; page++)
			{
				uint8_t mask = 0xFF;
				if (page == start_page)
				{
					mask &= start_mask;
				}
				if (page == end_page)
				{
					mask &= end_mask;
				}
				if (page >= 0 && page < OLED_HEIGHT / 8)
				{
					// 确保页数有效
					oled_display_buffer[page][x_pos] |= mask;
				}
			}
		}
	}
}

void oled_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool is_filled)
{
	int16_t min_x = x0;
	int16_t min_y = y0;
	int16_t max_x = x0;
	int16_t max_y = y0;
	int16_t i;
	int16_t j;
	int16_t vx[] = { x0, x1, x2 };
	int16_t vy[] = { y0, y1, y2 };

	if (!is_filled)			//指定三角形不填充
	{
		/*调用画线函数，将三个点用直线连接*/
		oled_draw_line(x0, y0, x1, y1);
		oled_draw_line(x0, y0, x2, y2);
		oled_draw_line(x1, y1, x2, y2);
	}
	else					//指定三角形填充
	{
		/*找到三个点最小的X、Y坐标*/
		if (x1 < min_x)
		{
			min_x = x1;
		}
		if (x2 < min_x)
		{
			min_x = x2;
		}
		if (y1 < min_y)
		{
			min_y = y1;
		}
		if (y2 < min_y)
		{
			min_y = y2;
		}

		/*找到三个点最大的X、Y坐标*/
		if (x1 > max_x)
		{
			max_x = x1;
		}
		if (x2 > max_x)
		{
			max_x = x2;
		}
		if (y1 > max_y)
		{
			max_y = y1;
		}
		if (y2 > max_y)
		{
			max_y = y2;
		}

		/*最小最大坐标之间的矩形为可能需要填充的区域*/
		/*遍历此区域中所有的点*/
		/*遍历X坐标*/
		for (i = min_x; i <= max_x; i++)
		{
			/*遍历Y坐标*/
			for (j = min_y; j <= max_y; j++)
			{
				/*调用OLED_pnpoly，判断指定点是否在指定三角形之中*/
				/*如果在，则画点，如果不在，则不做处理*/
				if (oled_pnpoly(3, vx, vy, i, j))
				{
					oled_draw_point(i, j);
				}
			}
		}
	}
}

void oled_draw_circle(int16_t x, int16_t y, int16_t radius, bool is_filled)
{
	int16_t dx, dy, d, j;

	/*使用Bresenham算法画圆，可以避免耗时的浮点运算，效率更高*/
	/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
	/*参考教程：https://www.bilibili.com/video/BV1VM4y1u7wJ*/

	d = 1 - radius;
	dx = 0;
	dy = radius;

	/*画每个八分之一圆弧的起始点*/
	oled_draw_point(x + dx, y + dy);
	oled_draw_point(x - dx, y - dy);
	oled_draw_point(x + dy, y + dx);
	oled_draw_point(x - dy, y - dx);

	if (is_filled)		//指定圆填充
	{
		/*遍历起始点Y坐标*/
		for (j = -dy; j < dy; j++)
		{
			/*在指定区域画点，填充部分圆*/
			oled_draw_point(x, y + j);
		}
	}

	while (dx < dy)		//遍历X轴的每个点
	{
		dx++;
		if (d < 0)		//下一个点在当前点东方
		{
			d += 2 * dx + 1;
		}
		else			//下一个点在当前点东南方
		{
			dy--;
			d += 2 * (dx - dy) + 1;
		}

		/*画每个八分之一圆弧的点*/
		oled_draw_point(x + dx, y + dy);
		oled_draw_point(x + dy, y + dx);
		oled_draw_point(x - dx, y - dy);
		oled_draw_point(x - dy, y - dx);
		oled_draw_point(x + dx, y - dy);
		oled_draw_point(x + dy, y - dx);
		oled_draw_point(x - dx, y + dy);
		oled_draw_point(x - dy, y + dx);

		if (is_filled)	//指定圆填充
		{
			/*遍历中间部分*/
			for (j = -dy; j < dy; j++)
			{
				/*在指定区域画点，填充部分圆*/
				oled_draw_point(x + dx, y + j);
				oled_draw_point(x - dx, y + j);
			}

			/*遍历两侧部分*/
			for (j = -dy; j < dy; j++)
			{
				/*在指定区域画点，填充部分圆*/
				oled_draw_point(x - dy, y + j);
				oled_draw_point(x + dy, y + j);
			}
		}
	}
}

void oled_draw_ellipse(int16_t x, int16_t y, int16_t a, int16_t b, bool is_filled)
{
	int16_t dx, dy, j;
	float d1, d2;

	/*使用Bresenham算法画椭圆，可以避免部分耗时的浮点运算，效率更高*/
	/*参考链接：https://blog.csdn.net/myf_666/article/details/128167392*/

	dx = 0;
	dy = b;
	d1 = b * b + a * a * (-b + 0.5);

	if (is_filled)	//指定椭圆填充
	{
		/*遍历起始点Y坐标*/
		for (j = -dy; j < dy; j++)
		{
			/*在指定区域画点，填充部分椭圆*/
			oled_draw_point(x, y + j);
			oled_draw_point(x, y + j);
		}
	}

	/*画椭圆弧的起始点*/
	oled_draw_point(x + dx, y + dy);
	oled_draw_point(x - dx, y - dy);
	oled_draw_point(x - dx, y + dy);
	oled_draw_point(x + dx, y - dy);

	/*画椭圆中间部分*/
	while (b * b * (dx + 1) < a * a * (dy - 0.5))
	{
		if (d1 <= 0)		//下一个点在当前点东方
		{
			d1 += b * b * (2 * dx + 3);
		}
		else				//下一个点在当前点东南方
		{
			d1 += b * b * (2 * dx + 3) + a * a * (-2 * dy + 2);
			dy--;
		}
		dx++;

		if (is_filled)	//指定椭圆填充
		{
			/*遍历中间部分*/
			for (j = -dy; j < dy; j++)
			{
				/*在指定区域画点，填充部分椭圆*/
				oled_draw_point(x + dx, y + j);
				oled_draw_point(x - dx, y + j);
			}
		}

		/*画椭圆中间部分圆弧*/
		oled_draw_point(x + dx, y + dy);
		oled_draw_point(x - dx, y - dy);
		oled_draw_point(x - dx, y + dy);
		oled_draw_point(x + dx, y - dy);
	}

	/*画椭圆两侧部分*/
	d2 = b * b * (dx + 0.5) * (dx + 0.5) + a * a * (dy - 1) * (dy - 1) - a * a * b * b;

	while (dy > 0)
	{
		if (d2 <= 0)		//下一个点在当前点东方
		{
			d2 += b * b * (2 * dx + 2) + a * a * (-2 * dy + 3);
			dx++;

		}
		else				//下一个点在当前点东南方
		{
			d2 += a * a * (-2 * dy + 3);
		}
		dy--;

		if (is_filled)	//指定椭圆填充
		{
			/*遍历两侧部分*/
			for (j = -dy; j < dy; j++)
			{
				/*在指定区域画点，填充部分椭圆*/
				oled_draw_point(x + dx, y + j);
				oled_draw_point(x - dx, y + j);
			}
		}

		/*画椭圆两侧部分圆弧*/
		oled_draw_point(x + dx, y + dy);
		oled_draw_point(x - dx, y - dy);
		oled_draw_point(x - dx, y + dy);
		oled_draw_point(x + dx, y - dy);
	}
}

void oled_draw_arc(int16_t x, int16_t y, int16_t radius, int16_t start_angle, int16_t end_angle, bool is_filled)
{
	int16_t dx, dy, d, j;
	if (radius <= 0)
	{
		return; //半径为0，直接返回
	}
	/*此函数借用Bresenham算法画圆的方法*/

	d = 1 - radius;
	dx = 0;
	dy = radius;

	/*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
	if (oled_is_in_angle(dx, dy, start_angle, end_angle))
	{
		oled_draw_point(x + dx, y + dy);
	}
	if (oled_is_in_angle(-dx, -dy, start_angle, end_angle))
	{
		oled_draw_point(x - dx, y - dy);
	}
	if (oled_is_in_angle(dy, dx, start_angle, end_angle))
	{
		oled_draw_point(x + dy, y + dx);
	}
	if (oled_is_in_angle(-dy, -dx, start_angle, end_angle))
	{
		oled_draw_point(x - dy, y - dx);
	}

	if (is_filled)	//指定圆弧填充
	{
		/*遍历起始点Y坐标*/
		for (j = -dy; j < dy; j++)
		{
			/*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
			if (oled_is_in_angle(0, j, start_angle, end_angle))
			{
				oled_draw_point(x, y + j);
			}
		}
	}

	while (dx < dy)		//遍历X轴的每个点
	{
		dx++;
		if (d < 0)		//下一个点在当前点东方
		{
			d += 2 * dx + 1;
		}
		else			//下一个点在当前点东南方
		{
			dy--;
			d += 2 * (x - dy) + 1;
		}

		/*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
		if (oled_is_in_angle(dx, dy, start_angle, end_angle))
		{
			oled_draw_point(x + dx, y + dy);
		}
		if (oled_is_in_angle(dy, dx, start_angle, end_angle))
		{
			oled_draw_point(x + dy, y + dx);
		}
		if (oled_is_in_angle(-dx, -dy, start_angle, end_angle))
		{
			oled_draw_point(x - dx, y - dy);
		}
		if (oled_is_in_angle(-dy, -dx, start_angle, end_angle))
		{
			oled_draw_point(x - dy, y - dx);
		}
		if (oled_is_in_angle(dx, -dy, start_angle, end_angle))
		{
			oled_draw_point(x + dx, y - dy);
		}
		if (oled_is_in_angle(dy, -dx, start_angle, end_angle))
		{
			oled_draw_point(x + dy, y - dx);
		}
		if (oled_is_in_angle(-dx, dy, start_angle, end_angle))
		{
			oled_draw_point(x - dx, y + dy);
		}
		if (oled_is_in_angle(-dy, dx, start_angle, end_angle))
		{
			oled_draw_point(x - dy, y + dx);
		}

		if (is_filled)	//指定圆弧填充
		{
			/*遍历中间部分*/
			for (j = -dy; j < dy; j++)
			{
				/*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
				if (oled_is_in_angle(dx, j, start_angle, end_angle))
				{
					oled_draw_point(x + dx, y + j);
				}
				if (oled_is_in_angle(-dx, j, start_angle, end_angle))
				{
					oled_draw_point(x - dx, y + j);
				}
			}

			/*遍历两侧部分*/
			for (j = -dy; j < dy; j++)
			{
				/*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
				if (oled_is_in_angle(-dy, j, start_angle, end_angle))
				{
					oled_draw_point(x - dy, y + j);
				}
				if (oled_is_in_angle(dy, j, start_angle, end_angle))
				{
					oled_draw_point(x + dy, y + j);
				}
			}
		}
	}
}

void oled_draw_rounded_rectangle(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius, bool is_filled)
{
	// 安全检查
	if (width == 0 || height == 0)
	{
		return;  // 宽度或高度为0,直接返回
	}
	if (radius > width / 2 || radius > height / 2)
	{
		radius = (width < height ? width : height) / 2;  // 限制圆角半径不超过宽高的一半
	}

	if (radius <= 0)
	{
		oled_draw_rectangle(x, y, width, height, is_filled);  // 如果半径为0,退化为普通矩形
		return;
	}
	else if (radius == 2)
	{	// 如果半径为2，圆角可以简化为两个点，优化性能
		oled_draw_point(x + 1, y);
		oled_draw_point(x + width - 2, y);

		oled_draw_point(x + 1, y + height - 1);
		oled_draw_point(x + width - 2, y + height - 1);

		oled_draw_point(x, y + 1);
		oled_draw_point(x, y + height - 2);

		oled_draw_point(x + width - 1, y + 1);
		oled_draw_point(x + width - 1, y + height - 2);
	}
	else
	{
		// 绘制四个圆角
		oled_draw_arc(x + radius, y + radius, radius, 180, -90, is_filled);
		oled_draw_arc(x + width - radius - 1, y + radius, radius, -90, 0, is_filled);
		oled_draw_arc(x + radius, y + height - radius - 1, radius, 90, 180, is_filled);
		oled_draw_arc(x + width - radius - 1, y + height - radius - 1, radius, 0, 90, is_filled);
	}

	// 填充或绘制矩形主体
	if (is_filled)
	{
		oled_draw_rectangle(x + radius, y, width - 2 * radius, height, true);
		oled_draw_rectangle(x, y + radius, width, height - 2 * radius, true);
	}
	else
	{
		// 绘制顶部和底部的直线
		oled_draw_line(x + radius, y, x + width - radius - 1, y);
		oled_draw_line(x + radius, y + height - 1, x + width - radius - 1, y + height - 1);
		// 绘制左侧和右侧的直线
		oled_draw_line(x, y + radius, x, y + height - radius - 1);
		oled_draw_line(x + width - 1, y + radius, x + width - 1, y + height - radius - 1);
	}
}

