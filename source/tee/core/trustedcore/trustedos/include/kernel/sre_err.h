/**
 * @file sre_err.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ������������ͷ�ļ��� \n
 */

/**
*@defgroup  SRE_err  ������
*@ingroup SRE_inspect
*/

/**
*@defgroup  SRE_cda  CDA
*@ingroup SRE_inspect
*/
/**
*@defgroup  SRE_cda  CDA
*@ingroup SRE_inspect
*/

#ifndef _SRE_ERR_H
#define _SRE_ERR_H
#include "sre_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/**
 * @ingroup SRE_cda
 * CDA��¼��Ϣ������ջ������ڴ��������ʼ��ַ�ʹ�С��
 */
typedef struct tagPtInfo {
	UINT32 uwAddr;                  /**< �ڴ������ʼ��ַ   */
	UINT32 uwSize;                  /**< �ڴ�����Ĵ�С     */
} CDA_PT_INFO_S;

/**
*@ingroup SRE_err
*
*@brief �������Ӻ�����
*
*@par ����:
*������Ĺ��Ӻ������û��Զ���Ĵ�����������Ҫ���ϸù��Ӻ����Ĳ���Ҫ��
*
*@attention ��
*
*@param pcFileName [IN] ����#CHAR *�������ļ�����
*@param uwLineNo [IN] ����#UINT32 ��������кš�
*@param uwErrorNo [IN] ����#UINT32���û�����Ĵ����롣
*@param uwParaLen [IN] ����#UINT32�����pPara�ĳ��ȡ�
*@param pPara [IN] ����#VOID *����¼�û����ڴ����������������
*
*@retval ��
*@par ����:
*<li>sre_err.h���ýӿ��������ڵ�ͷ�ļ���</li>
*@since UniDSP V100R001C01
*@see ��
*/
typedef VOID (*SRE_ERRORHANDLE_FUNC)(CHAR    *pcFileName,
				     UINT32   uwLineNo,
				     UINT32   uwErrorNo,
				     UINT32   uwParaLen,
				     VOID     *pPara);

/**
*@ingroup SRE_err
*
*@brief ע���û�������Ĺ��Ӻ�����
*
*@par ����:
*ע��pfnHook��Ϊ�û����Ӻ������ڴ�����ʱ�Ըù��ӽ��е��á�
*@attention �û�����Ĺ��Ӻ����������#SRE_ERRORHANDLE_FUNC�������ʽ������ֻ�ܶ���һ�����Ӻ�����
*<ul>
<li>�����pfnHookΪNULL,��Ϊȡ�����ӡ�
<li>�ظ�ע�ᣬ�������һ��ע�ṳ��Ϊ׼��
*</ul>
*
*@param pfnHook [IN] ����#SRE_ERRORHANDLE_FUNC���û����Ӻ����ĺ궨�塣
*
*@retval ��
*@par ����:
*<li>sre_err.h���ýӿ��������ڵ�ͷ�ļ���</li>
*@since UniDSP V100R001C01
*@see ��
*/
extern VOID SRE_ErrRegHook(SRE_ERRORHANDLE_FUNC pfnHook);

/**
*@ingroup SRE_err
*
*@brief �������
*
*@par ����:
*�û����øú�������ص��û�SRE_ErrRegHook��ע��Ĺ��Ӻ�����
*@attention
* <ul>
* <li>ϵͳ��������μ�⣬�û�SRE_ErrHandle��ȫ����������������롣
* </ul>
*
*@param pcFileName [IN] ����#CHAR *�����������ļ�����������__FILE__��Ϊ���롣
*@param uwLineNo [IN] ����#UINT32�����������ڵ��кţ�������__LINE__��Ϊ���롣
*@param uwErrorNo [IN] ����#UINT32���û�����Ĵ����롣
*@param uwParaLen [IN] ����#UINT32�����pPara�ĳ��ȡ�
*@param pPara [OUT] ����#VOID *����¼�û����ڴ����������������
*
*@retval #SRE_OK      0x00000000���������ɹ���
*@par ����:
*<li>sre_err.h: �ýӿ��������ڵ�ͷ�ļ���</li>
*@since UniDSP V100R001C01
*@see ��
*/
extern UINT32 SRE_ErrHandle(CHAR *pcFileName, UINT32 uwLineNo, UINT32 uwErrorNo, UINT32 uwParaLen, VOID *pPara);

/**
*@ingroup SRE_cda
*
*@brief ��ȡCDA������Ϣ������
*
*@par ����:
*��ȡ����ջ��ʼ��ַ�ͳ���(��������ʱ��Ч)���Լ���ǰ�˴��������з�����ʼ��ַ�Լ����ȡ�
*@attention
* <ul>
* <li>pstCdaPtInfo����һ�����飬����������CDA_PT_INFO_S��Ϊ��ֹ����дԽ�磬��ȷ������ռ��㹻��
* <li>��������Ԫ��˳��Ϊ���û����õ�����ջ+osĬ�ϴ����ķ���+�û����õķ��������CDAʹ��ʵ����
* <li>���pstCdaPtInfo�����Null���ӿڲ��᷵�ش����룬���������سɹ���
* </ul>
*
*@param pstCdaPtInfo [OUT] ����#CDA_PT_INFO_S *����������ջ���ڴ�������׵�ַ�볤�ȡ�
*
*@retval ��
*@par ����:
*<li>sre_err.h: �ýӿ��������ڵ�ͷ�ļ���</li>
*@since UniDSP V100R001C01
*@see ��
*/
extern VOID SRE_CdaInfoGet(CDA_PT_INFO_S *pstCdaPtInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_ERR_H */

/*
 * History: \n
 * 2009-3-13, sdk, Create this file. \n
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */
