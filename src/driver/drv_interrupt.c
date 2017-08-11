#include "public_def.h"
#include "stm32f10x.h"

/*****************************************************************************
 函 数 名  : NVIC_Configuration
 功能描述  : NVIC配置
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
VOID NVIC_Configuration()
{
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Set the Vector Table base location at 0x08000000 */
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    NVIC_SetVectorTable(0x08000000, 0);
    
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
    /* Usart1 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    

    /* Usart3 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
   // NVIC_Init(&NVIC_InitStructure);    

    /* Enable the EXTI8 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
    
}
