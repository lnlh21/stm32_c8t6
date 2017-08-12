/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : drv_init.c
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��12��18��
  ����޸�   :
  ��������   : ������ʼ��
  �����б�   :
              DRV_Init
  �޸���ʷ   :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos.h"
#include "stm32f10x.h"
#if DESC("�ж�")
ULONG BSP_InterruptInit()
{
	NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;

	/* �����ж�������ַ */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	/* �������ȼ� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

#if 0
    /* ��̫���ж����� */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
#endif

	/* �����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);    

#if 0
	/* Usart3 */
	NVIC_InitStructure.NVIC_IRQChannel					 = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
   // NVIC_Init(&NVIC_InitStructure);	 

	/* Enable the EXTI8 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel					 = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);
#endif 
}
#endif

#if DESC("���")

void BSP_LedInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIOC->BRR = GPIO_Pin_13;
}

void BSP_ApiLedCtrl(ULONG ulNo, ULONG ulSw)
{
    if (ulSw == PUB_ENABLE)
    {
        if (ulNo == 0)
        {
            GPIOC->BSRR = GPIO_Pin_13;
        }
    }
    else if (ulSw == PUB_DISABLE)
    {
        if (ulNo == 0)
        {
            GPIOC->BRR = GPIO_Pin_13;
        }
    }
}

#endif

#if DESC("SPI")

/*   STM32F103 SPI�ӿڶ���
     PA4  ---  CS
     PA5  ---  SCK
     PA6  ---  MISO
     PA7  ---  MOSI
*/

ULONG BSP_SpiInit()
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* SPI GPIO CONFIG */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    /* SPI CONFIG */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
    SPI_Cmd(SPI1, ENABLE);

}

UCHAR BSP_ApiSpiRead(ULONG ulDevId)
{
    /* Wait for SPI1 Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    
    /* Send SPI1 data */
    SPI_I2S_SendData(SPI1, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);

}

UCHAR BSP_ApiSpiWrite(ULONG ulDevId, UCHAR ucData)
{
    /* Wait for SPI1 Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    
    /* Send SPI1 data */
    SPI_I2S_SendData(SPI1, ucData);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);

}

#endif

#if DESC("UART")
/*****************************************************************************
 �� �� ��  : DRV_UsartInit
 ��������  : ���ڳ�ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
void BSP_UartInit()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* �������� */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* ����1Tx Gpio */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����1Rx Gpio */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_Init(USART1, &USART_InitStructure);

    /* �������� */
    USART_Cmd(USART1, ENABLE);

    /* �����ж� */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
}

void BSP_ApiUartPutc(UCHAR ch)
{
    /* ������д�� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
  
    /* ���� */
    USART_SendData(USART1, (uint8_t)ch);
}

#endif

#if DESC("��ʼ��")
/*****************************************************************************
 �� �� ��  : BSP_Init
 ��������  : BSP��ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID BSP_ApiInit()
{
    RCC_ClocksTypeDef RCC_Clocks;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* ����Դģ��Ŀ��� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2 | 
		                   RCC_APB1Periph_I2C2 | RCC_AHBPeriph_DMA1,  ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                              RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1 |
                              RCC_APB2Periph_AFIO, ENABLE);

    /* TICK��ʼ�� */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.SYSCLK_Frequency / OS_TICKS_PER_SEC);
    
    /* ��ʼ���� */
    BSP_LedInit();

    /* ��ʼ������ */
    BSP_UartInit();
   
    /* ��ʼ���ж� */
    BSP_InterruptInit();
}
#endif

