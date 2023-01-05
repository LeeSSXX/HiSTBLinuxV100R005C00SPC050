/**
 * @file sre_swi.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ���������ж�ģ��Ķ���ͷ�ļ��� \n
 */
/**@defgroup SRE_interrupt �ж�
   *@ingroup SRE_kernel
   */
/** @defgroup SRE_swi ���ж�
  *@ingroup SRE_interrupt
 */

#ifndef _SRE_SWI_H
#define _SRE_SWI_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#if (OS_INCLUDE_SWI == YES)
/**
 * @ingroup SRE_swi
 * ���õ����ж����ȼ��ĺ궨�塣
 *
 * ���жϹ�֧��16�����ȼ����ֱ�Ϊ0-15��0��ʾ������ȼ���15��ʾ������ȼ���
 * Tick�ж�ʹ�����ȼ�0���û�Ӧ�����������ȼ������жϡ�
 * ������ģ��ر�ʱ��IDLE���ж��Զ�������ʹ�����ȼ�15���û�Ӧ�����������ȼ������жϡ�
 */
#define OS_SWI_PRIORITY_00                                  0

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ1��
  */
#define OS_SWI_PRIORITY_01                                  1

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ2��
  */
#define OS_SWI_PRIORITY_02                                  2

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ3��
  */
#define OS_SWI_PRIORITY_03                                  3

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ4��
  */
#define OS_SWI_PRIORITY_04                                  4

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ5��
  */
#define OS_SWI_PRIORITY_05                                  5

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ6��
  */
#define OS_SWI_PRIORITY_06                                  6

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ7��
  */
#define OS_SWI_PRIORITY_07                                  7

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ8��
  */
#define OS_SWI_PRIORITY_08                                  8

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ9��
  */
#define OS_SWI_PRIORITY_09                                  9

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ10��
  */
#define OS_SWI_PRIORITY_10                                  10

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ11��
  */
#define OS_SWI_PRIORITY_11                                  11

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ12��
  */
#define OS_SWI_PRIORITY_12                                  12

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ13��
  */
#define OS_SWI_PRIORITY_13                                  13

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ14��
  */
#define OS_SWI_PRIORITY_14                                  14

/**
 * @ingroup SRE_swi
 * ���ж����ȼ�Ϊ15��Ҳ��������ȼ���
  */
#define OS_SWI_PRIORITY_15                                  15

/**
 * @ingroup SRE_swi
 * ��ʹ�����ȼ�����ΪOS_SWI_PRIORITY_ALL�����������ȼ������ж϶����ܵõ����ȡ�
  */
#define OS_SWI_PRIORITY_ALL                                 16

/**
 * @ingroup SRE_swi
 * ��ֹ���ж����ȼ���
 */
#define OS_SWI_PRIORITY_NONE                                254     /**< Disabled SWI priority */


/**
 * @ingroup SRE_swi
 * ���ж����ȼ���������������ȡ�
 */
#define OS_SWI_PRIORITY_LOWEST                              15      /**< ���ж����ȼ���������������� */

/**
 * @ingroup SRE_swi
 * ���ж�������󳤶ȡ�
 *
 * ���ж�������󳤶ȣ�������β��'\0'��
 */
#define OS_SWI_NAME_LEN                                     16

/**
 * @ingroup SRE_swi
 * ���жϻ����жϿ��ƿ�״̬��־��
 *
 * OS_SWI_UNUSED     --- ���жϿ��ƿ�δ��ʹ�á�
 */
#define OS_SWI_UNUSED                                       0x0000  /**< ���жϿ��ƿ�δ��ʹ�á� */
/**
 * @ingroup SRE_swi
 * ���жϻ����жϿ��ƿ�״̬��־��
 *
 * OS_SWI_CREATED    --- ���ж��Ѵ�����
 */
#define OS_SWI_CREATED                                      0x0001  /**< ���ж��Ѵ�����         */
/**
 * @ingroup SRE_swi
 * ���жϻ����жϿ��ƿ�״̬��־��
 *
 * OS_SWI_READY      --- ���ж��Ѽ���������С�
 */
#define OS_SWI_READY                                        0x0004  /**< ���ж��Ѽ���������С� */
/**
 * @ingroup SRE_swi
 * ���жϻ����жϿ��ƿ�״̬��־��
 *
 * OS_SWI_ACTIVE     --- ���ж���������
 */

#define OS_SWI_ACTIVE                                       0x0008  /**< ���ж���������       */

#if  (OS_HAVE_COPROCESSOR1 == YES)
/**
 * @ingroup SRE_swi
 * ���жϻ����жϿ��ƿ�״̬��־��
 *
 * OS_SWI_CP_CONTEXT_SAVED     --- ���ж�ʹ��coprocessor��ʸ���Ĵ����ֳ��ѱ��档
 */

