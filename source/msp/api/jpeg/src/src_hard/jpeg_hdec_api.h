/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : jpeg_hdec_mem.h
Version           : Initial Draft
Author            :
Created          : 2017/07/05
Description     :
Function List   :


History           :
Date                      Author           Modification
2017/07/05            sdk              Created file
******************************************************************************/
#ifndef __JPEG_HDEC_API_H__
#define __JPEG_HDEC_API_H__


/*********************************add include here********************************/
#include  "jpeglib.h"

#include  "hi_jpeg_config.h"
#include  "hi_jpeg_api.h"

#include  "hi_drv_jpeg.h"
#include  "jpeg_hdec_rwreg.h"

#ifdef CONFIG_JPEG_EXIF_SUPPORT
#include "jpeg_sdec_exif.h"
#endif

#include "jpeg_sdec_color.h"


#ifdef CONFIG_JPEG_SAVE_SCEN
#include "hi_jpeg_reg.h"
#endif

/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
   extern "C" {
#endif
#endif /* __cplusplus */


    /***************************** Macro Definition ******************************/
     #define CONFIG_JPEG_SCALE_MULTIPLE_8                           (3)
     #define CONFIG_JPED_INPUT_DATA_BUFFER_NUM                      (2)

     #define CONFIG_JPED_INPUT_DATA_BUFFER_ALIGN                    (128)
     #define CONFIG_JPED_INPUT_DATA_ONE_BUFFER_RESERVE_SIZE         (128)
     #define CONFIG_JPED_INPUT_DATA_ONE_BUFFER_SIZE                 (256 * 1024)

     #define JPEG_API_MODULE_DEFINE_HANDLE()\
              JPEG_HDEC_HANDLE_S_PTR pJpegHandle = NULL;\
              pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);\
              if (NULL == pJpegHandle)\
              {\
                 ERREXIT(cinfo, JERR_NULL_POINT);\
              }
     /*************************** Enum Definition ****************************/


    /*************************** Structure Definition ****************************/
     typedef struct tatINPUT_STREAM_BUF_S
     {
         HI_BOOL UseSecondInputBuf;           /**<-- use second input buffer     >**//**<-- �Ƿ�ʹ�õڶ�������buffer >**/
         HI_BOOL SupportUserPhyInputDataBuf;  /**<-- whether user input data buffer is physical      >**//**<-- ʹ���û�����������ڴ�buffer ���ݴ���>**/
         HI_BOOL SupportUserInputBufReturn;   /**<-- check whether stream save with two buffer  >**//**<-- ֧���ڴ���Ʒ�ʽ�Ĵ��� >**/
         HI_BOOL UserInputDataEof;            /**<-- check the input data whether has been read empty, file or vir mem used  >**//**<-- �Ƿ��Ѿ����������������>**/
         HI_CHAR *pUserSecondInputDataVirBuf; /**<-- this is second input data virtual buffer  >**//**<-- �ڴ���Ʒ�ʽ�ڶ������������ڴ�>**/
         HI_CHAR *pUserFirstInputDataVirBuf;  /**<-- this is first input data virtual buffer or input data start virtual buffer   >**//**<-- ��һ�����������ڴ�>**/
         HI_U32 UserInputDataPhyBuf;          /**<-- input one or pool physical buffer start address  >**//**<-- �û�������������������ַ >**/
         HI_ULONG UserTotalInputBufSize;      /**<-- input one or pool buffer size   >**//**<-- �û�������������ݴ�С >**/
         HI_ULONG UserFirstInputBufSize;      /**<-- first input data buffer size       >**//**<-- �û�����ĵ�һ���������ݴ�С>**/
         HI_ULONG UserSecondInputBufSize;     /**<-- second input data buffer size  >**//**<-- �û�����ĵڶ����������ݴ�С >**/
         HI_ULONG UserInputCurPos;            /**<-- the input data current position, file or vir mem used  >**//**<-- ����Ӳ������֮ǰ������ȡ��λ��>**/
         HI_ULONG UserDataHasBeenReadSize;    /**<-- the input data has been read size, memory return used  >**//**<-- �����Ѿ�����ȡ�Ĵ�С�������ڴ���Ʒ�ʽʹ��>**/
     }INPUT_STREAM_BUF_S;

     typedef struct tagINPUT_DATA_BUF_S
     {
         HI_BOOL DecDataEof;          /**<-- check whether decode buffer is empty                  >**//**<-- �ж��Ƿ��Ѿ����뵽���һ������ >**/
         HI_BOOL SaveStreamBufBusy;   /**<-- check whether current save stream buffer is busy  >**//**<-- ����洢����buffer �Ƿ��Ѿ�������������>**/
         HI_CHAR *pSaveStreamVirBuf;  /**<-- input data virtual buffer that be use to decode        >**//**<-- �洢����buffer  �����ַ >**/
         HI_U32 SaveStreamPhyBuf;     /**<-- input data physical buffer that be use to decode     >**//**<-- �洢����buffer ���������ַ >**/
     }INPUT_DEC_DATA_BUF_S;

     typedef struct tagJPEG_INPUT_DATA_BUF_INFO_S
     {
         HI_BOOL bUseInsideData;               /**<-- dispose stream with inside or callback  >**//**<-- ��ʹ�ûص���ʽ�����ڴ����ݴ���ʽ>**/
         HI_BOOL EnReadToInputDataBuf;         /**<-- enable read input data to save stream buffer with callback used  >**//**<-- �Ƿ���Ҫ�����洢����buffer ��>**/
         HI_BOOL BeginReadToInputDataBuf;      /**<-- begin read input data to save stream buffer  >**//**<-- ��ʼ�����洢����buffer  �� >**/
         HI_U32 SaveStreamBufSize;             /**<-- input data buffer size that be use to decode  >**//**<-- �洢����buffer  ��С >**/
         HI_U32 DecDataBufIndex;               /**<-- decode use which buffer    >**//**<-- ���뵽�ڼ�������buffer  >**/
         HI_U32 SaveStreamBufIndex;            /**<-- save data to which buffer  >**//**<-- �Ѿ���д���ڼ���洢����buffer >**/
         INPUT_STREAM_BUF_S stInputStreamBuf;  /**<-- input data buffer infomation >**//**<-- ��������buffer ��Ϣ >**/
         INPUT_DEC_DATA_BUF_S stSaveStreamBuf[CONFIG_JPED_INPUT_DATA_BUFFER_NUM]; /**<-- hard decode data buffer >**//**<-- ����ʹ�õĴ洢����buffer ��Ϣ >**/
     }JPEG_INPUT_DATA_BUF_INFO_S;

    /** the jpeg sof information��the hard decode is only use dec para */
    /** CNcomment:jpeg��С�����Ϣ��Ӳ������ֻ�н���ֱ��ʵĸ��� */
    typedef struct tagJPEG_HDEC_SOFINFO_S
    {
         HI_BOOL bCalcSize;        /**< the jpeg size has  calculated >**//**<CNcomment:jpeg��С�Ѿ��������ˣ�����Ҫ���¼�����  >**/
         HI_U32  u32YWidth;        /**< the lu width                 >**//**<CNcomment:���ȿ���        >**/
         HI_U32  u32YHeight;       /**< the lu height                >**//**<CNcomment:���ȸ߶�              >**/
         HI_U32  u32YMcuHeight;    /**< the lu height,mcu align >**//**<CNcomment:���ȸ߶ȣ�MCU����>**/
         HI_U32  u32YSize;         /**< the lu size                   >**//**<CNcomment:���ȴ�С          >**/
         HI_U32  u32CWidth;        /**< the ch width                >**//**<CNcomment:ɫ�ȿ��          >**/
         HI_U32  u32CHeight;       /**< the ch height               >**//**<CNcomment:ɫ�ȸ߶�          */
         HI_U32  u32CMcuHeight;    /**< the ch height,mcu align >**//**<CNcomment:ɫ�ȸ߶ȣ�MCU���� >**/
         HI_U32  u32CSize;         /**< the ch size                   >**//**<CNcomment:ɫ�ȴ�С          >**/
         HI_U32  u32YStride;       /**< the lu stride                 >**//**<CNcomment:�����м��     >**/
         HI_U32  u32CbCrStride;    /**< the ch stride                >**//**<CNcomment:ɫ���м��     >**/
         HI_U32  DecOutWidth;      /**< the dec width               >**//**<CNcomment:������          >**/
         HI_U32  DecOutHeight;     /**< the dec height              >**//**<CNcomment:����߶�          >**/
         HI_U32  DecOutStride;     /**< the display stride          >**//**<CNcomment:��ʾ�м��    >**/
         HI_U32  u32InWandH;       /**< set to register size       >**//**<CNcomment:���Ӳ���ķֱ���  >**/
         #ifdef CONFIG_JPEG_HARDDEC2ARGB
         HI_U32  u32McuWidth;      /**< the mcu width        >**/
         HI_U32  u32MINSize;       /**< the min size           >**/
         HI_U32  u32MIN1Size;      /**< the min1 size         >**/
         HI_U32  u32RGBSizeReg;    /**< set to register size >**//**<CNcomment:���Ӳ���Ĵ�����ֵ  >**/
         #endif
    }JPEG_HDEC_SOFINFO_S;

    typedef struct tagJPEG_MIDDLE_SURFACE_S
    {
         HI_CHAR* pOutVir; /**<  csc output virtual buffer  >**//**<CNcomment:��ɫ�ռ�ת����������ַ   */
         HI_CHAR* DecOutVir[MAX_PIXEL_COMPONENT_NUM];  /**< jpeg hard dec output vitual  buffer   >**//**<CNcomment:jpeg Ӳ��������������ַ  */
         HI_U32 DecOutPhy[MAX_PIXEL_COMPONENT_NUM];    /**< jpeg hard dec output physics buffer >**//**<CNcomment:jpeg Ӳ��������������ַ  */
         HI_U32   OutPhy; /**<  csc output physics buffer    >**//**<CNcomment:��ɫ�ռ�ת����������ַ   */
    }JPEG_MIDDLE_SURFACE_S;

    typedef struct tagJPEG_HDEC_HANDLE_S
    {
          volatile HI_CHAR *pJpegRegVirAddr;  /**<-- mmap the jpeg virtual address  >**//**<CNcomment:jpegӳ�������������ַ         >**/
          HI_BOOL bHasHufTable;
          HI_BOOL bDiscardScanlines;          /**<-- skip scan lines  >**//**<CNcomment: ���д���>**/
          HI_BOOL bSeekToSoftDec;             /**<-- seek to soft decode  >**//**<CNcomment: ���˵��������>**/
          HI_BOOL bOutUsrBuf;                 /**<-- whether out to usr buffer  >**//**<CNcomment: �Ƿ���Ҫ��������û�buffer ��>**/
          HI_BOOL SupportHardDec;             /**<-- whether support hard decode  >**//**<CNcomment: �Ƿ�֧��Ӳ������>**/
          HI_BOOL EnStopDecode;               /**<-- whether support stop decode in decoding  >**//**<CNcomment: �Ƿ�֧�ֽ����������ֹ����>**/
          HI_BOOL bHdecEnd;                   /**<-- hard decode success  >**//**<CNcomment:Ӳ������ɹ�>**/
          HI_BOOL bCSCEnd;                    /**<--color space convert success  >**//**<CNcomment:��ɫ�ռ�ת���ɹ� >**/
          HI_BOOL bOutYCbCrSP;                /**<--if output yuvsp  >**//**<CNcomment:�Ƿ�������yuv semi_planer    >**/
          HI_BOOL bDecOutColorSpaecXRGB;      /**<--the jpeg support dec to argb  >**//**<CNcomment:jpegӲ��֧�ֽ������ARGB        >**/
          HI_BOOL bCheckedSupportHardDec;     /**<-- has been check whether support hard decode  >**//**<CNcomment:�Ѿ��ж����Ƿ�֧��Ӳ������>**/
          HI_BOOL bCrop;                      /**<--  if crop  >**//**<CNcomment:�Ƿ�ü� CNend*/
          HI_BOOL bSetCropRect;               /**< -- if has been set crop rect >**//**<CNcomment: �Ƿ������˲ü�����>**/
          HI_U32  u32MinPhyBuf;               /**<-- dec to argb need row buffer  >**//**<CNcomment:�������ARGB��Ҫ����buffer      >**/
          HI_U32  SkipLines;                  /**<-- skip output lines >**//**<CNcomment:������Ե�����>**/
          #ifdef CONFIG_JPEG_SAVE_SCEN
          HI_S32  s32RegData[JPGD_REG_LENGTH];/**<--save scen>**//**<CNcomment:�����ֳ���Ϣ>**/
          FILE* pScenFile;
          #endif
          HI_S32  s32LuDcLen[6];              /**<-- huffman lu dc codelenght  >**//**<CNcomment:����DC���CodeLength >**/
          HI_BOOL  bOutYUV420SP;              /**<--all dec output yuv420sp     >**//**<CNcomment:ͳһ���yuv420sp��ʽ >**/
          #ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
          HI_U64   u64LuPixValue;             /**<--the lu pixle value  >**//**<CNcomment:����ֵ��>**/
          #endif
          HI_U32   HardDecTimes;              /**<--hard decode cost times  >**//**<CNcomment:Ӳ�����뻨�ѵ�ʱ��>**/
          HI_U32   DecCostTimes;              /**<--calc the decode time  >**//**<CNcomment:�������ʱ��>**/
          #ifdef CONFIG_JPEG_LOW_DELAY_SUPPORT
          HI_BOOL  bLowDelayEn;
          HI_U32   u32PhyLineBuf;
          HI_CHAR* pVirLineBuf;
          #endif
          HI_S32   s32QuantTab[DCTSIZE2];
          HI_U32   u32HuffDcTab[12];
          HI_U32   u32HufAcMinTab[DCTSIZE];
          HI_U32   u32HufAcBaseTab[DCTSIZE];
          HI_U32   u32HufAcSymbolTab[256];
          HI_S32   s32ClientData; /**<--the client pointer of private structure   >***//**<CNcomment:˽�нṹ��ָ��>***/
          HI_S32   JpegDev;       /**<--the jpeg device  >**//**<CNcomment:jpeg�豸 >**/
          HI_S32   CscDev;
          HI_S32   MemDev;
          HI_U32   ScalRation;    /**<--the dec scale   >**//**<CNcomment:���ű���  >**/
          HI_BOOL  bReleaseRes;   /**<--the resouce has release   >**//**<CNcomment:��Դ�Ѿ��ͷŵ���>**/
          #ifdef CONFIG_GFX_MEM_ION
          HI_VOID *pSaveStreamMemHandle;
          HI_VOID *pMiddleMemHandle;
          HI_VOID *pMinMemHandle;
          HI_VOID *pOutMemHandle;
          #endif
          HI_U32 ResByteConsum; /**<--consume byte after suspend  >**//**<CNcomment:�������ĵ�������   >**/
          HI_U8 u8Fac[MAX_PIXEL_COMPONENT_NUM][2];/**< save the sample factor   >**//**<CNcomment:����������Ӵ�С >**/
          HI_JPEG_RECT_S stCropRect; /**<-- output crop size >**//**<--����ü����� >**/
          JPEG_HDEC_SOFINFO_S  stJpegSofInfo;     /**< the jpeg sof information               >**//**<CNcomment:jpegͼƬ��С��Ϣ           >**/
          JPEG_INPUT_DATA_BUF_INFO_S  stInputDataBufInfo; /**< the hard decode data buffer message    >**/
          HI_JPEG_FMT_E enImageFmt; /**< jpeg picture type   >**//**<CNcomment:jpegͼƬ��ʽ>**/
          HI_JPEG_SURFACE_S  stOutSurface; /**< the user output message description    >**//**<CNcomment:�û����õ������Ϣ����     */
          JPEG_MIDDLE_SURFACE_S  stDecSurface;
          JPEG_SDEC_COLOR_CONVERT_S stSdecColorConvert;
          #ifdef CONFIG_JPEG_EXIF_SUPPORT
	      HI_BOOL  bExifSupport; /**<-- exif para define begine,  support exif mark >**//**<-- ��ʼ����exif  �������Ƿ�֧��exif >**/
	      HI_BOOL  bIsHDR;
	      HI_U8    u8ImageCount;
	      HI_CHAR* pThumbnailAddr;
	      HI_U32   u32ThumbnailSize;
          JPEG_EXIF_INFO_S  stExifInfo;
          #endif
    }JPEG_HDEC_HANDLE_S,*JPEG_HDEC_HANDLE_S_PTR;

    /********************** Global Variable declaration *****************************/

    /******************************* API declaration *****************************/

    /************************************************************************/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __JPEG_HDEC_API_H__*/
