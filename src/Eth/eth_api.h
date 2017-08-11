/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : Eth_api.h
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : Eth_Api.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��10��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __ETH_API_H__
#define __ETH_API_H__

/* �ֽڳ��ȶ��� */
#define ETH_API_MAC_LEN     6       /* MAC��ַ���� */
#define ETH_API_IP_LEN      4       /* IP��ַ���� */

/* ���ջص�������ʽ���� */
typedef ULONG (*EthApiRxCallBack)(UCHAR *pucRxData, USHORT usRxLen);

typedef struct
{
    UCHAR  aucMac[ETH_API_MAC_LEN];             /* ����MAC��ַ */
    USHORT usPort;                              /* �������˿� */
    UCHAR  aucIp[ETH_API_IP_LEN];               /* ������IP��ַ */
    UCHAR  aucMask[ETH_API_IP_LEN];             /* �����������ַ */
    UCHAR  aucGateway[ETH_API_IP_LEN];          /* ���������ص�ַ */
    UCHAR  aucIpReport[ETH_API_IP_LEN];         /* �����ϱ�ʱPC IP��ַ */
    USHORT usPortReport;                        /* �����ϱ��ö˿� */
    EthApiRxCallBack pfRxFun;                   /* ���յ��������ݺ�Ļص����� */
    EthApiRxCallBack pfRxReportFun;             /* �����ϱ����ݷ��صĻص����� */
}ETH_API_INFO_S;

extern VOID ETH_ApiInit();
extern ULONG ETH_ApiSend(UCHAR *pucData, USHORT usLen);

#endif /* __ETH_API_H__ */
