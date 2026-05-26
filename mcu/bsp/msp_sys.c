#include "msp_sys.h"
#include <ti/driverlib/m0p/dl_systick.h>

volatile uint32_t sys_tick = 0;

void init_sys()
{
    sys_tick = 0;
    DL_SYSTICK_config(SYS_TICK_PERIOD);
    DL_SYSTICK_enableInterrupt();
    DL_SYSTICK_enable();
}

void SysTick_Handler(void)
{
    ++sys_tick;
}

void delay_us(uint32_t us)
{
    uint32_t cycles = TIME_US(us);
    while (cycles-- > 0)
    {
        __NOP();
    }
}

void delay_ms(uint32_t ms)
{
    uint32_t start = sys_tick;
    while ((uint32_t)(sys_tick - start) < ms)
    {
        __NOP();
    }
}

void delay_s(uint32_t s)
{
    while (s-- > 0)
    {
        delay_ms(1000);
    }
}

void HardFault_Handler(void)
{
    __BKPT(0);
}
