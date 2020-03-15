#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rtc.h"
#include "misc.h"


#ifdef BOOTLOADER
    #define VECT_TAB_OFFSET  0x00000
#else
    #define VECT_TAB_OFFSET  0x00000
#endif


enum {
    PLLP = 2,
    PLLQ = 4,
    PLLM = 25,
    PLLN = 336,
};

void SystemInit(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  //set CP10 and CP11 Full Access
#endif
//--------------------------------------------------------
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN);
    FLASH_SetLatency(FLASH_Latency_5);
    FLASH_PrefetchBufferCmd(ENABLE);
    FLASH_InstructionCacheCmd(ENABLE);
    FLASH_DataCacheCmd(ENABLE);
    FLASH_InstructionCacheReset();
    FLASH_DataCacheReset();

    PWR_MainRegulatorModeConfig(PWR_Regulator_Voltage_Scale1);
    RCC_DeInit();

    RCC_HSEConfig(RCC_HSE_ON);
    if (RCC_WaitForHSEStartUp() == ERROR) {
        /* init hsi*/
        while(1) {};
    }
    /*AHB clock HCLK = 168 MHz*/
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    /*APB1 clock PCLK1 = 42 MHz, APB1 tim clock = 84 MHz*/
    RCC_PCLK1Config(RCC_HCLK_Div4);
    /*APB2 clock PCLK2 = 84 MHz, APB1 tim clock = 168 MHz*/
    RCC_PCLK2Config(RCC_HCLK_Div2);

    RCC_PLLConfig(RCC_PLLSource_HSE, PLLM, PLLN, PLLP, PLLQ);

    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08 );
//--------------------------------------------------------
    enum {
        ms_10 = 100,
        ms_1 = 1000,
        ms_01 = 10000,
    };
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    if (SysTick_Config(168000000ULL / ms_1)) {
        while(1);
    }

    NVIC_SetPriority(SysTick_IRQn, 0x14);
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, VECT_TAB_OFFSET);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//--------------------------------------------------------
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA
                          | RCC_AHB1Periph_GPIOB
                          | RCC_AHB1Periph_GPIOC
                          | RCC_AHB1Periph_GPIOD
                          | RCC_AHB1Periph_GPIOE
                          | RCC_AHB1Periph_GPIOF
                          | RCC_AHB1Periph_GPIOG, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    __enable_irq();
}
