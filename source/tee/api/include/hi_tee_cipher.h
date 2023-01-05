/******************************************************************************

Copyright (C), 2015-2016, Hisilicon Tech. Co., Ltd.

******************************************************************************
File Name : hi_tee_cipher.h
Version : Initial Draft
Author : Hisilicon multimedia software group
Created : 2016/06/08
Description :
History :
1.Date : 2016/06/08
Author : sdk
Modification: Created file

*******************************************************************************/
/**
* \file
* \brief Describes the API about the cipher.
CNcomment:�ṩCIPHER API CNend
*/

#ifndef __HI_TEE_CIPHER_H__
#define __HI_TEE_CIPHER_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/*************************** Structure Definition ****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */

/** max length of SM2, unit: word */
/** CNcomment: SM2���ݳ��ȣ���λword */
#define SM2_LEN_IN_WROD                   (8)

/** max length of SM2, unit: byte */
/** CNcomment: SM2���ݳ��ȣ���λbyte */
#define SM2_LEN_IN_BYTE                   (SM2_LEN_IN_WROD * 4)

/** CIPHER set IV for first package */
/** CNcomment: CIPHER ֻ����һ�����ݰ���IV */
#define CIPHER_IV_CHANGE_ONE_PKG        (1)

/** CIPHER set IV for first package */
/** CNcomment: CIPHER �����������ݰ���IV */
#define CIPHER_IV_CHANGE_ALL_PKG        (2)

/** Cipher work mode */
/** CNcomment:CIPHER����ģʽ */
typedef enum hiHI_TEE_CIPHER_WORK_MODE_E
{
    HI_TEE_CIPHER_WORK_MODE_ECB,        /**<Electronic codebook (ECB) mode*/    /**< CNcomment:ECBģʽ */
    HI_TEE_CIPHER_WORK_MODE_CBC,        /**<Cipher block chaining (CBC) mode*/  /**< CNcomment:CBCģʽ */
    HI_TEE_CIPHER_WORK_MODE_CFB,        /**<Cipher feedback (CFB) mode*/        /**< CNcomment:CFBģʽ */
    HI_TEE_CIPHER_WORK_MODE_OFB,        /**<Output feedback (OFB) mode*/        /**< CNcomment:OFBģʽ */
    HI_TEE_CIPHER_WORK_MODE_CTR,        /**<Counter (CTR) mode*/                /**< CNcomment:CTRģʽ */
    HI_TEE_CIPHER_WORK_MODE_CCM,        /**<Counter (CCM) mode*/                /**< CNcomment:CCMģʽ */
    HI_TEE_CIPHER_WORK_MODE_GCM,        /**<Counter (GCM) mode*/                /**< CNcomment:GCMģʽ */
    HI_TEE_CIPHER_WORK_MODE_CBC_CTS,    /**<Cipher block chaining CipherStealing mode*/  /**< CNcomment:CBC-CTSģʽ */
    HI_TEE_CIPHER_WORK_MODE_BUTT,
    HI_TEE_CIPHER_WORK_MODE_INVALID  = 0xffffffff,
}HI_TEE_CIPHER_WORK_MODE_E;

/** Cipher algorithm */
/** CNcomment:CIPHER�����㷨 */
typedef enum hiHI_TEE_CIPHER_ALG_E
{
    HI_TEE_CIPHER_ALG_DES           = 0x0,  /**< Data encryption standard (DES) algorithm */     /**< CNcomment: DES�㷨 */
    HI_TEE_CIPHER_ALG_3DES          = 0x1,  /**< 3DES algorithm */                               /**< CNcomment: 3DES�㷨 */
    HI_TEE_CIPHER_ALG_AES           = 0x2,  /**< Advanced encryption standard (AES) algorithm */ /**< CNcomment: AES�㷨 */
    HI_TEE_CIPHER_ALG_SM1           = 0x3,  /**<SM1 algorithm*/  /**< CNcomment: SM1�㷨 */
    HI_TEE_CIPHER_ALG_SM4           = 0x4,  /**<SM4 algorithm*/  /**< CNcomment: SM4�㷨 */
    HI_TEE_CIPHER_ALG_DMA           = 0x5,  /**<DMA copy*/  /**< CNcomment: DMA���� */
    HI_TEE_CIPHER_ALG_BUTT          = 0x6,
    HI_TEE_CIPHER_ALG_INVALID       = 0xffffffff,
}HI_TEE_CIPHER_ALG_E;

/** Key length */
/** CNcomment: ��Կ���� */
typedef enum hiHI_TEE_CIPHER_KEY_LENGTH_E
{
    HI_TEE_CIPHER_KEY_AES_128BIT    = 0x0,  /**< 128-bit key for the AES algorithm */ /**< CNcomment:AES���㷽ʽ�²���128bit��Կ���� */
    HI_TEE_CIPHER_KEY_AES_192BIT    = 0x1,  /**< 192-bit key for the AES algorithm */ /**< CNcomment:AES���㷽ʽ�²���192bit��Կ���� */
    HI_TEE_CIPHER_KEY_AES_256BIT    = 0x2,  /**< 256-bit key for the AES algorithm */ /**< CNcomment:AES���㷽ʽ�²���256bit��Կ���� */
    HI_TEE_CIPHER_KEY_DES_3KEY      = 0x2,  /**< Three keys for the DES algorithm */  /**< CNcomment:DES���㷽ʽ�²���3��key */
    HI_TEE_CIPHER_KEY_DES_2KEY      = 0x3,  /**< Two keys for the DES algorithm */    /**< CNcomment: DES���㷽ʽ�²���2��key  */
    HI_TEE_CIPHER_KEY_DEFAULT       = 0x0,  /**< default key length, DES-8, SM1-48, SM4-16 */    /**< CNcomment: Ĭ��Key���ȣ�DES-8, SM1-48, SM4-16*/
	HI_TEE_CIPHER_KEY_INVALID       = 0xffffffff,
}HI_TEE_CIPHER_KEY_LENGTH_E;

/** Cipher bit width */
/** CNcomment: ����λ�� */
typedef enum hiHI_TEE_CIPHER_BIT_WIDTH_E
{
    HI_TEE_CIPHER_BIT_WIDTH_64BIT   = 0x0,  /**< 64-bit width */   /**< CNcomment:64bitλ�� */
    HI_TEE_CIPHER_BIT_WIDTH_8BIT    = 0x1,  /**< 8-bit width */    /**< CNcomment:8bitλ�� */
    HI_TEE_CIPHER_BIT_WIDTH_1BIT    = 0x2,  /**< 1-bit width */    /**< CNcomment:1bitλ�� */
    HI_TEE_CIPHER_BIT_WIDTH_128BIT  = 0x3,  /**< 128-bit width */  /**< CNcomment:128bitλ�� */
    HI_TEE_CIPHER_BIT_WIDTH_INVALID = 0xffffffff,
}HI_TEE_CIPHER_BIT_WIDTH_E;

/** Cipher control parameters */
/** CNcomment:���ܿ��Ʋ��������־ */
typedef struct hiTEE_CIPHER_CTRL_CHANGE_FLAG_S
{
    HI_U32   bit1IV:4;              /**< Initial Vector change flag, 0-don't set, 1-set IV for first package, 2-set IV for each package  */ /**< CNcomment:�������, 0-�����ã�1-ֻ���õ�һ������2-ÿ���������� */
    HI_U32   bitsResv:28;           /**< Reserved */                     /**< CNcomment:���� */
}HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S;

/** Key ladder selecting parameters */
/** CNcomment:ʹ���ĸ�key ladder��־ */
typedef enum hiTEE_CIPHER_CA_TYPE_E
{
    HI_TEE_CIPHER_CA_TYPE_R2R   = 0x0,      /**< Using R2R key ladder */                                                 /**< CNcomment:ʹ��R2R key ladder */
    HI_TEE_CIPHER_CA_TYPE_SP,               /**< Using SP key ladder */                                                  /**< CNcomment:ʹ��SP key ladder */
    HI_TEE_CIPHER_CA_TYPE_CSA2,             /**< Using CSA2 key ladder */                                                /**< CNcomment:ʹ��CSA2 key ladder */
    HI_TEE_CIPHER_CA_TYPE_CSA3,             /**< Using CSA3 key ladder */                                                /**< CNcomment:ʹ��CSA3 key ladder */
    HI_TEE_CIPHER_CA_TYPE_MISC,             /**< Using MISC ladder */                                                    /**< CNcomment:ʹ��SP key ladder */
    HI_TEE_CIPHER_CA_TYPE_GDRM,             /**< Using GDRM ladder */                                                    /**< CNcomment:ʹ��GDRM key ladder */
    HI_TEE_CIPHER_CA_TYPE_BLPK,             /**< Using BLPK ladder */                                                    /**< CNcomment:ʹ��BLPK key ladder */
    HI_TEE_CIPHER_CA_TYPE_LPK,              /**< Using LPK ladder */                                                     /**< CNcomment:ʹ��LPK key ladder */
    HI_TEE_CIPHER_CA_TYPE_IRDETO_HCA,       /**< Using Irdeto high level code authentication key ladder, only used for Irdeto MSR2.2 */ /**< CNcomment:Irdeto�ϲ�Ӧ��У��keyladder��������Irdeto MSR2.2 */
    HI_TEE_CIPHER_CA_TYPE_STBROOTKEY,       /**< Useing for AntiCopy With STB_Root_Key  */                                                  /**< CNcomment:ʹ��STB_Root_Key�ӽ���,���ڷ����� */
    HI_TEE_CIPHER_CA_TYPE_DCAS,             /**< Using DCAS ladder */                                                    /**< CNcomment:ʹ��LPK DCAS ladder */
    HI_TEE_CIPHER_CA_TYPE_SECSTORE,
    HI_TEE_CIPHER_CA_TYPE_BUTT,
    HI_TEE_CIPHER_CA_TYPE_INVALID = 0xffffffff,
}HI_TEE_CIPHER_CA_TYPE_E;

