
/*******************************************************************************
Copyright (C), 2009-2012, Hisilicon Tech. Co., Ltd.
File name: imedia_videnc.h
Author & ID: ��С��+00133955
Version: 1.00
Date:  2009-12-18
Description: ���ļ��������м䴦��ģ��Ľӿں�������ؽṹ����
Function List:

History:
* Revision 1.00  2010/03/09 10:00:00  songxg+00133955
* ��ʼ�汾�������
*
*******************************************************************************/
#ifndef __QUALITY_PARAMS_H__
#define __QUALITY_PARAMS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif    /* __cpluscplus*/
#endif    /* __cpluscplus*/

#include "iMedia_common.h"

#pragma pack(push, 1)

typedef enum ENUM_IVIDENC_ME_METHOD
{
    ME_DIA = 0,     /**< Diamond search*/
    ME_HEX,         /**< Hexagonal search */
    ME_UMH,         /**< Uneven Multi-hexagon search */
    //ME_FULL,      /**< Full motion search */
} ENUM_IVIDENC_ME_METHOD;

typedef enum ENUM_IVIDENC_RC_METHOD
{
    RC_CQP = 0,
    RC_CBR,
    RC_VBR,
} ENUM_IVIDENC_RC_METHOD;

typedef enum ENUM_IVIDENC_PICTURE_CODING_TYPE
{
    PIC_CODING_FRAME = 0,    /**< ֡���� */
    PIC_CODING_FIELD,        /**< ������ */

    /**< ��H264��Ч */
    PIC_CODING_PICAFF,       /**< ֡��ͼ������Ӧ����(NO MBAFF) */
    PIC_CODING_MBAFF,        /**< ֡���������Ӧ����(ONLY MBAFF) */
    PIC_CODING_PICAFF_MBAFF, /**< ֡��ͼ��ͺ������Ӧ���� */
} ENUM_IVIDENC_PICTURE_CODING_TYPE;

typedef enum ENUM_IVIDENC_SCENECUT_METHOD
{
    SCENECUT_NONE = 0,       /* not detect scene-cut */
    SCENECUT_SIMPLE,         /* simple scene-cut detection method, real-time*/
    SCENECUT_COMPLEX,        /* complex scene-cut detection with fade */
} ENUM_IVIDENC_SCENECUT_METHOD;

/* Define B as reference picture method. */
typedef enum ENUM_IVIDENC_BREF_METHOD
{
    BREF_NONE,          /* no B picture as reference pic */
    BREF_PYRAMID        /* the center B ref picture as reference */
    //BREF_ALL          /* all B picture as reference pic, not support */
} ENUM_IVIDENC_BREF_METHOD;

typedef enum ENUM_IVIDENC_QUANT_TYPE
{
    QUANT_NORMAL,    /* normal quantization */
    QUANT_TRELLIS    /* trellis, Rate Distortion Optimized Quantization */
    //QUANT_AROUND   /* not support, Adaptive Rounding based on JVT-N011 */
} ENUM_IVIDENC_QUANT_TYPE;

typedef enum ENUM_IVIDENC_DIRECT_MODE
{
    DIRECT_NONE,         /* none */
    DIRECT_SPATIAL,      /* spatial direct mode */
    DIRECT_TEMPORAL,     /* temporal direct mode */
    DIRECT_ADAPT         /* adapt between spatial and temporal direct mode */
} ENUM_IVIDENC_DIRECT_MODE;

typedef enum ENUM_IVIDENC_WEIGHTED_PRED
{
    WP_OFF,
    WP_EXPLICIT,     /* Explicit Weighted Prediction, now not support */
    WP_IMPLICIT      /* Implicit Weighted Prediction */
} ENUM_IVIDENC_WEIGHTED_PRED;

typedef enum ENUM_IVIDENC_FILTER_TYPE
{
    FILTER_FILTER,    /* enable loop filter */
    FILTER_NONE,      /* disable loop filter*/
    FILTER_NOSLICE    /* no filter in slice edge */
} ENUM_IVIDENC_FILTER_TYPE;

/**< ��H264��Ч */
typedef enum ENUM_IVIDENC_SLICE_MODE
{
    SLICE_MODE_ONLY_ONE = 0,    /**< ��֡һ��slice */
    SLICE_MODE_FIXMB,           /**< �����������slice */
    SLICE_MODE_FIXBYTE,         /**< ���ֽ�������slice */
    SLICE_MODE_STRIP,           /**< ���л���slice����һ�к��Ϊһ���� */
} ENUM_IVIDENC_SLICE_MODE;

