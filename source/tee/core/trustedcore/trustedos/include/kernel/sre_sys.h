/**
 * @file sre_sys.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������ϵͳģ��Ķ���ͷ�ļ��� \n
 */


/** @defgroup SRE_sys ϵͳ�������ܽӿ� */

#ifndef _SRE_SYS_H
#define _SRE_SYS_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/*
 * ϵͳģ��Ĵ����붨�塣
 */

/**
 * @ingroup SRE_sys
 * ϵͳ�������ܴ����룺ָ�����Ϊ�ա�
 *
 * ֵ: 0x02000011
 *
 * �������: ��
 */
#define OS_ERRNO_SYS_PTR_NULL                               SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x11)

/**
 * @ingroup SRE_sys
 * ϵͳ�������ܴ����룺ϵͳ��Ƶ���÷Ƿ���
 *
 * ֵ: 0x02000012
 *
 * �������: ��Config.h�����ú����ϵͳ��Ƶ��
 */
#define OS_ERRNO_SYS_CLOCK_INVALID                          SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x12)

/**
 * @ingroup SRE_sys
 * ϵͳ�������ܴ����룺�û�������ѡ��OS_SYS_CORE_MAX_NUM����Ӧ�ú�оƬƥ�䡣
 *
 * ֵ: 0x02000013
 *
 * �������:�޸�����ѡ��OS_SYS_CORE_MAX_NUM����оƬƥ�䡣
 *
 */
#define OS_ERRNO_SYS_MAXNUMOFCORES_IS_INVALID               SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x13)

/**
 * @ingroup SRE_sys
 * ϵͳ�������ܴ����룺�����쳣�ӹܺ����ô���
 *
 * ֵ: 0x02000014
 *
 * �������:�����쳣�ӹܺ�ֻ����CPU12-CPU16���е�һ��,�����о����config���õĺ˺ű���һ�¡�
 *
 */
#define OS_ERRNO_SYS_PERIERRCOREID_IS_INVALID               SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x14)

/**
 * @ingroup SRE_sys
 * ϵͳ�������ܴ����룺�����Ѵﵽ��������ע������
 *
 * ֵ: 0x02000017
 *
 * �������:�����ظ�ע��
 *
 */
#define OS_ERRNO_SYS_HOOK_IS_FULL                           SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x17)

extern UINT32               g_uwSystemClock;
extern UINT64               g_ullUniTicks;
/**
 * @ingroup  SRE_sys
 * @brief ��ȡ���˵�tick����˽�е�ַ
 *
 * @par ����:
 * ��ȡ���˵�tick����˽�е�ַ
 *
 * @attention ��
 *
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see ��
 */
#define SRE_GetTickAddr()   ((UINT32)(UINT32 *)&g_ullUniTicks)

#define SRE_SYS_MS_PER_SECOND                               1000
#define SRE_SYS_US_PER_SECOND                               1000000

/**
 * @ingroup  SRE_sys
 * @brief ת��cycleΪ����
 *
 * @par ����:
 * ת��cycleΪ����
 *
 * @attention ��
 *
 * @param  udwCycle    [IN] ����Ϊ#UINT64��cycle��
 *
 * @retval 0~0xFFFFFFFFFFFFFFFF ת����ĺ�����
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_Cycle2US
 */
OS_SEC_ALW_INLINE INLINE UINT64 SRE_Cycle2MS(UINT64 udwCycle)
{
	return (UINT64)((udwCycle / (g_uwSystemClock / SRE_SYS_MS_PER_SECOND)));
}

/**
 * @ingroup  SRE_sys
 * @brief ת��cycleΪ΢��
 *
 * @par ����:
 * ת��cycleΪ΢��
 *
 * @attention ��
 *
 * @param  udwCycle    [IN] ����Ϊ#UINT64��cycle��
 *
 * @retval 0~0xFFFFFFFFFFFFFFFF ת�����΢����
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_Cycle2MS
 */
OS_SEC_ALW_INLINE INLINE UINT64 SRE_Cycle2US(UINT64 udwCycle)
{
	return (UINT64)((udwCycle / (g_uwSystemClock / SRE_SYS_US_PER_SECOND)));
}


/**
 * @ingroup  SRE_sys
 * @brief ��ȡ��ǰ��tick����
 *
 * @par ����:
 * ��ȡ��ǰ��tick����
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval  0~0xFFFFFFFFFFFFFFFF ��ǰ��tick����
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CycleCountGet
 */
extern UINT64 SRE_TickCountGet(VOID);

/**
 * @ingroup  SRE_sys
 * @brief ��ȡÿ��tick��Ӧ��Ӳ��cycle��
 *
 * @par ����:
 * ��ȡÿ��tick��Ӧ��Ӳ��cycle��
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval 0~0xFFFFFFFF ÿ��tick��Ӧ��Ӳ��cycle��
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see ��
 */
