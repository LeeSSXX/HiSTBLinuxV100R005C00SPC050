/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : jpeg_sdec_comm.c
Version           : Initial Draft
Author            :
Created          : 2017/07/05
Description     : hi_gfx_comm.h  ͷ�ļ�Ҫ���ذ�����������������
                        ͬ��JPEG �ڲ�����.c �ĵط�ʹ�ú꿪�ؿ��ƣ���֤
                        ʹ�õ�ʱ��Ŵ򿪸ú���ʵ�֣��Ӷ���֤���벻��
                        �����͡�
Function List   :

History           :
Date                          Author                    Modification
2017/07/05                sdk                        Created file
******************************************************************************/


/****************************  add include here     *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "hi_common.h"

#ifdef CONFIG_GFX_ANDROID_SDK
    #include <utils/Log.h>
#endif

#ifdef CONFIG_GFX_256BYTE_ALIGN
#include "hi_math.h"
#endif

/***************************** Macro Definition     ********************************/
#ifndef CLOCK_MONOTONIC_RAW
    #define CLOCK_MONOTONIC_RAW    4
#endif
/***************************** Structure Definition *******************************/


/********************** Global Variable declaration ********************************/


/********************** API forward declarations    ********************************/

/**********************       API realization       ***********************************/


/*****************************************************************************
* func              : JPEG_SDEC_GetTimeStamp
* description    : get timer
                          CNcomment: ��ȡʱ��CNend\n
* param[in]     : cinfo             CNcomment: ���������    CNend\n
* retval           : NA
* others:        : NA
*****************************************************************************/
HI_VOID JPEG_SDEC_GetTimeStamp(HI_U32 *pu32TimeMs, HI_U32 *pu32TimeUs)
{
    HI_S32 Ret = HI_SUCCESS;
    struct timespec timenow = {0, 0};
    clockid_t id = CLOCK_MONOTONIC_RAW;

    if (HI_NULL == pu32TimeMs)
    {
        return;
    }

    Ret = clock_gettime(id, &timenow);
    if (Ret < 0)
    {
        return;
    }

    *pu32TimeMs = (HI_U32)(timenow.tv_sec * 1000 + timenow.tv_nsec/1000000);

    if (HI_NULL == pu32TimeUs)
    {
        return;
    }

    *pu32TimeUs = (HI_U32)(timenow.tv_sec * 1000000 + timenow.tv_nsec/1000);

    return;
}


HI_VOID JPEG_SDEC_GetStride(HI_U32 u32SrcW, HI_U32 *pu32Stride,HI_U32 u32Align)
{
     if (NULL == pu32Stride)
     {
         return;
     }

#ifdef CONFIG_GFX_256BYTE_ALIGN
     HI_UNUSED(u32Align);
     *pu32Stride = HI_SYS_GET_STRIDE(u32SrcW);
#else
     *pu32Stride = (u32SrcW + u32Align - 1) & (~(u32Align - 1));
#endif

      return;
}
