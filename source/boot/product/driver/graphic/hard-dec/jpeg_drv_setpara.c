/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : jpeg_drv_setpara.c
Version            : Initial Draft
Author             :
Created           : 2017/06/08
Description       :
Function List    :


History           :
Date                        Author                Modification
2017/06/08            sdk                  Created file
******************************************************************************/

/*********************************add include here******************************/

#include "hi_jpeg_reg.h"
#include "jpeg_drv_parse.h"
#include "jpeg_drv_error.h"
#include "hi_gfx_sys_k.h"

#ifdef CONFIG_JPEG_OMX_FUNCTION


/***************************** Macro Definition ******************************/

//#define CONIFG_SETPARA_DEBUG

/*************************** Structure Definition ****************************/


/********************** Global Variable declaration **************************/


/******************************* API forward declarations *******************/

extern HI_VOID DRV_JPEG_SetDqt(HI_DRV_JPEG_OUTMSG_S *pstOutMsg,IMAG_INFO_S *pstImgInfo);
extern HI_S32 DRV_JPEG_SetDht(HI_DRV_JPEG_OUTMSG_S *pstOutMsg,IMAG_INFO_S *pstImgInfo);

/******************************* API realization *****************************/
/***************************************************************************************
* func            : jpeg_set_streambuf
* description    : set stream buffer
                  CNcomment: ��������buffer CNend\n
* others:        : NA
***************************************************************************************/
static HI_S32 jpeg_set_streambuf(HI_DRV_JPEG_INMSG_S *pstInMsg,IMAG_INFO_S *pstImgInfo)
{
    HI_U32 u32StartBufPhy   = 0;
    HI_U32 u32EndBufPhy     = 0;
    HI_U32 u32StartDataPhy  = 0;
    HI_U32 u32EndDataPhy    = 0;

    if (0 != pstInMsg->u32SaveLen[1])
    {
        return HI_FAILURE;
    }

    u32StartBufPhy   = pstInMsg->u32SavePhy - 128;
    u32StartBufPhy   = (u32StartBufPhy + 64 - 1) & (~(64 - 1));
    u32EndBufPhy     = pstInMsg->u32SavePhy + pstInMsg->u32SaveLen[0] + 128;

    u32StartDataPhy  = pstInMsg->u32SavePhy + pstImgInfo->u32CurPos[0];
    u32EndDataPhy    = pstInMsg->u32SavePhy + pstInMsg->u32SaveLen[0];

    /**<-- ����buffer  >**/
    jpeg_reg_write(JPGD_REG_STADDR, u32StartBufPhy);
    jpeg_reg_write(JPGD_REG_ENDADDR,u32EndBufPhy);

    /**<--�洢����buffer >**/
    jpeg_reg_write(JPGD_REG_STADD,  u32StartDataPhy);
    jpeg_reg_write(JPGD_REG_ENDADD, u32EndDataPhy);

    return HI_SUCCESS;
}

/***************************************************************************************
* func            : jpeg_set_outbuf
* description    : set output buffer
                  CNcomment: �������buffer CNend\n
* others:        : NA
***************************************************************************************/
static HI_VOID jpeg_set_outbuf(HI_DRV_JPEG_OUTMSG_S *pstOutMsg)
{
    jpeg_reg_write(JPGD_REG_YSTADDR, pstOutMsg->u32OutPhy[0]);
    jpeg_reg_write(JPGD_REG_UVSTADDR,pstOutMsg->u32OutPhy[1]);
}

/***************************************************************************************
* func            : jpeg_set_factor
* description    : ���ò�������
                  CNcomment: ���ò������� CNend\n
* others:        : NA
***************************************************************************************/
static HI_VOID jpeg_set_factor(HI_DRV_JPEG_OUTMSG_S *pstOutMsg,IMAG_INFO_S *pstImgInfo)
{
    HI_U8 u8YFac = 0,u8UFac = 0,u8VFac = 0;

    u8YFac = (HI_U8)(((pstImgInfo->u8Fac[0][0] << 4) | pstImgInfo->u8Fac[0][1]) & 0xff);
    u8UFac = (HI_U8)(((pstImgInfo->u8Fac[1][0] << 4) | pstImgInfo->u8Fac[1][1]) & 0xff);
    u8VFac = (HI_U8)(((pstImgInfo->u8Fac[2][0] << 4) | pstImgInfo->u8Fac[2][1]) & 0xff);
    jpeg_reg_write(JPGD_REG_SAMPLINGFACTOR,(HI_U32)((u8YFac << 16) |(u8UFac << 8) | u8VFac));
}

/***************************************************************************************
* func            : jpeg_set_yuv420sp
* description   : output yuv420 sp
                        CNcomment: ͳһ���yuv420 semi-planer CNend\n
* others:        : NA
***************************************************************************************/
static HI_VOID jpeg_set_yuv420sp(HI_VOID)
{
#ifdef CONFIG_JPEG_OUTPUT_YUV420SP
    jpeg_reg_write(JPGD_REG_OUTTYPE,0x4);
#endif
}

/***************************************************************************************
* func            : jpeg_set_lupixsum
* description  : output lu pixle sum
                         CNcomment: ͳһ������Ⱥ�ֵ CNend\n
* others:       : NA
***************************************************************************************/
static HI_VOID jpeg_set_lupixsum(HI_VOID)
{
#ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
    jpeg_reg_write(JPGD_REG_LPIXSUM1,(HI_U32)0x80000000);
#endif
}

/***************************************************************************************
* func              : jpeg_set_sofn
* description    : set sof message
                         CNcomment: ����ͼ�������Ϣ CNend\n
* others:        : NA
***************************************************************************************/
static HI_VOID  jpeg_set_sofn(HI_DRV_JPEG_OUTMSG_S *pstOutMsg,IMAG_INFO_S *pstImgInfo)
{

    HI_S32 s32Scale      = 0;
    HI_S32 s32TmpScale   = 0;
    HI_S32 s32Stride     = 0;
    HI_S32 s32YInWidth   = 0;
    HI_S32 s32YInHeight  = 0;
    HI_S32 s32PicSize    = 0;
    HI_S32 s32DefaultScale  = 0;

    switch (pstOutMsg->s32Scale)
    {
        case 1:
            s32TmpScale = 0;
            break;
        case 2:
            s32TmpScale = 1;
            break;
        case 4:
            s32TmpScale = 2;
            break;
        default:
            s32TmpScale = 3;
            break;
    }

    s32DefaultScale = (HI_S32)0x34;
    s32YInWidth     = (1 == pstImgInfo->u8Fac[0][0]) ? ((pstImgInfo->s32InWidth  + JPEG_DRV_MCU_8ALIGN - 1)>>3) : ((pstImgInfo->s32InWidth  + JPEG_DRV_MCU_16ALIGN - 1)>>4);
    s32YInHeight    = (1 == pstImgInfo->u8Fac[0][1]) ? ((pstImgInfo->s32InHeight + JPEG_DRV_MCU_8ALIGN - 1)>>3) : ((pstImgInfo->s32InHeight + JPEG_DRV_MCU_16ALIGN - 1)>>4);
    s32PicSize      = (s32YInWidth| (s32YInHeight << 16));
    s32Stride       = (HI_S32)((pstOutMsg->u32OutStride[1] << 16 ) | pstOutMsg->u32OutStride[0]);
    s32Scale        =  ((HI_U32)s32DefaultScale & 0xfffffffc) | s32TmpScale | 0x8;

    jpeg_reg_write(JPGD_REG_PICTYPE,  (HI_U32)pstImgInfo->eImgFmt);
    jpeg_reg_write(JPGD_REG_SCALE,    (HI_U32)s32Scale);
    jpeg_reg_write(JPGD_REG_PICSIZE,  (HI_U32)s32PicSize);
    jpeg_reg_write(JPGD_REG_STRIDE,   (HI_U32)s32Stride);

}


HI_VOID jpeg_set_dri(IMAG_INFO_S *pstImgInfo)
{
    jpeg_reg_write(JPGD_REG_DRI,  (HI_U32)pstImgInfo->s32RestartInterval);
}

/*****************************************************************************
* func            : jpeg_set_memmask
* description  : set the memory mask
                       CNcomment:  ����mem��Ϣ         CNend\n
* param[in]   :
* retval         : NA
* others:       : NA
*****************************************************************************/
static HI_VOID jpeg_set_memmask(HI_DRV_JPEG_OUTMSG_S *pstOutMsg)
{
    HI_U32 u32Mask =   JPEG_DRV_STREAM_MEM_MMU_TYPE            \
                     | JPEG_DRV_YOUTPUT_MEM_MMU_TYPE           \
                     | JPEG_DRV_UVOUTPUT_MEM_MMU_TYPE          \
                     | JPEG_DRV_XRGBSAMPLE0_READ_MEM_MMU_TYPE  \
                     | JPEG_DRV_XRGBSAMPLE1_READ_MEM_MMU_TYPE  \
                     | JPEG_DRV_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE \
                     | JPEG_DRV_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE \
                     | JPEG_DRV_XRGBOUTPUT_MEM_MMU_TYPE;

#ifdef CONFIG_GFX_MMU_SUPPORT
    u32Mask &= ~JPEG_DRV_STREAM_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_YOUTPUT_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_UVOUTPUT_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_XRGBSAMPLE0_READ_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_XRGBSAMPLE1_READ_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_DRV_XRGBOUTPUT_MEM_MMU_TYPE;
    jpeg_reg_write(JPGD_REG_MMU_BYPASS,(HI_U32)u32Mask);
#else
    /**<--  ��mmu����bypass  >**/
    jpeg_reg_write(JPGD_REG_MMU_BYPASS,(HI_U32)u32Mask);
#endif
}


