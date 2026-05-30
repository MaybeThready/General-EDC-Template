#include "ti_msp_dl_config.h"
#include "bsp/msp_sys.h"
#include "bsp/keyboard.h"
#include "ui_config.h"

int main()
{
    SYSCFG_DL_init();
    init_sys();
    init_oled();
    init_ui();
    init_keyboard();

    while (1)
    {
        keyboard_update();
        ui_update();
    }

    return 0;
}
