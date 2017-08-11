/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Eth_api.h
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : Eth_Api.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#ifndef __ETH_API_H__
#define __ETH_API_H__

/* 字节长度定义 */
#define ETH_API_MAC_LEN     6       /* MAC地址长度 */
#define ETH_API_IP_LEN      4       /* IP地址长度 */

/* 接收回调函数样式定义 */
typedef ULONG (*EthApiRxCallBack)(UCHAR *pucRxData, USHORT usRxLen);

typedef struct
{
    UCHAR  aucMac[ETH_API_MAC_LEN];             /* 本机MAC地址 */
    USHORT usPort;                              /* 服务器端口 */
    UCHAR  aucIp[ETH_API_IP_LEN];               /* 服务器IP地址 */
    UCHAR  aucMask[ETH_API_IP_LEN];             /* 服务器掩码地址 */
    UCHAR  aucGateway[ETH_API_IP_LEN];          /* 服务器网关地址 */
    UCHAR  aucIpReport[ETH_API_IP_LEN];         /* 主动上报时PC IP地址 */
    USHORT usPortReport;                        /* 主动上报用端口 */
    EthApiRxCallBack pfRxFun;                   /* 接收到正常数据后的回调函数 */
    EthApiRxCallBack pfRxReportFun;             /* 接收上报数据返回的回调函数 */
}ETH_API_INFO_S;

extern VOID ETH_ApiInit();
extern ULONG ETH_ApiSend(UCHAR *pucData, USHORT usLen);

#endif /* __ETH_API_H__ */
