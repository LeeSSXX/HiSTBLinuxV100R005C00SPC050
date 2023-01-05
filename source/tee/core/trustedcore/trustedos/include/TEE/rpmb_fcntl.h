/**
 * @file rpmb_fcntl.h
 *
 * Copyright(C), 2013-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������TEE(Trusted Execution Environment������ִ�л���)RPMB��ȫ�洢API \n
*/

/**@defgroup TEE_RPMB_API  RPMB��ȫ�洢
* @ingroup TEE_API
*/

#ifndef _RPMB_FCNTL_H
#define _RPMB_FCNTL_H

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ʼ��
 *
 * @par ����:
 * ������ʼ����ִ��дRPMB Key�͸�ʽ������
 *
 * @attention �ú���ֻ��ִ��һ��
 * @param ��
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_RPMB_GENERIC RPMB������ͨ�ô���
 * @retval #TEE_ERROR_RPMB_MAC_FAIL RPMB������MACУ�����
 * @retval #TEE_ERROR_RPMB_RESP_UNEXPECT_MAC RPMBӦ�����ݵ�MACУ�����
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Init(void);

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ȫ��ʽ��
 *
 * @par ����:
 * RPMB��ȫ�洢��ȫ��ʽ������
 *
 * @attention ��
 * @param ��
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_RPMB_GENERIC RPMB������ͨ�ô���
 * @retval #TEE_ERROR_RPMB_MAC_FAIL RPMB������MACУ�����
 * @retval #TEE_ERROR_RPMB_RESP_UNEXPECT_MAC RPMBӦ�����ݵ�MACУ�����
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Format(void);

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢�洢�ļ�
 *
 * @par ����:
 * RPMB��ȫ�洢�洢�ļ�
 *
 * @attention ���Ҫ����д�ļ������ܣ�
 * ��Ҫ��TA��manifest�ж���Ѵ�С�������ļ���С��3���ټ���256KB��
 * ����: Ҫд���С��100KB���ļ�������ĶѴ�С������556KB(3*100+256)��
 * ����Ѵ�С�޷����㣬д�ļ���Ȼ��ɹ��������ܽϵ͡�
 * @param filename [IN]  д�����ݵ��ļ�������󳤶�Ϊ64�ֽ�
 * @param buf [IN]  д�����ݵĻ�����
 * @param size [IN]  д�����ݵĴ�С������СΪ160KB
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����������󣬻��ļ������ȳ���96�ֽ�
 * @retval #TEE_ERROR_RPMB_NOSPC RPMB�������̿ռ䲻��
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Write(const char *filename, uint8_t *buf, size_t size);

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ȡ�ļ�
 *
 * @par ����:
 * RPMB��ȫ�洢��ȡ�ļ�
 *
 * @attention ���Ҫ�������ļ������ܣ�
 * ��Ҫ��TA��manifest�ж���Ѵ�С�������ļ���С��3���ټ���256KB��
 * ����: Ҫ��ȡ��С��100KB���ļ�������ĶѴ�С������556KB(3*100+256)��
 * ����Ѵ�С�޷����㣬���ļ���Ȼ��ɹ��������ܽϵ͡�
 * @param filename [IN]  ��ȡ���ݵ��ļ�������󳤶�Ϊ64�ֽ�
 * @param buf [IN]  ��ȡ���ݵĻ�����
 * @param size [IN]  ��ȡ���ݵĴ�С
 * @param count [OUT]  ʵ�ʶ�ȡ���ݵĴ�С
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����������󣬻��ļ������ȳ���96�ֽ�
 * @retval #TEE_ERROR_RPMB_FILE_NOT_FOUND �ļ�������
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Read(const char *filename, uint8_t *buf, size_t size, uint32_t *count);

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢�������ļ�
 *
 * @par ����:
 * RPMB��ȫ�洢�������ļ�
 *
 * @attention ��
 * @param old_name [IN]  ���ļ���
 * @param new_name [IN]  ���ļ���
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����������󣬻��ļ������ȳ���96�ֽ�
 * @retval #TEE_ERROR_RPMB_FILE_NOT_FOUND ���ļ�������
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Rename(const char *old_name, const char *new_name);

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢ɾ���ļ�
 *
 * @par ����:
 * RPMB��ȫ�洢ɾ���ļ�
 *
 * @attention ��
 * @param filename [IN]  ��ɾ�����ļ���
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����������󣬻��ļ������ȳ���96�ֽ�
 * @retval #TEE_ERROR_RPMB_FILE_NOT_FOUND �ļ�������
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Rm(const char *filename);

