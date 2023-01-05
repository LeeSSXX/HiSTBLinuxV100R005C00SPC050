/******************************************************************************

  Copyright (C), 2015-2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_tee_video_watermark.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2016/06/08
  Description   :
  History       :
  1.Date        : 2016/06/08
    Author      : sdk
    Modification: Created file

*******************************************************************************/
/**
 * \file
 * \brief Describes the API about the video watermark.
          CNcomment:�ṩ��ƵˮӡAPI CNend
 */
#ifndef __HI_TEE_VIDEO_WATERMARK_H__
#define __HI_TEE_VIDEO_WATERMARK_H__

#include "hi_type.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup      VIDEO_WATERMARK */
/** @{ */  /** <!-- [VIDEO_WATERMARK] */

/**Video watermark type */
/**CNcomment:��Ƶˮӡ���� */
typedef enum hiTEE_VWM_TYPE_E
{
    HI_TEE_VWM_TYPE_NEXGUARD = 1,   /**<Nexguard*/
    HI_TEE_VWM_TYPE_VERIMATRIX = 2, /**<Verimatrix*/
    HI_TEE_VWM_TYPE_BUTT
}HI_TEE_VWM_TYPE_E;

/**Nexguard watermark setting data type */
/**CNcomment:Nexguard��Ƶˮӡ�������� */
typedef enum hiTEE_VWM_NEXGUARD_SETTING_TYPE_E
{
    HI_TEE_VWM_NEXGUARD_SETTING_8bit =0,        /**<8bit setting*/
    HI_TEE_VWM_NEXGUARD_SETTING_10bit,          /**<10bit setting*/
    HI_TEE_VWM_NEXGUARD_SETTING_HDR,            /**<HDR setting*/
    HI_TEE_VWM_NEXGUARD_SETTING_CERTIFICATE,    /**<Certificate setting*/
    HI_TEE_VWM_NEXGUARD_SETTING_BUTT
}HI_TEE_VWM_NEXGUARD_SETTING_TYPE_E;

typedef struct hiTEE_VWM_USERSETTING_S
{
    HI_U8  *pu8DataBuf;
    HI_U32 u32DataLength;
}HI_TEE_VWM_USERSETTING_S;

/**Video watermark instance attribute */
/**CNcomment:��Ƶˮӡʵ������ */
typedef struct hiTEE_VWM_INS_ATTR_S
{
    HI_TEE_VWM_TYPE_E enType;       /**<Watermark type */
    HI_U32 u32ServiceId;            /**<Service ID */
}HI_TEE_VWM_INS_ATTR_S;

/**Nexguard watermark control parameter */
/**CNcomment:Nexguard��Ƶˮӡ���� */
typedef struct hiTEE_VWM_NEXGUARD_PARAM_S
{
    HI_BOOL bEnable;                /**<Enable watermark or not */
    HI_U32 u32Key;                  /**<If use reg key, config here; if use OTP key, don't care. */
    HI_U32 u32SubscriberID;         /**<If use reg SubscriberID, config here; if use OTP SubscriberID, don't care. */
    HI_U8 u8OperatorID;             /**<If use reg peratorID, config here; if use OTP peratorID, don't care. */
    HI_TEE_VWM_USERSETTING_S    astUserSetting[HI_TEE_VWM_NEXGUARD_SETTING_BUTT]; /**<user setting. */
}HI_TEE_VWM_NEXGUARD_PARAM_S;

/**Verimatrix watermark control parameter */
/**CNcomment:Verimatrix��Ƶˮӡ���� */
typedef struct hiTEE_VWM_VERIMATRIX_PARAM_S
{
    HI_U8 version_major;
    HI_U8 version_minor;

    //embedding part
    HI_U8 watermark_on;
    HI_U8 frequency_distance[3][3];
    HI_U8 background_embedding_on;
    HI_U16 embedding_strength_threshold_8[3];
    HI_U16 embedding_strength_threshold_bg_8[3];
    HI_U16 embedding_strength_threshold_10[12];
    HI_U16 embedding_strength_threshold_bg_10[12];
    HI_U16 embedding_strength_threshold_12[48];
    HI_U16 embedding_strength_threshold_bg_12[48];
    HI_U16 direction_max;
    HI_S8 strength_multiply;

    //rendering part
    HI_U8 payload_symbols[1920];
    HI_U8 symbols_rows;
    HI_U8 symbols_cols;
    HI_U8 symbols_xpos;
    HI_U8 symbols_ypos;
    HI_U8 symbol_size;
    HI_U16 spacing_vert;
    HI_U16 spacing_horz;
    HI_U8 symbol_scale_control;
}HI_TEE_VWM_VERIMATRIX_PARAM_S;

