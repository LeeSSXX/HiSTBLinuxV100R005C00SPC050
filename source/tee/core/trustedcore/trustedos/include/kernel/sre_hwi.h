/**
 * @file sre_hwi.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������Ӳ�ж�ģ��Ķ���ͷ�ļ��� \n
 */

/**@defgroup SRE_interrupt �ж�
  *@ingroup SRE_kernel
*/
/** @defgroup SRE_hwi Ӳ�ж�
 *@ingroup SRE_interrupt
*/

#ifndef _SRE_HWI_H
#define _SRE_HWI_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#define SRE_CPU0    0
#define SRE_CPU1    1
#define SRE_CPU2    2
#define SRE_CPU3    3


/**
 * @ingroup SRE_hwi
 * Ӳ�ж�������ȼ���
 */
#define SRE_HWI_HIGHEST_PRIORITY      SRE_HWI_PRIORITY0

/**
 * @ingroup SRE_hwi
 * Ӳ�ж�֧�ֵ����ȼ���Ŀ��
 */
#define SRE_HWI_PRIORITIES_NUMBER     32

/**
 * Ӳ�ж�ģʽ�ĺ궨�塣
 */

/**
 * @ingroup SRE_hwi
 * �����Ӳ�жϡ�
 */
#define OS_HWI_MODE_COMBINE                                 0x8000

/**
 * @ingroup SRE_hwi
 * ������Ӳ�жϡ�
 */
#define OS_HWI_MODE_ENGROSS                                 0x4000

/**
 * @ingroup SRE_hwi
 * ȱʡӲ�ж�ģʽ��
 */
#define OS_HWI_MODE_DEFAULT                                 OS_HWI_MODE_ENGROSS

/**
 * @ingroup SRE_hwi
 * ��Ϊʹ�ܻ���������Ӳ�жϺ�������Σ���Tensilicaƽ̨��Ч��
 */
#define OS_HWI_ALL                                          0x7fff

/*
 * Ӳ�ж�ģ��Ĵ����붨�塣
 */

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺�жϺţ��¼��ţ��Ƿ���
 *
 * ֵ: 0x02001400
 *
 * ���������ȷ���жϺźϷ���Tensilica:[0,31];
 */
#define OS_ERRNO_HWI_NUM_INVALID                            SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x00)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺Ӳ�ж��ѱ���������Ӧ�ж��������ѱ������ж�ռ�á�
 *
 * ֵ: 0x02001402
 *
 * ��������������жϺ�
 */
#define OS_ERRNO_HWI_ALREADY_CREATED                        SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x02)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺Ӳ�жϴ�����Ϊ�ա�
 *
 * ֵ: 0x02001403
 *
 * �������������ǿյ���Ч������
 */
#define OS_ERRNO_HWI_PROC_FUNC_NULL                         SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x03)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺ɾ��TICK�жϡ�
 *
 * ֵ: 0x02001405
 *
 * ��������������жϺ�
 */
#define OS_ERRNO_HWI_DELETE_TICK_INT                        SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x05)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺������жϴ���ʧ�ܣ�Ϊ������жϽڵ�����˽�о�̬�ڴ�ʧ��
 *
 * ֵ: 0x02001408
 *
 * �������: ����˽�о�̬�ڴ������С
 */
#define OS_ERRNO_HWI_MEMORY_ALLOC_FAILED                    SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x08)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺������жϺ���ע��ʧ�ܣ���������ж��Ѵ�������ͬ���жϴ�����
 *
 * ֵ: 0x02001409
 *
 * �������: ��
 */
#define OS_ERRNO_HWI_COMBINEHOOK_ALREADY_CREATED            SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x09)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺�������жϺ��������ǰ�ȫ�ͣ�Ҳ���Ƿǰ�ȫ��
 *
 * ֵ: 0x0200140a
 *
 * �������: ��
 */
