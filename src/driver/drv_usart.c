/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : drv_usart.c
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��12��17��
  ����޸�   :
  ��������   : ��������
  �����б�   :
              DRV_UsartInit
  �޸���ʷ   :
  1.��    ��   : 2013��12��17��
    ��    ��   : linhao
    �޸�����   : �����ļ�

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
    /* ������д�� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }
  
    /* �������ݵ�USART2 */
    USART_SendData(USART1, (uint8_t) ch);

    return ch;
}

/*****************************************************************************
 �� �� ��  : DRV_UsartDbgPrint
 ��������  : ���Դ�ӡ��
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
void DRV_UsartInit()
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

void DRV_UsartPutChar(UCHAR ch)
{
    /* ������д�� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
  
    /* ���� */
    USART_SendData(USART1, (uint8_t)ch);
}

