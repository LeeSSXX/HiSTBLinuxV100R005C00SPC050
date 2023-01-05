/**
  * @file sre_hunt.h
  *
  * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
  *
  * ������PID��ȡ�Ķ���ͷ�ļ���\n
  */

#ifndef _SRE_HUNT_H
#define _SRE_HUNT_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/**@defgroup sre_hunt Hunt����
  *@ingroup SRE_comm
  */

/**
 * @ingroup sre_hunt
 * HUNT�����룺HUNTģ���ʼ��ʧ��.
 *
 * ֵ: 0x02001f00
 *
 * ���������ȷ�����빲��̬fsc�ڴ����˽��fsc�ڴ�����Ƿ�ʧ��
 */
#define OS_ERRNO_HUNT_INIT_ERROR        SRE_ERRNO_OS_ERROR(OS_MID_HUNT, 0x00)

/**
 * @ingroup sre_hunt
 * HUNT�����룺������������ж�ʱ���߳�������.
 *
 * ֵ: 0x02001f01
 *
 * ���������������������ж�ʱ��ȷ���߳���������Ψһ.
 */
#define OS_ERRNO_HUNT_THREAD_NAME_REPEAT    SRE_ERRNO_OS_ERROR(OS_MID_HUNT, 0x01)

/**
 * @ingroup sre_hunt
 * HUNT�����룺ͨ���߳�ID���߳�����ȡ�߳�PIDʱ�����߳�δ��������ɾ��.
 *
 * ֵ: 0x02001f02
 *
 * ���������ȷ���߳��Ѵ�����δ��ɾ��.
 */
#define OS_ERRNO_HUNT_THREAD_NOT_CREAT      SRE_ERRNO_OS_ERROR(OS_MID_HUNT, 0x02)

/**
 * @ingroup sre_hunt
 * HUNT�����룺ͨ���߳�ID���߳�����ȡ�߳�PIDʱ����ηǷ�.
 *
 * ֵ: 0x02001f03
 *
 * ���������ȷ��ָ��ǿգ��Լ��˺źϷ�.
 */
#define OS_ERRNO_HUNT_ILLEGAL_INPUT         SRE_ERRNO_OS_ERROR(OS_MID_HUNT, 0x03)

/**
*@ingroup sre_hunt
*@brief ͨ��ָ���˵��߳�(�������ж�)����ȡ��PID��
*
*@par ����:
*ͨ�����ӿڻ�ȡ�߳�PID��
*@attention
*SRE_HuntByName������ʹ�������ַ�ʽ���û����Զ�ѡһ:
*1��ÿ���˶���OS��ʼ�����֮�����SRE_HuntMature��Ȼ�����SRE_HuntByName��
*2��ÿ���˶���OS��ʼ�����֮�����SRE_WaitForAllCores��Ȼ�����SRE_HuntByName;
*@param ucCoreID  [IN] ����Ϊ#UINT8���߳����ں˺ţ�С�ں�����
*@param pThreadName [IN] ����Ϊ#CHAR*���߳���,������15���ַ���
*@param puwPID [OUT] ����Ϊ#UINT32*���߳�PID��
*
*@retval #SRE_OK ,��ȡPID�ɹ���
*@retval #OS_ERRNO_HUNT_THREAD_NOT_CREAT 0x02001f02�����߳�δ��������ɾ����
*@retval #OS_ERRNO_HUNT_ILLEGAL_INPUT    0x02001f03����ηǷ���
*@par ����:
*sre_hunt.h���ýӿ��������ڵ�ͷ�ļ���
*@since RTOSck V100R001C01
*@see SRE_HuntMature
*/
extern UINT32 SRE_HuntByName(UINT8 ucCoreID, CHAR  *pThreadName, UINT32 *puwPID);


/**
*@ingroup sre_hunt
*@brief ��Ǳ������е���������ж��Ѵ�����ɣ�OS��IDLE���⣩�������˿��Գɹ���ѯ���˵�PID��
*
*@par ����:
*���ϱ���mature��־��ͬʱ�ñ�־��ʹ�ñ����ڵ���#SRE_HuntByNameʱ�����Ŀ�ĺ�û����������һֱ�ȴ���ֱ��Ŀ�ĺ�������mature��־�ŷ��ز�ѯ�����
*@attention
*�����ڱ������е���������жϣ�OS��IDLE���⣩�������֮����á�SRE_HuntByName������SRE_HuntMatureִ��֮�����.
*@param �ޡ�
*
*@retval �ޡ�
*@par ����:
*sre_hunt.h���ýӿ��������ڵ�ͷ�ļ���
*@since RTOSck V100R001C01
*@see SRE_HuntByName
*/
extern VOID SRE_HuntMature(VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SRE_HUNT_H */

/**
 * History:
 * 2010-03-10 sdk: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */
