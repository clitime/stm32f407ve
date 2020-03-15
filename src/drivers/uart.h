#ifndef _UART_H_
#define _UART_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"


enum {
#ifndef TRANSMIT_BUFFER_SIZE
    TRANSMIT_BUFFER_SIZE = 256
#endif
};


#define rts_off() (GPIOA->BSRRH = GPIO_Pin_12)
#define rts_on() (GPIOA->BSRRL = GPIO_Pin_12)


#define UART_MAP_SPEED(XX)                                                     \
    XX(600, 0)                                                                 \
    XX(1200, 1)                                                                \
    XX(2400, 2)                                                                \
    XX(4800, 3)                                                                \
    XX(9600, 4)                                                                \
    XX(19200, 5)                                                               \
    XX(38400, 6)                                                               \
    XX(57600, 7)                                                               \
    XX(115200, 8)


enum {
#define XX(speed, ix) UART_##speed = ix,
    UART_MAP_SPEED(XX)
#undef XX
    UART_DEFAULT = 8,
};


void uart_init(uint8_t ix_speed);
uint32_t get_timeout_recv_byte(uint8_t ixspeed);
void put_into_transmit_buffer(const uint8_t *data, uint32_t len);


#endif
