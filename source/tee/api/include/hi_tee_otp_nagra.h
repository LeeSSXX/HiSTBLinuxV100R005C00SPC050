/******************************************************************************

  Copyright (C), 2015-2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_tee_otp_nagra.h
  Version       : Initial Draft
  Author        : Hisilicon hisecurity team
  Created       : 2013-12-28
  Description   :
  History       :
  1.Date        : 2016/06/23
    Author      : sdk
    Modification: format file

*******************************************************************************/
/**
 * \file
 * \brief Describes the API about the otp.
          CNcomment:�ṩotp API CNend
 */

#ifndef __HI_TEE_OTP_NAGRA_H__
#define __HI_TEE_OTP_NAGRA_H__

#include "hi_type.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup      OTP_NAGRA */
/** @{ */  /** <!-- [OTP_NAGRA] */

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API declaration *****************************/
/** \addtogroup      OTP_NAGRA */
/** @{ */  /** <!-- [OTP_NAGRA] */

/**
\brief Get the nuid of the STB. CNcomment:��ȡNUID CNend
\attention \n
None CNcomment:�� CNend
\param[out] pu8Nuid NUID. CNcomment:ָ�����ͣ�NUID CNend
\param[in/out] pu32Len point to the length of NUID, current is 4. CNcomment:ָ�����ͣ�NUID���ȣ���ǰ��4 CNend
\retval ::HI_SUCCESS Success.  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Failure.  CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_TEE_ERR_UNINITED The advanced OTP module is not initialized CNcomment:HI_TEE_ERR_UNINITED  OTPδ��ʼ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM The input parameter value is invalid CNcomment:HI_TEE_ERR_INVALID_PARAM  ��������Ƿ� CNend
\see \n
::HI_UNF_OTP_NAGRA_GetNUID
*/
HI_S32 HI_TEE_OTP_NAGRA_GetNUID(HI_U8 *pu8Nuid, HI_U32 *pu32Len);


/**
\brief enable nagra privileged mode. CNcomment:ʹ��nagra ��Ȩģʽ CNend
\attention \n
N/A
\retval ::HI_SUCCESS Success CNcomment:HI_SUCCESS �ɹ� CNend
\retval ::HI_FAILURE This API fails to be called CNcomment:HI_FAILURE  APIϵͳ����ʧ�� CNend
\retval ::HI_TEE_ERR_UNINITED The advanced OTP module is not initialized CNcomment:HI_TEE_ERR_UNINITED  OTPδ��ʼ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM The input parameter value is invalid CNcomment:HI_TEE_ERR_INVALID_PARAM  ��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_NAGRA_EnablePrivilegedMode(HI_VOID);

/**
\brief get nagra privileged mode. CNcomment:��ȡnagra Ȩģʽ CNend
\attention \n
N/A
\param[out]  pbPVMode Point to the privileged mode . Ncomment: ָ�����ͣ���ȡ��ȨģʽCNend
\retval ::HI_SUCCESS Success CNcomment:HI_SUCCESS �ɹ� CNend
\retval ::HI_FAILURE This API fails to be called CNcomment:HI_FAILURE  APIϵͳ����ʧ�� CNend
\retval ::HI_TEE_ERR_UNINITED The advanced OTP module is not initialized CNcomment:HI_TEE_ERR_UNINITED  OTPδ��ʼ�� CNend
\retval ::HI_TEE_ERR_INVALID_PARAM The input parameter value is invalid CNcomment:HI_TEE_ERR_INVALID_PARAM  ��������Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_NAGRA_GetPrivilegedMode(HI_BOOL *pbPVMode);


/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif

#endif