/** Encryption/Decryption type selecting */
/** CNcomment:CIPHE�ӽ�������ѡ�� */
typedef enum
{
    HI_TEE_CIPHER_TYPE_NORMAL  = 0x0,
    HI_TEE_CIPHER_TYPE_COPY_AVOID,
    HI_TEE_CIPHER_TYPE_BUTT,
    HI_TEE_CIPHER_TYPE_INVALID = 0xffffffff,
}HI_TEE_CIPHER_TYPE_E;

/** Structure of the cipher type */
/** CNcomment:�������ͽṹ */
typedef struct
{
    HI_TEE_CIPHER_TYPE_E enCipherType;
}HI_TEE_CIPHER_ATTS_S;

/** sm1 round config */
/** CNcomment: sm1��������� */
typedef enum hiHI_TEE_CIPHER_SM1_ROUND_E
{
    HI_TEE_CIPHER_SM1_ROUND_08 = 0x00,			/**< sm1 round 08 */ /**< CNcomment:SM1���������Ϊ08 */
    HI_TEE_CIPHER_SM1_ROUND_10 = 0x01,			/**< sm1 round 10 */ /**< CNcomment:SM1���������Ϊ10 */
    HI_TEE_CIPHER_SM1_ROUND_12 = 0x02,			/**< sm1 round 12 */ /**< CNcomment:SM1���������Ϊ12 */
    HI_TEE_CIPHER_SM1_ROUND_14 = 0x03,			/**< sm1 round 14 */ /**< CNcomment:SM1���������Ϊ14 */
    HI_TEE_CIPHER_SM1_ROUND_BUTT,
    HI_TEE_CIPHER_SM1_ROUND_INVALID  = 0xffffffff,
}HI_TEE_CIPHER_SM1_ROUND_E;

/** Structure of the cipher control information */
/** CNcomment:���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_S
{
    HI_U32 u32Key[8];                               /**< Key input */                                                                                                     /**< CNcomment:������Կ */
    HI_U32 u32IV[4];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_BOOL bKeyByCA;                               /**< Encryption using advanced conditional access (CA) or decryption using keys */                                    /**< CNcomment:�Ƿ�ʹ�ø߰�ȫCA���ܻ����Key */
    HI_TEE_CIPHER_CA_TYPE_E enCaType;                   /**< Select keyladder type when using advanced CA */                                                                  /**< CNcomment:ʹ�ø߰�ȫCAʱ,ѡ��������͵�keyladder */
    HI_TEE_CIPHER_ALG_E enAlg;                      /**< Cipher algorithm */                                                                                              /**< CNcomment:�����㷨 */
    HI_TEE_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */                                                                        /**< CNcomment:���ܻ���ܵ�λ�� */
    HI_TEE_CIPHER_WORK_MODE_E enWorkMode;           /**< Operating mode */                                                                                                /**< CNcomment:����ģʽ */
    HI_TEE_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */                                                                                                    /**< CNcomment:��Կ���� */
    HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all would be change except they have been in the choices */    /**< CNcomment:������Ϣ���ѡ�ѡ����û�б�ʶ����Ĭ��ȫ����� */
} HI_TEE_CIPHER_CTRL_S;

/** Structure of the cipher AES control information */
/** CNcomment:AES���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_AES_S
{
    HI_U32 u32EvenKey[8];                           /**< Key input, default use this key*/                                                                                /**< CNcomment:����ż��Կ, Ĭ��ʹ��ż��Կ */
    HI_U32 u32OddKey[8];                            /**< Key input, only valid for Multi encrypt/decrypt*/                                                                /**< CNcomment:��������Կ, ֻ�Զ���ӽ�����Ч */
    HI_U32 u32IV[4];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_TEE_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */                                                                        /**< CNcomment:���ܻ���ܵ�λ�� */
    HI_TEE_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */                                                                                                    /**< CNcomment:��Կ���� */
    HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */    /**< CNcomment:������Ϣ���ѡ�ѡ����û�б�ʶ����Ĭ��ȫ����� */
} HI_TEE_CIPHER_CTRL_AES_S;

/** Structure of the cipher AES CCM/GCM control information */
/** CNcomment:AES CCM/GCM ���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_AES_CCM_GCM_S
{
    HI_U32 u32Key[8];                               /**< Key input */                                                                                                     /**< CNcomment:������Կ */
    HI_U32 u32IV[4];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_TEE_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */                                                                                                    /**< CNcomment:��Կ���� */
    HI_U32 u32IVLen;                                /**< IV lenght for CCM/GCM, which is an element of {4,6,8,10,12,14,16} for CCM, and is an element of [1-16] for GCM*/  /**< CNcomment: CCM/GCM��IV���ȣ�CCM��ȡֵ��Χ{7, 8, 9, 10, 11, 12, 13}�� GCM��ȡֵ��Χ[1-16]*/
    HI_U32 u32TagLen;                               /**< Tag lenght for CCM which is an element of {4,6,8,10,12,14,16}*/          /**< CNcomment: CCM��TAG���ȣ�ȡֵ��Χ{4,6,8,10,12,14,16}*/
    HI_U32 u32ALen;                                 /**< Associated data for CCM and GCM*/                                        /**< CNcomment: CCM/GCM�Ĺ������ݳ���*/
    HI_U32 u32APhyAddr;                             /**< Physical address of Associated data for CCM and GCM*/                                        /**< CNcomment: CCM/GCM�Ĺ������ݳ���*/
} HI_TEE_CIPHER_CTRL_AES_CCM_GCM_S;

/** Structure of the cipher DES control information */
/** CNcomment:DES���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_DES_S
{
    HI_U32 u32Key[2];                               /**< Key input */                                                                                                     /**< CNcomment:����ż��Կ */
    HI_U32 u32IV[2];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_TEE_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */                                                                        /**< CNcomment:���ܻ���ܵ�λ�� */
    HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */    /**< CNcomment:������Ϣ���ѡ�ѡ����û�б�ʶ����Ĭ��ȫ����� */
} HI_TEE_CIPHER_CTRL_DES_S;

/** Structure of the cipher 3DES control information */
/** CNcomment:3DES���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_3DES_S
{
    HI_U32 u32Key[6];                               /**< Key input */                                                                                                     /**< CNcomment:����ż��Կ */
    HI_U32 u32IV[2];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_TEE_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */                                                                        /**< CNcomment:���ܻ���ܵ�λ�� */
    HI_TEE_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */                                                                                                    /**< CNcomment:��Կ���� */
    HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */    /**< CNcomment:������Ϣ���ѡ�ѡ����û�б�ʶ����Ĭ��ȫ����� */
} HI_TEE_CIPHER_CTRL_3DES_S;

/** Structure of the cipher SM1 control information */
/** CNcomment:SM1���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_SM1_S
{
    HI_U32 u32EK[4];                               /**< Key of EK input */                                                                                                /**< CNcomment:������ԿEK */
    HI_U32 u32AK[4];                               /**< Key of AK input */                                                                                                /**< CNcomment:������ԿAK */
    HI_U32 u32SK[4];                               /**< Key of SK input */                                                                                                /**< CNcomment:������ԿSK */
    HI_U32 u32IV[4];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_TEE_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */                                                                        /**< CNcomment:���ܻ���ܵ�λ�� */
    HI_TEE_CIPHER_SM1_ROUND_E enSm1Round;           /**< SM1 round number, should be 8, 10, 12 or 14*/                                                                                                    /**< CNcomment:sm1������������� */
    HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */    /**< CNcomment:������Ϣ���ѡ�ѡ����û�б�ʶ����Ĭ��ȫ����� */
} HI_TEE_CIPHER_CTRL_SM1_S;

/** Structure of the cipher SM4 control information */
/** CNcomment:SM4���ܿ�����Ϣ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_SM4_S
{
    HI_U32 u32Key[4];                               /**< Key  input */                                                                                                    /**< CNcomment:������Կ */
    HI_U32 u32IV[4];                                /**< Initialization vector (IV) */                                                                                    /**< CNcomment:��ʼ���� */
    HI_TEE_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */    /**< CNcomment:������Ϣ���ѡ�ѡ����û�б�ʶ����Ĭ��ȫ����� */
} HI_TEE_CIPHER_CTRL_SM4_S;

