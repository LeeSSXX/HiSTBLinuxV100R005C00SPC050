/**
 * @file tee_crypto_api.h
 *
 * Copyright(C), 2013-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������TEE(Trusted Execution Environment������ִ�л���)�ӽ���API \n
 * Message Digest Functions \n
 * Symmetric Cipher Functions \n
 * MAC Functions \n
 * Authenticated Encryption Functions \n
 * Asymmetric Functions \n
 * Key Derivation Functions \n
 * Random Data Generation Function \n
*/
#ifndef TEE_CRYPTO_API_H
#define TEE_CRYPTO_API_H

#include "tee_internal_api.h"
#include "tee_mem_mgmt_api.h"

#ifndef NULL
#define NULL            ((void *)0)
#endif

/** @defgroup crypto ���ܺͽ���
* @ingroup TEE_API
*/

/**
 * @ingroup  crypto
 * �Ա���Ϊ��λ�������Կ����
 *
*/
#define TEE_MAX_KEY_SIZE_IN_BITS 2048

/**
 * @ingroup  crypto
 * RSA(��Կ�����㷨) ��Կ����
 *
*/
#define SW_RSA_KEYLEN 1024
#if 1
/**
 * @ingroup  crypto
 * ��Կ����ʱ����ṹ������ֽ�
 *
*/
#define TEE_DH_MAX_SIZE_OF_OTHER_INFO  64 /*bytes*/
#endif
/**
 * @ingroup  crypto
 *
 * TEE�ļӽ��ܲ���ö��
*/
enum __TEE_Operation_Constants {
	TEE_OPERATION_CIPHER = 0x1,         /**< �ӽ��� */
	TEE_OPERATION_MAC,                  /**< MAC */
	TEE_OPERATION_AE,                   /**< ��֤���� */
	TEE_OPERATION_DIGEST,               /**< ժҪ */
	TEE_OPERATION_ASYMMETRIC_CIPHER,    /**< �ǶԳƼӽ��� */
	TEE_OPERATION_ASYMMETRIC_SIGNATURE, /**< �ǶԳ�ǩ�� */
	TEE_OPERATION_ASYMMETRIC_VERIFY,    /**< �ǶԳ���֤ */
	TEE_OPERATION_KEY_DERIVATION        /**< ��Կ���� */
};

/**
 * @ingroup  crypto
 *
 * TEE�ļӽ����㷨ID,�����ԳƼӽ��ܡ��ǶԳƼӽ��ܡ�ժҪHMAC��\n
 * ע��:�Գ��㷨�е�nopadģʽ��ҪTA(Trusted Application������Ӧ��)�������
*/
enum __TEE_CRYPTO_ALGORITHM_ID {
	TEE_ALG_INVALID = 0x0,                              /**< ��ЧID */
	TEE_ALG_AES_ECB_NOPAD =  0x10000010,                /**< AES_ECB_NOPAD */
	TEE_ALG_AES_CBC_NOPAD = 0x10000110,                 /**< AES_CBC_NOPAD */
	TEE_ALG_AES_CTR = 0x10000210,                       /**< AES_CTR */
	TEE_ALG_AES_CTS = 0x10000310,                       /**< AES_CTS */
	TEE_ALG_AES_XTS = 0x10000410,                       /**< AES_XTS */
	TEE_ALG_AES_CBC_MAC_NOPAD = 0x30000110,             /**< AES_CBC_MAC_NOPAD */
	TEE_ALG_AES_CBC_MAC_PKCS5 = 0x30000510,             /**< AES_CBC_MAC_PKCS5 */
	TEE_ALG_AES_CMAC = 0x30000610,                      /**< AES_CMAC */
	TEE_ALG_AES_CCM = 0x40000710,                       /**< AES_CCM */
	TEE_ALG_AES_GCM = 0x40000810,                       /**< AES_GCM */
	TEE_ALG_DES_ECB_NOPAD = 0x10000011,                 /**< DES_ECB_NOPAD */
	TEE_ALG_DES_CBC_NOPAD = 0x10000111,                 /**< DES_CBC_NOPAD */
	TEE_ALG_DES_CBC_MAC_NOPAD = 0x30000111,             /**< DES_CBC_MAC_NOPAD */
	TEE_ALG_DES_CBC_MAC_PKCS5 = 0x30000511,             /**< DES_CBC_MAC_PKCS5 */
	TEE_ALG_DES3_ECB_NOPAD = 0x10000013,                /**< DES3_ECB_NOPAD */
	TEE_ALG_DES3_CBC_NOPAD = 0x10000113,                /**< DES3_CBC_NOPAD */
	TEE_ALG_DES3_CBC_MAC_NOPAD = 0x30000113,            /**< DES3_CBC_MAC_NOPAD */
	TEE_ALG_DES3_CBC_MAC_PKCS5 = 0x30000513,            /**< DES3_CBC_MAC_PKCS5 */
	TEE_ALG_RSASSA_PKCS1_V1_5_MD5 = 0x70001830,         /**< RSASSA_PKCS1_V1_5_MD5 */
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA1 = 0x70002830,        /**< RSASSA_PKCS1_V1_5_SHA1 */
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA224 = 0x70003830,      /**< RSASSA_PKCS1_V1_5_SHA224 */
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA256 = 0x70004830,      /**< RSASSA_PKCS1_V1_5_SHA256 */
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA384 = 0x70005830,      /**< RSASSA_PKCS1_V1_5_SHA384 */
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA512 = 0x70006830,      /**< RSASSA_PKCS1_V1_5_SHA512 */
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1 = 0x70212930,    /**< RSASSA_PKCS1_PSS_MGF1_SHA1 */
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224 = 0x70313930,  /**< RSASSA_PKCS1_PSS_MGF1_SHA224 */
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256 = 0x70414930,  /**< RSASSA_PKCS1_PSS_MGF1_SHA256 */
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384 = 0x70515930,  /**< RSASSA_PKCS1_PSS_MGF1_SHA384 */
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512 = 0x70616930,  /**< RSASSA_PKCS1_PSS_MGF1_SHA512 */
	TEE_ALG_RSAES_PKCS1_V1_5 = 0x60000130,              /**< RSAES_PKCS1_V1_5 */
	TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1 = 0x60210230,    /**< RSAES_PKCS1_OAEP_MGF1_SHA1 */
	TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224 = 0x60211230,  /**< RSAES_PKCS1_OAEP_MGF1_SHA224 */
	TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256 = 0x60212230,  /**< RSAES_PKCS1_OAEP_MGF1_SHA256 */
	TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384 = 0x60213230,  /**< RSAES_PKCS1_OAEP_MGF1_SHA384 */
	TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512 = 0x60214230,  /**< RSAES_PKCS1_OAEP_MGF1_SHA512 */
	TEE_ALG_RSA_NOPAD = 0x60000030,                     /**< RSA_NOPAD */
	TEE_ALG_DSA_SHA1 = 0x70002131,                      /**< DSA_SHA1 */
	TEE_ALG_DH_DERIVE_SHARED_SECRET = 0x80000032,       /**< DH_DERIVE_SHARED_SECRET */
	TEE_ALG_MD5 = 0x50000001,                           /**< MD5 */
	TEE_ALG_SHA1 = 0x50000002,                          /**< SHA1 */
	TEE_ALG_SHA224 = 0x50000003,                        /**< SHA224 */
	TEE_ALG_SHA256 = 0x50000004,                        /**< SHA256 */
	TEE_ALG_SHA384 = 0x50000005,                        /**< SHA384 */
	TEE_ALG_SHA512 = 0x50000006,                        /**< SHA512 */
	TEE_ALG_HMAC_MD5 = 0x30000001,                      /**< HMAC_MD5 */
	TEE_ALG_HMAC_SHA1 = 0x30000002,                     /**< HMAC_SHA1 */
	TEE_ALG_HMAC_SHA224 = 0x30000003,                   /**< HMAC_SHA224 */
	TEE_ALG_HMAC_SHA256 = 0x30000004,                   /**< HMAC_SHA256 */
	TEE_ALG_HMAC_SHA384 = 0x30000005,                   /**< HMAC_SHA384 */
	TEE_ALG_HMAC_SHA512 = 0x30000006,                   /**< HMAC_SHA512 */
	TEE_ALG_AES_ECB_PKCS5 = 0x10000020,                 /**< AES_ECB_PKCS5 */
	TEE_ALG_AES_CBC_PKCS5 = 0x10000220,                 /**< AES_CBC_PKCS5 */
	TEE_ALG_ECC = 0x80000001,
} ;
/**
 * @ingroup  crypto
 *
 * TEE�ļӽ����㷨ID����
*/
typedef enum __TEE_CRYPTO_ALGORITHM_ID TEE_CRYPTO_ALGORITHM_ID;
#if 1
/**
 * @ingroup  crypto
 *
 * TEE����Կ����HASHģʽ
*/
typedef enum {
	TEE_DH_HASH_SHA1_mode = 0,              /**< HASH_SHA1 */
	TEE_DH_HASH_SHA224_mode = 1,            /**< HASH_SHA224 */
	TEE_DH_HASH_SHA256_mode = 2,            /**< HASH_SHA256 */
	TEE_DH_HASH_SHA384_mode = 3,            /**< HASH_SHA384 */
	TEE_DH_HASH_SHA512_mode = 4,            /**< HASH_SHA512 */
	TEE_DH_HASH_NumOfModes,                 /**< num of modes */
} TEE_DH_HASH_Mode;

/**
 * @ingroup  crypto
 *
 * TEE����Կ����DHЭ��
*/
typedef enum {
	TEE_DH_PKCS3_mode  = 0,              /**< PKCS3 */
	TEE_DH_ANSI_X942_mode = 1,           /**< X942 */
	TEE_DH_NumOfModes,                   /**< num of modes */
} TEE_DH_OpMode_t;

/**
 * @ingroup  crypto
 *
 * TEE����Կ��������ģʽ
*/
typedef enum {
	TEE_DH_ASN1_DerivMode = 0,                          /**< ASN1_DerivMode */
	TEE_DH_ConcatDerivMode = 1,                         /**< ConcatDerivMode */
	TEE_DH_X963_DerivMode = TEE_DH_ConcatDerivMode,     /**< X963_DerivMode */
	TEE_DH_OMADRM_DerivMode = 2,                        /**< OMADRM_DerivMode */
	TEE_DH_ISO18033_KDF1_DerivMode = 3,                 /**< ISO18033_KDF1_DerivMode */
	TEE_DH_ISO18033_KDF2_DerivMode = 4,                 /**< ISO18033_KDF2_DerivMode */
	TEE_DH_DerivFunc_NumOfModes,                        /**< num of modes */
} TEE_DH_DerivFuncMode;
#endif

/**
 * @ingroup  crypto
 *
 * TEE����Կ��������
*/
enum __TEE_DK_ObjectAttribute {
	TEE_DK_SECRECT = 0,         /**< A pointer to shared secret value */
	TEE_DK_OTHER,               /**< A pointer to structure containing other data */
	TEE_DK_HASH_MODE,           /**< The enumerator ID of the HASH function to be used */
	TEE_DK_DERIVATION_MODE      /**< The enumerator ID of the derivation function mode */
};
/**
 * @ingroup  crypto
 *
 * TEE����Կ������������
*/
typedef enum __TEE_DK_ObjectAttribute TEE_DK_ObjectAttribute;
/**
 * @ingroup  crypto
 *
 * TEE�ļӽ��ܲ���ģʽ
*/
enum __TEE_OperationMode {
	TEE_MODE_ENCRYPT = 0x0, /**< ���� */
	TEE_MODE_DECRYPT,       /**< ���� */
	TEE_MODE_SIGN,          /**< ǩ�� */
	TEE_MODE_VERIFY,        /**< ��֤ */
	TEE_MODE_MAC,           /**< MAC */
	TEE_MODE_DIGEST,        /**< ժҪ */
	TEE_MODE_DERIVE        /**< ���� */
} ;

/**
 * @ingroup  crypto
 *
 * TEE�ļӽ��ܲ���ģʽ����
*/
typedef enum __TEE_OperationMode TEE_OperationMode;
#if 1
/**
 * @ingroup  crypto
 *
 * TEE��Կ������other���ݽṹ, ��������Կ������һЩ��ѡ���ݣ�����Ҫ����NULL
 */
typedef struct {
	uint8_t    AlgorithmID[TEE_DH_MAX_SIZE_OF_OTHER_INFO];  /**< objectΨһ��ʾ(OID) */
	uint32_t   SizeOfAlgorithmID;                           /**< AlgorithmID�ĳ��� */
	uint8_t    PartyUInfo[TEE_DH_MAX_SIZE_OF_OTHER_INFO];   /**< ���ͷ���public��Ϣ */
	uint32_t   SizeOfPartyUInfo;                            /**< PartyUInfo�ĳ��� */
	uint8_t    PartyVInfo[TEE_DH_MAX_SIZE_OF_OTHER_INFO];   /**< ���շ���public��Ϣ */
	uint32_t   SizeOfPartyVInfo;                            /**< PartyVInfo���� */
	uint8_t    SuppPrivInfo[TEE_DH_MAX_SIZE_OF_OTHER_INFO]; /**< ˫�����е�private��Ϣ */
	uint32_t   SizeOfSuppPrivInfo;                          /**< SuppPrivInfo�ĳ��� */
	uint8_t    SuppPubInfo[TEE_DH_MAX_SIZE_OF_OTHER_INFO];  /**< ˫�����е�public��Ϣ */
	uint32_t   SizeOfSuppPubInfo;                           /**< SuppPubInfo���� */
} TEE_DH_OtherInfo;
#endif
/**
 * @ingroup  crypto
 *
 * TEE�ļӽ��ܲ�����Ϣ������#TEE_GetOperationInfo()
*/
struct __TEE_OperationInfo {
	uint32_t algorithm;         /**< #__TEE_CRYPTO_ALGORITHM_ID �㷨 */
	uint32_t operationClass;    /**< #__TEE_Operation_Constants �ӽ��ܲ��� */
	uint32_t mode;              /**< #__TEE_OperationMode �ӽ��ܲ���ģʽ */
	uint32_t digestLength;      /**< ժҪ���� */
	uint32_t maxKeySize;        /**< ��Կ��󳤶� */
	uint32_t keySize;           /**< ��Կ���� */
	uint32_t requiredKeyUsage;  /**< �Ƿ���Ҫ��Կ */
	uint32_t handleState;       /**< ����״̬ */
	void *keyValue;             /**< ��Կָ�� */
} ;
/**
 * @ingroup  crypto
 *
 * TEE�ļӽ��ܲ�����Ϣ����
*/
typedef struct __TEE_OperationInfo TEE_OperationInfo;

