/**
 * @file sre_task.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ����������ģ��Ķ���ͷ�ļ��� \n
 */
/**@defgroup SRE_task �����������
  *@ingroup SRE_tsk
*/

/**@defgroup SRE_tsk ����
 *@ingroup SRE_kernel
*/

#ifndef _SRE_TASK_H
#define _SRE_TASK_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/**
 * @ingroup SRE_task
 * ����������󳤶ȡ�
 *
 * ����������󳤶ȣ�������β��'\0'��
 */
#define OS_TSK_NAME_LEN                                     16

/**
 * @ingroup SRE_task
 * �������Ϣ�������ĸ�����
 *
 */
#define OS_TSK_MSGQ_MAX                                     8


/**
 * @ingroup SRE_task
 * ������ID��
 *
 * ����uniTskYieldʱ��ʹ��OS_TSK_NULL_ID����OSѡ����������еĵ�һ������
 */
#define OS_TSK_NULL_ID                                      0xFFFFFFFF


/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 * ����֧��32�����ȼ����ֱ�Ϊ0-31��0��ʾ������ȼ���31��ʾ������ȼ���
 * IDLE����ʹ�ô����ȼ����û�Ӧ�����������ȼ�������
 */
#define OS_TSK_PRIORITY_HIGHEST                             0

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_01                                  1

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_02                                  2

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_03                                  3

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_04                                  4

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_05                                  5

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_06                                  6

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_07                                  7

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_08                                  8

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_09                                  9

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_10                                  10

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_11                                  11

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_12                                  12

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_13                                  13

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_14                                  14

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_15                                  15

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_16                                  16

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_17                                  17

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_18                                  18

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_19                                  19

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_20                                  20

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_21                                  21

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_22                                  22

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_23                                  23

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_24                                  24

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_25                                  25

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_26                                  26

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_27                                  27

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_28                                  28

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_29                                  29

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 */
#define OS_TSK_PRIORITY_30                                  30

/**
 * @ingroup SRE_task
 * ���õ��������ȼ��궨�塣
 *
 * �����������ȼ���
 */
#define OS_TSK_PRIORITY_LOWEST                              31

/**
 * @ingroup SRE_task
 * ��������ȼ�������
 *
 * ����֧��32�����ȼ���
 */
#define OS_TSK_NUM_OF_PRIORITIES                            32


/**
 * @ingroup SRE_task
 * ��С����ջ��С��
 *
 * ��ָ��������ջ��С���ڴ�ֵ�����������Ĵ�С����16�ֽڶ��� + 160�ֽڣ���
 * ���񽫴���ʧ�ܡ�
 */
#define OS_TSK_MIN_STACK_SIZE                               (ALIGN(0x130, 16) + 0xA0)

#define OS_TSK_STACK_GAP_SIZE                               0x1000
#define OS_TSK_STACK_GAP_MAGICNUMBER                        0xDEADBEEF
#define OS_TSK_USER_HEAP_RESERVED_SIZE                      0x1000
#define OS_TSK_PT_RESERVED_SIZE                             0x1000
/*
______
CODE
__r1___ //OS_TSK_STACK_GAP_SIZE reseved region
HEAP
OS_TSK_USER_HEAP_RESERVED_SIZE
__r2___ //OS_TSK_STACK_GAP_SIZE reseved region
OS_TSK_PT_RESERVED_SIZE
STACK
__r3___ //OS_TSK_STACK_GAP_SIZE reseved region
*/
#define OS_TSK_USER_STACK_HEAP_EXTRA (OS_TSK_PT_RESERVED_SIZE + OS_TSK_USER_HEAP_RESERVED_SIZE)
#define OS_TSK_USER_STACK_HEAP_EXTRA_ALIGN 0x1000
/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * ������ƿ�δ��ʹ�á�
 */
#define OS_TSK_UNUSED                                       0x0001

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * ���񱻹���
 */
#define OS_TSK_SUSPEND                                      0x0004

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * �����������ȴ��ź�������
 */
#define OS_TSK_PEND                                         0x0008  /**< �����������ȴ��ź�������     */

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * ����ȴ���ʱ��P�ź�����ʱ����
 */
#define OS_TSK_TIMEOUT                                      0x0010  /**< ����ȴ���ʱ��P�ź�����ʱ����  */

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * ������ʱ��
 */
#define OS_TSK_DELAY                                        0x0020  /**< ������ʱ��                   */

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * �����Ѿ������Ѽ���������С�
 */
#define OS_TSK_READY                                        0x0040  /**< �����Ѿ������Ѽ���������С�   */

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * ���������У����ھ������С�
 */
#define OS_TSK_RUNNING                                      0x0080

/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * �����������ȴ������ź�������
 */
#define OS_TSK_FSEM_PEND                                    0x0100
/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * �����������ȴ���Ϣ����
 */
#define OS_TSK_MSG_PEND                                     0x0200
/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * �����������ȴ��˼��ź�������
 */
#define OS_TSK_MCSEM_PEND                                   0x0400

/**
 * @ingroup SRE_task
 * �������ں�̬
 *
 * �����������ں�̬��
 */
#define OS_TSK_KERNEL_PEND                                   0x0800


#if  (OS_HAVE_COPROCESSOR1 == YES)
/**
 * @ingroup SRE_task
 * �����������ƿ�״̬��־��
 *
 * ����ʹ��coprocessor��ʸ���Ĵ����ֳ��ѱ��档
 */
#define OS_TSK_CP_CONTEXT_SAVED                             0x0002  /**< ����ʹ��coprocessor��ʸ���Ĵ����ֳ��ѱ��档 */
#endif

/*
 * ����ģ��Ĵ����붨�塣
 */
/**
 * @ingroup SRE_task
 * ��������룺�����ڴ�ʧ�ܡ�
 *
 * ֵ: 0x03000800
 *
 * �������: ��������˽��FSC�ڴ����
 *
 */
#define OS_ERRNO_TSK_NO_MEMORY                              SRE_ERRNO_OS_FATAL(OS_MID_TSK, 0x00)

/**
 * @ingroup SRE_task
 * ��������룺ָ�����Ϊ�ա�
 *
 * ֵ: 0x02000801
 *
 * �������: ������ָ���Ƿ�ΪNUL��
 */
#define OS_ERRNO_TSK_PTR_NULL                               SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x01)

/**
 * @ingroup SRE_task
 * ��������룺����ջ��Сδ��16�ֽڴ�С���롣
 *
 * ֵ: 0x02000802
 *
 * �������: ����������ջ��С�Ƿ�16�ֽڶ��롣
 */
#define OS_ERRNO_TSK_STKSZ_NOT_ALIGN                        SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x02)

/**
 * @ingroup SRE_task
 * ��������룺�������ȼ��Ƿ���
 *
 * ֵ: 0x02000803
 *
 * �������: �������������ȼ��Ƿ����31��
 */
#define OS_ERRNO_TSK_PRIOR_ERROR                            SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x03)

/**
 * @ingroup SRE_task
 * ��������룺������ں���Ϊ�ա�
 *
 * ֵ: 0x02000804
 *
 * �������: ������������ں����Ƿ�ΪNULL��
 */