/**Watermark parameter */
/**CNcomment:��Ƶˮӡ���� */
typedef union hiTEE_VWM_PARAM_U
{
    HI_TEE_VWM_NEXGUARD_PARAM_S     stNexguard;     /**<Nexguard watermark parameter */
    HI_TEE_VWM_VERIMATRIX_PARAM_S   stVerimatrix;   /**<Verimatrix watermark parameter */
}HI_TEE_VWM_PARAM_U;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API declaration *****************************/
/** \addtogroup      VIDEO_WATERMARK */
/** @{ */  /** <!-- [VIDEO_WATERMARK] */

/**
\brief Initializes the video watermark module.CNcomment:��ʼ��VMWģ�� CNend
\attention \n
Before calling ::HI_TEE_VWM_Create to create an watermark instance, you must call this API to initialize the module.
CNcomment �ڵ���::HI_TEE_VWM_Create����ˮӡʵ��ǰ��Ҫ�����ȵ��ñ��ӿ��Գ�ʼ��ģ�� CNend
\param[in] enType   Video watermark type. It can accept one type or HI_TEE_VWM_TYPE_NEXGUARD | HI_TEE_VWM_TYPE_VERIMATRIX.
CNcomment:���ʼ������Ƶˮӡ����,���Խ���һ�ֻ��������(HI_TEE_VWM_TYPE_NEXGUARD | HI_TEE_VWM_TYPE_VERIMATRIX) . CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM  Invalid input parameter. CNcomment:��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_VWM_Init(HI_TEE_VWM_TYPE_E enType);

/**
\brief Deinitializes the video watermark module.CNcomment:ȥ��ʼ��VMWģ�� CNend
\attention \n
Before calling ::HI_TEE_VWM_Create to create an watermark instance, you must call this API to initialize the module.
CNcomment �ڵ���::HI_TEE_VWM_Create����ˮӡʵ��ǰ��Ҫ�����ȵ��ñ��ӿ��Գ�ʼ��ģ�� CNend
\param[in] enType   Video watermark type. It can accept one type or HI_TEE_VWM_TYPE_NEXGUARD | HI_TEE_VWM_TYPE_VERIMATRIX.
CNcomment:��ȥ��ʼ������Ƶˮӡ����,���Խ���һ�ֻ��������(HI_TEE_VWM_TYPE_NEXGUARD | HI_TEE_VWM_TYPE_VERIMATRIX) . CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM  Invalid input parameter. CNcomment:��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_VWM_DeInit(HI_TEE_VWM_TYPE_E enType);

/**
\brief Create a video watermark instance.CNcomment:����һ����Ƶˮӡʵ�� CNend
\attention \n
You need indicate which type to use. Don't need to care u32ServiceId if you use Nexguard.
CNcomment ��ָ��ˮӡ����;�����Nexguard,����Ҫ����u32ServiceId���� CNend
\param[in] pstInstAttr  Video watermark instance attribute. CNcomment:��Ƶˮӡʵ������. CNend
\param[out] phVWM       Output the handle of video watermark instance. CNcomment:��Ƶˮӡʵ�����. CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM  Invalid input parameter. CNcomment:��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_VWM_Create(HI_TEE_VWM_INS_ATTR_S *pstInstAttr, HI_HANDLE *phVWM);

/**
\brief Destroy a video watermark instance.CNcomment:����һ����Ƶˮӡʵ�� CNend
\attention \n
    None. CNcomment �� CNend
\param[in] hVWM     Video watermark instance handle. CNcomment:��Ƶˮӡʵ�����. CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM  Invalid input parameter. CNcomment:��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_VWM_Destroy(HI_HANDLE hVWM);

/**
\brief Set video watermark parameter and start/stop it.CNcomment:����ˮӡ�����Լ�����/ֹͣˮӡ���� CNend
\attention \n
    None. CNcomment �� CNend
\param[in] hVWM         Video watermark instance handle. CNcomment:��Ƶˮӡʵ�����. CNend
\param[in] punParam     Video watermark parameter. CNcomment:��Ƶˮӡʵ������. CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM  Invalid input parameter. CNcomment:��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_VWM_SetParameter(HI_HANDLE hVWM, HI_TEE_VWM_PARAM_U *punParam);

/**
\brief Get video watermark parameter.CNcomment:��ȡˮӡ���� CNend
\attention \n
    None. CNcomment �� CNend
\param[in] hVWM         Video watermark instance handle. CNcomment:��Ƶˮӡʵ�����. CNend
\param[out] punParam    Video watermark parameter. CNcomment:��Ƶˮӡʵ������. CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM  Invalid input parameter. CNcomment:��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_VWM_GetParameter(HI_HANDLE hVWM, HI_TEE_VWM_PARAM_U *punParam);


/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif

#endif

