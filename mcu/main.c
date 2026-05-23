#include "ti_msp_dl_config.h"
#include "bsp/msp_sys.h"
#include "bsp/oled.h"
#include "bsp/keyboard.h"
#include "bsp/led.h"
#include <string.h>


int main()
{
    SYSCFG_DL_init();
    init_sys();
    init_oled();
    init_keyboard();

    char test_str[] = "Hello, World!";

    while (1)
    {
        keyboard_update();
        if (keyboard_keys[0][0].state_event == KEY_ON)
        {
            strcpy(test_str, "World, Hello!");
        }
        else
        {
            strcpy(test_str, "Hello, World!");
        }
        oled_show_string(0, 0, test_str);
        delay_cycles(TIME_MS(10));
    }

    return 0;
}