HI_S32 DRV_JPEG_SetPara(HI_JPEG_DECINFO_S *pstDecInfo,IMAG_INFO_S *pstImgInfo)
{

    HI_S32 s32Ret = HI_SUCCESS;

#ifdef CONIFG_SETPARA_DEBUG
    mdelay(100);
    JPEG_TRACE("\n===============================================%s %d\n",__func__,__LINE__);
    JPEG_TRACE("begin DRV_JPEG_SetDqt\n");
#endif
    if ((NULL == pstDecInfo) || (NULL == pstImgInfo))
    {
       return HI_FAILURE;
    }

    /** set dqt **/
    DRV_JPEG_SetDqt(&(pstDecInfo->stOutMsg),pstImgInfo);

     /**set dri value **/
    jpeg_set_dri(pstImgInfo);

    jpeg_set_memmask(&(pstDecInfo->stOutMsg));

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end DRV_JPEG_SetDqt\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin DRV_JPEG_SetDht\n");
#endif

    /** set dht **/
    s32Ret = DRV_JPEG_SetDht(&(pstDecInfo->stOutMsg),pstImgInfo);
    if (HI_SUCCESS != s32Ret)
    {
        JPEG_TRACE("set huff table failure\n");
        return HI_ERR_JPEG_SERPARA;
    }

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end DRV_JPEG_SetDht\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin jpeg_set_sofn\n");
#endif

    s32Ret = DRV_JPEG_GetSofn(&(pstDecInfo->stOutMsg),pstImgInfo);
    if (HI_SUCCESS != s32Ret)
    {
        JPEG_TRACE("jpeg_get_sofn failure\n");
        return HI_ERR_JPEG_SERPARA;
    }
    jpeg_set_sofn(&(pstDecInfo->stOutMsg),pstImgInfo);

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end jpeg_set_sofn\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin jpeg_set_factor\n");
#endif

    jpeg_set_factor(&(pstDecInfo->stOutMsg),pstImgInfo);

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end jpeg_set_factor\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin jpeg_set_yuv420sp\n");
#endif

    if (HI_TRUE == pstDecInfo->stOutMsg.bOutYuvSp420)
    {
       if (8 == pstDecInfo->stOutMsg.s32Scale)
       {
              return HI_ERR_JPEG_SERPARA;
       }
       jpeg_set_yuv420sp();
    }

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end jpeg_set_yuv420sp\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin jpeg_set_lupixsum\n");
#endif

    if (HI_TRUE == pstDecInfo->stOutMsg.bLuPixSum)
    {
        jpeg_set_lupixsum();
    }

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end jpeg_set_lupixsum\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin jpeg_set_streambuf\n");
#endif

    if (HI_TRUE == pstDecInfo->stInMsg.bUserPhyMem)
    {
        s32Ret = jpeg_set_streambuf(&(pstDecInfo->stInMsg),pstImgInfo);
    }
    else
    {
        JPEG_TRACE("set buffer para failure\n");
        return HI_ERR_JPEG_SERPARA;
    }

    if (HI_SUCCESS != s32Ret)
    {
        JPEG_TRACE("set buffer para failure\n");
        return HI_ERR_JPEG_SERPARA;
    }

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end jpeg_set_streambuf\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
    JPEG_TRACE("begin jpeg_set_outbuf\n");
#endif

    jpeg_set_outbuf(&(pstDecInfo->stOutMsg));

#ifdef CONIFG_SETPARA_DEBUG
    JPEG_TRACE("end jpeg_set_outbuf\n");
    JPEG_TRACE("===============================================%s %d\n",__func__,__LINE__);
    mdelay(100);
#endif

    return HI_SUCCESS;
}
#endif
