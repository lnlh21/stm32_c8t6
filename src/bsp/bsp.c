/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_init.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月18日
  最近修改   :
  功能描述   : 驱动初始化
  函数列表   :
              DRV_Init
  修改历史   :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "vos.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include "bsp.h"
#include <stdio.h>

#if DESC("GPIO")
VOID BSP_GpioInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

VOID BSP_GpioSet(ULONG ulDevId, ULONG ulSw)
{
	if (BSP_GPIO_OUT_2401_CE == ulDevId)
	{
		if (ulSw)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_3);
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		}
	}
	else if (BSP_GPIO_OUT_2401_CSN == ulDevId)
	{
		if (ulSw)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_4);
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		}
	}
}

ULONG BSP_GpioRead(ULONG ulDevId)
{
    if (BSP_GPIO_IN_2401_IRQ == ulDevId)
    {
		return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
    }
}

#endif

#if DESC("CPU延时")
/*****************************************************************************
 函 数 名  : TIME_ApiDeLayMs
 功能描述  : 延时函数，大概为毫秒级，不精确
 输入参数  : USHORT usTime  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年10月10日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID BSP_ApiDeLayMs(USHORT usTime)
{
    ULONG i;
    ULONG j;
    
    for (i = 0; i < usTime; i++)
    {
        for(j = 0; j < 12000; j++)
        {
            __asm ( "NOP" );
        }
    }
}

/*****************************************************************************
 函 数 名  : TIME_ApiDeLayUs
 功能描述  : 延时函数，大概为毫秒级，不精确
 输入参数  : USHORT usTime  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年10月10日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID BSP_ApiDeLayUs(USHORT usTime)
{
    ULONG i;
    ULONG j;
    
    for (i = 0; i < usTime; i++)
    {
        for(j = 0; j < 120; j++)
        {
            __asm ( "NOP" );
        }
    }
}

#endif

#if DESC("键盘")
void BSP_KeyInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* key */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; 

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /*
         if(0 == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))
        {
            if(w == 0)
            {
                w = 1;
            }
            else
            {
                w = 0;
            }
            printf("\r\n PIN 0");
            buf[0] = 0x06;
            buf[1] = w;
            //USB_SIL_Write(0x81, buf, 2);  
            //SetEPTxValid(1);
            
            for(i = 0; i < 500000; i++);
            {
                for(j = 0; j < 2000000; j++);
            }
            
        }
        
    }
    */
}

#endif

#if DESC("定时器")
#if 0
void TIM3_DMA_Config(void)
{

        DMA_InitTypeDef DMA_InitStructure;

    	DMA_DeInit(DMA1_Channel2);

        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)TIM2_CCR2_Address;     
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)LED_BYTE_Buffer;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
        DMA_InitStructure.DMA_BufferSize = 66;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		
        DMA_Init(DMA1_Channel2, &DMA_InitStructure);       
        DMA_Cmd (DMA1_Channel2,ENABLE); 

        //TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
        //DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
}


void Timer2_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* GPIOA Configuration: TIM3 Channel 1 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Compute the prescaler value */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 29; // 800kHz 
    TIM_TimeBaseStructure.TIM_Prescaler = 2;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 
    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
    /* configure DMA */
    /* TIM3 CC1 DMA Request enable */
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);

	TIM3_DMA_Config();
}
#endif
#endif

#if DESC("FLASH")

/*****************************************************************************
 函 数 名  : FLASH_ApiSaveData
 功能描述  : FLASH模块写入数据
 输入参数  : ULONG ulFlashAddr  
             UCHAR *pucSavebuf  
             ULONG ulLen        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG FLASH_ApiSaveData(ULONG ulFlashAddr, UCHAR *pucSavebuf, ULONG ulLen)
{
    ULONG ulPageStart;
    ULONG ulPageEnd;
    ULONG ulAddr;
    ULONG ulLenTemp;
    
    /* 地址合法性检查 */
#if 0
    if ((FLASH_API_DATA_ADDR_START > ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < (ulFlashAddr + ulLen)))
    {
        return PUB_ERR;
    }
#endif

    __disable_irq();

    /* 解锁FLASH */
    FLASH_Unlock();
    
    /* 计算要擦除的块 */
    ulPageStart = FLASH_API_GET_PAGE_ADDR(ulFlashAddr);
    ulPageEnd   = FLASH_API_GET_PAGE_ADDR(ulFlashAddr + ulLen);

    printf("%08x,%08x", ulPageStart, ulPageStart);
    /* 擦除FLASH */
    for (ulAddr = ulPageStart; ulAddr <= ulPageEnd; ulAddr+=FLASH_API_PAGE_SIZE)
    {
        printf("\r\n Flash E %08x", ulAddr);
        FLASH_ErasePage(ulAddr);
    }

    /* 编程FLASH */
    ulAddr = ulFlashAddr;
    for (ulLenTemp = 0; ulLenTemp < ulLen; ulLenTemp+=2)
    {
        FLASH_ProgramHalfWord(ulAddr, *(USHORT*)(pucSavebuf + ulLenTemp));
        ulAddr+=2;
    }
    
    /* 锁FLASH */
    FLASH_Lock();

    __enable_irq();

    return PUB_OK;
}


/*****************************************************************************
 函 数 名  : FLASH_ApiReadData
 功能描述  : FLASH模块读取数据
 输入参数  : ULONG ulFlashAddr  
             UCHAR *pucReadbuf  
             ULONG ulLen        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG FLASH_ApiReadData(ULONG ulFlashAddr, UCHAR *pucReadbuf, ULONG ulLen)
{
#if 0
    /* 地址合法性检查 */
    if ((FLASH_API_DATA_ADDR_START > ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < (ulFlashAddr + ulLen)))
    {
        return PUB_ERR;
    }
#endif

    /* 解锁FLASH */
    FLASH_Unlock();

    /* 读取 */
    memcpy(pucReadbuf, (VOID *)ulFlashAddr, ulLen);

    /* 锁FLASH */
    FLASH_Lock();

    return PUB_OK;
}

#endif

#if DESC("中断")
ULONG BSP_InterruptInit()
{
	NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;

	/* 设置中断向量地址 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	/* 设置优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

#if 0
    /* 以太网中断配置 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
#endif

	/* 串口中断 */
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

#if DESC("点灯")

void BSP_LedInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
#if (BOARD_TYPE_NEW == BOARD_TYPE)
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIOB->BRR = GPIO_Pin_12;

#else
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIOC->BRR = GPIO_Pin_13;
#endif
}

void BSP_ApiLedCtrl(ULONG ulNo, ULONG ulSw)
{
#if (BOARD_TYPE_NEW == BOARD_TYPE)
    if (ulSw == PUB_ENABLE)
    {
        if (ulNo == 0)
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_12);
        }
    }
    else if (ulSw == PUB_DISABLE)
    {
        if (ulNo == 0)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_12);
        }
    }
#else
	if (ulSw == PUB_ENABLE)
	{
		if (ulNo == 0)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		}
	}
	else if (ulSw == PUB_DISABLE)
	{
		if (ulNo == 0)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
		}
	}

#endif
}

#endif

#if DESC("SPI")

/*   STM32F103 SPI接口定义
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
    SPI_I2S_SendData(SPI1, 0xff);

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

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    /* 往串口写入 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }
  
    /* 发送数据到USART2 */
    USART_SendData(USART1, (uint8_t) ch);

    return ch;
}


/*****************************************************************************
 函 数 名  : DRV_UsartInit
 功能描述  : 串口初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
void BSP_UartInit()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* 串口配置 */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* 串口1Tx Gpio */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 串口1Rx Gpio */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_Init(USART1, &USART_InitStructure);

    /* 开启串口 */
    USART_Cmd(USART1, ENABLE);

    /* 开启中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
}

void BSP_ApiUartPutc(UCHAR ch)
{
    /* 往串口写入 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
  
    /* 传输 */
    USART_SendData(USART1, (uint8_t)ch);
}

#endif

#if DESC("初始化")
/*****************************************************************************
 函 数 名  : BSP_Init
 功能描述  : BSP初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID BSP_ApiInit()
{
    RCC_ClocksTypeDef RCC_Clocks;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* 初始化 */
	SystemInit();

	/* 各电源模块的开启 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2 | 
		                   RCC_APB1Periph_I2C2 | RCC_AHBPeriph_DMA1,  ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                              RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1 |
                              RCC_APB2Periph_AFIO, ENABLE);

    /* TICK初始化 */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.SYSCLK_Frequency / OS_TICKS_PER_SEC);
    
    /* 初始化灯 */
    BSP_LedInit();
    
    /* 初始化串口 */
    BSP_UartInit();

	/* 初始化GPIO */
	BSP_GpioInit();

	/* 初始化SPI */
	BSP_SpiInit();
	
    /* 初始化中断 */
    BSP_InterruptInit();
}
#endif

