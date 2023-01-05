/******************************************************************************
*
* Copyright (C) 2016 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name       : drv_hifb_adp.h
Version         : Initial Draft
Author          :
Created         : 2016/02/02
Description     :
Function List   :


History         :
Date                Author                Modification
2016/02/02            sdk              Created file
******************************************************************************/
#ifndef __DRV_HIFB_ADP_H__
#define __DRV_HIFB_ADP_H__


/*********************************add include here******************************/
#ifndef HI_BUILD_IN_BOOT
#include "hi_type.h"
#include "hi_common.h"
#include "drv_disp_ext.h"
#include "drv_hifb_common.h"
#else
#include "hifb_debug.h"
#include "hi_drv_disp.h"
#endif

#include "drv_hifb_hal.h"


#include "hi_gfx_comm_k.h"
#include "optm_define.h"
#ifdef CONFIG_GFX_PQ
#include "drv_pq_ext.h"
#endif

/*****************************************************************************/


/*****************************************************************************/



#ifdef __cplusplus
#if __cplusplus
   extern "C" {
#endif
#endif /* __cplusplus */


/***************************** Macro Definition ******************************/

#ifdef HI_BUILD_IN_BOOT
#define HIFB_ERROR(fmt...)              HI_ERR_PRINT(HI_ID_FB, fmt)
#define HIFB_WARNING(fmt...)            HI_WARN_PRINT(HI_ID_FB, fmt)
#define HIFB_INFO(fmt...)               HI_INFO_PRINT(HI_ID_FB, fmt)
#endif

#define OPTM_ENABLE                      1
#define OPTM_DISABLE                     0


#define OPTM_MASTER_GPID                 OPTM_GFX_GP_0
#define OPTM_SLAVER_GPID                 OPTM_GFX_GP_1
#define OPTM_SLAVER_LAYERID              HIFB_LAYER_SD_0
#define OPTM_MAX_LOGIC_HIFB_LAYER        ((HI_U32)HIFB_LAYER_ID_BUTT)

#define OPTM_CMAP_SIZE                   0x400

#define OPTM_CheckGPMask_BYLayerID(u32LayerID)  \
            do{\
                if (g_stGfxGPDevice[g_stGfxDevice[u32LayerID].enGPId].bMaskFlag)\
                {\
                    return HI_SUCCESS;\
                }\
            }while(0)

#define OPTM_CheckGPMask_BYGPID(enGPId) \
            do{\
                if (g_stGfxGPDevice[enGPId].bMaskFlag)\
                {\
                    return HI_SUCCESS;\
                }\
            }while(0)


#define IS_MASTER_GP(enGpId) ((g_enOptmGfxWorkMode == HIFB_GFX_MODE_HD_WBC)&&(enGpId == OPTM_GFX_GP_0))
#define IS_SLAVER_GP(enGpId) ((g_enOptmGfxWorkMode == HIFB_GFX_MODE_HD_WBC)&&(enGpId == OPTM_GFX_GP_1))


#ifndef CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT
#define CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT  17
#endif

/*************************** Structure Definition ****************************/
typedef struct tagOPTM_GFX_CSC_PARA_S
{
    HI_U32               u32Bright;
    HI_U32               u32Contrast;
    HI_U32               u32Saturation;
    HI_U32               u32Hue;
    HI_U32               u32Kr;
    HI_U32               u32Kg;
    HI_U32               u32Kb;
}OPTM_GFX_CSC_PARA_S;

typedef enum tagOPTM_GFX_GP_E
{
    OPTM_GFX_GP_0 = 0x0,  /** process gfx0,gfx1,gfx2,gfx3   */
    OPTM_GFX_GP_1,        /** process gfx4,gfx5             */
    OPTM_GFX_GP_BUTT
}OPTM_GFX_GP_E;

typedef struct tagOPTM_GFX_OFFSET_S
{
    HI_U32 u32Left;    /** left offset   */
    HI_U32 u32Top;     /** top offset    */
    HI_U32 u32Right;   /** right offset  */
    HI_U32 u32Bottom;  /** bottom offset */
}OPTM_GFX_OFFSET_S;



/** csc state*/
typedef enum tagOPTM_CSC_STATE_E
{
    OPTM_CSC_SET_PARA_ENABLE = 0x0,
    OPTM_CSC_SET_PARA_RGB         ,
    OPTM_CSC_SET_PARA_BGR         ,
    OPTM_CSC_SET_PARA_CLUT        ,
    OPTM_CSC_SET_PARA_CbYCrY      ,
    OPTM_CSC_SET_PARA_YCbYCr      ,
    OPTM_CSC_SET_PARA_BUTT
} OPTM_CSC_STATE_E;

typedef enum optm_COLOR_SPACE_E
{
    OPTM_CS_UNKNOWN = 0,

    OPTM_CS_BT601_YUV_LIMITED, /** BT.601 **/
    OPTM_CS_BT601_YUV_FULL,
    OPTM_CS_BT601_RGB_LIMITED,
    OPTM_CS_BT601_RGB_FULL,

    OPTM_CS_BT709_YUV_LIMITED, /** BT.709 **/
    OPTM_CS_BT709_YUV_FULL,
    OPTM_CS_BT709_RGB_LIMITED,
    OPTM_CS_BT709_RGB_FULL,

    OPTM_CS_BUTT
} OPTM_COLOR_SPACE_E;



/**
 ** wbc work state
 ** ��д״̬
 **/
#ifndef HI_BUILD_IN_BOOT
typedef struct tagOPTM_GFX_WORK_S
{
    HI_U32                   u32Data;
    struct work_struct       work;
}OPTM_GFX_WORK_S;
#endif

/**
 ** what that want to updata
 ** ��������״̬
 **/
typedef union tagOPTM_GFX_UP_FLAG_U
{
    struct
    {
        unsigned int    IsNeedRegUp     : 1 ; /*  [0] */
        unsigned int    IsEnable        : 1 ; /*  [1] */
        unsigned int    IsNeedUpInRect  : 1 ; /*  [2] */
        unsigned int    OutRect         : 1 ; /*  [3] */
        unsigned int    IsNeedUpAlpha   : 1 ; /*  [4] */
        unsigned int    IsNeedUpPreMult : 1 ; /*  [5] */
        unsigned int    WbcMode         : 1 ; /*  [6] */
        unsigned int    Reserved        : 25; /*  [31...7] */
    } bits;
    unsigned int    u32;
}OPTM_GFX_UP_FLAG_U;


/**
 ** display ID
 ** ��ʾͨ��ID������ͨ���ͱ���ͨ��
 **/
typedef enum tagOPTM_DISPCHANNEL_E
{
    OPTM_DISPCHANNEL_0   = 0,    /** gfx4,gfx5             **/
    OPTM_DISPCHANNEL_1      ,    /** gfx0,gfx1,gfx2,gfx3   **/
    OPTM_DISPCHANNEL_BUTT
}OPTM_DISPCHANNEL_E;


typedef struct tagHIFB_ADP_LOWPOWER_INFO_S
{
    HI_BOOL LowPowerEn;
    HI_U32 LpInfo[CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT];
}HIFB_ADP_LOWPOWER_INFO_S;


/**
 **GP Para
 **GP��ز�����Ϣ�ṹ��
 **/
typedef struct tagOPTM_GFX_GP_S
{
    HI_BOOL bOpen;
    HI_BOOL bHdr;
    HI_BOOL bMaskFlag;
    HI_BOOL bBGRState;
    HI_BOOL bInterface;
    HI_BOOL bGpClose;
    HI_BOOL bResume;
    HI_BOOL bRecoveryInNextVT;
    HI_BOOL bNeedExtractLine;
    HI_BOOL bDispInitial;
    HI_RECT_S stInRect;
    HI_RECT_S stOutRect;
    HI_U32    u32ZmeDeflicker;
#ifdef CHIP_TYPE_hi3798cv200/** hdmi cts bug **/
    HI_DRV_DISP_FMT_E         enDispFmt;
#endif
    HIFB_STEREO_MODE_E        enTriDimMode;
    OPTM_GFX_CSC_PARA_S       stCscPara;
#ifndef CONFIG_GFX_PQ
    OPTM_COLOR_SPACE_E        enInputCsc;
    OPTM_COLOR_SPACE_E        enOutputCsc;
#else
    HI_PQ_GFX_COLOR_SPACE_E   enInputCsc;
    HI_PQ_GFX_COLOR_SPACE_E   enOutputCsc;
#endif
    OPTM_VDP_LAYER_GP_E       enGpHalId;
    OPTM_DISPCHANNEL_E        enDispCh;
    OPTM_GFX_UP_FLAG_U        unUpFlag;

#ifndef HI_BUILD_IN_BOOT
    struct workqueue_struct   *queue;
    OPTM_GFX_WORK_S           stOpenSlvWork;
    OPTM_GFX_WORK_S           st3DModeChgWork;
#endif
    HI_U32 u32Prior;
    HI_U32 u32Alpha;
    OPTM_VDP_DATA_RMODE_E     enReadMode;
    OPTM_VDP_BKG_S            stBkg;
    OPTM_VDP_CBM_MIX_E        enMixg;
}OPTM_GFX_GP_S;


typedef struct tagOPTM_GFX_LAYER_S{
    HI_BOOL              bOpened;
    HI_BOOL              bMaskFlag;
    HI_BOOL              bSharpEnable;
    HI_BOOL              bExtractLine;
    HI_BOOL              bEnable;
    HI_S32               s32Depth;
    HI_BOOL              bPreMute;
    HI_U32               u32ZOrder;
    HI_BOOL              bCmpOpened;
    HI_BOOL              bDmpOpened;
    HI_BOOL              bBufferChged;
    HI_U32               s32BufferChgCount;
    HI_U32               NoCmpBufAddr;
    HI_U32               ARHeadDdr;
    HI_U32               ARDataDdr;
    HI_U32               GBHeadDdr;
    HI_U32               GBDataDdr;
    HI_U32               u32TriDimAddr;
    HI_U16               Stride;           /** no compression mode stride  */
    HI_U16               CmpStride;        /** compression mode stride     */
    HIFB_COLOR_FMT_E     enDataFmt;
    HIFB_RECT            stInRect;         /** Inres of gfx                */
    HIFB_ALPHA_S         stAlpha;
    HIFB_COLORKEYEX_S    stColorkey;
    HIFB_STEREO_MODE_E   enTriDimMode;
    OPTM_VDP_BKG_S       stBkg;
    OPTM_VDP_GFX_BITEXTEND_E enBitExtend;  /** the g_ctrl of bitext        **/
    OPTM_VDP_DATA_RMODE_E    enReadMode;
    OPTM_VDP_DATA_RMODE_E    enUpDateMode;
    OPTM_VDP_LAYER_GFX_E     enGfxHalId;   /** the gfx's hal id            */
    OPTM_GFX_GP_E            enGPId;       /** which gp the gfx belong to  */
    OPTM_CSC_STATE_E         CscState;
    volatile HI_U32          vblflag;
    wait_queue_head_t        vblEvent;
    GFX_MMZ_BUFFER_S         stCluptTable;
}OPTM_GFX_LAYER_S;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            initial operation
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct
{
    /*gfx init*/
    /*CNcomment:ͼ�γ�ʼ��*/
    HI_S32 (*OPTM_GfxInit)(HI_VOID);

    /*gfx deinit*/
    /*CNcomment:ͼ��ȥ��ʼ��*/
    HI_S32 (*OPTM_GfxDeInit)(HI_VOID);

    /*open layer*/
    /*CNcomment:ͼ���*/
    HI_S32 (*OPTM_GfxOpenLayer)(HIFB_LAYER_ID_E enLayerId);

    /*close layer*/
    /*CNcomment:ͼ��ر�*/
    HI_S32 (*OPTM_GfxCloseLayer)(HIFB_LAYER_ID_E enLayerId);

    /*enable/disable layer*/
    /*CNcomment:ͼ��ʹ�ܻ��߷�ʹ��*/
    HI_S32 (*OPTM_GfxSetEnable)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable);

    /*mask layer*/
    /*CNcomment:�Ƿ�����ͼ��*/
    HI_S32 (*OPTM_GfxMaskLayer)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bMask);

    /*set layer address*/
    /*CNcomment:����ͼ����ʾ��ַ*/
    HI_S32 (*OPTM_GfxSetLayerAddr)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Addr);

    /*get layer address*/
    /*CNcomment:��ȡͼ����ʾ��ַ*/
    HI_S32 (*OPTM_GfxGetLayerAddr)(HIFB_LAYER_ID_E enLayerId, HI_U32 *pu32Addr);

    /*set layer stride*/
    /*CNcomment:����ͼ���о�*/
    HI_S32 (*OPTM_GfxSetLayerStride)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Stride);

    /*set layer data format*/
    /*CNcomment:����ͼ�����ظ�ʽ*/
    HI_S32 (*OPTM_GfxSetLayerDataFmt)(HIFB_LAYER_ID_E enLayerId, HIFB_COLOR_FMT_E enDataFmt);

    HI_S32 (*OPTM_GfxSetColorReg)(HIFB_LAYER_ID_E u32LayerId, HI_U32 u32OffSet, HI_U32 u32Color, HI_S32 UpFlag);

    /*wait for vertical blank*/
    /*CNcomment:�ȴ���ֱ����*/
    HI_S32 (*OPTM_GfxWaitVBlank)(HIFB_LAYER_ID_E u32LayerId);

    /*set layer deflick level*/
    /*CNcomment:���ÿ�������*/
    HI_S32 (*OPTM_GfxSetLayerDeFlicker)(HIFB_LAYER_ID_E enLayerId, HIFB_DEFLICKER_S *pstDeFlicker);

    /*set layer alpha*/
    /*CNcomment:����ͼ��alpha ֵ*/
    HI_S32 (*OPTM_GfxSetLayerAlpha)(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha);

    /*set layer rect*/
    /*CNcomment:����ͼ���������*/
    HI_S32 (*OPTM_GfxSetLayerRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect);

    /*set graphics process device Inrect and Outrect*/
    /*CNcomment:����ͼ������������������*/
    HI_S32 (*OPTM_GfxSetGpRect)(OPTM_GFX_GP_E enGpId, const HIFB_RECT *pstInputRect);

    /*get graphics process device Inrect and Outrect*/
    /*CNcomment:��ȡͼ������������������*/
    HI_S32 (*OPTM_GfxGetGpRect)(OPTM_GFX_GP_E enGpId, HIFB_RECT *pstInputRect);

    HI_S32 (*OPTM_GfxSetGpInPutSize)(OPTM_GFX_GP_E enGpId, HI_U32 u32Width, HI_U32 u32Height);

    /*set layer key mask*/
    /*CNcomment:����ͼ��colorkey mask*/
    HI_S32 (*OPTM_GfxSetLayKeyMask)(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S *pstColorkey);

    /*set layer pre mult*/
    /*CNcomment:����ͼ��Ԥ��*/
    HI_S32 (*OPTM_GfxSetLayerPreMult)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable);

    HI_VOID (*OPTM_GfxGetClosePreMultState)(OPTM_GFX_GP_E enGpId, HI_BOOL *pbShouldClosePreMult);