#define OS_SWI_CP_CONTEXT_SAVED                             0x0002  /**< ���ж�ʹ��coprocessor��ʸ���Ĵ����ֳ��ѱ��� */
#endif

/*
 * ���ж�ģ��Ĵ����붨�塣
 */
/**
 * @ingroup SRE_swi
 * ���жϴ����룺�����ڴ�ʧ�ܡ�
 *
 * ֵ: 0x02000900
 *
 * �������:���ʵ�����ϵͳĬ��˽�о�̬FSC������С
 */
#define OS_ERRNO_SWI_NO_MEMORY                              SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x00)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���ж�PID�Ƿ���
 *
 * ֵ: 0x02000901
 *
 * �������:ȷ����������ж�PID�ǺϷ���
 */
#define OS_ERRNO_SWI_ID_INVALID                             SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x01)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺ָ�����Ϊ�ա�
 *
 * ֵ: 0x02000902
 *
 * �������:�ڴ������ж�ʱ����Ҫȷ��ָ�������NULL
 */
#define OS_ERRNO_SWI_PTR_NULL                               SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x02)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���ж�δ�������Ѿ�ɾ����
 *
 * ֵ: 0x02000903
 *
 * �������:��ɾ�����ж�ʱ����Ҫȷ�������ж��Ѿ�������δɾ��
 */
#define OS_ERRNO_SWI_NOT_CREATED                            SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x03)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���ж�δ���
 *
 * ֵ: 0x02000904
 *
 * �������:��ѯ��ǰ�������жϵ�PIDʱ���뱣֤�����жϴ��ڼ���̬��
 */
#define OS_ERRNO_SWI_NOT_ACTIVE                             SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x04)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��δ������жϴ�����Ϊ��ָ�롣
 *
 * ֵ: 0x02000905
 *
 * �������:�ڴ������ж�ʱ����ȷ�����жϴ������ǿա�
 */
#define OS_ERRNO_SWI_PROC_FUNC_NULL                         SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x05)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���ж����ȼ��Ƿ������ȼ�Ϊ0-15��
 *
 * ֵ: 0x02000906
 *
 * �������:�ڴ������ж�ʱ����ȷ�����жϵ����ȼ�Ϊ0-15.
 */
#define OS_ERRNO_SWI_PRIORITY_INVALID                       SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x06)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���ж����õ����֧���������ʣ����֧�������ܴ���125��
 *
 * ֵ: 0x02000907
 *
 * �������:�����ж��������м������֧������ʹ֮С�ڵ���125.
 */
#define OS_ERRNO_SWI_MAX_NUM_NOT_SUITED                     SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x07)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���ж���Ϊ���ַ������߿�ָ��
 *
 * ֵ: 0x02000909
 *
 * �������:�������ж�ʱ��ȷ�����ж�����Ϊ�ա�
 */
#define OS_ERRNO_SWI_NAME_EMPTY                             SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x09)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺�˼��жϴ���ʧ�ܡ�
 *
 * ֵ: 0x0200090b
 *
 * �������:���˼����ж�������OS_INCLUDE_MCSWI�Ƿ�ΪYES
 */
#define OS_ERRNO_SWI_MC_INT_FAIL                            SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x0b)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺IDLE���жϲ���ɾ�����ڲü�������ģ�������£�ϵͳ��Ҫ����IDLE���ж��������IDLE����Ĺ��ܣ�IDLE���жϲ���ɾ����
 *
 * ֵ: 0x0200090c
 *
 * �������: ������ģ��ü����عر�ʱ��ȷ��ɾ�������жϲ���IDLE���жϡ�
 */
#define OS_ERRNO_SWI_DELETE_IDLE_SWI                        SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x0c)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���жϿ��ƿ������ꡣ
 *
 * ֵ: 0x0200090d
 *
 * �������:���������������������ж�������ֵ�Ĵ󣬵����ܳ���125��
 */

#define OS_ERRNO_SWI_SCB_USED_OUT                           SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x0d)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺ACTIVE���жϲ���ɾ��
 *
 * ֵ: 0x0200090e
 *
 * �������: ɾ�����ж�ǰ��ȷ��ɾ�������жϲ����ڼ���̬��
 */
#define OS_ERRNO_SWI_ACTIVE_DEL_INVALID                     SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x0e)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺IDLE���жϴ���ʧ�ܣ���������
 *
 * ֵ: 0x0200090f
 *
 * �������: ����ģ��ü�ʱ����ȷ��IDLE���жϴ����ɹ���
 */
#define OS_ERRNO_SWI_BGD_CREATE_FAIL                        SRE_ERRNO_OS_FATAL(OS_MID_SWI, 0x0f)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺�˼����жϴ���ʧ�ܣ����ж�û�д����������ж�PID�Ƿ����˴������ڽ��շ�����
 *
 * ֵ: 0x02000910
 *
 * �������: �����˼����ж�ʱ��ȷ�������ж�PID�Ϸ���
 */
