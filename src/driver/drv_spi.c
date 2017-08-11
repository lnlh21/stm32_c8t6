/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_spi.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月18日
  最近修改   :
  功能描述   : SPI驱动
  函数列表   :
              DRV_SpiInit
              DRV_SpiRecvMsg
              DRV_SpiSendMsg
              SPI_SendByte
  修改历史   :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "public_def.h"
#include "stm32f10x.h"

UCHAR ucSpiSendBuf[1000];
UCHAR ucSpiRecvBuf[1000];

/*****************************************************************************
 函 数 名  : DRV_SpiInit
 功能描述  : SPI初始化
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
void DRV_SpiInit()
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


/*****************************************************************************
 函 数 名  : SPI_SendByte
 功能描述  : SPI发送
 输入参数  : unsigned char byte  
 输出参数  : 无
 返 回 值  : unsigned
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
unsigned char SPI1_SendByte(unsigned char byte)
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

/*****************************************************************************
 函 数 名  : DRV_SpiSendMsg
 功能描述  : SPI发消息
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
VOID DRV_SpiSendMsg(UCHAR *pucData, USHORT usLen)
{
    ULONG i;
    ucSpiSendBuf[0] = 0x10;
    ucSpiSendBuf[1] = usLen >> 8;
    ucSpiSendBuf[2] = usLen;
    memcpy(&ucSpiSendBuf[3], pucData, usLen);
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    TIME_ApiDeLayUs(10);
    //printf("BeginSend(%u):", usLen);
    for (i = 0; i < (usLen + 3); i++)
    {
        SPI1_SendByte(ucSpiSendBuf[i]);
        TIME_ApiDeLayUs(10);
    //    printf("%02x ", ucSpiSendBuf[i]);
    }
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

/*****************************************************************************
 函 数 名  : DRV_SpiRecvMsg
 功能描述  : SPI收消息
 输入参数  : UCHAR *pucData  
             USHORT *pusLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG DRV_SpiRecvMsg(UCHAR *pucData, USHORT *pusLen)
{
    ULONG i;
    USHORT usLen;
    UCHAR ucLenH, ucLenL;

    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    TIME_ApiDeLayUs(10);
    
    /* 发送读取 */    
    SPI1_SendByte(0x20);
    TIME_ApiDeLayUs(10);

    /* 读取长度 */
    ucLenH = SPI1_SendByte(0xff);
    TIME_ApiDeLayUs(10);
    //printf("A%u,", ucLenH);

    ucLenL = SPI1_SendByte(0xff);
    TIME_ApiDeLayUs(10);
    // printf("B%u,", ucLenL);
     
    usLen = (ucLenH << 8) | ucLenL;

    //printf("rLen:%u", usLen);

    if (usLen > 1500)
    {
        printf("RecvLen Big, %u", usLen);
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        return PUB_ERR;
    }
    for (i = 0; i < usLen; i++)
    {
        pucData[i] =  SPI1_SendByte(0xff);
        TIME_ApiDeLayUs(10);
    }
    *pusLen = usLen;
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    return PUB_OK;
}