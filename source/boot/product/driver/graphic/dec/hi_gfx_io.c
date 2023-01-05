/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name          : hi_gfx_io.c
Version             : Initial Draft
Author              :
Created            : 2017/06/08
Description       : ����������
Function List     :
History             :
Date                                 Author                     Modification
2017/06/08                     sdk                         Created file
******************************************************************************/

#include "hi_gfx_io.h"
#include "hi_gfx_comm.h"
#include "hi_gfx_sys.h"

#include "exports.h"

#ifdef HI_BUILD_IN_MINI_BOOT
#include "mmu.h"
#endif
/***************************** Macro Definition ******************************/

/*************************** Structure Definition ****************************/

/********************** Global Variable declaration **************************/

/******************************* API declaration *****************************/


/***************************************************************************************
* func             : HI_GFX_IOCreate
* description   : CNcomment: �������������� CNend\n
* param[in]    : HI_VOID
* retval          : NA
* others:       : NA
***************************************************************************************/
HI_S32 HI_GFX_IOCreate(HIGFX_IO_INSTANCE_S* const pstStream, HI_CHAR* pStremBuf, HI_U32 u32StreamLen)
{

    if ((NULL == pstStream) || (NULL == pStremBuf) || (u32StreamLen <= 0))
    {
        return HI_FAILURE;
    }

    pstStream->pAddr       = pStremBuf;
    pstStream->u32Length   = u32StreamLen;
    pstStream->u32Position = 0;

    return HI_SUCCESS;
}

/***************************************************************************************
* func            : HI_GFX_IODestroy
* description  : CNcomment: �������������� CNend\n
* param[in]    : HI_VOID
* retval          : NA
* others:        : NA
***************************************************************************************/
HI_S32 HI_GFX_IODestroy(HIGFX_IO_INSTANCE_S* const pstStream)
{
    CHECK_POINT_NULL(pstStream);

    HI_GFX_Memset(pstStream,0,sizeof(HIGFX_IO_INSTANCE_S));

    return HI_SUCCESS;
}

/***************************************************************************************
* func           : HI_GFX_IORead
* description : CNcomment: ������ CNend\n
* param[in]  : HI_VOID
* retval         : NA
* others:      : NA
***************************************************************************************/
HI_S32 HI_GFX_IORead(HIGFX_IO_INSTANCE_S* const pstStream, HI_VOID *pBuf, HI_U32 BufLen, HI_U32 *pCopyLen, HI_BOOL *pEndFlag)
{
    HI_U32 Length = 0;

    if ( (NULL == pstStream) || (NULL == pBuf) || (0 == BufLen) || (NULL == pCopyLen) || (NULL == pEndFlag))
    {
        return HI_FAILURE;
    }

    if (pstStream->u32Length < pstStream->u32Position)
    {
        return HI_FAILURE;
    }

    Length = pstStream->u32Length - pstStream->u32Position;
    if (BufLen < Length)
    {
        *pCopyLen = BufLen;
    }
    else
    {
        *pCopyLen = Length;
    }

    if (0 == *pCopyLen)
    {
        return HI_FAILURE;
    }

    #ifdef HI_BUILD_IN_MINI_BOOT
        mmu_cache_enable();
    #else
        dcache_enable(0);
    #endif

    HI_GFX_Memcpy(pBuf,(const HI_VOID*)(pstStream->pAddr + pstStream->u32Position), *pCopyLen);

    #ifdef HI_BUILD_IN_MINI_BOOT
        mmu_cache_disable();
    #else
        dcache_disable();
    #endif

    if (*pCopyLen < BufLen)
    {
        *pEndFlag = HI_TRUE;
    }
    else
    {
        *pEndFlag = HI_FALSE;
    }

    pstStream->u32Position += *pCopyLen;

    return HI_SUCCESS;
}

/***************************************************************************************
* func            : HI_GFX_IOSeek
* description  : CNcomment: �������˵�λ�� CNend\n
* param[in]   : HI_VOID
* retval         : NA
* others:      : NA
***************************************************************************************/
HI_S32 HI_GFX_IOSeek(HIGFX_IO_INSTANCE_S* const pstStream,HI_U32 Offset)
{
    CHECK_POINT_NULL(pstStream);

    if (Offset > pstStream->u32Length)
    {
        return HI_FAILURE;
    }

    pstStream->u32Position = Offset;

    return HI_SUCCESS;
}
