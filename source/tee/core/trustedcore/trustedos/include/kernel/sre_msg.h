/**
 * @file sre_msg.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ��������Ϣģ��Ķ���ͷ�ļ��� \n
 */

/** @defgroup SRE_msg ��Ϣ
   *@ingroup SRE_comm
*/

#ifndef _SRE_MSG_H
#define _SRE_MSG_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/**
 * @ingroup SRE_msg
 * ����Ϣ�巢����Ϣʱ����һ�����������ֵ��
 */
#define OS_MSG_NO_PAYLOAD     0

/**
 * @ingroup SRE_msg
 * ʹ���ڽ�����Ϣ�ӿڣ���Ϣ�ȴ�ʱ���趨����ʾ���õȴ���
 */
#define OS_WAIT_FOREVER     0xFFFFFFFF

/**
 * @ingroup SRE_msg
 * ʹ���ڽ�����Ϣ�ӿڣ���ʾ��Ϣ�ķ�����ΪӲ�жϡ�
 */
#define OS_MSG_HWI_SENDER   0x000000FF

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣ��ʧ�ܣ�����ucAlignPow��ηǷ��������Ŀ��PID�Ƿ��������ڴ����þ����߱�����Ϣ�ڵ���Դ����
 *
 * ֵ: 0x00000000
 *
 * �������:���������Ϣ��ӿ�����Ƿ�Ϸ���ָ���ķ����ڴ��Ƿ����ȡ�
 */
#define OS_MSG_CREATE_ERR   0

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺��Ϣ���պ����н�����Ϣʧ�ܣ�����ԭ������Ӳ�ж��н�����Ϣ��������Ϣ�����ж��н�����Ϣ�������н�����Ϣ��ʱ������������������ʱ���г�ʱ�ȴ���
 *
 * ֵ: 0xffffffff
 *
 * �������:��������Ϣ���߳����ͣ�Ӳ�ж��в����������Ϣ��ȷ���������г�ʱ�ȴ���Ϣʱ��ϵͳ������������״̬��
 */
#define OS_MSG_RECEIVE_ERR      0xFFFFFFFF

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺��ѯָ�������ָ����Ϣ����ʧ�ܡ�
 *
 * ֵ: 0xffffffff
 *
 * �������: ֻ�ܲ�ѯ�����Ѵ���������ϵͳ��idle����֧�ֲ�ѯ��ֻ�ܲ鿴�����ָ�����У��Ҳ��ܴ��������кš�
 */
#define OS_MSGQ_NUM_GET_ERR     0xFFFFFFFF

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺��Ϣ���Ȼ�ȡ����(SRE_MsgLength)�Ĵ��󷵻�ֵ,��Ϣ����Ƿ���
 *
 * ֵ: 0x00000000
 *
 * �������: ��鴫�����Ϣ��ָ���Ƿ���ȷ��
 */
#define OS_MSG_LENGTH_ERR       0x00000000

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺�������Ϣ�����Ƿ����������ɾ������ָ��ΪNULL����������Ϣ�����ڱ�ʹ��
 *
 * ֵ: 0x02000d00
 *
 * �������: ������Ϣ���Ƿ�ֻ����һ�λ����Ƿ�����ʹ���У���鴫�����Ϣ��ָ���Ƿ���ȷ��
 */
#define OS_ERRNO_MSG_HANDLE_INVALID         SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x00)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣʱ�߳�PID�Ƿ����̷߳Ƿ���δ��������ɾ����Ŀ���߳�Ϊϵͳ��idle���жϡ�idle����Ŀ��������Ϣ���зǷ�����
 * ע��:��������Ϣ�Ӻ�ϻ���е�������Ϊ�˼�ͨ��ʱ��Ŀ�ĺ˲��񵽵Ĵ�����Ϣ��
 *
 * ֵ: 0x02000d01
 *
 * �������: ���ӿڴ���Ĳ����Ƿ���ȷ��
 */
#define OS_ERRNO_MSG_PID_INVALID            SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x01)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣģ���ʼ�������У�����˽�о�̬�ڴ�ʧ��
 *
 * ֵ: 0x02000d02
 *
 * �������: Ϊ˽�о�̬�ڴ����ø����ڴ�ռ�
 *
 */
