/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : drv_init.h
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��12��18��
  ����޸�   :
  ��������   : drv_init.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __DRV_INIT_H__
#define __DRV_INIT_H__

#define BOARD_TYPE_MAINBOARD   1
#define BOARD_TYPE_SUB1        2 
#define BOARD_TYPE_SUB2        3 


#define BOARD_TYPE  BOARD_TYPE_MAINBOARD

enum
{
	BSP_GPIO_OUT_2401_CE,
	BSP_GPIO_OUT_2401_CSN,
	BSP_GPIO_OUT_BUTT
};

enum
{
	BSP_GPIO_IN_2401_IRQ,
	BSP_GPIO_IN_BUTT
};



enum
{
	BSP_LED_0,
	BSP_LED_BUTT
};

enum
{
	BSP_SPI_24L01,
	BSP_SPI_BUTT
};

/* STM32F103RBT6    FLASH:128K     RAM:20K    
   ҳ���СΪ           2K
   ��ҳ��               128
   
-----------------   0x08000000
|               |
|               |
|  BOOT����(32K)|
|               |
|               |
-----------------   0x08008000
|               |
|               |
|  ������(94K)  |
|               |
|               |
-----------------   0x0801F800
|  ������(1K)   |
-----------------   0x0801FC00
| BOOT������(1K)|
-----------------   0x08020000
*/



#define FLASH_API_APP_AREA_START            0x08008000
#define FLASH_API_APP_BEGIN                 0x08008200

/* ��������ʼ��ַ */
#define FLASH_API_DATA_ADDR_START           0x0801F800

/* ������������ַ */
#define FLASH_API_DATA_ADDR_END             0x08020000

/* FLASHҳ���С ����STM32F107�����Ͳ�Ʒ��FLASHҳ���СΪ2K����ֲʱ��ע�� */
#define FLASH_API_PAGE_SIZE                 1024

/* ��ȡ����ҳ���ַ */
#define FLASH_API_GET_PAGE_ADDR(addr)       (addr&0xFFFFFC00)
/* ��ȡ����ҳ��ƫ�Ƶ�ַ */
#define FLASH_API_GET_PAGE_OFFSET(addr)     (addr&0x000003ff)

#define TIM2_CCR2_Address 0x40000038

VOID BSP_GpioSet(ULONG ulDevId, ULONG ulSw);
ULONG BSP_GpioRead(ULONG ulDevId);
UCHAR BSP_ApiSpiRead(ULONG ulDevId);
UCHAR BSP_ApiSpiRead(ULONG ulDevId);
UCHAR BSP_ApiSpiWrite(ULONG ulDevId, UCHAR ucData);

#endif /* __DRV_INIT_H__ */

