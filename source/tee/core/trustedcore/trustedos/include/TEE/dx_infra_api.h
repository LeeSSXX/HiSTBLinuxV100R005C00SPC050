/*******************************************************************************
* All rights reserved, Copyright (C) huawei LIMITED 2012
*------------------------------------------------------------------------------
* File Name   : dx_infra_api.h
* Description :
* Platform    :
* Author      : qiqingchao
* Version     : V1.0
* Date        : 2013.5.17
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
* @file dx_infra_api.h
*
* Copyright(C), 2008-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
*
* ������DX����API���ݽṹ�ͽӿڶ���\n
*/

/** @defgroup dx_infra_api DX��ȫ��ӿ�
* @ingroup TEE_API
*/

#ifndef _DX_INFRA_API_
#define _DX_INFRA_API_

#include "tee_internal_api.h"
/**
 * @ingroup dx_infra_api
 * �޷������Ͷ���
 */
typedef unsigned int              DxUint32;
/**
 * @ingroup dx_infra_api
 * �޷����ַ��Ͷ���
 */
typedef unsigned char             DxByte;
#if 1
//#ifndef CC_DRIVER
/**
 * @ingroup dx_infra_api
 * ���ؽ������
 */
typedef uint32_t DxBool;
#else
/**
 * @ingroup dx_infra_api
 * ״̬��������
 */
typedef DxBool_t DxBool;
#endif
/**
 * @ingroup dx_infra_api
 * ����״̬ö�ٶ���
 */
typedef enum {
	CONNECTED = 0,              /**< 0 ������ */
	UNCONNECTED                 /**< 1 δ���� */
} EDxTzConnectionStatus_t;
/**
 * @ingroup dx_infra_api
 * ��ȫ�ڴ��־λ 0:����ȫ����;1:�ǰ�ȫ�Ͱ�ȫ���ܷ���
 */
typedef enum {
	INIT = 0,                   /**< 0 �����ڴ�Ϊ����ȫ���� */
	TERMINATE                   /**< 1 �����ڴ�Ϊ��ȫ�ͷǰ�ȫ���ܷ��� */
} CP_Flag;
/**
 * @ingroup dx_infra_api
 * ��ȫ�ڴ����ݽṹ����
 */
typedef struct {
	DxByte     *addr;           /**< �����ڴ��ַ */
	DxUint32    size;           /**< �����ڴ泤�� */
	CP_Flag     flag;           /**< �����ڴ��ʶ */
} ContentPath;
/**
 * @ingroup dx_infra_api
 * ��ȫ�ڴ�����ݺ͵�ַ����Ҫ32KB����
 */
#define ALIGN_SIZE 0x8000 //32KB
/**
 * @ingroup  dx_infra_api
 * @brief g_dx_content_path_addr �޸�DX��ȫ�ڴ��ַȫ�ֱ���
 *
 * @par ����:
 * DX��ȫ�ڴ�ͨ��ȫ�ֱ��������ַ���ýӿڻ��ȫ�ֱ��������޸�
 *
 * @attention ȫ�ֱ����(�����ڴ�)ֻ��һ������֧�ֲ�������
 * @param contentpath [IN]  ��ȫ�ڴ����ݽṹ #ContentPath
 *
 * @retval #FALSE ȫ�ֱ�����ֵʧ��
 * @retval #TRUE ȫ�ֱ�����ֵ�ɹ�
 *
 * @par ����:
 * @li dx_infra_api DX����API
 * @li tee_internal_api.h TEE�������ݶ���
 * @see DxTzService_IsSecureContentMemory
 * @since V100R002C00B302
*/
DxBool g_dx_content_path_addr(ContentPath *contentpath);
/**
 * @ingroup  dx_infra_api
 * @brief DxTzService_IsDeviceRooted �Ӱ�ȫ�����豸�Ƿ�root
 *
 * @par ����:
 * �Ӱ�ȫ�����豸�Ƿ�root�������root��ӵ��rootȨ��
 *
 * @attention ��
 * @param ��
 *
 * @retval 0 û��root
 * @retval 1 �Ѿ�root
 *
 * @par ����:
 * @li dx_infra_api DX����API
 * @li tee_internal_api.h TEE�������ݶ���
 * @see ��
 * @since V100R002C00B302
*/
DxUint32  DxTzService_IsDeviceRooted(void);
/**
 * @ingroup  dx_infra_api
 * @brief DxTzService_IsSecureContentMemory �ڰ�ȫ����һ���ڴ��Ƿ�Ϊ����ȫ����
 *
 * @par ����:
 * �ڰ�ȫ����һ���ڴ��Ƿ�Ϊ����ȫ����
 *
 * @attention ��
 * @param mem [IN] �ڴ��ַ #DxByte
 * @param len [IN] �ڴ泤�� #DxUint32
 *
 * @retval #TEE_SUCCESS ��ȫ��־λ���óɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS ��������
 *
 * @par ����:
 * @li dx_infra_api DX����API
 * @li tee_internal_api.h TEE�������ݶ���
 * @see DxTzService_IsSecureContentMemory
 * @since V100R002C00B302
*/
DxBool DxTzService_IsSecureContentMemory(DxByte *mem, DxUint32 len);
#endif
/**
* History: \n
* 2013-06-25��sdk��Create \n
*
* vi: set expandtab ts=4 sw=4 tw=80:
*/
