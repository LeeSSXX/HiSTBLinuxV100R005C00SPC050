/******************************************************************************

  Copyright (C), 2015-2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_tee_otp.h
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

#ifndef __HI_TEE_OTP_H__
#define __HI_TEE_OTP_H__

#include "hi_type.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup      OTP */
/** @{ */  /** <!-- [OTP] */

/** @} */  /** <!-- ==== Structure Definition end ==== */
typedef enum
{
    OTP_TA_INDEX_1 = 0,
    OTP_TA_INDEX_2,
    OTP_TA_INDEX_3,
    OTP_TA_INDEX_4,
    OTP_TA_INDEX_5,
    OTP_TA_INDEX_6
} OTP_TA_INDEX_E;

/** @} */  /** <!-- ==== Structure Definition end ==== */

#define HI_TEE_OTP_ROOTKEY_LENGTH       (0x10)
#define HI_TEE_OTP_CHIPID_LENGTH        (0x08)

/******************************* API declaration *****************************/
/** \addtogroup      OTP */
/** @{ */  /** <!-- [OTP] */

/**
\brief Init an otp device.
\brief CNcomment:��ʼ��OTP�豸�� CNend
\attention \n
N/A
\param N/A                                          CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_Init(HI_VOID);

/**
\brief Deinit an otp device.
\brief CNcomment:ȥ��ʼ��OTP�豸�� CNend
\attention \n
N/A
\param N/A                                          CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_DeInit(HI_VOID);

/**
\brief reset the otp device.
\brief CNcomment:��λOTP�豸�� CNend
\attention \n
N/A
\param N/A                                          CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_Reset(HI_VOID);

/**
\brief Get the device chip ID.
\brief CNcomment:��ȡ�豸оƬ��ʶ�� CNend
\attention \n
N/A
\param[out] pKey:  Buffer to store the chip ID.     CNcomment:�洢��ȡоƬ��ʶ�Ļ�����ָ�롣 CNend
\param[in] u32ChipIDLength:  Buffer length.         CNcomment:���������ȡ� CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetChipID(HI_U8 *pu8ChipID, const HI_U32 u32ChipIDLength);

/**
\brief Read otp value by word.
\brief CNcomment���ֶ�ȡOTP��ֵ�� CNend
\attention \n
N/A
\param[in] u32Addr:  OTP address.                           CNcomment:OTP ��ַ�� CNend
\param[out] pu32Value:  Buffer to store the otp by word.    CNcomment:�洢���ֻ�ȡOTPֵ�Ļ�����ָ�롣 CNend
\retval ::HI_SUCCESS  Call this API successful.             CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                  CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_ReadWord(const HI_U32 u32Addr, HI_U32 *pu32Value);

/**
\brief Read otp value by byte.
\brief CNcomment���ֽڶ�ȡOTP��ֵ�� CNend
\attention \n
N/A
\param[in] u32Addr:  OTP address.                           CNcomment:OTP ��ַ�� CNend
\param[out] pu32Value:  Buffer to store the otp by word.    CNcomment:�洢���ֽڻ�ȡOTPֵ�Ļ�����ָ�롣 CNend
\retval ::HI_SUCCESS  Call this API successful.             CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                  CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_ReadByte(const HI_U32 u32Addr, HI_U8 *pu8Value);


/**
\brief Write otp value by byte.
\brief CNcomment���ֽ�дOTP��ֵ�� CNend
\attention \n
N/A
\param[in] u32Addr:  OTP address.                       CNcomment:OTP ��ַ�� CNend
\param[in] u32Value:   value to be write.                 CNcomment:��д��OTP��ֵ�� CNend
\retval ::HI_SUCCESS  Call this API successful.         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.              CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_WriteByte(const HI_U32 u32Addr, const HI_U8 u8Value);

/**
\brief Set secure store root key.
\brief CNcomment:���ð�ȫ�洢��Կ�� CNend
\attention \n
N/A
\param[in] pu8Key: Buffer to store secure store rookey.     CNcomment:�洢��ȫ�洢��Կ��ָ�롣 CNend
\param[out] u32Length:  Secure store rootkey length.        CNcomment:��ȫ�洢��Կ�ĳ��ȡ� CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_SetSecStoreKey(const HI_U8 *pu8Key, const HI_U32 u32Length);

/**
\brief Get secure store rootkey lock flag.
\brief CNcomment:��ȡ��ȫ�洢��Կ��״̬�� CNend
\attention \n
N/A
\param[out] pbFlag:  Point to the secure store rootkey lock status.     CNcomment:��ȡ��ȫ�洢��״ָ̬�롣 CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetSecStoreKeyLockFlag(HI_U8 *pbFlag);


/**
\brief Get the device ca chip ID.
\brief CNcomment:��ȡ�߰�оƬID�� CNend
\attention \n
N/A
\param[out] pKey:  Buffer to store the CA chip ID.     CNcomment:�洢��ȡоƬ��ʶ�Ļ�����ָ�롣 CNend
\param[in]  u32ChipIDLength:  The length of the buffer.   CNcomment:�洢�߰�ChipID�Ļ��������ȡ� CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetCAChipID(HI_U8 *pu8CAChipID, const HI_U32 u32CAChipIDLen);

/**
\brief Set antirollback version
\brief CNcomment ���÷��ع��汾�š� CNend
\attention \n
N/A
\param[in] u32Addr:    OTP address.                       CNcomment:OTP ��ַ�� CNend
\param[in] u32Length:  The length of otp.                 CNcomment:���ع��汾��OTP���ȡ� CNend
\param[in] u32Version:  The version of otp.               CNcomment:���õ�version�� CNend
\retval ::HI_SUCCESS  Call this API successful.         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.              CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_SetAntiRollBackVersion(const HI_U32 u32OTPAddr, const HI_U32 u32Length, const HI_U32 u32Version);

/**
\brief Get antirollback version
\brief CNcomment ��ȡ���ع��汾�š� CNend
\attention \n
N/A
\param[in] u32Addr:    OTP address.                       CNcomment:OTP ��ַ�� CNend
\param[in] u32Length:  The length of otp.                 CNcomment:���ع��汾��OTP���ȡ� CNend
\param[out] pu32Version: antirollback version.            CNcomment:���ع��汾�š� CNend
\retval ::HI_SUCCESS  Call this API successful.         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.              CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetAntiRollBackVersion(const HI_U32 u32OTPAddr, const HI_U32 u32Length, HI_U32 *pu32Version);



/**
\brief Set ta certificate version
\brief CNcomment ����TA֤��汾�š� CNend
\attention \n
N/A
\param[in] enIndex:     TA index.                   CNcomment:TA ���� CNend
\param[in] u32Version:  version.                    CNcomment:�汾��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_SetTACertificateVersion(const OTP_TA_INDEX_E enIndex, const HI_U32 u32Version);

/**
\brief Get ta certificate version
\brief CNcomment ��ȡTA֤��汾�š� CNend
\attention \n
N/A
\param[in] enIndex:  TA index.                      CNcomment:TA ���� CNend
\param[out] pu32Version:  the point of version.     CNcomment:��ȡ�汾��ָ��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetTACertificateVersion(const OTP_TA_INDEX_E enIndex, HI_U32 *pu32Version);

/**
\brief Set ta secure version
\brief CNcomment ����TA�汾�š� CNend
\attention \n
N/A
\param[in] enIndex:     TA index.                   CNcomment:TA ���� CNend
\param[in] u32Version:  version.                    CNcomment:�汾��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_SetTASecureVersion(const OTP_TA_INDEX_E enIndex, const HI_U32 version);

/**
\brief Get ta secure version
\brief CNcomment ��ȡTA�汾�š� CNend
\attention \n
N/A
\param[in] enIndex:     TA index.                   CNcomment:TA ���� CNend
\param[out] pu32Version:  the point of version.     CNcomment:��ȡ�汾��ָ��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetTASecureVersion(const OTP_TA_INDEX_E enIndex, HI_U32 *pu32Version);


/**
\brief Get ta segmentid
\brief CNcomment ��ȡTA�����롣 CNend
\attention \n
N/A
\param[in] enIndex:     TA index.                   CNcomment:TA ���� CNend
\param[out] pu32TAID:  the point of TA id.          CNcomment:��ȡTA ID��ָ��  CNend
\param[out] pu32SMID:  the point of TA segment id.  CNcomment:��ȡTA������ָ��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetTAIDAndSegmentID(const OTP_TA_INDEX_E enIndex, HI_U32 *pu32TAID, HI_U32 *pu32SMID);

/**
\brief Get ta index
\brief CNcomment ����TA ID�Ż�ȡ����ֵ�� CNend
\attention \n
N/A
\param[in] u32TAID:     TA ID.                      CNcomment:TA ID�� CNend
\param[out] enIndex:    the point of TA index.      CNcomment:��ȡTA������ָ��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetTAIndex(const HI_U32 u32TAID, OTP_TA_INDEX_E *enIndex);

/**
\brief Get ta index
\brief CNcomment ��ȡ���õ�����ֵ�� CNend
\attention \n
N/A
\param[out] enIndex:    the point of available TA index.      CNcomment:��ȡ����TA������ָ��  CNend
\retval ::HI_SUCCESS  Call this API successful.     CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.          CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_OTP_GetAvailableTAIndex(OTP_TA_INDEX_E *enIndex);
/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif

#endif

