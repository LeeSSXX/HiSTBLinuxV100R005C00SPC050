/**
 * @file sre_errno.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������ͨ�ô����붨��ͷ�ļ��� \n
 */

#ifndef _SRE_ERRNO_H
#define _SRE_ERRNO_H

#include "sre_typedef.h"
#include "sre_err.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/* 0x00��ʾOS,0xFF��ʾDRV,Ӧ��ϵͳ����ʹ�� 1~254 ����ʾӦ�ø�����ϵͳ */
/**
 * @ingroup SRE_err
 * OS��������λ��(0x00��ʾOS,0xFF��ʾDRV,Ӧ��ϵͳ����ʹ�� 1~254 ����ʾӦ�ø�����ϵͳ)
 *
 */
#define SRE_ERRNO_OS_ID                                     ((UINT32)0x00 << 16)

/**
 * @ingroup SRE_err
 * �������ĵȼ�:��ʾ����
 *
 */
#define SRE_ERRTYPE_NORMAL                                  ((UINT32)0x00 << 24)

/**
 * @ingroup SRE_err
 * �������ĵȼ�:�澯����
 *
 */
#define SRE_ERRTYPE_WARN                                    ((UINT32)0x01 << 24)

/**
 * @ingroup SRE_err
 * �������ĵȼ�:���ؼ���
 *
 */
#define SRE_ERRTYPE_ERROR                                   ((UINT32)0x02 << 24)

/**
 * @ingroup SRE_err
 * �������ĵȼ�:��������
 *
 */
#define SRE_ERRTYPE_FATAL                                   ((UINT32)0x03 << 24)

/**
 * @ingroup  SRE_err
 * @brief ����OS��������
 *
 * @par ����:
 * �궨�壬����OS��������
 *
 * @attention ��

 * @param  MID [IN] ģ��ID��š�
 * @param  ERRNO [IN] �������š�
 *
 * @retval ��
 * @par ����:
 * sre_errno.h: �ú궨�����ڵ�ͷ�ļ���
 * @since UniDSP V100R001C01
 * @see SRE_ERRNO_OS_ERROR | SRE_ERRNO_OS_WARN | SRE_ERRNO_OS_NORMAL
 */
#define SRE_ERRNO_OS_FATAL(MID,ERRNO)   \
            (SRE_ERRTYPE_FATAL | SRE_ERRNO_OS_ID | ((UINT32)(MID) << 8) |  (ERRNO))


/**
 * @ingroup  SRE_err
 * @brief ����OS���ش���
 *
 * @par ����:
 * �궨�壬����OS���ش���
 *
 * @attention ��
 * @param  MID [IN] ģ��ID��š�
 * @param  ERRNO [IN] �������š�
 *
 * @retval ��
 * @par ����:
 * sre_errno.h: �ú궨�����ڵ�ͷ�ļ���
 * @since UniDSP V100R001C01
 * @see SRE_ERRNO_OS_FATAL | SRE_ERRNO_OS_WARN | SRE_ERRNO_OS_NORMAL
 */
#define SRE_ERRNO_OS_ERROR(MID,ERRNO)  \
            (SRE_ERRTYPE_ERROR | SRE_ERRNO_OS_ID | ((UINT32)(MID) << 8) | (ERRNO))


/**
 * @ingroup  SRE_err
 * @brief ����OS�澯��Ϣ
 *
 * @par ����:
 * �궨�壬����OS�澯
 *
 * @attention ��
 * @param  MID [IN] ģ��ID��š�
 * @param  ERRNO [IN] �������š�
 *
 * @retval ��
 * @par ����:
 * sre_errno.h: �ú궨�����ڵ�ͷ�ļ���
 * @since UniDSP V100R001C01
 * @see SRE_ERRNO_OS_FATAL | SRE_ERRNO_OS_ERROR | SRE_ERRNO_OS_NORMAL
 */
#define SRE_ERRNO_OS_WARN(MID,ERRNO)  \
            (SRE_ERRTYPE_WARN | SRE_ERRNO_OS_ID | ((UINT32)(MID) << 8) | (ERRNO))


/**
 * @ingroup  SRE_err
 * @brief ����OS��ʾ��Ϣ
 *
 * @par ����:
 * �궨�壬����OS��ʾ��Ϣ
 *
 * @attention ��
 * @param  MID [IN] ģ��ID��š�
 * @param  ERRNO [IN] �������š�
 *
 * @retval ��
 * @par ����:
 * sre_errno.h: �ú궨�����ڵ�ͷ�ļ���
 * @since UniDSP V100R001C01
 * @see SRE_ERRNO_OS_FATAL | SRE_ERRNO_OS_ERROR | SRE_ERRNO_OS_WARN
 */
#define SRE_ERRNO_OS_NORMAL(MID,ERRNO)  \
            (SRE_ERRTYPE_NORMAL | SRE_ERRNO_OS_ID | ((UINT32)(MID) << 8) | (ERRNO))

#define REPORT_ERROR(uwErrNo)  \
    do \
    { \
        (VOID)SRE_ErrHandle("__FILE__", __LINE__, uwErrNo, 0, NULL); \
    } while (0)

#define REPORT_ERROR_P2(uwErrLine, uwErrNo)  \
    do \
    { \
        (VOID)SRE_ErrHandle("__FILE__", uwErrLine, uwErrNo, 0, NULL); \
    } while (0)

#define RETURN_ERROR(uwErrNo) \
    do \
    {\
       (VOID)SRE_ErrHandle("__FILE__", __LINE__, uwErrNo, 0, NULL); \
        \
        return uwErrNo; \
    } while (0)

#define RETURN_ERROR_P2(uwErrLine, uwErrNo) \
    do \
    {\
        (VOID)SRE_ErrHandle("__FILE__", uwErrLine, uwErrNo, 0, NULL); \
        \
        return uwErrNo; \
    }while(0)

#define GOTO_ERR_HANDLER(uwErrorNo) \
    do \
    { \
        uwErrNo    = uwErrorNo; \
        uwErrLine  = __LINE__; \
        goto ErrHandler; \
    }while(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_ERRNO_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */


