/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : drv_enc28j60.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年12月23日
  最近修改   :
  功能描述   : Enc28j60驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2013年12月23日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/
#include "public_def.h"
#include "stm32f10x.h"
#include "drv_spi.h"
#include "drv_enc28j60.h"

static unsigned char Enc28j60Bank;
static unsigned int NextPacketPtr;

unsigned char enc28j60ReadOp(unsigned char op, unsigned char address)
{
    unsigned char dat = 0;
    
    ENC28J60_CSL();
    
    dat = op | (address & ADDR_MASK);
    SPI1_SendByte(dat);
    
    dat = SPI1_SendByte(0xFF);
    /* do dummy read if needed (for mac and mii, see datasheet page 29) */
    if(address & 0x80)
    {
        dat = SPI1_SendByte(0xFF);
    }

    ENC28J60_CSH();
    return dat;
}

void enc28j60WriteOp(unsigned char op, unsigned char address, unsigned char data)
{
    unsigned char dat = 0;
      
    ENC28J60_CSL();
    /* issue write command */
    dat = op | (address & ADDR_MASK);
    SPI1_SendByte(dat);
    /* write data */
    dat = data;
    SPI1_SendByte(dat);
    ENC28J60_CSH();
}

void enc28j60ReadBuffer(unsigned int len, unsigned char* data)
{
    ENC28J60_CSL();
    // issue read command
    SPI1_SendByte(ENC28J60_READ_BUF_MEM);
    while(len)
        {
        len--;
        // read data
        *data = (unsigned char)SPI1_SendByte(0);
        data++;
        }
    *data='\0';
    ENC28J60_CSH();
}

void enc28j60WriteBuffer(unsigned int len, unsigned char* data)
{
    ENC28J60_CSL();
    // issue write command
    SPI1_SendByte(ENC28J60_WRITE_BUF_MEM);
    
    while(len)
    {
        len--;
        SPI1_SendByte(*data);
        data++;
    }
    ENC28J60_CSH();
}

void enc28j60SetBank(unsigned char address)
{
    // set the bank (if needed)
    if((address & BANK_MASK) != Enc28j60Bank)
    {
        // set the bank
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        Enc28j60Bank = (address & BANK_MASK);
    }
}

unsigned char enc28j60Read(unsigned char address)
{
    // set the bank
    enc28j60SetBank(address);
    
    // do the read
    return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

void enc28j60Write(unsigned char address, unsigned char data)
{
    // set the bank
    enc28j60SetBank(address);
    
    // do the write
    enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void enc28j60PhyWrite(unsigned char address, unsigned int data)
{
    // set the PHY register address
    enc28j60Write(MIREGADR, address);
    
    // write the PHY data
    enc28j60Write(MIWRL, data);
    enc28j60Write(MIWRH, data>>8);
    
    // wait until the PHY write completes
    while(enc28j60Read(MISTAT) & MISTAT_BUSY)
    {
        //Del_10us(1);
        //_nop_();
    }
}

void enc28j60PacketSend(unsigned int len, unsigned char* packet)
{
    // Set the write pointer to start of transmit buffer area
    enc28j60Write(EWRPTL, TXSTART_INIT&0xFF);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // Set the TXND pointer to correspond to the packet size given
    enc28j60Write(ETXNDL, (TXSTART_INIT+len)&0xFF);
    enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);

    // write per-packet control byte (0x00 means use macon3 settings)
    enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // copy the packet into the transmit buffer
    enc28j60WriteBuffer(len, packet);

    // send the contents of the transmit buffer onto the network
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

    // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
    if( (enc28j60Read(EIR) & EIR_TXERIF) )
    {
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }
}

unsigned int enc28j60PacketReceive(unsigned int maxlen, unsigned char* packet)
{
    unsigned int rxstat;
    unsigned int len;

    // check if a packet has been received and buffered
    //if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
    // The above does not work. See Rev. B4 Silicon Errata point 6.
    if( enc28j60Read(EPKTCNT) ==0 )
    {
        return(0);
    }

    // Set the read pointer to the start of the received packet
    enc28j60Write(ERDPTL, (NextPacketPtr));
    enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

    // read the next packet pointer
    NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // read the packet length (see datasheet page 43)
    len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    len-=4; //remove the CRC count
    // read the receive status (see datasheet page 43)
    rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
    // limit retrieve length
    if (len>maxlen-1)
    {
        len=maxlen-1;
    }

        // check CRC and symbol errors (see datasheet page 44, table 7-3):
        // The ERXFCON.CRCEN is set by default. Normally we should not
        // need to check this.
    if ((rxstat & 0x80)==0)
    {
        // invalid
        len=0;
    }
    else
    {
        // copy the packet from the receive buffer
        enc28j60ReadBuffer(len, packet);
    }
    
    // Move the RX read pointer to the start of the next received packet
    // This frees the memory we just read out
    enc28j60Write(ERXRDPTL, (NextPacketPtr));
    enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);

    // decrement the packet counter indicate we are done with this packet
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    return(len);
}