#define OS_ERRNO_SWI_MC_ACTIVE_FAIL                         SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x10)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���жϵ�coprocessor�����ı�������δ��16�ֽڶ��롣
 *
 * ֵ: 0x02000911
 *
 * �������: ��ȷ����������жϵ�coprocessor�����ı��������ַ��16�ֽڶ��롣
 */
#define OS_ERRNO_SWI_CP_SAVE_AREA_NOT_ALIGN                 SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x11)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���жϵ�coprocessor�����ı�������ĵ�ַΪ��ָ�롣
 *
 * ֵ: 0x02000912
 *
 * �������: ��ȷ����������жϵ�coprocessor�����ı��������ַ��Ϊ�ա�
 */
#define OS_ERRNO_SWI_CP_SAVE_AREA_NULL                       SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x12)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺IDLE���ж�δ���
 *
 * ֵ: 0x02000913
 *
 * �������:����ģ�鱻�ü����뱣֤IDLE���жϡ�
 */
#define OS_ERRNO_SWI_BGD_NOT_ACTIVE                           SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x13)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺�������ж�ʱ����������ж�PID�Ƿ���
 *
 * ֵ: 0x02000914
 *
 * �������:�������ж�ʱ���뱣֤��������ж�PID�Ϸ���
 */
#define OS_ERRNO_SWI_ACT_ID_INVALID                            SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x14)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��������ģʽ���������ж�PID�Ƿ���
 *
 * ֵ: 0x02000915
 *
 * �������:ȷ����������ж�PID�ǺϷ��ġ�
 */
#define OS_ERRNO_SWI_ID_ACT_INC_INVALID                        SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x15)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��������ģʽ���������ж�PID�Ƿ���
 *
 * ֵ: 0x02000916
 *
 * �������:ȷ����������ж�PID�ǺϷ��ġ�
 */
#define OS_ERRNO_SWI_ID_ACT_DEC_INVALID                        SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x16)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺ֱ�Ӵ���ģʽ���������ж�δ�������Ѿ�ɾ����
 *
 * ֵ: 0x02000917
 *
 * �������:�ڴ������ж�ʱ����Ҫȷ�������ж��Ѿ�������δɾ����
 */
#define OS_ERRNO_SWI_ACT_NOT_CREATED                           SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x17)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��������ģʽ���������ж�δ�������Ѿ�ɾ����
 *
 * ֵ: 0x02000918
 *
 * �������:�ڴ������ж�ʱ����Ҫȷ�������ж��Ѿ�������δɾ����
 */
#define OS_ERRNO_SWI_ACT_INC_NOT_CREATED                       SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x18)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��������ģʽ���������ж�δ�������Ѿ�ɾ����
 *
 * ֵ: 0x02000919
 *
 * �������:�ڴ������ж�ʱ����Ҫȷ�������ж��Ѿ�������δɾ����
 */
#define OS_ERRNO_SWI_ACT_DEC_NOT_CREATED                       SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x19)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��λ����ģʽ�������ж�ʱ����������ж�PID�Ƿ���
 *
 * ֵ: 0x0200091a
 *
 * �������:��ʹ����λ����ģʽ�������ж�ʱ����ȷ��PID�Ϸ���
 */
#define OS_ERRNO_SWI_ID_ACT_OR_INVALID                         SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x1a)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��λ����ģʽ���������ж�δ���������Ѿ�ɾ����
 *
 * ֵ: 0x0200091b
 *
 * �������:��ʹ����λ����ģʽ�������ж�ʱ����ȷ�������ж��Ѿ�������
 */
#define OS_ERRNO_SWI_ACT_OR_NOT_CREATED                        SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x1b)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��λ����ģʽ�������ж�ʱ����������ж�PID�Ƿ���
 *
 * ֵ: 0x0200091c
 *
 * �������:��ʹ����λ����ģʽ�������ж�ʱ����ȷ��PID�Ϸ���
 */
#define OS_ERRNO_SWI_ID_ACT_AND_INVALID                        SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x1c)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��λ����ģʽ���������ж�δ���������Ѿ�ɾ����
 *
 * ֵ: 0x0200091d
 *
 * �������:��ʹ����λ����ģʽ�������ж�ʱ����ȷ�������ж��Ѿ�������
 */
#define OS_ERRNO_SWI_ACT_AND_NOT_CREATED                       SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x1d)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��ѯ���ж���Ϣʱ����������ж�PID�Ƿ���
 *
 * ֵ: 0x0200091e
 *
 * �������:��ѯ���ж���Ϣʱ��ȷ����������ж�PID�ǺϷ���
 */
#define OS_ERRNO_SWI_ID_INFO_INVALID                           SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x1e)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��ѯ���ж���Ϣʱ��ָ�����Ϊ�ա�
 *
 * ֵ: 0x0200091f
 *
 * �������:��ѯ���ж���Ϣʱ����Ҫȷ��ָ�������NULL
 */