/**
 * @ingroup  crypto
 *
 * TEE�ļӽ���ȫ�־��
*/
struct __TEE_OperationHandle {
	uint32_t algorithm;         /**< #__TEE_CRYPTO_ALGORITHM_ID �㷨 */
	uint32_t operationClass;    /**< #__TEE_Operation_Constants �ӽ��ܲ��� */
	uint32_t mode;              /**< #__TEE_OperationMode �ӽ��ܲ���ģʽ */
	uint32_t digestLength;      /**< ժҪ���� */
	uint32_t maxKeySize;        /**< ��Կ��󳤶� */
	uint32_t keySize;           /**< ��Կ���� */
	uint32_t keySize2;          /**< ��Կ2���� */
	uint32_t requiredKeyUsage;  /**< �Ƿ���Ҫ��Կ */
	uint32_t handleState;       /**< ����״̬ */
	void *keyValue;             /**< ��Կbuffer */
	void *keyValue2;            /**< ��Կ2buffer */
	void *crypto_ctxt;          /**< DX�ӽ��ܽṹ���� */
	void *hmac_rest_ctext;      /**< DX hmac ��λ��� */
	void *IV;                   /**< ��ʼ������ָ�� */
	void *publicKey;            /**< ��Կָ�룬�ǶԳƼӽ���ʹ�� */
	uint32_t publicKeyLen;        /**< ��Կ�ܳ���*/
	void *privateKey;           /**< ˽Կָ�룬�ǶԳƼӽ���ʹ�� */
	uint32_t privateKeyLen;        /**< ˽Կ�ܳ���*/
	uint32_t IVLen;             /**< ��ʼ���������� */
	//start of DH
	TEE_DH_OtherInfo *dh_otherinfo;         /**< #TEE_DH_OtherInfo DH other���ݽṹ */
	uint32_t dh_hash_mode;          /**< #TEE_DH_HASH_Mode DH hashģʽ */
	uint32_t dh_derive_func;    /**< #TEE_DH_DerivFuncMode DH�������� */
	uint32_t dh_op_mode;             /**< #TEE_DH_OpMode_t DHЭ�� */
	//end of DH
};

/**
 * @ingroup  crypto
 *
 * �ǶԳƼӽ��ܹ�Կ�ṹ������ֵ
 */
#define RSA_PUBKEY_MAXSIZE sizeof(CRYS_RSAUserPubKey_t)

/**
 * @ingroup  crypto
 *
 * �ǶԳƼӽ���˽Կ�ṹ������ֵ
 */
#define RSA_PRIVKEY_MAXSIZE sizeof(CRYS_RSAUserPrivKey_t)

/**
 * @ingroup  crypto
 *
 * TEE�ļӽ���ȫ�־��ָ������
*/
typedef struct __TEE_OperationHandle *TEE_OperationHandle;
/**
 * @ingroup  crypto
 *
 * TEE�ļӽ���ȫ�־������
*/
typedef struct __TEE_OperationHandle TEE_OperationHandleVar;
/**
 * @ingroup  crypto
 *
 * TEE��object�������
*/
typedef struct __TEE_ObjectHandle TEE_ObjectHandleVar;

/**
 * @ingroup  crypto
 * @brief TEE_AllocateOperation ����ӽ�����Դ
 *
 * @par ����:
 * malloc #TEE_OperationHandle��Ϊ��Ҫ���е��㷨������Դ����ʼ�����ֱ���
 *
 * @attention ��ΪժҪ������Դ��ʱ�����˳�ʼ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param algorithm [IN]  �ӽ����㷨 #TEE_CRYPTO_ALGORITHM_ID
 * @param mode [IN]  �ӽ���ģʽ #TEE_OperationMode
 * @param maxKeySize [IN]  �����Կ���� 64bytes
 *
 * @retval #TEE_SUCCESS ��ȫ�����ӽ�����Դ�ɹ�
 * @retval #TEE_ERROR_OUT_OF_MEMORY #TEE_OperationHandle mallocʧ��
 * @retval #TEE_ERROR_NOT_SUPPORTED �㷨��ʽ��֧��
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_FreeOperation
 * @since V100R002C00B302
*/
TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
				 uint32_t algorithm, uint32_t mode, uint32_t maxKeySize);
/**
 * @ingroup  crypto
 * @brief TEE_FreeOperation �ͷżӽ�����Դ
 *
 * @par ����:
 * free TEE_OperationHandle���ͷżӽ�����ص���Դ
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ����
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AllocateOperation
 * @since V100R002C00B302
*/
void TEE_FreeOperation(TEE_OperationHandle operation);
/**
 * @ingroup  crypto
 * @brief TEE_GetOperationInfo ��ȡ�ӽ�����Ϣ
 *
 * @par ����:
 * ��ȡģ�����ļӽ�����Ϣ�����浽operationInfo��
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param operationInfo [IN/OUT]  �ӽ�����Ϣ #TEE_OperationInfo
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see ��
 * @since V100R002C00B302
*/
void TEE_GetOperationInfo(TEE_OperationHandle operation,
			  TEE_OperationInfo *operationInfo);
