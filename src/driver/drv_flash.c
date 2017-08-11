/******************************************************************************

              版权所有 (C)2010, 深圳市中兴长天信息技术有限公司

 ******************************************************************************
  文 件 名   : Flash_Api.c
  版 本 号   : 初稿
  作    者   : linhao
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : FLASH读写模块对外接口 
  函数列表   :
              
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : linhao
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "public_def.h"
#include "stm32f10x.h"
#include "drv_flash.h"


/*****************************************************************************
 函 数 名  : FLASH_ApiSaveData
 功能描述  : FLASH模块写入数据
 输入参数  : ULONG ulFlashAddr  
             UCHAR *pucSavebuf  
             ULONG ulLen        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG FLASH_ApiSaveData(ULONG ulFlashAddr, UCHAR *pucSavebuf, ULONG ulLen)
{
    ULONG ulPageStart;
    ULONG ulPageEnd;
    ULONG ulAddr;
    ULONG ulLenTemp;
    
    /* 地址合法性检查 */
#if 0
    if ((FLASH_API_DATA_ADDR_START > ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < (ulFlashAddr + ulLen)))
    {
        return PUB_ERR;
    }
#endif

    __disable_irq();

    /* 解锁FLASH */
    FLASH_Unlock();
    
    /* 计算要擦除的块 */
    ulPageStart = FLASH_API_GET_PAGE_ADDR(ulFlashAddr);
    ulPageEnd   = FLASH_API_GET_PAGE_ADDR(ulFlashAddr + ulLen);

    printf("%08x,%08x", ulPageStart, ulPageStart);
    /* 擦除FLASH */
    for (ulAddr = ulPageStart; ulAddr <= ulPageEnd; ulAddr+=FLASH_API_PAGE_SIZE)
    {
        printf("\r\n Flash E %08x", ulAddr);
        FLASH_ErasePage(ulAddr);
    }

    /* 编程FLASH */
    ulAddr = ulFlashAddr;
    for (ulLenTemp = 0; ulLenTemp < ulLen; ulLenTemp+=2)
    {
        FLASH_ProgramHalfWord(ulAddr, *(USHORT*)(pucSavebuf + ulLenTemp));
        ulAddr+=2;
    }
    
    /* 锁FLASH */
    FLASH_Lock();

    __enable_irq();

    return PUB_OK;
}


/*****************************************************************************
 函 数 名  : FLASH_ApiReadData
 功能描述  : FLASH模块读取数据
 输入参数  : ULONG ulFlashAddr  
             UCHAR *pucReadbuf  
             ULONG ulLen        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : linhao
    修改内容   : 新生成函数

*****************************************************************************/
ULONG FLASH_ApiReadData(ULONG ulFlashAddr, UCHAR *pucReadbuf, ULONG ulLen)
{
#if 0
    /* 地址合法性检查 */
    if ((FLASH_API_DATA_ADDR_START > ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < ulFlashAddr)
        || (FLASH_API_DATA_ADDR_END < (ulFlashAddr + ulLen)))
    {
        return PUB_ERR;
    }
#endif

    /* 解锁FLASH */
    FLASH_Unlock();

    /* 读取 */
    memcpy(pucReadbuf, (VOID *)ulFlashAddr, ulLen);

    /* 锁FLASH */
    FLASH_Lock();

    return PUB_OK;
}

