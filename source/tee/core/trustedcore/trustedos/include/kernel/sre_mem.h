/**
* @file sre_mem.h
*
* Copyright(C), 2008-2008, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
*
* �������ڴ�������ܶ���ͷ�ļ��� \n
*/


/** @defgroup SRE_mem �ڴ��������
 *@ingroup SRE_memory
*/

#ifndef _SRE_MEM_H
#define _SRE_MEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "sre_base.h"


/**
 * @ingroup SRE_mem
 * �ڴ�������������
 */
#define OS_SLICE_MAXTYPE            8
/**
 * @ingroup SRE_mem
 * ϵͳȱʡ���ڴ����������
 * TA2TA share memory
 */
#define OS_MEM_DEFAULT_PTNUM        2
/**
 * @ingroup SRE_mem
 * ϵͳԤ��TA�ڴ����������
 */
#define OS_MEM_TASK_PTNUM        3
/**
 * @ingroup SRE_mem
 * ϵͳԤ��GT�ڴ�ط���������
 */
#define OS_MEM_GTPOOL_PTNUM        4
/**
 * @ingroup SRE_mem
 * ȱʡ˽��FSC�ڴ������
 */
#define OS_MEM_DEFAULT_FSC_PT    0
/**
 * @ingroup SRE_mem
 * OS֧�ֺ˼�ͨ�ŵ�ǰ���£�ȱʡ����̬FSC�ڴ������
 */
#define OS_MEM_DEFAULT_MCSTA_FSC_PT     1

/**
 * @ingroup SRE_mem
 * task memory partition
 */
#define OS_MEM_DEFAULT_TASKMEM_PT    4

/**
 * @ingroup SRE_mem
 * �����POOL�ڴ��ַ�����׼:16�ֽڶ��롣
 */
#define OS_MEM_ADDR_ALIGN           16

/**
 * @ingroup SRE_mem
 * �����FSC�ڴ��ַ�����׼:4�ֽڶ��롣
 */
#define OS_MEM_FSC_ADDR_ALIGN           4

/**
 * @ingroup SRE_mem
 * �ڴ��㷨����,Ŀǰ��#MEM_ARITH_MCPOOL֧��Cache�ڴ�ռ�
 */
typedef enum
{
	MEM_ARITH_FSC,        /**< ˽��FSC�㷨*/
	MEM_ARITH_POOL,       /**< ˽��POOL��̬�㷨*/
	MEM_ARITH_MCDYNFSC,   /**< ����̬FSC�㷨*/
	MEM_ARITH_MCPOOL,     /**< ����POOL��̬�㷨*/
	MEM_ARITH_MCBLOCK,     /*����Block�ڴ��㷨�����㷨�������ֵ���֧��*/
	MEM_ARITH_BUTT
}
OS_MEM_ARITH_E;

/**
 * @ingroup SRE_mem
 * �ڴ���뷽ʽ
 */
typedef enum {
	MEM_ADDR_ALIGN_004  = 4  , /**< 4�ֽڶ���*/
	MEM_ADDR_ALIGN_008  = 8  , /**< 8�ֽڶ���*/
	MEM_ADDR_ALIGN_016  = 16  ,/**< 16�ֽڶ���*/
	MEM_ADDR_ALIGN_032  = 32  ,/**< 32�ֽڶ���*/
	MEM_ADDR_ALIGN_064  = 64  ,/**< 64�ֽڶ���*/
	MEM_ADDR_ALIGN_128  = 128 ,/**< 128�ֽڶ���*/
	MEM_ADDR_ALIGN_256  = 256 ,/**< 256�ֽڶ���*/
	MEM_ADDR_ALIGN_1024 = 1024,/**< 1024�ֽڶ���*/
	MEM_ADDR_ALIGN_4096 = 4096,/**< 4096�ֽڶ���*/
	MEM_ADDR_BUTT              /**< �ֽڶ���Ƿ�*/
} OS_MEM_ALIGN_E;

/**
 * @ingroup SRE_mem
 * ��̬���������ӿڲ����ṹ�塣
 */
typedef struct tagCreatePtPara {
	OS_MEM_ARITH_E  enArith;                           /**< �������㷨����                  */
	VOID           *pPtAddr;                           /**< ������ʼ��ַ                    */
	UINT32          uwPtSize;                          /**< ������С (��λ���ֽڣ�          */
	BOOL            bCachable;                         /**< ָ���ÿ������ڴ��Ƿ�Ϊ��cache   */
	UINT16          ausSliceSize[OS_SLICE_MAXTYPE];    /**< �������ڴ���С(��λ���ֽڣ�  */
} MEM_CREATEPT_PARA;