/** Expand Structure of the cipher control information */
/** CNcomment:���ܿ�����Ϣ��չ�ṹ */
typedef struct hiHI_TEE_CIPHER_CTRL_EX_S
{
    HI_TEE_CIPHER_ALG_E enAlg;                      /**< Cipher algorithm */                                                                                              /**< CNcomment:�����㷨 */
    HI_TEE_CIPHER_WORK_MODE_E enWorkMode;           /**< Operating mode */                                                                                                /**< CNcomment:����ģʽ */
    HI_BOOL bKeyByCA;                               /**< Encryption using advanced conditional access (CA) or decryption using keys */                                    /**< CNcomment:�Ƿ�ʹ�ø߰�ȫCA���ܻ����Key */
    /**< Parameter for special algorithm
        for AES, the pointer should point to HI_TEE_CIPHER_CTRL_AES_S;
        for AES_CCM or AES_GCM, the pointer should point to HI_TEE_CIPHER_CTRL_AES_CCM_GCM_S;
        for DES, the pointer should point to HI_TEE_CIPHER_CTRL_DES_S;
        for 3DES, the pointer should point to HI_TEE_CIPHER_CTRL_3DES_S;
        for SM1, the pointer should point to HI_TEE_CIPHER_CTRL_SM1_S;
        for SM4, the pointer should point to HI_TEE_CIPHER_CTRL_SM4_S;
    */
    /**< CNcomment: �㷨��ר�ò���
        ���� AES, ָ��Ӧָ�� HI_TEE_CIPHER_CTRL_AES_S;
        ���� AES_CCM �� AES_GCM, ָ��Ӧָ�� HI_TEE_CIPHER_CTRL_AES_CCM_GCM_S;
        ���� DES, ָ��Ӧָ�� HI_TEE_CIPHER_CTRL_DES_S;
        ���� 3DES, ָ��Ӧָ�� HI_TEE_CIPHER_CTRL_3DES_S;
        ���� SM1, ָ��Ӧָ�� HI_TEE_CIPHER_CTRL_SM1_S;
        ���� SM4, ָ��Ӧָ�� HI_TEE_CIPHER_CTRL_SM4_S;
    */
    HI_VOID *pParam;
} HI_TEE_CIPHER_CTRL_EX_S;

/** Cipher data */
/** CNcomment:�ӽ������� */
typedef struct hiHI_TEE_CIPHER_DATA_S
{
    HI_U32 u32SrcPhyAddr;     /**< phy address of the original data */   /**< CNcomment:Դ���������ַ */
    HI_U32 u32DestPhyAddr;    /**< phy address of the purpose data */    /**< CNcomment:Ŀ�����������ַ */
    HI_U32 u32ByteLength;     /**< cigher data length*/                 /**< CNcomment:�ӽ������ݳ��� */
    HI_BOOL bOddKey;          /**< Use odd key or even key*/                  /**< CNcomment:�Ƿ�ʹ������Կ */
} HI_TEE_CIPHER_DATA_S;

/** Cipher info */
/** CNcomment: ��Ϣ */
typedef struct hiHI_TEE_CIPHER_INFO_S
{
    HI_BOOL bIsCipherBusy;     /**< Cipher busy */   /**< CNcomment: CIPHER�Ƿ�æ */
} HI_TEE_CIPHER_INFO_S;

/** Cipher usage rule*/
/** CNcomment:Cipherʹ�ù������� */
typedef struct hiTEE_CIPHER_UR_S
{
    HI_U32 bProtectedSrcAddr:1;   /**< Protecte the source address or nor, if set to 1, the source address must be security*/   /**< CNcomment: �Ƿ񱣻�Դ��ַ�������Ϊ1��Դ��ַ�����ǰ�ȫ�� */
    HI_U32 bResv:31;              /**< Reserved */                      /**< CNcomment:���� */
}HI_TEE_CIPHER_UR_S;

/** Hash algrithm type */
/** CNcomment:��ϣ�㷨���� */
typedef enum hiHI_TEE_CIPHER_HASH_TYPE_E
{
    HI_TEE_CIPHER_HASH_TYPE_SHA1,
    HI_TEE_CIPHER_HASH_TYPE_SHA224,
    HI_TEE_CIPHER_HASH_TYPE_SHA256,
    HI_TEE_CIPHER_HASH_TYPE_SHA384,
    HI_TEE_CIPHER_HASH_TYPE_SHA512,
    HI_TEE_CIPHER_HASH_TYPE_HMAC_SHA1,
    HI_TEE_CIPHER_HASH_TYPE_HMAC_SHA224,
    HI_TEE_CIPHER_HASH_TYPE_HMAC_SHA256,
    HI_TEE_CIPHER_HASH_TYPE_HMAC_SHA384,
    HI_TEE_CIPHER_HASH_TYPE_HMAC_SHA512,
    HI_TEE_CIPHER_HASH_TYPE_SM3,
    HI_TEE_CIPHER_HASH_TYPE_IRDETO_CBCMAC, /**< Irdeto MSR2.2 CBC-MAC*/ /**< CNcomment: Irdeto MSR2.2 CBC-MAC*/
    HI_TEE_CIPHER_HASH_TYPE_BUTT,
    HI_TEE_CIPHER_HASH_TYPE_INVALID  = 0xffffffff,
}HI_TEE_CIPHER_HASH_TYPE_E;

/** Hash init struct input */
/** CNcomment:��ϣ�㷨��ʼ������ṹ�� */
typedef struct
{
    HI_U8 *pu8HMACKey;
    HI_U32 u32HMACKeyLen;
    HI_TEE_CIPHER_HASH_TYPE_E eShaType;
}HI_TEE_CIPHER_HASH_ATTS_S;

/** PBKDF2 struct input */
/** CNcomment: PBKDF2��Կ�����㷨�����ṹ�� */
typedef struct hiHI_TEE_CIPHER_PBKDF2_S
{
    HI_U8 *password;
    HI_U32 plen;
    HI_U8 *salt;
    HI_U32 slen;
    HI_U32 iteration_count;
    HI_U32 key_length;
}HI_TEE_CIPHER_PBKDF2_S;
typedef HI_TEE_CIPHER_PBKDF2_S CIPHER_PBKDF2_S;

typedef enum hiHI_TEE_CIPHER_RSA_ENC_SCHEME_E
{
    HI_TEE_CIPHER_RSA_ENC_SCHEME_NO_PADDING,            /**< without padding */             /**< CNcomment: ����� */
    HI_TEE_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0,          /**< PKCS#1 block type 0 padding*/  /**< CNcomment: PKCS#1��block type 0��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1,          /**< PKCS#1 block type 1 padding*/  /**< CNcomment: PKCS#1��block type 1��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2,          /**< PKCS#1 block type 2 padding*/  /**< CNcomment: PKCS#1��block type 2��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1,       /**< PKCS#1 RSAES-OAEP-SHA1 padding*/    /**< CNcomment: PKCS#1��RSAES-OAEP-SHA1��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA224,     /**< PKCS#1 RSAES-OAEP-SHA224 padding*/  /**< CNcomment: PKCS#1��RSAES-OAEP-SHA224��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256,     /**< PKCS#1 RSAES-OAEP-SHA256 padding*/  /**< CNcomment: PKCS#1��RSAES-OAEP-SHA256��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA384,     /**< PKCS#1 RSAES-OAEP-SHA384 padding*/    /**< CNcomment: PKCS#1��RSAES-OAEP-SHA384��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA512,     /**< PKCS#1 RSAES-OAEP-SHA512 padding*/  /**< CNcomment: PKCS#1��RSAES-OAEP-SHA512��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5,      /**< PKCS#1 RSAES-PKCS1_V1_5 padding*/   /**< CNcomment: PKCS#1��PKCS1_V1_5��䷽ʽ*/
    HI_TEE_CIPHER_RSA_ENC_SCHEME_BUTT,
    HI_TEE_CIPHER_RSA_ENC_SCHEME_INVALID  = 0xffffffff,
}HI_TEE_CIPHER_RSA_ENC_SCHEME_E;

typedef enum hiHI_TEE_CIPHER_RSA_SIGN_SCHEME_E
{
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1 = 0x100, /**< PKCS#1 RSASSA_PKCS1_V15_SHA1 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_V15_SHA1ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA224 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_V15_SHA224ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA256 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_V15_SHA256ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA384 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_V15_SHA384ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA512 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_V15_SHA512ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1,         /**< PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_PSS_SHA1ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA224 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_PSS_SHA224ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_PSS_SHA256ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_PSS_SHA384ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/   /**< CNcomment: PKCS#1 RSASSA_PKCS1_PSS_SHA512ǩ���㷨*/
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_BUTT,
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_INVALID  = 0xffffffff,
}HI_TEE_CIPHER_RSA_SIGN_SCHEME_E;

