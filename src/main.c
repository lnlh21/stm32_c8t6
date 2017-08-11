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
    SYS_VER,                            /* �汾�� */
    SYS_DEFAULT_MAC,                    /* MAC��ַ */ 
    SYS_DEFAULT_IP,                     /* IP��ַ */
    SYS_DEFAULT_MASK,                   /* ���� */
    SYS_DEFAULT_GW,                     /* ���� */
    SYS_DEFAULT_TFTP_REMOTE_IP,         /* TFTPԶ�̵�ַ */
    SYS_DEFAULT_HTTP_PORT,              /* HTTP�˿� */
    SYS_DEFAULT_TELNET_PORT,            /* TELNET�˿� */
    SYS_DEFAULT_TFTP_LOCAL_PORT,        /* TFTP���ض˿� */
    SYS_DEFAULT_TFTP_REMOTE_PORT,       /* TFTPԶ�̶˿� */
    "ministm32_app.bin"                 /* TFTP�ļ��� */     
};
ULONG g_ulNetResetFlag = 0;

APP_LOAD_PARA_S *g_pstAppLoadPara;

static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskLedStk[APP_TASK_START_STK_SIZE];
static   OS_STK      Task6050Stk[APP_TASK_START_STK_SIZE];
static   OS_STK      TaskWs2812Stk2[APP_TASK_START_STK_SIZE];

VOID SYS_ParaInit(VOID)
{
    /* �ȶ�ȡ */
    FLASH_ApiReadData(FLASH_API_DATA_ADDR_START, (UCHAR *)&g_stSysInfo, sizeof(g_stSysInfo));
   
    if (SYS_TAG != g_stSysInfo.ulTag)
    {
        /* ����Ĭ��ֵ */
        memcpy((UINT8 *)&g_stSysInfo, (UCHAR *)&g_stSysInfoDefault, sizeof(g_stSysInfo));

        /* д��FLASH */
        FLASH_ApiSaveData(FLASH_API_DATA_ADDR_START, (UINT8 *)&g_stSysInfo, sizeof(g_stSysInfo));
    }
}

VOID SYS_UpdatePara(VOID)
{
    FLASH_ApiSaveData(FLASH_API_DATA_ADDR_START, (UINT8 *)&g_stSysInfo, sizeof(g_stSysInfo));
}

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
     //�½�����  
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

