/*
 * UART驱动模块，提供UART初始化和数据发送功能
 *对接收数据进行处理时，需注册UartReceiveCallbackFunc回调函数
 */

#pragma once

#include "../ti_msp_dl_config.h"

typedef void (*UartReceiveCallbackFunc)(uint8_t data);

// UART实例
#define UART_INST DEFAULT_UART_INST

// UART接收回调函数指针，用户可以将其设置为一个函数来处理接收到的数据
extern UartReceiveCallbackFunc uart_recieve_callback;

/**
 *@brief 初始化UART模块，配置UART参数并使能接收中断
 * 
 */
void init_uart();

/**
 *@brief 发送一个字节数据到UART，支持阻塞和非阻塞模式
 * 
 * @param data 要发送的数据字节
 * @param blocking 是否使用阻塞模式发送，true表示阻塞，false表示非阻塞
 */
void uart_send_byte(uint8_t data, bool blocking);

/**
 *@brief 发送一个字符到UART，支持阻塞和非阻塞模式
 * 
 * @param data 要发送的字符
 * @param blocking 是否使用阻塞模式发送，true表示阻塞，false表示非阻塞
 */
void uart_send_char(char data, bool blocking);

/**
 *@brief 发送一个字符串到UART，支持阻塞和非阻塞模式
 * 
 * @param str 要发送的字符串指针
 * @param blocking 是否使用阻塞模式发送，true表示阻塞，false表示非阻塞
 */
void uart_send_string(const char* str, bool blocking);

/**
 *@brief 发送一个无符号整数到UART，支持阻塞和非阻塞模式
 * 
 * @param num 要发送的无符号整数
 * @param blocking 是否使用阻塞模式发送，true表示阻塞，false表示非阻塞
 */
void uart_send_unum(uint32_t num, bool blocking);

/**
 *@brief 发送一个有符号整数到UART，支持阻塞和非阻塞模式
 * 
 * @param num 要发送的有符号整数
 * @param blocking 是否使用阻塞模式发送，true表示阻塞，false表示非阻塞
 */
void uart_send_num(int32_t num, bool blocking);

void DEFAULT_UART_INST_IRQHandler();
