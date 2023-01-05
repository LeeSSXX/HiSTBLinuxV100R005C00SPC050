/*******************************************************************************
* All rights reserved, Copyright (C) huawei LIMITED 2012
*------------------------------------------------------------------------------
* File Name   : tee_common.h
* Description : tee common definitions, must be consistent with teec definitions
* Author      : yaodongdong@huawei.com
* Date        : 2012.12.31
* Notes       :
*
*------------------------------------------------------------------------------
* Modifications:
*   Date        Author          Modifications
*******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by HUAWEI on an
 * AS-IS basis. Anyone receiving this source code is licensed under HUAWEI
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the HUAWEI copyright notice and this paragraph in
 * the transferred software.
*******************************************************************************/

/**
* @file tee_common.h
*
* Copyright(C), 2008-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED.
*
* ������ServiceID��CommandID����ͷ�ļ�
*/

/** @defgroup TEE_API ��ȫ��ӿ� */
/** @defgroup TEE_COMMON_DATA �������ݶ���
* @ingroup TEE_API
*/

#ifndef __TEE_COMMON_H_
#define __TEE_COMMON_H_

#define CIPHER_ENCRYPT 0
#define CIPHER_DECRYPT 1

#define MD5_OUTPUT_LEN 16
#define SHA1_OUTPUT_LEN 20
#define SHA224_OUTPUT_LEN 28
#define SHA256_OUTPUT_LEN 32
#define SHA384_OUTPUT_LEN 48
#define SHA512_OUTPUT_LEN 64
#define HMAC_KEY_LEN 16
#define HMAC_DATA_LEN 50
#define HMAC_OUTPUT_LEN 16
#define AES_128_CBC_LEN 16
#define AES_128_ECB_LEN 16
#define AES_128_CTR_LEN 16
#define AES_128_XTS_LEN 16
#define DES_ECB_LEN 8
#define DES_CBC_LEN 8
#define DES3_CBC_LEN 8
#define DES3_ECB_LEN 8
#define IGNORE_PARAM  0xff
#if 1
/**
 * @ingroup  TEE_COMMON_DATA
 * ��Կ����ʱ����ṹ������ֽ�
 *
*/
#define TEE_DK_MAX_SIZE_OF_OTHER_INFO  64 /*bytes*/
#endif

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����Global
 */
