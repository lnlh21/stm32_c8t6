/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_spi.h
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月18日
  最近修改   :
  功能描述   : drv_spi.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__
/*   STM32F103 SPI接口定义
     PA4  ---  CS
     PA5  ---  SCK
     PA6  ---  MISO
     PA7  ---  MOSI
*/
#define DRV_SPI1_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define DRV_SPI1_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

extern void DRV_SpiInit();
extern ULONG DRV_SpiRecvMsg(UCHAR *pucData, USHORT *pusLen);
extern VOID DRV_SpiSendMsg(UCHAR *pucData, USHORT usLen);
extern unsigned char SPI1_SendByte(unsigned char byte);

#endif /* __DRV_SPI_H__ */

