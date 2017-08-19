/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_init.h
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月18日
  最近修改   :
  功能描述   : drv_init.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 创建文件

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
   页面大小为           2K
   总页数               128
   
-----------------   0x08000000
|               |
|               |
|  BOOT区域(32K)|
|               |
|               |
-----------------   0x08008000
|               |
|               |
|  程序区(94K)  |
|               |
|               |
-----------------   0x0801F800
|  数据区(1K)   |
-----------------   0x0801FC00
| BOOT参数区(1K)|
-----------------   0x08020000
*/



#define FLASH_API_APP_AREA_START            0x08008000
#define FLASH_API_APP_BEGIN                 0x08008200

/* 数据区起始地址 */
#define FLASH_API_DATA_ADDR_START           0x0801F800

/* 数据区结束地址 */
#define FLASH_API_DATA_ADDR_END             0x08020000

/* FLASH页面大小 对于STM32F107互联型产品，FLASH页面大小为2K，移植时请注意 */
#define FLASH_API_PAGE_SIZE                 1024

/* 获取所在页面地址 */
#define FLASH_API_GET_PAGE_ADDR(addr)       (addr&0xFFFFFC00)
/* 获取所在页面偏移地址 */
#define FLASH_API_GET_PAGE_OFFSET(addr)     (addr&0x000003ff)

#define TIM2_CCR2_Address 0x40000038

VOID BSP_GpioSet(ULONG ulDevId, ULONG ulSw);
ULONG BSP_GpioRead(ULONG ulDevId);
UCHAR BSP_ApiSpiRead(ULONG ulDevId);
UCHAR BSP_ApiSpiRead(ULONG ulDevId);
UCHAR BSP_ApiSpiWrite(ULONG ulDevId, UCHAR ucData);

#endif /* __DRV_INIT_H__ */