#ifndef HI_BUILD_IN_BOOT
    HI_VOID (*OPTM_GfxWhetherDiscardFrame)(HIFB_LAYER_ID_E enLayerId, HI_BOOL *pNoDiscardFrame);
#endif

    /*set clut address*/
    /*CNcomment:����CLUT ��ַ*/
    HI_S32 (*OPTM_GfxSetClutAddr)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32PhyAddr);

    HI_VOID (*OPTM_GfxGetOSDData)  (HIFB_LAYER_ID_E enLayerId, HIFB_OSD_DATA_S  *pstLayerData);
#ifndef HI_BUILD_IN_BOOT
    HI_VOID (*HIFB_ADP_GetLogoData)(HIFB_LAYER_ID_E enLayerId, HIFB_LOGO_DATA_S *pstLogoData);
#endif
    /*set call back*/
    /*CNcomment:ע��ص�����*/
#ifndef HI_BUILD_IN_BOOT
    HI_S32 (*OPTM_GfxSetCallback)(HIFB_LAYER_ID_E enLayerId, IntCallBack pCallBack, HIFB_CALLBACK_TPYE_E eIntType);
#endif
    /*update layer register*/
    /*CNcomment:���¼Ĵ���*/
    HI_S32 (*OPTM_GfxUpLayerReg)(HIFB_LAYER_ID_E enLayerId);

    HI_VOID (*HIFB_ADP_SetInOutScanLine)(HIFB_LAYER_ID_E LayerId, HI_BOOL bInScanLine);

#ifdef CFG_HIFB_STEREO3D_HW_SUPPORT
    /*set stereo mode*/
    /*CNcomment:����3D ģʽSBS/TB/MVC*/
    HI_S32 (*OPTM_GfxSetTriDimMode)(HIFB_LAYER_ID_E enLayerId, HIFB_STEREO_MODE_E enMode, HIFB_STEREO_MODE_E enWbcSteroMode);

    /*set stereo address*/
    /*CNcomment:����3D ģʽ�����۵�ַ*/
    HI_S32 (*OPTM_GfxSetTriDimAddr)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32TriDimAddr);
#endif

    /*set the priority of layer in gp*/
    /*CNcomment:����ͼ����GP �е����ȼ�*/
    HI_S32 (*OPTM_GfxSetLayerPriority)(HIFB_LAYER_ID_E u32LayerId, HIFB_ZORDER_E enZOrder);

    /*get the priority of layer in gp*/
    /*CNcomment:��ȡͼ����GP �е����ȼ�*/
    HI_S32 (*OPTM_GfxGetLayerPriority)(HIFB_LAYER_ID_E u32LayerId, HI_U32 *pU32Priority);

    /*��ȡͼ���������*/
    HI_VOID (*OPTM_GFX_GetDevCap)(const HIFB_CAPABILITY_S **pstCap);
    /*��ȡͼ�����������С*/
    HI_S32 (*OPTM_GfxGetOutRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT * pstOutputRect);
    /*��ȡͼ������������С*/
    HI_S32 (*OPTM_GfxGetLayerRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstRect);
    /*set gp mask flag*/
    HI_S32 (*OPTM_GFX_SetGPMask)(OPTM_GFX_GP_E enGpId, HI_BOOL bFlag);
    /*get gfx mask flag*/
    HI_S32 (*OPTM_GFX_GetGfxMask)(OPTM_GFX_GP_E enGpId);
    HI_S32 (*OPTM_GfxGetDispFMTSize)(OPTM_GFX_GP_E enGpId, HIFB_RECT *pstOutRect);

#ifndef HI_BUILD_IN_BOOT
    HI_S32  (*OPTM_GFX_ClearLogoOsd)         (HIFB_LAYER_ID_E enLayerId);
    HI_S32  (*OPTM_GFX_SetStereoDepth)       (HIFB_LAYER_ID_E enLayerId, HI_S32 s32Depth);
    HI_VOID (*HIFB_ADP_SetDeCmpDdrInfo)      (HIFB_LAYER_ID_E enLayerId, HI_U32 ARHeadDdr,HI_U32 ARDataDdr,HI_U32 GBHeadDdr,HI_U32 GBDataDdr, HI_U32 DeCmpStride);
    HI_VOID (*HIFB_ADP_GetDeCmpStatus)       (HIFB_LAYER_ID_E enLayerId, DRV_HIFB_DECMPSTATUS_S *pstDeCmpStatus, HI_BOOL bDebug);
    HI_VOID (*HIFB_ADP_SetVdpIntMask)        (HI_U32 IntMask);
    HI_VOID (*HIFB_ADP_GetVdpIntMask)        (HI_U32 *pIntMask);
    HI_VOID (*HIFB_ADP_GetResumeStatus)      (HIFB_LAYER_ID_E enLayerId, HI_BOOL *pbResume);
    HI_VOID (*OPTM_GFX_DECMP_Open)           (HIFB_LAYER_ID_E enLayerId);
    HI_VOID (*OPTM_GFX_DECMP_Close)          (HIFB_LAYER_ID_E enLayerId);
    HI_S32  (*OPTM_GFX_CMP_Open)             (HIFB_LAYER_ID_E enLayerId);
    HI_S32  (*OPTM_GFX_CMP_Close)            (HIFB_LAYER_ID_E enLayerId);
    HI_S32  (*OPTM_GFX_CMP_GetSwitch)        (HIFB_LAYER_ID_E enLayerId);
    HI_S32  (*OPTM_GFX_SetCmpRect)           (HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstRect);
    HI_S32  (*OPTM_GFX_SetCmpMode)           (HIFB_LAYER_ID_E enLayerId, HIFB_CMP_MODE_E enCMPMode);
    HIFB_CMP_MODE_E (*OPTM_GFX_GetCmpMode)   (HIFB_LAYER_ID_E enLayerId);
    HI_S32  (*OPTM_GFX_SetCmpDDROpen)        (HIFB_LAYER_ID_E enLayerId, HI_BOOL bOpen);
    HI_S32  (*OPTM_GFX_GetSlvLayerInfo)      (HIFB_SLVLAYER_DATA_S *pstLayerInfo);
    HI_VOID (*HIFB_ADP_SetLowPowerInfo)      (HIFB_LAYER_ID_E enLayerId,HIFB_ADP_LOWPOWER_INFO_S *pstLowPowerInfo);