#define OS_ERRNO_HWI_MODE_ERROR                             SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x0a)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺ɾ��δ������Ӳ�ж�
 *
 * ֵ: 0x0200140b
 *
 * �������: ��
 */
#define OS_ERRNO_HWI_DELETED                                SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x0b)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺δ����Ӳ�ж�ģʽ����
 *
 * ֵ: 0x0200140c
 *
 * �������: �����жϴ�������ǰ����Ҫ�ȵ����ж�ģʽ���ú���������ģʽ��������
 */
#define OS_ERRNO_HWI_MODE_UNSET                             SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x0c)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺Ӳ�жϴ����ӿ���δ���������Ч�ĺ˺�
 *
 * ֵ: 0x0200140d
 *
 * �������: ���뱾�˺˺�
 */
#define OS_ERRNO_HWI_CORE_ID_INVALID                            SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x0d)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺Ӳ�жϴ����ӿ���δ���������Ч���жϺ�
 *
 * ֵ: 0x0200140e
 *
 * �������: SD6182��CPU,DSP,HAC CC�ˣ���SD6181/SD6108��CPU��ֻ��7�ſ������������SD6181/SD6108��DSP��7����11�ſ����������
 */
#define OS_ERRNO_HWI_NUM_NOT_SOFT_TRIGGERD                      SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x0e)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ����룺�ж����ȼ���������
 *
 * ֵ: 0x0200140f
 *
 * ������������ȼ�ӦС��256
 */
#define OS_ERRNO_HWI_PRIORITY_VALUE                       SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x0f)

/**
 * @ingroup SRE_hwi
 * No free slot for HWI IPC notification
 *
 * ֵ: 0x02001410
 *
 */
#define OS_ERRNO_HWI_IPC_ALL_BUSY                        SRE_ERRNO_OS_ERROR(OS_MID_HWI, 0x10)

/**
 * @ingroup SRE_hwi
 * Ӳ�жϺŵ����Ͷ��塣
 */
typedef UINT32                                              HWI_HANDLE_T;

/**
 * @ingroup SRE_hwi
 * Ӳ�ж����ȼ������Ͷ��塣
 */
typedef UINT16                                              HWI_PRIOR_T;

/**
 * @ingroup SRE_hwi
 * Ӳ�ж�ģʽ������Ϣ�����Ͷ��塣
 */
typedef UINT16                                              HWI_MODE_T;

/**
 * @ingroup SRE_hwi
 * Ӳ�жϴ������Ĳ������Ͷ��塣
 */
typedef UINT32                                              HWI_ARG_T;

/**
* @ingroup  SRE_hwi
* @brief Ӳ�жϴ����������Ͷ��塣
*
* @par ����:
* ͨ��Ӳ�жϴ����������Ͷ���Ӳ�жϴ���������Ӳ�жϴ���ʱ���ø��жϴ�������
*
* @attention �ޡ�
*
* @param  uwParam1 [IN] ����#HWI_ARG_T��Ӳ�жϴ������Ĳ�����
*
* @retval �ޡ�
* @par ����:
* <ul><li>sre_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since UniDSP V100R001C01
* @see �ޡ�
*/
typedef VOID (* HWI_PROC_FUNC)(HWI_ARG_T);