/**
 * @ingroup SRE_mem
 * ��ȡָ�������ڴ���Ϣ�ӿڲ����ṹ�塣
 */
typedef struct tagMemStatInfo {
	UINT32 uwFreeSize;                              /**< �����ڴ�Ĵ�С����λ���ֽڣ�  */
	UINT32 uwPeakUsed;                              /**< ��ʹ���ڴ�Ĵ�С����λ���ֽڣ�*/
	UINT32 uwPtSize;                                /**< �ڴ�����ܴ�С                */
} OS_MEM_PT_STATE;

/**
 * @ingroup SRE_mem
 * �ڴ�����룺���������ķ�����С̫С
 *
 * ֵ: 0x02000101
 *
 * ���������������С�Ĵ�
 */
#define OS_ERRNO_MEM_PTCREATE_SIZE_TOOSMALL               SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x01)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺�����Ŵ��󣬻���������ڴ��СΪ0�������ڴ�����ķ���û�д���
 *
 * ֵ: 0x02000102
 *
 * ���������������η����ţ������ڴ��С����Ч��
 */
#define OS_ERRNO_MEM_ALLOC_PARA_ERR                         SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x02)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ͷŵĵ�ַΪ��
 *
 * ֵ: 0x02000103
 *
 * ��������������ͷŵ��ڴ���ַ�Ƿ���ȷ��
 */
#define OS_ERRNO_MEM_FREE_ADDR_INVALID                    SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x03)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ͷ��ڴ����ڵķ�����Ч(����δ����������Ŵ��ڷ�����)
 *
 * ֵ: 0x02000104
 *
 * �����������������ŵ���Ч��
 */
#define OS_ERRNO_MEM_FREE_PTNO_INVALID                    SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x04)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺���õ����ɴ����ڴ����������(����ܳ���20)
 *
 * ֵ: 0x02000105
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_PTNUMBER_OVERMANY                      SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x05)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ڴ�����ʱ����Ĵ�С̫��(����Ϊ��ֵ)
 *
 * ֵ: 0x02000106
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_ALLOC_SIZETOOLARGE                     SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x06)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ڴ����õ��ڴ�����Ͷ�����
 *
 * ֵ: 0x02000107
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_ALLSLICE_ZERO                       SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x07)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ڴ����õ��ڴ�����Ͳ��ǰ�������˳�����õ�
 *
 * ֵ: 0x02000108
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_MEMSLICE_NOT_INORDER                SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x08)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺���������������(������Ӧ����1��С����������������+2��������Ϣ�ṹ��ͷ�����ʼ��ַ��Ϊ��
 * �������㷨��ȷ�ԡ�������С��Ϊ0)
 *
 * ֵ: 0x02000109
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_PTCREATE_PARAMETER_INVALID           SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x09)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺�����Ѿ�����
 *
 * ֵ: 0x0200010a
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_PT_ALREADY_CREATED                     SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x0a)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺��̬�����ڴ����ʱ���������ص���
 *
 * ֵ: 0x0200010b
 *
 * �������: ��ͬ����֮������ص�
 */
#define OS_ERRNO_MEM_PT_OVERLAP                             SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x0b)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ڴ��㷨û�д�����
 *
 * ֵ: 0x0200010c
 *
 * �������: �����Ӧ�����Ƿ��
 */
#define OS_ERRNO_MEM_ALGORITHM_UNINSTALLED                  SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x0c)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺���ڴ����δ���������߷����ŷǷ���
 *
 * ֵ: 0x0200010d
 *
 * �������: �������ŵ���Ч��
 */
#define OS_ERRNO_MEM_NO_USE                                 SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x0d)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺���û�ȡ������ʹ������ӿ�ʱ���ýӿڲ������ڿ�ָ�롣
 *
 * ֵ: 0x0200010e
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_INPUT_INVAILD                          SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x0e)
/**
 * @ingroup SRE_mem
 * �ڴ�����룺FSC�ڴ��㷨��֧�ֿ�Cache�ռ�
 *
 * ֵ: 0x0200010f
 *
 * �������: ���ò���Cache�ռ䣬�����POOL�㷨��
 */
