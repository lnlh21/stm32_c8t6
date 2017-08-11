/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : Etg_Srv.c
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : ETHģ��ҵ������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��10��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#include "public_def.h"
#include "stm32f10x.h"
#include "tcp.h"
#include "udp.h"
#include "eth_mid.h"
#include "eth_srv.h"
#include "drv_flash.h"
#include "drv_usart.h"
#include "main.h"

/* �������洢���� */
ETH_SRV_DATA_S g_stEthServerData;

/* TELNET���������� */
ETH_SRV_DATA_S g_stEthTelnetData;

/* TELNET���ͻ��� */
UCHAR ucTelnetSendBuf[ETH_TELNET_BUF_LEN];

/* TELNET���ͻ��泤�� */
UCHAR ucTelnetBufLen = 0;

/* ��һ��TELNET���ͻ��泤�� */
UCHAR ucTelnetBufLenOld = 0;

/* HTTP�й� */
UCHAR g_ucHttpFile[1000];
ULONG g_ucHttpFileLen = 0;
ULONG ulLedStatus = 0;

extern struct netif netif;

#if DESC("����˴���")

/*****************************************************************************
 �� �� ��  : ETH_SrvServerRecv
 ��������  : ��д����ʵ�ֵ�TCP�����������մ���λ�������ı���
 �������  : void *arg            
             struct tcp_pcb *pcb  
             struct pbuf *p       
             err_t err            
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t
ETH_SrvServerRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    USHORT usLen;

    if (ERR_OK != err)
    {
        /* �д��������Ȱ������̷��سɹ� */
        return ERR_OK;
    }
    
    if (NULL != p)
    {
        /* ��ȡ���ģ�ֻ��ȡ1024�ֽڣ�
           (by linhao �ϰ汾����Գ���֧�ֲ��ã��øĽ�) */
        usLen = p->tot_len;
        if (p->tot_len > 1418)
        {
            usLen = 1418;
        }

        /* �͸��м�㴦�� */
        ETH_MidServerRecv(p->payload, usLen);
        
        /* ����Э��ջ���ݽ��մ�С */
        tcp_recved(pcb, p->tot_len);

        /* �ͷ�PCB */
        pbuf_free(p);

        /* output��������ʲô��?? */
        tcp_output(pcb);
    }
    else if (NULL == p)
    {
        /* �յ�һ���յİ� */
        tcp_arg(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_err(pcb, NULL);
        tcp_poll(pcb, NULL, 1);

        /* �ر�TCP���� */
        tcp_close(pcb);

        /* ��Ϣ��� */
        g_stEthServerData.pConnectPCB = NULL;
        g_stEthServerData.eTCPState = E_ETH_SERVER_TCP_LISTEN;
    }
    
    return ERR_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerWrite
 ��������  : ��д����ʵ�ֵ�TCP�����������ͱ��ĸ���λ��
 �������  : UCHAR *pucData  
             USHORT usLen    
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG ETH_SrvServerWrite(UCHAR *pucData, USHORT usLen)
{
    USHORT usLenTmp;

    //LNLH_PrintPkt(pucData, usLen);
     
    if (g_stEthServerData.pConnectPCB == NULL)
    {
        return PUB_ERR;
    }

    __disable_irq();
    printf("OK");
    /* ��ȡ���Ͷ��п����ֽڴ�С */
    usLenTmp = tcp_sndbuf(g_stEthServerData.pConnectPCB);
    
    if ( usLenTmp > usLen)
    {
        usLenTmp = usLen;
    }
    
    tcp_write(g_stEthServerData.pConnectPCB, pucData, usLenTmp, 1);
    
    /* ���ⷢ������ */
    tcp_output(g_stEthServerData.pConnectPCB);
    
    __enable_irq();
    return PUB_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerError
 ��������  : ��д����ʵ�ֵ�TCP��������TCP������
 �������  : void *arg  
             err_t err  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static void
ETH_SrvServerError(void *arg, err_t err)
{
    /* ���г����¼ */
    g_stEthServerData.ucErrorCount++;
    g_stEthServerData.eLastErr = err;

    /* ��ʼ����Ӧ��Ϣ */
    g_stEthServerData.pConnectPCB         = NULL;
    g_stEthServerData.eTCPState           = E_ETH_SERVER_TCP_LISTEN;
    g_stEthServerData.ulConnectionTimeout = 0;
    g_stEthServerData.bLinkLost           = 0;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerPoll
 ��������  : ��д����ʵ�ֵ�TCP��������TCP��ѯ����
 �������  : void *arg            
             struct tcp_pcb *pcb  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t
ETH_SrvServerPoll(void *arg, struct tcp_pcb *pcb)
{
    /* ��ʱ���Լ� */
    g_stEthServerData.ulConnectionTimeout++;

    /* ע�⣬0Ϊ������ʱ */
    if ((g_stEthServerData.ulMaxTimeout != 0) 
        && (g_stEthServerData.ulConnectionTimeout > g_stEthServerData.ulMaxTimeout))
    {
        /* ��ʱ�󣬹ر����� */
        tcp_abort(pcb);
    }

    return ERR_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerSent
 ��������  : ��д����ʵ�ֵ�TCP��������TCP���ͻص�����
 �������  : void *arg            
             struct tcp_pcb *pcb  
             u16_t len            
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t
ETH_SrvServerSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    /* ����ʲô�£�ֻ��һ�³�ʱ��ʱ�� */
    g_stEthServerData.ulConnectionTimeout = 0;

    return ERR_OK;
}



/*****************************************************************************
 �� �� ��  : ETH_SrvServerAccept
 ��������  : ��д����ʵ�ֵ�TCP��������TCP���մ���
 �������  : void *arg            
             struct tcp_pcb *pcb  
             err_t err            
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t 
ETH_SrvServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    printf("\r\nAccept");
    /* �����ǰ��������״̬ */
    if (E_ETH_SERVER_TCP_LISTEN != g_stEthServerData.eTCPState)
    {
        /* �����ǰûLink�ϣ���������Ͽ������ܻᵼ���޷��ٴ����� */
        //if (!g_stEthServerData.bLinkLost)
        //{
        //    return(ERR_CONN);
        //}

        g_stEthServerData.bLinkLost = 0;

        /* �رյ�ǰ���� */
        tcp_abort(g_stEthServerData.pConnectPCB);
        g_stEthServerData.pConnectPCB = NULL;
    }

    /* �ѵ�ǰPCB�������� */
    g_stEthServerData.pConnectPCB = pcb;
    g_stEthServerData.eTCPState   = E_ETH_SERVER_TCP_CONNECTED;

    /* ���ܴ�PCB */
    tcp_accepted(pcb);

    /* ��ʱʱ����0 */
    g_stEthServerData.ulConnectionTimeout = 0;

    /* ��ѯʱ��Ϊ120S */
    g_stEthServerData.ulMaxTimeout = 0;        

    /* ����TCP���ȼ� */
    tcp_setprio(pcb, TCP_PRIO_MIN);

    /* ����TCP���ջص����� */
    tcp_recv(pcb, ETH_SrvServerRecv);

    /* ����TCP������ص����� */
    tcp_err(pcb, ETH_SrvServerError);

    /* ����TCP��ѯ�ص�������ʱ������1��һ�� */
    tcp_poll(pcb, ETH_SrvServerPoll, (1000 / TCP_SLOW_INTERVAL));

    /* ����TCP���ͻص����� */
    tcp_sent(pcb, ETH_SrvServerSent);

    return ERR_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerInit
 ��������  : ��д����ʵ�ֵ�TCP��������ʼ��
 �������  : VOID  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID ETH_SrvServerInit()
{
    struct tcp_pcb *pcb;

    /* ��ʼ����Ϣ */
    g_stEthServerData.pConnectPCB         = NULL;
    g_stEthServerData.pListenPCB          = NULL;
    g_stEthServerData.eTCPState           = E_ETH_SERVER_TCP_LISTEN;
    g_stEthServerData.ulConnectionTimeout = 0;
    g_stEthServerData.ulMaxTimeout        = 0;
    g_stEthServerData.usTelnetRemotePort  = 0;
    g_stEthServerData.usTelnetLocalPort   = 0;
    g_stEthServerData.ulTelnetRemoteIP    = 0;
    g_stEthServerData.bLinkLost           = 0;
    g_stEthServerData.ucConnectCount      = 0;
    g_stEthServerData.ucReconnectCount    = 0;
    g_stEthServerData.ucErrorCount        = 0;
    g_stEthServerData.eLastErr            = ERR_OK;

    /* ����һ���µ�PCB���� */
    pcb = tcp_new();

    /* TCP�󶨵�IP�˿� */
    tcp_bind(pcb, IP_ADDR_ANY, 80);       

    /* ��ʼ���� */
    pcb = tcp_listen(pcb);
    g_stEthServerData.pListenPCB = pcb;
        
    /* �趨���ܺ�Ĵ������� */	
    tcp_accept(pcb, ETH_SrvServerAccept);   
}
#endif

#if DESC("HTTP")
void httpfill(UCHAR *progmem_s)
{
    UCHAR c;

    while (c = *(UCHAR*)(progmem_s++)) 
    {
        g_ucHttpFile[g_ucHttpFileLen]=c;
        g_ucHttpFileLen++;
    }
}



/*-----------------------------------------------------------------------------------*/
static void
conn_err(void *arg, err_t err)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(err);

  hs = arg;
  mem_free(hs);
}
/*-----------------------------------------------------------------------------------*/
static void
close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{
  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, NULL);
  mem_free(hs);
  tcp_close(pcb);
}
/*-----------------------------------------------------------------------------------*/
static void
send_data(struct tcp_pcb *pcb, struct http_state *hs)
{
  err_t err;
  u16_t len;

  /* We cannot send more data than space available in the send
     buffer. */     
  if (tcp_sndbuf(pcb) < hs->left) {
    len = tcp_sndbuf(pcb);
  } else {
    len = hs->left;
  }
  err = tcp_write(pcb, hs->file, len, 0);
#if 0
  do {
    err = tcp_write(pcb, hs->file, len, 0);
    if (err == ERR_MEM) {
      len /= 2;
    }
  } while (err == ERR_MEM && len > 1);  
#endif
  if (err == ERR_OK) {
    hs->file += len;
    hs->left -= len;
    /*  } else {
    printf("send_data: error %s len %d %d\n", lwip_strerr(err), len, tcp_sndbuf(pcb));*/
  }
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
  struct http_state *hs;

  hs = arg;
  
  /*  printf("Polll\n");*/
  if (hs == NULL) {
    /*    printf("Null, close\n");*/
    tcp_abort(pcb);
    return ERR_ABRT;
  } else {
    ++hs->retries;
    if (hs->retries == 4) {
      tcp_abort(pcb);
      return ERR_ABRT;
    }
    send_data(pcb, hs);
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct http_state *hs;

  LWIP_UNUSED_ARG(len);

  hs = arg;

  hs->retries = 0;
  
  if (hs->left > 0) {    
    send_data(pcb, hs);
  } else {
    close_conn(pcb, hs);
  }

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    int i;
    char *data;
    struct http_state *hs;

    hs = arg;

    if (err == ERR_OK && p != NULL) 
    {

        /* Inform TCP that we have taken the data. */
        tcp_recved(pcb, p->tot_len);
        
        if (hs->file == NULL) 
        {
            data = p->payload;
          
            if (strncmp(data, "GET ", 4) == 0) 
            {
                for(i = 0; i < 40; i++) 
                {
                    if (((char *)data + 4)[i] == ' ' ||
                    ((char *)data + 4)[i] == '\r' ||
                    ((char *)data + 4)[i] == '\n') 
                    {
                        ((char *)data + 4)[i] = 0;
                    }
                }
                
                if ('0' == data[5])
                {
                    ulLedStatus = 0;
                    DRV_LedCtrl(0, 0);
                }
                else if ('1' == data[5])
                {
                    ulLedStatus = 1;
                    DRV_LedCtrl(0, 1);
                }
          

                g_ucHttpFileLen = 0;
                httpfill("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<center><h1>ssǶ��ʽ�������ϵͳ</h1>");
                httpfill("<p>·�Ƶ�ǰ״̬: ");
                if (1 == ulLedStatus)
                {
                    httpfill("<font color=\"#00FF00\"> ��</font>");
                }
                else
                {
                    httpfill("��");
                }
                httpfill("</p><p><a href=\"/\">ˢ��</a>&nbsp;&nbsp;<a href=\"");
                if (1 == ulLedStatus)
                {
                    httpfill("/0\">�ر�·��");
                }
                else
                {
                    httpfill("/1\">����·��");
                }
                httpfill("</a></p></center><hr><br>����������<br>���<br>");
                
                
                hs->file = g_ucHttpFile;
                hs->left = g_ucHttpFileLen;
                /* printf("data %p len %ld\n", hs->file, hs->left);*/

                pbuf_free(p);
                send_data(pcb, hs);

                /* Tell TCP that we wish be to informed of data that has been
                   successfully sent by a call to the http_sent() function. */
                tcp_sent(pcb, http_sent);
            } 
            else 
            {
                pbuf_free(p);
                close_conn(pcb, hs);
            }
        }

        else 
        {
            pbuf_free(p);
        }
    }

    if (err == ERR_OK && p == NULL) {
        close_conn(pcb, hs);
    }
    return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t
http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct http_state *hs;
  UCHAR *pucIpAddr;
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  tcp_setprio(pcb, TCP_PRIO_MIN);
  
  /* Allocate memory for the structure that holds the state of the
     connection. */
  hs = (struct http_state *)mem_malloc(sizeof(struct http_state));

  if (hs == NULL) {
    printf("http_accept: Out of memory\n");
    return ERR_MEM;
  }

  pucIpAddr = (UCHAR*)&pcb->remote_ip;
  printf("Access:%u.%u.%u.%u\r\n", pucIpAddr[0], pucIpAddr[1], pucIpAddr[2], pucIpAddr[3]);
  
  /* Initialize the structure. */
  hs->file = NULL;
  hs->left = 0;
  hs->retries = 0;
  
  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  tcp_arg(pcb, hs);

  /* Tell TCP that we wish to be informed of incoming data by a call
     to the http_recv() function. */
  tcp_recv(pcb, http_recv);

  tcp_err(pcb, conn_err);
  
  tcp_poll(pcb, http_poll, 4);
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void
httpd_init(void)
{
  struct tcp_pcb *pcb;

  pcb = tcp_new();
  tcp_bind(pcb, IP_ADDR_ANY, g_stSysInfo.usHttpPort);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, http_accept);
}
/*-----------------------------------------------------------------------------------*/

#endif

#if DESC("����TELNET�������Ӵ���")

/*****************************************************************************
 �� �� ��  : ETH_SrvServerRecv
 ��������  : ��д����ʵ�ֵ�TCP�����������մ���λ�������ı���
 �������  : void *arg            
             struct tcp_pcb *pcb  
             struct pbuf *p       
             err_t err            
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t
ETH_SrvTelnetRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    USHORT usLen;
    UCHAR *ps;
    ULONG i;
       
    if (ERR_OK != err)
    {
        /* �д��������Ȱ������̷��سɹ� */
        return ERR_OK;
    }
    
    if (NULL != p)
    {
        /* ��ȡ���ģ�ֻ��ȡ1024�ֽڣ�
           (by linhao �ϰ汾����Գ���֧�ֲ��ã��øĽ�) */
        usLen = p->tot_len;
        if (p->tot_len > 1418)
        {
            usLen = 1418;
        }

        /* ����Printf��������� */
        DRV_UART_SET_PRINT_NET();

        /* �͸�DEBUGģ�鴦�� */
        ps = (UCHAR *)p->payload;
        for (i = 0; i < usLen; i++)
        {
            CMD_Rx(ps[i]);
        }
        
        /* ����Э��ջ���ݽ��մ�С */
        tcp_recved(pcb, p->tot_len);

        /* �ͷ�PCB */
        pbuf_free(p);

        /* output��������ʲô��?? */
        tcp_output(pcb);
    }
    else if (NULL == p)
    {
        /* �յ�һ���յİ� */
        tcp_arg(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_err(pcb, NULL);
        tcp_poll(pcb, NULL, 1);

        /* �ر�TCP���� */
        tcp_close(pcb);

        /* ��Ϣ��� */
        g_stEthTelnetData.pConnectPCB = NULL;
        g_stEthTelnetData.eTCPState = E_ETH_SERVER_TCP_LISTEN;
    }
    
    return ERR_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerWrite
 ��������  : ��д����ʵ�ֵ�TCP�����������ͱ��ĸ���λ��
 �������  : UCHAR *pucData  
             USHORT usLen    
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG ETH_SrvTelnetWrite(UCHAR *pucData, USHORT usLen)
{
    USHORT usLenTmp;
    
    if (g_stEthTelnetData.pConnectPCB == NULL)
    {
        return PUB_ERR;
    }

    if (E_ETH_SERVER_TCP_CONNECTED != g_stEthTelnetData.eTCPState)
    {
        return PUB_OK;
    }

    /* ��ȡ���Ͷ��п����ֽڴ�С */
    usLenTmp = tcp_sndbuf(g_stEthTelnetData.pConnectPCB);
    
    if ( usLenTmp > usLen)
    {
        usLenTmp = usLen;
    }

    tcp_write(g_stEthTelnetData.pConnectPCB, pucData, usLenTmp, 1);
    
    /* ���ⷢ������ */
    tcp_output(g_stEthTelnetData.pConnectPCB);
    
    return PUB_OK;
}


/*****************************************************************************
 �� �� ��  : ETH_SrvTelnetWriteBuf
 ��������  : TELNETд�뻺����
 �������  : UCHAR ch  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��6��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
void ETH_SrvTelnetWriteBuf(UCHAR ch)
{
    if (ucTelnetBufLen < ETH_TELNET_BUF_LEN)
    {
        ucTelnetSendBuf[ucTelnetBufLen++] = ch;

        if (ucTelnetBufLen < ETH_TELNET_BUF_LEN)
        {
            __enable_irq();
            return;
        }
    }

    /* �������� */
    ETH_SrvTelnetWrite(ucTelnetSendBuf, ucTelnetBufLen);
    ucTelnetBufLen = 0;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvTelnetSendPoll
 ��������  : TELNET��ʱ����
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��6��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
void ETH_SrvTelnetSendPoll(void)
{
    __disable_irq();
    if (0 != ucTelnetBufLen)
    {
        if (ucTelnetBufLenOld != ucTelnetBufLen)
        {
            /* �����ͣ��ٵȵ����� */
            ;
        }
        else
        {
            ETH_SrvTelnetWrite(ucTelnetSendBuf, ucTelnetBufLen);
            ucTelnetBufLen = 0;
        }
    }
    ucTelnetBufLenOld = ucTelnetBufLen;
    __enable_irq();
}
/*****************************************************************************
 �� �� ��  : ETH_SrvServerError
 ��������  : ��д����ʵ�ֵ�TCP��������TCP������
 �������  : void *arg  
             err_t err  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static void
ETH_SrvTelnetError(void *arg, err_t err)
{
    /* ���г����¼ */
    g_stEthTelnetData.ucErrorCount++;
    g_stEthTelnetData.eLastErr = err;

    /* ��ʼ����Ӧ��Ϣ */
    g_stEthTelnetData.pConnectPCB         = NULL;
    g_stEthTelnetData.eTCPState           = E_ETH_SERVER_TCP_LISTEN;
    g_stEthTelnetData.ulConnectionTimeout = 0;
    g_stEthTelnetData.bLinkLost           = 0;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerPoll
 ��������  : ��д����ʵ�ֵ�TCP��������TCP��ѯ����
 �������  : void *arg            
             struct tcp_pcb *pcb  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t
ETH_SrvTelnetPoll(void *arg, struct tcp_pcb *pcb)
{    
    /* ��ʱ���Լ� */
    g_stEthTelnetData.ulConnectionTimeout++;

    /* ע�⣬0Ϊ������ʱ */
    if ((g_stEthTelnetData.ulMaxTimeout != 0) 
        && (g_stEthTelnetData.ulConnectionTimeout > g_stEthTelnetData.ulMaxTimeout))
    {
        /* ��ʱ�󣬹ر����� */
        tcp_abort(pcb);
    }

    return ERR_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerSent
 ��������  : ��д����ʵ�ֵ�TCP��������TCP���ͻص�����
 �������  : void *arg            
             struct tcp_pcb *pcb  
             u16_t len            
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t
ETH_SrvTelnetSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    /* ����ʲô�£�ֻ��һ�³�ʱ��ʱ�� */
    g_stEthTelnetData.ulConnectionTimeout = 0;

    return ERR_OK;
}



/*****************************************************************************
 �� �� ��  : ETH_SrvServerAccept
 ��������  : ��д����ʵ�ֵ�TCP��������TCP���մ���
 �������  : void *arg            
             struct tcp_pcb *pcb  
             err_t err            
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
static err_t 
ETH_SrvTelnetAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{        
    /* �����ǰ��������״̬ */
    if (E_ETH_SERVER_TCP_LISTEN != g_stEthTelnetData.eTCPState)
    {
        /* �����ǰûLink�� */
        //if (!g_stEthServerData.bLinkLost)
        //{
        //  return(ERR_CONN);
        //}

        g_stEthTelnetData.bLinkLost = 0;

        /* �رյ�ǰ���� */
        tcp_abort(g_stEthTelnetData.pConnectPCB);
        g_stEthTelnetData.pConnectPCB = NULL;
    }

    /* �ѵ�ǰPCB�������� */
    g_stEthTelnetData.pConnectPCB = pcb;
    g_stEthTelnetData.eTCPState   = E_ETH_SERVER_TCP_CONNECTED;

    /* ���ܴ�PCB */
    tcp_accepted(pcb);

    /* ��ʱʱ����0 */
    g_stEthTelnetData.ulConnectionTimeout = 0;

    /* ��ѯʱ��Ϊ120S */
    g_stEthTelnetData.ulMaxTimeout = 0;        

    /* ����TCP���ȼ� */
    tcp_setprio(pcb, TCP_PRIO_MIN);

    /* ����TCP���ջص����� */
    tcp_recv(pcb, ETH_SrvTelnetRecv);

    /* ����TCP������ص����� */
    tcp_err(pcb, ETH_SrvTelnetError);

    /* ����TCP��ѯ�ص�������ʱ������1��һ�� */
    tcp_poll(pcb, ETH_SrvTelnetPoll, (1000 / TCP_SLOW_INTERVAL));

    /* ����TCP���ͻص����� */
    tcp_sent(pcb, ETH_SrvTelnetSent);

    return ERR_OK;
}

/*****************************************************************************
 �� �� ��  : ETH_SrvServerInit
 ��������  : ��д����ʵ�ֵ�TCP��������ʼ��
 �������  : VOID  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��12��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID ETH_SrvTelnetInit()
{
    struct tcp_pcb *pcb;
    
    /* ��ʼ����Ϣ */
    g_stEthTelnetData.pConnectPCB         = NULL;
    g_stEthTelnetData.pListenPCB          = NULL;
    g_stEthTelnetData.eTCPState           = E_ETH_SERVER_TCP_LISTEN;
    g_stEthTelnetData.ulConnectionTimeout = 0;
    g_stEthTelnetData.ulMaxTimeout        = 0;
    g_stEthTelnetData.usTelnetRemotePort  = 0;
    g_stEthTelnetData.usTelnetLocalPort   = 0;
    g_stEthTelnetData.ulTelnetRemoteIP    = 0;
    g_stEthTelnetData.bLinkLost           = 0;
    g_stEthTelnetData.ucConnectCount      = 0;
    g_stEthTelnetData.ucReconnectCount    = 0;
    g_stEthTelnetData.ucErrorCount        = 0;
    g_stEthTelnetData.eLastErr            = ERR_OK;

    /* ����һ���µ�PCB���� */
    pcb = tcp_new();

    /* TCP�󶨵�IP�˿� */
    tcp_bind(pcb, IP_ADDR_ANY, g_stSysInfo.usTelnetPort);       

    /* ��ʼ���� */
    pcb = tcp_listen(pcb);
    g_stEthTelnetData.pListenPCB = pcb;
        
    /* �趨���ܺ�Ĵ������� */
    tcp_accept(pcb, ETH_SrvTelnetAccept);   
}
#endif