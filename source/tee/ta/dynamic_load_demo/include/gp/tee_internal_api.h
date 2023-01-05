/*******************************************************************************
* All rights reserved, Copyright (C) huawei LIMITED 2012
*------------------------------------------------------------------------------
* File Name   : tee_internal_api.h
* Description : Reference of TEE internal api and internal definitions
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
* @file tee_internal_api.h
*
* Copyright(C), 2008-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
*
* �������������ͺͳ�������\n
*/

/** @defgroup TEE_CommonDef �������ͺͳ�������
* @ingroup TEE_API
*/

#ifndef __TEE_INTERNAL_API_H
#define __TEE_INTERNAL_API_H

/**
 * @ingroup TEE_CommonDef
 * �޷������Ͷ���
 */
typedef unsigned long long uint64_t;
/**
 * @ingroup TEE_CommonDef
 * �޷������Ͷ���
 */
typedef unsigned int     uint32_t;

/**
 * @ingroup TEE_CommonDef
 * �з������Ͷ���
 */
typedef signed int       int32_t;
/**
 * @ingroup TEE_CommonDef
 * �޷��Ŷ����Ͷ���
 */
typedef unsigned short    uint16_t;
/**
 * @ingroup TEE_CommonDef
 * �з��Ŷ����Ͷ���
 */
typedef signed short      int16_t;
/**
 * @ingroup TEE_CommonDef
 * �޷����ַ��Ͷ���
 */
typedef unsigned char     uint8_t;
/**
 * @ingroup TEE_CommonDef
 * �з����ַ��Ͷ���
 */
typedef signed char       int8_t;

#ifndef bool
/**
 * @ingroup TEE_CommonDef
 * �����Ͷ���
 */
#define bool   _Bool
#endif
/**
 * @ingroup TEE_CommonDef
 * ���ݳ������Ͷ���
 */
typedef unsigned int    size_t;

typedef int *TEE_MUTEX_HANDLE;

/**
 * @ingroup TEE_CommonDef
 * true��ֵ����
 */
#undef true
#define true    1

/**
 * @ingroup TEE_CommonDef
 * false��ֵ����
 */
#undef false
#define false   0

/**
 * @ingroup TEE_CommonDef
 * NULL��ֵ����
 */
#ifndef NULL
#define  NULL   ((void*) 0)
#endif

#define PARAM_NOT_USED(val) ((void)val)

/**
 * @ingroup  TEE_CommonDef
 * TEE����������ṹ����
 *
 * �������ǻ���������ʱ�������������ָ��memref\n
 * ����������������ʱ�������������ָ��value
 */
typedef union {
	struct {
		void *buffer;       /**< ������ָ��  */
		unsigned int size;  /**< ��������С  */
	} memref;
	struct {
		unsigned int a;     /**< ��������a  */
		unsigned int b;     /**< ��������b  */
	} value;
} TEE_Param;

/**
 * @ingroup TEE_CommonDef
 * ���ڼ���ǰ�ȫ�����밲ȫ���紫�ݲ�������ֵ
 */
#define TEE_PARAM_TYPES( param0Type, param1Type, param2Type, param3Type) \
    ((param3Type) << 12 | (param2Type) << 8 | (param1Type) << 4 | (param0Type))

/**
 * @ingroup TEE_CommonDef
 * ���ڼ���paramTypes���ֶ�index����ֵ
 */
#define TEE_PARAM_TYPE_GET( paramTypes, index) \
    (((paramTypes) >> (4*(index))) & 0x0F)

/**
 * @ingroup TEE_CommonDef
 *
 * @brief ����У��param type�Ƿ�ƥ��
 *
 * @attention ��
 * @param param_to_check [IN]  ��У��Ĳ�������
 * @param valid0-3 [IN] ��ȷ�Ĳ�������
 *
 * @retval #true �ȽϵĲ�������ƥ��
 * @retval #false �ȽϵĲ������Ͳ�ƥ��
 */
static inline bool check_param_type(uint32_t param_to_check, uint32_t valid0,
				    uint32_t valid1, uint32_t valid2, uint32_t valid3)
{
	return (TEE_PARAM_TYPES(valid0, valid1, valid2, valid3) == param_to_check);
}

/**
 * @ingroup  TEE_CommonDef
 *
 * �������Ͷ���\n
 */