#endif

    HI_S32  (*OPTM_GFX_SetTCFlag)         (HI_BOOL bFlag);
    HI_S32  (*OPTM_GfxSetGpDeflicker)     (OPTM_GFX_GP_E enGpId, HI_BOOL bDeflicker);
    HI_S32  (*OPTM_GFX_GetHaltDispStatus) (HIFB_LAYER_ID_E enLayerId, HI_BOOL *pbDispInit);
    HI_BOOL (*HIFB_ADP_GetIsGfxWorkAtWbc) (HI_VOID);
}OPTM_GFX_OPS_S;



/***************************  The enum of Jpeg image format  ******************/

/********************** Global Variable declaration **************************/


/******************************* API declaration *****************************/



/***************************************************************************************
* func          : OPTM_GFX_GetOps
* description   : get hifb options, then can call the functions \n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_VOID OPTM_GFX_GetOps(OPTM_GFX_OPS_S *ops);

/***************************************************************************************
* func          : OPTM_UnmapAndRelease
* description   : unmap the ddr and release it\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_VOID OPTM_UnmapAndRelease(GFX_MMZ_BUFFER_S *psMBuf);

/***************************************************************************************
* func          : OPTM_AllocAndMap
* description   : CNcomment: alloc and map mem CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, GFX_MMZ_BUFFER_S *psMBuf);


/***************************************************************************************
* func          : OPTM_GfxInitLayer
* description   : CNcomment: ��ʼ��ͼ����Ϣ CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_VOID OPTM_GfxInitLayer(HIFB_LAYER_ID_E enLayerId);

/***************************************************************************************
* func          : OPTM_GPOpen
* description   : CNcomment: ��GP�豸������ZMEϵ�� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GPOpen(OPTM_GFX_GP_E enGPId);

/***************************************************************************************
* func          : OPTM_GFX_ReleaseClutBuf
* description   : CNcomment: �ͷŵ�ɫ���ڴ� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GFX_ReleaseClutBuf(HIFB_LAYER_ID_E enLayerId);

/***************************************************************************************
* func          : OPTM_GfxSetLayerReadMode
* description   : CNcomment: ���ö�ȡ����ģʽ CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GfxSetLayerReadMode(HIFB_LAYER_ID_E enLayerId, OPTM_VDP_DATA_RMODE_E enReadMode);

/***************************************************************************************
* func          : OPTM_PixerFmtTransferToHalFmt
* description   : CNcomment: ������������ת�������ظ�ʽ CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
OPTM_VDP_GFX_IFMT_E OPTM_PixerFmtTransferToHalFmt(HIFB_COLOR_FMT_E enDataFmt);


HI_VOID DRV_HIFB_ADP_UpDataCallBack(HI_VOID* u32Param0, HI_VOID* u32Param1);

/***************************************************************************************
* func            : OPTM_GfxSetLayerAddr
* description    : CNcomment: ������ʾ��ַ CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 OPTM_GfxSetLayerAddr(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Addr);

/***************************************************************************************
* func          : OPTM_GfxSetLayerStride
* description   : CNcomment: ����ͼ��stride CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GfxSetLayerStride(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Stride);

/***************************************************************************************
* func          : OPTM_Adapt_AllocAndMap
* description   : CNcomment: �����ڴ沢ӳ���ַ CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_Adapt_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, GFX_MMZ_BUFFER_S *psMBuf);

/***************************************************************************************
* func          : OPTM_SetCallbackToDisp
* description   : CNcomment: ��Displayע���ж� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
HI_S32 OPTM_SetCallbackToDisp(OPTM_GFX_GP_E enGPId, IntCallBack pCallBack, HI_DRV_DISP_CALLBACK_TYPE_E eType, HI_BOOL bFlag);
#endif

/***************************************************************************************
* func          : OPTM_GfxSetEnable
* description   : CNcomment: ����ͼ��ʹ�� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GfxSetEnable(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable);

/***************************************************************************************
* func          : OPTM_GfxSetLayerRect
* description   : CNcomment: ����ͼ��ֱ��� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GfxSetLayerRect(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect);

/***************************************************************************************
* func          : OPTM_GfxSetGpRect
* description   : CNcomment: ����ͼ���Ӧ��GP�ֱ��� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GfxSetGpRect(OPTM_GFX_GP_E enGpId, const HIFB_RECT * pstInputRect);

/***************************************************************************************
* func          : OPTM_GfxCloseLayer
* description   : CNcomment: �ر�ͼ�� CNend\n
* param[in]     : HI_VOID
* retval        : NA
* others:       : NA
***************************************************************************************/
HI_S32 OPTM_GfxCloseLayer(HIFB_LAYER_ID_E enLayerId);


#ifdef HI_BUILD_IN_BOOT
HI_S32  OPTM_GfxInit(HI_VOID);
HI_S32  OPTM_GfxOpenLayer(HIFB_LAYER_ID_E enLayerId);
HI_S32  OPTM_GfxSetLayerAlpha(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha);
HI_S32  OPTM_GfxSetLayerDataFmt(HIFB_LAYER_ID_E enLayerId, HIFB_COLOR_FMT_E enDataFmt);
HI_S32  OPTM_GfxSetLayKeyMask(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S *pstColorkey);
HI_S32  OPTM_GfxUpLayerReg(HIFB_LAYER_ID_E enLayerId);
HI_S32  OPTM_GpInitFromDisp(OPTM_GFX_GP_E   enGPId, HI_BOOL Support4KLogo);
HI_S32  OPTM_GFX_SetTCFlag(HI_BOOL bFlag);
HI_BOOL HIFB_ADP_GetIsGfxWorkAtWbc(HI_VOID);
#endif

HI_S32  OPTM_GfxSetDispFMTSize(OPTM_GFX_GP_E   enGpId, const HI_RECT_S *pstOutRect);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