#define OS_ERRNO_SWI_INFO_PTR_NULL                             SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x1f)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺��ѯ��ǰ�������ж�PIDʱ��ָ�����Ϊ�ա�
 *
 * ֵ: 0x02000920
 *
 * �������:�ڲ�ѯ��ǰ�������ж�PIDʱ����Ҫȷ��ָ�������NULL
 */
#define OS_ERRNO_SWI_SELF_PTR_NULL                             SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x20)

/**
 * @ingroup SRE_swi
 * ���жϴ����룺���жϴ��������Ϣ���㡣
 *
 * ֵ: 0x02000921
 *
 * �������:ɾ�����ж�ǰ��ȷ���Ѵ�����������Ϣ
 */
#define OS_ERRNO_SWI_MSG_NONZERO                               SRE_ERRNO_OS_ERROR(OS_MID_SWI, 0x21)

/**
 * @ingroup SRE_swi
 * ���жϺŵ����Ͷ��塣
 */
typedef UINT32                                              SWI_HANDLE_T;


/**
 * @ingroup SRE_swi
 * ���ж����ȼ������Ͷ��塣
 */
typedef UINT16                                              SWI_PRIOR_T;


/**
 * @ingroup SRE_swi
 * ���ж�״̬�����Ͷ��塣
 */
typedef UINT16                                              SWI_STATUS_T;

/**
 * @ingroup  SRE_swi
 * @brief ���жϴ����������Ͷ��塣
 *
 * @par ����:
 * ͨ�����жϴ��������Ͷ������жϴ��������û�ͨ���ù��Ӻ�����Ӧ���жϡ�
 *
 * @attention �ޡ�
 *
 * @param  puwSwiID    [OUT] ����Ϊ#UINT32�����жϴ����ɹ��󷵻ص�PID��
 *
 * @retval �ޡ�
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see �ޡ�
 */
typedef VOID (* SWI_PROC_FUNC)(UINT32);

/**
 * @ingroup SRE_swi
 * ���ж�ģ��������Ϣ�Ľṹ�嶨�塣
 *
 * �������ж�ģ�����������Ϣ��
 */
typedef struct tagSwiModInfo {
	UINT32 uwMaxNum;                    /* ���֧�ֵ����ж��� */
} SWI_MOD_INFO_S;

/**
 * @ingroup SRE_swi
 * ���ж���Ϣ�Ľṹ�嶨�塣
 *
 * �û����Բ�ѯ�����ж���Ϣ��
 */
typedef struct tagSwiInfo {
	SWI_STATUS_T  usStatus;            /* ���ж�״̬          */
	SWI_PRIOR_T   usPriority;          /* ���ж����ȼ�        */
	UINT32        uwNumOfMsg;          /* ���д����յ���Ϣ��  */
} SWI_INFO_S;

/**
 * @ingroup SRE_swi
 * ���жϴ�����νṹ�嶨�塣
 *
 * �û��������ж���νṹ�壬�������жϴ����������ȼ�����ʼֵ�����ơ�
 */
typedef struct tagSWI_INIT_PARA {
	SWI_PROC_FUNC pfnHandler  ;         /* ���жϴ���ʱ�Ĵ�������         */
	SWI_PRIOR_T usPriority;             /* ����ΪSWI_PRIOR_T�����ж����ȼ���*/
	UINT16 usResved;                    /* ����ΪUINT16������ֵ��           */
	UINT32 uwInitValue;                 /* ����ΪUINT32�����жϳ�ʼֵ��     */
	CHAR *pcSwiName;                    /* ����ΪCHAR*�����ж����ơ�        */
} SWI_INIT_PARA_S;