/**
 * @ingroup  TEE_RPMB_API
 *
 * �洢��RPMB�������ļ�״̬������#TEE_RPMB_FS_Stat���� \n
*/
struct rpmb_fs_stat {
	uint32_t size;    /**< �ļ���С  */
	uint32_t reserved;    /**< Ԥ��  */
};
/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ȡ�ļ�״̬
 *
 * @par ����:
 * RPMB��ȫ�洢��ȡ�ļ�״̬
 *
 * @attention ��
 * @param filename [IN]  �ļ���
 * @param stat [OUT]  ��ȡ���ļ�״̬��Ϣ
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����������󣬻��ļ������ȳ���96�ֽ�
 * @retval #TEE_ERROR_RPMB_FILE_NOT_FOUND �ļ�������
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result TEE_RPMB_FS_Stat(const char *filename, struct rpmb_fs_stat *stat);

/**
 * @ingroup  TEE_RPMB_API
 *
 * �洢��RPMB�����Ĵ���״̬������#TEE_RPMB_FS_StatDisk���� \n
*/
struct rpmb_fs_statdisk {
	uint32_t disk_size;    /**< RPMB�����ܴ�С  */
	uint32_t ta_used_size;    /**< TA�Ѿ�ʹ�õĴ�С  */
	uint32_t free_size;    /**< RPMB�����ڷ����ļ��Ŀռ��С  */
	uint32_t reserved;    /**< Ԥ��  */
};
/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ȡ����״̬
 *
 * @par ����:
 * RPMB��ȫ�洢��ȡ����״̬
 *
 * @attention ��
 * @param stat [OUT]  ��ȡ�Ĵ���״̬��Ϣ
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ����������󣬻��ļ������ȳ���96�ֽ�
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R006C00
*/
TEE_Result TEE_RPMB_FS_StatDisk(struct rpmb_fs_statdisk *stat);

/**
 * @ingroup TEE_RPMB_API
 * �ļ����Զ��壬��ʾ�ļ��ڻָ��������ý׶β��ɲ���
 */
#define TEE_RPMB_FMODE_NON_ERASURE (1<<0)
/**
 * @ingroup TEE_RPMB_API
 * �ļ����Զ��壬��ʾ����ļ�������ֵ
 */
#define TEE_RPMB_FMODE_CLEAR (0)
/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢�����ļ�����
 *
 * @par ����:
 * RPMB��ȫ�洢�����ļ�����
 *
 * @attention ��
 * @param filename [IN]  �ļ���
 * @param fmode [IN]
 * �ļ����ԣ���ǰ֧��#TEE_RPMB_FMODE_NON_ERASURE��#TEE_RPMB_FMODE_CLEAR�������ԣ�����ֵ�᷵��#TEE_ERROR_BAD_PARAMETERS
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS �����������
 * @retval #TEE_ERROR_RPMB_FILE_NOT_FOUND �ļ�������
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R006C00
*/
TEE_Result TEE_RPMB_FS_SetAttr(const char *filename, uint32_t fmode);

/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ʽ��
 *
 * @par ����:
 * RPMB��ȫ�洢��ʽ����ɾ���ļ������ǿɲ������ļ���
 * �����ļ������ǲ��ɲ������ļ�
 *
 * @attention ��
 * @param filename [IN]  �ļ���
 * @param fmode [IN]  �ļ�����
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_RPMB_GENERIC RPMB������ͨ�ô���
 * @retval #TEE_ERROR_RPMB_MAC_FAIL RPMB������MACУ�����
 * @retval #TEE_ERROR_RPMB_RESP_UNEXPECT_MAC RPMBӦ�����ݵ�MACУ�����
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R006C00
*/
TEE_Result TEE_RPMB_FS_Erase(void);

/**
 * @ingroup  TEE_RPMB_API
 *
 * RPMB Key��״̬������#TEE_RPMB_KEY_Status���� \n
*/
enum TEE_RPMB_KEY_STAT {
	TEE_RPMB_KEY_INVALID = 0x0,
	TEE_RPMB_KEY_SUCCESS = 0x1,    /**< RPMB Key��д����ƥ����ȷ  */
	TEE_RPMB_KEY_NOT_PROGRAM,    /**< RPMB Keyδд��  */
	TEE_RPMB_KEY_NOT_MATCH,        /**< RPMB Key��д�뵫ƥ��ʧ��  */
};
/**
 * @ingroup  TEE_RPMB_API
 * @brief RPMB��ȫ�洢��ȡRPMB Key״̬
 *
 * @par ����:
 * RPMB��ȫ�洢��ȡRPMB Key״̬
 *
 * @attention ��
 * @param ��
 *
 * @retval #TEE_RPMB_KEY_SUCCESS RPMB Key��д����ƥ����ȷ
 * @retval #TEE_RPMB_KEY_NOT_PROGRAM RPMB Keyδд��
 * @retval #TEE_RPMB_KEY_NOT_MATCH RPMB Key��д�뵫ƥ��ʧ��
 * @retval #TEE_RPMB_KEY_INVALID RPMB Key״̬��Ч
 *
 * @par ����:
 * @li rpmb_fcntl.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
uint32_t TEE_RPMB_KEY_Status(void);

#endif //_RPMB_FCNTL_H

/**
 * History: \n
 * 2014-12-18��sdk����ʼ�����ļ� \n
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
*/