#define OS_ERRNO_MEM_PTCREATE_FSC_ARITH_UNCACHABLE          SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x0f)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺�����Ų��Ϸ��������ŷ�Χ[0,OS_MEM_MAX_PT_NUM+OS_MEM_DEFAULT_PTNUM-1],OS_MEM_MAX_PT_NUM���û����õķ�������OS_MEM_DEFAULT_PTNUMϵͳȱʡ�ķ�������
 *
 * ֵ: 0x02000110
 *
 * ���������������η�����
 */
#define OS_ERRNO_MEM_PTNO_INVALID                           SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x10)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺������ڴ���СΪ��
 *
 * ֵ: 0x02000111
 *
 * ������������������ڴ��С����Ч��
 */
#define OS_ERRNO_MEM_ALLOC_SIZE_ZERO                        SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x11)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺�ڴ�����ķ���û�д���
 *
 * ֵ: 0x02000112
 *
 * �������������÷����Ƿ��Ѿ�����
 */
#define OS_ERRNO_MEM_PT_NOT_CREATED                         SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x12)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺���뷽ʽ���Ϸ�
 *
 * ֵ: 0x02000113
 *
 * ���������������ζ��뷽ʽ
 */
#define OS_ERRNO_MEM_ALLOC_ALIGNPOW_INVALID                 SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x13)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺�����㷨�Ƿ���OS֧�ֵķ����㷨:MEM_ARITH_FSC��MEM_ARITH_POOL��MEM_ARITH_MCDYNFSC��MEM_ARITH_MCPOOL��
 *
 * ֵ: 0x02000114
 *
 * �����������������㷨����
 */
#define OS_ERRNO_MEM_PT_ARITH_INVALID                       SRE_ERRNO_OS_ERROR(OS_MID_MEM, 0x14)

/**
 * @ingroup SRE_mem
 * OSȱʡ˽��FSC�ڴ�����룺��˽��FSC�����������ڴ�ʱ���Ҳ��������ڴ棬����ʧ�ܡ�
 *
 * ֵ: 0x02000210
 *
 * �������: ����˽��FSC�ڴ������С
 */
#define OS_ERRNO_FSCMEM_ALLOC_NO_MEMORY                     SRE_ERRNO_OS_ERROR(OS_MID_FSCMEM, 0x10)

/**
 * @ingroup SRE_mem
 * ��̬�ڴ�����룺��̬�ڴ��ͷ�ʱҪ�ͷŵ��ڴ���ͷ���ƻ��������ͷ�
 *
 * ֵ: 0x02000311
 *
 * �������: ��֤���ڴ�д����ʱ����Ҫ����дԽ�硣
 *
 */
#define OS_ERRNO_MEM_FREE_SH_DAMAGED                        SRE_ERRNO_OS_ERROR(OS_MID_DYNMEM, 0x11)

/**
 * @ingroup SRE_mem
 * ��̬�ڴ�����룺��̬(POOL)�ڴ�����ʱ������ڴ�鱻�ƻ�
 *
 * ֵ: 0x02000312
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_DYNALLOC_SH_DAMAGED                    SRE_ERRNO_OS_ERROR(OS_MID_DYNMEM, 0x12)

/**
 * @ingroup SRE_mem
 * �ڴ�����룺˽��POOL�ڴ��ͷ�ʱ�ڴ���Ѿ����ͷ�(�����ڴ�û�б������)
 *
 * ֵ: 0x02000314
 *
 * �������: ����ͷŵ��ڴ��Ƿ������룬�����Ƿ��Ѿ����ͷš�
 */
#define OS_ERRNO_MEM_FREE_POOL_MEM_ALREADY_FREE          SRE_ERRNO_OS_ERROR(OS_MID_DYNMEM, 0x14)

/**
 * @ingroup SRE_mem
 * ��̬�ڴ�����룺��̬POOL�ڴ�����ʱ�ڴ�ռ䲻��
 *
 * ֵ: 0x02000315
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_DYN_ALLOC_MEM_SHORTAGE                 SRE_ERRNO_OS_ERROR(OS_MID_DYNMEM, 0x15)

/**
 * @ingroup SRE_mem
 * ��̬�ڴ�����룺�ڴ��㷨���ͷǷ���OS��֧�ָ��ڴ��㷨
 *
 * ֵ: 0x02000316
 *
 * �������: �鿴������㷨�Ƿ���ȷ
 */
