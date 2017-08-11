 /******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Cmd_Main.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : CMD模块主文件
  函数列表   :
              
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "vos_typedef.h"
#include "vos_cmd.h"

VOS_CMD_CTRL_S g_stVosCmdCtrl;

/*****************************************************************************
 函 数 名  : CMD_Init
 功能描述  : CMD模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID CMD_Init()
{
    memset(&g_stVosCmdCtrl, 0, sizeof(g_stVosCmdCtrl));

	g_stVosCmdCtrl.stRootCmd.pFunOrNext = g_stVosCmdCtrl.astRootCmdGroup;
}

/*****************************************************************************
 函 数 名  : CMD_Rx
 功能描述  : CMD模块输入函数
 输入参数  : unsigned char ch  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG g_ulCmdOverFlow = 0;
ULONG g_ulCmdOverFlowCount = 0;
UCHAR g_aucCmdOverFlowContext[2];

VOID CMD_Rx(UCHAR ch)
{
    ULONG i; 

    if (g_ulCmdOverFlow)
    {
        g_aucCmdOverFlowContext[g_ulCmdOverFlowCount] = ch;
        g_ulCmdOverFlowCount++;

		//特殊处理
        if (VOS_CMD_KEY_FUN_LEN == g_ulCmdOverFlowCount)
        {
            g_ulCmdOverFlow = 0;
            g_ulCmdOverFlowCount = 0;

            if (VOS_CMD_KEY_UP == g_aucCmdOverFlowContext[0])
            {
                if (0 == g_stVosCmdCtrl.ucCmdLenOld)
                {
                    return;
                }
                
                for (i = 0; i< g_stVosCmdCtrl.ucCmdLen; i++)
                {
                    printf("\b\b  \b\b");
                }
				
                memcpy(g_stVosCmdCtrl.ucCmdBuf, g_stVosCmdCtrl.ucCmdBufOld, 64);
                g_stVosCmdCtrl.ucCmdLen = g_stVosCmdCtrl.ucCmdLenOld;
                g_stVosCmdCtrl.ucCmdBufOld[g_stVosCmdCtrl.ucCmdLenOld] = 0;
                printf("%s", g_stVosCmdCtrl.ucCmdBufOld);
            }
        }
        return;
    }
        
    /* 溢出 */
    if (ch == VOS_CMD_KEY_FUNC)
    {
        g_ulCmdOverFlow = 1;
        g_ulCmdOverFlowCount = 0;
		return;
    }

    /* 回车 */
    else if (ch == 0x0D)
    {
        printf("\r");
        CMD_Proc();
    }

    /* 退格 */
    else if (ch == 0x08)
    {
        if (g_stVosCmdCtrl.ucCmdLen > 0)
        {
            g_stVosCmdCtrl.ucCmdLen--;
            printf("\b \b");
        }
    }

    /* 帮助 */
    else if (ch == '?')
    {
        if (g_stVosCmdCtrl.ucCmdLen < CMD_BUF_LEN)
        {
            g_stVosCmdCtrl.ucCmdBuf[g_stVosCmdCtrl.ucCmdLen] = ch;
            g_stVosCmdCtrl.ucCmdLen++;
            CMD_Proc();
        }
    }
    /* 直接显示 */
    else if (ch >= 0x20)
    {
        if (g_stVosCmdCtrl.ucCmdLen < CMD_BUF_LEN)
        {
            printf("%c", ch);
            g_stVosCmdCtrl.ucCmdBuf[g_stVosCmdCtrl.ucCmdLen] = ch;
            g_stVosCmdCtrl.ucCmdLen++;
        }
    }
}

/*****************************************************************************
 函 数 名  : CMD_GetCmdName
 功能描述  : CMD模块获取命令名
 输入参数  : UCHAR *pucBuf        
             UCHAR *ucCmdName     
             ULONG *ulCmdNameLen  
 输出参数  : 无
 返 回 值  : UCHAR
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID CMD_GetCmdName(UCHAR *ucCmdName, ULONG *ulCmdNameLen)
{
    UCHAR *pucPosTemp;
    ULONG ulLenTemp;

    pucPosTemp = g_stVosCmdCtrl.pucCurPos;
    ulLenTemp = 0;
    
    while (*pucPosTemp != ' ' && *pucPosTemp != 0)
    {
        ucCmdName[ulLenTemp] = *pucPosTemp;
        ulLenTemp++;
        pucPosTemp++;
    }

    ucCmdName[ulLenTemp] = 0;

    /* 去掉多余空格 */
    while (*pucPosTemp==' ')
    {
        pucPosTemp++;
    }

    *ulCmdNameLen = ulLenTemp;
    g_stVosCmdCtrl.pucCurPos = pucPosTemp;
    
}

