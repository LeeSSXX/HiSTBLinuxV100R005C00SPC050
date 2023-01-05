/******************************************************************************
 *
 * Copyright (C), 2004-2014, Hisilicon Tech. Co., Ltd.
 * ******************************************************************************
 * File Name     : drv_cipher_intf.h
 * Version       : Initial
 * Author        : Hisilicon hisecurity team
 * Created       : 2013-12-28
 * Last Modified :
 * Description   :
 * Function List :
 * History       :
 * ******************************************************************************/
#ifndef __TEE_CIPHER_INTF_H__
#define __TEE_CIPHER_INTF_H__

//#include "hi_common.h"
#include "tee_drv_cipher_define.h"
#include "tee_drv_cipher.h"
#include "hi_tee_drv_cipher.h"
#include "tee_cipher_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
/******************************* API Declaration *****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */
/* ---CIPHER---*/
/**
\brief  Init the cipher device.  CNcomment:��ʼ��CIPHER�豸�� CNend
\attention \n
This API is used to start the cipher device.
CNcomment:���ô˽ӿڳ�ʼ��CIPHER�豸�� CNend
\param N/A                   													CNcomment:�� CNend
\retval ::HI_SUCCESS  Call this API succussful. 								CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                   	CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_FAILED_INIT  The cipher device fails to be initialized. CNcomment:CIPHER�豸��ʼ��ʧ�� CNend
\see \n
N/A
*/
HI_S32 Cipher_Init(	HI_VOID);

/**
\brief  Deinit the cipher device.
CNcomment:\brief  ȥ��ʼ��CIPHER�豸�� CNend
\attention \n
This API is used to stop the cipher device. If this API is called repeatedly, HI_SUCCESS is returned, but only the first operation takes effect.
CNcomment:���ô˽ӿڹر�CIPHER�豸���ظ��رշ��سɹ�����һ�������á� CNend

\param N/A                   													CNcomment:�� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 		CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                            			CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.  		CNcomment:CIPHER�豸δ��ʼ�� CNend
\see \n
N/A
*/
HI_S32 Cipher_DeInit(HI_VOID);

/**
\brief Obtain a cipher handle for encryption and decryption.
CNcomment������һ·Cipher����� CNend

\param[in] cipher attributes                                             		CNcomment:cipher ���ԡ� CNend
\param[out] phCipher Cipher handle                                       		CNcomment:CIPHER����� CNend
\retval ::HI_SUCCESS Call this API succussful. 						 			CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.										CNcomment: APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.  		CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.              		CNcomment:ָ�����Ϊ�� CNend
\retval ::HI_ERR_CIPHER_FAILED_GETHANDLE  The cipher handle fails to be obtained, because there are no available cipher handles. CNcomment: ��ȡCIPHER���ʧ�ܣ�û�п��е�CIPHER��� CNend
\see \n
N/A
*/
HI_S32 Cipher_CreateHandle(HI_HANDLE* phCipher, const HI_TEE_CIPHER_ATTS_S *pstCipherAttr);

/**
\brief Destroy the existing cipher handle. CNcomment:�����Ѵ��ڵ�CIPHER����� CNend
\attention \n
This API is used to destroy existing cipher handles.
CNcomment:���ô˽ӿ������Ѿ�������CIPHER����� CNend

\param[in] hCipher Cipher handle                 								CNcomment:CIPHER����� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 		CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.    									CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized. 		CNcomment:CIPHER�豸δ��ʼ�� CNend
\see \n
N/A
*/
HI_S32 Cipher_DestroyHandle(HI_HANDLE hCipher);