#define OS_ERRNO_MEM_ARITH_INVALID                          SRE_ERRNO_OS_ERROR(OS_MID_DYNMEM, 0x16)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺����̬POOL�ڴ�����ʱ�ڴ�ռ䲻��
 *
 * ֵ: 0x02000417
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_SHARE_ALLOC_MEM_SHORTAGE               SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x17)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺����̬POOL�ڴ�����ʱ������ڴ�鱻�ƻ�
 *
 * ֵ: 0x03000418
 *
 * �������: ��֤��ַ���ڴ��ַ����֤�û�д��������Խ�硣
 */
#define OS_ERRNO_MEM_SHAREALLOC_SH_DAMAGED                  SRE_ERRNO_OS_FATAL(OS_MID_MCMEM, 0x18)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺����̬POOL�ڴ��ͷ�ʱ�ڴ���Ѿ����ͷ�(�����ڴ�û�б������)
 *
 * ֵ: 0x02000419
 *
 * �������: �����ڴ���Ƿ��Ѿ����룬�����Ƿ��Ѿ����ͷš�
 */
#define OS_ERRNO_MEM_FREE_SHARE_MEM_ALREADY_FREE         SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x19)
/**
 * @ingroup SRE_mem
 *
 * �ڴ�����룺��ͬ�˴�����ͬ��ŵĹ������ʱ��������ͬ�����߷�����ʼ��ַ��ͬ���������Ų���ͬ��
 *
 * ֵ: 0x0200041a
 *
 * �������: �������ţ������㷨��������ʼ��ַ��������С�����������ͼ���С��һ����
 */
#define OS_ERRNO_MEM_SHARE_PARA_INCONSISTENT                SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x1a)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺����̬FSC�ڴ�����ĳߴ���ͬBLOCK��������ڴ�ߴ粻ƥ��
 *
 * ֵ: 0x0200041c
 *
 * �������: ��
 */
#define OS_ERRNO_MEM_MCSTA_FSCMEM_UNMATCHED                  SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x1c)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺�ӹ���̬FSC�����������ڴ�ʱ���Ҳ��������ڴ棬����ʧ�ܡ�
 *
 * ֵ: 0x0200041d
 *
 * �������: ������̬FSC�ڴ������С
 */
#define OS_ERRNO_MCSTA_FSCMEM_ALLOC_NO_MEMORY                SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x1d)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺����̬FSC���ڴ治���ͷ�
 *
 * ֵ: 0x0200041f
 *
 * �������: ����̬FSC�㷨��֧���ڴ��ͷŲ����������Ƿ��ͷ��˴�����ڴ�顣
 */
#define OS_ERRNO_MCSTA_FSCMEM_FREE_ISFORBIDEN                SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x1e)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺�ӹ���̬FSC�����������ڴ�ʱ���Ҳ��������ڴ棬����ʧ�ܡ�
 *
 * ֵ: 0x0200041f
 *
 * �������: ������̬FSC�ڴ������С
 */
#define OS_ERRNO_MCDYN_FSCMEM_ALLOC_NO_MEMORY                SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x1f)

/**
 * @ingroup SRE_mem
 * �����ڴ�����룺��ͬimage�ж���OSȱʡʹ�õ�1�ŷ������ò�һ�¡�
 *
 * ֵ: 0x02000420
 *
 * �������: ��ȷ������image�ж���OSȱʡʹ�õ�1�ŷ������õ���ʼ��ַһ�¡�������Сһ�¡�
 */
#define OS_ERRNO_MCSTA_FSCMEM_PT_PARA_INCONSISTENT           SRE_ERRNO_OS_ERROR(OS_MID_MCMEM, 0x20)

/**
 *@ingroup SRE_mem
 *@brief ��RTOSck�Ѵ�����ָ�����������ڴ档
 *
 *@par ����:
 *<li>�ڷ�����ΪucPtNo�ķ����У������СΪuwSize���ڴ档</li>
 *@attention
 *<ul>
 *<li>�����ڴ�ʱ�ķ����ţ�����ʵ�ʴ����ķ�������ʹ�á�</li>
 *<li>���ú�����ע���жϷ��صĵ�ַ�Ƿ�Ϊ���Ա���������ʿ�ָ�롣</li>
 *<li>����FSC�ڴ��㷨�����뵽���ڴ��׵�ַ�ǰ�4�ֽڶ���ģ�����POOL�ڴ��㷨�����뵽���ڴ��׵�ַ�ǰ�16�ֽڶ����</li>
 *<li>����FSC��������ʹ�û������ķ����㹻��һ������������ڴ���СҲ���ܳ���0xFFE0�ֽڡ�</li>
 *<li>����ڴ�����ʧ�ܣ�����ֵΪNULL��������ʧ�ܵ�ԭ�򽫼�¼�ڴ�����ռ��У��û�����ͨ����ϻ��ģ��Dump���ڴ�����������Ϣ��</li>
 *</ul>
 *@param uwMid        [IN] ����#UINT32�������ģ��š�
 *@param ucPtNo       [IN] ����#UINT8�� �����š�
 *@param uwSize       [IN] ����#UINT32������Ĵ�С��
 *
 *@retval NULL  0������ʧ�ܡ�
 *@retval �ڴ��׵�ֵַ��
 *@par ����:
 *sre_mem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C00
 *@see SRE_MemFree
*/
extern VOID *SRE_MemAlloc(UINT32 uwMid, UINT8 ucPtNo, UINT32 uwSize);

/**
 *@ingroup SRE_mem
 *@brief ��RTOSck�Ѵ�����ָ����������ָ����С���ڴ�顣
 *
 *@par ����:
 *<ul>
 *<li>�ڷ�����ΪucPtNo�ķ����У������СΪuwSize���ڴ棬���뷽ʽΪenAlignPow��</li>
 *</ul>
 *@attention
 *<ul>
 *<li>�����ڴ����ʱ�ķ����ţ�����ʵ�ʴ����ķ�������ʹ�á�</li>
 *<li>���ú�����ע���жϷ��صĵ�ַ�Ƿ�Ϊ���Ա���������ʿ�ָ�롣</li>
  *<li>enAlignPow�Ƕ��뷽ʽ������POOL�㷨�����ֻ֧��16�ֽڶ��뷽ʽ��</li>
 *<li>����FSC��������ʹ�û������ķ����㹻��һ������������ڴ���СҲ���ܳ���0xFFE0�ֽڡ�</li>
 *<li>����ڴ�����ʧ�ܣ�����ֵΪNULL��������ʧ�ܵ�ԭ�򽫼�¼�ڴ�����ռ��У��û�����ͨ����ϻ��ģ��Dump���ڴ�����������Ϣ��</li>

 *</ul>
 *@param uwMid        [IN] ����#UINT32�������ģ��š�
 *@param ucPtNo       [IN] ����#UINT8�� �����š�
 *@param uwSize       [IN] ����#UINT32������Ĵ�С��
 *@param enAlignPow   [IN] ����#UINT8����̬���룬��Чȡֵ��ΧΪ#OS_MEM_ALIGN_E��
 *
 *@retval NULL  0������ʧ�ܡ�
 *@retval �ڴ��׵�ֵַ��
 *@par ����:
 *sre_mem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C00
 *@see SRE_MemFree
*/
extern VOID *SRE_MemAllocAlign(UINT32 uwMid, UINT8 ucPtNo, UINT32 uwSize, OS_MEM_ALIGN_E enAlignPow);

/**
 *@ingroup SRE_mem
 *@brief �ͷ�������ڴ档
 *
 *@par ����:
 *�ýӿڸ����ڴ��ĵ�ַpAddr���ҵ����ڴ���������ڴ�������ٸ��ݷ����ź��û�ʹ�õĵ�ַpAddr�ͷŸ��ڴ档
 *@attention
 *<ul>
 *<li>�������ֵ����#SRE_OK�����ڴ治�ᱻ�ͷš�</li>
 *<li>���ƻ����ڴ治�ܱ��ͷš�</li>
 *</ul>
 *@param uwMid       [IN] ����#UINT32��Ҫ�ͷŵ�ģ��š�
 *@param pAddr       [IN] ����#VOID *���ͷŵĵ�ַ��
 *
 *@retval #OS_ERRNO_MEM_FREE_PTNO_INVALID      0x02000104���ͷŵ�ַ�����ڴ�����С�
 *@retval #OS_ERRNO_MEM_FREE_SH_DAMAGED          0x03000311���ڴ��ͷ�ʱҪ�ͷŵ��ڴ���ͷ���ƻ��������ͷš�
 *@retval #OS_ERRNO_MEM_FREE_POOL_MEM_ALREADY_FREE        0x02000314��˽��POOL�ڴ��ͷ�ʱ�ڴ���Ѿ����ͷ�(�����ڴ�û�б������)��
 *@retval #OS_ERRNO_MEM_FREE_SHARE_MEM_ALREADY_FREE       0x02000419������̬POOL�ڴ��ͷ�ʱ�ڴ���Ѿ����ͷ�(�����ڴ�û�б������)��
 *@retval #OS_ERRNO_MCSTA_FSCMEM_FREE_ISFORBIDEN       0x0200041e������̬FSC���ڴ治���ͷš�
 *@retval #SRE_OK                            0x00000000���ڴ��ͷųɹ���
 *@retval #OS_ERRNO_MEM_FREE_ADDR_INVALID      0x02000103���ͷŵ�ַΪ�ա�
 *@par ����:
 *sre_mem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C00
 *@see SRE_MemAlloc
*/
extern UINT32 SRE_MemFree(UINT32 uwMid, VOID *pAddr);

