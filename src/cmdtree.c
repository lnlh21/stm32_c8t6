#include "vos_typedef.h"
#include "vos_cmd.h"
#include "ucos_ii.h"
#include "stm32f10x.h"
extern void ShowMpu6050();
void CMD_FUN_ShowMsg(void)
{
    printf("\r\n CPU        : STM32");
    printf("\r\n Flash Size : %d K",
            *(__IO uint16_t*)(0x1FFFF7E0));

	printf("\r\n UID        : %08x-%08x-%08x",
            *(__IO uint32_t*)(0x1FFFF7E8),
            *(__IO uint32_t*)(0x1FFFF7Ec),
            *(__IO uint32_t*)(0x1FFFF7F0));

	SystemCoreClockUpdate();
    printf("\r\n Core Clock : %dHz",
            SystemCoreClock);
	printf("\r\n CPU Usage  : %d%", OSCPUUsage);  

}

#if DESC("显示CPU命令")
VOID CMD_ChangeBit(UCHAR * pucStr)
{
    while(*pucStr)
    {
        if ((*pucStr >= 'a')
            && (*pucStr <= 'z'))
        {
            *pucStr = *pucStr - ('a' - 'A');
        }
        pucStr++;
    }
}

VOID CMD_DisplayCpuMem(VOID)
{
    ULONG ulAddr;
    ULONG ulRet;
    ULONG ulLen;
    ULONG i;

    /* 获取地址 */
    ulRet = CMD_GetULONG(&ulAddr);
    if (PUB_OK != ulRet)
    {
        printf("参数错误");
        return;
    }

    ulRet = CMD_GetULONG(&ulLen);
    if (PUB_OK != ulRet)
    {
        printf("参数错误");
        return;
    }

    if (ulLen > 64)
    {
        ulLen = 64;
    }

    for (i = 0; i <= ulLen; i+=4)
    {
        printf("\r\n Addr(%08x) = (%08x)", (ulAddr+i), *(ULONG *)(ulAddr+i));
    }
}

/* display命令注册 */
CMD_S g_stCmdDisplayCpu[] = 
{
    {
     "mem",
     "查询CPU地址",
     CMD_FUN(CMD_DisplayCpuMem),
    },
};

#endif

#if DESC("显示命令")

VOID CMD_DisplayVersion(VOID)
{
    printf("\r\n Compile Time : "__DATE__ " - " __TIME__);
	printf("\r\n STDPERIPH_VERSION : %d.%d.%d",
            __STM32F10X_STDPERIPH_VERSION_MAIN,
            __STM32F10X_STDPERIPH_VERSION_SUB1,
            __STM32F10X_STDPERIPH_VERSION_SUB2);
}

/* display命令注册 */
CMD_S g_stCmdDisplayGroup[] = 
{
    {
     "version",
     "查询版本号",
     CMD_FUN(CMD_DisplayVersion),
    },
    {
     "sys",
     "查询系统信息",
     CMD_FUN(CMD_FUN_ShowMsg),
    },
    {
     "mpu6050",
     "查询系统信息",
     CMD_FUN(ShowMpu6050),
    },
    {
     "cpu",
     "显示CPU命令",
     SUB_CMD(g_stCmdDisplayCpu),
    }
};
#endif

#if DESC("根命令")
/* 根命令注册 */
CMD_S g_stCmdDisplay[] = 
{
    {   
        "display",
        "显示命令",
        SUB_CMD(g_stCmdDisplayGroup),
    },
};

void exit();
VOID CMD_Exit(VOID)
{
    //exit();
}

CMD_S g_stCmdExit[] = 
{
    {   
        "exit",
        "退出系统",
        CMD_FUN(CMD_Exit),
    },
};

ULONG CMDTREE_Init()
{
	VOS_CMD_Reg(g_stCmdDisplay);
	VOS_CMD_Reg(g_stCmdExit);
}

#endif

