/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : Eth_Mid.c
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : ��̫���ӿڲ��ļ�
  �����б�   :

  �޸���ʷ   :
  1.��    ��   : 2013��9��10��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
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

/* LWIP�ڲ�ʹ�õĴ����� */
#define  ETH_ERROR              ((uint32_t)0)
#define  ETH_SUCCESS            ((uint32_t)1)

/* ����ӿ�ȫ�ֽṹ */
struct netif netif;

/* ETHģ��ȫ�ֱ��� */
ULONG g_ulLocalTimer = 0;
ULONG g_ulARPTimer = 0;
ULONG g_ulTcpTimer = 0;
ULONG g_ulTelnetTimer = 0;
#if DESC("���Խӿ�")
/*****************************************************************************
 �� �� ��  : ETH_MidDbgPrintPkt
 ��������  : ���������ӡһ����
 �������  : UCHAR *pucData  
             USHORT usLen    
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��10��10��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

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
 �� �� ��  : ETH_MidDbgShowInfo
 ��������  : ���������ʾ����ģ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��10��10��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

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
 �� �� ��  : ETH_MidNetIfInit
 ��������  : ETH�ӿڲ�Netif��ʼ��
 �������  : VOID  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��13��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID ETH_MidNetIfInit(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    /* ��ʼ���ڴ�� */
    mem_init();

    /* ��ʼ���ڴ�� */
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

    /* �������ӿ� */
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /* ����ΪĬ������ӿ� */
    netif_set_default(&netif);

    /* ����ӿ�UP */
    netif_set_up(&netif);
}

/*****************************************************************************
 �� �� ��  : ETH_MidIrqHandle
 ��������  : ETH�ӿڲ��жϴ���
 �������  : VOID  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��13��
    ��    ��   : linha
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID ETH_MidIrqHandle(void)
{
    /* �жϣ��հ�����LWIPЭ��ջ���� */
    ethernetif_input(&netif);
}

/*****************************************************************************
 �� �� ��  : ETH_MidInit
 ��������  : ETH�ӿڲ��ʼ��
 �������  : ETH_API_INFO_S * pstEthApiInfo  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��13��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID ETH_MidInit()
{  
    /* ��ʼ��Ӳ���� */
    DRV_EncInit(g_stSysInfo.aucMacAddr);
        
    /* ��ʼ������ӿڲ� */
    ETH_MidNetIfInit();

    httpd_init();

    ETH_SrvTelnetInit();
}


/*****************************************************************************
 �� �� ��  : ETH_MidTimer
 ��������  : ETH Timer����
 �������  : ULONG ulTimeMs
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��13��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

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
 �� �� ��  : ETH_MidServerRecv
 ��������  : ETH�ӿڲ��������
 �������  : UCHAR *pucRxData  
             USHORT usRxLen    
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��13��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

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
 �� �� ��  : ETH_MidServerSend
 ��������  : ETH�ӿڲ㷢������
 �������  : CHAR *pucRxData  
             USHORT usRxLen   
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��13��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG ETH_MidServerSend(UCHAR *pucRxData, USHORT usRxLen)
{
    ETH_SrvServerWrite(pucRxData, usRxLen);
    return PUB_OK;
}

