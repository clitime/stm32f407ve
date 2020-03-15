#include <tim6.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"


void tim6_configure (void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // включаем тактирование таймера

    TIM_TimeBaseInitTypeDef timer;
    TIM_TimeBaseStructInit(&timer);

    timer.TIM_Prescaler = 8400 - 1;
    timer.TIM_Period = 10 - 1;
    TIM_TimeBaseInit(TIM6, &timer);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);

    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