#define OS_ERRNO_MSG_NO_MEMORY              SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x02)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺ע�������Ϣʧ�ܣ�������Ϣ�ڵ�ĸ���������Ϊ��0
 *
 * ֵ: 0x02000d03
 *
 * �������: ��ʹ�ú�����Ϣͨ�Ź��ܣ������ô���0�ĺ�����Ϣ�ڵ���
 *
 */
#define OS_ERRNO_MSG_REG_ERROR              SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x03)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣʱ�޿�����Ϣ�ڵ㣬������ϵͳ֧�ֵ������Ϣ�ڵ�����
 * ע��:SRE_MsgQSend��SRE_MsgQRecv���ʹ�ã���ȷ��ʹ�ú����Ϣ�ڵ㱻�ͷ�
 *
 * ֵ: 0x02000d04
 *
 * �������: �Ų鱾���Ƿ��ж���Ϣ(��82ƽ̨)��������Ϣ����Ϣδ���գ����ѽ��գ��������ϵͳ֧�ֵ������Ϣ�ڵ���
 *
 */
#define OS_ERRNO_MSG_NO_MSGNODE             SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x04)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺���ͺ˼���Ϣʧ��
 *
 * ֵ: 0x02000d05
 *
 * �������: �鿴Ŀ�ĺ�ID�Ƿ���ȷ
 */
#define OS_ERRNO_MSG_MC_BUSY                SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x05)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣ���кŷǷ������ڸ������̵߳������Ϣ���кš�
 *
 * ֵ: 0x02000d06
 *
 * �������:ȷ�ϴ������Ϣ���к��Ƿ�Ϸ���
 *
 */
#define OS_ERRNO_MSG_QID_INVALID            SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x06)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺�����̷߳Ƿ�����Ӳ�ж��н���
 *
 * ֵ: 0x02000d07
 *
 * �������: ��ȷ����Ҫ��Ӳ�ж��߳��н�����Ϣ��
 *
 */
#define OS_ERRNO_MSG_THREAD_INVALID         SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x07)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺��Ϣ���ճ�ʱ�����û����õĳ�ʱʱ����û�н��յ���Ϣ��
 *
 * ֵ: 0x02000d08
 *
 * �������: ȷ����ʱʱ����ָ������������Ϣ���͹���������ʱʱ�䲻������������ʱ��
 *
 */
#define OS_ERRNO_MSG_RECEIVE_TIMEOUT        SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x08)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺���ͺ˼���Ϣʱ��Ŀ��PID�Ƿ���
 *
 * ֵ: 0x02000d09
 *
 * �������: ȷ����Ϣ���ͽӿڵ�Ŀ��PID�Ƿ���ȷ��
 *
 */
#define OS_ERRNO_MCMSG_PID_INVALID          SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x09)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺���ն���������Ϣ��
 *
 * ֵ: 0x02000d0a
 *
 * �������: �����Ƿ������Ϣ�����з�������Ϣ��
 *
 */
#define OS_ERRNO_MSG_QUEUE_EMPTY            SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x0a)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺���ͺ˼���Ϣʱ���˼���Ϣģ��δ�򿪡�
 *
 * ֵ: 0x02000d0b
 *
 * �������: ���鴫�����Ϣ����ֵ�Ƿ���Ч��
 *
 */
#define OS_ERRNO_MCMSG_HOOK_NULL             SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x0b)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺�ͷ���Ϣ���ڴ�ʱ������ľ����ַδ32�ֽڶ��롣
 *
 * ֵ: 0x02000d0c
 *
 * �������: ���鴫�����Ϣ����ֵ�Ƿ���Ч��
 *
 */
#define OS_ERRNO_MSG_HANDLE_ADDR_UNALIGN          SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x0c)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺����Ϣδ���մ�����ǿ���ͷ�
 *
 * ֵ: 0x02000d0d
 *
 * �������: ��ȷ������Ϣ�����մ�������ͷţ���������ڴ�й©
 *
 */
#define OS_ERRNO_MSG_SMSG_CAN_NOT_DELETE          SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x0d)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣ�壬�������Ϣ���ڴ��С����Ϊ��
 *
 * ֵ: 0x02000d0e
 *
 * �������: ��ȷ���������Ϣ���ڴ��С�Ƿ���Ч��
 *
 */
