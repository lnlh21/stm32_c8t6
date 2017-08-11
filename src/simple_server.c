/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Ethernet remote device and sensor
 * UDP and HTTP interface 
        url looks like this http://baseurl/password/command
        or http://baseurl/password/
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 * Note: there is a version number in the text. Search for tuxgraphics
 *********************************************/


/*********************************************
 * modified: 2007-08-08
 * Author  : awake
 * Copyright: GPL V2
 * http://www.icdev.com.cn/?2213/
 * Host chip: ADUC7026
**********************************************/
//#include <includes.h>
#include <string.h>
#include "drv_enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "simple_server.h"

static unsigned char mymac[6] = {0x00, 0x22, 0x89, 0x43, 0x00,0x01};
static unsigned char myip[4] = {10,81,118,254};
static unsigned int mywwwport =69;

#define BUFFER_SIZE 1500
static unsigned char buf[BUFFER_SIZE+1];

int simple_server_init()
{
    init_ip_arp_udp_tcp(mymac, myip, mywwwport);
    
    //指示灯状态:0x476 is PHLCON LEDA(绿)=links status, LEDB(红)=receive/transmit
    enc28j60PhyWrite(PHLCON, 0x7a4);    
}

void encint()
{
    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE|EIE_PKTIE);
    simple_server();
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
}

void simple_server(void)
{      
    unsigned int plen;
    unsigned int dat_p;
    unsigned long j;
    unsigned char cmd_pos=0;
    unsigned char cmd;
    unsigned char payloadlen=0;
    char str[30];
    char cmdval;

    enc28j60Write(EIR, 0);
    
    /* 获取一个新的包 */
    plen = enc28j60PacketReceive(BUFFER_SIZE, buf);

    printf("\r\nRecv:");
    for (j = 0; j < 14; j++)
    {
        printf("%02x ", buf[j]);
    }
    
    /* plen=0为非法包 */
    if (plen==0)
    {
        return;
    }
    
    /* arp处理 */
    if(eth_type_is_arp_and_my_ip(buf, plen))
    {
        make_arp_answer_from_request(buf);
        return;
    }

    /* ip报文及ip等于自己过滤 */
    if(eth_type_is_ip_and_my_ip(buf,plen)==0)
    {
        return;
    }

    /* icmp ping报文处理 */
    if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
    {
        make_echo_reply_from_request(buf, plen);
        return;
    }

    /* UDP报文处理 */
    if ((buf[IP_PROTO_P]==IP_PROTO_UDP_V)
     && (buf[UDP_DST_PORT_H_P]==0)
     && (buf[UDP_DST_PORT_L_P]==69))
    {
        printf("\r\n UDP RECV OK");
        //payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
        //buf[UDP_DATA_P];
        //make_udp_reply_from_request(buf,str,strlen(str),myudpport);
    }
}