/**
 * @ingroup  SRE_hwi
 * @brief ����Ӳ�ж����Խӿڡ�
 *
 * @par ����:
 * �ڴ���Ӳ�ж�ǰ������Ҫ���ú�Ӳ�жϵ�ģʽ��
 ���������ͣ�#OS_HWI_MODE_ENGROSS��������ͣ�#OS_HWI_MODE_COMBINE����������ģʽ��
 *
 * @attention
 * <ul>
 * <li>Tensilica�µ�SD6181, SD6108, SD6182оƬ������Ӳ�жϺţ��¼��ţ�Ϊ0-31��</li>
 * <li>����SD6181��SD6108�����ȼ�Ϊ1-6��SD6182�����ȼ�Ϊ1-5��</li>
 * <li>Ӳ�жϴ����ɹ��󣬻�ʹ����Ӧ���¼��ţ�������ʹ����Ӧ�������жϣ���Ҫ��ʽ����#SRE_HwiEnable����ʹ�ܡ�</li>
 * </ul>
 *
 * @param  uwHwiNum [IN] ����#HWI_HANDLE_T��Ӳ�жϺš�
 * @param  usHwiPrio [IN] ����#HWI_PRIOR_T��Ӳ�ж����ȼ�������Tensilicaƽ̨�����ȼ���Ӳ���̶����ʴ˲�����Ч��
 * @param  usMode [IN] ����#HWI_MODE_T����Ӳ�жϵĹ���ģʽ�������ͻ�������͡�
 *
 * @retval #OS_ERRNO_HWI_NUM_INVALID                     0x02001400���жϺţ��¼��ţ��Ƿ���
 * @retval #OS_ERRNO_HWI_MODE_ERROR                      0x0200140a���������жϺ��������Ƕ����ͣ�Ҳ��������͡�
 * @retval #OS_ERRNO_HWI_ALREADY_CREATED                 0x02001402���ظ�����Ӳ�жϺ�����
 * @retval #SRE_OK                                       0x00000000��Ӳ�жϴ����ɹ���
 * @par ����:
 * <ul><li>SRE_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_HwiCreate
 **/

/**
 * @ingroup  SRE_hwi
 * @brief ����Ӳ�жϺ�����
 *
 * @par ����:
 * ע��Ӳ�жϵĴ�������
 *
 * @attention
 * <ul>
 * <li>�ڵ��øú���֮ǰ������ȷ���Ѿ��������ж����ԡ�</li>
 * <li>Tensilica�µ�SD6181, SD6108, SD6182оƬ������Ӳ�жϺţ��¼��ţ�Ϊ0-31��</li>
 * <li>����SD6181��SD6108�����ȼ�Ϊ1-6��SD6182�����ȼ�Ϊ1-5��</li>
 * <li>Ӳ�жϴ����ɹ��󣬻�ʹ����Ӧ���¼��ţ�������ʹ����Ӧ�������жϣ���Ҫ��ʽ����#SRE_HwiEnable����ʹ�ܡ�</li>
 * </ul>
 *
 * @param  uwHwiNum [IN] ����#HWI_HANDLE_T��Ӳ�жϺš�
 * @param  pfnHandler [IN] ����#HWI_PROC_FUNC��Ӳ�жϴ���ʱ�Ĵ�������
 * @param  uwArg [IN] ����#HWI_ARG_T������Ӳ�жϴ�����ʱ���ݵĲ�����
 *
 * @retval #OS_ERRNO_HWI_NUM_INVALID                     0x02001400���жϺţ��¼��ţ��Ƿ���
 * @retval #OS_ERRNO_HWI_PROC_FUNC_NULL                  0x02001403��Ӳ�жϴ�����Ϊ�ա�
 * @retval #OS_ERRNO_HWI_MODE_UNSET                      0x0200140c��δ����Ӳ�ж�ģʽ���á�
 * @retval #OS_ERRNO_HWI_MEMORY_ALLOC_FAILED             0x02001408��������жϽڵ�����˽�о�̬�ڴ�ʧ��
 * @retval #OS_ERRNO_HWI_COMBINEHOOK_ALREADY_CREATED     0x02001409��������ж��Ѵ�������ͬ���жϴ�������
 * @retval #OS_ERRNO_HWI_ALREADY_CREATED                 0x02001402��Ӳ�ж��ѱ���������Ӧ�ж��������ѱ������ж�ռ�á�
 * @retval #SRE_OK                                       0x00000000��Ӳ�жϴ����ɹ���
 * @par ����:
 * <ul><li>sre_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_HwiDelete
 **/
