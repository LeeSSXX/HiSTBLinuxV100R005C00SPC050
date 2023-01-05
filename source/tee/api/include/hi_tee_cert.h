/******************************************************************************

Copyright (C), 2016-2020, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_tee_cert.h
Version       : Initial
Author        : Hisilicon hisecurity team
Created       : 2016-11-4
Last Modified :
Description   : Hisilicon CERT API declaration
Function List :
History       :
******************************************************************************/

#ifndef __HI_TEE_CERT__
#define __HI_TEE_CERT__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*************************** Structure Definition ****************************/
/** \addtogroup      CERT */
/** @{ */  /** <!-- [CERT] */

/** Input and Output data Num */
/**CNcomment:Input data ��Output data���ݴ�С*/
#define CERT_AKL_REG_DATA_NUM             8

/** Initialization vector max length */
/**CNcomment:IV������ݳ���*/
#define HI_TEE_CERT_IV_MAX_LEN            16

/**< Engine type*/
/**< CNcomment: ��������*/
typedef enum
{
    HI_TEE_CERT_KEY_PORT_DEMUX        = 0X0,   /**< DEMUX*/
    HI_TEE_CERT_KEY_PORT_CIPHER       = 0x1,   /**< MCipher*/
    HI_TEE_CERT_KEY_PORT_PLCIPHER     = 0x2,   /**< PayLoad Cipher*/
    HI_TEE_CERT_KEY_PORT_UNKNOWN      = 0x3,
} HI_TEE_CERT_KEY_PORT_SEL_E;

/**< Target engine type*/
/**< CNcomment: Ŀ����������*/
typedef enum
{
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_CSA2          = 1,   /**<Demux PAYLOAD CSA2 */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_CSA3             ,   /**<Demux PAYLOAD CSA3 */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_AES_CBC_IDSA     ,   /**<Demux/PLCipher PAYLOAD AES  IPTV Mode */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_AES_ECB          ,   /**<Demux/PLCipher PAYLOAD AES  ECB Mode */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_AES_CBC_CI       ,   /**<Demux/PLCipher PAYLOAD AES  CIPLUS */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_AES_CLRSTART_ECB ,   /**<Demux PAYLOAD AES  CLRSTART ECB */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_AES_CLRSTART_CBC ,   /**<Demux PAYLOAD AES  CLRSTART CBC */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_ASA              ,   /**<Demux PAYLOAD ASA */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_TDES_IPTV        ,   /**<Demux PAYLOAD TDES IPTV */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_TDES_ECB         ,   /**<Demux PAYLOAD TDES ECB */
    HI_TEE_CERT_TARGET_ENGINE_PAYLOAD_TDES_CBC         ,   /**<Demux PAYLOAD TDES CBC */
    HI_TEE_CERT_TARGET_ENGINE_RAW_AES                  ,   /**<MultiCipher AES CBC */
    HI_TEE_CERT_TARGET_ENGINE_RAW_TDES                 ,   /**<MultiCipher AES CBC */
    HI_TEE_CERT_TARGET_ENGINE_BUTT
} HI_TEE_CERT_TARGET_ENGINE_E;

/**< Timeout type*/
/**< CNcomment: ��ʱ����*/
typedef enum
{
    HI_TEE_CERT_TIMEOUT_DEFAULT, /**<  Default timeout. Indicates that the associated command does not write in OTP.*/
                                 /**<  CNcomment: Ĭ�ϳ�ʱ��������ص������ǲ�����otp��д�����ݵ�����*/
    HI_TEE_CERT_TIMEOUT_OTP,     /**<  The associated command is writing in OTP memory.*/
                                 /**<  CNcomment: ������ص�����Ὣ����д�뵽otp�е�����*/
    HI_TEE_CERT_LAST_TIMEOUT     /**<  Other use.*/  /**< CNcomment: ������;*/
} HI_TEE_CERT_TIMTOUT_E;

