#include "led.h"

GPIO_Regs* LED_PORTS[NUM_LEDS] = LED_PORTS_VALUES;

const uint32_t LED_PINS[NUM_LEDS] = LED_PINS_VALUES;

uint8_t led_states = 0x00;

void set_led_state(uint8_t leds, bool state)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (leds & (1 << i))
        {
            if (state)
            {
                DL_GPIO_clearPins(LED_PORTS[i], LED_PINS[i]);
                led_states |= (1 << i);
            }
            else
            {
                DL_GPIO_setPins(LED_PORTS[i], LED_PINS[i]);
                led_states &= ~(1 << i);
            }
        }
    }
}

void toggle_led(uint8_t leds)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (leds & (1 << i))
        {
            if (led_states & (1 << i))
            {
                DL_GPIO_setPins(LED_PORTS[i], LED_PINS[i]);
                led_states &= ~(1 << i);
            }
            else
            {
                DL_GPIO_clearPins(LED_PORTS[i], LED_PINS[i]);
                led_states |= (1 << i);
            }
        }
    }
}