#define OS_ERRNO_MSG_MEM_SIZE_ZERO          SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x0e)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺������Ϣ�壬��ָ�����������벻���㹻��С����Ϣ���ڴ�
 *
 * ֵ: 0x02000d0f
 *
 * �������: ��鴫��ķ������Ƿ���Ч������Ч���С�������Ϣ���ڴ��������ָ���ķ����ڴ��С��
 *
 */
#define OS_ERRNO_MSG_MEM_ALLOC_FAIL         SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x0f)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺����Ϣδ���մ�����ǿ���ͷ���Ϣ���ڴ�
 *
 * ֵ: 0x02000d10
 *
 * �������: ��ȷ������Ϣ�����մ�������ͷţ���������ڴ�й©
 *
 */
#define OS_ERRNO_MSG_LMSG_CAN_NOT_DELETE          SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x10)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺��ȡ���˶���Ϣ�峤��ֵʧ��
 *
 * ֵ: 0x02000d11
 *
 * �������: ���ͷŸö���Ϣǰ������������Ϣ���ֵ�Ի�ȡ��ȷ�Ķ���Ϣ�峤��
 *
 */
#define OS_ERRNO_MSG_GET_SMSG_LENGTH_FAIL        SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x11)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺��ȡ���˳���Ϣ�峤��ֵʧ��
 *
 * ֵ: 0x02000d12
 *
 * �������: ���ͷŸó���Ϣǰ������������Ϣ���ֵ�Ի�ȡ��ȷ�ĳ���Ϣ�峤��
 *
 */
#define OS_ERRNO_MSG_GET_LMSG_LENGTH_FAIL       SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x12)

/**
 * @ingroup SRE_msg
 * ��Ϣ�����룺82ƽ̨��Ϣ����ʱ�����յ���Ϣ����Ϊ����Ϣ��������Ϣħ����У�����
 *
 * ֵ: 0x02000d13
 *
 * �������: ���82ƽ̨ʹ��SRE_QportSend����OS��Ϣʱ����Ϣ�Ƿ���SRE_QportMsgCreate����������
 *
 */
#define OS_ERRNO_MSG_LONGMSG_INVALID            SRE_ERRNO_OS_ERROR(OS_MID_MSG, 0x13)


/**
 * @ingroup SRE_msg
 * ��Ϣ�������Ͷ��塣
 */
typedef UINT32 MSG_HANDLE_T;

/**
 * @ingroup SRE_msg
 * ��Ϣ�����߻�����ߵ��߳�PID���Ͷ��塣
 */
typedef UINT32 MSG_PID_T;

/**
 * @ingroup SRE_msg
 * ��Ϣģ��������Ϣ�Ľṹ�嶨�塣
 */
typedef struct tagMsgModInfo {
	UINT32 uwMaxMsgNum;                  /*���õ������Ϣ������  */
} MSG_MOD_INFO_S;

