/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_tee_mailbox.h
  Version       : Initial Draft
  Author        : Hisilicon hisecurity team
  Created       : 2016-11-4
  Description   :
  History       :
  1.Date        :
  Author        : sdk
  Modification  :

*******************************************************************************/

#ifndef __HI_TEE_MAILBOX_H__
#define __HI_TEE_MAILBOX_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/** Mailbox ID */
#define MID_SCPU_COMMON          (0x01)
#define MID_TCPU_COMMON          (0x40)

typedef struct
{
    const void * pMsgBuf;
    HI_U32 u32MsgLen;
    void * pBufOut;
    HI_U32 *pu32BufOutLen;
} HI_TEE_MAILBOX_BUFFER_S;

/******************************* API declaration *****************************/
/**
\brief Open mailbox device.                                                     CNcomment:�� mailbox �豸������ maibox ����� CNend
\attention \n
N/A
\param[out] pHandle: Create handle/A                                             CNcomment:�ޡ� CNend
\param[in]  s32Id:  mailbox ID/A                                                CNcomment:�ޡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:ϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API failure.                                    CNcomment:������ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_MAILBOX_Open(HI_HANDLE* pHandle, HI_U32 u32Id);

/**
\brief Destory mailbox instance.                                                CNcomment:�ر� mailbox �豸ʵ���� CNend
\attention \n
N/A
\param [in] handle: mailbox instance handle/A                                   CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:ϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API failure.                                    CNcomment:�ͷž��ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_MAILBOX_Close(HI_HANDLE handle);

/**
\brief  send data by mailbox.                                                   CNcomment:ͨ�� mailbox �������ݡ� CNend
\attention \n
N/A
\param [in] handle: mailbox instance handle/A                                   CNcomment:�ޡ� CNend
\param [in] u32ReceiverId: mailbox ID of data receiver/A                        CNcomment:�ޡ� CNend
\param [in] pstBuffer: mailbox input and output /A                              CNcomment:�ޡ� CNend
\param [in] s32Flags: mail box mode/A                                           CNcomment:��ʱģʽ������ģʽ�� CNend
\param [in] S32Param: mailbox max wait time/A                                   CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:ϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API failure.                                    CNcomment:ϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_MAILBOX_Send
(
    HI_HANDLE handle,
    HI_U32 u32ReceiverId,
    HI_TEE_MAILBOX_BUFFER_S *pstBuffer,
    HI_U32 u32Flags,
    HI_U32 u32Param
);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif

#endif /** __HI_TEE_MAILBOX_H__*/