extern UINT32 SRE_HwiCreate(HWI_HANDLE_T  uwHwiNum,
			    HWI_PRIOR_T   usHwiPrio,
			    HWI_MODE_T    usMode,
			    HWI_PROC_FUNC pfnHandler,
			    HWI_ARG_T     uwArg);
extern UINT32 SRE_HwiSetTargetCPU(HWI_HANDLE_T uwHwiNum);

/**
 * @ingroup  SRE_hwi
 * @brief ɾ��Ӳ�жϺ�����
 *
 * @par ����:
 * ������ӦӲ�жϻ��¼���ȡ��Ӳ�жϴ�������ע�ᡣ
 *
 * @attention
 * <ul>
 * <li>Tensilica�µ�SD6181, SD6108, SD6182оƬ������Ӳ�жϺţ��¼��ţ�Ϊ0-31��</li>
 * </ul>
 *
 * @param  uwHwiNum [IN] ����#HWI_HANDLE_T��Ӳ�жϺš�
 *
 * @retval #OS_ERRNO_HWI_NUM_INVALID            0x02001400���жϺţ��¼��ţ��Ƿ���
 * @retval #OS_ERRNO_HWI_DELETE_TICK_INT        0x02001405��ɾ��TICK�жϡ�
 * @retval #OS_ERRNO_HWI_DELETED                0x0200140b��ɾ��δ������Ӳ�жϡ�
 * @retval #SRE_OK                              0x00000000��Ӳ�ж�ɾ���ɹ���
 * @par ����:
 * <ul><li>sre_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_HwiCreate
 */
extern UINT32 SRE_HwiDelete(HWI_HANDLE_T uwHwiNum);

/**
* @ingroup  SRE_hwi
* @brief ��ԭָ���жϺŵ��ж�ʹ�ܼĴ�����ֵ��
*
* @par ����:
* ��֮ǰ����#uniHwiDisable��#uniHwiEnable�ķ���ֵ�����������ж�ʹ�ܼĴ�����
*
* @attention
* <ul>
* <li>���Tensilicaƽ̨��Ч��</li>
* </ul>
*
* @param  uwHwiNum [IN] ����#HWI_HANDLE_T��Ӳ�жϺš�
* @param  uvHwiSave [IN] ����#UINTPTR���ж�ʹ�ܼĴ����ı���ֵ��
*
* @retval ��
* @par ����:
* <ul><li>SRE_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since RTOSck V100R001C01
* @see SRE_HwiEnable | SRE_HwiDisable
*/
extern VOID SRE_HwiRestore(HWI_HANDLE_T  uwHwiNum, UINTPTR  uvHwiSave);

/**
* @ingroup  SRE_hwi
* @brief ����ָ���˺��ڵ�Ӳ�жϡ�
*
* @par ����:
*
*
* @attention
* <ul>
* <li>���Tensilicaƽ̨��Ч��</li>
* </ul>
*
* @param  uwDstCore [IN] ����#UINT32�� Ŀ��˺š� Ŀǰֻ֧��ָ��Ϊ���ˡ�
* @param  uwHwiNum [IN] ����#HWI_HANDLE_T��Ӳ�жϺţ�ֻ֧������ɴ������жϺţ�оƬ�����ԣ���
*
* @retval #OS_ERRNO_HWI_NUM_INVALID                0x02001400���жϺţ��¼��ţ��Ƿ���
* @retval #OS_ERRNO_HWI_CORE_ID_INVALID            0x0200140d��������Ч�ĺ˺š�
* @retval #OS_ERRNO_HWI_NUM_NOT_SOFT_TRIGGERD      0x0200140e��������Ч���жϺţ�������ɴ�����
* @retval #SRE_OK                                  0x00000000��Ӳ�ж�����λ����ɹ���
* @par ����:
* <ul><li>SRE_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since RTOSck V100R001C01
*/
extern UINT32 SRE_HwiTrigger(UINT32 uwMode, UINT32 uwDstCore , HWI_HANDLE_T uwHwiNum);