/**
 * ingroup  crypto
 * @brief ���üӽ���ģ����
 *
 * @par ����:
 * ���üӽ���ģ����
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see ��
 * @since V100R002C00B302
*/
void TEE_ResetOperation(TEE_OperationHandle operation);
/**
 * @ingroup  crypto
 * @brief ���üӽ���ģ����Կ
 *
 * @par ����:
 * ���üӽ���ģ����Կ
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param key [IN/OUT]  ��Կ��� #TEE_ObjectHandle
 *
 * @retval #TEE_SUCCESS ������Կ�ɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS  ����ΪNULL�����߲�������
 * @retval #TEE_ERROR_OUT_OF_MEMORY  ��Կbuffer����ʧ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_SetOperationKey2
 * @since V100R002C00B302
*/
TEE_Result TEE_SetOperationKey(TEE_OperationHandle operation, TEE_ObjectHandle key);
/**
 * ingroup  crypto
 * @brief ���üӽ���ģ����Կ1����Կ2
 *
 * @par ����:
 * ���üӽ���ģ����Կ1����Կ2
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param key1 [IN/OUT]  ��Կ1��� #TEE_ObjectHandle
 * @param key2 [IN/OUT]  ��Կ2��� #TEE_ObjectHandle
 *
 * @retval #TEE_SUCCESS ����2����Կ�ɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS  ����ΪNULL�����߲�������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_SetOperationKey
 * @since V100R002C00B302
*/
TEE_Result TEE_SetOperationKey2(TEE_OperationHandle operation, TEE_ObjectHandle key1, TEE_ObjectHandle key2);
/**
 * @ingroup  crypto
 * @brief �����ӽ���ģ��������һ�����
 *
 * @par ����:
 * �����ӽ���ģ��������һ�����
 *
 * @attention ��
 * @param dstOperation [IN/OUT]  Ŀ��ģ���� #TEE_OperationHandle
 * @param srcOperation [IN/OUT]  Դģ���� #TEE_OperationHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see ��
 * @since V100R002C00B302
*/
void TEE_CopyOperation(TEE_OperationHandle dstOperation, TEE_OperationHandle srcOperation);
/**
 * @ingroup  crypto
 * @brief �ӽ��ܳ�ʼ��
 *
 * @par ����:
 * �ӽ��ܳ�ʼ��������AES��DES��ʼ��
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param IV [IN]  ��ʼ������ָ�룬�������Ҫ��ʼ�������Ļ���ΪNULL
 * @param IVLen [IN]  ��ʼ���������ȣ�
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_CipherUpdate | TEE_CipherDoFinal
 * @since V100R002C00B302
*/
void TEE_CipherInit(TEE_OperationHandle operation, void *IV, size_t IVLen);
/**
 * @ingroup  crypto
 * @brief �ӽ�������
 *
 * @par ����:
 * �ӽ������㣬����AES��DES����
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param srcData [IN]  �������/��������ָ��
 * @param srcLen [IN]  �������/�������ݳ��� AES���ݳ���Ϊ16�ֽڱ�����DES���ݳ���Ϊ8�ֽڱ���
 * @param destData [OUT] �������/��������ָ��
 * @param destLen [OUT] �������/�������ݳ���
 *
 * @retval #TEE_SUCCESS �ӽ�������ɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����ΪNULL���������
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_CipherInit | TEE_CipherDoFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_CipherUpdate(TEE_OperationHandle operation,
			    void *srcData, size_t srcLen, void *destData, size_t *destLen);
/**
 * @ingroup  crypto
 * @brief �ӽ����������
 *
 * @par ����:
 * �ӽ������������ֻ��AES��Ҫִ�д˽ӿڣ�DES����Ҫ
 *
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param srcData [IN]  �������/��������ָ��
 * @param srcLen [IN]  �������/�������ݳ��� AES���ݳ���Ϊ16�ֽڱ�����DES���ݳ���Ϊ8�ֽڱ���
 * @param destData [OUT] �������/��������ָ��
 * @param destLen [OUT] �������/�������ݳ���
 *
 * @retval #TEE_SUCCESS �ӽ��ܽ����ɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����ΪNULL���������
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_CipherInit | TEE_CipherUpdate
 * @since V100R002C00B302
*/
TEE_Result TEE_CipherDoFinal(TEE_OperationHandle operation,
			     void *srcData, size_t srcLen, void *destData, size_t *destLen);