/**
 *@ingroup SRE_mem
 *@brief ��̬����������
 *
 *@par ����:
 *����һ����������ΪpstPara��������ΪucPtNo���ڴ������
 *@attention
 *<ul>
 *<li>�����ڴ����ʱ�ķ����ţ�ȡֵ��Χ��[2,#OS_MEM_MAX_PT_NUM+2)��</li>
 *<li>�û������������ļ�(sre_config.h)������ϵͳ֧�ֵ���������#OS_MEM_MAX_PT_NUM������Ϊ20�����û�����#OS_MEM_MAX_PT_NUMΪ4�������ʹ�õķ�����ӦΪ:2��3��4��5��</li>
 *<li>����˽��FSC�㷨�������㷨������ͨ���ڴ�ü����ؾ����Ƿ񴴽��� </li>
 *<li>os��֧�ֺ˼�ͨ�ŵ�ǰ���£���ռ��2������(������ϵͳ֧�ֵ�����������)�������Ŵ�0��1��0��Ϊϵͳʹ�õ�˽��FSC��
      1��Ϊϵͳʹ�õĹ���̬FSC</li>
 *<li>��tensilicaƽ̨�£������ڴ�ʱ���û����õ��ڴ��ַ�������ڴ��Ƿ��cache��</li>
 *<li>����Block�ڴ��㷨��֧��</li>
 *</ul>
 *@param pstPara  [IN] ����#MEM_CREATEPT_PARA *����������������Ϣ��
 *@param ucPtNo   [IN] ����#UINT8�� �����š�
 *
 *@retval #OS_ERRNO_MEM_PTCREATE_PARAMETER_INVALID 0x02000109�����������������
 *@retval #OS_ERRNO_MEM_PT_ALREADY_CREATED           0x0200010a�������Ѿ�������
 *@retval #OS_ERRNO_MEM_PT_OVERLAP                   0x0200010b����̬�����ڴ����ʱ���������ص���
 *@retval #OS_ERRNO_MEM_SHARE_PARA_INCONSISTENT      0x0200041a����������������ò�ͳһ����ͬ�˴�����ͬ��ŵĹ������ʱ����Ӧ��ͬ��
 *@retval #OS_ERRNO_MEM_ALGORITHM_UNINSTALLED        0x0200010c���ڴ��㷨û�д�������������Ӧ����û�д򿪡�
 *@retval #OS_ERRNO_MEM_PTCREATE_SIZE_TOOSMALL     0x02000101�����������ķ�����С̫С��
 *@retval #OS_ERRNO_FSCMEM_ALLOC_NO_MEMORY           0x02000210����������ʱ����ȱʡ˽��FSC�ڴ����������������ƿ�ʧ�ܡ�
 *@retval #OS_ERRNO_MCSTA_FSCMEM_ALLOC_NO_MEMORY     0x0200041d����������ʱ����ȱʡ����̬FSC�ڴ����������������ƿ�ʧ�ܡ�
 *@retval #SRE_OK ���������ɹ���
 *@par ����:
 *sre_mem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since UniDSP V100R001C01
 *@see SRE_MemFree | SRE_MemAlloc
*/
extern  UINT32 SRE_MemPtCreate(MEM_CREATEPT_PARA *pstPara, UINT8 ucPtNo);

extern UINT32 SRE_MemPtDelete(UINT8 ucPtNo);

