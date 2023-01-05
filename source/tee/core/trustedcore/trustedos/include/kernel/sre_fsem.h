/**
 * @file sre_fsem.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * �����������ź����Ķ���ͷ�ļ����� \n
 */
#ifndef _OS_FSEM_H
#define _OS_FSEM_H

#include "sre_base.h"
#include "sre_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**@defgroup SRE_fsem �����ź���
  *@ingroup SRE_comm
*/

/**
 * @ingroup SRE_fsem
 * �������������PID��
 */
typedef UINT32 TASK_PID_T;

/**
 * @ingroup SRE_fsem
 * �����ź���������ȡʱ������PID�Ƿ���������ΪIDLE����
 */
#define OS_FSEM_COUNT_ERR   0x80000000

/**
 * @ingroup SRE_fsem
 * �����ź���������ȡʱ������δ������
 */
#define OS_FSEM_TASK_NO_CREATE   0x80000001

/**
 * @ingroup SRE_fsem
 * �����ź��������룺�������Ƿ��������δ������ΪIdle����
 *
 * ֵ: 0x02001000
 *
 * �������������������Ƿ�Ϊ�Ѵ���
 */
#define OS_ERRNO_FSEM_TASK_INVALID        SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x00)
/**
 * @ingroup SRE_fsem
 * �����ź��������룺�����ź�������ֵ�趨Ϊ��ֵ
 *
 * ֵ: 0x02001001
 *
 * ��������������ź����趨Ϊ�Ǹ�ֵ��PEND�����ĵȴ�����С��1
 */
#define OS_ERRNO_FSEM_SET_NEGATIVE        SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x01)

/**
 * @ingroup SRE_fsem
 * �����ź��������룺Pend�����ź���ʱָ���ĵȴ���ֵ�Ƿ�
 *
 * ֵ: 0x02001002
 *
 * ���������ȷ��Pend�����ź���ʱָ���ĵȴ���ֵ��[1, 0x7FFFFFFF]��Χ��
 */
#define OS_ERRNO_FSEM_PEND_VAL_INVALID    SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x02)

/**
 * @ingroup SRE_fsem
 * �����ź��������룺�����ź�����ȡ�������ó�������
 *
 * ֵ: 0x02001003
 *
 * �����������Ҫ���ж���PEND�����ź���
 */
#define OS_ERRNO_FSEM_PEND_INTERR         SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x03)
/**
 * @ingroup SRE_fsem
 * �����ź��������룺�����л�����ʱ����ֹ���������ڿ����ź���
 *
 * ֵ: 0x02001004
 *
 * �����������Ҫ���������PEND�����ź���
 */
#define OS_ERRNO_FSEM_PEND_IN_LOCK        SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x04)
/**
 * @ingroup SRE_fsem
 * �����ź��������룺POST�����ź���ʱ��������ֵ����������为����
 *                   ����post�˼�����ź������˴������ڽ��շ�����
 *
 * ֵ: 0x02001005
 *
 * ����������Կ����ź�������PEND����
 */
#define OS_ERRNO_FSEM_OVERFLOW            SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x05)
/**
 * @ingroup SRE_fsem
 * �����ź��������룺���������ڿ����ź���
 *
 * ֵ: 0x02001006
 *
 * ������������ÿ����ź�����ֵʱ��ȷ������û�б������ڿ����ź���
 */
#define OS_ERRNO_FSEM_PENDED              SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x06)

/**
 * @ingroup SRE_fsem
 * �����ź��������룺�˼�����ź���postʧ�ܣ����ն����񲻴���
 *                   �˴������ڽ��շ�����
 *
 * ֵ: 0x02001007
 *
 * �������: �鿴���ն������Ƿ���Ч
 */
#define OS_ERRNO_FSEM_MCPOST_FAIL         SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x07)
/**
 * @ingroup SRE_fsem
 * �����ź��������룺�����ź���postʧ�ܣ���ͼpost�Ǳ�������ȴδ�򿪺˼�����ź���ģ���������
 *
 * ֵ: 0x02001008
 *
 * �������: �������Ƿ��Ԥ�����Ǻϣ����Ǻϣ���������ź���ģ�����ÿ����Ƿ��
 */
#define OS_ERRNO_FSEM_POST_FAIL          SRE_ERRNO_OS_ERROR(OS_MID_FSEM, 0x08)


/**
 *@ingroup SRE_fsem
 *@brief ���ÿ����ź�����ֵ��
 *
 *@par ����:
 *����ָ����������ź�����ֵ�������������Ϊ�����ź��������ţ������ʧ�ܡ�
 *@attention ��˻����£�ֻ�ܶԱ���������д˲�����
 *
 *@param uwTaskPID [IN] ����#TASK_PID_T������PID����Դ��ͨ��Huntģ��ӿڻ�ȡ���������PID��
 *@param swFsemVal [IN] ����#INT32���趨�Ŀ����ź�����ֵ��ȡֵ��ΧΪ[0, 0x7FFFFFFF]��
 *
 *@retval #SRE_OK                 0x00000000�������ɹ���
 *@retval #OS_ERRNO_FSEM_TASK_INVALID 0x02001000��������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_FSEM_SET_NEGATIVE 0x02001001���趨��ֵ�����涨��Χ��
 *@retval #OS_ERRNO_FSEM_PENDED       0x02001005���������������ڸ��ź�������ֹ���衣
 *@par ����:
 *@li sre_fsem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since UniDSP V100R001C02
 *@see SRE_FsemGet
*/
extern UINT32 SRE_FsemSet(TASK_PID_T uwTaskPID, INT32 swFsemVal);

