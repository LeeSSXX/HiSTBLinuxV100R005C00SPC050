/******************************************************************************
*
* Copyright (C) 2017 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name        : hi_jpeg_api.h
Version           : Initial Draft
Author            :
Created          : 2017/07/05
Description     : The user will use this api to realize some function
Function List   :


History           :
Date                            Author                    Modification
2017/07/05                   sdk                       Created file
******************************************************************************/

#ifndef __HI_JPEG_API_H__
#define __HI_JPEG_API_H__


/*********************************add include here******************************/

#include "jpeglib.h"
#include "hi_type.h"

/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" {
#endif
#endif /* __cplusplus */

     /***************************** Macro Definition ******************************/
     /** \addtogroup      JPEG */
     /** @{ */    /** <!--[JPEG]*/

     /** Maximum number of data components */
     /** CNcomment:������ݷ����� CNend */
     #define MAX_PIXEL_COMPONENT_NUM            3

     /** @} */  /** <!-- ==== Macro Definition end ==== */

     /*************************** Enum Definition ****************************/
     /****************************************************************************/
     /*                               jpeg enum                                    */
     /****************************************************************************/

    /** \addtogroup      JPEG */
    /** @{ */    /** <!--[JPEG]*/

    /** enum of the input jpeg color space */
    /** CNcomment:�����jpegͼƬ��ʽ CNend */
    typedef enum hiHI_JPEG_FMT_E
    {
        JPEG_FMT_YUV400 = 0,
        JPEG_FMT_YUV420,
        JPEG_FMT_YUV422_21,
        JPEG_FMT_YUV422_12,
        JPEG_FMT_YUV444,
        JPEG_FMT_BUTT
    }HI_JPEG_FMT_E;

    /** @} */  /** <!-- ==== enum Definition end ==== */

    /*************************** Structure Definition ****************************/
    /****************************************************************************/
    /*                               jpeg api structure                            */
    /****************************************************************************/

    /** \addtogroup      JPEG */
    /** @{ */    /** <!--[JPEG]*/

    /** Structure of Rectangle */
    /** CNcomment:���� CNend */
    typedef struct hiJPEG_RECT_S
    {
        HI_U32 x, y;
        HI_U32 w, h;
    }HI_JPEG_RECT_S;

    /** Structure of the output yuvsp/rgb/yuv package message, only support 1/2/4/8/ scale */
    /** CNcomment:�������yuvsp/rgb/yuv pacekage �������Ϣ��ֻ֧��1/2/4/8���� CNend */
    typedef struct hiJPEG_SURFACE_S
    {
         HI_U32   u32OutPhy[MAX_PIXEL_COMPONENT_NUM];    /**< output physic address,0 is luminance or rgb buf, 1 is chroma buf    *//**<CNcomment:��������ַ��0�������y����rgb��ַ��1����Ϊɫ�ȵ�ַ CNend     */
         HI_CHAR* pOutVir[MAX_PIXEL_COMPONENT_NUM];      /**< output virtual address,0 is luminance or rgb buf, 1 is chroma buf   *//**<CNcomment:��������ַ��0�������y����rgb��ַ��1����Ϊɫ�ȵ�ַ CNend     */
         HI_U32   u32OutStride[MAX_PIXEL_COMPONENT_NUM]; /**< output stride                     *//**<CNcomment:����м�� CNend      */
    }HI_JPEG_SURFACE_S;

    /** Structure of the input and output jpeg width and height,input jpeg format */
    /** CNcomment:jpeg�ļ���ԭʼ��С�Լ����������С��ԭʼͼƬ��ʽ CNend */
    typedef struct hiJPEG_INFO_S
    {
         HI_BOOL bOutInfo; /**< false is get input information and the other get output information   *//**<CNcomment:FALSE��ȡ������Ϣ��TRUE��ȡ�����Ϣ CNend */
         HI_U32  u32Width[MAX_PIXEL_COMPONENT_NUM];     /**< 0 input and output rgb width and lu width,1 is ch width               *//**<CNcomment:0����ͽ������rgbͼƬ��Ȼ��߽���yuvsp���ȿ�ȣ�1Ϊɫ�ȿ�� CNend          */
         HI_U32  u32Height[MAX_PIXEL_COMPONENT_NUM];    /**< 0 input and output rgb height and lu height,1 is ch height            *//**<CNcomment:0����ͽ������rgbͼƬ�߶Ȼ��߽���yuvsp���ȸ߶ȣ�1Ϊɫ�ȸ߶� CNend          */
         HI_U32  u32McuHeight[MAX_PIXEL_COMPONENT_NUM]; /**< 0 input and output rgb height and lu height mcu align,1 is ch height  *//**<CNcomment:0����ͽ������rgbͼƬ�߶Ȼ��߽���yuvsp���ȸ߶ȣ�MCU���룬1Ϊɫ�ȸ߶� CNend */
         HI_U32  u32OutStride[MAX_PIXEL_COMPONENT_NUM]; /**< 0 output rgb stride and lu stride,1 is ch stride                      *//**<CNcomment:0�������rgb��yubsp�������м�࣬1Ϊɫ���м���� CNend                      */
         HI_U32  u32OutSize[MAX_PIXEL_COMPONENT_NUM];   /**< 0 output rgb size and lu size,1 is ch size                               *//**<CNcomment:0�������rgb��yubsp�Ĵ�С��1Ϊɫ�ȴ�С CNend                                   */
         HI_JPEG_FMT_E enFmt;  /**< the input format  *//**<CNcomment:�����ͼƬ��ʽ CNend      */
    }HI_JPEG_INFO_S;

    typedef struct hiJPEG_INPUT_DATA_BUF_INFO_S
    {
       HI_BOOL BeginReadToInputDataBuf ;  /**<-- check whether can read input data to data buffer >**/
       HI_VOID *pInputDataVirBuf;         /**<-- input data virtual buffer   >**/
       HI_U32 InputDataPhyBuf;            /**<-- input data physical buffer >**/
       HI_U32 InputDataBufSize;           /**<-- input data buffer size       >**/
    }HI_JPEG_INPUT_DATA_BUF_INFO_S;

    /** @} */  /** <!-- ==== Structure Definition End ==== */

    /********************** Global Variable declaration **************************/

    /******************************* API declaration *****************************/


    /** \addtogroup      JPEG */
    /** @{ */    /** <!--[JPEG]*/

    /**
       \brief get input data buffer information. CNcomment:��ȡ��������buffer ��Ϣ CNend
       \attention \n
        at fill input buffer function, before read data to which buffer, can call this function\n
       CNcomment:�����������ʱ�򣬿���ͨ���ú����жϿ���������д��������CNend\n

       \param[in]    cinfo. CNcomment:������� CNend
       \param[in]    *pInputDataBufInfo. CNcomment:��������buffer ��ϢCNend

       \retval ::NA

       \see \n
       ::HI_JPEG_GetInputDataBufInfo
       */
    HI_VOID HI_JPEG_GetInputDataBufInfo(const struct jpeg_decompress_struct *cinfo, HI_JPEG_INPUT_DATA_BUF_INFO_S *pInputDataBufInfo);


    /**
       \brief enable read data to input data buffer, that decode no need cpy data again. CNcomment:ʹ��������buffer �����ݣ������ʱ����Ҫ�ٿ���CNend
       \attention \n
        after fill input buffer function, can call this function\n
       CNcomment:�������������ʱ��ʹ���Ƿ�������buffer ������CNend\n

       \param[in]    cinfo. CNcomment:������� CNend
       \param[in]    BeginReadDataToInputDataBuf. CNcomment:�Ƿ�������buffer ������CNend

       \retval ::NA

       \see \n
       ::HI_JPEG_EnReadDataToInputDataBuf
       */
    HI_VOID HI_JPEG_EnReadDataToInputDataBuf(const struct jpeg_decompress_struct *cinfo, HI_BOOL EnReadToInputDataBuf);


    /**
       \brief exit jpeg decode. CNcomment:�˳�jpeg ����CNend
       \attention \n
        when you want not to decode, call this function\n
       CNcomment:�����Ƿ��˳�����CNend\n

       \param[in]    cinfo. CNcomment:������� CNend

       \retval ::NA

       \see \n
       ::HI_JPEG_StopDec
       */
    HI_VOID HI_JPEG_StopDec(const struct jpeg_decompress_struct *cinfo);

     /**
        \brief set stream mem information. CNcomment:���������ڴ���Ϣ CNend
        \attention \n
         if want to use this function,should call after jpeg_stdio_src or jpeg_mem_src\n
        CNcomment:���Ҫ���ã������ڹ�������֮��� CNend\n

        \param[in]    cinfo. CNcomment:������� CNend
        \param[in]    StreamPhyAddr.    CNcomment:����buffer �ڴ����ʼ�����ַ CNend
        \param[in]    StremBufSize.       CNcomment:����buffer �ڴ�ش�С                         CNend
        \param[in]    pStreamVirBuf.      CNcomment:����buffer �ڴ����ʼ�����ַ��Ҳ���������Ƶĵڶ����ַCNend
        \param[in]    SecondStreamBufSize.    CNcomment: �ڶ�������buffer ��С CNend

        \retval ::NA

        \see \n
        ::HI_JPEG_SetStreamBufInfo
        */
     HI_VOID HI_JPEG_SetStreamBufInfo(const struct jpeg_decompress_struct *cinfo,HI_U32 StreamPhyAddr,HI_U32 StremBufSize,HI_CHAR* pStreamVirBuf, HI_U32 SecondStreamBufSize);


     /**
        \brief set decode crop rect. CNcomment:���òü����� CNend
        \attention \n
        HI_JPEG_SetCropRect should have called after jpeg_crop_scanline\n
        CNcomment:�ڵ���jpeg_crop_scanline  ֮����� CNend\n

        \param[in]    cinfo. CNcomment:������� CNend
        \param[in]    *pstCropRect. CNcomment:����������� CNend

        \retval ::NA

        \see \n
        ::HI_JPEG_SetOutDesc
        */
     HI_VOID HI_JPEG_SetCropRect(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_RECT_S *pstCropRect);

    /**
    \brief Sets dec output message. CNcomment:���ý����������Ϣ������ CNend
    \attention \n
    HI_JPEG_SetOutDesc should have called create jpeg decoder.set the output address \n
    and output stride,set whether crop, set crop rect \n
    CNcomment:�����ڴ���������֮����������֮ǰ���øýӿڣ���Ҫ���ý��������ַ����� \n
              �м�࣬�����Ƿ�ü��Լ���Ӧ�Ĳü����� CNend\n

    \param[in]    cinfo. CNcomment:������� CNend
    \param[in]    *pstSurfaceDesc. CNcomment:�������������Ϣ CNend

    \retval ::HI_SUCCESS
    \retval ::HI_FAILURE

    \see \n
    ::HI_JPEG_SetOutDesc
    */
    HI_S32    HI_JPEG_SetOutDesc(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_SURFACE_S *pstSurfaceDesc);

    /**
    \brief Get Jpeg information. CNcomment:��ȡjpegͼƬ��Ϣ CNend
    \attention \n
    if you want to get input format and input width and input height,you should set bOutInfo false.\n
    others you can get the information as follows: output rgb widht/height/stride/size or output \n
    yuvsp lu width/height/stride/size and ch width/height/stride/size.\n
    you call this function should after read header and set the ouput parameter.\n
    CNcomment:��bOutInfo���ó�FALSE��ʱ�򣬿��Ի�ȡ��ͼƬ����Ŀ�Ⱥ͸߶��Լ����ظ�ʽ�������ó�TRUE�� \n
              ʱ������Ի�ȡ��������Ϣ��Ҫ�ǽ���RGB���ȡ�����/�߶�/�м��/��С,Ҫ�ǽ������yuvsp��\n
              ����Ի�ȡ�����Ⱥ�ɫ�ȵĿ��/�߶�/�м��/��С����Ϣ�� CNend\n

    \param[in]    cinfo. CNcomment:������� CNend
    \param[out]    pJpegInfo.  CNcomment:����jpeg�������Ϣ  CNend

    \retval ::HI_SUCCESS
    \retval ::HI_FAILURE

    \see \n
    ::HI_JPEG_GetJpegInfo
    */
    HI_S32  HI_JPEG_GetJpegInfo(j_decompress_ptr cinfo, HI_JPEG_INFO_S *pJpegInfo);

    /**
    \brief get lu pixle sum value. CNcomment:��ȡ����ֵ CNend
    \attention \n
    If you want to get the luminance value, you can call this function, \n
    but you should call it after jpeg_start_decompress and have call HI_JPEG_SetLuPixSumFlag.\n
    CNcomment:Ҫ����õ�����ֵ�����Ե��øú�������������jpeg_start_decompress֮����ö��ҽ��� \n
              ֮ǰҪ����HI_JPEG_SetLuPixSumFlag CNend\n

    \param[in]    cinfo. CNcomment:������� CNend
    \param[out]    u64LuPixSum. CNcomment:�������ֵ CNend

    \retval ::HI_SUCCESS
    \retval ::HI_FAILURE

    \see \n
    ::HI_JPEG_GetLuPixSum
    */
    HI_S32 HI_JPEG_GetLuPixSum(const struct jpeg_decompress_struct *cinfo,HI_U64 *u64LuPixSum);

    /**
        \brief set low delay information. CNcomment:���õ��ӳ���Ϣ CNend
        \attention the s32LineNum value should in [1 ~ output height]\n
        NA.\n
        CNcomment:CNend\n

        \param[in]    cinfo.         CNcomment:�������                     CNend
        \param[in]    u32PhyLineBuf. CNcomment:�����к�������ַ             CNend
        \param[in]    pVirLineBuf.   CNcomment:�����к������ַ             CNend

        \retval ::HI_SUCCESS
        \retval ::HI_FAILURE

        \see \n
        ::HI_JPEG_SetLowDelayInf
        */
    HI_S32 HI_JPEG_SetLowDelayInf(const struct jpeg_decompress_struct *cinfo, HI_U32 u32PhyLineBuf, HI_CHAR* pVirLineBuf);


    /** @} */  /** <!-- ==== API Declaration End ==== */

    /****************************************************************************/


#ifdef __cplusplus

#if __cplusplus

}
#endif
#endif /* __cplusplus */

#endif /* __HI_JPEG_API_H__*/