typedef struct
{
    HI_U8  *pu8N;              /**< point to public modulus  */   /**< CNcomment: ָ��RSA��ԿN��ָ��*/
    HI_U8  *pu8E;              /**< point to public exponent */   /**< CNcomment: ָ��RSA��ԿE��ָ��*/
    HI_U16 u16NLen;            /**< length of public modulus, max value is 512Byte*/  /**< CNcomment: RSA��ԿN�ĳ���, ���Ϊ512Byte*/
    HI_U16 u16ELen;            /**< length of public exponent, max value is 512Byte*/  /**< CNcomment: RSA��ԿE�ĳ���, ���Ϊ512Byte*/
}HI_TEE_CIPHER_RSA_PUB_KEY_S;

/** RSA private key struct */
/** CNcomment:RSA˽Կ�ṹ�� */
typedef struct
{
    HI_U8 *pu8N;                      /*!<  public modulus    */ /**< CNcomment: ָ��RSA��ԿN��ָ��*/
    HI_U8 *pu8E;                      /*!<  public exponent   */ /**< CNcomment: ָ��RSA��ԿE��ָ��*/
    HI_U8 *pu8D;                      /*!<  private exponent  */ /**< CNcomment: ָ��RSA˽ԿD��ָ��*/
    HI_U8 *pu8P;                      /*!<  1st prime factor  */ /**< CNcomment: ָ��RSA˽ԿP��ָ��*/
    HI_U8 *pu8Q;                      /*!<  2nd prime factor  */ /**< CNcomment: ָ��RSA˽ԿQ��ָ��*/
    HI_U8 *pu8DP;                     /*!<  D % (P - 1)       */ /**< CNcomment: ָ��RSA˽ԿDP��ָ��*/
    HI_U8 *pu8DQ;                     /*!<  D % (Q - 1)       */ /**< CNcomment: ָ��RSA˽ԿDQ��ָ��*/
    HI_U8 *pu8QP;                     /*!<  1 / (Q % P)       */ /**< CNcomment: ָ��RSA˽ԿQP��ָ��*/
    HI_U16 u16NLen;                   /**< length of public modulus */   /**< CNcomment: RSA��ԿN�ĳ���*/
    HI_U16 u16ELen;                   /**< length of public exponent */  /**< CNcomment: RSA��ԿE�ĳ���*/
    HI_U16 u16DLen;                   /**< length of private exponent */ /**< CNcomment: RSA˽ԿD�ĳ���*/
    HI_U16 u16PLen;                   /**< length of 1st prime factor */ /**< CNcomment: RSA˽ԿP�ĳ���*/
    HI_U16 u16QLen;                   /**< length of 2nd prime factor */ /**< CNcomment: RSA˽ԿQ�ĳ���*/
    HI_U16 u16DPLen;                  /**< length of D % (P - 1) */      /**< CNcomment: RSA˽ԿDP�ĳ���*/
    HI_U16 u16DQLen;                  /**< length of D % (Q - 1) */      /**< CNcomment: RSA˽ԿDQ�ĳ���*/
    HI_U16 u16QPLen;                  /**< length of 1 / (Q % P) */      /**< CNcomment: RSA˽ԿQP�ĳ���*/
}HI_TEE_CIPHER_RSA_PRI_KEY_S;

/** RSA public key encryption struct input */
/** CNcomment:RSA ��Կ�ӽ����㷨�����ṹ�� */
typedef struct
{
    HI_TEE_CIPHER_RSA_ENC_SCHEME_E enScheme;   /** RSA encryption scheme*/ /** CNcomment:RSA���ݼӽ����㷨����*/
    HI_TEE_CIPHER_RSA_PUB_KEY_S stPubKey;      /** RSA private key struct */ /** CNcomment:RSA˽Կ�ṹ�� */
}HI_TEE_CIPHER_RSA_PUB_ENC_S;

/** RSA private key decryption struct input */
/** CNcomment:RSA ˽Կ�����㷨�����ṹ�� */
typedef struct
{
    HI_TEE_CIPHER_RSA_ENC_SCHEME_E enScheme; /** RSA encryption scheme */ /** CNcomment:RSA���ݼӽ����㷨*/
    HI_TEE_CIPHER_RSA_PRI_KEY_S stPriKey;    /** RSA public key struct */ /** CNcomment:RSA��Կ�ṹ�� */
}HI_TEE_CIPHER_RSA_PRI_ENC_S;

/** RSA signature struct input */
/** CNcomment:RSAǩ���㷨�����ṹ�� */
typedef struct
{
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_E enScheme;  /** RSA signature scheme*/ /** CNcomment:RSA����ǩ������*/
    HI_TEE_CIPHER_RSA_PRI_KEY_S stPriKey;      /** RSA private key struct */ /** CNcomment:RSA˽Կ�ṹ�� */
 }HI_TEE_CIPHER_RSA_SIGN_S;

/** RSA signature verify struct input */
/** CNcomment:RSAǩ����֤�㷨��������ṹ�� */
typedef struct
{
    HI_TEE_CIPHER_RSA_SIGN_SCHEME_E enScheme; /** RSA signature scheme*/ /** CNcomment:RSA����ǩ������*/
    HI_TEE_CIPHER_RSA_PUB_KEY_S stPubKey;     /** RSA public key struct */ /** CNcomment:RSA��Կ�ṹ�� */
 }HI_TEE_CIPHER_RSA_VERIFY_S;

 /** SM2 signature struct input */
/** CNcomment: SM2ǩ���㷨�����ṹ�� */
 typedef struct hiHI_TEE_CIPHER_SM2_SIGN_S
{
    HI_U32 u32d[SM2_LEN_IN_WROD];
    HI_U32 u32Px[SM2_LEN_IN_WROD];
    HI_U32 u32Py[SM2_LEN_IN_WROD];
    HI_U8 *pu8Id;
    HI_U16 u16IdLen;
}HI_TEE_CIPHER_SM2_SIGN_S;

/** SM2 signature verify struct input */
/** CNcomment: SM2ǩ����֤�㷨��������ṹ�� */
typedef struct hiHI_TEE_CIPHER_SM2_VERIFY_S
{
    HI_U32 u32Px[SM2_LEN_IN_WROD];
    HI_U32 u32Py[SM2_LEN_IN_WROD];
    HI_U8 *pu8Id;
    HI_U16 u16IdLen;
}HI_TEE_CIPHER_SM2_VERIFY_S;

/** SM2 publuc key encryption struct input */
/** CNcomment: SM2��Կ�����㷨�����ṹ�� */
typedef struct hiHI_TEE_CIPHER_SM2_ENC_S
{
    HI_U32 u32Px[SM2_LEN_IN_WROD];
    HI_U32 u32Py[SM2_LEN_IN_WROD];
}HI_TEE_CIPHER_SM2_ENC_S;

/** SM2 private key decryption struct input */
/** CNcomment: SM2˽Կ�����㷨�����ṹ�� */
typedef struct hiHI_TEE_CIPHER_SM2_DEC_S
{
    HI_U32 u32d[SM2_LEN_IN_WROD];
}HI_TEE_CIPHER_SM2_DEC_S;

/** SM2 key generate struct input */
/** CNcomment: SM2��Կ�����㷨�����ṹ�� */
typedef struct hiHI_TEE_CIPHER_SM2_KEY_S
{
    HI_U32 u32d[SM2_LEN_IN_WROD];
    HI_U32 u32Px[SM2_LEN_IN_WROD];
    HI_U32 u32Py[SM2_LEN_IN_WROD];
}HI_TEE_CIPHER_SM2_KEY_S;

/** CENC subsample struct input */
/** CNcomment: CENC subsample �����ṹ�� */
typedef struct hiHI_TEE_CIPHER_SUBSAMPLE_S
{
    HI_U32 u32ClearHeaderLen;
    HI_U32 u32PayLoadLen;
    HI_U32 u32PayloadPatternEncryptLen;
    HI_U32 u32PayloadPatternClearLen;
    HI_U32 u32PayloadPatternOffsetLen;
}HI_TEE_CIPHER_SUBSAMPLE_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */


#define HI_TEE_CIPHER_Open(HI_VOID) HI_TEE_CIPHER_Init(HI_VOID);
#define HI_TEE_CIPHER_Close(HI_VOID) HI_TEE_CIPHER_DeInit(HI_VOID);

