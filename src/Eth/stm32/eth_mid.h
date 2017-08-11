/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : Eth_Mid.h
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : IF_Ethernet.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��10��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __ETH_MID_H__
#define __ETH_MID_H__

#define ETH_MAC_LEN     6       /* MAC��ַ���� */
#define ETH_IP_LEN      4       /* IP��ַ���� */

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