/**
\brief Configures the cipher control information.
CNcomment:\brief ����CIPHER������Ϣ�� CNend
\attention \n
Before encryption or decryption, you must call this API to configure the cipher control information.
The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.
CNcomment:���м��ܽ���ǰ������ʹ�ô˽ӿ�����CIPHER�Ŀ�����Ϣ��ʹ��TDES�㷨ʱ��������Կ��ǰ��64 bit���ݲ�����ͬ�� CNend

\param[in] hCipher Cipher handle.                                        		CNcomment:CIPHER��� CNend
\param[in] pstCtrl Cipher control information.                           		CNcomment:CIPHER������Ϣ CNend
\retval ::HI_SUCCESS Call this API succussful. 						 			CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                           			CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized. 		CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.             		CNcomment:ָ�����Ϊ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.         		CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.          		CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 Cipher_ConfigHandle(HI_HANDLE hCipher, HI_TEE_CIPHER_CTRL_S* pstCtrl);

HI_S32 Cipher_GetHandleConfig(HI_HANDLE hCipher, HI_TEE_CIPHER_CTRL_S* pstCtrl);

/**
\brief Performs encryption.
CNcomment:\brief ���м��ܡ� CNend

\attention \n
This API is used to perform encryption by using the cipher module.
The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering HI_TEE_CIPHER_ConfigHandle.
CNcomment:ʹ��CIPHER���м��ܲ�����TDESģʽ�¼��ܵ����ݳ���Ӧ����8�ı�����AES��Ӧ����16�ı��������⣬�������ݳ��Ȳ��ܳ���0xFFFFF�����β�����ɺ󣬴˴β�������������������������һ�β��������Ҫ�����������Ҫ���´μ��ܲ���֮ǰ����HI_TEE_CIPHER_ConfigHandle��������IV(��Ҫ����pstCtrl->stChangeFlags.bit1IVΪ1)�� CNend
\param[in] hCipher Cipher handle  												CNcomment:CIPHER��� CNend
\param[in] u32SrcPhyAddr Physical address of the source data            		CNcomment:Դ���������ַ CNend
\param[in] u32DestPhyAddr Physical address of the target data           		CNcomment:Ŀ�����������ַ CNend
\param[in] u32ByteLength   Length of the encrypted data                 		CNcomment:�������ݳ��� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 		CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                           			CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized. 		CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.         		CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.          		CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 Cipher_Encrypt(HI_HANDLE hCipher, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength);

/**
\brief ���м���
\attention \n
This API is used to perform decryption by using the cipher module.
The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering HI_TEE_CIPHER_ConfigHandle.
CNcomment:ʹ��CIPHER���н��ܲ�����TDESģʽ�½��ܵ����ݳ���Ӧ����8�ı�����AES��Ӧ����16�ı��������⣬�������ݳ��Ȳ��ܳ���0xFFFFF�����β�����ɺ󣬴˴β�������������������������һ�β��������Ҫ�����������Ҫ���´ν��ܲ���֮ǰ����HI_TEE_CIPHER_ConfigHandle��������IV(��Ҫ����pstCtrl->stChangeFlags.bit1IVΪ1)�� CNend
\param[in] hCipher Cipher handle.                                        		CNcomment:CIPHER��� CNend
\param[in] u32SrcPhyAddr Physical address of the source data.            		CNcomment:Դ���������ַ CNend
\param[in] u32DestPhyAddr Physical address of the target data.           		CNcomment:Ŀ�����������ַ CNend
\param[in] u32ByteLength Length of the decrypted data                   		CNcomment:�������ݳ��� CNend
\retval ::HI_SUCCESS Call this API succussful. 						 			CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                           			CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized. 		CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.         		CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.          		CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 Cipher_Decrypt(HI_HANDLE hCipher, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength);


/**
\brief Encrypt multiple packaged data.
CNcomment:\brief ���ж�������ݵļ��ܡ� CNend
\attention \n
You can not encrypt more than 128 data package one time. When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When encrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_TEE_CIPHER_ConfigHandle.Previous result will not affect the later result.
CNcomment:ÿ�μ��ܵ����ݰ�������಻�ܳ���128��������HI_ERR_CIPHER_BUSY��ʱ���������ݰ�һ��Ҳ���ᱻ�����û���Ҫ������������ݰ������������ٴγ��Լ��ܡ�ע��: ���ڶ�����Ĳ�����ÿ������ʹ��HI_TEE_CIPHER_ConfigHandle���õ������������㣬ǰһ����������������������������һ���������㣬ÿ�������Ƕ�������ġ�ǰһ�κ������õĽ��Ҳ����Ӱ���һ�κ������õ��������� CNend
\param[in] hCipher cipher handle                                                                  CNcomment:CIPHER����� CNend
\param[in] pstDataPkg data package ready for cipher                                               CNcomment:�����ܵ����ݰ��� CNend
\param[in] u32DataPkgNum  number of package ready for cipher                                      CNcomment:�����ܵ����ݰ������� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 						  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                     	  CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                             CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                            CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time   CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 Cipher_EncryptMulti(HI_HANDLE hCipher, HI_TEE_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);


/**
\brief Decrypt multiple packaged data.
CNcomment:\brief ���ж�������ݵĽ��ܡ� CNend
\attention \n
You can not decrypt more than 128 data package one time.When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_TEE_CIPHER_ConfigHandle.Previous result will not affect the later result.
CNcomment:ÿ�ν��ܵ����ݰ�������಻�ܳ���128��������HI_ERR_CIPHER_BUSY��ʱ���������ݰ�һ��Ҳ���ᱻ�����û���Ҫ������������ݰ������������ٴγ��Խ��ܡ�ע��: ���ڶ�����Ĳ�����ÿ������ʹ��HI_TEE_CIPHER_ConfigHandle���õ������������㣬ǰһ����������������������������һ���������㣬ÿ�������Ƕ�������ģ�ǰһ�κ������õĽ��Ҳ����Ӱ���һ�κ������õ��������� CNend
\param[in] hCipher cipher handle                                                                 CNcomment:CIPHER����� CNend
\param[in] pstDataPkg data package ready for cipher                                              CNcomment:�����ܵ����ݰ��� CNend
\param[in] u32DataPkgNum  number of package ready for cipher                                     CNcomment:�����ܵ����ݰ������� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 						 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                    	 CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time  CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 Cipher_DecryptMulti(HI_HANDLE hCipher, HI_TEE_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief used for AES CBC CTS mode decryption only.
CNcomment:\brief ��������AES CBC CTS����ģʽ�Ľ��ܡ� CNend
\attention \n
\param[in] hCipher cipher handle                                                                 CNcomment:CIPHER����� CNend
\param[in] u32SrcPhyAddr address of input data                                              	 CNcomment:�����ܵ����ݰ��ĵ�ַ�� CNend
\param[in] u32DestPhyAddr  address of output data                                     			 CNcomment:���ܽ���������ַ�� CNend
\param[in] au8IV  Initial vector for decryption                                     			 CNcomment:��ʼ�������� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 						 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                    	 CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time  CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 Cipher_Decrypt_AESCBCCTS(HI_HANDLE hCipherHandle, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength, const HI_U8 au8IV[16]);


/**
\brief Init the hash module, if other program is using the hash module, the API will return failure.
CNcomment:\brief ��ʼ��HASHģ�飬�����������������ʹ��HASHģ�飬����ʧ��״̬�� CNend

\attention \n
N/A

\param[in] pstHashAttr: The hash calculating structure input.                                      CNcomment:���ڼ���hash�Ľṹ����� CNend
\param[out] pHashHandle: The output hash handle.               									CNcomment:�����hash��� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 						CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                       								CNcomment: APIϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 Cipher_HashInit(HI_TEE_CIPHER_HASH_ATTS_S *pstHashAttr, HI_HANDLE *pHashHandle);

/**
\brief Calculate the hash, if the size of the data to be calculated is very big and the DDR ram is not enough, this API can calculate the data one block by one block.
CNcomment:\brief ����hashֵ�������Ҫ������������Ƚϴ󣬸ýӿڿ���ʵ��һ��blockһ��block�ļ��㣬�����������Ƚϴ������£��ڴ治������⡣ CNend

\attention \n
N/A

\param[in] hHashHandl:  Hash handle.                                  		CNcomment:Hash����� CNend
\param[in] pu8InputData:  The input data buffer.                  			CNcomment:�������ݻ��� CNend
\param[in] u32InputDataLen:  The input data length, attention: the block length input must be 4bytes aligned except the last block!			CNcomment:�������ݵĳ��ȡ���Ҫ�� �������ݿ�ĳ��ȱ�����4�ֽڶ��룬���һ��block�޴����ơ� CNend
\retval ::HI_SUCCESS  Call this API succussful. 						 	CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                       		CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 Cipher_HashUpdate(HI_HANDLE hHashHandle, HI_U8 *pu8InputData, HI_U32 u32InputDataLen);



/**
\brief Get the final hash value, after calculate all of the data, call this API to get the final hash value and close the handle.If there is some reason need to interupt the calculation, this API should also be call to close the handle.
CNcomment:��ȡhashֵ���ڼ��������е����ݺ󣬵�������ӿڻ�ȡ���յ�hashֵ���ýӿ�ͬʱ��ر�hash���������ڼ�������У���Ҫ�жϼ��㣬Ҳ������øýӿڹر�hash����� CNend

\attention \n
N/A

\param[in] hHashHandle:  Hash handle.                                  		CNcomment:Hash����� CNend
\param[out] pu8OutputHash:  The final output hash value.                   	CNcomment:�����hashֵ�� CNend

\retval ::HI_SUCCESS  Call this API succussful. 						 	CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                       		CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 Cipher_HashFinal(HI_HANDLE hHashHandle, HI_U8 *pu8OutputHash);

HI_S32 Cipher_HashHmacInit(HI_HANDLE hHashHandle, HI_U32 u32TotalLen);
HI_S32 Cipher_GetRandomNumber(HI_U32 *pu32Rnd, HI_U32 u32Timeout);
HI_S32 Cipher_GetIVOut(HI_HANDLE hCipher, HI_U8 *pu8IVOut);
HI_S32 cipher_drv_modInit(HI_VOID);

HI_S32 Cipher_Test(HI_U32 u32Cmd, HI_VOID *pu8Param, HI_U32 u32Size);

HI_S32 DRV_CIPHER_CalcRsa_SW(CIPHER_RSA_DATA_S *pCipherRsaData);
HI_S32 DRV_CIPHER_GenRsaKey_SW(CIPHER_RSA_KEY_S *pstRsaKey);
HI_S32 DRV_CIPHER_RSA_CallRMul(HI_U8*X, HI_U8* A, HI_U8* B, HI_U32 Xsize, HI_U32 Asize, HI_U32 Bsize);

HI_VOID * s_Cipher_RemapPhyAddr(HI_U32 u32PhyAddr, HI_U32 u32Size, HI_BOOL bIsSMMu, HI_BOOL bIsSec);
HI_S32 s_Cipher_UnmapPhyAddr(HI_U32 u32PhyAddr, HI_VOID* pu8SrcVirAddr, HI_BOOL bIsSMMu, HI_BOOL bIsSec);

HI_S32 TEE_CIPHER_Ioctl(HI_U32 cmd, HI_VOID *argp);

HI_S32 Cipher_CencDecrypt(CIPHER_CENC_DECRYPT_S *pstCencDecrypt);

HI_S32 Cipher_Rsa(CIPHER_RSA_DATA_S *pCipherRsaData);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_CIPHER_INTF_H__ */