#define OS_ERRNO_TSK_ENTRY_NULL                             SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x04)

/**
 * @ingroup SRE_task
 * ��������룺��������ָ��Ϊ�ջ�������Ϊ���ַ�����
 *
 * ֵ: 0x02000805
 *
 * �������: ���������ָ�����������
 */
#define OS_ERRNO_TSK_NAME_EMPTY                             SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x05)

/**
 * @ingroup SRE_task
 * ��������룺ָ��������ջ�ռ�̫С��
 *
 * ֵ: 0x02000806
 *
 * �������: �������ջ�Ƿ�С��0x1D0��
 */
#define OS_ERRNO_TSK_STKSZ_TOO_SMALL                        SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x06)

/**
 * @ingroup SRE_task
 * ��������룺����ID�Ƿ���
 *
 * ֵ: 0x02000807
 *
 * �������: ����������PID�Ƿ�Ϸ�
 */
#define OS_ERRNO_TSK_ID_INVALID                             SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x07)

/**
 * @ingroup SRE_task
 * ��������룺�����ѱ�����
 *
 * ֵ: 0x02000808
 *
 * �������: ��������������Ƿ�Ϊ�ѹ�������
 */
#define OS_ERRNO_TSK_ALREADY_SUSPENDED                      SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x08)

/**
 * @ingroup SRE_task
 * ��������룺����δ������
 *
 * ֵ: 0x02000809
 *
 * �������: ������ָ������Ƿ�δ����
 */
#define OS_ERRNO_TSK_NOT_SUSPENDED                          SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x09)

/**
 * @ingroup SRE_task
 * ��������룺����δ������
 *
 * ֵ: 0x0200080a
 *
 * �������: ��������Ƿ񴴽�
 */
#define OS_ERRNO_TSK_NOT_CREATED                            SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x0a)

/**
 * @ingroup SRE_task
 * ��������룺���������״̬��ɾ����ǰ����
 *
 * ֵ: 0x0300080b
 *
 *�������: �û�ȷ��ɾ������ʱ�������������
 *
 */
#define OS_ERRNO_TSK_DELETE_LOCKED                          SRE_ERRNO_OS_FATAL(OS_MID_TSK, 0x0b)

/**
 * @ingroup SRE_task
 * ��������룺������������Ϣ�����㡣
 *
 * ֵ: 0x0200080c
 *
 * �������: �����ɾ�������Ƿ�����δ�������Ϣ��
 */
#define OS_ERRNO_TSK_MSG_NONZERO                            SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x0c)

/**
 * @ingroup SRE_task
 * ��������룺��Ӳ�жϻ����жϵĴ����н�����ʱ������
 *
 * ֵ: 0x0300080d
 *
 *�������: �˲�����ֹ���ж��н��е��ȡ�
 *
 */
#define OS_ERRNO_TSK_DELAY_IN_INT                           SRE_ERRNO_OS_FATAL(OS_MID_TSK, 0x0d)

/**
 * @ingroup SRE_task
 * ��������룺���������״̬�½�����ʱ������
 *
 * ֵ: 0x0200080e
 *
 * �������: ����Ƿ�������
 */
#define OS_ERRNO_TSK_DELAY_IN_LOCK                          SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x0e)

/**
 * @ingroup SRE_task
 * ��������룺����ID����Yield����ָ�������ȼ������С�
 *
 * ֵ: 0x0200080f
 *
 * �������: ����������������ȼ���
 */
#define OS_ERRNO_TSK_YIELD_INVALID_TASK                     SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x0f)

/**
 * @ingroup SRE_task
 * ��������룺Yield����ָ�������ȼ������У�����������С��2��
 *
 * ֵ: 0x02000810
 *
 * �������: ���ָ�����ȼ���������ȷ����������������1��
 */
#define OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK                  SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x10)

/**
 * @ingroup SRE_task
 * ��������룺û�п��õ�������ƿ���Դ��������������ü��رա�
 *
 * ֵ: 0x02000811
 *
 * �������: ��������������ü����أ��������㹻���������Դ����
 */
#define OS_ERRNO_TSK_TCB_UNAVAILABLE                        SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x11)

/**
 * @ingroup SRE_task
 * ��������룺�����Ӳ�ƥ�䣬��Ҫɾ���Ĺ���δע�����ȡ����
 *
 * ֵ: 0x02000812
 *
 * �������: ��鹳���Ƿ���ע�ᡣ
 */
#define OS_ERRNO_TSK_HOOK_NOT_MATCH                         SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x12)

/**
 * @ingroup SRE_task
 * ��������룺�������Ѵﵽ��������ע������
 *
 * ֵ: 0x02000813
 *
 * �������: ��������Ӹ�����
 */
#define OS_ERRNO_TSK_HOOK_IS_FULL                           SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x13)

/**
 * @ingroup SRE_task
 * ��������룺����IDLE����
 *
 * ֵ: 0x02000814
 *
 * �������: ����Ƿ����IDLE����
 */
#define OS_ERRNO_TSK_OPERATE_IDLE                           SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x14)

/**
 * @ingroup SRE_task
 * ��������룺���������״̬�¹���ǰ����
 *
 * ֵ: 0x03000815
 *
 *�������: ȷ����������ʱ�������Ѿ�������
 *
 */
#define OS_ERRNO_TSK_SUSPEND_LOCKED                         SRE_ERRNO_OS_FATAL(OS_MID_TSK, 0x15)

/**
 * @ingroup SRE_task
 * ��������룺�ͷ�����ջʧ�ܡ�
 *
 * ֵ: 0x02000817
 *
 * �������: ��
 */
#define OS_ERRNO_TSK_FREE_STACK_FAILED                      SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x17)

/**
 * @ingroup SRE_task
 * ��������룺����ջ��������̫С��
 *
 * ֵ: 0x02000818
 *
 * �������: ��Config.h����������ջ��С����OS_TSK_MIN_STACK_SIZE ��
 */
#define OS_ERRNO_TSK_STKAREA_TOO_SMALL                      SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x18)

/**
 * @ingroup SRE_task
 * ��������룺ϵͳ��ʼ�����񼤻�ʧ��
 *
 * ֵ: 0x03000819
 *
 * �������: �鿴����ջ�Ƿ����ô���
 *
 */
#define OS_ERRNO_TSK_ACTIVE_FAILED                          SRE_ERRNO_OS_FATAL(OS_MID_TSK, 0x19)

/**
 * @ingroup SRE_task
 * ��������룺���õ���������̫�࣬���126��.������Idle����
 *
 * ֵ: 0x0200081a
 *
 * �������: �������������н������������ΪС�ڵ���126����
 */
#define OS_ERRNO_TSK_CONFIG_TOO_MANY                        SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x1a)

/**
 * @ingroup SRE_task
 * ��������룺�����coprocessor�����ı�������δ��16�ֽڶ��롣
 *
 * ֵ: 0x0200081b
 *
 * �������: ��鱣������ʼ��ַ�Ƿ�16�ֽڶ��롣
 */
