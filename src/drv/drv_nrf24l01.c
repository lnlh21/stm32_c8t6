/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_init.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月18日
  最近修改   :
  功能描述   : 驱动初始化
  函数列表   :
              DRV_Init
  修改历史   :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "vos.h"
#include "stm32f10x.h"
#include "bsp.h"
#include "drv_nrf24l01.h"

const UCHAR g_aucTxAddr[DRV_NRF24L01_TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const UCHAR g_aucRxAddr[DRV_NRF24L01_RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址


#if DESC("寄出器操作")

	 
UCHAR DRV_NRF24L01_WriteReg(UCHAR ucReg, UCHAR ucValue)
{
	UCHAR ucStatus;	
    
    /* 片选 */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 0);
	
	/* 发送寄存器号 */
  	ucStatus =BSP_ApiSpiWrite(BSP_SPI_24L01, ucReg);

	/* 写入寄存器的值 */
  	BSP_ApiSpiWrite(BSP_SPI_24L01, ucValue);

	/* 取消片选 */
  	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 1);
	
  	return ucStatus;
}

UCHAR DRV_NRF24L01_ReadReg(UCHAR ucReg)
{
	UCHAR ucValue;
	
    /* 片选 */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 0);

	/* 发送寄存器号 */
  	BSP_ApiSpiWrite(BSP_SPI_24L01, ucReg);
  	
	/* 读取寄存器内容 */
  	ucValue = BSP_ApiSpiRead(BSP_SPI_24L01);

	/* 取消片选 */
  	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 1);
	
  	return ucValue;
}	

UCHAR DRV_NRF24L01_ReadBuf(UCHAR reg, UCHAR *pBuf, UCHAR ucLen)
{
	UCHAR ucStatus;
	UCHAR ucLoop;	  

    /* 片选 */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 0);

    /* 发送寄存器值(位置),并读取状态值 */
  	ucStatus = BSP_ApiSpiWrite(BSP_SPI_24L01, reg);

	/* 读出数据 */
 	for(ucLoop = 0; ucLoop < ucLen; ucLoop++)
	{
		pBuf[ucLoop] = BSP_ApiSpiRead(BSP_SPI_24L01);
 	}
 	
 	/* 取消片选 */
  	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 1);
	
  	return ucStatus;
}

UCHAR DRV_NRF24L01_WriteBuf(UCHAR ucReg, UCHAR *pBuf, UCHAR ucLen)
{
	UCHAR ucStatus;
	UCHAR ucLoop;
	
    /* 片选 */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 0);

	/* 发送寄存器值(位置),并读取状态值 */
  	ucStatus = BSP_ApiSpiWrite(BSP_SPI_24L01, ucReg);

	/* 写入数据 */
  	for (ucLoop = 0; ucLoop < ucLen; ucLoop++)
	{
		BSP_ApiSpiWrite(BSP_SPI_24L01, *pBuf++);
  	}
  	
	/* 取消片选 */
  	BSP_GpioSet(BSP_GPIO_OUT_2401_CSN, 1);

  	return ucStatus;
}				   
#endif

#if DESC("自检")
UCHAR DRV_NRF24L01_Check(VOID)
{
	UCHAR buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
	UCHAR i;

	/* 写入5个字节的地址 */
	DRV_NRF24L01_WriteBuf(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_TX_ADDR, buf, 5);

	/* 读出写入的地址 */
	DRV_NRF24L01_ReadBuf(DRV_NRF24L01_TX_ADDR, buf, 5);
	for (i = 0; i < 5; i++)
	{
		if(buf[i] != 0XA5)
		{
			break;
		}
	}

	if (i != 5)
	{
		return VOS_ERR;
	}
	
	return VOS_OK;
}	 

#endif

