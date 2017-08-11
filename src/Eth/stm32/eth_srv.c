/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Etg_Srv.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : ETH模块业务层代码
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

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

/* 服务器存储数据 */
ETH_SRV_DATA_S g_stEthServerData;

/* TELNET服务器数据 */
ETH_SRV_DATA_S g_stEthTelnetData;

/* TELNET发送缓存 */
UCHAR ucTelnetSendBuf[ETH_TELNET_BUF_LEN];

/* TELNET发送缓存长度 */
UCHAR ucTelnetBufLen = 0;

/* 上一次TELNET发送缓存长度 */
UCHAR ucTelnetBufLenOld = 0;

/* HTTP有关 */
UCHAR g_ucHttpFile[1000];
ULONG g_ucHttpFileLen = 0;
ULONG ulLedStatus = 0;

extern struct netif netif;

#if DESC("服务端代码")

/*****************************************************************************
 函 数 名  : ETH_SrvServerRecv
 功能描述  : 读写器端实现的TCP服务器，接收从上位机过来的报文
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
             struct pbuf *p       
             err_t err            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t
ETH_SrvServerRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    USHORT usLen;

    if (ERR_OK != err)
    {
        /* 有错误，这里先按老流程返回成功 */
        return ERR_OK;
    }
    
    if (NULL != p)
    {
        /* 收取报文，只收取1024字节，
           (by linhao 老版本这里对长包支持不好，得改进) */
        usLen = p->tot_len;
        if (p->tot_len > 1418)
        {
            usLen = 1418;
        }

        /* 送给中间层处理 */
        ETH_MidServerRecv(p->payload, usLen);
        
        /* 告诉协议栈数据接收大小 */
        tcp_recved(pcb, p->tot_len);

        /* 释放PCB */
        pbuf_free(p);

        /* output在这里有什么用?? */
        tcp_output(pcb);
    }
    else if (NULL == p)
    {
        /* 收到一个空的包 */
        tcp_arg(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_err(pcb, NULL);
        tcp_poll(pcb, NULL, 1);

        /* 关闭TCP连接 */
        tcp_close(pcb);

        /* 信息清空 */
        g_stEthServerData.pConnectPCB = NULL;
        g_stEthServerData.eTCPState = E_ETH_SERVER_TCP_LISTEN;
    }
    
    return ERR_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerWrite
 功能描述  : 读写器端实现的TCP服务器，发送报文给上位机
 输入参数  : UCHAR *pucData  
             USHORT usLen    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

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
    /* 获取发送队列可用字节大小 */
    usLenTmp = tcp_sndbuf(g_stEthServerData.pConnectPCB);
    
    if ( usLenTmp > usLen)
    {
        usLenTmp = usLen;
    }
    
    tcp_write(g_stEthServerData.pConnectPCB, pucData, usLenTmp, 1);
    
    /* 对外发送数据 */
    tcp_output(g_stEthServerData.pConnectPCB);
    
    __enable_irq();
    return PUB_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerError
 功能描述  : 读写器端实现的TCP服务器，TCP错误处理
 输入参数  : void *arg  
             err_t err  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static void
ETH_SrvServerError(void *arg, err_t err)
{
    /* 进行出错记录 */
    g_stEthServerData.ucErrorCount++;
    g_stEthServerData.eLastErr = err;

    /* 初始化相应信息 */
    g_stEthServerData.pConnectPCB         = NULL;
    g_stEthServerData.eTCPState           = E_ETH_SERVER_TCP_LISTEN;
    g_stEthServerData.ulConnectionTimeout = 0;
    g_stEthServerData.bLinkLost           = 0;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerPoll
 功能描述  : 读写器端实现的TCP服务器，TCP轮询处理
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t
ETH_SrvServerPoll(void *arg, struct tcp_pcb *pcb)
{
    /* 定时器自加 */
    g_stEthServerData.ulConnectionTimeout++;

    /* 注意，0为永不超时 */
    if ((g_stEthServerData.ulMaxTimeout != 0) 
        && (g_stEthServerData.ulConnectionTimeout > g_stEthServerData.ulMaxTimeout))
    {
        /* 超时后，关闭连接 */
        tcp_abort(pcb);
    }

    return ERR_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerSent
 功能描述  : 读写器端实现的TCP服务器，TCP发送回调函数
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
             u16_t len            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t
ETH_SrvServerSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    /* 不做什么事，只清一下超时定时器 */
    g_stEthServerData.ulConnectionTimeout = 0;

    return ERR_OK;
}



/*****************************************************************************
 函 数 名  : ETH_SrvServerAccept
 功能描述  : 读写器端实现的TCP服务器，TCP接收处理
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
             err_t err            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t 
ETH_SrvServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    printf("\r\nAccept");
    /* 如果当前不在连接状态 */
    if (E_ETH_SERVER_TCP_LISTEN != g_stEthServerData.eTCPState)
    {
        /* 如果当前没Link上，这里如果断开，可能会导致无法再次重连 */
        //if (!g_stEthServerData.bLinkLost)
        //{
        //    return(ERR_CONN);
        //}

        g_stEthServerData.bLinkLost = 0;

        /* 关闭当前连接 */
        tcp_abort(g_stEthServerData.pConnectPCB);
        g_stEthServerData.pConnectPCB = NULL;
    }

    /* 把当前PCB保存下来 */
    g_stEthServerData.pConnectPCB = pcb;
    g_stEthServerData.eTCPState   = E_ETH_SERVER_TCP_CONNECTED;

    /* 接受此PCB */
    tcp_accepted(pcb);

    /* 超时时间置0 */
    g_stEthServerData.ulConnectionTimeout = 0;

    /* 轮询时间为120S */
    g_stEthServerData.ulMaxTimeout = 0;        

    /* 设置TCP优先级 */
    tcp_setprio(pcb, TCP_PRIO_MIN);

    /* 设置TCP接收回调函数 */
    tcp_recv(pcb, ETH_SrvServerRecv);

    /* 设置TCP出错处理回调函数 */
    tcp_err(pcb, ETH_SrvServerError);

    /* 设置TCP轮询回调函数及时间间隔，1秒一次 */
    tcp_poll(pcb, ETH_SrvServerPoll, (1000 / TCP_SLOW_INTERVAL));

    /* 设置TCP发送回调函数 */
    tcp_sent(pcb, ETH_SrvServerSent);

    return ERR_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerInit
 功能描述  : 读写器端实现的TCP服务器初始化
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_SrvServerInit()
{
    struct tcp_pcb *pcb;

    /* 初始化信息 */
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

    /* 创建一个新的PCB连接 */
    pcb = tcp_new();

    /* TCP绑定到IP端口 */
    tcp_bind(pcb, IP_ADDR_ANY, 80);       

    /* 开始侦听 */
    pcb = tcp_listen(pcb);
    g_stEthServerData.pListenPCB = pcb;
        
    /* 设定接受后的处理行数 */	
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
                httpfill("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<center><h1>ss嵌入式网络控制系统</h1>");
                httpfill("<p>路灯当前状态: ");
                if (1 == ulLedStatus)
                {
                    httpfill("<font color=\"#00FF00\"> 亮</font>");
                }
                else
                {
                    httpfill("灭");
                }
                httpfill("</p><p><a href=\"/\">刷新</a>&nbsp;&nbsp;<a href=\"");
                if (1 == ulLedStatus)
                {
                    httpfill("/0\">关闭路灯");
                }
                else
                {
                    httpfill("/1\">开启路灯");
                }
                httpfill("</a></p></center><hr><br>动力工作室<br>林皓<br>");
                
                
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

#if DESC("调试TELNET网络连接代码")

/*****************************************************************************
 函 数 名  : ETH_SrvServerRecv
 功能描述  : 读写器端实现的TCP服务器，接收从上位机过来的报文
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
             struct pbuf *p       
             err_t err            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t
ETH_SrvTelnetRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    USHORT usLen;
    UCHAR *ps;
    ULONG i;
       
    if (ERR_OK != err)
    {
        /* 有错误，这里先按老流程返回成功 */
        return ERR_OK;
    }
    
    if (NULL != p)
    {
        /* 收取报文，只收取1024字节，
           (by linhao 老版本这里对长包支持不好，得改进) */
        usLen = p->tot_len;
        if (p->tot_len > 1418)
        {
            usLen = 1418;
        }

        /* 设置Printf从网口输出 */
        DRV_UART_SET_PRINT_NET();

        /* 送给DEBUG模块处理 */
        ps = (UCHAR *)p->payload;
        for (i = 0; i < usLen; i++)
        {
            CMD_Rx(ps[i]);
        }
        
        /* 告诉协议栈数据接收大小 */
        tcp_recved(pcb, p->tot_len);

        /* 释放PCB */
        pbuf_free(p);

        /* output在这里有什么用?? */
        tcp_output(pcb);
    }
    else if (NULL == p)
    {
        /* 收到一个空的包 */
        tcp_arg(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_err(pcb, NULL);
        tcp_poll(pcb, NULL, 1);

        /* 关闭TCP连接 */
        tcp_close(pcb);

        /* 信息清空 */
        g_stEthTelnetData.pConnectPCB = NULL;
        g_stEthTelnetData.eTCPState = E_ETH_SERVER_TCP_LISTEN;
    }
    
    return ERR_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerWrite
 功能描述  : 读写器端实现的TCP服务器，发送报文给上位机
 输入参数  : UCHAR *pucData  
             USHORT usLen    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

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

    /* 获取发送队列可用字节大小 */
    usLenTmp = tcp_sndbuf(g_stEthTelnetData.pConnectPCB);
    
    if ( usLenTmp > usLen)
    {
        usLenTmp = usLen;
    }

    tcp_write(g_stEthTelnetData.pConnectPCB, pucData, usLenTmp, 1);
    
    /* 对外发送数据 */
    tcp_output(g_stEthTelnetData.pConnectPCB);
    
    return PUB_OK;
}


/*****************************************************************************
 函 数 名  : ETH_SrvTelnetWriteBuf
 功能描述  : TELNET写入缓冲区
 输入参数  : UCHAR ch  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年11月6日
    作    者   : linhao
    修改内容   : 新生成函数

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

    /* 发送数据 */
    ETH_SrvTelnetWrite(ucTelnetSendBuf, ucTelnetBufLen);
    ucTelnetBufLen = 0;
}

/*****************************************************************************
 函 数 名  : ETH_SrvTelnetSendPoll
 功能描述  : TELNET定时发送
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年11月6日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
void ETH_SrvTelnetSendPoll(void)
{
    __disable_irq();
    if (0 != ucTelnetBufLen)
    {
        if (ucTelnetBufLenOld != ucTelnetBufLen)
        {
            /* 不发送，再等等数据 */
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
 函 数 名  : ETH_SrvServerError
 功能描述  : 读写器端实现的TCP服务器，TCP错误处理
 输入参数  : void *arg  
             err_t err  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static void
ETH_SrvTelnetError(void *arg, err_t err)
{
    /* 进行出错记录 */
    g_stEthTelnetData.ucErrorCount++;
    g_stEthTelnetData.eLastErr = err;

    /* 初始化相应信息 */
    g_stEthTelnetData.pConnectPCB         = NULL;
    g_stEthTelnetData.eTCPState           = E_ETH_SERVER_TCP_LISTEN;
    g_stEthTelnetData.ulConnectionTimeout = 0;
    g_stEthTelnetData.bLinkLost           = 0;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerPoll
 功能描述  : 读写器端实现的TCP服务器，TCP轮询处理
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t
ETH_SrvTelnetPoll(void *arg, struct tcp_pcb *pcb)
{    
    /* 定时器自加 */
    g_stEthTelnetData.ulConnectionTimeout++;

    /* 注意，0为永不超时 */
    if ((g_stEthTelnetData.ulMaxTimeout != 0) 
        && (g_stEthTelnetData.ulConnectionTimeout > g_stEthTelnetData.ulMaxTimeout))
    {
        /* 超时后，关闭连接 */
        tcp_abort(pcb);
    }

    return ERR_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerSent
 功能描述  : 读写器端实现的TCP服务器，TCP发送回调函数
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
             u16_t len            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t
ETH_SrvTelnetSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    /* 不做什么事，只清一下超时定时器 */
    g_stEthTelnetData.ulConnectionTimeout = 0;

    return ERR_OK;
}



/*****************************************************************************
 函 数 名  : ETH_SrvServerAccept
 功能描述  : 读写器端实现的TCP服务器，TCP接收处理
 输入参数  : void *arg            
             struct tcp_pcb *pcb  
             err_t err            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
static err_t 
ETH_SrvTelnetAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{        
    /* 如果当前不在连接状态 */
    if (E_ETH_SERVER_TCP_LISTEN != g_stEthTelnetData.eTCPState)
    {
        /* 如果当前没Link上 */
        //if (!g_stEthServerData.bLinkLost)
        //{
        //  return(ERR_CONN);
        //}

        g_stEthTelnetData.bLinkLost = 0;

        /* 关闭当前连接 */
        tcp_abort(g_stEthTelnetData.pConnectPCB);
        g_stEthTelnetData.pConnectPCB = NULL;
    }

    /* 把当前PCB保存下来 */
    g_stEthTelnetData.pConnectPCB = pcb;
    g_stEthTelnetData.eTCPState   = E_ETH_SERVER_TCP_CONNECTED;

    /* 接受此PCB */
    tcp_accepted(pcb);

    /* 超时时间置0 */
    g_stEthTelnetData.ulConnectionTimeout = 0;

    /* 轮询时间为120S */
    g_stEthTelnetData.ulMaxTimeout = 0;        

    /* 设置TCP优先级 */
    tcp_setprio(pcb, TCP_PRIO_MIN);

    /* 设置TCP接收回调函数 */
    tcp_recv(pcb, ETH_SrvTelnetRecv);

    /* 设置TCP出错处理回调函数 */
    tcp_err(pcb, ETH_SrvTelnetError);

    /* 设置TCP轮询回调函数及时间间隔，1秒一次 */
    tcp_poll(pcb, ETH_SrvTelnetPoll, (1000 / TCP_SLOW_INTERVAL));

    /* 设置TCP发送回调函数 */
    tcp_sent(pcb, ETH_SrvTelnetSent);

    return ERR_OK;
}

/*****************************************************************************
 函 数 名  : ETH_SrvServerInit
 功能描述  : 读写器端实现的TCP服务器初始化
 输入参数  : VOID  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
VOID ETH_SrvTelnetInit()
{
    struct tcp_pcb *pcb;
    
    /* 初始化信息 */
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

    /* 创建一个新的PCB连接 */
    pcb = tcp_new();

    /* TCP绑定到IP端口 */
    tcp_bind(pcb, IP_ADDR_ANY, g_stSysInfo.usTelnetPort);       

    /* 开始侦听 */
    pcb = tcp_listen(pcb);
    g_stEthTelnetData.pListenPCB = pcb;
        
    /* 设定接受后的处理行数 */
    tcp_accept(pcb, ETH_SrvTelnetAccept);   
}
#endif