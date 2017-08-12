#include "vos.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "main.h"
#include "bsp.h"

#if DESC("����")
static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskLedStk[APP_TASK_START_STK_SIZE];
static   OS_STK      Task6050Stk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskWs2812Stk2[APP_TASK_START_STK_SIZE];

static void TaskLed(void *p_arg)
{
	for(;;)
	{
		DRV_LedCtrl(0, 1);
		OSTimeDly(500);
		DRV_LedCtrl(0, 0);
		OSTimeDly(500);
	}
}

static void AppTaskStart(void *p_arg)
{
     //�½�����  
    OSTaskCreate(TaskLed, (void *)0, TaskLedStk + APP_DEFAULT_TASK_SIZE - 1, 1);
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
    �޸�����   : �����ɺ���

*****************************************************************************/
int main(void)
{
	/* ��ʼ�� */
	SystemInit();

	/* ��ʼ��ϵͳ���� */
	//SYS_ParaInit();

	/* ��ʼ��Ӳ�� */
	DRV_Init();

	CMD_Init();
	CMDTREE_Init();
	
	__enable_irq();
	Set_USB();
	OSInit();

	OSTaskCreate(AppTaskStart,								 /* Create the start task									 */
					(void *)0,
					(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
					APP_TASK_START_PRIO);

	OSStart(); 
  
	while(1)
	{
		TIME_ApiDeLayMs(200);
		TIME_ApiDeLayMs(200);
			
		if (g_ulNetResetFlag)
		{
			TIME_ApiDeLayMs(500);
			NVIC_SystemReset();
		}
	}

}

#endif