extern UINT32 SRE_CyclePerTickGet(VOID);

/**
 * @ingroup  SRE_sys
 * @brief �ӳ�ʱ��(��λcycle)
 *
 * @par ����:
 * �ӳ�ʱ��(��λcycle)
 *
 * @attention ��
 *
 * @param  udwCycle    [IN] ����Ϊ#UINT32��cycle��
 *
 * @retval ��
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see ��
 */
extern VOID SRE_DelayCycle(UINT32 uwCycles);

/**
 * @ingroup  SRE_sys
 * @brief �ӳ�ʱ��(��λ΢��)
 *
 * @par ����:
 * �ӳ�ʱ��(��λ΢��)
 *
 * @attention ��
 *
 * @param  uwDelay    [IN] ����Ϊ#UINT32��΢����
 *
 * @retval ��
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_DelayMs
 */
extern VOID SRE_DelayUs(UINT32 uwDelay);

/**
 * @ingroup  SRE_sys
 * @brief �ӳ�ʱ��(��λ����)
 *
 * @par ����:
 * �ӳ�ʱ��(��λ����)
 *
 * @attention ��
 *
 * @param  uwDelay    [IN] ����Ϊ#UINT32��������
 *
 * @retval ��
 * @par ����:
 * <ul><li>SRE_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_DelayUs
 */
extern VOID SRE_DelayMs(UINT32 uwDelay);

/**
 * @ingroup  SRE_sys
 * @brief ϵͳ�������Ӻ��������Ͷ��塣
 *
 * @par ����:
 * �û�ͨ��ϵͳ�������Ӻ��������Ͷ���ϵͳ�������ӣ���ϵͳ����֮ǰ���øù��ӡ�
 *
 * @attention �ޡ�
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see �ޡ�
*/

typedef VOID (*SYS_REBOOT_FUNC)(VOID);

/**
 * @ingroup  SRE_sys
 * @brief IDLEѭ�����ӵ����Ͷ��塣
 *
 * @par ����:
 * �û�ͨ��IDLEѭ�����ӵĺ������Ͷ��庯����ϵͳ�ڽ���IDLEѭ��֮ǰ���øù��ӡ�
 *
 * @attention �ޡ�
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see �ޡ�
*/
typedef VOID (* OS_IDLE_HOOK)(VOID);


/**
 * @ingroup SRE_sys
 * ϵͳģ��������Ϣ�Ľṹ�嶨�塣
 *
 * ����ϵͳģ�����������Ϣ��
 */
typedef struct tagSysModInfo {
	UINT32 uwSystemClock;                                   /**< CPU��Ƶ��ʱ������          */
	UINT8  ucCoreID;                                        /**< ��ǰ��ID                   */
	UINT8  ucCoreRunNum;                                    /**< ʵ�����еĺ���             */
	UINT8  ucCoreMaxNum;                                    /**< ���֧�ֵĺ���             */
	UINT8  ucCoreMaster;                                    /**< ����ID                     */
	SYS_REBOOT_FUNC pfnRebootHook;                          /**< �û�ע��ĵ��帴λ����     */
	OS_IDLE_HOOK pfnIdleHook;                               /**< �û�ע���IDLE���Ӻ���     */
	UINT8  ucCpuType;                                       /**< CPU type                   */
	UINT8  ucPeriErrCoreID;                                 /**< �������ӹܺ�             */
	UINT16 usResved;
} SYS_MOD_INFO_S;


/**
 * @ingroup  SRE_sys
 * @brief ��λ���塣
 *
 * @par ����:
 * ��λ���壬�������¼���ִ�С�
 *
 * @attention
 * <ul>
 * <li>����ֱ�Ӹ�λ���壬���ǹ��жϣ��ȴ����Ź���λ��</li>
 * <li>�û�����ͨ��������OS_SYS_REBOOT_HOOK�ҽӸ�λ������</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see ��
 */
extern VOID SRE_Reboot(VOID);

/**
 * @ingroup  SRE_sys
 * @brief ��ǰ��ID��
 *
 * @par ����:
 * ��ǰ��ID��
 *
 * @attention
 * <ul>
 * <li>��IDΪӲ���Ĵ����е�ID�š�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval �����ID��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see g_ucMasterCoreID
 */
extern UINT8 g_ucLocalCoreID;

/**
 * @ingroup  SRE_sys
 * @brief ��ȡ��ǰ��ID��
 *
 * @par ����:
 * ��ȡ��ǰ��ID��
 *
 * @attention
 * <ul>
 * <li>��ȡ�ĺ�IDΪӲ���Ĵ����е�ID�š�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval �����ID��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetNumberOfCores | SRE_GetMasterCore
 */
