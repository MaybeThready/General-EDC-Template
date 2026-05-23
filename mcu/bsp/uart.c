#include "uart.h"

UartReceiveCallbackFunc uart_recieve_callback = NULL;

void init_uart()
{
    DL_UART_enableInterrupt(UART_INST, DL_UART_MAIN_IIDX_RX);
    NVIC_EnableIRQ(DEFAULT_UART_INST_INT_IRQN);
}

void uart_send_byte(uint8_t data, bool blocking)
{
    if (blocking)
    {
        DL_UART_transmitDataBlocking(UART_INST, data);
    }
    else
    {
        DL_UART_transmitData(UART_INST, data);
    }
}

void uart_send_char(char data, bool blocking)
{
    uart_send_byte((uint8_t)data, blocking);
    uart_send_byte((uint8_t)'\n', blocking);
}

void uart_send_string(const char* str, bool blocking)
{
    while (*str)
    {
        uart_send_byte((uint8_t)*str++, blocking);
    }
    uart_send_byte((uint8_t)'\n', blocking);
}

void uart_send_unum(uint32_t num, bool blocking)
{
    char buffer[11];
    int i = 0;

    if (num == 0)
    {
        uart_send_byte('0', blocking);
        return;
    }

    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i--)
    {
        uart_send_byte(buffer[i], blocking);
    }
    uart_send_byte((uint8_t)'\n', blocking);
}

void uart_send_num(int32_t num, bool blocking)
{
    if (num < 0)
    {
        uart_send_byte('-', blocking);
        num = -num;
    }
    uart_send_unum((uint32_t)num, blocking);
}

void DEFAULT_UART_INST_IRQHandler()
{
    switch (DL_UART_Main_getPendingInterrupt(UART_INST))
    {
    case DL_UART_MAIN_IIDX_RX:
        if (uart_recieve_callback)
        {
            uart_recieve_callback(DL_UART_Main_receiveData(UART_INST));
        }
        break;
    
    default:
        break;
    }
}
