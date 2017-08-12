/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_usart.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月17日
  最近修改   :
  功能描述   : 串口驱动
  函数列表   :
              DRV_UsartInit
  修改历史   :
  1.日    期   : 2013年12月17日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "public_def.h"
#include "stm32f10x.h"
#include "drv_usart.h"
#include <stdio.h>

ULONG ulPrintfDir = DRV_PRINT_UART;       


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
 函 数 名  : DRV_UsartDbgPrint
 功能描述  : 调试打印串
 输入参数  : UCHAR *pucData  
             USHORT usLen    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID DRV_UsartDbgPrint(UCHAR *pucData, USHORT usLen)
{
    ULONG i;

    printf("\r\n Len = %u", usLen);
     
    for (i = 0; i < usLen; i++)
    {
        if (0 == (i%8))
        {
            printf("\r\n");
        }
        printf("%02x ", pucData[i]);
    }
}