/**
 * @ingroup  SRE_swi
 * @brief �������жϡ�
 *
 * @par ����:
 * Ϊ���˴���һ�����жϡ����ж�һ����Ӳ�ж��м���Էֵ�����Ӳ�ж��е�һЩ��Ҫ���ܡ�
 *
 * @attention
 * <ul>
 * <li>���ȼ�ȡֵ��Χ0-15��������ȼ�0��������ȼ�15 </li>
 * <li>һ�����ȼ����Դ���������жϣ�����ü�������ģ�飬���ܴ���������ȼ������жϣ����򣬴�������ܽ��ò�������</li>
 * <li>���ж�������󳤶�Ϊ16�ֽ�(��������)ͬһ�������ж����������ظ����Ҳ����������������</li>
 * </ul>
 *
 * @param  puwSwiID    [OUT] ����Ϊ#SWI_HANDLE_T*�����жϴ����ɹ��󷵻ص�PID��
 * @param  pstInitPara [IN]  ����Ϊ#SWI_INIT_PARA_S�����жϴ�����������ṹ����Ϣ�������������ĸ���Ա:
 * @param  pfnHandler  [IN]  ����Ϊ#SWI_PROC_FUNC�����жϴ���ʱ�Ĵ�������
 * @param  usPriority  [IN]  ����Ϊ#SWI_PRIOR_T�����ж����ȼ���
 * @param  uwInitValue [IN]  ����Ϊ#UINT32�����жϳ�ʼֵ��
 * @param  pcSwiName   [IN]  ����Ϊ#CHAR*�����ж����ơ�
 *
 * @retval #OS_ERRNO_SWI_PTR_NULL               0x02000902��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_SWI_PROC_FUNC_NULL         0x02000905�����жϴ�����Ϊ�ա�
 * @retval #OS_ERRNO_SWI_PRIORITY_INVALID       0x02000906�����ж����ȼ��Ƿ���
 * @retval #OS_ERRNO_SWI_NAME_EMPTY             0x02000909�����ж����ֲ���ΪNULL�Ϳմ���
 * @retval #OS_ERRNO_SWI_SCB_USED_OUT           0x0200090d��û�п��õ����жϿ��ƿ�
 * @retval #OS_ERRNO_HUNT_THREAD_NAME_REPEAT    0x02001f01���������ж�ʱ���߳�������.
 * @retval #SRE_OK                              0x00000000���������жϳɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SwiDelete | SRE_SwiActivate
 */
extern UINT32 SRE_SwiCreate(SWI_HANDLE_T *puwSwiID, SWI_INIT_PARA_S *pstInitPara);

/**
 * @ingroup  SRE_swi
 * @brief ɾ�����жϡ�
 *
 * @par ����:
 * ɾ��һ�����жϣ��������жϿ��ƿ���Դ��
 *
 * @attention
 * <ul>
 * <li>ֻ��ɾ�����˵����жϡ�</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 *
 * @retval #OS_ERRNO_SWI_ID_INVALID             0x02000901�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_NOT_CREATED            0x02000903�����ж�δ������
 * @retval #OS_ERRNO_SWI_MSG_NONZERO            0x02000921�����жϴ��������Ϣ���㡣
 * @retval #OS_ERRNO_SWI_DELETE_IDLE_SWI        0x0200090c��IDLE���жϲ���ɾ����
 * @retval #OS_ERRNO_SWI_ACTIVE_DEL_INVALID     0x0200090e���������жϲ���ɾ����
 * @retval #SRE_OK                              0x00000000��ɾ�����жϳɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SwiDelete | SRE_SwiActivate
 */
extern UINT32 SRE_SwiDelete(SWI_HANDLE_T uwSwiID);


/**
 * @ingroup  SRE_swi
 * @brief ʹ�����жϡ�
 *
 * @par ����:
 * ʹ�ܱ��˸���ָ�����ȼ������жϡ�ֻ�и���ָ�����ȼ������жϴ���Żᱻʹ�ܣ�
 * ����OS_SWI_PRIORITY_ALL��ʹ��ȫ�����ȼ����жϡ�
 *
 * @attention
 * <ul>
 * <li>�����ý��Ա�����Ч��</li>
 * <li>�ڱ����߳�δ����ǰ��������ô˽ӿڡ�</li>
 * </ul>
 *
 * @param  usPriority [IN] ����Ϊ#SWI_HANDLE_T�����ж����ȼ���
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SwiDisable
 */
extern VOID SRE_SwiEnable(SWI_PRIOR_T usPriority);


/**
 * @ingroup  SRE_swi
 * @brief ��ֹ���жϡ�
 *
 * @par ����:
 * ��ֹ�������е����жϴ���
 *
 * @attention
 * <ul>
 * <li>��ֹ�󽫲���������жϴ�����������������ж��Ի����������С�</li>
 * <li>�ڱ����߳�δ����ǰ��������ô˽ӿڡ�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ��ֹǰ�����ж�ʹ�ܼ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiEnable
 */
extern SWI_PRIOR_T SRE_SwiDisable(VOID);