/**
 * @ingroup  SRE_hwi
 * @brief ���Ӳ�жϵ�Pendingλ��
 *
 * @par ����:
 * ��ʽ���Ӳ�жϻ��¼�������λ����Ϊ�е�Ӳ����Ӧ�жϺ󲻻��Զ���Pendingλ��
 *
 * @attention
 * <ul>
 * <li>����Tensilica���˲���Ϊ�ж�������0-31��</li>
 * </ul>
 *
 * @param  uwHwiNum [IN] ����#HWI_HANDLE_T��Ӳ�жϺš�
 *
 * @retval #OS_ERRNO_HWI_NUM_INVALID            0x02001400���жϺţ��¼��ţ��Ƿ���
 * @retval #SRE_OK                              0x00000000��Ӳ�ж�����λ����ɹ���
 * @par ����:
 * <ul><li>sre_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_HwiCreate
 */
extern UINT32 SRE_HwiClearPendingBit(HWI_HANDLE_T uwHwiNum);


/**
* @ingroup  SRE_hwi
* @brief ����ָ����Ӳ�жϡ�
*
* @par ����:
* ��ֹDSP��Ӧָ��Ӳ�жϵ�����
*
* @attention
* <ul>
* <li>����Tensilica���˲���Ϊ�ж�������0-31��</li>
* <li>Tensilica���й���:�����ΪOS_HWI_ALLʱ������������Ӳ�жϡ�</li>
* </ul>
*
* @param  uwHwiNum [IN] ����#HWI_HANDLE_T�����ݲ�ͬ��оƬ��Ӳ�жϺŻ��ж������ţ���ע�����
*
* @retval #����ǰ���ж�ʹ�ܼĴ�����ֵ��

* @par ����:
* <ul><li>sre_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since RTOSck V100R001C01
* @see SRE_HwiEnable
*/
extern UINTPTR SRE_HwiDisable(HWI_HANDLE_T uwHwiNum);

/**
* @ingroup  SRE_hwi
* @brief ʹ��ָ����Ӳ�жϡ�
*
* @par ����:
* ����DSP��Ӧָ��Ӳ�жϵ�����
*
* @attention
* <ul>
* <li>����Tensilica���˲���Ϊ�ж�������0-31��</li>
* <li>Tensilica���й���:�����ΪOS_HWI_ALLʱ����ʹ������Ӳ�жϡ�</li>
* <li>���ڲ�ͬоƬ���˷���ֵ����������������죬����ϸ�ڼ����淵��ֵ˵��</li>
* </ul>
*
* @param  uwHwiNum [IN] ����#HWI_HANDLE_T�����ݲ�ͬ��оƬ��Ӳ�жϺŻ��ж������ţ���ע�����
*
* @retval #ʹ��ǰ���ж�ʹ�ܼĴ�����ֵ��

* @par ����:
* <ul><li>sre_hwi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since RTOSck V100R001C01
* @see SRE_HwiDisable
*/
extern UINTPTR SRE_HwiEnable(HWI_HANDLE_T uwHwiNum);
extern UINT32 SRE_OsIntLock(UINT32 *puwSpsr);
extern VOID SRE_OsIntRestore(UINT32 *puwSpsr, UINT32 uwR0);
extern UINT32 SRE_OsIntUnLock(UINT32 *puwSpsr);
extern UINT32 SRE_GetCurrCPSR(VOID);
extern UINT32 SRE_GetCurrSPSR(VOID);


extern UINT32 osHwiIpcInit(VOID);
extern UINT32 SRE_HwiMsgRegister(HWI_HANDLE_T uwHwiNum);
extern UINT32 SRE_HwiMsgDeregister(HWI_HANDLE_T uwHwiNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_HWI_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

