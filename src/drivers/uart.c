#include "uart.h"

#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

#include <string.h>

#include "delay.h"


uint8_t transmitBuffer[TRANSMIT_BUFFER_SIZE];


static const uint32_t baud_rate[] = {
#define XX(speed, ix) speed,
    UART_MAP_SPEED(XX)
#undef XX
        9600
};


static void uartio_init(void) {
    GPIO_InitTypeDef init;

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART1);
    /*tx init*/
    GPIO_StructInit(&init);
    init.GPIO_Pin = GPIO_Pin_9;
    init.GPIO_Mode = GPIO_Mode_AF;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_PuPd = GPIO_PuPd_UP;
    init.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &init);
    /*rx init*/
    GPIO_StructInit(&init);
    init.GPIO_Pin = GPIO_Pin_10;
    init.GPIO_Mode = GPIO_Mode_AF;
    init.GPIO_PuPd = GPIO_PuPd_UP;
    init.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &init);
    /*rtc initialize*/
    GPIO_StructInit(&init);
    init.GPIO_Pin = GPIO_Pin_12;
    init.GPIO_Mode = GPIO_Mode_OUT;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_PuPd = GPIO_PuPd_UP;
    init.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &init);
}


void uart_init(uint8_t ix_speed) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    uartio_init();

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baud_rate[ix_speed];
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);

    NVIC_SetPriority(USART1_IRQn, 1);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);

    NVIC_EnableIRQ(USART1_IRQn);
    USART_Cmd(USART1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low; // DMA_Priority_High;
    /* Configure TX DMA */
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)transmitBuffer;
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);

    NVIC_SetPriority(DMA2_Stream7_IRQn, 1);
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);
    DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);

    delay_msec(5);
}


uint32_t getTimeoutRecv(uint8_t ix_speed) {
    return 10000000 / baud_rate[ix_speed];
}


void put_into_transmit_buffer(const uint8_t *data, uint32_t len) {
    if (len > TRANSMIT_BUFFER_SIZE) {
        return;
    }
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    rts_on();
    delay_msec(15);
    memcpy(transmitBuffer, data, len);

    DMA2_Stream7->NDTR = len;
    DMA_Cmd(DMA2_Stream7, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}