/**
 * @ingroup  crypto
 * @brief ժҪ����
 *
 * @par ����:
 * ժҪ���㣬֧��SHA1��SHA224��SHA256��SHA384��SHA512
 *
 * @attention ��֧��MD5
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param chunk [IN]  ��������ָ��
 * @param chunkSize [IN]  �������ݳ���
 *
 * @retval #TEE_SUCCESS ժҪ����ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_DigestDoFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_DigestUpdate(TEE_OperationHandle operation, void *chunk, size_t chunkSize);
/**
 * @ingroup  crypto
 * @brief ժҪ�������
 *
 * @par ����:
 * ժҪ���������֧��SHA1��SHA224��SHA256��SHA384��SHA512
 *
 * @attention ��֧��MD5
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param chunk [IN]  ��������ָ��
 * @param chunkSize [IN]  �������ݳ���
 * @param hash [OUT]  ���ժҪ����ָ��
 * @param hashLen [OUT]  ���ժҪ���ݳ��� �̶����������㷨�й�
 *
 * @retval #TEE_SUCCESS ժҪ�����ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_DigestUpdate
 * @since V100R002C00B302
*/
TEE_Result TEE_DigestDoFinal(TEE_OperationHandle operation,
			     void *chunk, size_t chunkLen, void *hash, size_t *hashLen);
/**
 * @ingroup  crypto
 * @brief MAC��ʼ��
 *
 * @par ����:
 * MAC��ʼ����֧��SHA1��SHA224��SHA256��SHA384��SHA512
 *
 * @attention ��֧��MD5
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param IV [IN]  ��ʼ������ָ��
 * @param IVLen [IN]  ��ʼ����������
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_MACUpdate | TEE_MACComputeFinal | TEE_MACCompareFinal
 * @since V100R002C00B302
*/
void TEE_MACInit(TEE_OperationHandle operation, void *IV, size_t IVLen);
/**
 * @ingroup  crypto
 * @brief MAC����
 *
 * @par ����:
 * MAC���㣬֧��SHA1��SHA224��SHA256��SHA384��SHA512
 *
 * @attention ��֧��MD5
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param chunk [IN]  ��������ָ��
 * @param chunkSize [IN]  �������ݳ���
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_MACInit | TEE_MACComputeFinal | TEE_MACCompareFinal
 * @since V100R002C00B302
*/
void TEE_MACUpdate(TEE_OperationHandle operation, void *chunk, size_t chunkSize);
/**
 * @ingroup  crypto
 * @brief MAC�������
 *
 * @par ����:
 * MAC���������֧��SHA1��SHA224��SHA256��SHA384��SHA512
 *
 * @attention ��֧��MD5
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param message [IN]  ����Ƚ�����ָ��
 * @param messageLen [IN]  ����Ƚ����ݳ���
 * @param mac [OUT] ���ժҪ����ָ��
 * @param macLen [OUT] ���ժҪ���ݳ���
 *
 * @retval #TEE_SUCCESS MAC�����ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_MACInit | TEE_MACUpdate | TEE_MACCompareFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_MACComputeFinal(TEE_OperationHandle operation,
			       void *message, size_t messageLen, void *mac, size_t *macLen);
/**
 * @ingroup  crypto
 * @brief MAC�������������������������ݱȽ�
 *
 * @par ����:
 * MAC���������֧��SHA1��SHA224��SHA256��SHA384��SHA512
 *
 * @attention ��֧��MD5
 * @param operation [IN/OUT]  ģ����
 * @param message [IN]  ����Ƚ�����ָ��
 * @param messageLen [IN]  ����Ƚ����ݳ���
 * @param mac [IN] ���ժҪ����ָ��
 * @param macLen [IN] ���ժҪ���ݳ���
 *
 * @retval #TEE_SUCCESS MAC��֤�ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 * @retval #TEE_ERROR_MAC_INVALID У��ʧ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_MACInit | TEE_MACUpdate | TEE_MACComputeFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_MACCompareFinal(TEE_OperationHandle operation,
			       void *message, size_t messageLen, void *mac, size_t *macLen);
/**
 * @ingroup  crypto
 * @brief ���ݴ������������Թ�˽Կ
 *
 * @par ����:
 * ���ݴ������������Թ�˽Կ�����ں��ڵ���Կ�������ýӿ�TEEδ�ṩ�������Զ���ӿ�
 *
 * @attention �������н����뿪���ĵ�
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param params [IN]  ��������ָ�� #TEE_Attribute
 * @param paramCount [IN]  �������ݸ���
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see ��
 * @since V100R002C00B302
*/
void GeneratePubPrv(TEE_OperationHandle operation, TEE_Attribute *params, uint32_t paramCount);
/**
 * @ingroup  crypto
 * @brief ���ݴ������������Կ
 *
 * @par ����:
 * ���ݴ������������Կ���������Ϊshared secret��������Կ����rootkey�����ȹ̶�Ϊ128bits
 *
 * @attention ��
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param params [IN]  �����������ָ�� #TEE_Attribute
 * @param paramCount [IN]  ����������ݸ���
 * @param derivedKey [OUT] ������Կ��� #TEE_ObjectHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see ��
 * @since V100R002C00B302
*/
void TEE_DeriveKey(TEE_OperationHandle operation, TEE_Attribute *params,
		   uint32_t paramCount, TEE_ObjectHandle derivedKey);