/** Structure of the CERT exchange information */
/** CNcomment:CERT exchange��Ϣ�ṹ */
typedef struct
{
    HI_U8 u8InputData[4 * CERT_AKL_REG_DATA_NUM];  /**< Input data to be written in the 8 32-bit CERT common interface DATA_IN_X
                                                        registers. First 4 MSB inputData[0:3] shall be written in DATA_IN_0
                                                        register, next 4 MSB inputData[4:7] in DATA_IN_1 register and so on.*/
                                                   /**< CNcomment: �������ݣ�����д�뵽CERT DATA_IN_X�Ĵ�����8��32�������ݣ�
                                                        ǰ4�ֽ�(MSB)д��DATA_IN_0�Ĵ���,������4�ֽ�(MSB)д��DATA_IN_1���Դ����ơ�*/
    HI_U8 u8OutputData[4 * CERT_AKL_REG_DATA_NUM]; /**< Buffer where to write values of the 8 32-bit CERT common interface
                                                        DATA_OUT_X registers following the processing of a command. */
                                                   /**< CNcomment: ����ִ��֮�󣬴˻�������д���CERT DATA_OUT_X�Ĵ�����ȡ��8��32��������*/
    HI_U8 u8Status[4];                             /**< Buffer where to write the value of the CERT common interface STATUS_AKL
                                                        register following the processing of a command.*/
                                                   /**< CNcomment: ����ִ��֮�󣬴˻�������д���STATUS_AKL�Ĵ�����ȡ������*/
    HI_U8 u8Opcodes[4];                            /**< Command operation codes to be written in the CERT common interface
                                                        COMMAND register. The least significant bit that acts as the command
                                                        start bit is already set to 1.*/
                                                   /**< CNcomment: д��CERT COMMAND�Ĵ�������������룬���һ��bit����Ϊ1��Ч*/
    HI_TEE_CERT_TIMTOUT_E enTimeout;               /**< This field characterizes the processing duration of the command. It is
                                                        not expressed as a duration. Associated timeout values is to be defined
                                                        by the entity in charge of developing the CERT driver. This field is
                                                        mainly used to know whether the command is going to write in OTP or not.*/
                                                   /**< CNcomment: ��������Ĵ���ʱ��, ��������������ʱ��ĳ��ȣ����峬ʱʱ������������
                                                        ����ֶ���Ҫ���ڸ�֪���������Ƿ���дotp������*/
} HI_TEE_CERT_COMMAND;


/** Structure of the CERT CTRL information */
/** CNcomment:CERT CTRL��Ϣ�ṹ */
typedef struct
{
    HI_HANDLE hHandle;                               /**< Target module handle, address information included*/ 
    HI_BOOL bIsEven;                                 /**< Type of key odd or even*/ 
    HI_TEE_CERT_TARGET_ENGINE_E enEngine;            /**< Target crypto engine*/
    HI_TEE_CERT_KEY_PORT_SEL_E enPortSel;            /**< Port select*/
    HI_BOOL bSecEn;                                  /**< Secure enable*/
} HI_TEE_CERT_KEY_DATA_S;

/** Structure of the IV information */
/** CNcomment:IV��Ϣ�ṹ */
typedef struct
{
    HI_HANDLE hHandle;                               /**< Target module handle, address information included*/
    HI_BOOL bIsEven;                                 /**< Type of IV odd or even*/
    HI_U8   u8IV[HI_TEE_CERT_IV_MAX_LEN];               /**< Initialization vector (IV) */
    HI_U32  u32Len;                                  /**< Initialization vector (IV) length*/
} HI_TEE_CERT_IV_S;

/**< Structure of CERT resource */
/**< CNcomment: CERT��Դ�ṹ*/
typedef struct
{
    HI_HANDLE hResHandle;  /**< Cert resource handle */  /**< CNcomment: Cert ���*/
} HI_TEE_CERT_RES_HANDLE;

typedef struct
{
    HI_U32 addr;
    HI_U32 val;
} CERT_REG_S;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      CERT */
/** @{ */  /** <!--[CERT]*/