/*!��Ƶ�����������Ʋ���*/
// ����Ƶ���������������нϴ�Ӱ��Ĳ��������㷨����Ϥ���˿�ͨ����������Ż����������ݲ���������ܻ��в����ֶε�����
typedef struct STRU_IVIDENC_QUALITY_PARAMS
{
    ENUM_IVIDEO_CODEC_LEVEL  eLevel;    /**< level idc */
    ENUM_IVIDENC_PICTURE_CODING_TYPE ePicCodingType; /**< ͼ��������� (FRAME, FIELD, PICAFF, ...) */
    INT32  iMultiPass;

    BOOL   bCabacFlag;      /**< �ر���ģʽ 0-CAVLC 1-CABAC */
    BOOL   bDCT8x8Flag;

    /**< �����ز��� */
    //INT32  iRCMethod;            /**< ������ͣ�һ��ֻ��CBR(��ABRʵ��) */
    UINT32 uiVbvBufDelay;          /**< VBV Buffer, ��λms, Range[500-30000] */
    //UINT32 uiInitVbvBuf;         /**< Init VBV Buffer, ��λms */
    BOOL   bSkipFrameFlag;         /**< ������֡��ʶ�����޷�����Ŀ������ʱ������֡��� */
    INT32  iChromaQPOffset;        /**< Chroma QP offset (-12..12) */
    INT32  iSecondChromaQPOffset;  /**< Secondary chroma QP offset  */
    //INT32  iQPISlice;             /**< I Slices��ʼQPֵ (0-51) */
    //INT32  iQPSlice;              /**< non-I Slices��ʼQPֵ */
    //INT32  iRCQPMax;              /**< ���QPֵ  Range[0,51] */
    //INT32  iRCQPMin;              /**< ��СQPֵ Range[0,51] */
    //INT32  iRCQPDiff;             /**< ��ص���ʱQP�仯�����ֵ */

    /**< �����㷨��ز��� */
    ENUM_IVIDENC_ME_METHOD eMEMethod;  /**< ME����: Full Search or Low Power ME or Hybrid ME */
    INT32  iSubMELevel;
    INT32  iMERange;
    INT32  iRefFrameNum;
    BOOL   bMixedRefFlag;
    BOOL   bChromaMEFlag;
    BOOL   bBiMEFlag;
    BOOL   bFastPSkipFlag;

    /**< ǰ�����㷨��ز��� */
    ENUM_IVIDENC_SCENECUT_METHOD eScenecutType;

    /**< B֡��ز��� */
    ENUM_IVIDENC_BREF_METHOD eBFrameRefType;         /**< B֡���ο����� */
    ENUM_IVIDENC_DIRECT_MODE eBFrameDirectMode;
    INT32  iBFrameNum;
    //INT32  iBFrameDirect8x8;
    BOOL   bBFrameAdaptFlag;
    BOOL   bBFrameRDOFlag;

    /**< ���ģʽ��ز��� */
    BOOL  bI4x4Flag;        //use I4x4 or not.
    BOOL  bI8x8Flag;        //use I8x8 or not.
    BOOL  bPSub16x16Flag;   //use sub P16x16 or not.
    BOOL  bPSub8x8Flag;     //use sub P8x8 or not.
    BOOL  bBSub16x16Flag;   //use sub B16x16 or not.

    /**< ��ȨԤ����ز��� */
    BOOL  bWPFlag;          //weighted prediction on or not.

    /**< ��·�˲���ز��� */
    ENUM_IVIDENC_FILTER_TYPE eLfDisableIdc;         /**< Loop Filter enable/disable control */
    INT32  iLFAlphaOffset;         /**< Alpha & C0 offset div. 2��Range[-6-6] */
    INT32  iLFBetaOffset;          /**< Beta offset div. 2, Range[-6-6] */

    ENUM_IVIDENC_QUANT_TYPE eQuantType;
    // slice partition
    ENUM_IVIDENC_SLICE_MODE eSliceMode;             /**< slice����ģʽ */
    INT32  iSliceParam;            /**< ��slice����ģʽΪSLICE_MODE_FIXMB��SLICE_MODE_FIXBYTEʱ����Ӧ��ֵ */
} STRU_IVIDENC_QUALITY_PARAMS;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif    /* __cpluscplus*/
#endif    /* __cpluscplus*/

#endif /**< __QUALITY_PARAMS_H__ */

/************************************************************************
* $Log$
************************************************************************/