#define SRE_GetCoreID()          (UINT8)(g_ucLocalCoreID)

/**
 * @ingroup  SRE_sys
 * @brief ����ID��
 *
 * @par ����:
 * ����ID��
 *
 * @attention
 * <ul>
 * <li>��IDΪӲ���Ĵ����е�ID�š�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval �����ID��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see g_ucLocalCoreID
 */
extern UINT8 g_ucMasterCoreID;

/**
 * @ingroup  SRE_sys
 * @brief ��ȡ����ID��
 *
 * @par ����:
 * ��ȡ����ID��
 *
 * @attention
 * <ul>
 * <li>��ȡ�ĺ�IDΪӲ���Ĵ����е�ID�š�</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval �����ID��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetNumberOfCores | SRE_GetCoreID
 */
#define SRE_GetMasterCore()      ((UINT8)(g_ucMasterCoreID + (UINT8)0))

/**
 * @ingroup  SRE_sys
 * @brief ʵ�����еĺ�����
 *
 * @par ����:
 * ʵ�����еĺ�����
 *
 * @attention
 * <ul>
 * <li>ʵ�����еĺ�����������RTOSck���֧�ֵĺ�����</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ������
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since  RTOSck V100R001C01
 * @see g_ucMaxNumOfCores
 */
extern UINT8 g_ucNumOfCores;

/**
 * @ingroup  SRE_sys
 * @brief ����֧�����еĺ�����
 *
 * @par ����:
 * ����֧�����еĺ�����
 *
 * @attention
 * <ul>
 * <li>ΪRTOSck���֧�ֵĺ�����������RTOSckʵ�����еĺ�����</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ������
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since  RTOSck V100R001C01
 * @see g_ucNumOfCores
 */
extern UINT8 g_ucMaxNumOfCores;
/**
 * @ingroup  SRE_sys
 * @brief ��ȡʵ�����еĺ�����
 *
 * @par ����:
 * ��ȡʵ�����еĺ�����
 *
 * @attention
 * <ul>
 * <li>��ȡ��Ϊʵ�����еĺ�����������OS���֧�ֵĺ�����</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ������
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since  RTOSck V100R001C01
 * @see SRE_GetCoreID | SRE_GetMasterCore
 */
#define SRE_GetNumberOfCores()       ((UINT8)(g_ucNumOfCores + (UINT8)0))

/**
 * @ingroup  SRE_sys
 * @brief ��ȡ����֧�����еĺ�����
 *
 * @par ����:
 * ��ȡ����֧�����еĺ�����
 *
 * @attention
 * <ul>
 * <li>��ȡ��ΪOS���֧�ֵĺ�����������OSʵ�����еĺ�����</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ������
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since  RTOSck V100R001C01
 * @see SRE_GetCoreID | SRE_GetMaxNumberOfCores
 */
#define SRE_GetMaxNumberOfCores()     ((UINT8)(g_ucMaxNumOfCores + (UINT8)0))

/**
 * @ingroup  SRE_sys
 * @brief ��ȡϵͳ����Ƶ��
 *
 * @par ����:
 * ��ȡϵͳ����Ƶ��
 *
 * @attention
 *
 * @param
 *
 * @retval ϵͳ��Ƶ
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetCoreID
 */
extern UINT32 SRE_GetSysClk(VOID);

/**
 * @ingroup  SRE_sys
 * @brief ��ȡOS�İ汾�š�
 *
 * @par ����:
 * ��ȡOS�İ汾�š��汾��Ÿ�ʽΪRTOSck Vx00R00xCxx SVNID(XXX)
 *
 * @attention ��
 *
 * @param  ��
 *
 * @retval ָ��OS�汾�ŵ��ַ���ָ�롣 �磺RTOSck V100R001C01 SVNID (XXX) ��
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SetAppVersion | SRE_GetAppVersion
 */
extern CHAR *SRE_GetOsVersion(VOID);


/**
 * @ingroup  SRE_sys
 * @brief ע��Ӧ�ó���İ汾�š�
 *
 * @par ����:
 * ע��Ӧ�ó���İ汾�š�
 *
 * @attention
 * <ul>
 * <li>ϵͳAPP�汾�ű�����ȫ�������У��������õ�APP�汾����󳤶�Ϊ64�ֽ�</li>
 * </ul>
 *
 * @param  pcAppVersion [IN] ����#CHAR *��ָ��Ӧ�ó���汾�ŵ��ַ���ָ�롣
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL               0x02000011��ָ�����Ϊ�ա�
 * @retval #SRE_OK                          0x02000000��ע��APP�汾�ųɹ���
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetAppVersion | SRE_GetOsVersion
 */
extern UINT32 SRE_SetAppVersion(CHAR *pcAppVersion);