/**
 *@ingroup SRE_msg
 *@brief ��Ϣ������뺯����
 *
 *@par ����:
 *����ָ����С����Ϣ���ڴ档
 *@attention
 *@li �ýӿ�����Ϣ���ͷŽӿ�#SRE_MsgFree���ʹ�ã���Ϣ���ͺ���Ҫ�ͷš�
 *@li �����Ϣֻ��Ҫ������ϢID�ţ����ʡ�����롢�ͷ���Ϣ�岽�衣
 *@li ��Ϣͨ�����û�ָ���ڴ������uwPtNo��Ϊ������Ϣ����ڴ������ʹ�á�
 *@li ������Ϣͨ�ſ�����˽���ڴ������Ҳ�����ǹ����ڴ������������ʹ��˽���ڴ������Ч�ʣ��˼�ı����ǹ����ڴ����
 *@li ˵������Ϣ���ٶ���Ϊ32�ֽڶ��룬����û�����Ķ��뷽ʽС��32�ֽڶ��������32�ֽڣ��ڲ��ڴ��������32�ֽڶ������롣
 *�����˽��pool�㷨ʵ�����ģ��û������ֽ���������32���룩+ �ڴ��������32��64�ֽڣ�mcpool�㷨�����cache����ʱ����64�ֽڣ�����Ϊ32�ֽڣ� + ʵ���û�size��
 *�����˽��fsc�㷨����ģ��û������ֽ���������32���룩+ �ڴ��������44�ֽ� + ʵ���û�size��
 *
 *@param uwPtNo     [IN] ����#UINT32�������š�
 *@param usSize     [IN] ����#UINT16����Ҫ�������Ϣ���С����λByte����
 *@param uwAlignPow [IN] ����#UINT32����Ϣ���ڴ�Ķ��뷽ʽ��ȡֵ��ΧΪ[0, 8]��
 *����2���ݴη���ʽ��ʾ��������Ҫ64���룬��ò���ӦΪ6��2��6�η�Ϊ64����
 *�����������ucAlignPowС��5ʱ����32�ֽڶ��롣
 *
 *@retval #OS_MSG_CREATE_ERR 0x00000000������ʧ�ܣ�ʧ��ԭ��ܿ����Ƕ��뷽ʽ����8���������ڷ���δ������
 *@retval ��0ֵ����ʾ���뵽����Ϣ������
 *
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgFree
*/
extern MSG_HANDLE_T SRE_MsgAlloc(UINT32 uwPtNo, UINT16 usSize, UINT32 uwAlignPow);

/**
 *@ingroup SRE_msg
 *@brief ��Ϣ�ͷź���
 *
 *@par ����:
 *��Ϣ���ͷź�����������Ϣ��ɹ����ղ�ʹ�����֮��
 *@attention
 *@li �˺�����Ҫ�������Ϣ���#SRE_MsgQRecv����#SRE_MsgReceive�ɶ�ʹ�ã��Ƚ���Ȼ���ͷš�
 *@li ��Ϣ�Ľ��պͷ�����׼ȷ��ԣ��������ܱ�֤��Ϣ��������ͷš�
 *@li ����Ϣ�����Ϣ������Ҫ���ô˺����ͷ���Ϣ�塣
 *@li ����ͨ����������ֵ�Ƿ�Ϊ#SRE_OK���ж���Ϣ�Ƿ��ͷųɹ���
 *@li �ú��������ȷ�����û���֤��OSֻ���Ƿ�Ϊ0��ַ�����жϡ�
 *
 *@param uwMsgHandle [IN] ����Ϊ#MSG_HANDLE_T����Ϣ������
 *
 *@retval #SRE_OK                                       0x00000000�������ɹ���
 *@retval #OS_ERRNO_MSG_HANDLE_INVALID                  0x02000d00����Ϣ����Ϊ�Ƿ�ֵ������Ϣ�����ڱ�ʹ�á�
 *@retval #OS_ERRNO_MSG_SMSG_CAN_NOT_DELETE             0x02000d0d������Ϣ���ܱ�ɾ��������δ�����մ������δ��ʹ�á�
 *@retval #OS_ERRNO_MSG_HANDLE_ADDR_UNALIGN             0x02000d0c���������Ϣ������ֵַδ32�ֽڶ��롣
 *@retval #OS_ERRNO_MSG_LMSG_CAN_NOT_DELETE             0x02000d10������Ϣ���ܱ�ɾ��������δ�����մ������δ��ʹ�á�
 *@retval #OS_ERRNO_MEM_FREE_PTNO_INVALID               0x02000104���ͷŵ�ַ�����ڴ�����С�
 *@retval #OS_ERRNO_MEM_FREE_SH_DAMAGED                 0x02000311���ڴ��ͷ�ʱҪ�ͷŵ��ڴ���ͷ���ƻ��������ͷš�
 *@retval #OS_ERRNO_MEM_FREE_POOL_MEM_ALREADY_FREE      0x02000314��˽��POOL�ڴ��ͷ�ʱ�ڴ���Ѿ����ͷ�(�����ڴ�û�б������)��
 *@retval #OS_ERRNO_MEM_FREE_SHARE_MEM_ALREADY_FREE     0x02000419������̬POOL�ڴ��ͷ�ʱ�ڴ���Ѿ����ͷ�(�����ڴ�û�б������)��
 *@retval #OS_ERRNO_MCSTA_FSCMEM_FREE_ISFORBIDEN        0x0200041e������̬FSC���ڴ治���ͷš�
 *@retval #OS_ERRNO_MEM_FREE_ADDR_INVALID               0x02000103���ͷŵ�ַΪ�ա�
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgAlloc
*/
extern UINT32 SRE_MsgDelete(MSG_HANDLE_T uwMsgHandle);