#define TEE_SERVICE_GLOBAL \
{ \
    0x00000000, \
    0x0000, \
    0x0000, \
    { \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
    } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����Echo
 */
#define TEE_SERVICE_ECHO \
{ \
    0x01010101, \
    0x0101, \
    0x0101, \
    { \
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 \
    } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ�洢����
 */
#define TEE_SERVICE_STORAGE \
{ \
    0x02020202, \
    0x0202, \
    0x0202, \
    { \
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02 \
    } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����UT����
 */
#define TEE_SERVICE_UT \
{ \
    0x03030303, \
    0x0303, \
    0x0303, \
    { \
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03 \
    } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * �ӽ��ܷ���
 */
#define TEE_SERVICE_CRYPT \
{ \
    0x04040404, \
    0x0404, \
    0x0404, \
    { \
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 \
    } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * efuse��д����
 */
#define TEE_SERVICE_EFUSE \
{ \
    0x05050505, \
    0x0505, \
    0x0505, \
    { \
        0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05 \
    } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * HDCP��ȫ�洢����
 */
#define TEE_SERVICE_HDCP \
{ \
    0x06060606, \
    0x0606, \
    0x0606, \
    { \
        0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06 \
    } \
}


/**
 * @ingroup  TEE_COMMON_DATA
 *
 * KeyMaster����
 */
#define TEE_SERVICE_KEYMASTER \
{ \
     0x07070707, \
     0x0707, \
     0x0707, \
     { \
         0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07 \
     } \
}

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * secboot����
 */
#define TEE_SERVICE_SECBOOT \
{ \
     0x08080808, \
     0x0808, \
     0x0808, \
     { \
         0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 \
     } \
}

#if 0
#define TEE_SERVICE_IMAXCRYPTO \
{ \
     0x09090909, \
     0x0909, \
     0x0909, \
     { \
         0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09 \
     } \
}
#endif
#define TEE_SERVICE_IMAXCRYPTO \
{ \
     0xd2b09738, \
     0x6ff6, \
     0x40d3, \
     { \
         0x9f, 0xde, 0x54, 0x71, 0x9c, 0xde, 0xc3, 0x59 \
     } \
}

#define TEE_SERVICE_GATEKEEPER \
{ \
    0x0B0B0B0B, \
    0x0B0B, \
    0x0B0B, \
    { \
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B \
    } \
}

#define TEE_SERVICE_SSA \
{ \
    0x999286b9, \
    0x54da, \
    0x4235, \
    { \
        0x9e, 0x77, 0x96, 0xe8, 0x1f, 0xea, 0x1e, 0xe4 \
    } \
}
/* 004555da-864e-4a8e-8540-285818e7e529 */
#define TEE_SERVICE_RPMB \
{ \
    0x004555da, \
    0x864e, \
    0x4a8e, \
    { \
        0x85, 0x40, 0x28, 0x58, 0x18, 0xe7, 0xe5, 0x29 \
    } \
}


#define TEE_SERVICE_HISI_DEMO \
{ \
	0x8f194022, \
	0xc9a2, \
	0x11e6, \
	{ \
		0x9d, 0x9d, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01 \
	} \
}



/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����imax֧�ֵ�����ID
 */
enum SVC_IMAX_CMD_ID {
	IMAX_CMD_ID_INVALID = 0x0,         /**< IMAX Task ��ЧID*/
	IMAX_CMD_ID_AES_CBC_256,           /**< IMAX AES�ԳƼӽ���*/
	IMAX_CMD_ID_GKEYPAIR_GPUB,         /**< IMAX ���ɹ�˽Կ�Ժ͹�Կ*/
	IMAX_CMD_ID_MESSAGE_DECRYPT,       /**< IMAX ��Ϣ����*/
	IMAX_CMD_ID_MESSAGE_AESKEY_DECRYPT,
	IMAX_CMD_ID_CLOUND_KEY_STORE,      /**< IMAX �ƶ���Կ��֤�洢*/
	IMAX_CMD_ID_CLOUND_DATA_CIPHER,   /**< IMAX .......*/
	IMAX_CMD_ID_CLOUND_DATA_AES_CBC_NOPADDING_CIPHER, /**IMAX CIPHER*/
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * Notification task
 */
#define TEE_SERVICE_REET \
{ \
     0x0A0A0A0A, \
     0x0A0A, \
     0x0A0A, \
     { \
         0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A \
     } \
}

#define TEE_SERVICE_MDPP \
{ \
    0x41d21d0f, \
    0x9757, \
    0x41f1, \
    { \
        0x87, 0x06, 0xb3, 0xba, 0xa3, 0x5f, 0xe8, 0x2f \
    } \
}

#define TEE_SERVICE_VDEC \
{ \
     0x4A1A44E8, \
     0x3ED7, \
     0x4E00, \
     { \
         0x9C, 0x1E, 0x02, 0xA2, 0x57, 0x7E, 0xA2, 0x4A \
     } \
}


//79b77788-9789-4a7a-a2be-b60155eef5f4
#define TEE_SEVICE_VIDEO_TEST \
{ \
    0x79b77788, \
    0x9789, \
    0x4a7a, \
	{ \
		0xa2, 0xbe, 0xb6, 0x01, 0x55, 0xee, 0xf5, 0xf4 \
	} \
}
//e7ed1f64-4687-41da-96dc-cbe4f27c838f
#define TEE_SERVICE_ANTIROOT \
{ \
    0xE7ED1F64, \
    0x4687, \
    0x41DA, \
    { \
        0x96, 0xDC, 0xCB, 0xE4, 0xF2, 0x7C, 0x83, 0x8F \
    } \
}
/**
 * @ingroup  TEE_HI_VCODEC
 *
 * hivcodec
 * 528822b7-fc78-466b-b57e-62093d6034a7
 */
#define TEE_SERVICE_HIVCODEC \
{ \
     0x528822b7, \
     0xfc78, \
     0x466b, \
     { \
         0xb5, 0x7e, 0x62, 0x09, 0x3d, 0x60, 0x34, 0xa7 \
     } \
}


/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����secboot֧�ֵ�����ID
 */
enum SVC_SECBOOT_CMD_ID {
	SECBOOT_CMD_ID_INVALID = 0x0,      /**< Secboot Task ��ЧID*/
	SECBOOT_CMD_ID_COPY_VRL,           /**< Secboot Task ����VRL*/
	SECBOOT_CMD_ID_COPY_DATA,          /**< Secboot Task ��������*/
	SECBOOT_CMD_ID_VERIFY_DATA,        /**< Secboot Task ��֤*/
	SECBOOT_CMD_ID_RESET_IMAGE,        /**< Secboot Task ��λSoC����*/
	SECBOOT_CMD_ID_COPY_VRL_TYPE,     /**< Secboot Task ����VRL��������SoC Type*/
	SECBOOT_CMD_ID_COPY_DATA_TYPE,     /**< Secboot Task ��������,������SoC Type*/
	SECBOOT_CMD_ID_VERIFY_DATA_TYPE,    /**< Secboot Task У�飬������SoC Type��У��ɹ��⸴λSoC*/
	SECBOOT_CMD_ID_VERIFY_DATA_TYPE_LOCAL, /**< Secboot Taskԭ��У�飬������SoC Type,У��ɹ��⸴λSoC*/
	SECBOOT_CMD_ID_BSP_MODEM_CALL,           /**< Secboot Task ִ�ж�Ӧ����*/
	SECBOOT_CMD_ID_BSP_MODULE_VERIFY,        /**< Secboot Task modem moduleУ�麯��*/
};

/**
 * @ingroup TEE_COMMON_DATA
 *
 * ��ȫ����secboot֧�ֵľ�������
 */
enum SVC_SECBOOT_IMG_TYPE {
	MODEM,
	HIFI,
	DSP,
	XDSP,
	TAS,
	WAS,
	CAS,
	MODEM_DTB,
	ISP,
	SOC_MAX
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����keymaster֧�ֵ�����ID
 */
enum SVC_KEYMASTER_CMD_ID {
	KM_CMD_ID_INVALID = 0x0,       /**< KeyMaster Task ��ЧID*/
	KM_CMD_ID_CONFIGURE,
	KM_CMD_ID_GENERATE_KEY,
	KM_CMD_ID_GET_KEY_CHARACTER,
	KM_CMD_ID_IMPORT_KEY,
	KM_CMD_ID_EXPORT_KEY,
	KM_CMD_ID_ATTEST_KEY,
	KM_CMD_ID_UPGRADE,
	KM_CMD_ID_BEGIN,
	KM_CMD_ID_UPDATE,
	KM_CMD_ID_FINISH,
	KM_CMD_ID_ABORT,
	KM_CMD_ID_STORE_KB,
	KM_CMD_ID_VERIFY_KB
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����gatekeeper֧�ֵ�����ID
 */
enum SVC_GATEKEEPER_CMD_ID {
	GK_CMD_ID_INVALID = 0x0,
	GK_CMD_ID_ENROLL,
	GK_CMD_ID_VERIFY,
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����global֧�ֵ�����ID
 */
enum global_service_cmd_id {
	GLOBAL_CMD_ID_INVALID = 0x0,    /**< Global Task ��ЧID*/
	GLOBAL_CMD_ID_BOOT_ACK = 0x1,         /**< Global Task ����Ӧ��*/
	GLOBAL_CMD_ID_OPEN_SESSION = 0x2,     /**< Global Task ��Session*/
	GLOBAL_CMD_ID_CLOSE_SESSION = 0x3,    /**< Global Task �ر�Session*/
	GLOBAL_CMD_ID_LOAD_SECURE_APP = 0x4,  /**< Global Task ��̬���ذ�ȫӦ��*/
	GLOBAL_CMD_ID_NEED_LOAD_APP = 0x5,    /**< Global Task �ж��Ƿ�����Ҫ���ذ�ȫӦ��*/
	GLOBAL_CMD_ID_REGISTER_AGENT = 0x6,   /**< Global Task ע�����*/
	GLOBAL_CMD_ID_UNREGISTER_AGENT = 0x7, /**< Global Task ע������*/
	GLOBAL_CMD_ID_REGISTER_NOTIFY_MEMORY = 0x8,   /**< Global Task ע���첽���û�����*/
	GLOBAL_CMD_ID_UNREGISTER_NOTIFY_MEMORY = 0x9, /**< Global Task ע���첽���û�����*/
	GLOBAL_CMD_ID_INIT_CONTENT_PATH = 0xa,      /**< Global Task��ʼ��content path*/
	GLOBAL_CMD_ID_TERMINATE_CONTENT_PATH= 0xb,   /**< Global Task�ͷ�content path*/
	GLOBAL_CMD_ID_ALLOC_EXCEPTION_MEM = 0xc,  /**< Global Task �����쳣��Ϣ����ռ�*/
	GLOBAL_CMD_ID_TEE_TIME = 0xd,         /**< Global Task ��ȡ��ȫOS��ʱ�� */
	GLOBAL_CMD_ID_TEE_INFO = 0xe,        /** tlogcat ��ȡ��ȫOS  ״̬ */
	GLOBAL_CMD_ID_REGISTER_RDR_MEM = 0xf, /**ע��RDR �ڴ�*/
	GLOBAL_CMD_ID_KILL_TASK = 0x10,       /** Global Task Kill session */
	GLOBAL_CMD_ID_TUI_EXCEPTION = 0x11,       /** TUI�쳣�¼����� */
	GLOBAL_CMD_ID_ADJUST_TIME = 0x12,       /** TIMEʱ��У������ */
	GLOBAL_CMD_ID_SET_CA_HASH = 0x13,   /*set ca hash info�������ڼ���CA��ݵ�hash*/
	GLOBAL_CMD_ID_SET_BUILD_VERSION = 0x14, /* set the Android's build version */
	GLOBAL_CMD_ID_REGISTER_TTF_MEM = 0x15,
	GLOBAL_CMD_ID_REGISTER_MAILBOX = 0x17, /** register mailbox memory pool */
	GLOBAL_CMD_ID_UNKNOWN         = 0x7FFFFFFE, /**< Global Task δ֪ID*/
	GLOBAL_CMD_ID_MAX             = 0x7FFFFFFF  /**< Global Task ���ID*/
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����echo֧�ֵ�����ID
 */
enum echo_service_cmd_id {
	ECHO_CMD_ID_INVALID = 0x0,     /**< Echo Task ��ЧID*/
	ECHO_CMD_ID_SEND_CMD,           /**< Echo Task��������*/
	ECHO_CMD_ID_EFUSE_READ,
	ECHO_CMD_ID_EFUSE_WRITE,
	ECHO_CMD_ID_UNKNOWN         = 0x7FFFFFFE,   /**< Echo Task δ֪ID*/
	ECHO_CMD_ID_MAX             = 0x7FFFFFFF    /**< Echo Task ���ID*/
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����CRYPTO֧�ֵ�����ID, �����ԳƼӽ��ܡ��ǶԳƼӽ��ܡ�ժҪHMAC��\n
 * ע��:�Գ��㷨�е�nopadģʽ��ҪTA(Trusted Application������Ӧ��)�������
 */
enum crypt_service_cmd_id {
	CRYPT_CMD_ID_INVALID = 0x10,            /**< ��ЧID */
	CRYPT_CMD_ID_ENCRYPT,                   /**< ���� */
	CRYPT_CMD_ID_DECRYPT,                   /**< ���� */
	CRYPT_CMD_ID_MD5,                       /**< ժҪ�㷨MD5 */
	CRYPT_CMD_ID_SHA1,                      /**< ժҪ�㷨SHA1 */
	CRYPT_CMD_ID_SHA224,                    /**< ժҪ�㷨SHA224 */
	CRYPT_CMD_ID_SHA256,                    /**< ժҪ�㷨SHA256 */
	CRYPT_CMD_ID_SHA384,                    /**< ժҪ�㷨SHA384 */
	CRYPT_CMD_ID_SHA512,                    /**< ժҪ�㷨SHA512 */
	CRYPT_CMD_ID_HMAC_MD5,                  /**< HMAC MD5 */
	CRYPT_CMD_ID_HMAC_SHA1,                 /**< HMAC SHA1 */
	CRYPT_CMD_ID_HMAC_SHA224,               /**< HMAC SHA224 */
	CRYPT_CMD_ID_HMAC_SHA256,               /**< HMAC SHA256 */
	CRYPT_CMD_ID_HMAC_SHA384,               /**< HMAC SHA384 */
	CRYPT_CMD_ID_HMAC_SHA512,               /**< HMAC SHA512 */
	CRYPT_CMD_ID_CIPHER_AES_CBC,        /**< �ԳƼӽ���AES 128bits��Կ CBCģʽ */
	CRYPT_CMD_ID_CIPHER_AES_CBC_CTS,    /**< �ԳƼӽ���AES 128bits��Կ CBC_CTSģʽ */
	CRYPT_CMD_ID_CIPHER_AES_ECB,        /**< �ԳƼӽ���AES 128bits��Կ ECBģʽ */
	CRYPT_CMD_ID_CIPHER_AES_ECB_PKCS5,
	CRYPT_CMD_ID_CIPHER_AES_CBC_PKCS5,
	CRYPT_CMD_ID_CIPHER_AES_CTR,        /**< �ԳƼӽ���AES 128bits��Կ CTRģʽ */
	CRYPT_CMD_ID_CIPHER_AES_CBC_MAC,    /**< �ԳƼӽ���AES 128bits��Կ CBC_MACģʽ */
	CRYPT_CMD_ID_CIPHER_AES_XCBC_MAC,   /**< �ԳƼӽ���AES 128bits��Կ XCBC_MACģʽ */
	CRYPT_CMD_ID_CIPHER_AES_CMAC,       /**< �ԳƼӽ���AES 128bits��Կ CMACģʽ */
	CRYPT_CMD_ID_CIPHER_AES_CCM,        /**< �ԳƼӽ���AES 128bits��Կ CCMģʽ */
	CRYPT_CMD_ID_CIPHER_AES_XTS,        /**< �ԳƼӽ���AES 128bits��Կ XTSģʽ */
	CRYPT_CMD_ID_CIPHER_DES_ECB,            /**< �ԳƼӽ���DES ECBģʽ */
	CRYPT_CMD_ID_CIPHER_DES_CBC,            /**< �ԳƼӽ���DES CBCģʽ */
	CRYPT_CMD_ID_CIPHER_DES3_ECB,           /**< �ԳƼӽ���DES3 ECBģʽ */
	CRYPT_CMD_ID_CIPHER_DES3_CBC,           /**< �ԳƼӽ���DES3 CBCģʽ */
	CRYPT_CMD_ID_CIPHER_RND,                /**< �����ģʽ */
	CRYPT_CMD_ID_CIPHER_DK,                 /**< ��Կ����ģʽ */
	CRYPT_CMD_ID_RSAES_PKCS1_V1_5,          /**< �ǶԳƼӽ���PKCS1_V1_5ģʽ */
	CRYPT_CMD_ID_RSAES_PKCS1_OAEP_MGF1_SHA1,/**< �ǶԳƼӽ���OAEP_SHA1ģʽ */
	CRYPT_CMD_ID_RSAES_PKCS1_OAEP_MGF1_SHA224,/**< �ǶԳƼӽ���OAEP_SHA224ģʽ */
	CRYPT_CMD_ID_RSAES_PKCS1_OAEP_MGF1_SHA256,/**< �ǶԳƼӽ���OAEP_SHA256ģʽ */
	CRYPT_CMD_ID_RSAES_PKCS1_OAEP_MGF1_SHA384,/**< �ǶԳƼӽ���OAEP_SHA384ģʽ */
	CRYPT_CMD_ID_RSAES_PKCS1_OAEP_MGF1_SHA512,/**< �ǶԳƼӽ���OAEP_SHA512ģʽ */
	CRYPT_CMD_ID_RSA_NOPAD,                 /**< �ǶԳƼӽ��������ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_V1_5_MD5,     /**< �ǶԳ�ǩ����֤PKCS1_V1_5_MD5ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_V1_5_SHA1,    /**< �ǶԳ�ǩ����֤PKCS1_V1_5_SHA1ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_V1_5_SHA224,  /**< �ǶԳ�ǩ����֤PKCS1_V1_5_SHA224ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_V1_5_SHA256,  /**< �ǶԳ�ǩ����֤PKCS1_V1_5_SHA256ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_V1_5_SHA384,  /**< �ǶԳ�ǩ����֤PKCS1_V1_5_SHA384ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_V1_5_SHA512,  /**< �ǶԳ�ǩ����֤PKCS1_V1_5_SHA512ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_PSS_MGF1_SHA1,/**< �ǶԳ�ǩ����֤PSS_MGF1_SHA1ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_PSS_MGF1_SHA224,/**< �ǶԳ�ǩ����֤PSS_MGF1_SHA224ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_PSS_MGF1_SHA256,/**< �ǶԳ�ǩ����֤PSS_MGF1_SHA256ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_PSS_MGF1_SHA384,/**< �ǶԳ�ǩ����֤PSS_MGF1_SHA384ģʽ */
	CRYPT_CMD_ID_RSASSA_PKCS1_PSS_MGF1_SHA512,/**< �ǶԳ�ǩ����֤PSS_MGF1_SHA512ģʽ */
	CRYPT_CMD_ID_DSA_SHA1,                  /**< �ǶԳ�ǩ����֤DSAģʽ */
	CRYPT_CMD_ID_UNKNOWN = 0x7FFFFFFE,      /**< δ֪ģʽ */
	CRYPT_CMD_ID_MAX = 0x7FFFFFFF           /**< ���CMD ID */
};

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����CRYPTO״̬����
 */
typedef enum crypt_cmd_status {
	CRYPT_INIT = 0,         /**< ��ʼ�� */
	CRYPT_UPDATE,           /**< ������ */
	CRYPT_UPDATEAAD,        /**< AE����֤���ݿ����� */
	CRYPT_DOFINAL,          /**< ���� */
	CRYPT_CMPFINAL,         /**< ��������֤ */
	CRYPT_SIGNDIGEST,       /**< ǩ�� */
	CRYPT_VERIFYDIGEST,     /**< ��֤ */
	CRYPT_STATUS_NUM        /**< ״̬���� */
} crypt_status;

/**
 * @ingroup  TEE_COMMON_DATA
 *
 * ��ȫ����STORAGE֧�ֵ�����ID
 */
enum storage_service_cmd_id {
	STORAGE_CMD_ID_INVALID = 0x10,          /**< Storage Task ��ЧID*/
	STORAGE_CMD_ID_OPEN,                    /**< Storage Task���ļ�*/
	STORAGE_CMD_ID_CLOSE,                   /**< Storage Task�ر��ļ�*/
	STORAGE_CMD_ID_CLOSEALL,                /**< Storage Task�ر������ļ�*/
	STORAGE_CMD_ID_READ,                    /**< Storage Task��ȡ�ļ�*/
	STORAGE_CMD_ID_WRITE,                   /**< Storage Taskд���ļ�*/
	STORAGE_CMD_ID_SEEK,                    /**< Storage Task��ȡ��ǰ�ļ�λ��*/
	STORAGE_CMD_ID_TELL,                    /**< Storage Task�ض�λ�ļ�*/
	STORAGE_CMD_ID_TRUNCATE,                /**< Storage Task�ı��ļ���С*/
	STORAGE_CMD_ID_REMOVE,                  /**< Storage Taskɾ���ļ�*/
	STORAGE_CMD_ID_FINFO,                   /**< Storage Task�����ļ�״̬*/
	STORAGE_CMD_ID_FSYNC,                   /**< Storage Taskͬ���ļ����洢�豸*/
	STORAGE_CMD_ID_UNKNOWN = 0x7FFFFFFE,    /**< Storage Task δ֪ID*/
	STORAGE_CMD_ID_MAX = 0x7FFFFFFF         /**< Storage Task ���ID*/
};
#if 1
/**
 * ingroup  TEE_COMMON_DATA
 *
 * tee DK other structure, containing the optional data for KDF,
 * if any data is not needed, then the pointer value and
 * the size must be set to NULL
 */
typedef struct {
	/* a unique object identifier (OID), indicating algorithm(s)
	for which the keying data will be used*/
	unsigned char    AlgorithmID[TEE_DK_MAX_SIZE_OF_OTHER_INFO];    /**< a unique object identifier (OID) */
	unsigned int   SizeOfAlgorithmID;                           /**< size of AlgorithmID */
	/* Public information contributed by the initiator */
	unsigned char    PartyUInfo[TEE_DK_MAX_SIZE_OF_OTHER_INFO]; /**< Public information contributed by the initiator */
	unsigned int   SizeOfPartyUInfo;                            /**< size of PartyUInfo */
	/* Public information contributed by the responder */
	unsigned char    PartyVInfo[TEE_DK_MAX_SIZE_OF_OTHER_INFO]; /**< Public information contributed by the responder */
	unsigned int   SizeOfPartyVInfo;                            /**< size of PartyVInfo */
	/* Mutually-known private information, e.g. shared information
	communicated throgh a separate channel */
	unsigned char    SuppPrivInfo[TEE_DK_MAX_SIZE_OF_OTHER_INFO];   /**< Mutually-known private information */
	unsigned int   SizeOfSuppPrivInfo;                          /**< size of SuppPrivInfo */
	/* Mutually-known public information, */
	unsigned char    SuppPubInfo[TEE_DK_MAX_SIZE_OF_OTHER_INFO];    /**< Mutually-known public information */
	unsigned int   SizeOfSuppPubInfo;                           /**< size of SuppPubInfo */
} tee_DK_OtherInfo;

/**
 * ingroup  TEE_COMMON_DATA
 *
 * TEE����Կ��������ģʽ����
*/
typedef enum {
	TEE_DK_ASN1_DerivMode = 0,                          /**< ASN1_DerivMode */
	TEE_DK_ConcatDerivMode = 1,                         /**< ConcatDerivMode */
	TEE_DK_X963_DerivMode = TEE_DK_ConcatDerivMode,     /**< X963_DerivMode */
	TEE_DK_OMADRM_DerivMode = 2,                        /**< OMADRM_DerivMode */
	TEE_DK_ISO18033_KDF1_DerivMode = 3,                 /**< ISO18033_KDF1_DerivMode */
	TEE_DK_ISO18033_KDF2_DerivMode = 4,                 /**< ISO18033_KDF2_DerivMode */
	TEE_DK_DerivFunc_NumOfModes,                        /**< num of modes */
} tee_DK_DerivFuncMode;

/**
 * ingroup  TEE_COMMON_DATA
 *
 * TEE����Կ����HASHģʽ����
*/
typedef enum {
	TEE_DK_HASH_SHA1_mode = 0,              /**< HASH_SHA1 */
	TEE_DK_HASH_SHA224_mode = 1,            /**< HASH_SHA224 */
	TEE_DK_HASH_SHA256_mode = 2,            /**< HASH_SHA256 */
	TEE_DK_HASH_SHA384_mode = 3,            /**< HASH_SHA384 */
	TEE_DK_HASH_SHA512_mode = 4,            /**< HASH_SHA512 */
	TEE_DK_HASH_NumOfModes,                 /**< num of modes */
} tee_DK_HASH_Mode;
#endif
#endif

/**
* History: \n
* 2013-05-03��y00204347��Create \n
*
* vi: set expandtab ts=4 sw=4 tw=80:
*/
