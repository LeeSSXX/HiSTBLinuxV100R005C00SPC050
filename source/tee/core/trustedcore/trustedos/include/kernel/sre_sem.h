/**
 * @file sre_sem.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * �������ź���ģ�����ͷ�ļ��� \n
 */

#ifndef _SRE_SEM_H
#define _SRE_SEM_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**@defgroup SRE_sem �ź���
  *@ingroup SRE_comm
  */

/**
 * @ingroup SRE_sem
 * �ź����ȴ�ʱ���趨����ʾ���õȴ���
 */
#define OS_WAIT_FOREVER     0xFFFFFFFF

/**
 * @ingroup SRE_sem
 * �ź��������룺��ʼ���ź����ڴ治��
 *
 * ֵ: 0x02000f00
 *
 * �������: ���Խ�˽�еľ�̬�ڴ�ռ����
 */
#define OS_ERRNO_SEM_NO_MEMORY         SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x00)
/**
 * @ingroup SRE_sem
 * �ź��������룺�ź�������Ƿ����������ɾ����
 *
 * ֵ: 0x02000f01
 *
 * �������: �鿴������ź������ֵ�Ƿ���Ч
 */
#define OS_ERRNO_SEM_INVALID           SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x01)
/**
 * @ingroup SRE_sem
 * �ź��������룺����ָ��Ϊ��
 *
 * ֵ: 0x02000f02
 *
 * �������: �鿴ָ���Ƿ�����Ϊ��
 */
#define OS_ERRNO_SEM_PTR_NULL       SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x02)
/**
 * @ingroup SRE_sem
 * �ź��������룺û�п����ź���
 *
 * ֵ: 0x02000f03
 *
 * �������: �鿴�ź���ģ���Ƿ�򿪣������ø����ź���
 */
#define OS_ERRNO_SEM_ALL_BUSY          SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x03)
/**
 * @ingroup SRE_sem
 * �ź��������룺�ź�����ȡʧ��
 *
 * ֵ: 0x02000f04
 *
 * �������: �鿴�Ƿ񽫵ȴ�ʱ�����ó�0����ֵ��Ч��
 */
#define OS_ERRNO_SEM_UNAVAILABLE       SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x04)
/**
 * @ingroup SRE_sem
 * �ź��������룺��ֹ�жϴ������������ź���
 *
 * ֵ: 0x02000f05
 *
 * �������: �鿴�Ƿ����ж���Pend�ź���
 */
#define OS_ERRNO_SEM_PEND_INTERR       SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x05)
/**
 * @ingroup SRE_sem
 * �ź��������룺�����л�����ʱ����ֹ�����������ź���
 *
 * ֵ: 0x02000f06
 *
 * �������: ��
 */
#define OS_ERRNO_SEM_PEND_IN_LOCK      SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x06)
/**
 * @ingroup SRE_sem
 * �ź��������룺�ź����ȴ���ʱ
 *
 * ֵ: 0x02000f07
 *
 * �������: ��
 */
#define OS_ERRNO_SEM_TIMEOUT           SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x07)
/**
 * @ingroup SRE_sem
 * �ź��������룺�ź����������
 *
 * ֵ: 0x02000f08
 *
 * �������: �鿴������ź�������ֵ�Ƿ���Ч
 */
#define OS_ERRNO_SEM_OVERFLOW          SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x08)
/**
 * @ingroup SRE_sem
 * �ź��������룺���������ź���������
 *
 * ֵ: 0x02000f09
 *
 * �������: ��
 */
#define OS_ERRNO_SEM_PENDED            SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x09)

/**
 * @ingroup SRE_sem
 * �ź��������룺�ź���������ȡʧ��
 *
 * ֵ: 0x02000f0A
 *
 * �������: �鿴�ź�������Ƿ�Ϸ������ź����Ƿ��Ѵ�����
 */
#define OS_ERRNO_SEM_GET_COUNT_ERR      SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x0A)


/**
 * @ingroup SRE_sem
 * �ź��������룺ע������ź���ʧ��
 *
 * ֵ: 0x02000f10
 *
 * �������: �鿴�ź���ģ����Ϣ�����Ƿ���ȷ
 */
#define OS_ERRNO_SEM_REG_ERROR         SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x10)

/**
 * @ingroup SRE_sem
 * �ź��������룺����#SRE_SemPendListGetʱ��ָ�����ڴ�ռ䲻�㣬�޷�����ȫ������������PID��
 *
 * ֵ: 0x02000f11
 *
 * �������: ���齫���鳤������Ϊ(#OS_TSK_MAX_SUPPORT_NUM + 1) * 4
 */
#define OS_ERRNO_SEM_INPUT_BUF_NOT_ENOUGH    SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x11)

/**
 * @ingroup SRE_sem
 * �ź��������룺����#SRE_SemPendListGetʱ������ָ��Ϊ�ջ���uwBufLenС��4
 *
 * ֵ: 0x02000f12
 *
 * �������: ��
 */
#define OS_ERRNO_SEM_INPUT_ERROR       SRE_ERRNO_OS_ERROR(OS_MID_SEM, 0x12)