#define OS_ERRNO_TSK_CP_SAVE_AREA_NOT_ALIGN                 SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x1b)

/**
 * @ingroup SRE_task
 * ��������룺�����MSG���и�������8��
 *
 * ֵ: 0x0200081d
 *
 * �������: ȷ�����񴴽�����Ϣ������������8��
 */
#define OS_ERRNO_TSK_MSG_Q_TOO_MANY                         SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x1d)

/**
 * @ingroup SRE_task
 * ��������룺�����coprocessor�����ı�������ĵ�ַΪ��ָ�롣
 *
 * ֵ: 0x0200081e
 *
 * �������: ��鱣������ʼ��ַ�Ƿ�ΪNULL
 */
#define OS_ERRNO_TSK_CP_SAVE_AREA_NULL                      SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x1e)

/**
 * @ingroup SRE_task
 * ��������룺������ɾ��ʱ�ͷ�δ������Ϣ���ڴ�ʧ��
 *
 * ֵ: 0x0200081f
 *
 * �������: ��
 */
#define OS_ERRNO_TSK_SELF_DELETE_ERR                        SRE_ERRNO_OS_ERROR(OS_MID_TSK, 0x1f)


#define OS_INIT_TASK_NAME "TEEGlobalTask"
/**
 * @ingroup SRE_task
 * ����ID�����Ͷ��塣
 */
typedef UINT32                                              TSK_HANDLE_T;

/**
 * @ingroup SRE_task
 * �������ȼ������Ͷ��塣
 */
typedef UINT16                                              TSK_PRIOR_T;

/**
 * @ingroup SRE_task
 * ����״̬�����Ͷ��塣
 */
typedef UINT16                                              TSK_STATUS_T;

/**
 * @ingroup SRE_task
 * ����ջ��Ϣ�Ľṹ�嶨�塣
 *
 * ��������ջ����Ϣ��
 */
typedef struct tagStackInfo {
	UINT32 uwTop;                                           /**< ջ��                       */
	UINT32 uwBottom;                                        /**< ջ��                       */
	UINT32 uwSP;                                            /**< ջ��ǰSPָ��ֵ             */
	UINT32 uwCurrUsed;                                      /**< ջ��ǰʹ�õĴ�С           */
	UINT32 uwPeakUsed;                                      /**< ջʹ�õ���ʷ��ֵ           */
	BOOL   bOvf;                                            /**< ջ�Ƿ����                 */
} STACK_INFO_S;

/**
* @ingroup  SRE_task
* @brief ������ں������Ͷ��塣
*
* @par ����:
* �û�ͨ��������ں������Ͷ���������ں����������񴴽�����֮����øú�����������ִ�С�
* @attention �ޡ�
*
* @param  uwParam1 [IN] ����#UINT32�����ݸ����������ĵ�һ��������
* @param  uwParam2 [IN] ����#UINT32�����ݸ����������ĵڶ���������
* @param  uwParam3 [IN] ����#UINT32�����ݸ����������ĵ�����������
* @param  uwParam4 [IN] ����#UINT32�����ݸ����������ĵ��ĸ�������
*
* @retval �ޡ�
* @par ����:
* <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since UniDSP V100R001C01
* @see �ޡ�
*/
typedef VOID (* TSK_ENTRY_FUNC)(UINT32 uwParam1,
				UINT32 uwParam2,
				UINT32 uwParam3,
				UINT32 uwParam4);
/**
 * @ingroup  SRE_task
 * @brief ���񴴽����Ӻ������Ͷ��塣
 *
 * @par ����:
 * �û�ͨ�����񴴽����Ӻ������Ͷ������񴴽����ӣ���ϵͳ��������ʱ�����øù��ӡ�
 * @attention �ޡ�
 *
 * @param  puwTaskPID [IN] ����#UINT32���´��������PID��
 *
 * @retval �ޡ�
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see �ޡ�
 */
typedef UINT32(* TSK_CREATE_HOOK)(UINT32 uwTaskPID);

/**
* @ingroup  SRE_task
* @brief ����ɾ�����Ӻ������Ͷ��塣
*
* @par ����:
* �û�ͨ������ɾ�����Ӻ������Ͷ�������ɾ�����ӣ���ϵͳ���������ɾ��ʱ�����øù��ӡ�
* @attention �ޡ�
*
* @param  puwTaskPID [IN] ����#UINT32��ɾ�������PID��
*
* @retval �ޡ�
* @par ����:
* <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
* @since UniDSP V100R001C01
* @see �ޡ�
*/
typedef UINT32(* TSK_DELETE_HOOK)(UINT32 uwTaskPID);

/**
 * @ingroup  SRE_task
 * @brief �����л����Ӻ������Ͷ��塣
 *
 * @par ����:
 * �û�ͨ�������л����Ӻ������Ͷ��������л����ӣ���ϵͳ����������л�ʱ�����øù��ӡ�
 * @attention �ޡ�
 *
 * @param  uwLastTaskPID [IN] ����#UINT32����һ�������PID��
 * @param  uwNextTaskPID [IN] ����#UINT32����һ�������PID��
 *
 * @retval �ޡ�
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see �ޡ�
 */
typedef UINT32(* TSK_SWITCH_HOOK)(UINT32 uwLastTaskPID, UINT32 uwNextTaskPID);

/**
 * @ingroup  SRE_task
 * @brief Ӳ���豸�ͷŻص��������Ͷ��塣
 *
 * @par ����:
 * TSK�����˳���û��ִ��Ӳ���豸�ͷ�����ʱ�����øûص���
 * @attention �ޡ�
 *
 * @param  data [IN] ����#VOID �ص�����������
 *
 * @retval �ޡ�
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see �ޡ�
 */
typedef INT32(* DEV_RELEASE_CALLBACK)(VOID *data);

/**
 * @ingroup SRE_task
 * ���ں˼�¼��TSK���Ͷ��塣
 *
 */
enum TSK_TYPE {
	TASK_TYPE_NONE = 0x0,
	TASK_TYPE_TUI = 0x01,
	TASK_TYPE_NORMAL = 0x02,
};

/**
 * @ingroup SRE_task
 * ���ں˼�¼������TSK PID��SET��GET��CLR�������Ͷ��塣
 *
 */
enum TSK_OPER_TYPE {
	TASK_OPER_NONE = 0x0,
	TASK_OPER_SET = 0x01,
	TASK_OPER_GET = 0x02,
	TASK_OPER_CLR = 0x03,
};

/**
 * @ingroup SRE_task
 * ����ģ��������Ϣ�Ľṹ�嶨�塣
 *
 * ��������ģ�����������Ϣ��
 */
typedef struct tagTskModInfo {
	UINT32 uwMaxNum;                /**< ���֧�ֵ�������           */
	UINT32 uwDefaultSize;           /**< ȱʡ������ջ��С           */
	UINT32 uwIdleStackSize;         /**< Idle���������ջ��С       */
	UINT32 uwStackAreaAddr;         /**< ����ջ�������ʼ��ַ       */
	UINT32 uwStackAreaSize;         /**< ����ջ����Ĵ�С           */
	UINT32 uwPriThreshold;          /**<������ռ���ȼ���ֵ����
                                                    --YES,  ֻ�е�X��������ȼ�����A�������ռ��ֵ�ģ��ſ�����ռA����;
                                                    --NO,  (Default),  ��X��������ȼ�����A��������ȼ����Ϳ�����ռA����*/
} TSK_MOD_INFO_S;