/**
 * @ingroup  crypto
 * @brief ���������
 *
 * @par ����:
 * ���������
 *
 * @attention ��
 * @param randomBuffer [IN/OUT]  �����ָ��
 * @param randomBufferLen [IN]  ���������
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see ��
 * @since V100R002C00B302
*/
void TEE_GenerateRandom(void *randomBuffer, size_t randomBufferLen);
/**
 * @ingroup  crypto
 * @brief ��֤�ӽ��ܳ�ʼ��
 *
 * @par ����:
 * ��֤�ӽ��ܳ�ʼ����ֻ֧��AES-CCM
 *
 * @attention ��֧��AES-GCM
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param nonce [IN]  �������������ָ��
 * @param nonceLen [IN]  ������������ݳ��� [7,8,9,10,11,12,13]
 * @param tagLen [IN] ��λ��ʾ��ժҪ���ݳ���[128, 112, 96, 64, 48, 32]
 * @param AADLen [IN] ������֤���ݳ���
 * @param payloadLen [IN] �ӽ������ݳ���
 *
 * @retval #TEE_SUCCESS AE(Authenticated Encryption)��ʼ���ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AEUpdateAAD | TEE_AEUpdate | TEE_AEEncryptFinal | TEE_AEDecryptFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_AEInit(TEE_OperationHandle operation,
		      void *nonce, size_t nonceLen, uint32_t tagLen,
		      uint32_t AADLen, uint32_t payloadLen);
/**
 * @ingroup  crypto
 * @brief ��֤�ӽ���AAD����
 *
 * @par ����:
 * ��֤�ӽ���AAD���㣬ֻ֧��AES-CCM
 *
 * @attention ��֧��AES-GCM
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param AADdata [IN]  ���븽����֤����ָ��
 * @param AADdataLen [IN]  ���븽����֤���ݳ���
 *
 * @retval ��
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AEInit | TEE_AEUpdate | TEE_AEEncryptFinal | TEE_AEDecryptFinal
 * @since V100R002C00B302
*/
void TEE_AEUpdateAAD(TEE_OperationHandle operation,
		     void *AADdata, size_t AADdataLen);
/**
 * ingroup  crypto
 * @brief ��֤�ӽ�������
 *
 * @par ����:
 * ��֤�ӽ������㣬ֻ֧��AES-CCM
 *
 * @attention ��֧��AES-GCM
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param srcData [IN]  �������/��������ָ��
 * @param srcLen [IN]  �������/�������ݳ���
 * @param destData [OUT] �������/��������ָ��
 * @param destLen [OUT] �������/�������ݳ���
 *
 * @retval #TEE_SUCCESS AE(Authenticated Encryption)�ӽ�������ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AEUpdateAAD | TEE_AEUpdateAAD | TEE_AEEncryptFinal | TEE_AEDecryptFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_AEUpdate(TEE_OperationHandle operation, void *srcData,
			size_t srcLen, void *destData, size_t *destLen);
/**
 * @ingroup  crypto
 * @brief ��֤�������������������tag
 *
 * @par ����:
 * ��֤�������������������tag��ֻ֧��AES-CCM
 *
 * @attention ��֧��AES-GCM
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param srcData [IN]  �������/��������ָ��
 * @param srcLen [IN]  �������/�������ݳ���
 * @param destData [OUT] �������/��������ָ��
 * @param destLen [OUT] �������/�������ݳ���
 * @param tag [OUT] У������ָ��
 * @param tagLen [OUT] У�����ݳ���
 *
 * @retval #TEE_SUCCESS AE(Authenticated Encryption)�����������
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AEUpdateAAD | TEE_AEUpdateAAD | TEE_AEUpdate | TEE_AEDecryptFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
			      void *srcData, size_t srcLen, void *destData, size_t *destLen,
			      void *tag, size_t *tagLen);
/**
 * @ingroup  crypto
 * @brief ��֤�������������������tag
 *
 * @par ����:
 * ��֤�������������������tag��ֻ֧��AES-CCM
 *
 * @attention ��֧��AES-GCM
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param srcData [IN]  �������/��������ָ��
 * @param srcLen [IN]  �������/�������ݳ���
 * @param destData [OUT] �������/��������ָ��
 * @param destLen [OUT] �������/�������ݳ���
 * @param tag [IN] У������ָ��
 * @param tagLen [IN] У�����ݳ���
 *
 * @retval #TEE_SUCCESS AE(Authenticated Encryption)�����������
 * @retval #TEE_ERROR_MAC_INVALID У�鷵�ش���
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AEUpdateAAD | TEE_AEUpdateAAD | TEE_AEUpdate | TEE_AEEncryptFinal
 * @since V100R002C00B302
*/
TEE_Result TEE_AEDecryptFinal(TEE_OperationHandle operation, void *srcData,
			      size_t srcLen, void *destData, size_t *destLen,
			      void *tag, size_t tagLen);
/**
 * @ingroup  crypto
 * @brief �ǶԳ����ݼ���
 *
 * @par ����:
 * �ǶԳ����ݼ��ܣ�ִֻ��һ�Ρ�
 *
 * @attention �ǶԳƼ���ֻ��ʹ�ù�Կ��˽Կ���ܻ᷵�ش���
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param params [IN] ����ָ�� #TEE_Attribute
 * @param paramCount [IN] ��������
 * @param srcData [IN]  �����������ָ��
 * @param srcLen [IN]  ����������ݳ���
 * @param destData [OUT] �����������ָ��
 * @param destLen [OUT] ����������ݳ���
 *
 * @retval #TEE_SUCCESS �ǶԳƼ��ܳɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 * @retval #TEE_ERROR_BAD_PARAMETERS ����ΪNULL���������
 * @retval #TEE_ERROR_NO_DATA ��ԿΪ�գ�û������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AsymmetricDecrypt
 * @since V100R002C00B302
*/
TEE_Result TEE_AsymmetricEncrypt(TEE_OperationHandle operation,
				 TEE_Attribute *params, uint32_t paramCount, void *srcData,
				 size_t srcLen, void *destData, size_t *destLen);
/**
 * @ingroup  crypto
 * @brief �ǶԳ����ݽ���
 *
 * @par ����:
 * �ǶԳ����ݽ��ܣ�ִֻ��һ�Ρ�
 *
 * @attention �ǶԳƽ���ֻ��ʹ��˽Կ����Կ���ܻ᷵�ش���
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param params [IN] ����ָ�� #TEE_Attribute
 * @param paramCount [IN] ��������
 * @param srcData [IN]  �����������ָ��
 * @param srcLen [IN]  ����������ݳ���
 * @param destData [OUT] �����������ָ��
 * @param destLen [OUT] ����������ݳ���
 *
 * @retval #TEE_SUCCESS �ǶԳƼ��ܳɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 * @retval #TEE_ERROR_BAD_PARAMETERS ����ΪNULL���������
 * @retval #TEE_ERROR_NO_DATA ˽ԿΪ�գ�û������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AsymmetricEncrypt
 * @since V100R002C00B302
*/
TEE_Result TEE_AsymmetricDecrypt(TEE_OperationHandle operation,
				 TEE_Attribute *params, uint32_t paramCount, void *srcData,
				 size_t srcLen, void *destData, size_t *destLen);
/**
 * @ingroup  crypto
 * @brief �ǶԳ�����ǩ��ժҪ
 *
 * @par ����:
 * �ǶԳ�����ǩ��ժҪ
 *
 * @attention ֻ��ʹ��˽Կ����ǩ������Կǩ�����ش���
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param params [IN] ����ָ�� #TEE_Attribute
 * @param paramCount [IN] ��������
 * @param digest [IN]  ժҪ����ָ��
 * @param digestLen [IN]  ժҪ���ݳ���
 * @param signature [OUT] ǩ������ָ��
 * @param signatureLen [OUT] ǩ�����ݳ���
 *
 * @retval #TEE_SUCCESS �ǶԳ�ǩ���ɹ�
 * @retval #TEE_ERROR_GENERIC Ӳ���ӽ��ܵײ���������
 * @retval #TEE_ERROR_BAD_PARAMETERS ����ΪNULL���������
 * @retval #TEE_ERROR_NO_DATA ˽ԿΪ�գ�û������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AsymmetricVerifyDigest
 * @since V100R002C00B302
*/
TEE_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
				    TEE_Attribute *params, uint32_t paramCount, void *digest,
				    size_t digestLen, void *signature, size_t *signatureLen);
/**
 * @ingroup  crypto
 * @brief �ǶԳ�������֤ժҪ
 *
 * @par ����:
 * �ǶԳ�������֤ժҪ
 *
 * @attention ֻ��ʹ�ù�Կ������֤��˽Կ��֤���ش���
 * @param operation [IN/OUT]  ģ���� #TEE_OperationHandle
 * @param params [IN] ����ָ�� #TEE_Attribute
 * @param paramCount [IN] ��������
 * @param digest [IN]  ժҪ����ָ��
 * @param digestLen [IN]  ժҪ���ݳ���
 * @param signature [IN] ǩ������ָ��
 * @param signatureLen [IN] ǩ�����ݳ���
 *
 * @retval #TEE_SUCCESS �ǶԳ���֤�ɹ�
 * @retval #TEE_ERROR_GENERIC ����Ӳ���������ش���
 * @retval #TEE_ERROR_BAD_PARAMETERS ����ΪNULL���������
 * @retval #TEE_ERROR_NO_DATA ��ԿΪ�գ�û������
 *
 * @par ����:
 * @li crypto �ӽ���ģ��
 * @li tee_crypto_api.h �ӽ���APIͷ�ļ�
 * @see TEE_AsymmetricSignDigest
 * @since V100R002C00B302
*/
TEE_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
				      TEE_Attribute *params, uint32_t paramCount, void *digest,
				      size_t digestLen, void *signature, size_t signatureLen);
#endif

/**
* History: \n
* 2013-04-11��sdk��Create \n
*
* vi: set expandtab ts=4 sw=4 tw=80:
*/