/**
 * @ingroup  SRE_sys
 * @brief ��ȡӦ�ó���İ汾�š�
 *
 * @par ����:
 * ��ȡӦ�ó���İ汾�š�
 *
 * @attention ��֮ǰδע�ᣬ�򷵻�ָ��"Unknown"��ָ�롣
 *
 * @param  ��
 *
 * @retval ָ��Ӧ�ó���汾�ŵ��ַ���ָ�롣
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SetAppVersion | SRE_GetOsVersion
 */
extern CHAR *SRE_GetAppVersion(VOID);


/**
 * @ingroup  SRE_sys
 * @brief ��cycleֵת���ɺ��롣
 *
 * @par ����:
 * ��cycleֵת���ɺ��롣
 *
 * @attention ��ϵͳ��Ƶ��Ϊ1000000������ʱ���ýӿڻ�ȡ����us������
 *
 * @param  pstCpuTick [IN] ����#CPU_TICK *��cycle����ֵ��
 * @param  puwMsHi [IN] ����#UINT32 *���������ĸ�32λ��
 * @param  puwMsLo [IN] ����#UINT32 *���������ĵ�32λ��
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL               0x02000011��ָ�����Ϊ�ա�
 * @retval #SRE_OK                          0x02000000�������ɹ���
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CpuTick2US
 */
extern UINT32 SRE_CpuTick2MS(CPU_TICK *pstCpuTick, UINT32 *puwMsHi, UINT32 *puwMsLo);


/**
 * @ingroup  SRE_sys
 * @brief ��cycleֵת����΢�롣
 *
 * @par ����:
 * ��cycleֵת����΢�롣
 *
 * @attention ��
 *
 * @param  pstCpuTick [IN] ����#CPU_TICK *��cycle����ֵ��
 * @param  puwUsHi [IN] ����#UINT32 *������΢��ĸ�32λ��
 * @param  puwUsLo [IN] ����#UINT32 *������΢��ĵ�32λ��
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL               0x02000011��ָ�����Ϊ�ա�
 * @retval #SRE_OK                          0x00000000�������ɹ���
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CpuTick2MS
 */
extern UINT32 SRE_CpuTick2US(CPU_TICK *pstCpuTick, UINT32 *puwUsHi, UINT32 *puwUsLo);


/**
 * @ingroup  SRE_sys
 * @brief ע��IDLEѭ������ǰ���õĹ��ӡ�
 *
 * @par ����:
 * Ϊ����ע��IDLEѭ������ǰ���õĹ��ӣ��ù���ֻ�ᱻ����һ�Ρ�
 *
 * @attention
 * <ul>
 * <li>ע��Ĺ���ֻ�ڽ���IDLEѭ��ǰִ��һ�Ρ�</li>
 * <li>������δ�ü��������õ�������IDLEѭ���������񱻲ü��������õ������ж�IDLEѭ����</li>
 * </ul>
 *
 * @param  pfnHook    [IN] ����Ϊ#OS_IDLE_HOOK�����Ӻ�����
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL               0x02000011��ָ�����Ϊ�ա�
 * @retval #OS_ERRNO_SYS_HOOK_IS_FULL           0x02000016��IDLE�����Ѵﵽ��������ע������
 * @retval #SRE_OK                          0x00000000���ɹ���
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_IdleHookAdd
 */
extern UINT32 SRE_IdlePrefixHookAdd(OS_IDLE_HOOK pfnHook);


/**
 * @ingroup  SRE_sys
 * @brief ע��IDLEѭ���е��õĹ���
 *
 * @par ����:
 * ע����IDLE�����IDLE���ж�ѭ���е��õĹ��Ӻ��������ֻ��ע��һ����NULL��ʾȡ������ע�ᡣ
 *
 * @attention
 *<ul>
 * <li>�����в��ܵ������������������л��ĺ�����</li>
 * <li>�����sre_config.h��������OS_TSK_IDLE_HOOK�ٵ��øú����᷵��ʧ�ܡ�</li>
 * </ul>
 *
 * @param  pfnHook [OUT] ����#OS_IDLE_HOOK��IDLE���Ӻ������ò�������Ϊ�ա�
 *
 * @retval #SRE_OK                  0x00000000��ע��ɹ���
 * @retval #OS_ERRNO_SYS_HOOK_IS_FULL   0x02000016��IDLE�����Ѵﵽ��������ע������
 * @par ����:
 * <ul><li>sre_sys.h���ýӿ��������ڵ�ͷ�ļ���</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_IdlePrefixHookAdd
 */
#if defined(TESTSUITE_RTOSck_UT) || defined(TESTSUITE_RTOSck_PT)||defined(TESTSUITE_RTOSck_IT)
extern UINT32 SRE_IdleHookAdd(OS_IDLE_HOOK pfnHook);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_SYS_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