typedef struct {
	UINT32 uwRegionStart;  /*start of TA load mem virt addr*/
	UINT32 uwRegionSize;   /*size of TA running mem*/
	UINT32 uwStackPtSize; /*task user space stack size*/
	UINT32 uwHeapSize;   /*task heap size*/
	UINT32 uwVirtAddr;  /*task_table addr*/
} TSK_MEM_STRUCT_S;

typedef struct {
	UINT32 uwRegionStart;
	UINT32 uwRegionSize;
	UINT32 uwStackPtSize;
	UINT32 uwHeapSize;
	UINT32 uwVirtAddr;
	UINT32 uwHeap;
} TSK_MEM_INFO_S;

typedef void (*task_entry_t)(void);

struct sessison_task_info {
	task_entry_t pentry;
	UINT32 mem_region_start;
	UINT32 mem_region_size;
	UINT32 mem_table;
	UINT32 stack_pt_number;
	char *task_name;
	UINT32 mem_virt_addr;
	UINT32 stack_virt_addr;
	UINT32 heap_virt_addr;
	UINT32 stack_kernel_addr;
};

typedef struct {
	UINT32 timeLow;
	UINT16 timeMid;
	UINT16 timeHiAndVersion;
	UINT8 clockSeqAndNode[8];
} TEE_UUID_TSK;


/**
 * @ingroup SRE_task
 * ���񴴽������Ľṹ�嶨�塣
 *
 * �������񴴽�ʱָ���Ĳ�����Ϣ��
 */
typedef struct tagTskInitParam {
	TSK_ENTRY_FUNC pfnTaskEntry;     /**< ������ں���               */
	TSK_PRIOR_T    usTaskPrio;       /**< �������ȼ�                 */
	UINT16 usQNum;                   /**< ��Ϣ������                 */
	UINT32 auwArgs[4];               /**< ������������4��          */
	UINT32 uwStackSize;              /**< ����ջ�Ĵ�С               */
	CHAR  *pcName;                   /**< ������                     */
	UINT32 uwResved;                 /**< ����                       */
	UINT32 uwPrivateData;            /**< ר���ڱ������˽������     */
	UINT32 uwUsrSpacePartNo;
	UINT64 ullPermissions;           /**< Permission bit field    */
	UINT32 uwSPID;                   /**< Service Provider ID     */
	TSK_MEM_STRUCT_S usMemStruct;
	TEE_UUID_TSK uuid;
} TSK_INIT_PARAM_S;

/**
 * @ingroup SRE_task
 * ������Ϣ�Ľṹ�嶨�塣
 *
 * �û����Բ�ѯ��������Ϣ��
 */
typedef struct tagTskInfo {
	UINT32 uwSP;                     /**< �����л�ʱ��SP             */
	UINT32 uwPC;                     /**< �����л�ʱ��PC             */
	TSK_STATUS_T usTaskStatus;       /**< ����״̬                   */
	TSK_PRIOR_T  usTaskPrio;         /**< �������ȼ�                 */
	UINT32 uwStackSize;              /**< ����ջ�Ĵ�С               */
	UINT32 uwTopOfStack;             /**< ����ջ��ջ��               */
	UINT32 uwNumOfMsg;               /**< ������յ�����Ϣ��         */
	CHAR   acName[OS_TSK_NAME_LEN];  /**< ������                     */
} TSK_INFO_S;

/**
 * @ingroup SRE_task
 * ���������ĵĽṹ�嶨�塣
 *
 * �û����Բ�ѯ��������������Ϣ
 */
typedef struct tagTskContext {
	UINT32 auwA[16];                /**< ��ǰ���ڵ�a0-a15               */
	UINT32 uwPC;                    /**< ���������                     */
	UINT32 uwPS;                    /**< ����״̬�Ĵ���                 */
	UINT32 uwLBEG;                  /**< Loop Begin�Ĵ���               */
	UINT32 uwLEND;                  /**< Loop End�Ĵ���                 */
	UINT32 uwLCOUNT;                /**< Loop Count�Ĵ���               */
	UINT32 uwSAR;                   /**< ��λ���Ĵ���                   */
} TSK_CONTEXT_S;

/**
 * @ingroup SRE_task
 * Structure contains interrupted task context
 *
 * Generic structure to hold taks GPRS and context
 */
typedef struct tagTskRegs {
	UINT32 r0;
	UINT32 r1;
	UINT32 r2;
	UINT32 r3;
	UINT32 r4;
	UINT32 r5;
	UINT32 r6;
	UINT32 r7;
	UINT32 r8;
	UINT32 r9;
	UINT32 r10;
	UINT32 r11;
	UINT32 r12;
	UINT32 usr_sp;
	UINT32 usr_lr;
	UINT32 svc_sp;
	UINT32 svc_lr;
	/* The following 2 will be used in bulk for RFE */
	UINT32 pc;
	UINT32 cpsr;
} TSK_REGS_S;
/**
 * @ingroup  SRE_task
 * @brief ��������
 *
 * @par ����:
 * ����һ��������Root��������ǰ����������ֻ�Ǽ򵥵ؼ���������С�
 * ϵͳ��ʼ���󴴽�������������ȼ����ڵ�ǰ������δ����������������������Ȳ������У�
 * �������������С�
 *
 * @attention
 * <ul>
 * <li>���񴴽�ʱ�����֮ǰ��ɾ�������������ƿ������ջ���л��ա�</li>
 * <li>����������󳤶�Ϊ16�ֽڣ�����������</li>
 * <li>��������ʱ��Ҫ������Ϣ��������</li>
 * <li>ͬһ�����������������ظ����Ҳ���������ж�������</li>
 * <li>��ָ��������ջ��СΪ0����ʹ��������#OS_TSK_DEFAULT_STACK_SIZEָ����Ĭ�ϵ�����ջ��С��</li>
 * <li>����ջ�Ĵ�С���밴16�ֽڴ�С���롣ȷ������ջ��С��ԭ���ǣ����þ��У������˷ѣ���������ջ������������ȡ������Ҫ���Ķ��ٵ�ջ�ڴ棬������������������ò��Խ�ջ�ڴ濪��Խ�󣬾ֲ�����Խ�󣬾ֲ�����Խ�࣬ջ�ڴ濪��ҲԽ��</li>
 * </ul>
 *
 * @param  puwTaskPID [OUT] ����#UINT32 *����������PID��
 * @param  pstInitParam [IN] ����#TSK_INIT_PARAM_S *�����񴴽�������
 *
 * @retval #OS_ERRNO_TSK_NO_MEMORY              0x03000800�������ڴ�ʧ�ܡ�
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_STKSZ_NOT_ALIGN        0x02000802������ջ��Сδ��16�ֽڴ�С���롣
 * @retval #OS_ERRNO_TSK_PRIOR_ERROR            0x02000803���������ȼ��Ƿ���
 * @retval #OS_ERRNO_TSK_ENTRY_NULL             0x02000804��������ں���Ϊ�ա�
 * @retval #OS_ERRNO_TSK_NAME_EMPTY             0x02000805����������ָ��Ϊ�գ��������ǿ��ַ�����
 * @retval #OS_ERRNO_TSK_STKSZ_TOO_SMALL        0x02000806��ָ��������ջ�ռ�̫С��
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_SUSPENDED          0x02000809������δ������
 * @retval #OS_ERRNO_TSK_FREE_STACK_FAILED      0x02000817���ͷ�����ջʧ�ܡ�
 * @retval #OS_ERRNO_TSK_TCB_UNAVAILABLE        0x02000811��û�п��õ�������ƿ���Դ��
 * @retval #OS_ERRNO_TSK_MSG_Q_TOO_MANY         0x0200081d�������MSG���и�������8��
 * @retval #OS_ERRNO_HUNT_THREAD_NAME_REPEAT    0x02001f01����������ʱ���߳�������.
 * @retval #SRE_OK                          0x00000000�����񴴽��ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskDelete | SRE_TaskCreateHookAdd
 */
