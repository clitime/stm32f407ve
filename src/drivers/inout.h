#ifndef INOUT_H_
#define INOUT_H_


#include <stdint.h>
#include "stm32f4xx_gpio.h"


enum io_state {
    io_reset = 0,
    io_set,
};


    /*  name         port     pin             mode          pu_pd     */
#define IN_MAP(XX)                                                            \
    XX(BOOT,        GPIOD,  GPIO_Pin_2,     GPIO_Mode_IN, GPIO_PuPd_UP)       \
    XX(DEF_NET,     GPIOD,  GPIO_Pin_3,     GPIO_Mode_IN, GPIO_PuPd_UP)       \
    XX(DEF_PARAM,   GPIOD,  GPIO_Pin_4,     GPIO_Mode_IN, GPIO_PuPd_UP)       \
    XX(TAMPER,      GPIOD,  GPIO_Pin_14,    GPIO_Mode_IN, GPIO_PuPd_UP)


/*    name         port       pin          mode           o_type              pu_pd             speed         state  */
#define OUT_MAP(XX)                                                                                                    \
    XX(LED_3,     GPIOD,  GPIO_Pin_3,  GPIO_Mode_OUT,  GPIO_OType_PP,  GPIO_PuPd_UP,    GPIO_High_Speed,  io_reset) \
    XX(LED_4,     GPIOD,  GPIO_Pin_4,  GPIO_Mode_OUT,  GPIO_OType_PP,  GPIO_PuPd_UP,    GPIO_High_Speed,  io_reset)


enum input_map {
    #define XX(name, port, pin, mode, pupd) IN_##name,
        IN_MAP(XX)
    #undef XX
    IN_NULL,
};

enum output_map {
    #define XX(name, port, pin, mode, type, pupd, speed, state) OUT_##name,
        OUT_MAP(XX)
    #undef XX
    OUT_NULL,
};


void config_output(void);
void config_input(void);

uint8_t get_input_state(enum input_map name);
void set_output_state(enum output_map name, enum io_state state);
uint8_t get_output_state(enum output_map name);
void toggle_output_state(enum output_map name);

#endif
