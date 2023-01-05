/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : hi_drv_jpeg.h
Version           : Initial Draft
Author            : sdk
Created          : 2017/07/05
Description     :
Function List   :


History          :
Date                   Author                Modification
2017/07/05          sdk                   Created file
******************************************************************************/

#ifndef __HI_DRV_JPEG_H__
#define __HI_DRV_JPEG_H__


/***************************** add include here*********************************/
#include "hi_type.h"


/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" {
#endif
#endif /* __cplusplus */


    /***************************** Macro Definition ********************************/
    #define VID_CMD_MAGIC     'j'



    #define CMD_JPG_GETDEVICE                  _IO(VID_CMD_MAGIC,   0)
    #define CMD_JPG_GETSUSPEND                 _IO(VID_CMD_MAGIC,   1)
    #define CMD_JPG_GETRESUME                  _IO(VID_CMD_MAGIC,   2)

    #define CMD_JPG_RESET                      _IO(VID_CMD_MAGIC,   3)
    #define CMD_JPG_CANCEL_RESET               _IO(VID_CMD_MAGIC,   4)

#ifdef CHIP_TYPE_X5HD_hi3716m
    #define CMD_JPG_GETINTSTATUS               _IOWR(VID_CMD_MAGIC, 5,  JPEG_GETINTTYPE_S*)
#else
    #define CMD_JPG_GETINTSTATUS               _IOWR(VID_CMD_MAGIC, 5,  JPEG_GETINTTYPE_S)
#endif
    #define CMD_JPG_GETRESUMEVALUE             _IOWR(VID_CMD_MAGIC, 6,  HI_JPG_SAVEINFO_S)

    #define CMD_JPG_READPROC                   _IOWR(VID_CMD_MAGIC, 7,  HI_JPEG_PROC_INFO_S)

    #define CMD_JPG_LOWDELAY_LINEINFO          _IOWR(VID_CMD_MAGIC, 8, HI_JPEG_OUTPUTINFO_S)
    #define CMD_JPG_COMPAT_LOWDELAY_LINEINFO   _IOWR(VID_CMD_MAGIC, 8, HI_JPEG_COMPAT_OUTPUTINFO_S)

    #define CMD_JPG_GETINFLEXION               _IOWR(VID_CMD_MAGIC, 9, HI_U32)

    #define MAX_COMPONENT_NUM                 3
    #define JPEG_INTTYPE_DELAY_TIME           10000 /** 10s **/

    /***************************** Structure Definition ****************************/
    typedef enum hiJPEG_INTTYPE_E{
        JPG_INTTYPE_NONE       = 0,
        JPG_INTTYPE_CONTINUE,
        JPG_INTTYPE_LOWDEALY,
        JPG_INTTYPE_FINISH,
        JPG_INTTYPE_ERROR,
        JPG_INTTYPE_STREAM_ERROR,
        JPG_INTTYPE_BUTT
    }JPEG_INTTYPE_E;

    typedef enum hiJPEG_FMT_E{
        JPG_FMT_YUV400         = 0,
        JPG_FMT_YUV420         = 1,
        JPG_FMT_YUV422_21      = 2,
        JPG_FMT_YUV422_12      = 3,
        JPG_FMT_YUV444         = 4,
        JPG_FMT_YCCK           = 5,
        JPG_FMT_CMYK           = 6,
        JPG_FMT_BUTT
    }JPEG_FMT_E;

    typedef struct tagJPEG_RECT_S
    {
        HI_U32 x, y;
        HI_U32 w, h;
    }JPEG_RECT_S;

    typedef struct hiJPEG_GETINTTYPE_S
    {
        HI_U32 TimeOut;
        JPEG_INTTYPE_E IntType;
    }JPEG_GETINTTYPE_S;

    typedef struct hiJPG_SAVEINFO_S{
        HI_U32  u32ResumeData0;
        HI_U32  u32ResumeData1;
        HI_U32  u32ResBitRemain;
        HI_U32  u32ResByteConsu;
        HI_U32  u32ResMcuy;
        HI_U32  u32Pdy;
        HI_U32  u32Pdcbcr;
        HI_U32  u32DriCnt;
    }HI_JPG_SAVEINFO_S;

    typedef struct hiJPEG_PROC_INFO_S{
         HI_BOOL bSeekToSoftDec;   /**<-- seek to soft decode   >**//**<-- ���˵��������>**/
         HI_BOOL bHardDec;         /**<-- whether hard decode    >**//**<-- �Ƿ�ΪӲ������ >**/
         HI_U32 CurOffset;         /**<-- current offset that will seek >**//**<-- Ҫ���˵ĵ�ǰλ��>**/
         HI_U32 SkipScanLines;     /**<-- skip scan lines >**//**<-- �����˶����� >**/
         HI_U32 HardDecTimes;      /**<-- hard decode cost times >**//**<-- Ӳ�����������ѵ�ʱ�� >**/
         HI_U32 DecCostTimes;      /**<-- decode cost times       >**//**<-- ���뵱ǰ֡�����ѵ�ʱ�� >**/
         HI_U32 DecInflexion;      /**<-- hard or soft decode inflexion  >**//**<-- ���Ӳ������Ĺյ�  >**/
         HI_U32 u32YWidth;         /**<-- the lu width       >**//**<-- ���ȿ��� >**/
         HI_U32 u32YHeight;        /**<-- the lu height      >**//**<-- ���ȸ߶�       >**/
         HI_U32 u32YSize;          /**<-- the lu size         >**//**<-- :���ȴ�С      >**/
         HI_U32 u32CWidth;         /**<-- the ch width      >**//**<-- ɫ�ȿ��       >**/
         HI_U32 u32CHeight;        /**<--  the ch height    >**//**<-- ɫ�ȸ߶�       >**/
         HI_U32 u32CSize;          /**<-- the ch size        >**//**<-- ɫ�ȴ�С        >**/
         HI_U32 u32YStride;        /**<-- the lu stride      >**//**<-- :�����м�� >**/
         HI_U32 u32CbCrStride;     /**<-- the ch stride     >**//**<-- ɫ���м��  > **/
         HI_U32 StartStreamBuf;    /**<-- the stream start address     >**//**<-- ����buffer��ʼ��ַ��Ҫ64�ֽڶ���      >**/
         HI_U32 EndStreamBuf;      /**<-- the stream end address       >**//**<-- ����buffer������ַ��Ҫ64�ֽڶ���      >**/
         HI_U32 StartSaveDataBuf;  /**<-- the save data start address  >**//**<-- �洢��������ʼ��ַ(��������ַ������)  >**/
         HI_U32 EndSaveDataBuf;    /**<-- the save data end address    >**//**<-- �洢�����Ľ�����ַ(��������ַ������)  >**/
         HI_U32 ImageWidth;        /**<-- the input widht             >**//**<-- ������           >**/
         HI_U32 ImageHeight;       /**<-- the input height            >**//**<-- ����߶�           >**/
         HI_U32 OutputWidth;       /**<-- decode output width    >**//**<-- ����������>**/
         HI_U32 OutputHeight;      /**<--decode output height    >**//**<-- ��������߶�>**/
         HI_U32 OutputStride;      /**<--decode output stride     >**//**<-- ����м��      >**/
         HI_U32 ImageFmt;          /**<-- the input format           >**//**<-- �������ظ�ʽ>**/
         HI_U32 OutputFmt;         /**<-- the output format        >**//**<-- ������ظ�ʽ>**/
         HI_U32 OutputLuPhyBuf;    /**<-- output lu physics address  >**//**<-- ������������ַ >**/
         HI_U32 OutputChPhyBuf;    /**<-- output ch physics address  >**//**<-- ɫ����������ַ >**/
         HI_U32 ScaleTimes;        /**<-- decode scale times            >**//**<-- �������ű��� >**/
         HI_U32 DecPthreadNums;    /**<-- decode pthread numbers   >**//**<-- ��ǰ�����߳���>**/
         HI_S32 DecLockTimes;      /**<-- decode has been lock times  >**//**<-- ��ǰ���뱻���Ĵ���>**/
         HI_S32 OpenDevConTimes;   /**<-- open dev continue times       >**//**<-- �豸�Ѿ��������򿪵Ĵ���>**/
         JPEG_RECT_S stCropRect;   /**<-- crop rect       >**//**<-- �ü�����>**/
    }HI_JPEG_PROC_INFO_S;


    typedef struct tagJPEG_INMSG_S{
         HI_U32     u32AllPhy;         /**<--  ��������buffer����ʼ�����ַ>**/
         HI_U32     u32SavePhy;        /**<--  ��һ��������������ַ         >**/
         HI_U64     u64SaveVir[2];     /**<--  �������������ַ���ڶ��������ַ����������buffer�������ַ��Ӧ >**/
         HI_U32     u32AllLen;         /**<--  ��������buffer�ĳ���            > **/
         HI_U32     u32SaveLen[2];     /**<--  ������������buffer�Ĵ�С>**/
         HI_U32     s32InWidth;        /**<--  ����ͼ���           >**/
         HI_U32     s32InHeight;       /**<--  ����ͼ���           >**/
         HI_U32     DecHandle;         /**<--  ������                       >**/
         HI_BOOL    bUserPhyMem;       /**<--  �Ƿ����Ϊ�����ַ  >**/
         JPEG_FMT_E  enInFmt;          /**<--  ����ͼ�����ظ�ʽ             >**/
    }HI_DRV_JPEG_INMSG_S;

    typedef struct tagJPEG_OUTMSG_S{
         HI_U32      u32OutPhy[MAX_COMPONENT_NUM];
         HI_U32      u32OutVir[MAX_COMPONENT_NUM];
         HI_U32      u32OutWidth[MAX_COMPONENT_NUM];
         HI_U32      u32OutHeight[MAX_COMPONENT_NUM];
         HI_U32      u32OutStride[MAX_COMPONENT_NUM];
         HI_U32      u32OutSize[MAX_COMPONENT_NUM];
         HI_S32      s32Scale;
         HI_U32      DecHandle;
         HI_U64      u64LuPixValue;
         HI_BOOL     bOutYuvSp420;
         HI_BOOL     bLuPixSum;
         JPEG_FMT_E  enOutFmt;
    }HI_DRV_JPEG_OUTMSG_S;

    typedef struct hiJPEG_DECINFO_S{
         HI_DRV_JPEG_INMSG_S  stInMsg;
         HI_DRV_JPEG_OUTMSG_S stOutMsg;
    }HI_JPEG_DECINFO_S;


    typedef struct hiJPEG_OUTPUTINFO_S{
         HI_CHAR* pVirLineBuf;
         HI_U32 u32OutHeight[MAX_COMPONENT_NUM];
    }HI_JPEG_OUTPUTINFO_S;

    typedef struct hiJPEG_COMPAT_OUTPUTINFO_S{
         HI_U32 u32VirLineBuf;
         HI_U32 u32OutHeight[MAX_COMPONENT_NUM];
    }HI_JPEG_COMPAT_OUTPUTINFO_S;

    /***************************** Global Variable declaration *********************/

    /***************************** API forward declarations ************************/

    /***************************** API realization *********************************/
    HI_S32 HI_DRV_JPEG_CreateDec(HI_U32 *pDecHandle);

    HI_S32 HI_DRV_JPEG_DecInfo(HI_U32 DecHandle,HI_DRV_JPEG_INMSG_S *stInMsg);

    HI_S32 HI_DRV_JPEG_DecOutInfo(HI_U32 DecHandle,HI_DRV_JPEG_OUTMSG_S *stOutMsg);

    HI_S32 HI_DRV_JPEG_DecFrame(HI_U32 DecHandle,HI_JPEG_DECINFO_S *pstDecInfo);

    HI_VOID HI_DRV_JPEG_DestoryDec(HI_U32 DecHandle);

    HI_VOID HI_DRV_JPEG_GetLuPixSum(HI_U32 DecHandle,HI_U64* pu64LuPixValue);

    /****************************************************************************/


#ifdef __cplusplus

#if __cplusplus

}
#endif
#endif /* __cplusplus */

#endif /* __HI_DRV_JPEG_H__*/