#if DESC("收发包")
UCHAR DRV_NRF24L01_TxPkt(UCHAR *txbuf)
{
	UCHAR ucStatus;
 
 	/* 拉低CE */
 	BSP_GpioSet(BSP_GPIO_OUT_2401_CE, 0);

	/* 写数据到TX BUF  32个字节 */
  	DRV_NRF24L01_WriteBuf(DRV_NRF24L01_WR_TX_PLOAD, txbuf, DRV_NRF24L01_TX_PLOAD_WIDTH);

 	/* 启动发送	*/
 	BSP_GpioSet(BSP_GPIO_OUT_2401_CE, 1);

	/* 等待发送完成 */
	while(BSP_GpioRead(BSP_GPIO_IN_2401_IRQ) != 0);
	
	/* 读取状态寄存器的值 */
	ucStatus = DRV_NRF24L01_ReadReg(DRV_NRF24L01_STATUS);
	
	/* 清除TX_DS或MAX_RT中断标志 */
	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_STATUS, ucStatus);

	if (ucStatus & DRV_NRF24L01_STATUS_BIT_MAX_TX)//达到最大重发次数
	{
		/* 清除TX FIFO寄存器 */
		DRV_NRF24L01_WriteReg(DRV_NRF24L01_FLUSH_TX, 0xff);

		return ucStatus; 
	}
	if (ucStatus & DRV_NRF24L01_STATUS_BIT_TX_OK)//发送完成
	{
		return VOS_OK;
	}
	
	return VOS_ERR;
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
UCHAR DRV_NRF24L01_RxPkt(UCHAR *rxbuf)
{
	UCHAR ucStatus;

	/* 如果没有中断 */
	if (0 != BSP_GpioRead(BSP_GPIO_IN_2401_IRQ))
	{
		return VOS_ERR;
	}

	/* 读取状态寄存器的值 */
	ucStatus = DRV_NRF24L01_ReadReg(DRV_NRF24L01_STATUS);

	/* 清除TX_DS或MAX_RT中断标志 */
	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_STATUS, ucStatus);
    
	/* 接收到数据 */
	if (ucStatus & DRV_NRF24L01_STATUS_BIT_RX_OK)
	{
		/* 读取数据 */
		DRV_NRF24L01_ReadBuf(DRV_NRF24L01_RD_RX_PLOAD, rxbuf, DRV_NRF24L01_RX_PLOAD_WIDTH);

		/* 清除RX FIFO寄存器 */
		DRV_NRF24L01_WriteReg(DRV_NRF24L01_FLUSH_RX, 0xff);
		return VOS_OK; 
	}

	return VOS_ERR;//没收到任何数据
}

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void DRV_NRF24L01_RX_Mode(void)
{
    /* 拉低CE */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CE, 0);

	/* 写RX节点地址 */
  	DRV_NRF24L01_WriteBuf(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RX_ADDR_P0, (UCHAR*)g_aucRxAddr, DRV_NRF24L01_RX_ADR_WIDTH);

    /* 使能通道0的自动应答 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_EN_AA, 0x01);

	/* 使能通道0的接收地址 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_EN_RXADDR, 0x01);

	/* 设置RF通信频率 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RF_CH, 40);

	/* 选择通道0的有效数据宽度 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RX_PW_P0, DRV_NRF24L01_RX_PLOAD_WIDTH);

	/* 设置TX发射参数,0db增益,2Mbps,低噪声增益开启 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RF_SETUP, 0x0f);

	/* 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_CONFIG, 0x0f);

	/* CE为高,进入接收模式 */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CE, 1);
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
VOID DRV_NRF24L01_TX_Mode(VOID)
{														 
    /* 拉低CE */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CE, 0);

	/* 写TX节点地址 */
  	DRV_NRF24L01_WriteBuf(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_TX_ADDR, (UCHAR*)g_aucTxAddr, DRV_NRF24L01_TX_ADR_WIDTH);

	/* 设置TX节点地址,主要为了使能ACK */
  	DRV_NRF24L01_WriteBuf(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RX_ADDR_P0, (UCHAR*)g_aucTxAddr, DRV_NRF24L01_TX_ADR_WIDTH);

	/* 使能通道0的自动应答 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_EN_AA, 0x01);

	/* 使能通道0的接收地址 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_EN_RXADDR, 0x01);

	/* 设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_SETUP_RETR, 0x1a);

	/* 设置RF通道为40 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RF_CH, 40);

	/* 设置TX发射参数,0db增益,2Mbps,低噪声增益开启 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_RF_SETUP, 0x0f);

	/* 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断 */
  	DRV_NRF24L01_WriteReg(DRV_NRF24L01_WRITE_REG | DRV_NRF24L01_CONFIG, 0x0e);

    /* CE为高,10us后启动发送 */
	BSP_GpioSet(BSP_GPIO_OUT_2401_CE, 1);
}	

#endif



