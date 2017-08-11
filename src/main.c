#include "public_def.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "main.h"
#include "drv_usart.h"
#include "drv_flash.h"
#include "drv_mpu6050.h"

SYS_INFO_S g_stSysInfo;
SYS_INFO_S g_stSysInfoDefault = 
{
    SYS_TAG,
    SYS_VER,                            /* 版本号 */
    SYS_DEFAULT_MAC,                    /* MAC地址 */ 
    SYS_DEFAULT_IP,                     /* IP地址 */
    SYS_DEFAULT_MASK,                   /* 掩码 */
    SYS_DEFAULT_GW,                     /* 网关 */
    SYS_DEFAULT_TFTP_REMOTE_IP,         /* TFTP远程地址 */
    SYS_DEFAULT_HTTP_PORT,              /* HTTP端口 */
    SYS_DEFAULT_TELNET_PORT,            /* TELNET端口 */
    SYS_DEFAULT_TFTP_LOCAL_PORT,        /* TFTP本地端口 */
    SYS_DEFAULT_TFTP_REMOTE_PORT,       /* TFTP远程端口 */
    "ministm32_app.bin"                 /* TFTP文件名 */     
};
ULONG g_ulNetResetFlag = 0;

APP_LOAD_PARA_S *g_pstAppLoadPara;

static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskLedStk[APP_TASK_START_STK_SIZE];
static   OS_STK      Task6050Stk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskWs2812Stk2[APP_TASK_START_STK_SIZE];

VOID SYS_ParaInit(VOID)
{
    /* 先读取 */
    FLASH_ApiReadData(FLASH_API_DATA_ADDR_START, (UCHAR *)&g_stSysInfo, sizeof(g_stSysInfo));
   
    if (SYS_TAG != g_stSysInfo.ulTag)
    {
        /* 拷贝默认值 */
        memcpy((UINT8 *)&g_stSysInfo, (UCHAR *)&g_stSysInfoDefault, sizeof(g_stSysInfo));

        /* 写入FLASH */
        FLASH_ApiSaveData(FLASH_API_DATA_ADDR_START, (UINT8 *)&g_stSysInfo, sizeof(g_stSysInfo));
    }
}

VOID SYS_UpdatePara(VOID)
{
    FLASH_ApiSaveData(FLASH_API_DATA_ADDR_START, (UINT8 *)&g_stSysInfo, sizeof(g_stSysInfo));
}

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
UCHAR eightbit[30][3] =
{
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},

	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},

	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
};

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
static void Task6050(void *p_arg)
{
    short usG_X = 0;
	short usG_XOld = 0;

	for(;;)
	{
		//usG_X = GetData(GYRO_XOUT_H);
		if ((usG_X - usG_XOld) > 5000)
		{
			printf("\r\n Left");
		}
		else if  ((usG_XOld - usG_X) > 5000)
		{
			printf("\r\n Right");
		}

		usG_XOld = usG_X;
		OSTimeDly(100);
	}

}


static void AppTaskStart(void *p_arg)
{
     //新建任务  
    OSTaskCreate(TaskLed, (void *)0, TaskLedStk + APP_DEFAULT_TASK_SIZE - 1, 1);
	OSTaskCreate(Task6050, (void *)0, Task6050Stk + APP_DEFAULT_TASK_SIZE - 1, 2);
	OSStatInit();

    while(1)  
    {  
        OSTimeDlyHMSM(0,0,3,0);  
    }
}


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

    OSTaskCreate(AppTaskStart,                               /* Create the start task                                    */
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