/**
 * @ingroup  SRE_swi
 * @brief ֱ�Ӵ������жϡ�
 *
 * @par ����:
 * ����һ�����жϣ����ı����жϵļ���ֵ��֧�ֺ˼������
 *
 * @attention
 * <ul>
 * <li>�ڱ����߳�δ����ǰ��������ô˽ӿڡ�</li>
 * <li>���жϵ���ִ��ʱ��ʵʱֵ��������жϵ�����ֵ�У�ʵʱֵ�ָ�Ϊ����ʱ�ĳ�ʼֵ��</li>
 * <li>���ڶ��RTOSckϵͳ�����ӿڵĲ�����������Ǳ��˻��������˵����жϡ�</li>
 * <li>��ȡ�Ǳ������ж�PID�ķ���������ʱ�����ж�PID�����ں˼乲������У���ʹ��Huntģ��ӿ�ͨ�����ж����ƻ�ȡ��PID��</li>
 * <li>�����������Ϊ�Ǳ������жϣ���ᴥ���˼��жϣ����ͼ��������</li>
 * <li>�����˼����ж�ʱ��ֻ֧��Ŀ�ĺ˺��Ƿ�Ϸ����жϣ���֧�����ж�PID�Ƿ�Ϸ����жϡ�</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID����Դ�����жϴ����ɹ��������
 *
 * @retval #OS_ERRNO_SWI_ACT_ID_INVALID         0x02000914�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_ACT_NOT_CREATED        0x02000917�����ж�δ������
 * @retval #OS_ERRNO_SWI_MC_INT_FAIL            0x0200090b�������˼����ж�ʧ�ܡ�
 * @retval #OS_ERRNO_MCHWI_DST_INVALID          0x02001480���������ж�ʱ�����жϵ�ID��Ч��
 * @retval #OS_ERRNO_IPC_SHQ_FULL               0x02001486��6181��6108ƽ̨�£����ͺ˼��жϣ��������������
 * @retval #SRE_OK                              0x00000000���������жϳɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiEnable | SRE_SwiDisable
 */
extern UINT32 SRE_SwiActivate(SWI_HANDLE_T uwSwiID);


/**
 * @ingroup  SRE_swi
 * @brief ��������ģʽ�������жϡ�
 *
 * @par ����:
 * ʵʱֵ��1����������Ӧ���жϡ�
 *
 * @attention
 * <ul>
 * <li>����Ϊ�����ɹ�ʱ���ص�PID�ţ�Ҳ����ͨ��SRE_HuntByName��ȡ���жϵ�PID��</li>
 * <li>���ж�ʵʱֵ�ڴ����ж�ʱͨ���жϳ�ʼֵָ����</li>
 * <li>�����˼����ж�ʱ��ֻ֧��Ŀ�ĺ˺��Ƿ�Ϸ����жϣ���֧�����ж�PID�Ƿ�Ϸ����жϡ�</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 *
 * @retval #OS_ERRNO_SWI_ID_ACT_INC_INVALID     0x02000915�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_ACT_INC_NOT_CREATED    0x02000918�����ж�δ������
 * @retval #OS_ERRNO_SWI_MC_INT_FAIL            0x0200090b���˼����жϴ���ʧ�ܡ�
 * @retval #OS_ERRNO_MCHWI_DST_INVALID          0x02001480���������ж�ʱ�����жϵ�ID��Ч��
 * @retval #OS_ERRNO_IPC_SHQ_FULL               0x02001486��6181��6108ƽ̨�£����ͺ˼��жϣ��������������
 * @retval #SRE_OK                              0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiDec | SRE_SwiOr | SRE_SwiAndn
 */
extern UINT32 SRE_SwiInc(SWI_HANDLE_T uwSwiID);


/**
 * @ingroup  SRE_swi
 * @brief ��������ģʽ�������жϡ�
 *
 * @par ����:
 * ʵʱֵ��1������0ʱ�ż������жϣ�����ֻ������1����
 *
 * @attention
 * <ul>
 * <li>���ʵʱֵ�Ѿ�Ϊ0�ˣ����øýӿڣ������ٴδ������жϡ�</li>
 * <li>����Ϊ�����ɹ�ʱ���ص�PID�ţ�Ҳ����ͨ��SRE_HuntByName��ȡ���жϵ�PID��</li>
 * <li>���ж�ʵʱֵ�ڴ������ж�ʱͨ�����жϳ�ʼֵָ����</li>
 * <li>�����˼����ж�ʱ��ֻ֧��Ŀ�ĺ˺��Ƿ�Ϸ����жϣ���֧�����ж�PID�Ƿ�Ϸ����жϡ�</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 *
 * @retval #OS_ERRNO_SWI_ID_ACT_DEC_INVALID     0x02000916�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_ACT_DEC_NOT_CREATED    0x02000919�����ж�δ������
 * @retval #OS_ERRNO_SWI_MC_INT_FAIL            0x0200090b���˼����жϴ���ʧ�ܡ�
 * @retval #OS_ERRNO_MCHWI_DST_INVALID          0x02001480���������ж�ʱ�����жϵ�ID��Ч��
 * @retval #OS_ERRNO_IPC_SHQ_FULL               0x02001486��6181��6108ƽ̨�£����ͺ˼��жϣ��������������
 * @retval #SRE_OK                              0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiInc | SRE_SwiOr | SRE_SwiAndn
 */
extern UINT32 SRE_SwiDec(SWI_HANDLE_T uwSwiID);