/******************************* API Declaration *****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */
/* ---CIPHER---*/
/**
\brief  Init the cipher device.  CNcomment:��ʼ��CIPHER�豸�� CNend
\attention \n
This API is used to start the cipher device.
CNcomment:���ô˽ӿڳ�ʼ��CIPHER�豸�� CNend
\param N/A                                                                      CNcomment:�� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_FAILED_INIT  The cipher device fails to be initialized. CNcomment:CIPHER�豸��ʼ��ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Init(HI_VOID);

/**
\brief  Deinit the cipher device.
CNcomment:\brief  ȥ��ʼ��CIPHER�豸�� CNend
\attention \n
This API is used to stop the cipher device. If this API is called repeatedly, HI_SUCCESS is returned, but only the first operation takes effect.
CNcomment:���ô˽ӿڹر�CIPHER�豸���ظ��رշ��سɹ�����һ�������á� CNend

\param N/A                                                                      CNcomment:�� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_DeInit(HI_VOID);

/**
\brief Obtain a cipher handle for encryption and decryption.
CNcomment������һ·Cipher����� CNend

\param[in] cipher attributes                                                    CNcomment:cipher ���ԡ� CNend
\param[out] phCipher Cipher handle                                              CNcomment:CIPHER����� CNend
\retval ::HI_SUCCESS Call this API successful.                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                                       CNcomment: APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     CNcomment:ָ�����Ϊ�� CNend
\retval ::HI_ERR_CIPHER_FAILED_GETHANDLE  The cipher handle fails to be obtained, because there are no available cipher handles. CNcomment: ��ȡCIPHER���ʧ�ܣ�û�п��е�CIPHER��� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_CreateHandle(HI_HANDLE* phCipher, const HI_TEE_CIPHER_ATTS_S *pstCipherAttr);

/**
\brief Destroy the existing cipher handle. CNcomment:�����Ѵ��ڵ�CIPHER����� CNend
\attention \n
This API is used to destroy existing cipher handles.
CNcomment:���ô˽ӿ������Ѿ�������CIPHER����� CNend

\param[in] hCipher Cipher handle                                                CNcomment:CIPHER����� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_DestroyHandle(HI_HANDLE hCipher);

/**
\brief Configures the cipher control information.
CNcomment:\brief ����CIPHER������Ϣ�� CNend
\attention \n
Before encryption or decryption, you must call this API to configure the cipher control information.
The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.
CNcomment:���м��ܽ���ǰ������ʹ�ô˽ӿ�����CIPHER�Ŀ�����Ϣ��ʹ��TDES�㷨ʱ��������Կ��ǰ��64 bit���ݲ�����ͬ�� CNend

\param[in] hCipher Cipher handle.                                               CNcomment:CIPHER��� CNend
\param[in] pstCtrl Cipher control information.                                  CNcomment:CIPHER������Ϣ CNend
\retval ::HI_SUCCESS Call this API successful.                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     CNcomment:ָ�����Ϊ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_ConfigHandle(HI_HANDLE hCipher, HI_TEE_CIPHER_CTRL_S* pstCtrl);

/**
\brief Configures the cipher control information.
CNcomment:\brief ����CIPHER������Ϣ�� CNend
\attention \n
Before encryption or decryption, you must call this API or HI_UNF_CIPHER_ConfigHandle to configure the cipher control information.
The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.
Support AES/DES/3DES/SM1/SM4 algorithm, support ECB/CBC/CTR/OFB/CFB/CCM/GCM mode.
CNcomment:���м��ܽ���ǰ������ʹ�ô˽ӿڻ�HI_UNF_CIPHER_ConfigHandle����CIPHER�Ŀ�����Ϣ��
ʹ��TDES�㷨ʱ��������Կ��ǰ��64 bit���ݲ�����ͬ��
֧�� AES/DES/3DES/SM1/SM4 �㷨, ECB/CBC/CTR/OFB/CFB/CCM/GCM ģʽ.CNend

\param[in] hCipher Cipher handle.                                               CNcomment:CIPHER��� CNend
\param[in] pstExCtrl Cipher control information.                                CNcomment:CIPHER��չ������Ϣ CNend
\retval ::HI_SUCCESS Call this API succussful.                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     CNcomment:ָ�����Ϊ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_ConfigHandleEx(HI_HANDLE hCipher, HI_TEE_CIPHER_CTRL_EX_S* pstExCtrl);

/**
\brief Set usage rule.
CNcomment:\brief ���м��ܡ� CNend

\attention \n
This API is used to set usage rule to the cipher module.
CNcomment:����ʹ�ù��� CNend
\param[in] hCipher Cipher handle                                                CNcomment:CIPHER��� CNend
\param[in] pstCipherUR usage rule information.                                  CNcomment:ʹ�ù��� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_SetUsageRule(HI_U32 hCipher, HI_TEE_CIPHER_UR_S *pstCipherUR);

/**
\brief Performs encryption.
CNcomment:\brief ���м��ܡ� CNend

\attention \n
This API is used to perform encryption by using the cipher module.
The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering HI_TEE_CIPHER_ConfigHandle.
CNcomment:ʹ��CIPHER���м��ܲ�����TDESģʽ�¼��ܵ����ݳ���Ӧ����8�ı�����AES��Ӧ����16�ı��������⣬�������ݳ��Ȳ��ܳ���0xFFFFF�����β�����ɺ󣬴˴β�������������������������һ�β��������Ҫ�����������Ҫ���´μ��ܲ���֮ǰ����HI_TEE_CIPHER_ConfigHandle��������IV(��Ҫ����pstCtrl->stChangeFlags.bit1IVΪ1)�� CNend
\param[in] hCipher Cipher handle                                                CNcomment:CIPHER��� CNend
\param[in] u32SrcPhyAddr Physical address of the source data                    CNcomment:Դ���������ַ CNend
\param[in] u32DestPhyAddr Physical address of the target data                   CNcomment:Ŀ�����������ַ CNend
\param[in] u32ByteLength   Length of the encrypted data                         CNcomment:�������ݳ��� CNend
\retval ::HI_SUCCESS  Call this API successful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Encrypt(HI_HANDLE hCipher, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength);

/**
\brief Performs decryption.
CNcomment:\brief ���н��� CNend

\attention \n
This API is used to perform decryption by using the cipher module.
The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering HI_TEE_CIPHER_ConfigHandle.
CNcomment:ʹ��CIPHER���н��ܲ�����TDESģʽ�½��ܵ����ݳ���Ӧ����8�ı�����AES��Ӧ����16�ı��������⣬�������ݳ��Ȳ��ܳ���0xFFFFF�����β�����ɺ󣬴˴β�������������������������һ�β��������Ҫ�����������Ҫ���´ν��ܲ���֮ǰ����HI_TEE_CIPHER_ConfigHandle��������IV(��Ҫ����pstCtrl->stChangeFlags.bit1IVΪ1)�� CNend
\param[in] hCipher Cipher handle.                                               CNcomment:CIPHER��� CNend
\param[in] u32SrcPhyAddr Physical address of the source data.                   CNcomment:Դ���������ַ CNend
\param[in] u32DestPhyAddr Physical address of the target data.                  CNcomment:Ŀ�����������ַ CNend
\param[in] u32ByteLength Length of the decrypted data                           CNcomment:�������ݳ��� CNend
\retval ::HI_SUCCESS Call this API successful.                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Decrypt(HI_HANDLE hCipher, HI_U32 u32SrcPhyAddr, HI_U32 u32DestPhyAddr, HI_U32 u32ByteLength);

/**
\brief Encrypt multiple packaged data.
CNcomment:\brief ���ж�������ݵļ��ܡ� CNend
\attention \n
You can not encrypt more than 128 data package one time. When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the customer should decrease the number of data package or run cipher again.Note:When encrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_TEE_CIPHER_ConfigHandle.Previous result will not affect the later result.
CNcomment:ÿ�μ��ܵ����ݰ�������಻�ܳ���128��������HI_ERR_CIPHER_BUSY��ʱ���������ݰ�һ��Ҳ���ᱻ�����û���Ҫ������������ݰ������������ٴγ��Լ��ܡ�ע��: ���ڶ�����Ĳ�����ÿ������ʹ��HI_TEE_CIPHER_ConfigHandle���õ������������㣬ǰһ����������������������������һ���������㣬ÿ�������Ƕ�������ġ�ǰһ�κ������õĽ��Ҳ����Ӱ���һ�κ������õ��������� CNend
\param[in] hCipher cipher handle                                                                  CNcomment:CIPHER����� CNend
\param[in] pstDataPkg data package ready for cipher                                               CNcomment:�����ܵ����ݰ��� CNend
\param[in] u32DataPkgNum  number of package ready for cipher                                      CNcomment:�����ܵ����ݰ������� CNend
\retval ::HI_SUCCESS  Call this API successful.                                                   CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                             CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                            CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time   CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_EncryptMulti(HI_HANDLE hCipher, HI_TEE_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief Get the cipher control information.
CNcomment:\brief ��ȡCIPHER������Ϣ�� CNend

\param[in] hCipher Cipher handle.                                               CNcomment:CIPHER��� CNend
\param[in] pstCtrl Cipher control information.                                  CNcomment:CIPHER������Ϣ CNend
\retval ::HI_SUCCESS Call this API successful.                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE Call this API fails.                                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.                     CNcomment:ָ�����Ϊ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_GetHandleConfig(HI_HANDLE hCipher, HI_TEE_CIPHER_CTRL_S* pstCtrl);

/**
\brief Decrypt multiple packaged data.
CNcomment:\brief ���ж�������ݵĽ��ܡ� CNend
\attention \n
You can not decrypt more than 128 data package one time.When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_TEE_CIPHER_ConfigHandle.Previous result will not affect the later result.
CNcomment:ÿ�ν��ܵ����ݰ�������಻�ܳ���128��������HI_ERR_CIPHER_BUSY��ʱ���������ݰ�һ��Ҳ���ᱻ�����û���Ҫ������������ݰ������������ٴγ��Խ��ܡ�ע��: ���ڶ�����Ĳ�����ÿ������ʹ��HI_TEE_CIPHER_ConfigHandle���õ������������㣬ǰһ����������������������������һ���������㣬ÿ�������Ƕ�������ģ�ǰһ�κ������õĽ��Ҳ����Ӱ���һ�κ������õ��������� CNend
\param[in] hCipher cipher handle                                                                 CNcomment:CIPHER����� CNend
\param[in] pstDataPkg data package ready for cipher                                              CNcomment:�����ܵ����ݰ��� CNend
\param[in] u32DataPkgNum  number of package ready for cipher                                     CNcomment:�����ܵ����ݰ������� CNend
\retval ::HI_SUCCESS  Call this API successful.                                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time  CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_DecryptMulti(HI_HANDLE hCipher, HI_TEE_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief Encrypt multiple packaged data asynchronously.
CNcomment:\brief �첽���ж�������ݵļ��ܡ� CNend
\attention \n
You can not encrypt more than 128 data package one time. When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When encrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_TEE_CIPHER_ConfigHandle.Previous result will not affect the later result.
CNcomment:ÿ�μ��ܵ����ݰ�������಻�ܳ���128��������HI_ERR_CIPHER_BUSY��ʱ���������ݰ�һ��Ҳ���ᱻ�����û���Ҫ������������ݰ������������ٴγ��Լ��ܡ�ע��: ���ڶ�����Ĳ�����ÿ������ʹ��HI_TEE_CIPHER_ConfigHandle���õ������������㣬ǰһ����������������������������һ���������㣬ÿ�������Ƕ�������ġ�ǰһ�κ������õĽ��Ҳ����Ӱ���һ�κ������õ��������� CNend
\param[in] hCipher cipher handle                                                                  CNcomment:CIPHER����� CNend
\param[in] pstDataPkg data package ready for cipher                                               CNcomment:�����ܵ����ݰ��� CNend
\param[in] u32DataPkgNum  number of package ready for cipher                                      CNcomment:�����ܵ����ݰ������� CNend
\retval ::HI_SUCCESS  Call this API succussful.                                                   CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                             CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                            CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time   CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_EncryptMultiAsyn(HI_HANDLE hCipher, HI_TEE_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief Decrypt multiple packaged data asynchronously.
CNcomment:\brief �첽���ж�������ݵĽ��ܡ� CNend
\attention \n
You can not decrypt more than 128 data package one time.When HI_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting more than one packaged data, every one package will be calculated using initial vector configured by HI_TEE_CIPHER_ConfigHandle.Previous result will not affect the later result.
CNcomment:ÿ�ν��ܵ����ݰ�������಻�ܳ���128��������HI_ERR_CIPHER_BUSY��ʱ���������ݰ�һ��Ҳ���ᱻ�����û���Ҫ������������ݰ������������ٴγ��Խ��ܡ�ע��: ���ڶ�����Ĳ�����ÿ������ʹ��HI_TEE_CIPHER_ConfigHandle���õ������������㣬ǰһ����������������������������һ���������㣬ÿ�������Ƕ�������ģ�ǰһ�κ������õĽ��Ҳ����Ӱ���һ�κ������õ��������� CNend
\param[in] hCipher cipher handle                                                                 CNcomment:CIPHER����� CNend
\param[in] pstDataPkg data package ready for cipher                                              CNcomment:�����ܵ����ݰ��� CNend
\param[in] u32DataPkgNum  number of package ready for cipher                                     CNcomment:�����ܵ����ݰ������� CNend
\retval ::HI_SUCCESS  Call this API succussful.                                                  CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized                        CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  parameter error                                            CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid                                           CNcomment:����Ƿ� CNend
\retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time  CNcomment:Ӳ����æ���޷�һ���Դ���ȫ�������ݰ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_DecryptMultiAsyn(HI_HANDLE hCipher, HI_TEE_CIPHER_DATA_S *pstDataPkg, HI_U32 u32DataPkgNum);

/**
\brief Waitting for cipher encrypt/decrypt done.
CNcomment:\brief �ȴ��ӽ������ CNend

\attention \n
This API is used to get the tag data of CCM/GCM.
\param[in] hCipher cipher handle                                                CNcomment:CIPHER����� CNend
\param[in] u32TimeOut time out with unit of  millisecond                        CNcomment:��ʱʱ�䣬��λ���롣 CNend
\retval ::HI_SUCCESS  Call this API succussful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_WaitDone(HI_HANDLE hCipher, HI_U32 u32TimeOut);


/**
\brief Get the tag data of CCM/GCM.
CNcomment:\brief ��ȡCCM/GCM��TAG���ݡ� CNend

\attention \n
This API is used to get the tag data of CCM/GCM.
\param[in] hCipher cipher handle                                                CNcomment:CIPHER����� CNend
\param[out] pu8Tag tag data of CCM/GCM                                          CNcomment:TAGָ�� CNend
\retval ::HI_SUCCESS  Call this API succussful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_GetTag(HI_HANDLE hCipher, HI_U8 *pu8Tag, HI_U32 *pu32TagLen);

/**
\brief Get the random number.
CNcomment:\brief ��ȡ������� CNend

\attention \n
This API is used to obtain the random number from the hardware.
CNcomment: ���ô˽ӿ����ڻ�ȡ������� CNend

\param[out] pu32RandomNumber Point to the random number.                                        CNcomment:�������ֵ�� CNend
\retval ::HI_SUCCESS  Call this API successful.                                                 CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                                                      CNcomment: APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_GetRandomNumber(HI_U32 *pu32RandomNumber);

/**
\brief Init the hash module, if other program is using the hash module, the API will return failure.
CNcomment:\brief ��ʼ��HASHģ�飬�����������������ʹ��HASHģ�飬����ʧ��״̬�� CNend

\attention \n
N/A

\param[in] pstHashAttr: The hash calculating structure input.                                      CNcomment:���ڼ���hash�Ľṹ����� CNend
\param[out] pHashHandle: The output hash handle.                                                CNcomment:�����hash��� CNend
\retval ::HI_SUCCESS  Call this API successful.                                                 CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                                                      CNcomment: APIϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_HashInit(HI_TEE_CIPHER_HASH_ATTS_S *pstHashAttr, HI_HANDLE *pHashHandle);

/**
\brief Calculate the hash, if the size of the data to be calculated is very big and the DDR ram is not enough, this API can calculate the data one block by one block. Attention: The input block length must be 64bytes aligned except for the last block.
CNcomment:\brief ����hashֵ�������Ҫ������������Ƚϴ󣬸ýӿڿ���ʵ��һ��blockһ��block�ļ��㣬�����������Ƚϴ������£��ڴ治������⡣ �ر�ע�⣬�������һ��block��ǰ���ÿһ������ĳ��ȶ�������64�ֽڶ��롣CNend

\attention \n
N/A

\param[in] hHashHandl:  Hash handle.                                        CNcomment:Hash����� CNend
\param[in] pu8InputData:  The input data buffer.                            CNcomment:�������ݻ��� CNend
\param[in] u32InputDataLen:  The input data length, attention: the block length input must be 64bytes aligned except the last block!            CNcomment:�������ݵĳ��ȡ���Ҫ�� �������ݿ�ĳ��ȱ�����64�ֽڶ��룬���һ��block�޴����ơ� CNend
\retval ::HI_SUCCESS  Call this API successful.                             CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                                  CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_HashUpdate(HI_HANDLE hHashHandle, HI_U8 *pu8InputData, HI_U32 u32InputDataLen);



/**
\brief Get the final hash value, after calculate all of the data, call this API to get the final hash value and close the handle.If there is some reason need to interrupt the calculation, this API should also be call to close the handle.
CNcomment:��ȡhashֵ���ڼ��������е����ݺ󣬵�������ӿڻ�ȡ���յ�hashֵ���ýӿ�ͬʱ��ر�hash���������ڼ�������У���Ҫ�жϼ��㣬Ҳ������øýӿڹر�hash����� CNend

\attention \n
N/A

\param[in] hHashHandle:  Hash handle.                                       CNcomment:Hash����� CNend
\param[out] pu8OutputHash:  The final output hash value.                    CNcomment:�����hashֵ�� CNend

\retval ::HI_SUCCESS  Call this API successful.                             CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_HashFinal(HI_HANDLE hHashHandle, HI_U8 *pu8OutputHash);

/**
\brief RSA encryption a plaintext with a RSA public key.
CNcomment:ʹ��RSA��Կ����һ�����ġ� CNend

\attention \n
N/A

\param[in] pstRsaEnc:   encryption struct.                                   CNcomment:�������Խṹ�塣 CNend
\param[in] pu8Input��   input data to be encryption                          CNcomment: �����ܵ����ݡ� CNend
\param[out] u32InLen:   length of input data to be encryption                CNcomment: �����ܵ����ݳ��ȡ� CNend
\param[out] pu8Output�� output data to be encryption                         CNcomment: ���ܽ�����ݡ� CNend
\param[out] pu32OutLen: length of output data to be encryption               CNcomment: ���ܽ�������ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_RsaPublicEncrypt(HI_TEE_CIPHER_RSA_PUB_ENC_S *pstRsaEnc,
                                  HI_U8 *pu8Input, HI_U32 u32InLen,
                                  HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA decryption a ciphertext with a RSA private key.
CNcomment:ʹ��RSA˽Կ����һ�����ġ� CNend

\attention \n
N/A

\param[in] pstRsaDec:   decryption struct.                                   CNcomment: ��Կ�������Խṹ�塣 CNend
\param[in] pu8Input��   input data to be decryption                          CNcomment: �����ܵ����ݡ� CNend
\param[out] u32InLen:   length of input data to be decryption                CNcomment: �����ܵ����ݳ��ȡ� CNend
\param[out] pu8Output�� output data to be decryption                         CNcomment: ���ܽ�����ݡ� CNend
\param[out] pu32OutLen: length of output data to be decryption               CNcomment: ���ܽ�������ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_RsaPrivateDecrypt(HI_TEE_CIPHER_RSA_PRI_ENC_S *pstRsaDec,
                                    HI_U8 *pu8Input, HI_U32 u32InLen,
                                    HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA encryption a plaintext with a RSA private key.
CNcomment:ʹ��RSA˽Կ����һ�����ġ� CNend

\attention \n
N/A

\param[in] pstRsaSign:   encryption struct.                                  CNcomment:�������Խṹ�塣 CNend
\param[in] pu8Input��   input data to be encryption                          CNcomment: �����ܵ����ݡ� CNend
\param[out] u32InLen:   length of input data to be encryption                CNcomment: �����ܵ����ݳ��ȡ� CNend
\param[out] pu8Output�� output data to be encryption                         CNcomment: ���ܽ�����ݡ� CNend
\param[out] pu32OutLen: length of output data to be encryption               CNcomment: ���ܽ�������ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_RsaPrivateEncrypt(HI_TEE_CIPHER_RSA_PRI_ENC_S *pstRsaEnc,
                                    HI_U8 *pu8Input, HI_U32 u32InLen,
                                    HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA decryption a ciphertext with a RSA public key.
CNcomment:ʹ��RSA��Կ����һ�����ġ� CNend

\attention \n
N/A

\param[in] pstRsaVerify:   decryption struct.                                CNcomment: �������Խṹ�塣 CNend
\param[in] pu8Input��   input data to be decryption                          CNcomment: �����ܵ����ݡ� CNend
\param[out] u32InLen:   length of input data to be decryption                CNcomment: �����ܵ����ݳ��ȡ� CNend
\param[out] pu8Output�� output data to be decryption                         CNcomment: ���ܽ�����ݡ� CNend
\param[out] pu32OutLen: length of output data to be decryption               CNcomment: ���ܽ�������ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_RsaPublicDecrypt(HI_TEE_CIPHER_RSA_PUB_ENC_S *pstRsaDec,
                               HI_U8 *pu8Input, HI_U32 u32InLen,
                               HI_U8 *pu8Output, HI_U32 *pu32OutLen);

/**
\brief RSA signature a context with appendix, where a signer��s RSA private key is used.
CNcomment:ʹ��RSA˽Կǩ��һ���ı��� CNend

\attention \n
N/A

\param[in] pstRsaSign:      signature struct.                                    CNcomment: ǩ�����Խṹ�塣 CNend
\param[in] pu8Input��       input context to be signature��maybe null            CNcomment: ��ǩ��������, ���pu8HashData��Ϊ�գ����ָ�տ���Ϊ�ա� CNend
\param[in] u32InLen:        length of input context to be signature              CNcomment: ��ǩ�������ݳ��ȡ� CNend
\param[in] pu8HashData��    hash value of context,if NULL, let pu8HashData = Hash(context) automatically   Ncomment: ��ǩ���ı���HASHժҪ�����Ϊ�գ����Զ������ı���HASHժҪ�� CNend
\param[out] pu8OutSign��    output message of signature                          CNcomment: ǩ����Ϣ�� CNend
\param[out] pu32OutSignLen: length of message of signature                       CNcomment: ǩ����Ϣ�����ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_RsaSign(HI_TEE_CIPHER_RSA_SIGN_S *pstRsaSign,
                             HI_U8 *pu8InData, HI_U32 u32InDataLen,
                             HI_U8 *pu8HashData,
                             HI_U8 *pu8OutSign, HI_U32 *pu32OutSignLen);

/**
\brief RSA signature verification a context with appendix, where a signer��s RSA public key is used.
CNcomment:ʹ��RSA��Կǩ����֤һ���ı��� CNend

\attention \n
N/A

\param[in] pstRsaVerify:    signature verification struct.                         CNcomment: ǩ����֤���Խṹ�塣 CNend
\param[in] pu8Input��       input context to be signature verification, maybe null CNcomment: ��ǩ����֤������, ���pu8HashData��Ϊ�գ����ָ�տ���Ϊ�ա� CNend
\param[in] u32InLen:        length of input context to be signature                CNcomment: ��ǩ����֤�����ݳ��ȡ� CNend
\param[in] pu8HashData��    hash value of context,if NULL, let pu8HashData = Hash(context) automatically   Ncomment: ��ǩ���ı���HASHժҪ�����Ϊ�գ����Զ������ı���HASHժҪ�� CNend
\param[in] pu8InSign��      message of signature                                 CNcomment: ǩ����Ϣ�� CNend
\param[in] pu32InSignLen:   length of message of signature                       CNcomment: ǩ����Ϣ�����ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API successful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_RsaVerify(HI_TEE_CIPHER_RSA_VERIFY_S *pstRsaVerify,
                               HI_U8 *pu8InData, HI_U32 u32InDataLen,
                               HI_U8 *pu8HashData,
                               HI_U8 *pu8InSign, HI_U32 u32InSignLen);

                               /**
\brief SM2 signature a context with appendix, where a signer��s SM2 private key is used.
CNcomment:ʹ��SM1˽Կǩ��һ���ı��� CNend

\attention \n
N/A

\param[in] pstSm2Sign:      signature struct.                                    CNcomment: ǩ�����Խṹ�塣 CNend
\param[in] pu8Msg:          input context to be signature��maybe null            CNcomment: ��ǩ��������, ���pu8HashData��Ϊ�գ����ָ�տ���Ϊ�ա� CNend
\param[in] u32MsgLen:       length of input context to be signature              CNcomment: ��ǩ�������ݳ��ȡ� CNend
\param[out] pu8R��          output message of signature                          CNcomment: ǩ�������Rֵ�� CNend
\param[out] pu8S:           length of message of signature                       CNcomment: ǩ�������Sֵ�� CNend

\retval ::HI_SUCCESS  Call this API succussful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Sm2Sign(HI_TEE_CIPHER_SM2_SIGN_S *pstSm2Sign, HI_U8 *pu8Msg, HI_U32 u32MsgLen, HI_U8 *pu8R, HI_U8 *pu8S);

/**
\brief SM2 signature verification a context with appendix, where a signer��s SM2 public key is used.
CNcomment:ʹ��SM2��Կǩ����֤һ���ı��� CNend

\attention \n
N/A

\param[in] pstSm2Verify:    signature verification struct.                         CNcomment: ǩ����֤���Խṹ�塣 CNend
\param[in] pu8Msg��         input context to be signature verification��maybe null CNcomment: ��ǩ����֤������, ���pu8HashData��Ϊ�գ����ָ�տ���Ϊ�ա� CNend
\param[in] u32MsgLen:       length of input context to be signature                CNcomment: ��ǩ����֤�����ݳ��ȡ� CNend
\param[in] pu8R��           output message of signature                            CNcomment: ǩ�������Rֵ�� CNend
\param[in] pu8S:            length of message of signature                         CNcomment: ǩ�������Sֵ�� CNend

\retval ::HI_SUCCESS  Call this API succussful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Sm2Verify(HI_TEE_CIPHER_SM2_VERIFY_S *pstSm2Verify, HI_U8 *pu8Msg, HI_U32 u32MsgLen, HI_U8 *pu8R, HI_U8 *pu8S);

/**
\brief SM2 encryption a plaintext with a RSA public key.
CNcomment:ʹ��SM2��Կ����һ�����ġ� CNend

\attention \n
N/A

\param[in] pstSm2Enc:   encryption struct.                                   CNcomment: �������Խṹ�塣 CNend
\param[in] pu8Msg��     input data to be encryption                          CNcomment: �����ܵ����ݡ� CNend
\param[in] u32MsgLen:   length of input data to be encryption                CNcomment: �����ܵ����ݳ��ȡ� CNend
\param[out] pu8C��      output data to be encryption                         CNcomment: ���ܽ�����ݡ� CNend
\param[out] pu32Clen:   length of output data to be encryption               CNcomment: ���ܽ�������ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API succussful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Sm2Encrypt(HI_TEE_CIPHER_SM2_ENC_S *pstSm2Enc,  HI_U8 *pu8Msg, HI_U32 u32MsgLen, HI_U8 *pu8C, HI_U32 *pu32Clen);

/**
\brief RSA decryption a ciphertext with a SM2 private key.
CNcomment:ʹ��SM2˽Կ����һ�����ġ� CNend

\attention \n
N/A

\param[in] pstSm2Dec:   decryption struct.                                   CNcomment: ��Կ�������Խṹ�塣 CNend
\param[in] pu8C��       input data to be decryption                          CNcomment: �����ܵ����ݡ� CNend
\param[out] u32Clen:    length of input data to be decryption                CNcomment: �����ܵ����ݳ��ȡ� CNend
\param[out] pu8Msg��    output data to be decryption                         CNcomment: ���ܽ�����ݡ� CNend
\param[out] pu32MsgLen: length of output data to be decryption               CNcomment: ���ܽ�������ݳ��ȡ� CNend

\retval ::HI_SUCCESS  Call this API succussful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Sm2Decrypt(HI_TEE_CIPHER_SM2_DEC_S *pstSm2Dec, HI_U8 *pu8C, HI_U32 u32Clen, HI_U8 *pu8Msg, HI_U32 *pu32MsgLen);

/**
\brief Generate a SM2 key pair.
CNcomment:����һ��SM2��Կ�ԡ� CNend

\attention \n
N/A

\param[out] pstSm2Key:   key pair struct.                               CNcomment: SM2��Կ�ԡ� CNend

\retval ::HI_SUCCESS  Call this API succussful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_Sm2GenKey(HI_TEE_CIPHER_SM2_KEY_S *pstSm2Key);

/** @} */  /** <!-- ==== API declaration end ==== */