/**
 *@ingroup SRE_msg
 *@brief �ͷŴ����ɹ�����Ϣ�塣
 *
 *@par ����:

 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgDelete
*/
#define SRE_MsgFree(pHandle)       (UINT32)SRE_MsgDelete(pHandle)


/**
 *@ingroup SRE_msg
 *@brief �ͷŽ��ճɹ�����Ϣ��
 *
 *@par ����:

 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgDelete
*/
#define SRE_MsgRelease(pHandle)     (UINT32)SRE_MsgDelete(pHandle)

/**
 *@ingroup SRE_msg
 *@brief ��ָ����������жϵ�ָ�����з���Ϣ��
 *
 *@par ����:
 *������Ϣ��ָ���������ָ�����У�֧�ֺ��ںͺ˼���Ϣ�ķ��͡�
 *������Ϣ�����жϵ�0���У�֧�ֺ��ںͺ˼���Ϣ�ķ��͡�
 *��������Ϊ���ոö��е���Ϣ����������ỽ�Ѹ�����
 *@attention
 *@li Ŀ���߳�ֻ������������жϣ�������Ӳ�жϡ�Ŀ���߳�������ϵͳռ�õ��̣߳�����IDLE����
 *@li ���Ŀ����������Ϊ���ոö��е���Ϣ����������ỽ�Ѹ�������Ŀ���������ȼ����������ں����������е�������ᷢ�������л���
 *@li ���Ŀ���߳������жϣ���ֱ�Ӽ�������жϣ�ʹ�䴦�ھ���̬��
 *@li ��Ϣ���Բ�������Ϣ�壬�����Ͳ�����Ϣ����Ϣ����MSG_HANDLE_T���ΪOS_MSG_NO_PAYLOAD��
 *@li ��������Ϣ�巢�ͣ�SD6181/SD6108/SD6182ƽֻ̨�ܴ���4�ֽ����ݡ�
 *@li ����Ҫ������Ϣ�壬����Ϣ��Ӧ��Դ��#SRE_MsgAlloc��������Ϣ���#SRE_MsgRecv���յ�������Ϣ�塣
 *@li ���д���Ϣ�巢�ͣ�SD6182���г�����Ϣ֮�֣��û��������ݲ�����8�ֽڣ�OS�ڲ��߶���Ϣ���������ͨ��Ч�ʡ���֮��OS�ڲ��߳���Ϣ���̣���SD6181/SD6108���ơ�
 *@li �˼䷢��ʱ���û���Ҫ���б�֤���ն˵ĺ���ʵ���ڣ���������ڴ�й©��

 *@param uwMsgHandle    [IN] ����#MSG_HANDLE_T����Ϣ��������Դ����Ϣ�崴���ɹ�����ճɹ������ֵ��
 *@param uwMsgID        [IN] ����#UINT32����ϢID�����Ա�ʶ����Ϣ��Ψһ�ԡ�
 *@param uwDstPID       [IN] ����#MSG_PID_T����ϢĿ���߳�PID�������Ǻ��ڻ�˼����������жϵ�PID��
 *@param ucDstQID       [IN] ����#UINT8����Ϣ���к�(���͸����жϣ��˲�����Ч��������ֵӦС��8�������񱣳�һ��)������������Ϣ��������8���������Ķ��к���7��
 *
 *@retval #SRE_OK                       0x00000000�������ɹ���
 *@retval #OS_ERRNO_MSG_HANDLE_INVALID  0x02000d00����Ϣ����Ϊ�Ƿ�ֵ�����ѱ�ɾ����
 *@retval #OS_ERRNO_MSG_PID_INVALID     0x02000d01���߳�PIDΪ�Ƿ�ֵ�����߳��ѱ�ɾ����
 *@retval #OS_ERRNO_MSG_NO_MSGNODE      0x02000d04���޿�����Ϣ�ڵ㡣
 *@retval #OS_ERRNO_MSG_MC_BUSY         0x02000d05���˼�����������������ͺ˼���Ϣʧ�ܡ�
 *@retval #OS_ERRNO_MSG_QID_INVALID     0x02000d06����������Ϣʱָ���Ľ�����Ϣ�������Qid�Ƿ���
 *@retval #OS_ERRNO_IPC_SHQ_FULL        0x02001486��SD6181��SD6108ƽ̨�£����ͺ˼��жϣ��������������
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgQRecv
*/
extern UINT32 SRE_MsgQSend(MSG_HANDLE_T uwMsgHandle, UINT32 uwMsgID, MSG_PID_T uwDstPID, UINT8 ucDstQID);