extern UINT32 SRE_TaskCreate(UINT32 *puwTaskPID, TSK_INIT_PARAM_S *pstInitParam);


/**
 * @ingroup  SRE_task
 * @brief �ָ�����
 *
 * @par ����:
 * �ָ����������
 *
 * @attention
 * <ul>
 * <li>��osStart֮ǰ���ܵ��øýӿڡ�</li>
 * <li>�������Դ�����ʱ������̬����ֻ��ȡ������̬����������������С�</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 *
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED            0x0200080a������δ������
 * @retval #OS_ERRNO_TSK_NOT_SUSPENDED          0x02000809������δ������
 * @retval #SRE_OK                          0x00000000���ָ�����ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskSuspend
 */
extern UINT32 SRE_TaskResume(UINT32 uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief ��������
 *
 * @par ����:
 * ����ָ�����������񽫴Ӿ��������б�ɾ����
 *
 * @attention
 * <ul>
 * <li>��osStart֮ǰ���ܵ��øýӿڡ�</li>
 * <li>��Ϊ��ǰ�����������������ܱ�����</li>
 * <li>IDLE�����ܱ�����</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 *
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_ALREADY_SUSPENDED      0x02000808�������ѱ�����
 * @retval #OS_ERRNO_TSK_NOT_CREATED            0x0200080a������δ������
 * @retval #OS_ERRNO_TSK_OPERATE_IDLE           0x02000814������IDLE����
 * @retval #OS_ERRNO_TSK_SUSPEND_LOCKED         0x03000815�����������״̬�¹���ǰ����
 * @retval #SRE_OK                          0x00000000����������ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskResume | SRE_TaskLock
 */
extern UINT32 SRE_TaskSuspend(UINT32 uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief ɾ������
 *
 * @par ����:
 * ɾ��ָ���������ͷ�����ջ��������ƿ���Դ��
 *
 * @attention
 * <ul>
 * <li>��Ϊ��ɾ������������ƿ������ջ����һ�δ�������ʱ�Ż��ա�</li>
 * <li>����������ɾ����RTOSck�������������ص��ź����ͽ��յ�����Ϣ��</li>
 * <li>������ɾ��ʱ��ɾ�����Ӳ��������pend�ź���������Ȳ�����</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 *
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED            0x0200080a������δ������
 * @retval #OS_ERRNO_TSK_DELETE_LOCKED          0x0300080b�����������״̬��ɾ����ǰ����
 * @retval #OS_ERRNO_TSK_MSG_NONZERO            0x0200080c��������������Ϣ�����㡣
 * @retval #OS_ERRNO_TSK_OPERATE_IDLE           0x02000814������IDLE����
 * @retval #SRE_OK                          0x00000000��ɾ������ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskCreate | SRE_TaskDeleteHookAdd
 */
extern UINT32 SRE_TaskDelete(UINT32 uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief �����ӳ١�
 *
 * @par ����:
 * �ӳٵ�ǰ���������ִ�С�������ʱ�ȴ�ָ����Tick�������²�����ȡ�
 *
 * @attention
 * <ul>
 * <li>��osStart֮ǰ���ܵ��øýӿڡ�</li>
 * <li>Ӳ�жϻ����жϴ����У���������������ʱ����ʧ�ܡ�</li>
 * <li>���������0����δ��������ȣ���˳ȡͬ���ȼ������е���һ������ִ�С�
 * ��û��ͬ���ľ��������򲻷���������ȣ�����ִ��ԭ����</li>
 * </ul>
 *
 * @param  uwTick [IN] ����#UINT32���ӳٵ�Tick����
 *
 * @retval #OS_ERRNO_TSK_PRIOR_ERROR            0x02000803���������ȼ��Ƿ���
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_YIELD_INVALID_TASK     0x0200080f������ID����Yield����ָ�������ȼ������С�
 * @retval #OS_ERRNO_TSK_DELAY_IN_INT           0x0300080d����Ӳ�жϻ����жϵĴ����н�����ʱ������
 * @retval #OS_ERRNO_TSK_DELAY_IN_LOCK          0x0200080e�����������״̬�½�����ʱ������
 * @retval #OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK  0x02000810��Yield����ָ�������ȼ������У�����������С��2��
 * @retval #SRE_OK                          0x00000000��������ʱ�ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskYield
 */
extern UINT32 SRE_TaskDelay(UINT32 uwTick);


/**
 * @ingroup  SRE_task
 * @brief ��������ȡ�
 *
 * @par ����:
 * ��������ȡ���������ȱ������򲻷��������л���
 *
 * @attention
 * <ul>
 * <li>ֻ����������ȣ��������жϣ���������Կɱ��жϴ�ϡ�</li>
 * <li>ִ�д˺�����������ֵ��1��������������ֵ��1����ˣ�������#SRE_TaskUnlock���ʹ�á�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskUnlock
 */
extern VOID SRE_TaskLock(VOID);


/**
 * @ingroup  SRE_task
 * @brief ����������ȡ�
 *
 * @par ����:
 * ����������ֵ��1����Ƕ�׼�������ֻ��������ֵ��Ϊ0�������Ľ�����������ȡ�
 *
 * @attention
 * <ul>
 * <li>��osStart֮ǰ���ܵ��øýӿڡ�</li>
 * <li>ִ�д˺�����������ֵ��1��������������ֵ��1����ˣ�������#SRE_TaskLock���ʹ�á�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskLock
 */
extern VOID SRE_TaskUnlock(VOID);


/**
 * @ingroup  SRE_task
 * @brief ��ȡ��ǰ����PID��
 *
 * @par ����:
 * ��ȡ��������̬������PID��
 *
 * @attention
 * <ul>
 * <li>Ӳ�жϻ����жϴ����У�Ҳ�ɻ�ȡ��ǰ����PID�������жϴ�ϵ�����</li>
 * <li>�������л����Ӵ����е���ʱ����ȡ�������������ID��</li>
 * </ul>
 *
 * @param  puwTaskPID [IN] ����#UINT32����������PID��
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������ID�Ƿ���
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskStatusGet | SRE_TaskInfoGet
 */
extern UINT32 SRE_TaskSelf(UINT32 *puwTaskPID);

extern UINT32 SRE_TaskSelfUUID(TEE_UUID_TSK *puwTaskUUID);


/**
 * @ingroup  SRE_task
 * @brief �������PID��
 *
 * @par ����:
 * �������PID�Ƿ�Ϸ���
 *
 * @attention
 * <ul>
 * <li>����ID�����Ǵ�0��ʼ��ŵġ�</li>
 * <li>�ýӿ�ֻ֧�ֱ�������PID�ļ�⡣</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 *
 * @retval #  �Ϸ�����TRUE���Ƿ�����FALSE��
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskSelf
 */
extern BOOL SRE_TaskIDCheck(UINT32 uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief ��ȡ����״̬��
 *
 * @par ����:
 * ��ȡָ�������״̬��
 *
 * @attention ��
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 *
 * @retval #OS_INVALID    ʧ�ܷ��ء�
 * @retval # �ɹ���������״̬��
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskInfoGet | SRE_TaskContextGet | SRE_TaskPrivateDataGet
 */
extern TSK_STATUS_T SRE_TaskStatusGet(UINT32 uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief ��ȡ������Ϣ��
 *
 * @par ����:
 * ��ȡָ���������Ϣ��
 *
 * @attention
 * <ul>
 * <li>����ȡ��ǰ�������Ϣ����ֻ��Ӳ�жϡ����жϡ��쳣�Ĵ����в������壬�����ȡ��SPֵ��PCֵ�ǲ�׼�ġ�(��TXensaƽ̨�²����������������л�ȡ��������Ϣ)</li>
 * <li>���������л�ʱ����������ϢҲ����������ջ�У����������Ϣ�е�SP�Ǳ���������֮���ʵ�ʵ�SPֵ��</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 * @param  pstTaskInfo [IN] ����#TSK_INFO_S *������������Ϣ��
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED            0x0200080a������δ������
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskStatusGet | SRE_TaskContextGet | SRE_TaskPrivateDataGet
 */
extern UINT32 SRE_TaskInfoGet(UINT32 uwTaskPID, TSK_INFO_S *pstTaskInfo);
extern UINT32 SRE_TaskInfoGet_EX(UINT32 uwTaskPID, TSK_INFO_S *pstTaskInfo);


/**
 * @ingroup  SRE_task
 * @brief ��ȡ���������ġ�
 *
 * @par ����:
 * ��ȡָ������������ġ�
 *
 * @attention
 * <ul>
 * <li>����ȡ��ǰ�������������Ϣ����ֻ��Ӳ�жϡ����жϡ��쳣�Ĵ����в������壬�����ȡ��PCֵ�ǲ�׼�ġ�</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 * @param  pstContext [IN] ����#TSK_CONTEXT_S *������������������Ϣ��
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED            0x0200080a������δ������
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskStatusGet | SRE_TaskInfoGet | SRE_TaskPrivateDataGet
 */
extern UINT32 SRE_TaskContextGet(UINT32 uwTaskPID, TSK_CONTEXT_S *pstContext);


/**
 * @ingroup  SRE_task
 * @brief ��ȡ˽�����ݡ�
 *
 * @par ����:
 * ��ȡ��ǰ�����˽�����ݡ�
 *
 * @attention
 * <ul>
 * <li>ֻ�ܻ�ȡ��ǰ�����˽�����ݡ�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval # ���ظ������Ӧ��˽�����ݡ���δ���ã��򷵻ص�ֵ��ȷ����
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskPrivateDataSet
 */
extern UINT32 SRE_TaskPrivateDataGet();


/**
 * @ingroup  SRE_task
 * @brief ����˽������ֵ��
 *
 * @par ����:
 * ���õ�ǰ�����˽������ֵ��
 *
 * @attention
 * <ul>
 * <li>ֻ�����������е��á������ж������ã���������Ǹձ���ϵ�����</li>
 * </ul>
 *
 * @param  uwPrivateData [IN] ����#UINT32������ֵ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskPrivateDataGet
 */
extern VOID SRE_TaskPrivateDataSet(UINT32 uwPrivateData);


/**
 * @ingroup  SRE_task
 * @brief ��ȡ���ȼ���
 *
 * @par ����:
 * ��ȡָ����������ȼ���
 *
 * @attention ��
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 *
 * @retval #OS_INVALID ʧ�ܷ��ء�
 * @retval # �ɹ������������ȼ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskPrioritySet
 */
extern TSK_PRIOR_T SRE_TaskPriorityGet(UINT32 uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief �������ȼ���
 *
 * @par ����:
 * ����ָ����������ȼ���
 *
 * @attention
 * <ul>
 * <li>��osStart֮ǰ���ܵ��øýӿڡ�</li>
 * <li>�����õ����ȼ����ڵ�ǰ���е��������������������ȡ�</li>
 * <li>��������ǰ������������ȼ���ͬ����������������ȡ�</li>
 * </ul>
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 * @param  usTaskPrio [IN] ����#TSK_PRIOR_T���������ȼ���
 *
 * @retval #OS_ERRNO_TSK_PRIOR_ERROR            0x02000803���������ȼ��Ƿ���
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED            0x0200080a������δ������
 * @retval #OS_ERRNO_TSK_OPERATE_IDLE           0x02000814������IDLE����
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskPriorityGet
 */
extern UINT32 SRE_TaskPrioritySet(UINT32 uwTaskPID, TSK_PRIOR_T usTaskPrio);


/**
 * @ingroup  SRE_task
 * @brief ����ָ�����ȼ����������˳��
 *
 * @par ����:
 * ��uwNextTaskΪ#OS_TSK_NULL_ID�������ȼ������еĵ�һ�����������������β��
 * ���򣬽�uwNextTaskָ����������������ȼ����еĶ��ס�
 *
 * @attention
 * <ul>
 * <li>��osStart֮ǰ���ܵ��øýӿڡ�</li>
 * <li>ǰ����ָ�����ȼ��ľ�����������Ҫ�������������񣬷��򱨴��ء�</li>
 * </ul>
 *
 * @param  usTaskPrio [IN] ����#TSK_PRIOR_T������usTaskPrio��ָ���������ȼ����С�
 * @param  uwNextTask [IN] ����#UINT32������ID��OS_TSK_NULL_ID��
 * @param  puwYeildTo [IN] ����#UINT32 *�����汻���������׵�����PID����ΪNULL��
 *
 * @retval #OS_ERRNO_TSK_PRIOR_ERROR            0x02000803���������ȼ��Ƿ���
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_YIELD_INVALID_TASK     0x0200080f������PID����Yield����ָ�������ȼ������С�
 * @retval #OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK  0x02000810��Yield����ָ�������ȼ������У�����������С��2��
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskSuspend
 */
extern UINT32 SRE_TaskYield(TSK_PRIOR_T  usTaskPrio,
			    UINT32 uwNextTask,
			    UINT32 *puwYeildTo);

/**
 * @ingroup  SRE_task
 * @brief ע�����񴴽����ӡ�
 *
 * @par ����:
 * ע�����񴴽����Ӻ��������Ӻ��������񴴽��ɹ��󱻵��á�
 *
 * @attention
 * <ul>
 * <li>��ǰ���ֻ����ע��һ�����񴴽����Ӻ�����</li>
 * <li>��Ӧ�����񴴽������ﴴ������</li>
 * </ul>
 *
 * @param  pfnHook [IN] ����#TSK_CREATE_HOOK�����񴴽����Ӻ�����
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_HOOK_IS_FULL           0x02000813���������Ѵﵽ��������ע������
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskCreateHookDelete | SRE_TaskDeleteHookAdd | SRE_TaskSwitchHookAdd
 */
extern UINT32 SRE_TaskCreateHookAdd(TSK_CREATE_HOOK pfnHook);


/**
 * @ingroup  SRE_task
 * @brief ȡ�����񴴽����ӡ�
 *
 * @par ����:
 * ȡ��ָ�������񴴽����Ӻ��������Ӻ��������񴴽��ɹ��󱻵��á�
 *
 * @attention
 * <ul>
 * <li>��ǰ���ֻ����ע��һ�����񴴽����Ӻ�����</li>
 * </ul>
 *
 * @param  pfnHook [IN] ����#TSK_CREATE_HOOK�����񴴽����Ӻ�����
 *
 * @retval #OS_ERRNO_TSK_HOOK_NOT_MATCH         0x02000812�������Ӳ�ƥ�䣬��Ҫɾ���Ĺ���δע�����ȡ����
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskCreateHookAdd
 */
extern UINT32 SRE_TaskCreateHookDelete(TSK_CREATE_HOOK pfnHook);


/**
 * @ingroup  SRE_task
 * @brief ע������ɾ�����ӡ�
 *
 * @par ����:
 * ע������ɾ�����Ӻ��������Ӻ�������Դ����ǰ�����á�
 *
 * @attention
 * <ul>
 * <li>��ǰ���ֻ����ע��һ������ɾ�����Ӻ�����</li>
 * <li>����ɾ�������в��������pend�ź���������</li>
 * </ul>
 *
 * @param  pfnHook [IN] ����#TSK_DELETE_HOOK������ɾ�����Ӻ�����
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_HOOK_IS_FULL           0x02000813���������Ѵﵽ��������ע������
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskDeleteHookDelete | SRE_TaskCreateHookAdd | SRE_TaskSwitchHookAdd
 */
extern UINT32 SRE_TaskDeleteHookAdd(TSK_DELETE_HOOK pfnHook);


/**
 * @ingroup  SRE_task
 * @brief ȡ������ɾ�����ӡ�
 *
 * @par ����:
 * ȡ��ָ��������ɾ�����ӡ����Ӻ�������Դ����ǰ�����á�
 *
 * @attention
 * <ul>
 * <li>��ǰ���ֻ����ע��һ������ɾ�����Ӻ�����</li>
 * <li>��Ӧ������ɾ��������ɾ������</li>
 * </ul>
 *
 * @param  pfnHook [IN] ����#TSK_DELETE_HOOK������ɾ�����Ӻ�����
 *
 * @retval #OS_ERRNO_TSK_HOOK_NOT_MATCH         0x02000812�������Ӳ�ƥ�䣬��Ҫɾ���Ĺ���δע�����ȡ����
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskDeleteHookAdd
 */
extern UINT32 SRE_TaskDeleteHookDelete(TSK_DELETE_HOOK pfnHook);


/**
 * @ingroup  SRE_task
 * @brief ע�������л����ӡ�
 *
 * @par ����:
 * ע�������л����Ӻ��������Ӻ���������������ǰ�����á�
 *
 * @attention
 * <ul>
 * <li>�ܹ�����ע��8�������л����Ӻ���������ϵͳ����ע��4�������л����Ӻ�������ˣ������û���Ҫע�ᳬ��4�������л����Ӻ�����</li>
 * <li>��ͬ���Ӻ�����ִ�е��Ⱥ�˳�򣬲�Ӧ������������ϵ��</li>
 * <li>��Ӧ�������л���������п�������������ȵĴ����磺������ʱ��P�ź����ȡ�</li>
 * </ul>
 *
 * @param  pfnHook [IN] ����#TSK_SWITCH_HOOK�������л����Ӻ�����
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL               0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_HOOK_IS_FULL           0x02000813���������Ѵﵽ��������ע������
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskSwitchHookDelete | SRE_TaskDeleteHookAdd | SRE_TaskCreateHookAdd
 */
extern UINT32 SRE_TaskSwitchHookAdd(TSK_SWITCH_HOOK pfnHook);


/**
 * @ingroup  SRE_task
 * @brief ȡ�������л����ӡ�
 *
 * @par ����:
 * ȡ��ָ���������л����ӡ����Ӻ���������������ǰ�����á�
 *
 * @attention  ��
 *
 * @param  pfnHook [IN] ����#TSK_SWITCH_HOOK�������л����Ӻ�����
 *
 * @retval #OS_ERRNO_TSK_HOOK_NOT_MATCH         0x02000812�������Ӳ�ƥ�䣬��Ҫɾ���Ĺ���δע�����ȡ����
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskSwitchHookAdd
 */
extern UINT32 SRE_TaskSwitchHookDelete(TSK_SWITCH_HOOK pfnHook);

/**
 * @ingroup  SRE_task
 * @brief ��ѯ����ָ����������PEND���ź�����
 *
 * @par ����:
 * ��������״̬��������ƿ飬�ж������Ƿ���PEND�ź������Լ�PEND���ź���ID��
 *
 * @attention
 * <ul>
 * <li>�û�Ӧ���ж�PEND״̬��״̬Ϊ0��������û�б��ź���������</li>
 * <li>�����ź���û���ź���ID�������������ڿ����ź������򷵻ص��ź���IDΪ#OS_INVALID��</li>
 * </ul>
 *
 * @param  uwTaskID     [IN]  ����#UINT32������PID��
 * @param  pusSemID     [OUT] ����#UINT16 *������PEND���ź���ID����#OS_INVALID��
 * @param  pusPendState [OUT] ����#UINT16 *�������PEND״̬��0��#OS_TSK_FSEM_PEND��#OS_TSK_PEND��#OS_TSK_MCSEM_PEND��
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL       0x02000801��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_TSK_ID_INVALID     0x02000807������PID�Ƿ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED    0x0200080a������δ������
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SemPendListGet | SRE_McSemPendListGet
 */
extern UINT32 SRE_TaskPendSemGet(TSK_HANDLE_T uwTaskID, UINT16 *pusSemID, UINT16 *pusPendState);

/**
 * @ingroup  SRE_task
 * @brief ��ѯ��ǰ��������ָ�����е���Ϣ�����ո�����
 *
 * @par ����:
 *��ѯ��ǰ������Ϣ����uwQid�ϴ�������Ϣ������
 *
 * @attention
 * <ul>
 * <li>6181/6108/6182ƽ̨�ϱ�ʾ�����ϵ�ָ������Ϣ���е���Ϣ��������ƽ̨��֧�ֶ���Ϣ���У��ýӿڱ�ʾ��Ϣ�����ϵ���Ϣ�������uwQid��Ч��</li>
 * </ul>
 *
 * @param  uwQid [IN] ����#UINT32��6181/6108/6182ƽ̨�ϱ�ʾ�����ϵ���Ϣ���У�����ƽ̨��Ч��Ρ�
 *
 * @retval  ��ǰ��������ָ�����е���Ϣ�����ո�����
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see ��
 */
extern UINT32 SRE_TaskMsgNumGet(UINT32 uwQid);

/**
 * @ingroup  SRE_task
 * @brief ��ȡָ������Ķ�ջ��Ϣ��
 *
 * @par ����:
 * ��ȡָ������Ķ�ջ��Ϣ��
 *
 * @attention ��
 *
 * @param  uwTaskPID [IN] ����#UINT32������PID��
 * @param  pstStackInfo [OUT] ����#STACK_INFO_S������ջ��Ϣ��
 *
 * @retval #OS_ERRNO_TSK_PTR_NULL      0x02000801�����ָ��Ϊ�ա�
 * @retval #OS_ERRNO_TSK_ID_INVALID    0x02000807���������ID���Ϸ���
 * @retval #OS_ERRNO_TSK_NOT_CREATED   0x0200080a������δ������
 * @retval #SRE_OK                     0X00000000���ɹ���
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C00
 */
extern UINT32 SRE_TaskStackInfoGet(TSK_HANDLE_T uwTaskPID, STACK_INFO_S *pstStackInfo);

/**
 * @ingroup  SRE_task
 * @brief
 *
 * @par ����:
 *
 *
 * @attention ��
 *
 * @param
 *
 * @retval #SRE_OK                     0X00000000���ɹ���
 * @par ����:
 * @since RTOSck V100R001C00
 */
extern UINT32 SRE_TaskCurrentMemInfo(TSK_MEM_INFO_S *usTaskMemInfo);

/**
 * @ingroup  SRE_task
 * @brief Get task vmem information
 *
 * @par ����:
 *
 *
 * @attention ��
 *
 * @param  uwTaskPID [IN] Task PID
 *
 * @retval
 * @par ����:
 * @since RTOSck V100R001C00
 */
extern VOID *SRE_TaskVmemGet(TSK_HANDLE_T uwTaskPID);


/**
 * @ingroup  SRE_task
 * @brief Get task mem table pointer
 *
 * @par ����:
 *
 *
 * @attention ��
 *
 * @param  uwTaskPID [IN] Task PID
 *
 * @retval
 * @par ����:
 * @since RTOSck V100R001C00
 */
extern VOID *SRE_TaskMemTableGet(TSK_HANDLE_T uwTaskPID);

/**
 * @ingroup  SRE_task
 * @brief ע��Ӳ���豸�ͷŻص�����
 *
 * @par ����:
 * ����ǰ����ע��Ӳ���豸�ͷŻص�����
 *
 * @attention ��
 *
 * @param  devRelCb [IN] �ص�����ָ��
 * @param  data [IN] �ص���������
 *
 * @retval #OS_ERRNO_TSK_NO_MEMORY          0x03000800�������ڴ�ʧ�ܡ�
 * @retval #SRE_OK                          0x00000000���ɹ���
 */
extern UINT32 SRE_TaskRegister_DevRelCb(DEV_RELEASE_CALLBACK devRelCb, void *data);

/**
 * @ingroup  SRE_task
 * @brief ɾ��Ӳ���豸�ͷŻص�����
 *
 * @par ����:
 * ����ǰ����ɾ��Ӳ���豸�ͷŻص�����
 *
 * @attention ��
 *
 * @param  devRelCb [IN] �ص�����ָ��
 *
 * @retval
 */
extern VOID SRE_TaskUnRegister_DevRelCb(DEV_RELEASE_CALLBACK devRelCb);

#if  (OS_HAVE_COPROCESSOR1 == YES)

/**
 * @ingroup  SRE_task
 * @brief ���������ʸ���������������ݱ�����
 *
 * @par ����:
 * ��ָ���������ʸ���������������ݱ�����
 *
 * @attention
 * <ul>
 * <li>�ò���������SD6108/SD6181ƽ̨��DSP�ˣ�SD6182ƽ̨��DSP�ˣ�CPU�ˣ�HACCC�ˡ�</li>
 * <li>����ʸ�������������õ��˵�˫LoadStoreָ��������õ�SaveAreaA/SaveAreaB�ڲ�ͬ��Memory Bank�С�</li>
 * <li>�����ʸ�����������ݱ�������ʼ��ַ���밴16�ֽڶ��롣</li>
 * <li>ʸ�����������ݱ�������С�����û�����оƬʵ�ʴ��ڵ�ʸ���Ĵ����ܴ�Сȷ����</li>
 * </ul>
 * @param  uwTaskID [IN] ����Ϊ#TSK_HANDLE_T������ID��
 * @param  pstParam [IN] ����#CP_SAVE_CFG_PARAM_S *�������ṹָ��
 *
 * @retval #OS_ERRNO_TSK_CP_SAVE_AREA_NULL      0x0200081e�������coprocessor�����ı�������ĵ�ַΪ��ָ�롣
 * @retval #OS_ERRNO_TSK_ID_INVALID             0x02000807����������ID�Ƿ���
 * @retval #OS_ERRNO_TSK_CP_SAVE_AREA_NOT_ALIGN 0x0200801b����������δ��16�ֽڶ��롣
 * @retval #SRE_OK                          0x00000000���ɹ���
 *
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskCpEntry | SRE_TaskCpExit
 */
extern UINT32 SRE_TaskCpSaveCfg(TSK_HANDLE_T uwTaskID, CP_SAVE_CFG_PARAM_S *pstParam);

/**
 * @ingroup  SRE_task
 * @brief ������ִ��ʸ�����������
 *
 * @par ����:
 * ������ִ��ʸ�����������, ʸ��������ʼ֮ǰ����ô˽ӿڡ�
 *
 * @attention
 * <ul>
 * <li>�ò���������SD6108/SD6181ƽ̨��DSP�ˣ�SD6182ƽ̨��DSP�ˣ�CPU�ˣ�HACCC�ˡ�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ��
 *
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskCpExit
 */
extern VOID SRE_TaskCpEntry(VOID);

/**
 * @ingroup  SRE_task
 * @brief ������ִ��ʸ�������ĳ���
 *
 * @par ����:
 * ������ִ��ʸ�������ĳ��ڣ�ʸ���������֮������ô˽ӿڡ�
 * @attention
 * <ul>
 * <li>�ò���������SD6108/SD6181ƽ̨��DSP�ˣ�SD6182ƽ̨��DSP�ˣ�CPU�ˣ�HACCC�ˡ�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ��
 *
 * @par ����:
 * <ul><li>sre_task.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_TaskCpEntry
 */
extern VOID SRE_TaskCpExit(VOID);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_BASE_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

