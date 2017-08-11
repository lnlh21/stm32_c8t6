/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : Eth_Srv.h
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : eth_srv.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��10��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __ETH_SRV_H__
#define __ETH_SRV_H__

#include "public_def.h"
#include "err.h"

/* ����������ѯʱ��,Ϊ60�� */
#define ETH_SERVER_POLL_TIME    120
#define ETH_TELNET_BUF_LEN      128

typedef enum
{
    E_ETH_SERVER_TCP_IDLE,
    E_ETH_SERVER_TCP_LISTEN,
    E_ETH_SERVER_TCP_CONNECTING,
    E_ETH_SERVER_TCP_CONNECTED,
    E_ETH_SERVER_TCP_BUTT
}ETH_SERVER_TCP_EN;


typedef struct
{
    struct tcp_pcb *pConnectPCB;    /* TCP_PCB Handle connected */
    struct tcp_pcb *pListenPCB;     /* TCP_PCB Handle listening */
    ETH_SERVER_TCP_EN eTCPState;    /* TCP��ǰ״̬ */
    USHORT usTelnetLocalPort;       /* Telnet���ض˿�(��������) */
    USHORT usTelnetRemotePort;      /* TelnetԶ�˶˿�(�ͻ���) */
    ULONG ulTelnetRemoteIP;         /* TelnetԶ��IP(�ͻ���) */
    ULONG ulConnectionTimeout;      /* Telnet��ʱ������ */
    ULONG ulMaxTimeout;             /* Telnet��ʱ���ʱ�� */
    UCHAR bLinkLost;                /* ��·�Ƿ�Ͽ� */  
    UCHAR ucErrorCount;             /* ������� */
    UCHAR ucReconnectCount;         /* �����Ӵ��� */
    UCHAR ucConnectCount;           /* ���Ӵ��� */
    UCHAR ucClientFlag;             /* ��־ */
    UCHAR ucRsv[3];                 /* ���� */
    err_t eLastErr;                 /* ���һ�γ���״̬ */
}ETH_SRV_DATA_S;

#pragma pack(1)

/* TFTPͷ */
typedef struct
{
    USHORT usCmd;
    USHORT usBlockNum;
}ETH_TFTP_ACK_S, ETH_TFTP_HEAD_S;

/* APP�ļ�ͷ */
typedef struct
{
    ULONG ulFlag;
    ULONG ulFileLen;
    ULONG ulRsv1;
    ULONG ulRsv2;
}BIN_FILE_HEAD_S;

enum 
{
    TFTP_STATUS_LOADING,
    TFTP_STATUS_SUCCESS,
    TFTP_STATUS_ERROR
};

typedef struct
{
    ULONG ulRecvLen;
    ULONG ulStatus;      /* ״̬ */
    ULONG ulFlashAddr;
    USHORT usTftpServerPort;
    USHORT usNextBlock;
    struct ip_addr stTftpServerIp;
    struct udp_pcb *pstUdpPcb;
}ETH_SRV_TFTP_INFO;

typedef struct
{
    ULONG ulFlag;
    UCHAR aucIp[4];
    UCHAR aucMask[4];
    UCHAR aucGw[4];
    UCHAR aucTftpIp[4];
    USHORT usPort;
    USHORT usPortTftp;
    UCHAR aucMac[6];
    UCHAR aucRsv[2];
}BOOT_PARA_S;

struct http_state {
  char *file;
  u16_t left;
  u8_t retries;
};
#pragma pack()



/* �������� */
extern VOID ETH_SrvServerInit();
extern VOID ETH_SrvClientInit();
    
#endif /* __ETH_SRV_H__ */
