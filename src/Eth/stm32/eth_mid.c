/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Eth_Mid.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : 以太网接口层文件
  函数列表   :

  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "public_def.h"
#include "stm32f10x.h"
#include "main.h"
#include "etharp.h"
#include "ethernetif.h"
#include "eth_api.h"
#include "tcp.h"
#include "eth_mid.h"
#include "eth_drv.h"
#include "eth_srv.h"
#include "memp.h"

/* LWIP内部使用的错误定义 */
#define  ETH_ERROR              ((uint32_t)0)
#define  ETH_SUCCESS            ((uint32_t)1)

/* 网络接口全局结构 */
struct netif netif;

/* ETH模块全局变量 */
ULONG g_ulLocalTimer = 0;
ULONG g_ulARPTimer = 0;
ULONG g_ulTcpTimer = 0;
ULONG g_ulTelnetTimer = 0;
#if DESC("调试接口")
/*****************************************************************************
 函 数 名  : ETH_MidDbgPrintPkt
 功能描述  : 调试命令，打印一个包
 输入参数  : UCHAR *pucData  
             USHORT usLen    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年10月10日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_MidDbgPrintPkt(UCHAR *pucData, USHORT usLen)
{
    ULONG i;

    for (i = 0; i < usLen; i++)
    {
        if (0 == (i%8))
        {
            printf("\r\n");
        }
        printf("%02x ", pucData[i]);
    }
}

/*****************************************************************************
 函 数 名  : ETH_MidDbgShowInfo
 功能描述  : 调试命令，显示网络模块参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年10月10日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_MidDbgShowInfo()
{
#if 0
    USHORT usReg;
    printf("\r\n === ETH Infomation ===");
    printf("\r\n MAC : %02x%02x-%02x%02x-%02x%02x", 
            g_stEthMidInfo.aucMac[0],
            g_stEthMidInfo.aucMac[1],
            g_stEthMidInfo.aucMac[2],
            g_stEthMidInfo.aucMac[3],
            g_stEthMidInfo.aucMac[4],
            g_stEthMidInfo.aucMac[5]);
    printf("\r\n IP  : %u.%u.%u.%u", 
            g_stEthMidInfo.aucIp[0],
            g_stEthMidInfo.aucIp[1],
            g_stEthMidInfo.aucIp[2],
            g_stEthMidInfo.aucIp[3]);
    printf("\r\n MSK : %u.%u.%u.%u", 
            g_stEthMidInfo.aucMask[0],
            g_stEthMidInfo.aucMask[1],
            g_stEthMidInfo.aucMask[2],
            g_stEthMidInfo.aucMask[3]);
    printf("\r\n GW  : %u.%u.%u.%u", 
            g_stEthMidInfo.aucGateway[0],
            g_stEthMidInfo.aucGateway[1],
            g_stEthMidInfo.aucGateway[2],
            g_stEthMidInfo.aucGateway[3]);
//    printf("\r\n P   : %u", 
//            g_stEthMidInfo.usPort);
#endif

}
#endif

/*****************************************************************************
 函 数 名  : ETH_MidNetIfInit
 功能描述  : ETH接口层Netif初始化
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_MidNetIfInit(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    /* 初始化内存堆 */
    mem_init();

    /* 初始化内存池 */
    memp_init();

    IP4_ADDR(&ipaddr, g_stSysInfo.aucIpAddr[0], 
                      g_stSysInfo.aucIpAddr[1],
                      g_stSysInfo.aucIpAddr[2],
                      g_stSysInfo.aucIpAddr[3]);
  
    IP4_ADDR(&netmask, g_stSysInfo.aucMask[0], 
                       g_stSysInfo.aucMask[1],
                       g_stSysInfo.aucMask[2],
                       g_stSysInfo.aucMask[3]);
  
    IP4_ADDR(&gw, g_stSysInfo.aucGw[0], 
                  g_stSysInfo.aucGw[1],
                  g_stSysInfo.aucGw[2],
                  g_stSysInfo.aucGw[3]);

    /* 添加网络接口 */
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /* 设置为默认网络接口 */
    netif_set_default(&netif);

    /* 网络接口UP */
    netif_set_up(&netif);
}

/*****************************************************************************
 函 数 名  : ETH_MidIrqHandle
 功能描述  : ETH接口层中断处理
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linha
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_MidIrqHandle(void)
{
    /* 中断，收包交与LWIP协议栈处理 */
    ethernetif_input(&netif);
}

/*****************************************************************************
 函 数 名  : ETH_MidInit
 功能描述  : ETH接口层初始化
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
VOID ETH_MidInit()
{  
    /* 初始化硬件层 */
    DRV_EncInit(g_stSysInfo.aucMacAddr);
        
    /* 初始化网络接口层 */
    ETH_MidNetIfInit();

    httpd_init();

    ETH_SrvTelnetInit();
}


/*****************************************************************************
 函 数 名  : ETH_MidTimer
 功能描述  : ETH Timer处理
 输入参数  : ULONG ulTimeMs
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_MidTimer(ULONG ulTimeMs)
{
    g_ulLocalTimer += ulTimeMs;

    if((g_ulLocalTimer - g_ulARPTimer) >= ARP_TMR_INTERVAL)
    {
        g_ulARPTimer = g_ulLocalTimer;
        etharp_tmr();
    }

    if((g_ulLocalTimer - g_ulTcpTimer) >= TCP_TMR_INTERVAL)
    {
        g_ulTcpTimer = g_ulLocalTimer;
        tcp_tmr();
    }

    if((g_ulLocalTimer - g_ulTelnetTimer) >= 125)
    {
        g_ulTelnetTimer = g_ulLocalTimer;
        ETH_SrvTelnetSendPoll();
    }
}

/*****************************************************************************
 函 数 名  : ETH_MidServerRecv
 功能描述  : ETH接口层接收数据
 输入参数  : UCHAR *pucRxData  
             USHORT usRxLen    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_MidServerRecv(UCHAR *pucRxData, USHORT usRxLen)
{
    ULONG i;
    ULONG ulLen = 16;

    if (usRxLen < ulLen)
    {
        ulLen = usRxLen;
    }

    printf("\r\nR:");
    for (i = 0; i < ulLen; i++)
    {
        printf("%c", pucRxData[i]);
    }

    if (0 != strncmp("GET ", pucRxData, 4))
    {
        printf("\r\nNOGET");
        ETH_MidServerSend("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>", 
                    strlen("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>")+1);
    }
    printf("\r\nGET");
    ETH_MidServerSend("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n<p>lnlh</p>", 
                    strlen("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n<p>lnlh</p>")+1);
}


/*****************************************************************************
 函 数 名  : ETH_MidServerSend
 功能描述  : ETH接口层发送数据
 输入参数  : CHAR *pucRxData  
             USHORT usRxLen   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG ETH_MidServerSend(UCHAR *pucRxData, USHORT usRxLen)
{
    ETH_SrvServerWrite(pucRxData, usRxLen);
    return PUB_OK;
}

