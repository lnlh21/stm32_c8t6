/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Eth_Api.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : 以太网接口对外API文件
  函数列表   :
              
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "public_def.h"
#include "stm32f10x.h"
#include "eth_api.h"
#include "eth_mid.h"
#include "main.h"

/*****************************************************************************
 函 数 名  : ETH_ApiInit
 功能描述  : ETH模块总初始化接口
 输入参数  : ETH_API_INFO_S * pstEthApiInfo  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_ApiInit()
{   
    /* 调用下一层进行初始化 */
    ETH_MidInit();
}

/*****************************************************************************
 函 数 名  : ETH_ApiSend
 功能描述  : ETH模块发送数据接口
 输入参数  : UCHAR *pucData  
             USHORT usLen    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG ETH_ApiSend(UCHAR *pucData, USHORT usLen)
{
    ULONG ulRet;
    ulRet = ETH_MidServerSend(pucData, usLen);
    return ulRet;
}