enum TEE_ParamType {
	TEE_PARAM_TYPE_NONE = 0x0,
	TEE_PARAM_TYPE_VALUE_INPUT = 0x1,
	TEE_PARAM_TYPE_VALUE_OUTPUT = 0x2,
	TEE_PARAM_TYPE_VALUE_INOUT = 0x3,
	TEE_PARAM_TYPE_MEMREF_INPUT = 0x5,
	TEE_PARAM_TYPE_MEMREF_OUTPUT = 0x6,
	TEE_PARAM_TYPE_MEMREF_INOUT = 0x7,
	TEE_PARAM_TYPE_ION_INPUT = 0x8,
};

#define S_VAR_NOT_USED(variable)  do{(void)(variable);}while(0);

/**
* @ingroup  TEE_CommonDef
*
* TEE����ṹ�嶨��
*/
typedef struct {
	uint32_t objectType;        /**< ��������  */
	uint32_t objectSize;        /**< �������ݳ���  */
	uint32_t maxObjectSize;     /**< �����������󳤶� */
	uint32_t objectUsage;       /**< ����ʹ�÷�Χ */
	uint32_t dataSize;          /**< ��������ݳ��� */
	uint32_t dataPosition;      /**< ���������λ�� */
	uint32_t handleFlags;       /**< �������ݷ��ʷ�ʽ */
} TEE_ObjectInfo;

