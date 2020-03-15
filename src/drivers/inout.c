#include <stddef.h>
#include "inout.h"


struct gpio {
    GPIO_TypeDef *port;
    uint32_t pin;
};


static struct gpio output[] = {
    #define XX(name, port, pin, mode, type, pull, speed, state)               \
        {port, pin},
    OUT_MAP(XX)
    {NULL, 0}
    #undef XX
};

static struct gpio input[] = {
    #define XX(name, port, pin, mode, pupd) \
        {port, pin},
    IN_MAP(XX)
    {NULL, 0}
    #undef XX
};


void config_output(void) {
    GPIO_InitTypeDef init = {0};

    #define XX(name, port, pin, mode, type, pull, speed, state) \
        init.GPIO_Pin = pin;                                    \
        init.GPIO_Mode = mode;                                  \
        init.GPIO_OType = type;                                 \
        init.GPIO_PuPd = pull;                                  \
        init.GPIO_Speed = speed;                                \
        GPIO_Init(port, &init);                                 \
        state == io_set ? GPIO_SetBits(port, pin) : GPIO_ResetBits(port, pin);
    OUT_MAP(XX)
    #undef XX
    (void)init;
}


void config_input(void) {
    GPIO_InitTypeDef init = {0};

    #define XX(name, port, pin, mode, pupd)                     \
        init.GPIO_Pin = pin;                                    \
        init.GPIO_Mode = mode;                                  \
        init.GPIO_PuPd = pupd;                                  \
        GPIO_Init(port, &init);                                 \
    IN_MAP(XX)
    #undef XX
    (void)init;
}


uint8_t get_input_state(enum input_map name) {
    return GPIO_ReadInputDataBit(input[name].port, input[name].pin);
}


void set_output_state(enum output_map name, enum io_state state) {
    if (state == io_set) {
        GPIO_SetBits(output[name].port, output[name].pin);
    } else {
        GPIO_ResetBits(output[name].port, output[name].pin);
    }
}


uint8_t get_output_state(enum output_map name) {
    return GPIO_ReadOutputDataBit(output[name].port, output[name].pin);
}


void toggle_output_state(enum output_map name) {
    if (get_output_state(name)) {
        set_output_state(name, io_reset);
    } else {
        set_output_state(name, io_set);
    }
}
