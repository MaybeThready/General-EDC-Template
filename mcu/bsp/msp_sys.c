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