/*****************************************************************************
 函 数 名  : CMD_Find
 功能描述  : CMD模块查找命令
 输入参数  : UCHAR *pucCmdName  
             CMD_S *pstCurCmd   
 输出参数  : 无
 返 回 值  : CMD_S
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
CMD_S * CMD_Find(UCHAR *pucCmdName, CMD_S *pstCurCmd)
{
    ULONG i;
    CMD_S *pstCmdSub;
    CMD_S *pstCmdFind = NULL;
    ULONG ulCmdNameLen1;
    ULONG ulCmdNameLen2;

    ulCmdNameLen1 = strlen((char *)pucCmdName);
    pstCmdSub = (CMD_S *)pstCurCmd->pFunOrNext;
    
    for (i = 0; i < pstCurCmd->ulNum; i++)
    {
        ulCmdNameLen2 = strlen((char *)pstCmdSub[i].ucCmd);

        if (ulCmdNameLen1 > ulCmdNameLen2)
        {
            continue;
        }
 
        if (0 == (memcmp(pucCmdName, pstCmdSub[i].ucCmd, ulCmdNameLen1)))
        {
            pstCmdFind = &pstCmdSub[i];

            if (ulCmdNameLen1 == ulCmdNameLen2)
            {
                return pstCmdFind;
            }
        }
    }

    return pstCmdFind;
}

/*****************************************************************************
 函 数 名  : CMD_Help
 功能描述  : CMD模块显示帮助命令
 输入参数  : CMD_S *pCmd  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID CMD_Help(CMD_S *pCmd)
{
    CMD_S * pCmdSub;
    ULONG i;
    
    if (pCmd->ulNum == 0)
    {
         printf("\r\n %-20s  %s", pCmd->ucCmd, pCmd->ucDesc);
    }
    else
    {
        pCmdSub = (CMD_S *)pCmd->pFunOrNext;
        for (i = 0; i < pCmd->ulNum; i++)
        {
            printf("\r\n %-20s  %s", pCmdSub[i].ucCmd, pCmdSub[i].ucDesc);
        }
    }
}

ULONG VOS_CMD_Reg(CMD_S *pCmd)
{
    if (g_stVosCmdCtrl.ulCmdCount >= VOS_CMD_ROOT_CMD_NUM)
    {
		return VOS_ERR;
    }

	memcpy(&g_stVosCmdCtrl.astRootCmdGroup[g_stVosCmdCtrl.ulCmdCount], pCmd, sizeof(CMD_S));
	g_stVosCmdCtrl.ulCmdCount++;
	g_stVosCmdCtrl.stRootCmd.ulNum = g_stVosCmdCtrl.ulCmdCount;
}

/*****************************************************************************
 函 数 名  : CMD_Proc
 功能描述  : CMD模块命令处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID CMD_Proc()
{
    CMD_S * pCmd;
    UCHAR aucCmdName[CMD_NAME_LEN];
    ULONG ulCmdNameLen;
    CMD_FUN aa;
    ULONG ulCount = 0;
    ULONG i;

    g_stVosCmdCtrl.ucCmdBuf[g_stVosCmdCtrl.ucCmdLen] = 0;
    g_stVosCmdCtrl.pucCurPos = g_stVosCmdCtrl.ucCmdBuf;
    pCmd = &g_stVosCmdCtrl.stRootCmd;
                
    while (1)
    {
        CMD_GetCmdName(aucCmdName, &ulCmdNameLen);

        if (0 == (strcmp((char *)aucCmdName, "?")))
        {
            CMD_Help(pCmd);
            break;
        }
        
        if (0 == ulCmdNameLen)
        {
            if (0!=ulCount)
            {
                printf("\r\n 命令格式错误");
                CMD_Help(pCmd);
            }
            break;
        }

        pCmd = CMD_Find(aucCmdName, pCmd);
        if (NULL == pCmd)
        {
            printf("\r\n 没有找到此命令");
            break;
        }

        if (pCmd->ulNum != 0)
        {
            ulCount++;
            continue;
        }

        printf("\r\n");

        memcpy(g_stVosCmdCtrl.ucCmdBufOld, g_stVosCmdCtrl.ucCmdBuf, 64);
        g_stVosCmdCtrl.ucCmdLenOld = g_stVosCmdCtrl.ucCmdLen;
        aa = (CMD_FUN)(pCmd->pFunOrNext);
        aa();
        
        break;
    }
    printf("\r\nLNLH>");
    g_stVosCmdCtrl.ucCmdLen = 0;
    return;
}

#if DESC("获得参数部分")
ULONG CMD_GetULONG(ULONG *pulPara)
{
    ULONG ulCmdNameLen;
    UCHAR ucCmdName[CMD_NAME_LEN];
    ULONG ulJz = 10;
    UCHAR * pucPos = 0;
    ULONG ulTemp;
    
    CMD_GetCmdName(ucCmdName, &ulCmdNameLen);
    if (0 == ulCmdNameLen || ulCmdNameLen > 10)
    {
        return PUB_ERR;
    }

    CMD_ChangeBit(ucCmdName);

    pucPos = ucCmdName;
    if ('0' == ucCmdName[0] && 'X' == ucCmdName[1])
    {
        ulJz = 16;
        pucPos+=2;
    }

    while(*pucPos)
    {
        if ((*pucPos >= '0')
        && (*pucPos <= '9'))
        {
            ulTemp = ulTemp * ulJz + (*pucPos - '0');
        }
        else if ((*pucPos >= 'A')
        && (*pucPos <= 'F'))
        {
            ulTemp = ulTemp * ulJz + (*pucPos - 'A' + 10);
        }
        pucPos++;
    }
    *pulPara = ulTemp;
    return PUB_OK;
}

ULONG CMD_GetIp(UCHAR *ucIp)
{
    ULONG ulCmdNameLen;
    UCHAR ucCmdName[CMD_NAME_LEN];
    UCHAR * pucPos = 0;
    ULONG i;
    ULONG ulIpPos;
    ULONG ulIpValue;
    CMD_GetCmdName(ucCmdName, &ulCmdNameLen);
    if (0 == ulCmdNameLen || ulCmdNameLen > 16)
    {
        return PUB_ERR;
    }

    pucPos = ucCmdName;
    ulIpPos = 0;
    ulIpValue = 0;
    for (i = 0; i < ulCmdNameLen; i++)
    {
        if (ucCmdName[i] >='0' && ucCmdName[i] <= '9')
        {
            ulIpValue *= 10;
            ulIpValue += (ucCmdName[i] - '0');
            if (ulIpValue > 255)
            {
                return CMD_FORMAT_ERR;
            }
        }
        else if (ucCmdName[i] == '.')
        {
        }
        else
        {
            return CMD_FORMAT_ERR;
        }
        
        if (ucCmdName[i] == '.' || i == (ulCmdNameLen-1))
        {
            ucIp[ulIpPos] = ulIpValue;
            ulIpValue = 0;
            ulIpPos++;
            if (ulIpPos > 4)
            {
                return CMD_FORMAT_ERR;
            }        
        }
    }
   
    if (ulIpPos != 4)
    {
        return CMD_FORMAT_ERR;
    }
    
    return PUB_OK;
}

ULONG CMD_GetMac(UCHAR *ucMac)
{
    ULONG ulCmdNameLen;
    UCHAR ucCmdName[CMD_NAME_LEN];
    UCHAR * pucPos = 0;
    ULONG i;
    ULONG ulMacPos;
    ULONG ulMACValue;
    CMD_GetCmdName(ucCmdName, &ulCmdNameLen);
    
    if (ulCmdNameLen != 14)
    {
        return PUB_ERR;
    }

    if ((ucCmdName[4]!=':')|| (ucCmdName[9]!=':'))
    {
        return PUB_ERR;
    }

    CMD_ChangeBit(ucCmdName);

    pucPos = ucCmdName;
    ulMacPos = 0;
    ulMACValue = 0;
    for (i = 0; i < ulCmdNameLen; i++)
    {
        if (ucCmdName[i] >='0' && ucCmdName[i] <= '9')
        {
            ulMACValue *= 16;
            ulMACValue += (ucCmdName[i] - '0');
            if (ulMACValue > 0xffff)
            {
                return CMD_FORMAT_ERR;
            }
        }
        else if (ucCmdName[i] >='A' && ucCmdName[i] <= 'F')
        {
            ulMACValue *= 16;
            ulMACValue += (ucCmdName[i] - 'A' + 10);
            if (ulMACValue > 0xffff)
            {
                return CMD_FORMAT_ERR;
            }
        }
        else if (ucCmdName[i] ==':')
        {
            
        }
        else
        {
            return CMD_FORMAT_ERR;
        }

        if (ucCmdName[i] == ':' || i == (ulCmdNameLen-1))
        {
            ucMac[ulMacPos*2] = ulMACValue >> 8;
            ucMac[ulMacPos*2 + 1] = ulMACValue;
            ulMACValue = 0;
            ulMacPos++;
            if (ulMacPos > 3)
            {
                return CMD_FORMAT_ERR;
            }        
        }
    }
   
    if (ulMacPos != 3)
    {
        return CMD_FORMAT_ERR;
    }
    
    return PUB_OK;
}

ULONG CMD_GetString(UCHAR *pucString)
{
    ULONG ulCmdNameLen;
    UCHAR ucCmdName[CMD_NAME_LEN];
    UCHAR * pucPos = 0;
    ULONG i;
    ULONG ulMacPos;
    ULONG ulMACValue;
    
    CMD_GetCmdName(ucCmdName, &ulCmdNameLen);
    
    memcpy(pucString, ucCmdName, ulCmdNameLen);
    pucString[ulCmdNameLen] = 0;
    
    return PUB_OK;
}

#endif