/**
 * @file sre_base.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������RTOSck�Ļ���ͷ�ļ��� \n
 */

/** @defgroup SRE_memory  �ڴ�ӿ� */
/** @defgroup SRE_kernel  �ں˽ӿ� */
/** @defgroup SRE_comm    ͨ�Žӿ� */
/** @defgroup SRE_inspect ���Խӿ� */
/** @defgroup v_Patch     �����ӿ� */

#ifndef _SRE_BASE_H
#define _SRE_BASE_H

#include "sre_buildef.h"
#include "sre_typedef.h"
#include "sre_module.h"
#include "sre_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#define osTskScheduleFast osTskSchedule
#define osTaskTrap  osSwiTrap
#define OS_ASSERT
#define OS_ASSERT_COND(expression)

#define ALIGN(uwAddr, uwBoundary)       (((uwAddr) + (uwBoundary) - 1) & ~((uwBoundary) - 1))
#define TRUNCATE(uwAddr, uwSize)        ((uwAddr) & ~((uwSize) - 1))

#define READ_UINT8(ucValue, ucArg)                          ucValue = (UINT8)(ucArg)
#define READ_UINT16(usValue, usArg)                         usValue = (UINT16)(usArg)
#define READ_UINT32(uwValue, uwArg)                         uwValue = (UINT32)(uwArg)
#define READ_UINT64(ullValue, ullArg)                       ullValue = (UINT64)(ullArg)
#define GET_UINT8(ucArg)                                    (UINT8)(ucArg)
#define GET_UINT16(usArg)                                   (UINT16)(usArg)
#define GET_UINT32(uwArg)                                   (UINT32)(uwArg)
#define GET_UINT64(ullArg)                                  (UINT64)(ullArg)
#define WRITE_UINT8(ucArg, ucValue)                         ucArg = (UINT8)(ucValue)
#define WRITE_UINT16(usArg, usValue)                        usArg = (UINT16)(usValue)
#define WRITE_UINT32(uwArg, uwValue)                        uwArg = (UINT32)(uwValue)
#define WRITE_UINT64(ullArg, ullValue)                      ullArg = (UINT64)(ullValue)

#define W_DWB {asm volatile ("dsb");}

#define write32(addr, val) {W_DWB; \
        (*(volatile unsigned int *)  (addr) = (val));\
        W_DWB;}
#define  read32(addr)      (*(volatile unsigned int *)  (addr))
#define write16(addr, val)  {W_DWB; \
        (*(volatile unsigned short *)(addr) = (val));\
         W_DWB;}
#define  read16(addr)      (*(volatile unsigned short *)(addr))
/**
 * @ingroup  SRE_sys
 * ���߳�PID��ȡ�����߳�handle��
 */
#define GET_HANDLE(pid)               ((pid)&0x000000FF)
/**
 * @ingroup  SRE_sys
 * ���߳�PID��ȡ�˺�
 */
#define GET_COREID(pid)               ((UINT8)(((pid)&0x0000FF00) >> 8))
/**
 * @ingroup  SRE_sys
 * ��coreid��handle���PID
 */
#define COMPOSE_PID(coreid, handle)   ((((UINT32)(coreid))<<8) + ((UINT8)(handle))) /* ��coreid��handle���PID,UIPC��ʹ�øýӿ� */
/**
 * @ingroup  SRE_sys
 * �˼书��ģ���δ��ʼ����־λ
 */
#define OS_MC_PHASE_UNINIT                                  0
/**
 * @ingroup  SRE_sys
 * �˼书��ģ������ڳ�ʼ����־λ
 */
#define OS_MC_PHASE_HANDLE                                  0xae1234ea
/**
 * @ingroup  SRE_sys
 * �˼书��ģ��ĳ�ʼ����ɱ�־λ
 */
#define OS_MC_PHASE_INITED                                  0xad5678da

/**
 * @ingroup  SRE_sys
 *�˼��ź������ź�������Ϣ�ȴ�ʱ���趨����ʾ���ȴ���
 */
#define OS_NO_WAIT            0

/**
 * @ingroup  SRE_sys
 * CpuTick�ṹ�����͡�
 *
 * ���ڼ�¼64λ��cycle����ֵ��
 */
typedef struct tagCpuTick {
	UINT32 uwCntHi;         /**<  cycle������32λ */
	UINT32 uwCntLo;         /**<  cycle������32λ */
} CPU_TICK;


/**
 * @ingroup  SRE_sys
 * @brief ���жϡ�
 *
 * @par ����:
 * ����ȫ�ֿ������жϡ�
 *
 * @attention �жϷ����������øýӿڣ��������ж����ȼ���ת
 *
 * @param ��
 *
 * @retval ����ȫ���ж�ǰ���ж�״ֵ̬��
 * @par ����:
 * <ul><li>sre_base.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_IntLock | SRE_IntRestore
 */
OS_SEC_ALW_INLINE INLINE UINTPTR SRE_IntUnLock(VOID)
{
	UINTPTR uwCpsSave;
	__asm__ __volatile__(
		"MRS   %0, cpsr\n"
		"BIC   %0, %0, #0xc0\n"
		"MSR   cpsr_c, %0"
		:"=r"(uwCpsSave) : "0"(uwCpsSave));
	return uwCpsSave;
}

/**
 * @ingroup  SRE_sys
 * @brief ���жϡ�
 *
 * @par ����:
 * �ر�ȫ�ֿ������жϡ�
 *
 * @attention �ڹ�ȫ���жϺ󣬽�ֹ���������ں˵��ȵ���ؽӿڣ���SRE_TaskDelay�ӿ�
 *
 * @param ��
 *
 * @retval �ر�ȫ���ж�ǰ���ж�״ֵ̬��
 * @par ����:
 * <ul><li>sre_base.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_IntUnLock | SRE_IntRestore
 */
OS_SEC_ALW_INLINE INLINE UINTPTR SRE_IntLock(VOID)
{
	/*DTS2013040106948--start*/
	UINTPTR uvIntSave;

	__asm__ __volatile__(
		"mrs %0, cpsr"
		: "=r"(uvIntSave));

	__asm__ __volatile__("cpsid if");


	return uvIntSave;

	/*DTS2013040106948--end*/
}

/**
 * @ingroup  SRE_sys
 * @brief �ָ��ж�״̬�ӿڡ�
 *
 * @par ����:
 * �ָ�ԭ�ж�״̬�Ĵ�����
 *
 * @attention
 *<ul>
 *<li>�ýӿڱ���͹ر�ȫ���жϻ����ǿ���ȫ���жϽӿڳɶ�ʹ�ã��Թ�ȫ���жϻ��߿�ȫ���жϲ����ķ���ֵΪ���</li>
 *<li>�Ա�֤�жϿ��Իָ�����ȫ���жϻ��߿�ȫ���жϲ���ǰ��״̬</li>
 *</ul>
 * @param  uvIntSave [IN]����#UINTPTR����ȫ���ж�SRE_IntLock�Ϳ�ȫ���ж�SRE_IntUnLock�ķ���ֵ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_base.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_IntUnLock | SRE_IntLock
 */

OS_SEC_ALW_INLINE INLINE VOID SRE_IntRestore(UINTPTR uvIntSave)
{
	/*DTS2013040106948--start*/
	__asm__ __volatile__(
		"MSR     cpsr_c, %0"
		::"r"(uvIntSave));
	/*DTS2013040106948--end*/
}

/**
 * @ingroup  SRE_sys
 * @brief ��ȡ��ǰ��64λcycle����,cycle�����ĸ�32λ���浽����puwCntHi����32Ϊ���浽����puwCntLo��
 *
 * @par ����:
 * ��ȡ��ǰ��64λcycle����,cycle�����ĸ�32λ���浽����puwCntHi����32Ϊ���浽����puwCntLo��
 *
 * @attention ��
 *
 * @param  puwCntHi [OUT]����#UINT32 *������cycle�����ĸ�32λ��
 * @param  puwCntLo [OUT]����#UINT32 *������cycle�����ĵ�32λ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_base.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CycleCountGet64()
 */
extern VOID SRE_CycleCountGet(UINT32 *puwCntHi, UINT32 *puwCntLo);

/**
 * @ingroup  SRE_sys
 * @brief ��ȡ��ǰ��64λtime stamp����(��ϵͳ���е�cycles)��
 *
 * @par ����:
 * ��ȡ��ǰ��64λtime stamp����(��ϵͳ���е�cycles)��
 *
 * @attention ��ȡ����64bit cycles ����
 *
 * @param  ��
 *
 * @retval 0~0xFFFFFFFFFFFFFFFF ϵͳ��ǰ��cycle��
 * @par ����:
 * <ul><li>sre_base.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CycleCountGet32()
 */
//extern UINT64    SRE_CycleCountGet64(void);

/**
 * @ingroup  SRE_sys
 * @brief ��ȡ��ǰ��32λtime stamp����(��ϵͳ���е�cycles)��
 *
 * @par ����:
 * ��ȡ��ǰ��32λtime stamp����(��ϵͳ���е�cycles)��
 *
 * @attention ��ȡ����32bit cycles ����
 * �����λ�ȡʱ��������ᷢ�����ݷ�ת���󣬴�����½���ʹ�û�ȡ64bit���ݽӿڡ�
 *
 * @param  ��
 *
 * @retval 0~0xFFFFFFFF  ϵͳ��ǰ��cycle��
 * @par ����:
 * <ul><li>sre_base.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CycleCountGet64()
 */
#if (OS_HARDWARE_PLATFORM != OS_CORTEX_A9)
OS_SEC_ALW_INLINE INLINE UINT32 SRE_CycleCountGet32()
{
	UINT32 uwCCOUNT;
	__asm__ __volatile__("rsr %0, ccount" : "=a"(uwCCOUNT));

	return uwCCOUNT;
}
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