/**
\brief Calculate a key of PBKDF2
CNcomment: ����PBKDF2��Կ CNend

\attention \n
N/A

\param[in] pstInfo:  The PBKDF2 key calculating structure input.     CNcomment:PBKDF2��Կ����ṹ�� CNend
\param[out] output:  The final output hash value.                    CNcomment:�����PBKDF2��Կ��   CNend

\retval ::HI_SUCCESS  Call this API successful.                      CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                           CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_PBKDF2(HI_TEE_CIPHER_PBKDF2_S *pstInfo, HI_U8 *output);

/**
\brief CENC decryption a ciphertext.
CNcomment: CENC��ʽ����һ�����ġ� CNend

\attention \n
This API is used to perform decryption ciphertext base on CENC format.
CNcomment:ʹ��CIPHER����CENC��ʽ���ܲ����� CNend
\param[in] hCipher Cipher handle                                              CNcomment:CIPHER��� CNend
\param[in] u8Key key for cipher decryption                                    CNcomment:CIPHER ������Կ CNend
\param[in] u8IV  iv for cipher decryption                                     CNcomment:CIPHER ����IV CNend
\param[in] u32NonSecInputPhyAddr non-secure Physical address of the source data    CNcomment:Դ���ݷǰ�ȫ�����ַ CNend
\param[in] u32SecOutputPhyAddr secure Physical address of the target data          CNcomment:Ŀ�����ݰ�ȫ�����ַ CNend
\param[in] u32ByteLength   Length of the decrypted data                         CNcomment:�������ݳ��� CNend
\param[in] u32FirstEncrypt offset of the first encrypt block data               CNcomment:��һ���������ݵ�ƫ�Ƶ�ַ��0~15 CNend
\retval ::HI_SUCCESS  Call this API succussful.                                 CNcomment:APIϵͳ���óɹ� CNend
\retval ::HI_FAILURE  Call this API fails.                                      CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.         CNcomment:CIPHER�豸δ��ʼ�� CNend
\retval ::HI_ERR_CIPHER_INVALID_PARA  The parameter is invalid.                 CNcomment:�������� CNend
\retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.                  CNcomment:����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_CENCDecrypt(HI_HANDLE hCipher, HI_U8 *pu8Key, HI_U8 *pu8IV,
                               HI_U32 u32NonSecInPhyAddr, HI_U32 u32SecOutPhyAddr, HI_U32 u32ByteLength,
                               HI_U32 u32FirstEncryptOffset,
                               HI_TEE_CIPHER_SUBSAMPLE_S *pstSubSample, HI_U32 u32SubsampleNum);

/**
\brief Generate Diffie-Hellman public/private key pair from g and p parameters. The public key is equal to g^x mod p, where x is random number considered as the private key.
CNcomment: ����DH��˽��Կ�ԡ� CNend

\attention \n
N/A

\param[in] pu8G: Buffer containing the DH generator g used for the operation. The caller ensures it is padded with leading zeros if the effective size of this key is smaller than the u32KeySize.  CNcomment: DH��g���������Ȳ���Key�Ĵ�С��ǰ�油0�� CNend
\param[in] pu8P: Buffer containing the DH generator p used for the operation. The caller ensures it is padded with leading zeros if the effective size of this key is smaller than the u32KeySize.  CNcomment: DH��p���������Ȳ���Key�Ĵ�С��ǰ�油0�� CNend
\param[in] pu8InputPrivKey: Buffer containing an optional input private key from which the public has to be generated.  The caller ensures it is padded with leading zeros if the effective size of this key is smaller than the u32KeySize.  If no private key is provided as input (\c pu8InputPrivKey=NULL), function generates a random private key and stores it in pu8OutputPrivKey this buffer. CNcomment: DH��˽Կ�����Ȳ���Key�Ĵ�С��ǰ�油0, ���Ϊ��ָ�룬�ú���������һ��˽Կ�ŵ�pu8OutputPrivKey�С� CNend
\param[out] pu8OutputPrivKey: Buffer where to write the generated private key, in case no private key is provided as input (pu8InputPrivKey==NULL). It must be padded with leading zeros if the effective size of the private key is smaller than the buffer size. CNcomment: DH��˽Կ�����Ȳ���Key�Ĵ�С��ǰ�油0, ���pu8InputPrivKeyΪ��ָ�룬�ú���������һ��˽Կ�ŵ����buffer�С� CNend
\param[in] u32KeySize: DH key size.                       CNcomment: DH��Կ����. CNend
\retval ::HI_SUCCESS  Call this API succussful.           CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_DhGenKey(HI_U8 *pu8G, HI_U8 *pu8P, HI_U8 *pu8InputPrivKey,
                                   HI_U8 *pu8OutputPrivKey, HI_U8 *pu8PubKey,
                                   HI_U32 u32KeySize);

/**
\brief Compute ECDH shared secret key. This key corresponds to the X coordinates of the computed P point.
CNcomment: ����ECC DH������Կ�� CNend

\attention \n
N/A

\param[in] pstParams:  Elliptic curve domain parameters. The caller is in charge padding each buffer with leading zeros if the effective size of the domain parameter conveyed is smaller than pstParams->keySize.  CNcomment: ECC��Բ���߲��������Ȳ���Key�Ĵ�С��ǰ�油0. CNend
\param[in] pu8PrivKey: Buffer containing the ECDH private key. The caller ensures it is padded with leading zeros if the effective size of this key is smaller than the u32KeySize.                       CNcomment: ECDH˽Կ�����Ȳ���Key�Ĵ�С��ǰ�油0�� CNend
\param[in] pu8OtherPubKeyX: Buffer containing the X coordinates of other peer's ECDH public key. It is padded by the caller with leading zeros if the effective size of the public key X coordinates is smaller than the buffer size.  CNcomment: �Է���ECDH��Կ��X���꣬���Ȳ���Key�Ĵ�С��ǰ�油0�� CNend
\param[in] pu8OtherPubKeyY: Buffer containing the Y coordinates of other peer's ECDH public key. It is padded by the caller with leading zeros if the effective size of the public key Y coordinates is smaller than the buffer size.  CNcomment: �Է���ECDH��Կ��Y���꣬���Ȳ���Key�Ĵ�С��ǰ�油0�� CNend
\param[out] pu8SharedSecret:  Buffer where to write the computed shared secret. The caller ensures it is padded with leading zeros if the effective size of this key is smaller than the u32KeySize.    CNcomment: ECDH������Կ�����Ȳ���Key�Ĵ�С��ǰ�油0�� CNend
\retval ::HI_SUCCESS  Call this API succussful.                         CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails.                              CNcomment:APIϵͳ����ʧ�ܡ� CNend

\see \n
N/A
*/
HI_S32 HI_TEE_CIPHER_DhComputeKey(HI_U8 * pu8P, HI_U8 *pu8PrivKey, HI_U8 *pu8OtherPubKey,
                                  HI_U8 *pu8SharedSecret, HI_U32 u32KeySize);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_TEE_CIPHER_H__ */