/**
 * @ingroup  SRE_swi
 * @brief ��λ����ģʽ�������жϡ�
 *
 * @par ����:
 * ���ж�ʵʱֵ��������ֵ�����������жϡ�
 *
 * @attention
 * <ul>
 * <li>����Ϊ�����ɹ�ʱ���ص�PID�ţ�Ҳ����ͨ��SRE_HuntByName��ȡ���жϵ�PID��</li>
 * <li>���ж�ʵʱֵ�ڴ������ж�ʱͨ�����жϳ�ʼֵָ����</li>
 * <li>�ýӿڲ�֧�ֺ˼䴥����</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 * @param  uwMask  [IN] ����Ϊ#UINT32������ֵ��
 *
 * @retval #OS_ERRNO_SWI_ID_ACT_OR_INVALID      0x0200091a�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_ACT_OR_NOT_CREATED     0x0200091b�����ж�δ������
 * @retval #SRE_OK                              0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiInc | SRE_SwiDec | SRE_SwiAndn
 */
extern UINT32 SRE_SwiOr(SWI_HANDLE_T uwSwiID, UINT32 uwMask);


/**
 * @ingroup  SRE_swi
 * @brief ��λ����ģʽ�������жϡ�
 *
 * @par ����:
 * ���ж�ʵʱֵ�������ֵΪ1��λ����ʵʱֵ��Ϊ0ʱ�ἤ�����жϡ�
 *
 * @attention
 * <ul>
 * <li>����Ϊ�����ɹ�ʱ���ص�PID�ţ�Ҳ����ͨ��SRE_HuntByName��ȡ���жϵ�PID��</li>
 * <li>���ж�ʵʱֵ�ڴ������ж�ʱͨ�����жϳ�ʼֵָ����</li>
 * <li>�ýӿڲ�֧�ֺ˼䴥����</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����жϺš�
 * @param  uwMask  [IN] ����Ϊ#UINT32������ֵ��
 *
 * @retval #OS_ERRNO_SWI_ID_ACT_AND_INVALID     0x0200091c�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_ACT_AND_NOT_CREATED    0x0200091d�����ж�δ������
 * @retval #SRE_OK                              0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiInc | SRE_SwiDec | SRE_SwiOr
 */
extern UINT32 SRE_SwiAndn(SWI_HANDLE_T uwSwiID, UINT32 uwMask);


/**
 * @ingroup  SRE_swi
 * @brief ��ȡ��ǰ���жϵ�����ֵ��
 *
 * @par ����:
 * ��ȡ���˵�ǰ���жϵ�����ֵ��ֻ�������жϴ���ʱ���á�
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval #OS_INVALID         0xffffffff��û�д��ڼ���̬�����жϣ���Ч����
 * @retval #����ֵ ���жϴ����з��ص�����ֵ��
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiInc | SRE_SwiDec | SRE_SwiOr | SRE_SwiAndn
 */
extern UINT32 SRE_SwiValueGet(VOID);


/**
 * @ingroup  SRE_swi
 * @brief ��ȡ���жϵ�״̬��
 *
 * @par ����:
 * ��ȡ����ָ�����жϵ�״̬����֧�ֻ�ȡ�����˵����ж�״̬��
 *
 * @attention ��
 * <ul>
 * <li>���ж�״̬����������״̬��϶��ɣ�δʹ��(OS_SWI_UNUSED), �Ѵ���(OS_SWI_CREATED), ����(OS_SWI_READY), ����(OS_SWI_ACTIVE)
 * ��Ϻ�����ж�״̬Ϊ��δʹ�á��Ѵ������Ѵ����Ҿ������Ѵ����Ҽ��</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 *
 * @retval #OS_INVALID         0xffffffff�������ж�PID��Ч��ʧ�ܷ��ء�
 * @retval #����ΪSWI��״ֵ̬��
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiValueGet
 */
extern SWI_STATUS_T SRE_SwiStatusGet(SWI_HANDLE_T uwSwiID);


/**
 * @ingroup  SRE_swi
 * @brief ��ȡ���жϵ���Ϣ
 *
 * @par ����:
 * ��ȡ����ָ�����жϵ���Ϣ���������ж�״̬�����ȼ���δ���յ���Ϣ����
 *
 * @attention ��
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 * @param  pstSwiInfo [OUT] ����#SWI_INFO_S *���������ж���Ϣ��
 *
 * @retval #OS_ERRNO_SWI_INFO_PTR_NULL          0x0200091f��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_SWI_ID_INFO_INVALID        0x0200091e�����ж�PID�Ƿ���
 * @retval #SRE_OK                              0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V200R003C03
 * @see SRE_SwiValueGet | SRE_SwiStatusGet | SRE_SwiSelf
 */
extern UINT32 SRE_SwiInfoGet(SWI_HANDLE_T uwSwiID, SWI_INFO_S *pstSwiInfo);


