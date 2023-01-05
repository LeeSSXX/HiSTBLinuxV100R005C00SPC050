/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : jpeg_hdec_setpara.c
Version           : Initial Draft
Author            :
Created          : 2017/07/05
Description     : write decode register
Function List   :


History           :
Date                            Author                    Modification
2017/07/05                   sdk                       Created file
******************************************************************************/


/****************************  add include here     *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "jerror.h"

#include "hi_jpeg_config.h"
#include "hi_jpeg_checkpara.h"
#include "hi_jpeg_reg.h"
#include "hi_gfx_sys.h"

#include "jpeg_hdec_api.h"
#include "jpeg_hdec_mem.h"
#include "jpeg_hdec_rwreg.h"

#define JPEG_SDEC_HUFF_REGISTER
#include "jpeg_sdec_table.h"
#undef JPEG_SDEC_HUFF_REGISTER
/***************************** Macro Definition     ********************************/


/***************************** Structure Definition *******************************/
typedef enum tagJPEG_MEMTYPE_E
{
    JPEG_STREAM_MEM_MMU_TYPE            = 0X1,    /**< 0bit �����ڴ�ΪMMU����                            >**/
    JPEG_YOUTPUT_MEM_MMU_TYPE           = 0X2,    /**< 1bit Y��������ڴ�ΪMMU����              >**/
    JPEG_UVOUTPUT_MEM_MMU_TYPE          = 0X4,    /**< 2bit UV��������ڴ�ΪMMU����            >**/
    JPEG_XRGBSAMPLE0_READ_MEM_MMU_TYPE  = 0X8,    /**< RGB�����Ҫ���ϲ���0buffer������ >**/
    JPEG_XRGBSAMPLE1_READ_MEM_MMU_TYPE  = 0X10,   /**< RGB�����Ҫ���ϲ���1buffer������ >**/
    JPEG_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE = 0X20,   /**< RGB�����Ҫ���ϲ���0bufferд���� >**/
    JPEG_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE = 0X40,   /**< RGB�����Ҫ���ϲ���1bufferд���� >**/
    JPEG_XRGBOUTPUT_MEM_MMU_TYPE        = 0X80,   /**< 7bitRGB����ڴ�ΪMMU����                       >**/
    JPEG_CSCOUTPUT_MEM_MMU_TYPE         = 0X100,  /**< 7bitRGB����ڴ�ΪMMU����                       >**/
    JPEG_MEMTYPE_BUTT
}JPEG_MEMTYPE_E;


/********************** Global Variable declaration ********************************/

/********************** API forward declarations    ********************************/

/**********************       API realization       ***********************************/
static HI_VOID JPEG_HDEC_ImagFmt2HardFmt(const HI_JPEG_FMT_E enJpegFmt,HI_U32 *pu32Fmt)
{
    switch (enJpegFmt)
    {
        case JPEG_FMT_YUV400:
           *pu32Fmt = 0;
           break;
        case JPEG_FMT_YUV420:
           *pu32Fmt = 3;
           break;
        case JPEG_FMT_YUV422_21:
           *pu32Fmt = 4;
            break;
        case JPEG_FMT_YUV422_12:
           *pu32Fmt = 5;
           break;
        case JPEG_FMT_YUV444:
           *pu32Fmt = 6;
           break;
        default:
           *pu32Fmt = 7;
           break;
    }

    return;
}

static HI_VOID JPEG_HDEC_SetDqt(const struct jpeg_decompress_struct *cinfo)
{
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);
    JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, (HI_VOID *)pJpegHandle->s32QuantTab, JPGD_REG_QUANT, sizeof(pJpegHandle->s32QuantTab));
    return;
}

static HI_VOID JPEG_HDEC_SetDhtDc(const struct jpeg_decompress_struct *cinfo)
{
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    if (HI_FALSE == pJpegHandle->bHasHufTable)
    {
       JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr,gs_HuffDcTab,JPGD_REG_HDCTABLE,48);
       return;
    }

    JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr,pJpegHandle->u32HuffDcTab,JPGD_REG_HDCTABLE,48);

    return;
}

