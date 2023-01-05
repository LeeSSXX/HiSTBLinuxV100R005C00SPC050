/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name          : jpeg_decode.c
Version             : Initial Draft
Author              :
Created            : 2017/06/08
Description       : decode images
                          CNcomment: JPEGͼƬ����CNend\n
Function List     :
History             :
Date                                 Author                     Modification
2017/06/08                     sdk                         Created file
******************************************************************************/


/***************************** add include here*********************************/
#include "hi_gfx_surface.h"
#include "hi_gfx_comm.h"
#include "hi_gfx_layer.h"
#include "hi_gfx_sys.h"

#include "jpeg_decode.h"
#include "hi_drv_jpeg.h"

/***************************** Macro Definition ********************************/

/***************************** Structure Definition ****************************/

/***************************** Global Variable declaration *********************/
static HI_DRV_JPEG_INMSG_S  gs_stInMsg;
static HI_DRV_JPEG_OUTMSG_S gs_stOutMsg;

/***************************** API forward declarations ************************/

static HI_S32 CalcScale(HI_U32 Width, HI_U32 Height, HI_BOOL *pVoDispSupport);

/***************************** API realization *********************************/
HI_S32 JPEG_ADP_CreateDecode(HI_U32 *pDecHandle,HI_CHAR* pSrcBuf,HI_U32 u32SrcLen)
{
    HI_S32 Ret = HI_SUCCESS;
    HI_U32 DecHandle = 0;
    if ((NULL == pDecHandle) || (NULL == pSrcBuf))
    {
       HI_GFX_Log();
       return HI_FAILURE;
    }

    HI_GFX_Memset(&gs_stInMsg, 0,sizeof(HI_DRV_JPEG_INMSG_S));
    HI_GFX_Memset(&gs_stOutMsg,0,sizeof(HI_DRV_JPEG_OUTMSG_S));
    gs_stInMsg.u32SavePhy    = (HI_U32)pSrcBuf;
    gs_stInMsg.u64SaveVir[0] = (HI_U64)(unsigned long)pSrcBuf;
    gs_stInMsg.u32SaveLen[0] = u32SrcLen;
    gs_stInMsg.bUserPhyMem   = HI_TRUE;

    Ret = HI_DRV_JPEG_CreateDec(&DecHandle);
    if (HI_SUCCESS != Ret)
    {
        HI_GFX_Log();
        return HI_FAILURE;
    }

    *pDecHandle = (unsigned long)DecHandle;

    return HI_SUCCESS;
}



