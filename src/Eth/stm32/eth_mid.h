/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Eth_Mid.h
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : IF_Ethernet.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#ifndef __ETH_MID_H__
#define __ETH_MID_H__

#define ETH_MAC_LEN     6       /* MAC地址长度 */
#define ETH_IP_LEN      4       /* IP地址长度 */

#pragma pack(1)
//typedef struct
//{
//}ETH_INFO_S;

#pragma pack()
//extern ETH_INFO_S g_stEthMidInfo;

extern VOID ETH_MidClientRecv(UCHAR *pucRxData, USHORT usRxLen);
extern ULONG ETH_MidClientSend(UCHAR *pucRxData, USHORT usRxLen);
extern VOID ETH_MidDbgPrintPkt(UCHAR *pucData, USHORT usLen);
extern VOID ETH_MidDbgShowInfo();
extern VOID ETH_MidHwInit();
extern VOID ETH_MidInit();
extern VOID ETH_MidIrqHandle(void);
extern VOID ETH_MidNetIfInit(void);
extern VOID ETH_MidServerRecv(UCHAR *pucRxData, USHORT usRxLen);
extern ULONG ETH_MidServerSend(UCHAR *pucRxData, USHORT usRxLen);
extern ULONG ETH_MidSetIpAddr(UCHAR *aucIp, UCHAR *aucMask, UCHAR *aucGateway);
extern ULONG ETH_MidSetMacAddr(UCHAR *aucMac);
extern ULONG ETH_MidSetReportInfo(UCHAR *aucIpReport, USHORT usPortReport);
extern ULONG ETH_MidSetServerPort(SHORT usPort);
extern VOID ETH_MidTimer(ULONG ulTimeMs);

#endif /* __ETH_MID_H__ */