/**
 * @ingroup SRE_sem
 * �ź���������Ͷ��塣
 */
typedef UINT32 SEM_HANDLE_T;

/**
 * @ingroup SRE_sem
 * �ź���ģ��������Ϣ�Ľṹ�嶨�塣
 */
typedef struct tagSemModInfo {
	UINT16 usMaxNum;                /**< ���֧�ֵ��ź�����  */
} SEM_MOD_INFO_S;


/**
 *@ingroup SRE_sem
 *@brief ����һ���ź�����
 *
 *@par ����:
 *�����û�ָ���ļ���ֵ������һ���ź������趨��ʼ��������ֵ��
 *@attention
 *@li �����Ƿ�ɹ����ܵ�"�����ź����ü�����"��"���֧���ź���"����������ơ�
 *
 *@param uwCount [IN] ����Ϊ#UINT32����������ʼֵ��ȡֵ��ΧΪ[0, 0xFFFFFFFE]��
 *@param puwSemHandle [OUT] ����Ϊ#SEM_HANDLE_T *������ź��������
 *
 *@retval #SRE_OK            0x00000000�������ɹ���
 *@retval #OS_ERRNO_SEM_OVERFLOW 0x02000f08����������ʼֵ������Χ��
 *@retval #OS_ERRNO_SEM_ALL_BUSY 0x02000f03��û�п����ź�������������"���֧���ź���"���á�
 *@retval #OS_ERRNO_SEM_PTR_NULL 0x02000f02�����ָ��Ϊ�ա�
 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_SemDelete
*/
extern  UINT32 SRE_SemCreate(UINT32 uwCount,  SEM_HANDLE_T *puwSemHandle);

/**
 *@ingroup SRE_sem
 *@brief ɾ��һ���ź�����
 *
 *@par ����:
 *ɾ���û�������ź������ָ�����ź�������������������ڸ��ź�������ɾ��ʧ�ܡ�
 *@attention ��
 *@li ����ɾ���������������ź�����
 *
 *@param uwSemHandle [IN] ����Ϊ#SEM_HANDLE_T���ź����������Դ���ź��������ɹ������ֵ��
 *
 *@retval #SRE_OK           0x00000000�������ɹ���
 *@retval #OS_ERRNO_SEM_INVALID 0x02000f01���ź������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_SEM_PENDED  0x02000f09�������������ڸ��ź�������ֹɾ����
 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_SemCreate
*/
extern UINT32 SRE_SemDelete(SEM_HANDLE_T uwSemHandle);

/**
 *@ingroup SRE_sem
 *@brief �����ź�����������ֵ������
 *
 *@par ����:
 *�����û������ź�������ͼ���ֵ�������ź�����������ֵ��
 *@attention ��
 *@li ��������������ڸ��ź�����������ʧ�ܡ�
 *
 *@param uwSemHandle [IN] ����Ϊ#SEM_HANDLE_T���ź����������Դ���ź��������ɹ������ֵ��
 *@param uwCount [IN] ����Ϊ#UINT32���������趨ֵ��ȡֵ��ΧΪ[0, 0xFFFFFFFE]��
 *
 *@retval #SRE_OK            0x00000000�������ɹ���
 *@retval #OS_ERRNO_SEM_INVALID  0x02000f01���ź������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_SEM_OVERFLOW 0x02000f08���������趨ֵ������Χ��
 *@retval #OS_ERRNO_SEM_PENDED   0x02000f09�������������ڸ��ź�������ֹ���衣
 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_SemCreate | SRE_SemCountGet
*/
extern UINT32 SRE_SemReset(SEM_HANDLE_T uwSemHandle, UINT32 uwCount);

/**
 *@ingroup SRE_sem
 *@brief ��ȡ�ź�����������ֵ��
 *
 *@par ����:
 *�����û������ź�������ͼ���ֵ����ȡ�ź�����������ֵ��
 *@attention ��
 *
 *@param uwSemHandle [IN]  ����Ϊ#SEM_HANDLE_T���ź����������Դ���ź��������ɹ������ֵ��
 *@param puwSemCnt   [OUT] ����Ϊ#UINT32 *���ź�������ֵ����Χ[0~0xfffffffe]��
 *
 *@retval #OS_ERRNO_SEM_GET_COUNT_ERR   0x02000f0A ��ȡʧ�ܣ��ź������������ѱ�ɾ����
 *@retval #SRE_OK                       0x00000000 ��ȡ�ź���������ֵ�ɹ���
 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_SemCreate | SRE_SemReset
*/
extern UINT32 SRE_SemCountGet(SEM_HANDLE_T uwSemHandle, UINT32 *puwSemCnt);