static HI_VOID JPEG_HDEC_SetDhtAc(const struct jpeg_decompress_struct *cinfo)
{
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    if (HI_FALSE == pJpegHandle->bHasHufTable)
    {
       JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, gs_HufAcMinTab,   JPGD_REG_HACMINTABLE,  32);
       JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, gs_HufAcBaseTab,  JPGD_REG_HACBASETABLE, 32);
       JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, gs_HufAcSymbolTab,JPGD_REG_HACSYMTABLE,  0x2c0);
       return;
    }

    JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, pJpegHandle->u32HufAcMinTab,  JPGD_REG_HACMINTABLE,  32);
    JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, pJpegHandle->u32HufAcBaseTab, JPGD_REG_HACBASETABLE, 32);
    /**< 0x2c0 = ac_max_sum_syms * 4  >**/
    /**< CNcomment: ֵ0x2c0 = ac_max_sum_syms * 4�������  >**/
    JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, pJpegHandle->u32HufAcSymbolTab,JPGD_REG_HACSYMTABLE,  0x2c0);

    return;
}

static HI_VOID JPEG_HDEC_SetDht(const struct jpeg_decompress_struct *cinfo)
{
   JPEG_HDEC_SetDhtDc(cinfo);
   JPEG_HDEC_SetDhtAc(cinfo);
   return;
}

static HI_S32 JPEG_HDEC_SetSof(const struct jpeg_decompress_struct *cinfo)
{
    HI_U32 DefaultScale    = 0;
    HI_U32 u32Stride       = 0;
    HI_U32 u32HardFmt      = 0;

    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    JPEG_HDEC_ImagFmt2HardFmt(pJpegHandle->enImageFmt,&u32HardFmt);
    if (u32HardFmt >= 7)
    {
        return HI_FAILURE;
    }
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_PICTYPE, u32HardFmt);

#ifdef CONFIG_JPEG_HARDDEC2ARGB
    if (HI_TRUE == pJpegHandle->bDecOutColorSpaecXRGB)
    {
       JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_SCALE, (0x30 | 0x8));
    }
    else
#endif
    {
        DefaultScale = 0x34;
        JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_SCALE, ((DefaultScale & 0xfffffffc) | pJpegHandle->ScalRation | 0x8));
    }

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_PICSIZE, pJpegHandle->stJpegSofInfo.u32InWandH);

    u32Stride = (pJpegHandle->stJpegSofInfo.u32CbCrStride << 16 ) | pJpegHandle->stJpegSofInfo.u32YStride;
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_STRIDE,u32Stride);

    return HI_SUCCESS;
}

static HI_VOID JPEG_HDEC_SetSampleFactor(const struct jpeg_decompress_struct *cinfo)
{
    HI_U8 u8YFac = 0,u8UFac = 0,u8VFac = 0;
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    u8YFac = (HI_U8)(((pJpegHandle->u8Fac[0][0] << 4) | pJpegHandle->u8Fac[0][1]) & 0xff);
    u8UFac = (HI_U8)(((pJpegHandle->u8Fac[1][0] << 4) | pJpegHandle->u8Fac[1][1]) & 0xff);
    u8VFac = (HI_U8)(((pJpegHandle->u8Fac[2][0] << 4) | pJpegHandle->u8Fac[2][1]) & 0xff);
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_SAMPLINGFACTOR,((u8YFac << 16) |(u8UFac << 8) | u8VFac));

    return;
}

static HI_VOID JPEG_HDEC_SetDri(const struct jpeg_decompress_struct *cinfo)
{
    HI_U32 DriValue = 0;
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    DriValue = cinfo->restart_interval;
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_DRI,DriValue);

    return;
}

static HI_VOID JPEG_HDEC_SetMemtype(const struct jpeg_decompress_struct *cinfo)
{
    /**=========================================================================
         ** [0] :  1 MMZ   0 MMU   �������ڴ�����
         ** [1] :  1 MMZ   0 MMU   Y��������ڴ�����
        ** [2] :  1 MMZ   0 MMU   UV��������ڴ�����
        **======================================================================**/
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);
    HI_U32 u32Mask =   JPEG_STREAM_MEM_MMU_TYPE            \
                     | JPEG_YOUTPUT_MEM_MMU_TYPE           \
                     | JPEG_UVOUTPUT_MEM_MMU_TYPE          \
                     | JPEG_XRGBSAMPLE0_READ_MEM_MMU_TYPE  \
                     | JPEG_XRGBSAMPLE1_READ_MEM_MMU_TYPE  \
                     | JPEG_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE \
                     | JPEG_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE \
                     | JPEG_XRGBOUTPUT_MEM_MMU_TYPE;

#ifdef CONFIG_GFX_MMU_SUPPORT
    u32Mask &= ~JPEG_STREAM_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_YOUTPUT_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_UVOUTPUT_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_XRGBSAMPLE0_READ_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_XRGBSAMPLE1_READ_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE;
    u32Mask &= ~JPEG_XRGBOUTPUT_MEM_MMU_TYPE;
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_MMU_BYPASS,u32Mask);
#else
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_MMU_BYPASS,u32Mask);
#endif
    return;
}

