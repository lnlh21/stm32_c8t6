#include "public_def.h"
#include "stm32f10x.h"

void Set_USB(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* 以PLL时钟的1.5倍分频做为USB时钟 72/1.5=48 */
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

    /* 打开USB时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

    /* Enable the USB interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USB Wake-up interrupt */
    //NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    //NVIC_Init(&NVIC_InitStructure);  

    /* D2连接USB */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* D2初始化 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIOD->BRR = GPIO_Pin_2;
}