/**
* @ingroup  TEE_CommonDef
*
* TEE���Խṹ�嶨��
*/
typedef struct {
	uint32_t attributeID;           /**<Attribute ID  */
	union {
		struct {
			void *buffer;           /**< ������ָ��  */
			size_t length;          /**< ����������  */
		} ref;
		struct {
			uint32_t a;             /**< ��������a  */
			uint32_t b;             /**< ��������b  */
		} value;
	} content;
} TEE_Attribute;

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * object attribute ������\n
*/
enum TEE_ObjectAttribute {
	TEE_ATTR_SECRET_VALUE = 0xC0000000,                   /**< attributeΪSECRET_VALUE */
	TEE_ATTR_RSA_MODULUS = 0xD0000130,                    /**< attributeΪRSA_MODULUS */
	TEE_ATTR_RSA_PUBLIC_EXPONENT = 0xD0000230,            /**< attributeΪRSA_PUBLIC_EXPONENT */
	TEE_ATTR_RSA_PRIVATE_EXPONENT = 0xC0000330,           /**< attributeΪRSA_PRIVATE_EXPONENT */
	TEE_ATTR_RSA_PRIME1 = 0xC0000430,                     /**< attributeΪRSA_PRIME1 */
	TEE_ATTR_RSA_PRIME2 = 0xC0000530,                     /**< attributeΪRSA_PRIME2 */
	TEE_ATTR_RSA_EXPONENT1 = 0xC0000630,                  /**< attributeΪRSA_EXPONENT1 */
	TEE_ATTR_RSA_EXPONENT2 = 0xC0000730,                  /**< attributeΪRSA_EXPONENT2 */
	TEE_ATTR_RSA_COEFFICIENT  = 0xC0000830,               /**< attributeΪRSA_COEFFICIENT */
	TEE_ATTR_DSA_PRIME = 0xD0001031,                      /**< attributeΪDSA_PRIME */
	TEE_ATTR_DSA_SUBPRIME = 0xD0001131,                   /**< attributeΪDSA_SUBPRIME */
	TEE_ATTR_DSA_BASE = 0xD0001231,                       /**< attributeΪDSA_BASE */
	TEE_ATTR_DSA_PUBLIC_VALUE = 0xD0000131,               /**< attributeΪDSA_PUBLIC_VALUE */
	TEE_ATTR_DSA_PRIVATE_VALUE = 0xC0000231,              /**< attributeΪDSA_PRIVATE_VALUE */
	TEE_ATTR_DH_PRIME = 0xD0001032,                       /**< attributeΪDH_PRIME */
	TEE_ATTR_DH_SUBPRIME = 0xD0001132,                    /**< attributeΪDH_SUBPRIME */
	TEE_ATTR_DH_BASE = 0xD0001232,                        /**< attributeΪDH_BASE */
	TEE_ATTR_DH_X_BITS = 0xF0001332,                      /**< attributeΪDH_X_BITS */
	TEE_ATTR_DH_PUBLIC_VALUE = 0xD0000132,                /**< attributeΪDH_PUBLIC_VALUE */
	TEE_ATTR_DH_PRIVATE_VALUE = 0xC0000232,               /**< attributeΪDH_PRIVATE_VALUE */
	TEE_ATTR_RSA_OAEP_LABEL = 0xD0000930,                 /**< attributeΪRSA_OAEP_LABEL*/
	TEE_ATTR_RSA_PSS_SALT_LENGTH = 0xF0000A30,             /**< attributeΪRSA_OAEP_LABEL */
	TEE_ATTR_ECC_PUBLIC_EXPONENT = 0xE0000001,            /**< attributeΪECC_PUBLIC_EXPONENT */
	TEE_ATTR_ECC_PRIVATE_EXPONENT = 0xE0000002,           /**< attributeΪECC_PRIVATE_EXPONENT */
	TEE_ATTR_ECC_DOMAINID = 0xE0000003,                   /**< attributeΪECC_DOMAINID */
	TEE_ATTR_ECC_HASHMODE = 0xE0000004,                   /**< attributeΪECC_HASHMODE */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * object�����ͣ�������object �е�key ��ʲô���� \n
*/
enum TEE_ObjectType {
	TEE_TYPE_AES = 0xA0000010,                    /**< object�е�keyΪAES���� */
	TEE_TYPE_DES = 0xA0000011,                    /**< object�е�keyΪDES���� */
	TEE_TYPE_DES3 = 0xA0000013,                   /**< object�е�keyΪDES3���� */
	TEE_TYPE_HMAC_MD5 = 0xA0000001,               /**< object�е�keyΪHMAC_MD5���� */
	TEE_TYPE_HMAC_SHA1 = 0xA0000002,              /**< object�е�keyΪHMAC_SHA1���� */
	TEE_TYPE_HMAC_SHA224 = 0xA0000003,            /**< object�е�keyΪHMAC_SHA224���� */
	TEE_TYPE_HMAC_SHA256 = 0xA0000004,            /**< object�е�keyΪHMAC_SHA256���� */
	TEE_TYPE_HMAC_SHA384 = 0xA0000005,            /**< object�е�keyΪHMAC_SHA384���� */
	TEE_TYPE_HMAC_SHA512 = 0xA0000006,            /**< object�е�keyΪHMAC_SHA512���� */
	TEE_TYPE_RSA_PUBLIC_KEY = 0xA0000030,         /**< object�е�keyΪRSA_PUBLIC_KEY���� */
	TEE_TYPE_RSA_KEYPAIR = 0xA1000030,            /**< object�е�keyΪRSA_KEYPAIR���� */
	TEE_TYPE_DSA_PUBLIC_KEY = 0xA0000031,         /**< object�е�keyΪDSA_PUBLIC_KEY���� */
	TEE_TYPE_DSA_KEYPAIR = 0xA1000031,            /**< object�е�keyΪDSA_KEYPAIR���� */
	TEE_TYPE_DH_KEYPAIR = 0xA1000032,             /**< object�е�keyΪDH_KEYPAIR���� */
	TEE_TYPE_GENERIC_SECRET = 0xA0000000,         /**< object�е�keyΪGENERIC_SECRET���� */
	TEE_TYPE_DATA = 0xA1000033,                   /**< objectû��key��Ϊ���������� */
	TEE_TYPE_ECC_PUBLIC_KEY = 0xA0000034,         /**< object�е�keyΪRSA_PUBLIC_KEY���� */
	TEE_TYPE_ECC_KEYPAIR = 0xA1000035,            /**< object�е�keyΪRSA_KEYPAIR���� */
	TEE_TYPE_ECC_PRIVATE_KEY = 0xA0000036,         /**< object�е�keyΪDSA_PUBLIC_KEY���� */

};

/**
* @ingroup  TEE_CommonDef
*
* TEE�������ṹ�嶨��
*/
struct __TEE_ObjectHandle {
	void *dataPtr;                  /**< ��������ݶ������ָ��  */
	uint32_t dataLen;               /**< ��������ݶ������Ƴ���  */
	uint8_t dataName[255];          /**< ��������ݶ�������  */
	TEE_ObjectInfo *ObjectInfo;     /**< �������Ϣ����ָ��  */
	TEE_Attribute *Attribute;       /**< ���������ָ��  */
	uint32_t attributesLen;         /**< ��������Գ���  */
	uint32_t CRTMode;               /**< �����CRTģʽ  */
	void *infoattrfd;               /**< �������Ϣָ��  */
};
typedef struct __TEE_ObjectHandle *TEE_ObjectHandle;

typedef struct {
	uint32_t timeLow;
	uint16_t timeMid;
	uint16_t timeHiAndVersion;
	uint8_t clockSeqAndNode[8];
} TEE_UUID;

/**
* @ingroup  TEE_CommonDef
*
* �������صĴ�����
*/
enum TEE_Result_Value {
	TEE_SUCCESS = 0x0,                          /**< �ɹ�  */
	TEE_ERROR_INVALID_CMD,                      /**< �Ƿ����� */
	TEE_ERROR_SERVICE_NOT_EXIST,                /**< ���񲻴��� */
	TEE_ERROR_SESSION_NOT_EXIST,                /**< ���Ӳ����� */
	TEE_ERROR_SESSION_MAXIMUM,                  /**< ���������� */
	TEE_ERROR_REGISTER_EXIST_SERVICE,           /**< ע���Ѿ����ڵķ��� */
	TEE_ERROR_TARGET_DEAD_FATAL,                 /**< Global Task ����  */
	TEE_ERROR_READ_DATA,                        /**< ��ȡ�ļ�����  */
	TEE_ERROR_WRITE_DATA,                       /**< д���ļ�����  */
	TEE_ERROR_TRUNCATE_OBJECT,                  /**< �ض��ļ�����  */
	TEE_ERROR_SEEK_DATA,                        /**< �����ļ�����  */
	TEE_ERROR_SYNC_DATA,                        /**< ͬ���ļ�����  */
	TEE_ERROR_RENAME_OBJECT,                    /**< �������ļ�����  */
	TEE_ERROR_TRUSTED_APP_LOAD_ERROR,           /**< �򿪻Ựʱ�����ؿ���Ӧ�ó���ʧ��*/
	TEE_ERROR_STORAGE_EIO = 0x80001001,         /**<��ȫ�洢����I/O����*/
	TEE_ERROR_STORAGE_EAGAIN = 0x80001002,      /**<��ȫ�洢������ʱ�����ã�������*/
	TEE_ERROR_STORAGE_ENOTDIR = 0x80001003,     /**<��������Ŀ¼���޷�����*/
	TEE_ERROR_STORAGE_EISDIR = 0x80001004,      /**<����������Ŀ¼���޷�����*/
	TEE_ERROR_STORAGE_ENFILE = 0x80001005,      /**<����ϵͳ��˵�����ļ����Ѵﵽ���ֵ*/
	TEE_ERROR_STORAGE_EMFILE = 0x80001006,      /**<���ڽ�����˵�����ļ����Ѵﵽ���ֵ*/
	TEE_ERROR_STORAGE_EROFS = 0x80001007,       /**<��ȫ�洢����ֻ��*/
	TEE_ERROR_CORRUPT_OBJECT = 0xF0100001,      /**<�ļ�����*/
	TEE_ERROR_STORAGE_NOT_AVAILABLE = 0xF0100003,   /**<��ȫ�洢����������*/
	TEE_ISOCKET_ERROR_PROTOCOL = 0xF1007001,
	TEE_ISOCKET_ERROR_REMOTE_CLOSED = 0xF1007002,
	TEE_ISOCKET_ERROR_TIMEOUT = 0xF1007003,
	TEE_ISOCKET_ERROR_OUT_OF_RESOURCES = 0xF1007004,
	TEE_ISOCKET_ERROR_LARGE_BUFFER = 0xF1007005,
	TEE_ISOCKET_WARNING_PROTOCOL = 0xF1007006,
	TEE_ERROR_GENERIC = 0xFFFF0000,             /**< ͨ�ô���  */
	TEE_ERROR_ACCESS_DENIED = 0xFFFF0001 ,      /**< Ȩ��У��ʧ��  */
	TEE_ERROR_CANCEL = 0xFFFF0002 ,             /**< ������ȡ��  */
	TEE_ERROR_ACCESS_CONFLICT = 0xFFFF0003 ,    /**< �������ʵ��³�ͻ  */
	TEE_ERROR_EXCESS_DATA = 0xFFFF0004 ,        /**< ��������������̫��  */
	TEE_ERROR_BAD_FORMAT = 0xFFFF0005 ,         /**< ���ݸ�ʽ����ȷ  */
	TEE_ERROR_BAD_PARAMETERS = 0xFFFF0006 ,     /**< ������Ч  */
	TEE_ERROR_BAD_STATE = 0xFFFF0007,           /**< ��ǰ״̬�µĲ�����Ч  */
	TEE_ERROR_ITEM_NOT_FOUND = 0xFFFF0008,      /**< ���������δ�ҵ�  */
	TEE_ERROR_NOT_IMPLEMENTED = 0xFFFF0009,     /**< ����Ĳ������ڵ���δʵ��  */
	TEE_ERROR_NOT_SUPPORTED = 0xFFFF000A,       /**< ����Ĳ�����Ч��δ֧��  */
	TEE_ERROR_NO_DATA = 0xFFFF000B,             /**< ���ݴ���  */
	TEE_ERROR_OUT_OF_MEMORY = 0xFFFF000C,       /**< ϵͳû�п�����Դ  */
	TEE_ERROR_BUSY = 0xFFFF000D,                /**< ϵͳ��æ  */
	TEE_ERROR_COMMUNICATION = 0xFFFF000E,       /**< �������ͨ��ʧ��  */
	TEE_ERROR_SECURITY = 0xFFFF000F,            /**< ��⵽��ȫ����  */
	TEE_ERROR_SHORT_BUFFER = 0xFFFF0010,        /**< �ڴ����볤��С���������  */
	TEE_ERROR_EXTERNAL_CANCEL = 0xFFFF0011,     /**< �������ж�  */
	TEE_PENDING = 0xFFFF2000,                   /**< ���ŷ����ڵȴ�״̬(�첽����) */
	TEE_PENDING2 = 0xFFFF2001,                  /**< ���ŷ����ڵȴ�״̬2(����δ���) */
	TEE_PENDING3 = 0xFFFF2002,
	TEE_ERROR_TIMEOUT = 0xFFFF3001,             /**< ����ʱ */
	TEE_ERROR_OVERFLOW = 0xFFFF300f,            /**< ������� */
	TEE_ERROR_TARGET_DEAD = 0xFFFF3024,          /**< Trusted Application����  */
	TEE_ERROR_STORAGE_NO_SPACE = 0xFFFF3041,    /**< û���㹻��Flash�ռ����洢�ļ� */
	TEE_ERROR_MAC_INVALID = 0xFFFF3071,         /**< MACֵУ�����  */
	TEE_ERROR_SIGNATURE_INVALID = 0xFFFF3072,   /**< У��ʧ�� */
	TEE_ERROR_TIME_NOT_SET = 0xFFFF5000,        /**< ʱ��δ���� */
	TEE_ERROR_TIME_NEEDS_RESET = 0xFFFF5001,    /**< ʱ����Ҫ���� */
	TEE_FAIL = 0xFFFF5002,    /**< ʱ����Ҫ���� */
	TEE_ERROR_TIMER = 0xFFFF6000,
	TEE_ERROR_TIMER_CREATE_FAILED,
	TEE_ERROR_TIMER_DESTORY_FAILED,
	TEE_ERROR_TIMER_NOT_FOUND,
	TEE_ERROR_RPMB_BASE = 0xFFFF7000,           /**< RPMB��ȫ�洢�������ַ */
	TEE_ERROR_RPMB_GENERIC = 0xFFFF7001,        /**< RPMB������ͨ�ô��� */
	TEE_ERROR_RPMB_MAC_FAIL,                    /**< RPMB������MACУ����� */
	TEE_ERROR_RPMB_COUNTER_FAIL,                /**< RPMB������������У����� */
	TEE_ERROR_RPMB_ADDR_FAIL,                   /**< RPMB��������ַУ����� */
	TEE_ERROR_RPMB_WRITE_FAIL,                  /**< RPMB������д���� */
	TEE_ERROR_RPMB_READ_FAIL,                   /**< RPMB������������ */
	TEE_ERROR_RPMB_KEY_NOT_PROGRAM,             /**< RPMB Keyδд�� */
	TEE_ERROR_RPMB_RESP_UNEXPECT_MSGTYPE = 0xFFFF7100,    /**< RPMBӦ�����ݵ���Ϣ����У����� */
	TEE_ERROR_RPMB_RESP_UNEXPECT_BLKCNT,        /**< RPMBӦ�����ݵ����ݿ�У����� */
	TEE_ERROR_RPMB_RESP_UNEXPECT_BLKIDX,        /**< RPMBӦ�����ݵ����ݵ�ַУ����� */
	TEE_ERROR_RPMB_RESP_UNEXPECT_WRCNT,         /**< RPMBӦ�����ݵļ�����У����� */
	TEE_ERROR_RPMB_RESP_UNEXPECT_NONCE,         /**< RPMBӦ�����ݵ������У����� */
	TEE_ERROR_RPMB_RESP_UNEXPECT_MAC,           /**< RPMBӦ�����ݵ�MACУ����� */
	TEE_ERROR_RPMB_FILE_NOT_FOUND,              /**< RPMB��ȫ�洢�ļ������� */
	TEE_ERROR_RPMB_NOSPC,                       /**< RPMB��ȫ�洢���̿ռ䲻�� */
	TEE_ERROR_RPMB_SPC_CONFLICT,                /**< RPMB��ȫ�洢���̿ռ������Ѵ�TA���� */
	TEE_ERROR_TUI_IN_USE = 0xFFFF7110,
	TEE_ERROR_TUI_SWITCH_CHANNAL,
	TEE_ERROR_TUI_CFG_DRIVER,
	TEE_ERROR_TUI_INVALID_EVENT,
	TEE_ERROR_TUI_POLL_EVENT,
	TEE_ERROR_TUI_CANCELED,
	TEE_ERROR_ANTIROOT_RSP_FAIL = 0xFFFF9110,  /** < AntiRoot Response verify failed. */
	TEE_ERROR_ANTIROOT_INVOKE_ERROR = 0xFFFF9111,  /** < AntiRoot ERROR during invokecmd . */
	TEE_FAIL2
};

/**
 * @ingroup  TEE_CommonDef
 *
 * Login��ʽ
 */
enum TEE_LoginMethod {
	TEE_LOGIN_PUBLIC = 0x0,             /**< ����ҪLogin����  */
	TEE_LOGIN_USER,                     /**< �ṩ�û����зǰ�ȫӦ�ó����Login����  */
	TEE_LOGIN_GROUP,                    /**< �ṩ���û����зǰ�ȫӦ�ó����Login����  */
	TEE_LOGIN_APPLICATION = 0x4,        /**< �ṩ�ǰ�ȫӦ�ó����Լ���Login����  */
	TEE_LOGIN_USER_APPLICATION = 0x5,   /**< �ṩ�û����зǰ�ȫӦ�ó����Login���ݣ�
                                             �Լ��ǰ�ȫӦ�ó����Լ���Login����*/
	TEE_LOGIN_GROUP_APPLICATION = 0x6,  /**< �ṩ���û����зǰ�ȫӦ�ó����Login���ݣ�
                                             �Լ��ǰ�ȫӦ�ó����Լ���Login����*/
	TEE_LOGIN_IDENTIFY = 0x7,           /**< ���ð�ȫ�洢Ҫ��  */
};

/**
 * @ingroup  TEE_CommonDef
 *
 * Client����ݱ�ʶ
 */
typedef struct {
	uint32_t login;     /**< login�ķ�ʽ */
	TEE_UUID uuid;      /**< Client��UUID */
} TEE_Identity;

/**
 * @ingroup TEE_CommonDef
 * ��������ֵ���Ͷ���
 *
 * ���ڱ�ʾ�������ؽ��
 */
typedef enum TEE_Result_Value   TEE_Result;
typedef TEE_Result  TEEC_Result;

#define TEE_ORIGIN_TEE  0x00000003
#define TEE_ORIGIN_TRUSTED_APP 0x00000004
#define TEE_ORIGIN_TRUSTED_APP_TUI 0x00000005

#ifndef _TEE_TASessionHandle
#define _TEE_TASessionHandle
typedef uint32_t TEE_TASessionHandle;
#endif


/**
 * @ingroup  TEE_TA_CALLER_INDENTIFY
 * @brief  verify TA's caller's identify
 *
 * @par  TA can call this API to add caller's info,
 *       which is allowed to call this TA.
 *       this API is for CA in form of binary-excuteble file.
 *
 * @param ca_name [IN]   CA caller's packagename
 * @param ca_uid  [IN]   CA caller's uid
 */
TEE_Result AddCaller_CA_exec(char *ca_name, uint32_t ca_uid);

#include "tee_crypto_api.h"
#include "tee_mem_mgmt_api.h"
#include "tee_property_api.h"
#include "tee_time_api.h"
#include "tee_trusted_storage_api.h"
#include "tee_core_api.h"
//#include "tee_internal_se_api.h"

#endif
