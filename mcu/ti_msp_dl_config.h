/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G351X
#define CONFIG_MSPM0G3519

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000



/* Defines for DEFAULT_UART */
#define DEFAULT_UART_INST                                                  UART0
#define DEFAULT_UART_INST_FREQUENCY                                     40000000
#define DEFAULT_UART_INST_IRQHandler                            UART0_IRQHandler
#define DEFAULT_UART_INST_INT_IRQN                                UART0_INT_IRQn
#define GPIO_DEFAULT_UART_RX_PORT                                          GPIOA
#define GPIO_DEFAULT_UART_TX_PORT                                          GPIOA
#define GPIO_DEFAULT_UART_RX_PIN                                  DL_GPIO_PIN_11
#define GPIO_DEFAULT_UART_TX_PIN                                  DL_GPIO_PIN_10
#define GPIO_DEFAULT_UART_IOMUX_RX                               (IOMUX_PINCM22)
#define GPIO_DEFAULT_UART_IOMUX_TX                               (IOMUX_PINCM21)
#define GPIO_DEFAULT_UART_IOMUX_RX_FUNC                IOMUX_PINCM22_PF_UART0_RX
#define GPIO_DEFAULT_UART_IOMUX_TX_FUNC                IOMUX_PINCM21_PF_UART0_TX
#define DEFAULT_UART_BAUD_RATE                                          (115200)
#define DEFAULT_UART_IBRD_40_MHZ_115200_BAUD                                (21)
#define DEFAULT_UART_FBRD_40_MHZ_115200_BAUD                                (45)




/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI0
#define SPI_0_INST_IRQHandler                                   SPI0_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI0_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                       DL_GPIO_PIN_2
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM15)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM15_PF_SPI0_PICO
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOB
#define GPIO_SPI_0_SCLK_PIN                                       DL_GPIO_PIN_3
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM16)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM16_PF_SPI0_SCLK



/* Defines for L1: GPIOA.14 with pinCMx 36 on package pin 53 */
#define LED_L1_PORT                                                      (GPIOA)
#define LED_L1_PIN                                              (DL_GPIO_PIN_14)
#define LED_L1_IOMUX                                             (IOMUX_PINCM36)
/* Defines for L2: GPIOA.17 with pinCMx 39 on package pin 69 */
#define LED_L2_PORT                                                      (GPIOA)
#define LED_L2_PIN                                              (DL_GPIO_PIN_17)
#define LED_L2_IOMUX                                             (IOMUX_PINCM39)
/* Defines for L3: GPIOB.26 with pinCMx 57 on package pin 96 */
#define LED_L3_PORT                                                      (GPIOB)
#define LED_L3_PIN                                              (DL_GPIO_PIN_26)
#define LED_L3_IOMUX                                             (IOMUX_PINCM57)
/* Defines for L4: GPIOB.28 with pinCMx 65 on package pin 29 */
#define LED_L4_PORT                                                      (GPIOB)
#define LED_L4_PIN                                              (DL_GPIO_PIN_28)
#define LED_L4_IOMUX                                             (IOMUX_PINCM65)
/* Defines for L5: GPIOB.29 with pinCMx 66 on package pin 30 */
#define LED_L5_PORT                                                      (GPIOB)
#define LED_L5_PIN                                              (DL_GPIO_PIN_29)
#define LED_L5_IOMUX                                             (IOMUX_PINCM66)
/* Defines for L6: GPIOB.30 with pinCMx 67 on package pin 31 */
#define LED_L6_PORT                                                      (GPIOB)
#define LED_L6_PIN                                              (DL_GPIO_PIN_30)
#define LED_L6_IOMUX                                             (IOMUX_PINCM67)
/* Defines for L7: GPIOC.6 with pinCMx 84 on package pin 78 */
#define LED_L7_PORT                                                      (GPIOC)
#define LED_L7_PIN                                               (DL_GPIO_PIN_6)
#define LED_L7_IOMUX                                             (IOMUX_PINCM84)
/* Defines for L8: GPIOC.7 with pinCMx 85 on package pin 79 */
#define LED_L8_PORT                                                      (GPIOC)
#define LED_L8_PIN                                               (DL_GPIO_PIN_7)
#define LED_L8_IOMUX                                             (IOMUX_PINCM85)
/* Port definition for Pin Group KEYBOARD */
#define KEYBOARD_PORT                                                    (GPIOB)

/* Defines for H1: GPIOB.6 with pinCMx 23 on package pin 40 */
#define KEYBOARD_H1_PIN                                          (DL_GPIO_PIN_6)
#define KEYBOARD_H1_IOMUX                                        (IOMUX_PINCM23)
/* Defines for H2: GPIOB.7 with pinCMx 24 on package pin 41 */
#define KEYBOARD_H2_PIN                                          (DL_GPIO_PIN_7)
#define KEYBOARD_H2_IOMUX                                        (IOMUX_PINCM24)
/* Defines for H3: GPIOB.8 with pinCMx 25 on package pin 42 */
#define KEYBOARD_H3_PIN                                          (DL_GPIO_PIN_8)
#define KEYBOARD_H3_IOMUX                                        (IOMUX_PINCM25)
/* Defines for H4: GPIOB.9 with pinCMx 26 on package pin 43 */
#define KEYBOARD_H4_PIN                                          (DL_GPIO_PIN_9)
#define KEYBOARD_H4_IOMUX                                        (IOMUX_PINCM26)
/* Defines for V1: GPIOB.20 with pinCMx 48 on package pin 82 */
#define KEYBOARD_V1_PIN                                         (DL_GPIO_PIN_20)
#define KEYBOARD_V1_IOMUX                                        (IOMUX_PINCM48)
/* Defines for V2: GPIOB.24 with pinCMx 52 on package pin 86 */
#define KEYBOARD_V2_PIN                                         (DL_GPIO_PIN_24)
#define KEYBOARD_V2_IOMUX                                        (IOMUX_PINCM52)
/* Defines for V3: GPIOB.25 with pinCMx 56 on package pin 95 */
#define KEYBOARD_V3_PIN                                         (DL_GPIO_PIN_25)
#define KEYBOARD_V3_IOMUX                                        (IOMUX_PINCM56)
/* Defines for V4: GPIOB.27 with pinCMx 58 on package pin 97 */
#define KEYBOARD_V4_PIN                                         (DL_GPIO_PIN_27)
#define KEYBOARD_V4_IOMUX                                        (IOMUX_PINCM58)
/* Defines for DC: GPIOC.8 with pinCMx 86 on package pin 80 */
#define OLED_DC_PORT                                                     (GPIOC)
#define OLED_DC_PIN                                              (DL_GPIO_PIN_8)
#define OLED_DC_IOMUX                                            (IOMUX_PINCM86)
/* Defines for CS: GPIOC.9 with pinCMx 87 on package pin 81 */
#define OLED_CS_PORT                                                     (GPIOC)
#define OLED_CS_PIN                                              (DL_GPIO_PIN_9)
#define OLED_CS_IOMUX                                            (IOMUX_PINCM87)
/* Defines for RES: GPIOB.23 with pinCMx 51 on package pin 85 */
#define OLED_RES_PORT                                                    (GPIOB)
#define OLED_RES_PIN                                            (DL_GPIO_PIN_23)
#define OLED_RES_IOMUX                                           (IOMUX_PINCM51)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_DEFAULT_UART_init(void);
void SYSCFG_DL_SPI_0_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
