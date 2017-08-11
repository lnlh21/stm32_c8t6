/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : drv_spi.c
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��12��18��
  ����޸�   :
  ��������   : SPI����
  �����б�   :
              DRV_SpiInit
              DRV_SpiRecvMsg
              DRV_SpiSendMsg
              SPI_SendByte
  �޸���ʷ   :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#include "public_def.h"
#include "stm32f10x.h"

UCHAR ucSpiSendBuf[1000];
UCHAR ucSpiRecvBuf[1000];

/*****************************************************************************
 �� �� ��  : DRV_SpiInit
 ��������  : SPI��ʼ��
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
 �� �� ��  : SPI_SendByte
 ��������  : SPI����
 �������  : unsigned char byte  
 �������  : ��
 �� �� ֵ  : unsigned
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

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
 �� �� ��  : DRV_SpiSendMsg
 ��������  : SPI����Ϣ
 �������  : UCHAR *pucData  
             USHORT usLen    
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

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
 �� �� ��  : DRV_SpiRecvMsg
 ��������  : SPI����Ϣ
 �������  : UCHAR *pucData  
             USHORT *pusLen  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG DRV_SpiRecvMsg(UCHAR *pucData, USHORT *pusLen)
{
    ULONG i;
    USHORT usLen;
    UCHAR ucLenH, ucLenL;

    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    TIME_ApiDeLayUs(10);
    
    /* ���Ͷ�ȡ */    
    SPI1_SendByte(0x20);
    TIME_ApiDeLayUs(10);

    /* ��ȡ���� */
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