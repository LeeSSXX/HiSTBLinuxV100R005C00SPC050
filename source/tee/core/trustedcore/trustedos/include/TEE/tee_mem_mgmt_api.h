/**
* @file tee_mem_mgnt_api.h
*
* Copyright(C), 2008-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED.
*
* �������ڴ�������API
*/

/** @defgroup grp_mem_api �ڴ����
*@ingroup TEE_API
*/

#ifndef __TEE_MEM_MGMT_API_H
#define __TEE_MEM_MGMT_API_H

#include "tee_internal_api.h"

enum MALLOC_HINT {
	ZERO = 0,
	NOT_ZERO = 1,
};

/**
* @ingroup  grp_mem_api
* @brief ��bufferָ��Ļ�������ǰsize���ֽ����Ϊx,buffer����ָ���������͵Ļ�������
*
* @par ����:
* ��
*
* @attention ��
* @param buffer [OUT] ָ�򻺳�����ָ��
* @param x [IN] ����ֵ
* @param size [IN] �����ֽ���
*
* @retval ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
**/
void TEE_MemFill(void *buffer, uint32_t x, uint32_t size);

/**
* @ingroup  grp_mem_api
* @brief ��srcָ��Ļ���������size�ֽڵ�destָ��Ļ�����
*
* @par ����:
* ��
*
* @attention src��dest����ָ���������͵Ļ�����
* @param dest [OUT] ָ��Ŀ�껺������ָ��
* @param src [IN] ָ��Դ��������ָ��
* @param size [IN] �������ֽ���
*
* @retval ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
**/
#if (TRUSTEDCORE_PLATFORM_CHOOSE == WITH_DEVCHIP_PLATFORM)
/* TEE_MemMove has performance issues, devchip should use ASM_MemMove */
#define TEE_MemMove ASM_MemMove
#else
void TEE_MemMove(void *dest, void *src, uint32_t size);
#endif
/**
* @ingroup  grp_mem_api
* @brief ��̬�����ֽ���Ϊsize���ڴ�
*
* @par ����:
* ���ص�ָ��Ķ��뷽ʽ��֤����ָ���κ�C�����ж���Ļ������ͣ�\n
* ����hint��һ�����ݸ��������ı�־����ǰ�汾ֻʵ����һ��hintֵ��\n
* �����汾�������hint�����࣬���������ص�������
*
* @attention ��
* @param size [IN] ������ڴ��С
* @param hint [IN] �����־��0��ʾ������ڴ��ڷ���ǰ�Ѿ������㣬����ֵ��������
*
* @retval ������NULL��ֵ ָ�����뵽���ڴ��ָ��
* @retval NULL ��ʾ����ʧ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
* @see TEE_Realloc | TEE_Free
**/
void *TEE_Malloc(size_t size, uint32_t hint);

/**
* @ingroup  grp_mem_api
* @brief �ͷŶ�̬������ڴ�
*
* @par ����:
* ���buffer����NULL����ôTEE_Free�����κζ�����\n
* �û���Ҫע�⴫�ݵ�bufferָ����ͨ��TEE_Malloc����TEE_Realloc����ģ�\n
* ����û�б�TEE_Free�ͷŹ���������������Ԥ�ϡ�
*
* @attention ��
* @param buffer [IN] ָ����Ҫ�ͷŵ��ڴ��ָ��
*
* @retval ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
* @see TEE_Malloc | TEE_Realloc
**/
void TEE_Free(void *buffer);

/**
* @ingroup grp_mem_api
* @brief �޸Ķ�̬�ڴ�Ĵ�С
*
* @par ����:
* �޸Ĵ�С����ڴ��������ԭʼ��С��ԭ�ڴ��е����ݻᱻ�������������ֵ�����������ġ�\n
* �ڸ����ڴ��Сʱ��������Ҫ���������ڴ棬�����ʱ����ʧ�ܣ���ôԭʼ�ڴ�ͻᱻ������\n
* ͬʱ��������NULL��\n
* ���buffer����NULL����ô�������ܾ���TEE_Malloc��ͬ��
*
* @attention
* �û���Ҫע�⴫�ݵ�bufferָ����ͨ��TEE_Malloc����TEE_Realloc����ģ�
* ����û�б�TEE_Free�ͷŹ���������������Ԥ�ϡ�
* @param buffer [IN] ָ����Ҫ�޸Ĵ�С���ڴ��ָ��
* @param new_size [IN] �޸ĺ�Ĵ�С
*
* @retval ������NULL��ֵ ָ���µĶ�̬�ڴ��ָ��
* @retval NULL ��ʾ����ʧ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
* @see TEE_Malloc | TEE_Free
**/
void *TEE_Realloc(void *buffer, uint32_t new_size);

/**
* @ingroup grp_mem_api
* @brief �ڴ����ݱȽ�
*
* @par ����:
* �ַ�����С�ǰ���˳��Ƚ�ÿ���ַ���ASCII��С��ֱ�����ִ�С��һ�����ַ����߽�������
*
* @attention ��
* @param buffer1 [IN] ָ��Ƚϵĵ�һ����������ָ��
* @param buffer2 [IN] ָ��Ƚϵĵڶ�����������ָ��
* @param size [IN] �Ƚϵ��ֽ���
*
* @retval -1 buffer1ָ��Ļ�����С��buffer2ָ��Ļ�����
* @retval 0 buffer1ָ��Ļ���������buffer2ָ��Ļ�����
* @retval 1 buffer1ָ��Ļ���������buffer2ָ��Ļ�����
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
**/
int32_t TEE_MemCompare(void *buffer1, void *buffer2, uint32_t size);

/**
* @brief ���bufferָ��Ļ������ķ���Ȩ��
*
* @par ����:
* ��ǰ�汾û��ʵ�֡�
*
* @attention ��
* @param accessFlags [IN] ���ķ�������
* @param buffer [IN] ָ����Ҫ�����ڴ��ָ��
* @param size [IN] ��Ҫ�����ڴ�Ĵ�С
*
* @retval #TEE_SUCCESS ���ڴ�ӵ��accessFlagsָ���ķ���Ȩ��
* @retval #TEE_ERROR_ACCESS_DENIED ���ڴ�û����accessFlagsָ���ķ���Ȩ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
**/
TEE_Result TEE_CheckMemoryAccessRights(uint32_t accessFlags, void *buffer, size_t size);

/**
* @ingroup grp_mem_api
* @brief
* ����һ��ȫ�ֱ�������ͬһInstance�ڵĶ��Session�乲��\n
* Instance��Session�ľ����������û������ֲ���Ӧ�½ڡ�
*
* @par ����:
* ��
*
* @attention ��
* @param instanceData [IN] ���õ�ȫ�ֱ����ĵ�ַ
*
* @retval ��
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
**/
void TEE_SetInstanceData(void *instanceData);

/**
* @ingroup grp_mem_api
* @brief ��ȡTEE_SetInstanceData���õ�ȫ�ֱ�����ָ��
*
* @par ����:
* ��
*
* @attention ��
* @param ��
*
* @retval ������NULL��ֵ ָ��TEE_SetInstanceData���õ�ȫ�ֱ�����ָ��
* @retval NULL û��InstanceData������
*
* @par ����:
* @li tee_internal_api.h���ڲ��������Ͷ���
**/
void *TEE_GetInstanceData(void);

bool TEE_IsSecureMemory(void *buffer, uint32_t size);

uint32_t get_mem_usage(bool show);

#endif

/**
* History: \n
* 2013-05-03��sdk��Create \n
*
* vi: set expandtab ts=4 sw=4 tw=80:
*/
