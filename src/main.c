#include "vos.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "main.h"
#include "bsp.h"

#if DESC("任务")
static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskLedStk[APP_TASK_START_STK_SIZE];
static   OS_STK      Task2401RxStk[APP_TASK_START_STK_SIZE];
static   OS_STK      Task2401TxStk[APP_TASK_START_STK_SIZE];

static void TaskLed(void *p_arg)
{
	for(;;)
	{
		//BSP_ApiLedCtrl(BSP_LED_0, 1);
		//OSTimeDly(500);
		//BSP_ApiLedCtrl(BSP_LED_0, 0);
		OSTimeDly(500);
	}
}

UCHAR ucRxBuf[32];
UCHAR ucTxBuf[32];
extern const UCHAR g_aucMyAddr[];

static void Task2401Rx(void *p_arg)
{
	DRV_NRF24L01_RX_Mode();

	for(;;)
	{
		if (VOS_OK == DRV_NRF24L01_RxPkt(ucRxBuf))
		{
			if (ucRxBuf[4])
			{
				BSP_ApiLedCtrl(BSP_LED_0, 1);
			}
			else
			{
				BSP_ApiLedCtrl(BSP_LED_0, 0);
			}

			if (1 == ucRxBuf[3])
			{
		        ucTxBuf[0] = ucRxBuf[1];   //dmac
				ucTxBuf[1] = g_aucMyAddr[4];
				ucTxBuf[2] = 0;    //len	
				ucTxBuf[3] = 2;    //cmd						
                ucTxBuf[4] = ucRxBuf[4];
				ucTxBuf[5] = 0;

				DRV_NRF24L01_TX_Mode(0xfe);
				DRV_NRF24L01_TxPkt(ucTxBuf);
				DRV_NRF24L01_RX_Mode();
			}

			OSTimeDly(10);
		}
        
        OSTimeDly(10);
	}
}

static void Task2401Tx(void *p_arg)
{    
    ULONG ulCount;   
	ULONG ulAddr = 0;
	for(;;)
	{
		ulCount++;
		ulAddr = (ulAddr+1)%2;
	    ucTxBuf[0] = ulAddr + 1;    //dmac		
		ucTxBuf[1] = g_aucMyAddr[4];   //smac
		ucTxBuf[2] = 0;    //len		
		ucTxBuf[3] = 1;    //cmd
	    ucTxBuf[4] = (ulCount % 4)/2;   //data
	    ucTxBuf[5] = 0;    //crc

        DRV_NRF24L01_TX_Mode(ulAddr + 1);
		OSTimeDly(5);
                
		if (VOS_OK == DRV_NRF24L01_TxPkt(ucTxBuf))
		{
           printf("\r\n ok");
		}
        else
        {
           printf("\r\n err");
        }
 
		DRV_NRF24L01_RX_Mode();
		OSTimeDly(300);
	}
}



static void AppTaskStart(void *p_arg)
{
     //新建任务  
    //OSTaskCreate(TaskLed, (void *)0, TaskLedStk + APP_DEFAULT_TASK_SIZE - 1, 1);

	OSTaskCreate(Task2401Rx, (void *)0, Task2401RxStk + APP_DEFAULT_TASK_SIZE - 1, 2);
#if (BOARD_TYPE_MAINBOARD == BOARD_TYPE)
	OSTaskCreate(Task2401Tx, (void *)0, Task2401TxStk + APP_DEFAULT_TASK_SIZE - 1, 3);
#endif 

	OSStatInit();

    while(1)  
    {  
        OSTimeDlyHMSM(0,0,3,0);  
    }
}

#endif

#if DESC("主函数")
/*****************************************************************************
 函 数 名  : main
 功能描述  : 主函数
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年12月18日
    作    者   : linhao
    修改内容   : 新生成函数5

*****************************************************************************/
extern UCHAR DRV_NRF24L01_Check(VOID);
int main(void)
{   
    ULONG i;
    
	/* 初始化BSP */
	BSP_ApiInit();

	/* 初始化VOS */
	CMD_Init();
	CMDTREE_Init();
	
	__enable_irq();
	BSP_ApiDeLayMs(500);
    
    if (VOS_OK == DRV_NRF24L01_Check())
    {
    	for (i = 0; i < 5; i++)
    	{
	    	BSP_ApiLedCtrl(BSP_LED_0, 1);
			BSP_ApiDeLayMs(50);
			BSP_ApiLedCtrl(BSP_LED_0, 0);
			BSP_ApiDeLayMs(50);
		}
    }
	DRV_NRF24L01_Init();
        
	OSInit();

	OSTaskCreate(AppTaskStart,								 /* Create the start task									 */
					(void *)0,
					(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
					APP_TASK_START_PRIO);

	OSStart(); 
  
	while(1)
	{
		BSP_ApiDeLayMs(200);
	}
}

#endif