HI_S32 JPEG_ADP_GetImgInfo(HI_U32 DecHandle,HI_HANDLE hSurface)
{

    HI_S32 Ret = HI_SUCCESS;
    HI_LOGO_SURFACE_S *pstSurface = (HI_LOGO_SURFACE_S*)hSurface;
    CHECK_POINT_NULL(pstSurface);

    Ret = HI_DRV_JPEG_DecInfo(DecHandle,&gs_stInMsg);
    if (HI_SUCCESS != Ret)
    {
        HI_GFX_Log();
        return HI_FAILURE;
    }

    gs_stOutMsg.s32Scale = CalcScale(gs_stInMsg.s32InWidth,gs_stInMsg.s32InHeight, &(pstSurface->bVoDispSupport));
    gs_stOutMsg.bOutYuvSp420 = (HI_TRUE == pstSurface->bVoDispSupport) ? (HI_TRUE) : (HI_FALSE);

    Ret = HI_DRV_JPEG_DecOutInfo(DecHandle,&gs_stOutMsg);
    if (HI_SUCCESS != Ret)
    {
        HI_GFX_Log();
        return HI_FAILURE;
    }

    pstSurface->u32Width[0]  = gs_stOutMsg.u32OutWidth[0];
    pstSurface->u32Height[0] = gs_stOutMsg.u32OutHeight[0];
    pstSurface->u32Stride[0] = gs_stOutMsg.u32OutStride[0];
    pstSurface->u32Size[0]   = gs_stOutMsg.u32OutSize[0];
    pstSurface->u32Width[1]  = gs_stOutMsg.u32OutWidth[1];
    pstSurface->u32Height[1] = gs_stOutMsg.u32OutHeight[1];
    pstSurface->u32Stride[1] = gs_stOutMsg.u32OutStride[1];
    pstSurface->u32Size[1]   = gs_stOutMsg.u32OutSize[1];
    switch (gs_stOutMsg.enOutFmt)
    {
        case JPG_FMT_YUV400:
            pstSurface->enPixelFormat = HI_GFX_PF_YUV400;
            break;
        case JPG_FMT_YUV420:
            pstSurface->enPixelFormat = HI_GFX_PF_YUV420;
            break;
        case JPG_FMT_YUV444:
            pstSurface->enPixelFormat = HI_GFX_PF_YUV444;
            break;
        case JPG_FMT_YUV422_12:
            pstSurface->enPixelFormat = HI_GFX_PF_YUV422_12;
            break;
        case JPG_FMT_YUV422_21:
            pstSurface->enPixelFormat = HI_GFX_PF_YUV422_21;
            break;
        default:
            HI_GFX_Log();
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static HI_S32 CalcScale(HI_U32 Width, HI_U32 Height, HI_BOOL *pVoDispSupport)
{
    HI_S32 Scale = 0;
    *pVoDispSupport = HI_FALSE;

    if( (Width >= CONFIG_GFX_DISP_REALLY4K_VIRTUAL_WIDTH * 2) || (Height >= CONFIG_GFX_DISP_4K_VIRTUAL_HEIGHT * 2))
    {
        Scale = 8;
    }
    else if ( (Width > CONFIG_GFX_DISP_REALLY4K_VIRTUAL_WIDTH) || (Height > CONFIG_GFX_DISP_4K_VIRTUAL_HEIGHT))
    {
        Scale = 4;
    }
    else
    {
        Scale = 1;
    }

#ifdef CONFIG_GFX_SUPPORT_VO_4K
    if ( (Width >= CONFIG_GFX_DISP_4K_VIRTUAL_WIDTH) && (Height >= CONFIG_GFX_DISP_4K_VIRTUAL_HEIGHT))
    {
        *pVoDispSupport = HI_TRUE;
        Scale = 1;
    }
#endif

    return Scale;
}


HI_S32 JPEG_ADP_StartDecode(HI_U32 DecHandle, HI_HANDLE hSurface)
{
    HI_S32 Ret = HI_SUCCESS;
    HI_LOGO_SURFACE_S *pstSurface = (HI_LOGO_SURFACE_S*)hSurface;
    HI_JPEG_DECINFO_S stDecInfo;

    CHECK_POINT_NULL(pstSurface);

    HI_GFX_Memset(&stDecInfo,0,sizeof(HI_JPEG_DECINFO_S));

    gs_stOutMsg.u32OutPhy[0]   = (HI_U32)pstSurface->pPhyAddr[0];
    gs_stOutMsg.u32OutPhy[1]   = (HI_U32)pstSurface->pPhyAddr[1];

#if 0
    HI_GFX_Print("\n=============================================\n");
    HI_GFX_Print("gs_stOutMsg.pOutPhy[0]    = 0x%x\n",gs_stOutMsg.u32OutPhy[0]);
    HI_GFX_Print("gs_stOutMsg.pOutPhy[1]    = 0x%x\n",gs_stOutMsg.u32OutPhy[1]);
    HI_GFX_Print("gs_stOutMsg.u32OutWidth[0]     = %d\n",gs_stOutMsg.u32OutWidth[0]);
    HI_GFX_Print("gs_stOutMsg.u32OutHeight[0]    = %d\n",gs_stOutMsg.u32OutHeight[0]);
    HI_GFX_Print("gs_stOutMsg.u32OutStride[0]    = %d\n",gs_stOutMsg.u32OutStride[0]);
    HI_GFX_Print("gs_stOutMsg.u32OutWidth[1]     = %d\n",gs_stOutMsg.u32OutWidth[1]);
    HI_GFX_Print("gs_stOutMsg.u32OutHeight[1]    = %d\n",gs_stOutMsg.u32OutHeight[1]);
    HI_GFX_Print("gs_stOutMsg.u32OutStride[1]    = %d\n",gs_stOutMsg.u32OutStride[1]);
    HI_GFX_Print("gs_stOutMsg.u32OutSize[0]      = %d\n",gs_stOutMsg.u32OutSize[0]);
    HI_GFX_Print("gs_stOutMsg.u32OutSize[1]      = %d\n",gs_stOutMsg.u32OutSize[1]);
    HI_GFX_Print("gs_stOutMsg.enOutFmt           = %d\n",gs_stOutMsg.enOutFmt);
    HI_GFX_Print("=============================================\n");
#endif

    stDecInfo.stInMsg   = gs_stInMsg;
    stDecInfo.stOutMsg  = gs_stOutMsg;
    Ret = HI_DRV_JPEG_DecFrame(DecHandle,&stDecInfo);
    if (HI_SUCCESS != Ret)
    {
        HI_GFX_Log();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


HI_S32 JPEG_ADP_DestroyDecode(HI_U32 DecHandle)
{
    HI_DRV_JPEG_DestoryDec(DecHandle);
    return HI_SUCCESS;
}