void DRV_EncInit(UCHAR *aucMac)
{
    UCHAR ucReg;

    /* 软复位 */
    enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    TIME_ApiDeLayMs(300);

    ucReg = enc28j60Read(ESTAT);
    if (0x01 & ucReg)
    {
        printf("Enc28j60 Reset Complete!\r\n");
    }
    
    NextPacketPtr = RXSTART_INIT;
    
    // Rx start
    enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXSTH, RXSTART_INIT>>8);
    
    // set receive pointer address
    enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);
    
    // RX end
    enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
    
    // TX start
    enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60Write(ETXSTH, TXSTART_INIT>>8);
    
    // TX end
    enc28j60Write(ETXNDL, TXSTOP_INIT&0xFF);
    enc28j60Write(ETXNDH, TXSTOP_INIT>>8);
    
    // do bank 1 stuff, packet filter:
    // For broadcast packets we allow only ARP packtets
    // All other packets should be unicast only for our mac (MAADR)
    //
    // The pattern to match on is therefore
    // Type     ETH.DST
    // ARP      BROADCAST
    // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
    // in binary these poitions are:11 0000 0011 1111
    // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
    
    enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_BCEN);
    //enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_ANDOR |ERXFCON_CRCEN|ERXFCON_BCEN);
    enc28j60Write(EPMM0, 0x3f);
    enc28j60Write(EPMM1, 0x30);
    enc28j60Write(EPMCSL, 0xf9);
    enc28j60Write(EPMCSH, 0xf7);

    // do bank 2 stuff
    // enable MAC receive
    enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
        
    // bring MAC out of reset
    enc28j60Write(MACON2, 0x00);
    
    // enable automatic padding to 60bytes and CRC operations
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
    
    // set inter-frame gap (non-back-to-back)
    enc28j60Write(MAIPGL, 0x12);
    enc28j60Write(MAIPGH, 0x0C);
    
    // set inter-frame gap (back-to-back)
    enc28j60Write(MABBIPG, 0x12);
    
    // Set the maximum packet size which the controller will accept
    // Do not send packets longer than MAX_FRAMELEN:
    enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);    
    enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);
    // do bank 3 stuff
    // write MAC address
    // NOTE: MAC address in ENC28J60 is byte-backward
    
    enc28j60Write(MAADR5, aucMac[0]);    
    enc28j60Write(MAADR4, aucMac[1]);
    enc28j60Write(MAADR3, aucMac[2]);
    enc28j60Write(MAADR2, aucMac[3]);
    enc28j60Write(MAADR1, aucMac[4]);
    enc28j60Write(MAADR0, aucMac[5]);

    printf("MAADR5 = 0x%02x\r\n", enc28j60Read(MAADR5));
    printf("MAADR4 = 0x%02x\r\n", enc28j60Read(MAADR4));
    printf("MAADR3 = 0x%02x\r\n", enc28j60Read(MAADR3));
    printf("MAADR2 = 0x%02x\r\n", enc28j60Read(MAADR2));
    printf("MAADR1 = 0x%02x\r\n", enc28j60Read(MAADR1));
    printf("MAADR0 = 0x%02x\r\n", enc28j60Read(MAADR0));

    enc28j60PhyWrite(PHCON1, PHCON1_PDPXMD);

    // no loopback of transmitted frames
    enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
    
    // switch to bank 0
    enc28j60SetBank(ECON1);
    
    // enable interrutps
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    
    // enable packet reception
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

void DRV_EncDbgShowState()
{
    
}

UCHAR ucTestDbg[64] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                       0x00, 0x22, 0x89, 0x43, 0x00, 0x01,
                       0xab, 0xcd, 0x00, 0x11, 0x22, 0x33,
                       0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
                       0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

void DRV_EncDbgSendTestPkt()
{
    enc28j60PacketSend(64, ucTestDbg);
}