static HI_VOID JPEG_HDEC_SetDecOutBuf(const struct jpeg_decompress_struct * cinfo)
{
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    if (HI_TRUE == pJpegHandle->bDecOutColorSpaecXRGB)
    {
        JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_YSTADDR,pJpegHandle->stDecSurface.OutPhy);
        return;
    }

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_YSTADDR,pJpegHandle->stDecSurface.DecOutPhy[0]);
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_UVSTADDR,pJpegHandle->stDecSurface.DecOutPhy[1]);

    return;
}


#ifdef CONFIG_JPEG_HARDDEC2ARGB
static HI_VOID JPEG_HDEC_SetXRGBFmt(const struct jpeg_decompress_struct *cinfo)
{
    HI_U32 OutType = 0x0;
    HI_U32 OutPut  = 0x0;
    JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    OutPut = (HI_U32)JPEG_HDEC_ReadReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTTYPE);

    if ((JCS_EXT_BGRX == cinfo->out_color_space) || (JCS_EXT_BGRA == cinfo->out_color_space))
    {
        /**< JCS_EXT_RGBX and JCS_EXT_RGBA and JCS_RGBA_8888**/
        OutType  =  0x000;
    }
    else if ((JCS_EXT_RGBX == cinfo->out_color_space) || (JCS_EXT_RGBA == cinfo->out_color_space) || (JCS_RGBA_8888 == cinfo->out_color_space))
    {
        OutType  =  0x100;
    }
    else if (JCS_RGBA_5551 == cinfo->out_color_space)
    {
        OutType  =  0x300;
    }
    else if (JCS_BGRA_5551 == cinfo->out_color_space)
    {
        OutType  =  0x200;
    }
    else if (JCS_BGR565 == cinfo->out_color_space)
    {
        OutType  =  0x500;
    }
    else if ((JCS_RGB565 == cinfo->out_color_space) || (JCS_RGB_565 == cinfo->out_color_space))
    {
        OutType  =  0x400;
    }
    else if ((JCS_RGB == cinfo->out_color_space) || (JCS_EXT_RGB == cinfo->out_color_space))
    {
        OutType  =  0x700;
    }
    else
    {
        OutType  =  0x600;
    }

    OutPut = OutPut | OutType;

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTTYPE,OutPut);

    return;
}


static HI_VOID JPEG_HDEC_SetDecARGBPara(const struct jpeg_decompress_struct *cinfo)
{
    HI_U32 CropX         = 0;
    HI_U32 CropY         = 0;
    HI_U32 CropStargPos  = 0;
    HI_U32 CropEndX      = 0;
    HI_U32 CropEndY      = 0;
    HI_U32 CropEndPos    = 0;

    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    CropX    = pJpegHandle->stCropRect.x;
    CropY    = pJpegHandle->stCropRect.y;
    CropEndX = (CropX + pJpegHandle->stCropRect.w - 1);
    CropEndY = (CropY + pJpegHandle->stCropRect.h - 1);

    CropStargPos = (CropX | (CropY << 16));
    CropEndPos   = (CropEndX | (CropEndY << 16));

    if (HI_TRUE == pJpegHandle->bOutUsrBuf)
    {
        JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_ARGBOUTSTRIDE,pJpegHandle->stOutSurface.u32OutStride[0]);
    }
    else
    {
        JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_ARGBOUTSTRIDE,pJpegHandle->stJpegSofInfo.DecOutStride);
    }

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTSTARTPOS,CropStargPos);
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTENDPOS,CropEndPos);

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_ALPHA,0XFF);

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_MINADDR,  pJpegHandle->u32MinPhyBuf);
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_MINADDR1, pJpegHandle->u32MinPhyBuf  + pJpegHandle->stJpegSofInfo.u32MINSize);

    JPEG_HDEC_SetXRGBFmt(cinfo);

    return;
}

static HI_VOID JPEG_HDEC_SetCropAndARGBInfo(const struct jpeg_decompress_struct *cinfo)
{
    JPEG_HDEC_SetDecARGBPara(cinfo);

    return;
}
#endif


#ifdef CONFIG_JPEG_OUTPUT_YUV420SP
static HI_VOID JPEG_HDEC_SetYUV420SPInfo(const struct jpeg_decompress_struct *cinfo)
{
    JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTTYPE,0x4);
    return;
}
#endif


#ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
static HI_VOID JPEG_HDEC_SetLuPixSumInfo(const struct jpeg_decompress_struct *cinfo)
{
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);
    if (HI_FALSE == pJpegHandle->bOutYCbCrSP)
    {
       return;
    }

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_LPIXSUM1,0x80000000);

    return;
}
#endif


#ifdef CONFIG_JPEG_LOW_DELAY_SUPPORT
static HI_S32 JPEG_HDEC_SetLowDelayInf(const struct jpeg_decompress_struct *cinfo)
{
    HI_U32 McuLineNum = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_JPEG_OUTPUTINFO_S stOutPutInfo;
    JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

    if (HI_TRUE != pJpegHandle->bLowDelayEn)
    {
        return HI_SUCCESS;
    }

    if (cinfo->output_height > 720)
    {
        McuLineNum = 8;
    }
    else
    {
        McuLineNum = 4;
    }

    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_LINE_ADDR,pJpegHandle->u32PhyLineBuf);
    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_LINE_NUM,McuLineNum);

    HI_GFX_Memset(&stOutPutInfo, 0x0, sizeof(HI_JPEG_OUTPUTINFO_S));
    stOutPutInfo.pVirLineBuf     = pJpegHandle->pVirLineBuf;
    stOutPutInfo.u32OutHeight[0] = pJpegHandle->stJpegSofInfo.u32YMcuHeight;
    stOutPutInfo.u32OutHeight[1] = pJpegHandle->stJpegSofInfo.u32CMcuHeight;
    s32Ret = ioctl(pJpegHandle->JpegDev, CMD_JPG_LOWDELAY_LINEINFO, &stOutPutInfo);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

HI_S32 JPEG_HDEC_SetPara(const struct jpeg_decompress_struct *cinfo)
{
    HI_S32 Ret = HI_SUCCESS;

    CHECK_JPEG_EQUAL_RETURN(NULL, cinfo, HI_FAILURE);
    JPEG_API_MODULE_DEFINE_HANDLE();

    /**< set dqt table register >**/
    /**< CNcomment: ����������21us CNend>**/
    JPEG_HDEC_SetDqt(cinfo);

    /**<set dht table register>**/
    /**<CNcomment: ���ù�������100us ~ 120us CNend>**/
    JPEG_HDEC_SetDht(cinfo);

    /**<set sof message>**/
    /**<CNcomment: ����SOF��Ϣ CNend  >**/
    Ret = JPEG_HDEC_SetSof(cinfo);
    if  (HI_SUCCESS != Ret)
    {
       return HI_FAILURE;
    }

    /**< set sample factor message >**/
    /**< CNcomment: ���ò���������Ϣ CNend >**/
    JPEG_HDEC_SetSampleFactor(cinfo);

    /**< set dri value>**/
    /**<  CNcomment: ����dri��ֵ CNend>**/
    JPEG_HDEC_SetDri(cinfo);

    /**< set mem type>**/
    /**<  CNcomment: �����ڴ����� CNend >**/
    JPEG_HDEC_SetMemtype(cinfo);

    /**< et middle buffer register>**/
    /**< CNcomment: ����JPEGӲ����������Ĵ��� CNend>**/
    JPEG_HDEC_SetDecOutBuf(cinfo);

#ifdef CONFIG_JPEG_HARDDEC2ARGB
    /**< set about dec out argb register>**/
       /**< CNcomment: ���ý������ΪARGB�����Ϣ CNend>**/
    if (HI_TRUE == pJpegHandle->bDecOutColorSpaecXRGB)
    {
       JPEG_HDEC_SetCropAndARGBInfo(cinfo);
    }
#endif

#ifdef CONFIG_JPEG_OUTPUT_YUV420SP
    /**< set all decode output yuv420sp informationv>**/
    /**< CNcomment: ���ý���ͳһ���yuvsp420sp�Ĵ��� CNend>**/
    if (HI_TRUE == pJpegHandle->bOutYUV420SP)
    {
       JPEG_HDEC_SetYUV420SPInfo(cinfo);
    }
#endif

#ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
    /**<  set count lu pixle sum value>**/
    /**< CNcomment: ����ͳ������ֵ�Ĵ��� CNend>**/
    JPEG_HDEC_SetLuPixSumInfo(cinfo);
#endif

#ifdef CONFIG_JPEG_LOW_DELAY_SUPPORT
    Ret = JPEG_HDEC_SetLowDelayInf(cinfo);
    if (HI_SUCCESS != Ret)
    {
       return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}