/**
\brief Initializes the CERT module.CNcomment:��ʼ��CERTģ�顣CNend
\attention \n
Before using CERT, you must call this application programming interface (API).\n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:�ڽ���PLCIPHER��ز���ǰӦ�����ȵ��ñ��ӿ�\n
�ظ����ñ��ӿڷ��سɹ���CNend
\param  N/A.CNcomment:�ޡ�CNend
\retval ::HI_SUCCESS Success. CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\see \n
N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_Init(HI_VOID);

/**
\brief Deinitializes the CERT module. CNcomment:ȥ��ʼ��CERTģ�顣CNend
\attention \n
After this API is called, the CERT module is stopped, and the CERT resources used by the process are released.\n
This API is valid when it is called for the first time. If this API is called repeatedly, the error code HI_SUCCESS is returned.
CNcomment:���ñ��ӿ�ֹͣʹ��CERTģ�飬���ͷű�������ռ�õ�CERT��Դ\n
���ӿڵ�һ�ε��������ã��ظ����÷��سɹ���CNend
\param  N/A.CNcomment:�ޡ�CNend
\retval ::HI_SUCCESS Success CNcomment:�ɹ���CNend
\retval ::HI_FAILURE Calling this API fails. CNcomment:APIϵͳ����ʧ�ܡ�CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_DeInit(HI_VOID);

/**
\brief Send the key on the bus to the subsequent encryption or descryption engine.
CNcomment:�������ϵ�key�͸��󼶼ӽ������档CNend
\param[in] pstCtlData     Pointer to the structure of the CERT CTRL information.
CNcomment:ָ�����ͣ�ָ��CERT CTRL��Ϣ���ƽṹ�塣CNend
\retval ::HI_SUCCESS Success.CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\retval ::HI_ERR_CERT_NOT_INIT  The CERT module is not initialized.CNcomment:ģ��û�г�ʼ����CNend
\retval ::HI_ERR_CERT_INVALID_PTR  The pointer is null. CNcomment:ָ�����Ϊ�ա�CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_UseKey( HI_TEE_CERT_KEY_DATA_S *pstCtlData);

/**
\brief Set initialization vector to the subsequent encryption or descryption engine.
CNcomment:��IV�͸��󼶼ӽ������档CNend
\param[in] pstCtlData     Pointer to the structure of the IV information.
CNcomment:ָ�����ͣ�ָ��IV��Ϣ���ƽṹ�塣CNend
\retval ::HI_SUCCESS Success.CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\retval ::HI_ERR_CERT_NOT_INIT  The CERT module is not initialized.CNcomment:ģ��û�г�ʼ����CNend
\retval ::HI_ERR_CERT_INVALID_PTR  The pointer is null. CNcomment:ָ�����Ϊ�ա�CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_SetIV(HI_TEE_CERT_IV_S *pstIv);

/**
\brief Reset cert. CNcomment:��λCERTģ�顣CNend
\retval ::HI_SUCCESS Success.CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\retval ::HI_ERR_CERT_NOT_INIT  The CERT module is not initialized.CNcomment:ģ��û�г�ʼ����CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_Reset(HI_VOID);

/**
\brief This function reserves the CERT resource for exclusive use to the caller.
CNcomment:��ȡcert����Դ��CNend
\param[out] ppstResourceHandle     Handle assigned to the CERT resource.
CNcomment:ָ�����ͣ�ָ��CERT ��Դ�ľ����CNend
\retval ::HI_SUCCESS Success.CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\retval ::HI_ERR_CERT_NOT_INIT  The CERT module is not initialized.CNcomment:ģ��û�г�ʼ����CNend
\retval ::HI_ERR_CERT_INVALID_PTR  The pointer is null. CNcomment:ָ�����Ϊ�ա�CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_Lock(HI_TEE_CERT_RES_HANDLE **ppstResourceHandle);

/**
\brief This function releases the CERT resource previously locked by HI_TEE_CERT_Lock()
CNcomment:�ͷű�����HI_TEE_CERT_Lock��ס����Դ��CNend
\param[in] pstResourceHandle     Pointer to the structure of the CERT resource handle.
CNcomment:ָ�����ͣ�ָ��CERT ��Դ�ľ����CNend
\retval ::HI_SUCCESS Success.CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\retval ::HI_ERR_CERT_NOT_INIT  The CERT module is not initialized.CNcomment:ģ��û�г�ʼ����CNend
\retval ::HI_ERR_CERT_INVALID_PTR  The pointer is null. CNcomment:ָ�����Ϊ�ա�CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_Unlock(HI_TEE_CERT_RES_HANDLE *pstResourceHandle);

/**
\brief
This function is used to send a series of commands to the CERT block, for chipset information or operating OTP.
CNcomment:�˺���������CERT IP����һϵ�е������ȡоƬ��Ϣ���߲���OTP�� CNend
\param[in] pstResourceHandle Handle to the CERT resource.
CNcomment:ָ�����ͣ�ָ��CERT ��Դ�ľ����CNend
\param[in] ulNumOfCommands   Number of commands to be processed by the CERT block.
CNcomment: ����CERT���������Ŀ����CNend
\param[in,out] pstCommands   This structure is used to accommodate input parameters of each command \n
as well as resulting output. The memory is allocated by the caller.
CNcomment: �ýṹ���ڴ洢ÿ���������������Լ����ɵ�������ڴ��ɵ����߷��䡣CNend
\param[out] pulNumOfProcessedCommands   Number of commands actually processed by the CERT block.
CNcomment:ʵ��ִ�е�����������CNend
\retval ::HI_SUCCESS Success.CNcomment:�ɹ���CNend
\retval ::HI_FAILURE  Calling this API fails.CNcomment:APIϵͳ����ʧ�ܡ�CNend
\retval ::HI_ERR_CERT_NOT_INIT  The CERT module is not initialized.CNcomment:ģ��û�г�ʼ����CNend
\retval ::HI_ERR_CERT_INVALID_PTR  The pointer is null. CNcomment:ָ�����Ϊ�ա�CNend
\retval ::HI_ERR_CERT_TIME_OUT  The timeout has expired and the CERT block is still processing a command.
CNcomment:CERTһֱ��ִ��ĳ������³�ʱ��CNend
\see \n
 N/A.CNcomment:�ޡ�CNend
*/
HI_S32 HI_TEE_CERT_Exchange(HI_TEE_CERT_RES_HANDLE *pstResourceHandle, HI_SIZE_T ulNumOfCommands,
                            const HI_TEE_CERT_COMMAND *pstCommands, HI_SIZE_T *pulNumOfProcessedCommands);


HI_S32 HI_TEE_CERT_READL( CERT_REG_S *reg);

HI_S32 HI_TEE_CERT_WRITEL( CERT_REG_S *reg);
/** @} */  /** <!-- ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __HI_TEE_CERT__ */

/* END OF FILE */