/**
 *@ingroup SRE_sem
 *@brief �ȴ�һ���ź�����
 *
 *@par ����:
 *�ȴ��û������ź������ָ�����ź��������������ֵ����0����ֱ�Ӽ�1���سɹ�����������������
 *�ȴ������̷߳������ź������ȴ�������ʱ����趨��
 *@attention
 *@li ��osStart֮ǰ���ܵ��øýӿڡ�
 *@li �ȴ�ʱ�����ѡ����ȴ����ȴ��ض�ʱ�䡢���õȴ���
 *@li ��û���ź�����Դʱ������ȴ�ʱ�ú���ֻ�ܱ�������á�
 *@li ������������£��û�ҪPEND�ź�����Ҫ��֤��ǰ�п����ź�����Դ��
 *@li �������жϻ�Ӳ�ж��е��ô˺���ʱ�����п�����Դ��pend�ɹ�������pendʧ�ܣ�uwTimeout������Ч��
 *
 *@param uwSemHandle [IN] ����Ϊ#SEM_HANDLE_T���ź����������Դ���ź��������ɹ������ֵ��
 *@param uwTimeout [IN] ����Ϊ#UINT32���ȴ�ʱ�����ƣ���λΪtick��ȡֵ��ΧΪ[0, 0xffffffff]��
 *#OS_NO_WAIT��0��ʾ���ȴ���#OS_WAIT_FOREVER��0xffffffff��ʾ���õȴ���
 *
 *@retval #SRE_OK                0x00000000�������ɹ���
 *@retval #OS_ERRNO_SEM_INVALID      0x02000f01���ź������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_SEM_UNAVAILABLE  0x02000f04���ź�����ȴ�ʱ��ȡʧ�ܡ�
 *@retval #OS_ERRNO_SEM_PEND_INTERR  0x02000f05���жϴ�������ֹ����������
 *@retval #OS_ERRNO_SEM_PEND_IN_LOCK 0x02000f06��ϵͳ������ԴΪ0�������л�����ʱ����ֹ������������
 *@retval #OS_ERRNO_SEM_TIMEOUT      0x02000f07���ź����ȴ���ʱ��
 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_SemPend
*/
extern UINT32 SRE_SemPend(SEM_HANDLE_T uwSemHandle, UINT32 uwTimeout);

/**
 *@ingroup SRE_sem
 *@brief ����ָ�����ź�����
 *
 *@par ����:
 *����ָ�����ź�������û������ȴ����ź�������ֱ�ӽ���������1���ء�
 *������Ϊ���ź��������������б��еĵ�һ���������������ģ���
 *@attention
 *@li ��osStart֮ǰ���ܵ��øýӿ�
 *@li ��δ�����������£�������ѵ��������ȼ����ڵ�ǰ����������̷��������л���
 *
 *@param uwSemHandle [IN] ����Ϊ#SEM_HANDLE_T���ź����������Դ���ź��������ɹ������ֵ��
 *
 *@retval #SRE_OK            0x00000000�������ɹ���
 *@retval #OS_ERRNO_SEM_INVALID  0x02000f01���ź������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_SEM_OVERFLOW 0x02000f08���ź����Ӳ����������
 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_SemPend
*/
extern UINT32 SRE_SemPost(SEM_HANDLE_T uwSemHandle);

/**
 *@ingroup SRE_sem
 *@brief ��ȡ������ָ�������ź����ϵ�����PID�嵥��
 *
 *@par ����:
 *�����û�ָ���ĺ˼��ź����������ȡ������ָ�������ź����ϵ�����PID�嵥���������������ڸú����ź������򷵻������ڸú����ź�����������Ŀ���Լ���Ӧ�����PID��
 *��û�����������ڸú����ź������򷵻������ڸú����ź�����������ĿΪ0��
 *@attention
 *@li �û�Ӧ��֤�洢����PID�嵥���ڴ�ռ��㹻�󣬽��齫uwBufLen����Ϊ(#OS_TSK_MAX_SUPPORT_NUM + 1) * 4bytes��
 *
 *@param uwSemHandle   [IN]  ����Ϊ#SEM_HANDLE_T���ź����������Դ���ź��������ɹ������ֵ��
 *@param puwTskCnt [OUT] ����Ϊ#UINT32 *�������ڸú����ź�������������
 *@param puwPidBuf [OUT] ����Ϊ#UINT32 *�����û�ָ�����ڴ������׵�ַ�����ڴ洢������ָ�������ź���������PID��
 *@param uwBufLen  [IN]  ����Ϊ#UINT32���û�ָ�����ڴ�����ĳ��ȣ���λ���ֽڣ���
 *
 *@retval #SRE_OK                        0x00000000�������ɹ���
 *@retval #OS_ERRNO_SEM_INVALID              0x02000f01���ź������Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_SEM_INPUT_ERROR          0x02000f12��ָ��Ϊ�ջ���uwBufLenС��4��
 *@retval #OS_ERRNO_SEM_INPUT_BUF_NOT_ENOUGH 0x02000f11��ָ�����ڴ�ռ䲻�㣬���ڴ��е�����PID��Ч��

 *@par ����:
 *@li sre_sem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_TaskPendSemGet | SRE_McSemPendListGet
*/
extern UINT32 SRE_SemPendListGet(SEM_HANDLE_T uwSemHandle,
				 UINT32 *puwTskCnt,
				 UINT32 *puwPidBuf,
				 UINT32  uwBufLen);
extern UINT32 SRE_SemRelease(UINT32 uwTaskID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _SRE_SEM_H */

/*
 * History: \n
 * 2009-3-4, sdk, Create this file. \n
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */
