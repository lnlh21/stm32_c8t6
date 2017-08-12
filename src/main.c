#include "vos.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "main.h"
#include "bsp.h"

#if DESC("任务")
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
     //新建任务  
    OSTaskCreate(TaskLed, (void *)0, TaskLedStk + APP_DEFAULT_TASK_SIZE - 1, 1);
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
    修改内容   : 新生成函数

*****************************************************************************/
int main(void)
{
	/* 初始化 */
	SystemInit();

	/* 初始化系统参数 */
	//SYS_ParaInit();

	/* 初始化硬件 */
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
