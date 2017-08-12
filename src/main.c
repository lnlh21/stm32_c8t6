#include "vos.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "main.h"
#include "bsp.h"

#if DESC("����")
static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskLedStk[APP_TASK_START_STK_SIZE];
static   OS_STK      Task2401Stk[APP_TASK_START_STK_SIZE];

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
static void Task2401(void *p_arg)
{
#if 0
	DRV_NRF24L01_RX_Mode();

	for(;;)
	{
		if (VOS_OK == DRV_NRF24L01_RxPkt(ucRxBuf))
		{
			if (ucRxBuf[0])
			{
				BSP_ApiLedCtrl(BSP_LED_0, 1);
			}
			else
			{
				BSP_ApiLedCtrl(BSP_LED_0, 0);
			}
		}
	}
#else
	DRV_NRF24L01_TX_Mode();

	for(;;)
	{
	    ucRxBuf[0] = ((ucRxBuf[0] + 1) % 2);
		if (VOS_OK == DRV_NRF24L01_TxPkt(ucRxBuf))
		{
			if (ucRxBuf[0])
			{
				BSP_ApiLedCtrl(BSP_LED_0, 1);
			}
			else
			{
				BSP_ApiLedCtrl(BSP_LED_0, 0);
			}

		}
		OSTimeDly(100);
	}

#endif
}


static void AppTaskStart(void *p_arg)
{
     //�½�����  
    //OSTaskCreate(TaskLed, (void *)0, TaskLedStk + APP_DEFAULT_TASK_SIZE - 1, 1);
	OSTaskCreate(Task2401, (void *)0, Task2401Stk + APP_DEFAULT_TASK_SIZE - 1, 2);
	
	OSStatInit();

    while(1)  
    {  
        OSTimeDlyHMSM(0,0,3,0);  
    }
}

#endif

#if DESC("������")
/*****************************************************************************
 �� �� ��  : main
 ��������  : ������
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��12��18��
    ��    ��   : linhao
    �޸�����   : �����ɺ���5

*****************************************************************************/
extern UCHAR DRV_NRF24L01_Check(VOID);
int main(void)
{   
    ULONG i;
    
	/* ��ʼ��BSP */
	BSP_ApiInit();

	/* ��ʼ��VOS */
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
