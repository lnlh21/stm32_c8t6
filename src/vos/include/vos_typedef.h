/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : public.h
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : ����ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��10��
    ��    ��   : linhao
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef unsigned char UCHAR
    ;
typedef unsigned long UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

typedef char CHAR;
typedef long LONG;
typedef short SHORT;

typedef char INT8;
typedef long INT32;
typedef short INT16;

typedef void VOID;

#ifndef NULL
#define NULL            0
#endif 

#define PUB_OK          0
#define PUB_ERR         1
#define VOS_OK          0
#define VOS_ERR         1
#define PUB_ENABLE      1 
#define PUB_DISABLE     0 

#define DESC(x) 1

#if (BYTE_ORDER == LITTLE_ENDIAN)
#define COM_HTONS(x) (unsigned short)(((unsigned short)(x) & 0x00ffUL) << 8)         \
                                   | (((unsigned short)(x) & 0xff00UL) >> 8)


#define COM_HTONL(x) (unsigned long)(((unsigned long)(x) & 0x000000ffUL) << 24)         \
                                  | (((unsigned long)(x) & 0x0000ff00UL) <<  8)         \
                                  | (((unsigned long)(x) & 0x00ff0000UL) >>  8)         \
                                  | (((unsigned long)(x) & 0xff000000UL) >> 24)))

#define COM_HTONS_SELF(x) (x) = (unsigned short)( (((unsigned short)(x) & 0x00ffUL) << 8)         \
                                                | (((unsigned short)(x) & 0xff00UL) >> 8) )


#define COM_HTONL_SELF(x) (x) = (unsigned long)( (((unsigned long)(x) & 0x000000ffUL) << 24)       \
                                               | (((unsigned long)(x) & 0x0000ff00UL) <<  8)       \
                                               | (((unsigned long)(x) & 0x00ff0000UL) >>  8)       \
                                               | (((unsigned long)(x) & 0xff000000UL) >> 24))
                                             
#else
#define COM_HTONS(x) (x)
#define COM_HTONL(x) (x)
#define COM_HTONS_SELF(x)
#define COM_HTONS_SELF(x)
#endif

#ifdef __cplusplus
	}
#endif /* __cplusplus */ 

#endif /* __PUBLIC_H__ */