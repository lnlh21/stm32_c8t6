/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_usart.h
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月17日
  最近修改   :
  功能描述   : drv_usart.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年12月17日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DRV_USART_H__
#define __DRV_USART_H__

/* 定义Printf函数输出方向 */
extern ULONG ulPrintfDir;

enum
{
    DRV_PRINT_UART,         /* 从串口输出 */
    DRV_PRINT_NET,          /* 从网口输出 */
    DRV_PRINT_BUTT
};


#define DRV_UART_SET_PRINT_UART()  (ulPrintfDir = DRV_PRINT_UART)
#define DRV_UART_SET_PRINT_NET()   (ulPrintfDir = DRV_PRINT_NET)

extern void DRV_UsartInit();


#endif /* __DRV_USART_H__ */



