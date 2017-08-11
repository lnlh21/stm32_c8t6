/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Eth_Srv.h
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : eth_srv.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#ifndef __ETH_SRV_H__
#define __ETH_SRV_H__

#include "public_def.h"
#include "err.h"

/* 服务器端轮询时间,为60秒 */
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
    ETH_SERVER_TCP_EN eTCPState;    /* TCP当前状态 */
    USHORT usTelnetLocalPort;       /* Telnet本地端口(服务器端) */
    USHORT usTelnetRemotePort;      /* Telnet远端端口(客户端) */
    ULONG ulTelnetRemoteIP;         /* Telnet远端IP(客户端) */
    ULONG ulConnectionTimeout;      /* Telnet超时计数器 */
    ULONG ulMaxTimeout;             /* Telnet超时最大时间 */
    UCHAR bLinkLost;                /* 链路是否断开 */  
    UCHAR ucErrorCount;             /* 出错次数 */
    UCHAR ucReconnectCount;         /* 重连接次数 */
    UCHAR ucConnectCount;           /* 连接次数 */
    UCHAR ucClientFlag;             /* 标志 */
    UCHAR ucRsv[3];                 /* 保留 */
    err_t eLastErr;                 /* 最后一次出错状态 */
}ETH_SRV_DATA_S;

#pragma pack(1)

/* TFTP头 */
typedef struct
{
    USHORT usCmd;
    USHORT usBlockNum;
}ETH_TFTP_ACK_S, ETH_TFTP_HEAD_S;

/* APP文件头 */
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
    ULONG ulStatus;      /* 状态 */
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



/* 函数申明 */
extern VOID ETH_SrvServerInit();
extern VOID ETH_SrvClientInit();
    
#endif /* __ETH_SRV_H__ */
