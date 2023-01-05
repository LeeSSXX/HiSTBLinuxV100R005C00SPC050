/**
 * @file sre_hwwdg.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������Ӳ�����Ź�ģ��Ķ���ͷ�ļ��� \n
 */
/** @defgroup SRE_hwwdg ���Ź�
   *@ingroup SRE_kernel
*/

#ifndef _SRE_HWWDG_H
#define _SRE_HWWDG_H

#include "sre_base.h"
#include "sre_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź���ʱ�Ĵ���ģʽ��
 */
typedef enum tagHwWdgProcMode
{
	OS_HWWDG_GENERATE_RESET = 0,        /**< Ӳ�����Ź���ʱʱ����λ����       */
	OS_HWWDG_GENERATE_NMI   = 1,        /**< Ӳ�����Ź���ʱʱ������NMI�ж�    */
	OS_HWWDG_GENERATE_INT   = 2,        /**< Ӳ�����Ź���ʱʱ�������������ж� */
	OS_HWWDG_GENERATE_BUTT
}
HWWDG_PROC_MODE_E;


/*
 * Ӳ�����Ź�ģ��Ĵ����붨�塣
 */
/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź������룺Ӳ�����Ź��ĳ�ʱʱ����Ϊ�㡣
 *
 * ֵ: 0x02001501
 *
 * �������: ��
 */
#define OS_ERRNO_HWWDG_TIMEOUT_ZERO                         SRE_ERRNO_OS_ERROR(OS_MID_HWWDG, 0x01)

/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź������룺Ӳ�����Ź��Ĵ���ģʽ�Ƿ���
 *
 * ֵ: 0x02001502
 *
 * �������: ��
 */
#define OS_ERRNO_HWWDG_INVALID_MODE                         SRE_ERRNO_OS_ERROR(OS_MID_HWWDG, 0x02)

/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź������룺û�п��õ�Ӳ����ʱ����Դ��
 *
 * ֵ: 0x02001503
 *
 * �������: ��
 */
#define OS_ERRNO_HWWDG_HWTMR_UNAVAILABLE                    SRE_ERRNO_OS_ERROR(OS_MID_HWWDG, 0x03)

/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź������룺��ʱ�������
 *
 * ֵ: 0x02001504
 *
 * �������: ��
 */
#define OS_ERRNO_HWWDG_HWTMR_UNREACHABLE                    SRE_ERRNO_OS_ERROR(OS_MID_HWWDG, 0x04)

/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź������룺����Ϊ�����жϣ�����������Ϊ��
 *
 * ֵ: 0x02001505
 *
 * �������: ��
 */
#define OS_ERRNO_HWWDG_INTHOOK_NULL                         SRE_ERRNO_OS_ERROR(OS_MID_HWWDG, 0x05)

/**
* @ingroup  SRE_hwwdg
* @brief Ӳ�����Ź����ܺ��������Ͷ��塣
*
* @par ����:
* ͨ����Ӳ�����Ź����ܺ������Ͷ��庯�������ڿ��Ź������жϵĴ�������
*
* @attention ��
*
* @param  ��
*
* @retval ��
* @par ����:
* <ul><li>sre_hwwdg.h:�ýӿ��������ڵ�ͷ�ļ� </li></ul>
* @since UniDSP V100R001C01
* @see
*/
typedef VOID (* HWWDG_PROC_FUNC)(VOID);

/**
 * @ingroup SRE_hwwdg
 * Ӳ�����Ź�ģ��������Ϣ�Ľṹ�嶨�塣
 *
 * ����Ӳ�����Ź�ģ�����������Ϣ��
 */
typedef struct tagHwWdgModInfo {
	UINT32 uwInterval;                                      /**< Ӳ�����Ź���ʱʱ�䣬��λms */
	UINT16 usProcMode;                                      /**< Ӳ�����Ź�����ģʽ         */
	UINT16 usResved;                                        /**< ����                       */
	HWWDG_PROC_FUNC pfnHwwdbHandle;                         /**< Ӳ�����Ź���ʱ������������ǿ������жϣ�����ô˹��Ӵ���*/
	HWI_HANDLE_T uwHwiNum;                                  /**< Ӳ�����Ź��󶨵��жϺ�     */
} HWWDG_MOD_INFO;


/**
 * @ingroup  SRE_hwwdg
 * @brief ������Ź���
 *
 * @par ����:
 * Ӳ�����Ź���ι��������Ӳ�����Ź��������´��㿪ʼ������
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_hwwdg.h:�ýӿ��������ڵ�ͷ�ļ� </li></ul>
 * @since UniDSP V100R001C01
 * @see OS_HWWDG_ENABLE | OS_HWWDG_DISABLE
 */
#define OS_HWWDG_SERVICE() \
    do \
    { \
        if (g_pfnHwWdgServiceHook != NULL) \
        { \
            g_pfnHwWdgServiceHook(); \
        } \
    } while (0)


/**
 * @ingroup  SRE_hwwdg
 * @brief ʹ�ܿ��Ź���
 *
 * @par ����:
 * ʹ�ܿ��Ź��󣬿��Ź����¿�ʼ������
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_hwwdg.h:�ýӿ��������ڵ�ͷ�ļ�</li></ul>
 * @since UniDSP V100R001C01
 * @see OS_HWWDG_SERVICE | OS_HWWDG_DISABLE
 */
#define OS_HWWDG_ENABLE() \
    do \
    { \
        if (g_pfnHwWdgEnableHook != NULL) \
        { \
            g_pfnHwWdgEnableHook(); \
        } \
    } while (0)


/**
 * @ingroup  SRE_hwwdg
 * @brief ��ֹ���Ź���
 *
 * @par ����:
 * ��ֹ���Ź��󣬿��Ź���ʱ��ֹͣ������
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_hwwdg.h:�ýӿ��������ڵ�ͷ�ļ�</li></ul>
 * @since UniDSP V100R001C01
 * @see OS_HWWDG_SERVICE | OS_HWWDG_ENABLE
 */
#define OS_HWWDG_DISABLE() \
    do \
    { \
        if (g_pfnHwWdgDisableHook != NULL) \
        { \
            g_pfnHwWdgDisableHook(); \
        } \
    } while (0)


/* ���򿪿��Ź����أ���ҽ�RTOSck�ĵĴ���������δ�򿪿��Ź����أ���ҽ��õĵĴ�����*/
EXTERN_FAR HWWDG_PROC_FUNC g_pfnHwWdgServiceHook;       /* ������Ź����Ӻ��� */
EXTERN_FAR HWWDG_PROC_FUNC g_pfnHwWdgEnableHook;        /* ʹ�ܿ��Ź����Ӻ��� */
EXTERN_FAR HWWDG_PROC_FUNC g_pfnHwWdgDisableHook;       /* ��ֹ���Ź����Ӻ��� */



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_HWWDG_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

