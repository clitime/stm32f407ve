#include "stm32f4xx_it.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_usart.h"

#include "delay.h"
#include "timers.h"
#include "uart.h"
#include "uart_queue_manager.h"


void NMI_Handler(void) {
}


void HardFault_Handler(void) {
    while (1) {
        continue;
    }
}


void MemManage_Handler(void) {
    while (1) {
        continue;
    }
}


void BusFault_Handler(void) {
    while (1) {
        continue;
    }
}


void UsageFault_Handler(void) {
    while (1) {
        continue;
    }
}


void SysTick_Handler(void) {
    dec_delay_msec();
    timer_process();
}


void TIM6_DAC_IRQHandler() {
    if (TIM_GetITStatus(TIM6, TIM_IT_Update)) {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}


void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        uint8_t data = (uint8_t)USART_ReceiveData(USART1);
        queue_append(data);
    }

    if (USART_GetITStatus(USART1, USART_IT_ORE_RX)) {
        USART_ClearITPendingBit(USART1, USART_IT_ORE_RX);
        (void)USART_ReceiveData(USART1);
    }

    if (USART_GetITStatus(USART1, USART_IT_TC)) {
        rts_off();
        USART_ClearITPendingBit(USART1, USART_IT_TC);
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
    }
}


void DMA2_Stream7_IRQHandler(void) {
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7)) {
        DMA_Cmd(DMA2_Stream7, DISABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
    }
}