/**
 *@ingroup SRE_mem
 *@brief ��ȡָ���ڴ�������ڴ�ռ���ʡ�
 *
 *@par ����:
 *��ȡ������ΪucPtNo���ڴ������ռ���ʡ�
 *@attention
 *<ul>
 *<li>����pool�㷨����ʹ�õ��ڴ�ռ��������֮���ͷŵ��ڴ�ռ䣬�����ڴ�����֮�󣬼�ʹ�ͷţ����ڴ�������Ѿ���ָ�������������ɷ��䡣</li>
 *<li>����fsc�㷨����ʹ�õ��ڴ�ռ䲻��������֮���ͷŵ��ڴ�ռ䣬�ͷŵĿռ�������±����䡣</li>
 *</ul>
 *@param ucPtNo         [IN] ����#UINT8�� �����š�
 *
 *@retval �ڴ����ռ����         �����ɹ���
 *@retval 0                     ����ʧ�ܻ�����Ų����ڻ��û���ѯ�ķ���ռ����Ϊ�㡣
 *@par ����:
 *sre_mem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since UniDSP V200R003C03
 *@see SRE_MemPtCreate
*/
extern UINT32 SRE_MemUsageGet(UINT8 ucPtNo);

/**
 *@ingroup SRE_mem
 *@brief ��ѯ�ڴ��ΪucPtNo���ڴ�����Ļ�����Ϣ������Ϣ���ظ�pstMemPtState��ָ��Ľṹ�塣
 *
 *@par ����:
 *��������ţ���ѯ�÷������ܴ�С����ʷʹ�÷�ֵ��ʣ���ڴ�Ĵ�С
 *@attention
 *<ul>
 *<li>֧��pool�㷨��fsc�㷨</li>
 *<li>�ڴ�����ܴ�С��ָ�û�����ķ�����С��÷������ƿ�Ĵ�С���ܺ�</li>
 *<li>��ʷʹ�÷�ֵ��ָ������ʷʹ�������ڴ�ռ�Ĵ�С��
 ����pool�㷨��˵�������ڴ�����֮�󣬼�ʹ�ͷţ����ڴ�������Ѿ���ָ�������������ɷ��䡣���Զ���pool�㷨����ʹ�ڴ�����֮���ͷţ����ռ�Ҳ������ʹ�õģ�
 ����fsc�㷨���������ֱ��ͷŵ��ڴ�������·��䣬����δ��ʹ�á�</li>
 *<li>����ʣ���ڴ��С������pool�㷨ָδ����ʽ���Ŀ����ڴ�Ĵ�С�����������Ѿ�����Ȼ���ͷŵĿ����ڴ�Ĵ�С������fsc�㷨����������Ȼ���ͷŵĿ����ڴ�Ĵ�С</li>
 *</ul>
 *@param ucPtNo         [IN] ����#UINT8�� �����š�
 *@param pstMemPtState  [OUT] ����#OS_MEM_PT_STATE *�������ڴ������ʹ��״̬��Ϣ��
 *
 *@retval #SRE_OK                   0x00000000�������ɹ���
 *@retval #OS_ERRNO_MEM_INPUT_INVAILD   0x0200010e���ṹ��Ϊ�ա�
 *@retval #OS_ERRNO_MEM_NO_USE          0x0200010d������δ����������ŷǷ���
 *@retval #OS_ERRNO_MEM_ARITH_INVALID   0x02000316���ڴ��㷨���ͷǷ���OS��֧�ָ��ڴ��㷨��
 *@par ����:
 *sre_mem.h���ýӿ��������ڵ�ͷ�ļ���
 *@since UniDSP V200R003C03
 *@see SRE_MemPtCreate
*/
extern UINT32 SRE_MemPtStateGet(UINT8 ucPtNo, OS_MEM_PT_STATE *pstMemPtState);
/**
 *��ѯpPtAddr�ĵ�ַ�Ƿ����ڴ��ΪucPtNo���ڴ������
 */
extern OS_SEC_TEXT UINT32 SRE_PtMemCheck(UINT8 ucPtNo, VOID *pPtAddr);

extern OS_SEC_TEXT UINT32 SRE_MemPagePtCreate(VOID *pPtAddr, UINT32 uwPtSize);
extern OS_SEC_TEXT VOID *SRE_MemPagePtAlloc(VOID);
extern OS_SEC_TEXT UINT32 SRE_MemPagePtFree(VOID * ptr);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SRE_MEM_H */
/**
 * History:
 * 2009-1-20 a149045: Create this file.
 * 2009-9-3  sdk: change this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