/**
 *@ingroup SRE_msg
 *@brief ����������жϵ���Ϣ�����л�ȡ��Ϣ��
 *
 *@par ����:
 *�����������ָ������Ϣ�����л�ȡ��һ����Ϣ����Ϣ�Ľ��ղ�ȡ�ȵ����յ�˳����С�
 *�����жϵ�0����Ϣ�����л�ȡ��һ����Ϣ����Ϣ�Ľ��ղ�ȡ�ȵ����յ�˳����С�
 *��֧��SD6108/SD6181/SD6182ƽ̨��
 *@attention
 *@li ���жϲ�֧�ֶ���Ϣ���У��������ж��е��ô˽ӿ�ʱ��ucRecvQID������Ч��Ĭ�ϴ�0����Ϣ���н�����Ϣ��
 *@li ���жϲ�֧�ֳ�ʱ�ȴ����������ж��е��ô˽ӿ�ʱ��uwTimeout������Ч��������ʧ����ֱ�ӷ��ز����ȴ���
 *@li ����Ϣ����Ϣ�����ճɹ����ʹ��*puwMsgHandle�����ȡ��Ϣ���׵�ַ��
 *@li ����Ϣ�����Ϣ�ķ����������������Ϣ�ڵ���Ϊ�н飬���͹��̻��ȡһ����Ϣ�ڵ㣬���չ����ͷ������Ϣ�ڵ㡣ϵͳ����ʹ�õ���Ϣ�ڵ�����������������к��ڡ��˼���Ϣ�ڵ������ơ�
 *
 *@param puwMsgHandle   [OUT] ����#MSG_HANDLE_T *����������Ϣ����Ϣ��puwMsgHandleΪ���ص���Ϣ���׵�ַ��������Ҫ�����������дNULL���ɡ�
 *@param puwMsgID       [OUT] ����#UINT32 *�����յ�����ϢID��������Ҫ�����������дNULL���ɡ�
 *@param puwSenderPID   [OUT] ����#MSG_PID_T *����Ϣ�����ߵ�PID��Ϊ#OS_MSG_HWI_SENDERʱ��ʾ������ΪӲ�жϡ�������Ҫ�����������дNULL���ɡ�
 *@param ucRecvQID      [IN]  ����#UINT8�������ָ��������Ϣ���кţ����ж��߳̽���ʱ���˲�����Ч��
 *@param uwTimeout      [IN]  ����#UINT32�����������Ϣ�ĵȴ�ʱ�����ƣ���λΪtick��ȡֵ��ΧΪ[0, 0xffffffff]�����жϽ�����Ϣʱ���˲�����Ч��
 *
 *@retval #SRE_OK                       0x00000000����ʾ������Ϣ�ɹ���
 *@retval #OS_ERRNO_MSG_QID_INVALID     0x02000d09���������Ϣ����ID�Ƿ���
 *@retval #OS_ERRNO_MSG_THREAD_INVALID  0x02000d0a�������̷߳Ƿ�����֧����Ӳ�ж��н�����Ϣ��
 *@retval #OS_ERRNO_MSG_RECEIVE_TIMEOUT 0x02000d0b����Ϣ���ճ�ʱ�����û����õĳ�ʱʱ����û�н��յ���Ϣ��
 *@retval #OS_ERRNO_MSG_QUEUE_EMPTY     0x02000d0e������Ϣ����������Ϣ��
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgQSend
*/
extern UINT32 SRE_MsgQRecv(MSG_HANDLE_T *puwMsgHandle, UINT32 *puwMsgID, MSG_PID_T *puwSenderPID, UINT8 ucRecvQID, UINT32 uwTimeout);

