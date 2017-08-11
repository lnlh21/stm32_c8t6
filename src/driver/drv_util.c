#include "public_def.h"
#include "stm32f10x.h"

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
VOID TIME_ApiDeLayMs(USHORT usTime)
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
VOID TIME_ApiDeLayUs(USHORT usTime)
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

void DRV_LedInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);    
}

void DRV_LedCtrl(ULONG ulNo, ULONG ulSw)
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

void DRV_KeyInit()
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

#define TIM2_CCR2_Address 0x40000038

UCHAR LED_BYTE_Buffer[800];


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

void WS2812_send(uint8_t (*color)[3], uint16_t len)
{
    uint8_t j;
    uint8_t led;
    uint16_t memaddr;
    uint16_t buffersize;
 
    buffersize = (len*24)+42;   // number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;                // reset buffer memory index
    led = 0;                    // reset led index
#if 1
	LED_BYTE_Buffer[memaddr] = 0;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 0;
	memaddr++;
	
    // fill transmit buffer with correct compare values to achieve
    // correct pulse widths according to color values
    while (len)
    {   
        for (j = 0; j < 8; j++)                  // GREEN data
        {
            if ( (color[led][1]<<j) & 0x80 )  // data sent MSB first, j = 0 is MSB j = 7 is LSB
            {
                LED_BYTE_Buffer[memaddr] = 20;  // compare value for logical 1
            }
            else
            {
                LED_BYTE_Buffer[memaddr] = 6;   // compare value for logical 0
            }
            memaddr++;
        }
         
        for (j = 0; j < 8; j++)                  // RED data
        {
            if ( (color[led][0]<<j) & 0x80 )  // data sent MSB first, j = 0 is MSB j = 7 is LSB
            {
                LED_BYTE_Buffer[memaddr] = 20;  // compare value for logical 1
            }
            else
            {
                LED_BYTE_Buffer[memaddr] = 6;   // compare value for logical 0
            }
            memaddr++;
        }
         
        for (j = 0; j < 8; j++)                  // BLUE data
        {
            if ( (color[led][2]<<j) & 0x80 )  // data sent MSB first, j = 0 is MSB j = 7 is LSB
            {
                LED_BYTE_Buffer[memaddr] = 20;  // compare value for logical 1
            }
            else
            {
                LED_BYTE_Buffer[memaddr] = 6;   // compare value for logical 0
            }
            memaddr++;
        }
         
        led++;
        len--;
    }
     
    // add needed delay at end of byte cycle, pulsewidth = 0
    while(memaddr < buffersize)
    {
        LED_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

	DMA_SetCurrDataCounter(DMA1_Channel2, buffersize);    // load number of bytes to be transferred
#else
	LED_BYTE_Buffer[memaddr] = 0;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 30;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 1;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 10;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 20;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 10;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 20;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 10;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 1;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 10;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 1;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 10;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 1;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 20;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 30;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 0;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 30;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 30;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 30;
	memaddr++;
	LED_BYTE_Buffer[memaddr] = 20;
	memaddr++;

	DMA_SetCurrDataCounter(DMA1_Channel2, 20);

#endif

    DMA_Cmd(DMA1_Channel2, ENABLE);                       // enable DMA channel 6
    TIM_Cmd(TIM2, ENABLE);                                // enable Timer 3
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC2))             // wait until transfer complete
   	{
   		//OSTimeDly(5);
   	}
    TIM_Cmd(TIM2, DISABLE);                               // disable Timer 3
    DMA_Cmd(DMA1_Channel2, DISABLE);         
    DMA_ClearFlag(DMA1_FLAG_TC2);   
       
}