/**
 *@ingroup SRE_fsem
 *@brief ��ȡ�����ź�����������ֵ��
 *
 *@par ����:
 *��ѯָ������Ŀ����ź�������ֵ�����Ϊ����ʾ��ǰ�����������ڿ����ź�����
 *@attention
 *@li ��˻����£�ֻ�ܶԱ���������д˲�����
 *@li ����ֵΪ�з�����������ͨ�����Ƿ�Ϊ#OS_FSEM_COUNT_ERR�жϻ�ȡ�Ƿ�ɹ���
 *@li �����ź���Ĭ�ϳ�ʼֵΪ0��
 *
 *@param uwTaskPID [IN] ����#TASK_PID_T������PID����Դ��Huntģ���ȡ���������PID��
 *
 *@retval #OS_FSEM_COUNT_ERR 0x80000000����ȡʧ�ܣ�ԭ���ǿ����ź���������ȡʱ������PID�Ƿ���������ΪIDLE����
 *@retval #OS_FSEM_TASK_NO_CREATE 0x80000001����ȡʧ�ܣ�ԭ���ǿ����ź���������ȡʱ������δ������
 *@retval ����ֵ             0xF80000001~0x07FFFFFFF�������ź���������ֵ��
 *@par ����:
 *@li sre_fsem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since UniDSP V100R001C02
 *@see SRE_FsemSet
*/
extern INT32 SRE_FsemGet(TASK_PID_T uwTaskPID);

/**
 *@ingroup SRE_fsem
 *@brief �ȴ������ź�����
 *
 *@par ����:
 *�ȴ������Լ��Ŀ����ź�������������ȥ����ĵȴ����������Ϊ����������������
 *@attention
 *@li ��osStart֮ǰ���ܵ��øýӿ�
 *@li �ýӿ�ֻ���������е��ȡ�
 *@li �����ź�����ʼֵĬ��Ϊ0��
 *
 *@param swFsemVal [IN] ����#INT32�������ź����ĵȴ�����ȡֵ��ΧΪ[1, 0x7FFFFFFF]��
 *
 *@retval #SRE_OK                 0x00000000�������ɹ���
 *@retval #OS_ERRNO_FSEM_PEND_VAL_INVALID 0x02001002���ȴ����Ƿ��������涨��Χ��
 *@retval #OS_ERRNO_FSEM_PEND_INTERR  0x02001003���ýӿڽ�ֹ�жϴ��������á�
 *@retval #OS_ERRNO_FSEM_PEND_IN_LOCK 0x02001004�������л�����ʱ����ֹ������������
 *@par ����:
 *@li sre_fsem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since UniDSP V100R001C02
 *@see SRE_FsemPost
*/
extern UINT32 SRE_FsemPend(INT32 swFsemVal);

/**
 *@ingroup SRE_fsem
 *@brief ����ָ����������ź�����
 *
 *@par ����:
 *��ָ����������ź���ֵ��1����������Ӹ�����Ϊ0���Ѹ�����
 *@attention
 *@li ��osStart֮ǰ���ܵ��øýӿ�
 *@li ������ѵ�����Ǳ��ˣ���ᴥ���˼��жϣ����ͻ�������Ĳ�����
 *@li ������ѵ������ڱ����Ҹ��ڵ�ǰ����������̷��������л���
 *@li �ýӿ��ܵ������κ������̣߳����жϡ�Ӳ�жϡ����񣩡�
 *
 *@param uwTaskPID [IN] ����#TASK_PID_T������PID����Դ��Huntģ���ȡ���������PID��
 *
 *@retval #SRE_OK                 0x00000000�������ɹ���
 *@retval #OS_ERRNO_FSEM_TASK_INVALID 0x02001000��������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_FSEM_OVERFLOW     0x02001005�������ź����Ӳ����������
 *@retval #OS_ERRNO_FSEM_POST_FAIL    0x02001008�������ź���postʧ�ܣ���ͼpost�Ǳ�������ȴδ�򿪺˼�����ź���ģ��������ء�
 *@retval #OS_ERRNO_IPC_SHQ_FULL      0x02001486��6181��6108ƽ̨�£����ͺ˼��жϣ��������������
 *@par ����:
 *@li sre_fsem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since V100R001C02
 *@see SRE_FsemPend
*/
extern UINT32 SRE_FsemPost(TASK_PID_T uwTaskPID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //_OS_FSEM_H


/*
 * History: \n
 * 2009-10-30, sdk, Create this file. \n
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

