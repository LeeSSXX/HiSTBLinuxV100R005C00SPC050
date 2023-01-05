/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : jpeg_drv_dec.c
Version           : Initial Draft
Author            : sdk
Created          : 2017/06/08
Description      :
Function List    :


History          :
Date                      Author                Modification
2017/06/08             sdk                     Created file
******************************************************************************/


/***************************** add include here*********************************/

#include "hi_jpeg_config.h"
#include "jpeg_drv_parse.h"
#include "hi_drv_jpeg.h"
#include "hi_jpeg_reg.h"
#include "jpeg_drv_hal.h"
#include "jpeg_drv_dec.h"
#include "hi_gfx_sys_k.h"
#include "hi_gfx_comm.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_reg_common.h"
#include "hi_gfx_mem.h"
#endif

#ifdef HI_BUILD_IN_MINI_BOOT
#include "delay.h"
#endif


#ifdef CONFIG_JPEG_OMX_FUNCTION
/***************************** Macro Definition ********************************/

#define CONFIG_JPEG_DEC_HANDLE_MAX            5

/***************************** Structure Definition ****************************/
typedef struct tagDRV_DEC_JPEG_INFO
{
   HI_U32 DecHandle;
   IMAG_INFO_S *pstImgInfo;
}DRV_DEC_JPEG_INFO;

/***************************** Global Variable declaration *********************/

#ifdef HI_BUILD_IN_BOOT
   volatile HI_U32 *gs_pJpegRegBase = NULL;
#endif

DRV_DEC_JPEG_INFO  gs_DrvDecJpegInfo;

/***************************** API forward declarations ************************/
#ifdef HI_BUILD_IN_BOOT
static inline HI_VOID JPEG_OpenDev(HI_VOID);
static inline HI_VOID JPEG_CloseDev(HI_VOID);
#endif
extern HI_S32 JPEG_DRV_SoftDec(HI_JPEG_DECINFO_S *stDecInfo);
extern HI_S32 JPEG_DRV_HardDec(HI_JPEG_DECINFO_S *stDecInfo,IMAG_INFO_S *pstImgInfo);
extern HI_S32 JPEG_DRV_GetDevice(HI_VOID);

/***************************** API realization *********************************/
HI_S32 HI_DRV_JPEG_CreateDec(HI_U32 *pDecHandle)
{
    if (NULL == pDecHandle)
    {
        return HI_FAILURE;
    }

    gs_DrvDecJpegInfo.DecHandle++;
    if (gs_DrvDecJpegInfo.DecHandle >= CONFIG_JPEG_DEC_HANDLE_MAX)
    {
        *pDecHandle = 0;
        return HI_FAILURE;
    }
    *pDecHandle = gs_DrvDecJpegInfo.DecHandle;

    if (gs_DrvDecJpegInfo.DecHandle > 1)
    {
        return HI_SUCCESS;
    }

#ifdef HI_BUILD_IN_BOOT
    gs_DrvDecJpegInfo.pstImgInfo = (IMAG_INFO_S*)HI_GFX_Malloc(sizeof(IMAG_INFO_S),"jpeg-drv-handle");
#else
    gs_DrvDecJpegInfo.pstImgInfo = (IMAG_INFO_S*)HI_GFX_KMALLOC(HIGFX_JPGDEC_ID,sizeof(IMAG_INFO_S),GFP_KERNEL);
#endif

    if (NULL == gs_DrvDecJpegInfo.pstImgInfo)
    {
       gs_DrvDecJpegInfo.DecHandle--;
       return HI_FAILURE;
    }

    HI_GFX_Memset((HI_VOID*)(gs_DrvDecJpegInfo.pstImgInfo),0,sizeof(IMAG_INFO_S));

    return HI_SUCCESS;
}


#ifdef HI_BUILD_IN_BOOT
static inline HI_VOID JPEG_OpenDev(HI_VOID)
{
    volatile U_PERI_CRG31 unTempValue;
  #ifdef CONFIG_GFX_MMU_CLOCK
    volatile U_PERI_CRG205 unTempSmmuValue;
  #endif
    unTempValue.u32 = g_pstRegCrg->PERI_CRG31.u32;
    unTempValue.bits.jpgd_cken     = 0x1;
    g_pstRegCrg->PERI_CRG31.u32 = unTempValue.u32;
    udelay(1000);
    unTempValue.bits.jpgd_srst_req = 0x0;
    g_pstRegCrg->PERI_CRG31.u32 = unTempValue.u32;

  #ifdef CONFIG_GFX_MMU_CLOCK
    unTempSmmuValue.u32 = g_pstRegCrg->PERI_CRG205.u32;
    unTempSmmuValue.bits.jpgd_smmu_cken = 0x1;
    g_pstRegCrg->PERI_CRG205.u32 = unTempSmmuValue.u32;
    udelay(1000);
    unTempSmmuValue.bits.jpgd_smmu_srst_req = 0x0;
    g_pstRegCrg->PERI_CRG205.u32 = unTempSmmuValue.u32;
  #endif

    gs_pJpegRegBase = (HI_U32*)JPGD_REG_BASEADDR;
    JPEG_HAL_Init(gs_pJpegRegBase);
    DRV_JPEG_SetIntMask(JPEG_REG_INTMASK_VALUE);

    return;
}
#endif

HI_S32 HI_DRV_JPEG_DecInfo(HI_U32 DecHandle,HI_DRV_JPEG_INMSG_S *stInMsg)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if ((NULL == stInMsg) || (NULL == gs_DrvDecJpegInfo.pstImgInfo))
    {
       return HI_FAILURE;
    }

    stInMsg->DecHandle = DecHandle;
    s32Ret = DRV_JPEG_Parse(stInMsg, gs_DrvDecJpegInfo.pstImgInfo);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    DRV_JPEG_GetImgInfo(stInMsg,gs_DrvDecJpegInfo.pstImgInfo);

    return HI_SUCCESS;
}


HI_S32 HI_DRV_JPEG_DecOutInfo(HI_U32 DecHandle,HI_DRV_JPEG_OUTMSG_S *stOutMsg)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if ((NULL == stOutMsg) || (NULL == gs_DrvDecJpegInfo.pstImgInfo))
    {
        return HI_FAILURE;
    }

    stOutMsg->DecHandle = DecHandle;
    s32Ret = DRV_JPEG_GetSofn(stOutMsg,gs_DrvDecJpegInfo.pstImgInfo);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 HI_DRV_JPEG_DecFrame(HI_U32 DecHandle,HI_JPEG_DECINFO_S *pstDecInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if ((0 == DecHandle) || (NULL == pstDecInfo) || (NULL == gs_DrvDecJpegInfo.pstImgInfo))
    {
         JPEG_TRACE("the dec handle has been destoryed\n");
         return HI_FAILURE;
    }

    pstDecInfo->stInMsg.DecHandle  = DecHandle;
    pstDecInfo->stOutMsg.DecHandle = DecHandle;

#ifndef HI_BUILD_IN_BOOT
    s32Ret = JPEG_DRV_GetDevice();
    if (HI_SUCCESS != s32Ret)
    {
        JPEG_TRACE("JPEG_DRV_GetDevice failure\n");
        return HI_FAILURE;
    }
#else
    JPEG_OpenDev();
#endif

    s32Ret = JPEG_DRV_HardDec(pstDecInfo,gs_DrvDecJpegInfo.pstImgInfo);
    if (HI_SUCCESS != s32Ret)
    {
#ifdef HI_BUILD_IN_BOOT
        JPEG_CloseDev();
#endif
        return HI_FAILURE;
    }

#ifndef HI_BUILD_IN_BOOT
    JPEG_DRV_UnLock(JPG_INTTYPE_FINISH);
#endif

    return HI_SUCCESS;
}


HI_VOID HI_DRV_JPEG_DestoryDec(HI_U32 DecHandle)
{

    HI_UNUSED(DecHandle);

    if (0 == gs_DrvDecJpegInfo.DecHandle)
    {
        return;
    }

    if (1 != gs_DrvDecJpegInfo.DecHandle)
    {
        gs_DrvDecJpegInfo.DecHandle--;
        return;
    }

    gs_DrvDecJpegInfo.DecHandle--;

    if (NULL == gs_DrvDecJpegInfo.pstImgInfo)
    {
        return;
    }

#ifdef HI_BUILD_IN_BOOT
    HI_GFX_Free((HI_CHAR*)gs_DrvDecJpegInfo.pstImgInfo);
    JPEG_CloseDev();
#else
    HI_GFX_KFREE(HIGFX_JPGDEC_ID, (HI_VOID *)gs_DrvDecJpegInfo.pstImgInfo);
    gs_DrvDecJpegInfo.pstImgInfo = NULL;
#endif

    return;
}

#ifdef HI_BUILD_IN_BOOT
static inline HI_VOID JPEG_CloseDev(HI_VOID)
{
    volatile U_PERI_CRG31 unTempValue;
  #ifdef CONFIG_GFX_MMU_CLOCK
    volatile U_PERI_CRG205 unTempSmmuValue;
  #endif

    unTempValue.u32 = g_pstRegCrg->PERI_CRG31.u32;
    unTempValue.bits.jpgd_srst_req  = 0x1;
    g_pstRegCrg->PERI_CRG31.u32 = unTempValue.u32;
    udelay(1000);
    unTempValue.bits.jpgd_cken      = 0x0;
    g_pstRegCrg->PERI_CRG31.u32 = unTempValue.u32;

  #ifdef CONFIG_GFX_MMU_CLOCK
    unTempSmmuValue.u32 = g_pstRegCrg->PERI_CRG205.u32;
    unTempSmmuValue.bits.jpgd_smmu_srst_req = 0x1;
    g_pstRegCrg->PERI_CRG205.u32 = unTempSmmuValue.u32;
    udelay(1000);
    unTempSmmuValue.bits.jpgd_smmu_cken = 0x0;
    g_pstRegCrg->PERI_CRG205.u32 = unTempSmmuValue.u32;
  #endif
    return;
}
#endif

HI_VOID HI_DRV_JPEG_GetLuPixSum(HI_U32 DecHandle,HI_U64* pu64LuPixValue)
{
    HI_UNUSED(DecHandle);
    CHECK_POINT_NULL_UNRETURN(pu64LuPixValue);

    *pu64LuPixValue = gs_DrvDecJpegInfo.pstImgInfo->u64LuPixValue;
    return;
}
#endif
