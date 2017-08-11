/******************************************************************************

              ��Ȩ���� (C)2010, ���������˳�����Ϣ�������޹�˾

 ******************************************************************************
  �� �� ��   : Flash_Api.c
  �� �� ��   : ����
  ��    ��   : linhao
  ��������   : 2013��9��10��
  ����޸�   :
  ��������   : FLASH��дģ�����ӿ� 
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
#include "drv_flash.h"


/*****************************************************************************
 �� �� ��  : FLASH_ApiSaveData
 ��������  : FLASHģ��д������
 �������  : ULONG ulFlashAddr  
             UCHAR *pucSavebuf  
             ULONG ulLen        
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG FLASH_ApiSaveData(ULONG ulFlashAddr, UCHAR *pucSavebuf, ULONG ulLen)
{
    ULONG ulPageStart;
    ULONG ulPageEnd;
    ULONG ulAddr;
    ULONG ulLenTemp;
    
    /* ��ַ�Ϸ��Լ�� */
#if 0
    if ((FLASH_API_DATA_ADDR_START > ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < (ulFlashAddr + ulLen)))
    {
        return PUB_ERR;
    }
#endif

    __disable_irq();

    /* ����FLASH */
    FLASH_Unlock();
    
    /* ����Ҫ�����Ŀ� */
    ulPageStart = FLASH_API_GET_PAGE_ADDR(ulFlashAddr);
    ulPageEnd   = FLASH_API_GET_PAGE_ADDR(ulFlashAddr + ulLen);

    printf("%08x,%08x", ulPageStart, ulPageStart);
    /* ����FLASH */
    for (ulAddr = ulPageStart; ulAddr <= ulPageEnd; ulAddr+=FLASH_API_PAGE_SIZE)
    {
        printf("\r\n Flash E %08x", ulAddr);
        FLASH_ErasePage(ulAddr);
    }

    /* ���FLASH */
    ulAddr = ulFlashAddr;
    for (ulLenTemp = 0; ulLenTemp < ulLen; ulLenTemp+=2)
    {
        FLASH_ProgramHalfWord(ulAddr, *(USHORT*)(pucSavebuf + ulLenTemp));
        ulAddr+=2;
    }
    
    /* ��FLASH */
    FLASH_Lock();

    __enable_irq();

    return PUB_OK;
}


/*****************************************************************************
 �� �� ��  : FLASH_ApiReadData
 ��������  : FLASHģ���ȡ����
 �������  : ULONG ulFlashAddr  
             UCHAR *pucReadbuf  
             ULONG ulLen        
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : linhao
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG FLASH_ApiReadData(ULONG ulFlashAddr, UCHAR *pucReadbuf, ULONG ulLen)
{
#if 0
    /* ��ַ�Ϸ��Լ�� */
    if ((FLASH_API_DATA_ADDR_START > ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < (ulFlashAddr + ulLen)))
    {
        return PUB_ERR;
    }
#endif

    /* ����FLASH */
    FLASH_Unlock();

    /* ��ȡ */
    memcpy(pucReadbuf, (VOID *)ulFlashAddr, ulLen);

    /* ��FLASH */
    FLASH_Lock();

    return PUB_OK;
}