/**
 * @ingroup  SRE_swi
 * @brief ��ȡ��ǰ���жϺš�
 *
 * @par ����:
 * ���ر��˵�ǰ���жϺţ�ֻ�������жϴ���ʱ���á�
 *
 * @attention ��
 *
 * @param  pusSwiID [OUT] ����Ϊ#SWI_HANDLE_T *���������жϺš�
 *
 * @retval #OS_ERRNO_SWI_SELF_PTR_NULL          0x02000920��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_SWI_NOT_ACTIVE             0x02000904�����ж�δ���
 * @retval #SRE_OK                              0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V100R001C01
 * @see SRE_SwiValueGet | SRE_SwiStatusGet
 */
extern UINT32 SRE_SwiSelf(SWI_HANDLE_T *pusSwiID);


/**
 * @ingroup  SRE_swi
 * @brief ��ȡ���ж����ȼ�
 *
 * @par ����:
 * ��ȡ����ָ�����жϵ����ȼ�
 *
 * @attention ��
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 *
 * @retval #0xFFFF    ��ȡʧ��
 * @retval #����ֵ    ��ȡ���ȼ��ɹ�
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SwiValueGet | SRE_SwiStatusGet | SRE_SwiSelf
 */
extern SWI_PRIOR_T SRE_SwiPriorityGet(SWI_HANDLE_T uwSwiID);

/**
 * @ingroup  SRE_swi
 * @brief ��ȡ���ж�δ������Ϣ�ĸ���
 *
 * @par ����:
 * ��ȡ���ж�δ������Ϣ����
 *
 * @attention ��
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 *
 * @retval #-1       ��ȡʧ��
 * @retval #����ֵ   ��ȡ�����յ���Ϣ�����ɹ�
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SwiValueGet | SRE_SwiStatusGet | SRE_SwiSelf
 */
extern INT32 SRE_SwiMsgNumGet(SWI_HANDLE_T uwSwiID);


#if  (OS_HAVE_COPROCESSOR1 == YES)
/**
 * @ingroup  SRE_swi
 * @brief ���ж���ִ��ʸ����������ڡ�
 *
 * @par ����:
 * ���ж���ִ��ʸ�����������, ʸ��������ʼ֮ǰ����ô˽ӿڡ�
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
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V200R003C05
 * @see SRE_TaskCpExit
 */
extern VOID SRE_SwiCpEntry(VOID);

/**
 * @ingroup  SRE_swi
 * @brief �����ж���ִ��ʸ�������ĳ��ڡ�
 *
 * @par ����:
 * ���ж���ִ��ʸ�������ĳ��ڣ�ʸ���������֮������ô˽ӿڡ�
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
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V200R003C05
 * @see SRE_SwiCpEntry
 */
extern VOID SRE_SwiCpExit(VOID);


/**
 * @ingroup  SRE_swi
 * @brief �����жϷ���ʸ���������������ݱ�����
 *
 * @par ����:
 * ��ָ�����жϷ���ʸ���������������ݱ�����
 *
 * @attention
 * <ul>
 * <li>�ò���������SD6108/SD6181ƽ̨��DSP�ˣ�SD6182ƽ̨��DSP�ˣ�CPU�ˣ�HACCC�ˡ�</li>
 * <li>����ʸ�������������õ��˵�˫LoadStoreָ��������õ�SaveAreaA/SaveAreaB�ڲ�ͬ��Memory Bank�С�</li>
 * <li>�����ʸ�����������ݱ�������ʼ��ַ���밴16�ֽڶ��롣</li>
 * <li>ʸ�����������ݱ�������С�����û�����оƬʵ�ʴ��ڵ�ʸ���Ĵ����ܴ�Сȷ����</li>
 * </ul>
 *
 * @param  uwSwiID [IN] ����Ϊ#SWI_HANDLE_T�����ж�PID�š�
 * @param  pstParam [IN] ����#CP_SAVE_CFG_PARAM_S *�������ṹָ�롣
 *
 * @retval #OS_ERRNO_SWI_CP_SAVE_AREA_NULL      0x02000912�����жϵ�coprocessor�����ı�������ĵ�ַΪ��ָ�롣
 * @retval #OS_ERRNO_SWI_ID_INVALID             0x02000901�����ж�PID�Ƿ���
 * @retval #OS_ERRNO_SWI_CP_SAVE_AREA_NOT_ALIGN 0x02000911�����жϵ�coprocessor�����ı�������δ��16�ֽڶ��롣
 * @retval #SRE_OK                          0x00000000���ɹ���
 *
 * @par ����:
 * <ul><li>sre_swi.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since UniDSP V200R003C05
 * @see SRE_SwiCpEntry | SRE_SwiCpExit
 */
extern UINT32 SRE_SwiCpSaveCfg(SWI_HANDLE_T uwSwiID, CP_SAVE_CFG_PARAM_S *pstParam);
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_SWI_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

