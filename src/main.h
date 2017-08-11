/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : main.h
  版 本 号   : 初稿
  作    者   : lnlh21
  生成日期   : 2014年8月31日
  最近修改   :
  功能描述   : main.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年8月31日
    作    者   : lnlh21
    修改内容   : 创建文件

******************************************************************************/
#ifndef __MAIN_H__
#define __MAIN_H__

/*  MINI STM32 接线
    PA0 -- LED1
    PA1 -- LED2
    PA2 -- 可变电阻
    PA3 -- KEY1
    PA4 -- CS
    PA5 -- SCK
    PA6 -- MISO
    PA7 -- MOSI
    PA8 -- KEY2
    PA9 -- U1TX
    PA10 - U1RX
    PA11 - USBDM
    PA12 - USBDP
    PA13 - SWDIO/JTMS
    PA14 - SWDCLK/JTCK
    PA15 - JTDI

    PB0
    PB1
    PB2  - BOOT1    --ENC_RESET
    PB3  - JTDO
    PB4  - JNTRST
    PB5
    PB6  - SCL
    PB7  - SDA
    PB8
    PB9
    PB10
    PB11
    PB12
    PB13
    PB14
    PB15

    PC0
    PC1
    PC2
    PC3
    PC4
    PC5
    PC6
    PC7
    PC8
    PC9
    PC10
    PC11
    PC12
*/

/*  ENC28J60模块图

               _________
    CLKOUT-----|1     6 |-----INT  (J1)
    WOL   -----|2     7 |-----MISO
    MOSI  -----|3     8 |-----SCK
    CS    -----|4     9 |-----RESET
    3.3   -----|5     10|-----GND
               ---------
*/


#define SYS_TAG                          2
#define SYS_VER                          1
#define SYS_DEFAULT_MAC                 {0x00, 0x22, 0x89, 0x43, 0x00, 0x01}
#define SYS_DEFAULT_IP                  {192, 168, 1, 250}
#define SYS_DEFAULT_MASK                {255, 255, 255, 0}
#define SYS_DEFAULT_GW                  {192, 168, 1, 1}
#define SYS_DEFAULT_TFTP_REMOTE_IP      {192, 168, 1, 200}
#define SYS_DEFAULT_TFTP_LOCAL_PORT     69
#define SYS_DEFAULT_TFTP_REMOTE_PORT    69
#define SYS_DEFAULT_HTTP_PORT           8089
#define SYS_DEFAULT_TELNET_PORT         23

#define START_MAGIC  0x5aa55aa5         /* 放在内存的加载魔术字 */
#define APP_MAGIC  0x5a5a0aa5
#define LOAD_PARA_ADDR  0x20004F00   /* 放加载参数的内存地址 */

#pragma pack(1)

typedef struct
{
    ULONG  ulTag;                   /* 参数区标识 */
    ULONG  ulVer;                   /* 版本号 */
    UCHAR  aucMacAddr[6];           /* MAC地址 */ 
    UCHAR  aucIpAddr[4];            /* IP地址 */
    UCHAR  aucMask[4];              /* 掩码 */
    UCHAR  aucGw[4];                /* 网关 */
    UCHAR  ucRemoteTftpIp[4];       /* TFTP远程地址 */
    USHORT usHttpPort;              /* HTTP端口 */
    USHORT usTelnetPort;            /* TFTP远程端口 */
    USHORT usLocalTftpPort;         /* TFTP本地端口 */
    USHORT usRemoteTftpPort;        /* TFTP远程端口 */
    UCHAR  aucTftpFile[32];
}SYS_INFO_S;


/* 放加载参数的结构 */
typedef struct
{
    ULONG ulStartMagic;
    UCHAR ucMyMac[6];
    UCHAR ucMyIp[4];
    UCHAR ucMyMask[4];
    UCHAR ucMyGw[4];
    UCHAR ucTftpServerIp[4];
    USHORT usTftpClientPort;
    USHORT usTftpServerPort;
    UCHAR  ucTftpFile[32];
}APP_LOAD_PARA_S;

#pragma pack()

extern APP_LOAD_PARA_S *g_pstAppLoadPara;
extern SYS_INFO_S g_stSysInfo;

#endif /* __MAIN_H__ */