/**
 *@ingroup SRE_msg
 *@brief ��ȡ���������ָ����Ϣ���е���Ϣ����
 *
 *@par ����:
 *��ѯ���˵������ָ�����У����ظö��е���Ϣ����
 *��֧��SD6108/SD6181/SD6182ƽ̨��
 *@attention
 *@li ֻ���ǲ�ѯ�����ָ��������Ϣ�������������жϻ�Ӳ�жϣ��Ҳ�����IDLE����
 *@li ֻ���ǲ�ѯ���˵������ָ����Ϣ������Ϣ�������ܲ�ѯ�����˵�������Ϣ���е���Ϣ����
 *
 *@param uwPID [IN] ����#UINT32��ֻ���Ǳ����ڵ������PID��
 *@param ucQID [IN] ����#UINT8��ָ���������Ϣ���кš�

 *@retval  #OS_MSGQ_NUM_GET_ERR  0xFFFFFFFF�������PID�Ƿ���IDLE�����PID����ָ���������Ϣ����ID�Ƿ���
 *@retval  ����               ָ�������ָ����Ϣ���е���Ϣ��Ŀ��
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgQRecv|SRE_MsgQSend
*/
extern UINT32 SRE_MsgQNumGet(UINT32 uwPID, UINT8 ucQID);

/**
 *@ingroup SRE_msg
 *@brief ��Ϣ���ͺ�����
 *
 *@par ����:
 *Ĭ�Ϸ��͵�Ŀ�Ķ��̵߳�0����Ϣ���У���������������SRE_MsgQSend
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgReceive
*/
#define SRE_MsgSend(uwMsgHandle, uwMsgID, uwDstPID) \
        SRE_MsgQSend(uwMsgHandle, uwMsgID, uwDstPID, 0)


/**
 *@ingroup SRE_msg
 *@brief ��Ϣ��ȡ������
 *
 *@par ����:
 *Ĭ���̵߳�0����Ϣ���н�����Ϣ��������Ϣ����Ϣ���ճɹ����򷵻���Ϣ����uwMsgHandle����������������SRE_MsgQRecv��
 *@attention
 *@li �����յ�������Ϣ����Ϣ���򷵻صľ��ֵuwMsgHandleΪ0�������յ��Ǵ���Ϣ����Ϣ�����صľ��ֵuwMsgHandle����Ϣ���׵�ַ��
 *
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgSend
*/
extern MSG_HANDLE_T SRE_MsgReceive(UINT32 uwTimeout,
				   UINT32 *puwMsgID,
				   MSG_PID_T *puwSenderPID);

extern UINT32 SRE_MsgSnd(UINT32 uwMsgID, MSG_PID_T uwDstPID, void *msgp, UINT16 size);
extern UINT32 SRE_MsgRcv(UINT32 uwTimeout, UINT32 *puwMsgID, void *msgp, UINT16 size, MSG_PID_T *puwSenderPID);


/**
 *@ingroup SRE_msg
 *@brief ��ȡ��Ϣ�峤�ȡ�
 *
 *@par ����:
 *��ȡ��Ϣ���ݳ��ȣ���λ:�ֽڡ�
 *@attention �û��豣֤������Ϣ�������Ч��
 *
 *@param uwMsgHandle [IN] ����#MSG_HANDLE_T����Ϣ��������Դ����Ϣ�崴���ɹ�����ճɹ������ֵ��
 *
 *@retval  #OS_MSG_LENGTH_ERR  0x00000000����Ϣ�����Ƿ����ѱ�ɾ������Ϣ����Ϣ�塣
 *@retval  ��0                 ��Ϣ�峤�ȡ�
 *@par ����:
 *@li SRE_msg.h���ýӿ��������ڵ�ͷ�ļ���
 *@since RTOSck V100R001C01
 *@see SRE_MsgAlloc
*/
extern UINT16 SRE_MsgLength(MSG_HANDLE_T uwMsgHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_MSG_H */

/**
 * History:
 * 2009-10-30 sdk: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */


