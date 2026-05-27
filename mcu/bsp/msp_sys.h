/**
 *@file msp_sys.h
 * @author Thready
 * @brief 系统模块
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "../ti_msp_dl_config.h"

// 定义时间转换宏，单位为系统时钟节拍数
#define TIME_US(us) ((CPUCLK_FREQ / 1000000) * (us))
#define TIME_MS(ms) ((CPUCLK_FREQ / 1000) * (ms))
#define TIME_S(s) ((CPUCLK_FREQ) * (s))

// 系统时钟节拍周期，单位为系统时钟节拍数
#define SYS_TICK_PERIOD TIME_MS(1)

// 系统时钟节拍计数器，每当系统时钟节拍中断发生时递增
extern volatile uint32_t sys_tick;

// 初始化系统时钟节拍器，设置节拍周期并使能中断
void init_sys();

// 系统时钟节拍中断处理函数，每当系统时钟节拍中断发生时调用，递增系统时钟节拍计数器
void SysTick_Handler(void);

// 微秒级阻塞延时
void delay_us(uint32_t us);

// 毫秒级阻塞延时
void delay_ms(uint32_t ms);

// 秒级阻塞延时
void delay_s(uint32_t s);

// HardFault异常处理函数，系统发生严重错误时调用，进入死循环以便调试
void HardFault_Handler(void);
