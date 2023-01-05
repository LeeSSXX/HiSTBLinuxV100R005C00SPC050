/**
 * @file sre_exc.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * �������쳣ģ��Ķ���ͷ�ļ��� \n
 */

/**@defgroup SRE_exc �쳣�ӹ�
  *@ingroup SRE_inspect
*/

#ifndef _SRE_EXC_H
#define _SRE_EXC_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/**
 *@ingroup SRE_exc
 *�쳣������: �쳣ģ���ʼ������������ʧ��
 *
 *ֵ��0x02001c01
 *
 *���������ȷ�����㹻�Ķ�̬�ڴ�
 */
#define OS_ERRNO_EXC_INIT_BUF_NOT_ENOUGH                        SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x1)

/**
 *@ingroup SRE_exc
 *�쳣������: ��ȡ�쳣��������Ϣ�ӿ����Ϊ��ָ��
 *
 *ֵ��0x02001c02
 *
 *���������������ָ����Ч��
 */
#define OS_ERRNO_EXC_PARA_INVLAID                               SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x2)

/**
 *@ingroup SRE_exc
 *�쳣������: �쳣������Ϊ��
 *
 *ֵ��0x02001c03
 *
 *�����������ȷ���쳣���ܲü������Ѵ�
 */
#define OS_ERRNO_EXC_BUFFER_INVLAID                             SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x3)

/**
 *@ingroup SRE_exc
 *�쳣������: ��������ע��Ĺ��Ӻ���Ϊ��
 *
 *ֵ��0x02001c04
 *
 *���������ע��ǿյ���Ч���Ӻ���
 */
#define OS_ERRNO_LASTWORDS_REGSHTCB_CBFUNC_ISNULL               SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x4)

/**
 *@ingroup SRE_exc
 *�쳣������: ��������ע��Ĺ��Ӻ����Ѿ�����
 *
 *ֵ��0x02001c05
 *
 *�����������������δע����Ĺ��Ӻ�������ע��
 */
#define OS_ERRNO_LASTWORDS_REGSHTCB_CBFUNC_REPEAT               SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x5)

/**
 *@ingroup SRE_exc
 *�쳣������: ȡ���Ĺ��Ӻ���Ϊ��
 *
 *ֵ��0x02001c06
 *
 *�������������ǿյ���Ч���Ӻ���
 */
#define OS_ERRNO_LASTWORDS_CANCEL_CBFUNC_ISNULL                 SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x6)

/**
 *@ingroup SRE_exc
 *�쳣������: ȡ���Ĺ��Ӻ���������
 *
 *ֵ��0x02001c07
 *
 *���������ȷ��Ҫȡ���Ĺ��Ӻ����Ǵ��ڵ�
 */
#define OS_ERRNO_LASTWORDS_CANCEL_CBFUNC_NOEXIST                SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x7)

/**
 *@ingroup SRE_exc
 *�쳣������: ע����������ʱ���Դﵽ�����������������
 *
 *ֵ��0x02001c08
 *
 *����������ͷŵ�������������Թ���
 */
#define OS_ERRNO_EXC_BUFFER_FULL                                SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x8)

/**
 *@ingroup SRE_exc
 *�쳣������: �쳣ģ��ע�ṳ�Ӻ���Ϊ��
 *
 *ֵ��0x02001c09
 *
 *����������鿴ע���쳣���Ӻ����Ƿ�Ϊ��
 */
#define OS_ERRNO_EXC_REG_HOOK_PTR_NULL                          SRE_ERRNO_OS_ERROR(OS_MID_EXC,0x9)

/**
 *@ingroup SRE_exc
 *�쳣������: �쳣ע�ắ����,double�쳣ջ�ռ�����̫С
 *
 *ֵ��0x02001c0a
 *
 *�������������double�쳣ջ�ռ�ĳ��ȣ�ʹ֮�����쳣�Ĵ�����Ϣ��EXC_REGS_S����
 */
#define OS_ERRNO_EXC_REGISTER_DOUBLE_EXC_STACK_TOOSMALL              SRE_ERRNO_OS_ERROR(OS_MID_EXC,0xa)

/**
 *@ingroup SRE_exc
 *�쳣������: �ж����ȼ����뱻�ȣ�����ϵͳ�쳣����
 *
 *ֵ��0x02001c0b
 *
 *���������
 *��Ʒ����ͨ��&g_auwIntMask�ҵ������ŵ�ַ���ж��Ƿ������µ�ֵ
 *SD6108/SD6181 DSP:0xC00010C0 0x307F8D20 0x0F80201E 0x00000201
 *SD6108/SD6181 CPU:0x000000C0 0xF07F9B20 0x0F80201E 0x00000401
 *SD6182:0x2BFFB8C1 0xD400043C 0x00000302
 *������ǣ�˵�����뱻�ȣ������ж����ȼ��������ڵ��ڴ��д����ݶϵ㣬��λΪ�λ�����ж����ȼ����뱻�ȵĴ���
 */
#define OS_ERRNO_EXC_INTERRUPT_LEVEL_MASK_ERROT                      SRE_ERRNO_OS_FATAL(OS_MID_EXC,0xb)

/**
 *@ingroup SRE_exc
 *�쳣������: Hac��ʱ������δע��
 *
 *ֵ��0x02001c12
 *
 *�������������˷�֧�����������һ����Hac��ʱ����soc�е�interrupt�Ĵ������壬�����жϵ��ȵ����쳣��֧��
 *һ�����ж����ȼ����뱻�ȣ����½��������֧��Ȼ�����ڲ�Ʒû��ע������쳣�Ĺ��ӣ���˽�������������
 *��Ʒ����ͨ��&g_auwIntMask�ҵ������ŵ�ַ���ж��Ƿ������µ�ֵ
 *SD6108/SD6181 DSP:0xC00010C0 0x307F8D20 0x0F80201E 0x00000201
 *SD6108/SD6181 CPU:0x000000C0 0xF07F9B20 0x0F80201E 0x00000401
 *SD6182:0x2BFFB8C1 0xD400043C 0x00000302
 *������붼��ȷ������hac���ֳ�ʱ�����ˡ�
 */
#define OS_ERRNO_EXC_HAC_EXCEPTION_HOOK_NOREG                      SRE_ERRNO_OS_FATAL(OS_MID_EXC, 0x12)


/**
 * ����ջ��Ϣ�ṹ��
 */
typedef struct tagCstkStackInfo {
	UINT32  uwStackAddr;                /**< ������ջ���ַ */
	UINT32  uwInstrAddr;                /**< �������ص�ַ   */
} CSTK_FRAME_INFO_S;

#if ((OS_HARDWARE_PLATFORM == OS_RISC170) || (OS_HARDWARE_PLATFORM == OS_DSP170))

/**
 *@ingroup SRE_exc
 * �Ĵ�����Ϣ�ṹ
 *
 * ����:SD6181/SD6108ƽ̨�µ��쳣����ʱ����ļĴ�����Ϣ
 * SD6182ƽ̨�ýṹ����Ϣ��sre_exc.h
 *
 * ע��:���¼Ĵ�������ȥ������ǰ׺��uw��������ӦоƬ�ֲ��еļĴ�������
 */
typedef struct tagTensilicaEccRegInfo {
	UINT32 uwMESR;          /**< ECC״̬�Ĵ���                          */
	UINT32 uwMECR;          /**< ECCУ��Ĵ���                          */
	UINT32 uwMEVADDR;       /**< ECC��ַ�Ĵ���                          */
	UINT32 uwMEPC;          /**< ECC�쳣ʱPC                            */
	UINT32 uwMEPS;          /**< ECC�쳣ʱPS                            */
	UINT32 uwMESAVE;        /**< ECC�쳣�������ݻ���Ĵ���              */
} ECC_REGS_S;

/**
 *@ingroup SRE_exc
 * �Ĵ�����Ϣ�ṹ
 *
 * ����:SD6181&SD6108ƽ̨�µ��쳣����ʱ����ļĴ�����Ϣ
 * SD6182ƽ̨�ýṹ����Ϣ��sre_exc.h
 *
 * ע��:���¼Ĵ�������ȥ������ǰ׺��uw��������ӦоƬ�ֲ��еļĴ�������
 * �ɽӹܵ��쳣����:���˰���ϵͳ�Դ���64���쳣�⣬�����Խӹ������쳣����
 * #define OS_EXCCAUSE_GHWSEMERROR          65   //ȫ��Ӳ���ź����쳣
 * #define OS_EXCCAUSE_LHWSEMERROR          66   //�ֲ�Ӳ���ź����쳣
 * #define OS_EXCCAUSE_WRITEERROR           67   //write error�쳣
 * #define OS_EXCCAUSE_ECCERROR             68   //L1 ECC�쳣
 * #define OS_EXCCAUSE_NMIERROR             69   //NMI�ж�
 * #define OS_EXCCAUSE_RESET_REENTRY        70   //��λ���������쳣
 * #define OS_EXCCAUSE_DOUBLEERROR          71   //double�쳣
 * #define OS_EXCCAUSE_FATALERROR           72   //���������쳣
 * #define OS_EXCCAUSE_STACKOVERFLOW        73   //ջԽ���쳣
 */
typedef struct tagTensilicaExcRegInfo {
	UINT32 uwPC;            /**< pcָ���ַ                             */
	UINT32 uwPS;            /**< ������״̬                             */
	UINT32 uwEXCCAUSE;      /**< ��һ���쳣����                         */
	UINT32 uwEXCVADDR;      /**< ��һ�ζ�ȡ�����ػ�ת���쳣�������ַ   */
	UINT32 uwLBEG;          /**< ѭ����ʼ��ַ                           */
	UINT32 uwLEND;          /**< ѭ��������ַ                           */
	UINT32 uwLCOUNT;        /**< ѭ���������Ĵ���                       */
	UINT32 uwSAR;           /**< ��λ�Ĵ���                             */
	UINT32 auwA[32];        /**< A��32��ͨ�üĴ���                      */
	UINT32 uwWINDOWBASE;    /**< ĿǰAR REG����λ�üĴ���               */
	UINT32 uwWINDOWSTART;   /**< AR REG����pending�Ĵ���                */
	UINT32 uwDEPC;          /**< double �쳣����Ĵ���                  */
	UINT32 uwBR;            /**< br �Ĵ���                              */
	UINT32 uwEPC1;          /**< lv1 �쳣PC�Ĵ���                       */
	UINT32 uwEPC2;          /**< lv2 �쳣PC�Ĵ���                       */
	UINT32 uwEPC3;          /**< lv3 �쳣PC�Ĵ���                       */
	UINT32 uwEPC4;          /**< lv4 �쳣PC�Ĵ���                       */
	UINT32 uwEPC5;          /**< lv5 �쳣PC�Ĵ���                       */
	UINT32 uwEPC6;          /**< lv6 �쳣PC�Ĵ���                       */
	UINT32 uwEPS2;          /**< lv2 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS3;          /**< lv3 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS4;          /**< lv4 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS5;          /**< lv5 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS6;          /**< lv6 �쳣״̬�Ĵ���                     */
	UINT32 uwEXCSAVE1;      /**< lv1 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE2;      /**< lv2 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE3;      /**< lv3 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE4;      /**< lv4 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE5;      /**< lv5 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE6;      /**< lv6 �쳣����Ĵ���                     */
	UINT32 uwINTENABLE;     /**< �ж�ʹ�ܼĴ���                         */
	UINT32 uwINTERRUPT;     /**< �ж�pending�Ĵ���                      */
	UINT32 uwSCOMPARE1;     /**< �����洢�ȽϼĴ���                     */
	UINT32 uwCCOMPARE0;     /**< �ȽϼĴ���0                            */
	UINT32 uwCCOMPARE1;     /**< �ȽϼĴ���1                            */
	UINT32 uwCCOMPARE2;     /**< �ȽϼĴ���2                            */
	UINT32 uwCCOUNT;        /**< �ȽϼĴ�������ֵ                       */
	UINT32 uwCPENABLE;      /**< Э����ʹ�ܼĴ���                       */
	UINT32 uwVECBASE;       /**< ��̬�������ַ                         */
	UINT32 uwDBREAKA0;      /**< ���ݶϵ��ַ0                          */
	UINT32 uwDBREAKA1;      /**< ���ݶϵ��ַ1                          */
	UINT32 uwDBREAKC0;      /**< ���ݶϵ���ƼĴ���0                    */
	UINT32 uwDBREAKC1;      /**< ���ݶϵ���ƼĴ���1                    */
	UINT32 uwDEBUGCAUSE;    /**< ��һ��debug�쳣ԭ��                    */
	UINT32 uwIBREAK0;       /**< ָ��ϵ��ַ0                          */
	UINT32 uwIBREAK1;       /**< ָ��ϵ��ַ1                          */
	UINT32 uwIBREAKENABLE;  /**< ָ��ϵ�ʹ�ܼĴ���                     */
	UINT32 uwICOUNT;        /**< ָ�������                             */
	UINT32 uwICOUNTLEVEL;   /**< ָ������ȼ���ֵ�Ĵ���                 */
	UINT32 uwHwSemErr;      /**< Ӳ���ź����쳣���ͱ�־λ               */
	ECC_REGS_S stEccRegInfo;/**< ECC�쳣����Ĵ�����Ϣ                  */
} EXC_REGS_S;

#else

/**
 * @ingroup SRE_exc
 * Ӳ���ź�������ṹ��
 * ��ǰֻ��6182֧�ָô���ṹ����Ϣ
 */
typedef struct tagTensilicaHwSemErrInfo {
	UINT16  usHwSemErrId: 6 ;    /**<Ӳ���ź�������ID[0-63],���û�з���ȫ���쳣��IDΪ�ֲ�Ӳ���ź���ID������Ϊȫ��Ӳ���ź���ID */
	UINT16  usHwSemGErrFlag: 4;  /**< Ӳ���ź���ȫ���쳣���ͱ�־λ */
	UINT16  usHwSemLErrFlag: 4;  /**< Ӳ���ź����ֲ��쳣���ͱ�־λ */
	UINT16  usResverd: 2;
} HWSEM_ERR_S;

/**
 *@ingroup SRE_exc
 * �Ĵ�����Ϣ�ṹ
 *
 * ����:SD6182ƽ̨�µ��쳣����ʱ����ļĴ�����Ϣ
 *
 * ע��:���¼Ĵ�������ȥ������ǰ׺��uw��������ӦоƬ�ֲ��еļĴ�������
 */
typedef struct tagTensilicaEccRegInfo {
	UINT32 uwMESR;          /**< ECC״̬�Ĵ���                          */
	UINT32 uwMECR;          /**< ECCУ��Ĵ���                          */
	UINT32 uwMEVADDR;       /**< ECC��ַ�Ĵ���                          */
	UINT32 uwMEPC;          /**< ECC�쳣ʱPC                            */
	UINT32 uwMEPS;          /**< ECC�쳣ʱPS                            */
	UINT32 uwMESAVE;        /**< ECC�쳣�������ݻ���Ĵ���              */
} ECC_REGS_S;

/*
 *@ingroup SRE_exc
 * �Ĵ�����Ϣ�ṹ
 *
 * ����:SD6182ƽ̨�µ��쳣����ʱ�����MSGM�쳣�Ĵ�����Ϣ
*/
typedef struct tagMsgmExcRegInfo {
	UINT16              usTraceDbgLostCnt;      /**< Filter������Ϣ����������ʧ������MSGM_DBG_LOST_CNT ��           */
	UINT8               ucNorQueOvLostCnt;      /**< ��������������Ϣ�������������MSGM_NOR_OVLOST_CNT��            */
	UINT8               ucPreserved;            /**< Ԥ���ֽ�                                                       */
	UINT8               aucNorQueOvCnt[6];      /**< ÿ���˵�n��Qport��Ӧ�ĵ�i�����������Ϣͳ�ƼĴ������������֣�
                                                     ������� ��DSP&CPU������QPORT��ÿ��QPORT��2�����У�
                                                     HACCC�˹�3��QPORT3��ÿ��QPORT��2�����У� �����6������         */
	UINT16              usPreserved;            /**< Ԥ���ֽ�                                                       */
	UINT32              uwLsiVioBusError;       /**< LSI���ߴ�������ͼ�¼,LSI�쳣����id�����ͼĴ���MSGM_LSI_VIO    */
	UINT32              uwLsiVioBusErrorAddr;   /**< LSI���߷�����������ַ�Ĵ���MSGM_LSI_VIO_ADDR                 */
	UINT32              uwVioTypeExc;           /**< MSGM�쳣�������ͼĴ���MSGM_VIO_TYPE                            */
	UINT32              uwCreditVioError;       /**< MSGM������Ϣ����credit������Ϣ�Ĵ���MSGM_CRED_VIO              */
} MSGM_REGS_S;

/**
 *@ingroup SRE_exc
 * �Ĵ�����Ϣ�ṹ
 *
 * ����:SD6182ƽ̨�µ��쳣����ʱ����ļĴ�����Ϣ
 *
 * ע��:���¼Ĵ�������ȥ������ǰ׺��uw��������ӦоƬ�ֲ��еļĴ�������
 * �ɽӹܵ��쳣����:���˰���ϵͳ�Դ���64���쳣�⣬�����Խӹ������쳣����
 * #define OS_EXCCAUSE_WRITEERROR           67   //write error�쳣
 * #define OS_EXCCAUSE_ECCERROR             68   //L1 ECC�쳣
 * #define OS_EXCCAUSE_NMIERROR             69   //NMI�ж�
 * #define OS_EXCCAUSE_RESET_REENTRY        70   //��λ���������쳣
 * #define OS_EXCCAUSE_DOUBLEERROR          71   //double�쳣
 * #define OS_EXCCAUSE_FATALERROR           72   //���������쳣
 * #define OS_EXCCAUSE_STACKOVERFLOW        73   //ջԽ���쳣
 * #define OS_EXCCAUSE_HACERROR             74   //HAC�쳣
 * #define OS_EXCCAUSE_HWSEMERROR           75   //Ӳ���ź����쳣
 */
typedef struct tagTensilicaExcRegInfo {
	UINT32 uwPC;            /**< pcָ���ַ                             */
	UINT32 uwPS;            /**< ������״̬                             */
	UINT32 uwEXCCAUSE;      /**< ��һ���쳣����                         */
	UINT32 uwEXCVADDR;      /**< ��һ�ζ�ȡ�����ػ�ת���쳣�������ַ   */
	UINT32 uwLBEG;          /**< ѭ����ʼ��ַ                           */
	UINT32 uwLEND;          /**< ѭ��������ַ                           */
	UINT32 uwLCOUNT;        /**< ѭ���������Ĵ���                       */
	UINT32 uwSAR;           /**< ��λ�Ĵ���                             */
	UINT32 auwA[64];        /**< A��64��ͨ�üĴ���                      */
	UINT32 uwWINDOWBASE;    /**< ĿǰAR REG����λ�üĴ���               */
	UINT32 uwWINDOWSTART;   /**< AR REG����pending�Ĵ���                */
	UINT32 uwDEPC;          /**< double �쳣����Ĵ���                  */
	UINT32 uwBR;            /**< br �Ĵ���                              */
	UINT32 uwEPC1;          /**< lv1 �쳣PC�Ĵ���                       */
	UINT32 uwEPC2;          /**< lv2 �쳣PC�Ĵ���                       */
	UINT32 uwEPC3;          /**< lv3 �쳣PC�Ĵ���                       */
	UINT32 uwEPC4;          /**< lv4 �쳣PC�Ĵ���                       */
	UINT32 uwEPC5;          /**< lv5 �쳣PC�Ĵ���                       */
	UINT32 uwEPS2;          /**< lv2 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS3;          /**< lv3 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS4;          /**< lv4 �쳣״̬�Ĵ���                     */
	UINT32 uwEPS5;          /**< lv5 �쳣״̬�Ĵ���                     */
	UINT32 uwEXCSAVE1;      /**< lv1 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE2;      /**< lv2 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE3;      /**< lv3 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE4;      /**< lv4 �쳣����Ĵ���                     */
	UINT32 uwEXCSAVE5;      /**< lv5 �쳣����Ĵ���                     */
	UINT32 uwINTENABLE;     /**< �ж�ʹ�ܼĴ���                         */
	UINT32 uwINTERRUPT;     /**< �ж�pending�Ĵ���                      */
	UINT32 uwCCOMPARE0;     /**< �ȽϼĴ���0                            */
	UINT32 uwCCOMPARE1;     /**< �ȽϼĴ���1                            */
	UINT32 uwCCOUNT;        /**< �ȽϼĴ�������ֵ                       */
	UINT32 uwCPENABLE;      /**< Э����ʹ�ܼĴ���                       */
	UINT32 uwVECBASE;       /**< ��̬�������ַ                         */
	UINT32 uwDBREAKA0;      /**< ���ݶϵ��ַ0                          */
	UINT32 uwDBREAKA1;      /**< ���ݶϵ��ַ1                          */
	UINT32 uwDBREAKC0;      /**< ���ݶϵ���ƼĴ���0                    */
	UINT32 uwDBREAKC1;      /**< ���ݶϵ���ƼĴ���1                    */
	UINT32 uwDEBUGCAUSE;    /**< ��һ��debug�쳣ԭ��                    */
	UINT32 uwIBREAK0;       /**< ָ��ϵ��ַ0                          */
	UINT32 uwIBREAK1;       /**< ָ��ϵ��ַ1                          */
	UINT32 uwIBREAKENABLE;  /**< ָ��ϵ�ʹ�ܼĴ���                     */
	UINT32 uwICOUNT;        /**< ָ�������                             */
	UINT32 uwICOUNTLEVEL;   /**< ָ������ȼ���ֵ�Ĵ���                 */
	UINT32 uwResvered0;     /**< ���뱣��                               */
	ECC_REGS_S stEccRegInfo;/**< ECC�쳣����Ĵ�����Ϣ                  */
	HWSEM_ERR_S astHwSemErrInfo[17];/**< Ӳ���ź����쳣����Ĵ�����Ϣ   */
	UINT16 usResvered;
	MSGM_REGS_S stMsgmExcRegInfo; /**< MSGM�쳣�Ĵ�����Ϣ               */
} EXC_REGS_S;

#endif

/**
 *@ingroup SRE_exc
 *@brief �����쳣�Ĵ���״̬�������Ͷ��塣
 *
 *@par ����:
 *�ڵ��˸�λʱ����ִ��ʧ�ܵķ�����Ϣͨ���ù��Ӻ������б��档
 *@attention �ޡ�
 *
 *@param uwState [IN] ����#UINT32���쳣����ĳ�����Ϣ��
 *
 *@retval �ޡ�
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C01
 *@see ��
 */
typedef VOID (* EXC_STATE_TRACE_HOOK)(UINT32 uwState);

/**
 *@ingroup SRE_exc
 *@brief �������Թ��Ӻ������Ͷ��塣
 *
 *@par ����:
 *ͨ���ú������Ͷ����������Թ��Ӻ�����
 *@attention �ޡ�
 *
 *@param �ޡ�
 *
 *@retval �ޡ�
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C01
 *@see ��
 */
typedef VOID (* SRE_LASTWORDS_FUNC)(VOID);

/**
 *@ingroup SRE_exc
 *@brief �쳣���������Ͷ��塣
 *
 *@par ����:
 *ͨ���쳣���������Ͷ����쳣���������ӣ���¼�쳣��Ϣ��
 *@attention �ޡ�
 *
 *@param pstExcRegs [IN] ����#EXC_REGS_S *���쳣ʱ�Ĵ�����Ϣ��
 *
 *@retval TRUE��ϵͳ�ڷ����쳣��(��������ش����)������ѭ�����ȴ�������
 *@retval FALSE��ϵͳ�ڷ����쳣��(��������ش����)�������С�
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C01
 *@see ��
 */
typedef BOOL (* EXC_PROC_FUNC)(EXC_REGS_S *pstExcRegs);

/**
 *@ingroup SRE_exc
 *@brief Hac��ʱ�����쳣���������Ͷ��塣
 *
 *@par ����:
 *ͨ��Hac��ʱ�����쳣���������Ͷ����쳣���������ӣ���Ʒͨ���鿴Hac��ʱ�Ĵ����ж��Ƿ���ڳ�ʱ��������ڳ�ʱ����TRUE(1)����������ڳ�ʱ��˵��ϵͳ�����쳣������FALSE(0),ϵͳ�����쳣�ӹܡ�
 *@attention �ޡ�
 *
 *@param VOID��
 *
 *@retval TRUE��ϵͳ��⵽HAC��ʱ������֮��OS���ж϶�������
 *@retval FALSE��ϵͳδ��⵽HAC��ʱ������֮��OS���쳣����
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V200R003C06
 *@see ��
 */
typedef BOOL (* HACTIMEOUT_PROC_FUNC)(VOID);

/**
 *@ingroup SRE_exc
 *ģ��������Ϣ�ṹ��
 */
typedef struct tagExcModInfo {
	HACTIMEOUT_PROC_FUNC    pfnHacExtHook;          /**< Hac�쳣����                                */
	EXC_PROC_FUNC           pfnExcepHook;           /**< �쳣ʱ�û�ע��ĺ�������                    */
	UINT32                  uwStackSize;            /**< �����쳣ʱ�����ջ�Ĵ�С                    */
	VOID                    *pDoubleExcStackStart;  /**< DoubleException��ջ��ʼ��ַ                */
	UINT32                  uwDoubleExcStackSize;   /**< DoubleException��ջ��С                    */
} EXC_MOD_INFO_S;

/**
 *@ingroup SRE_exc
 *@brief �쳣�������׵�ַ�ͳ��Ȼ�ȡ������
 *
 *@par ����:
 *��ȡ�쳣��Ϣ���������׵�ַ���С��
 *@attention �쳣����ʱ���ɸ��ݻ��������׵�ַ�ʹ�С��������ȡ���쳣��Ϣ�����쳣ʱ���˼��ڴ�ֵΪ0��
 *
 *@param ppBufAddr [OUT] ����#VOID **�����ػ������׵�ַ��
 *@param puwBufLen [OUT] ����#UINT32 *�����������ȡ�
 *
 *@retval #SRE_OK 0x00000000���ɹ����ء�
 *@retval #OS_ERRNO_EXC_PARA_INVLAID 0x02001c02�����ָ��Ϊ�ա�
 *@retval #OS_ERRNO_EXC_BUFFER_INVLAID 0x02001c03��������Ϊ�ա�
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C01
 *@see ��
 */
extern UINT32 SRE_ExcGetBufInfo(VOID **ppBufAddr, UINT32 *puwBufLen);

/**
 *@ingroup SRE_exc
 *@brief �������Թ���ע�ắ����
 *
 *@par ����:
 *ע���������Թ��ӵ��������������С�
 *@attention
 *@li ע���������֧��8����
 *
 *@param ptrHook [IN] ����#SRE_LASTWORDS_FUNC�����Ӻ�����
 *
 *
 *@retval #SRE_OK 0x00000000���ɹ����ء�
 *@retval #OS_ERRNO_EXC_BUFFER_FULL 0x02001c08��ע����������ʱ���Դﵽ�����������������
 *@retval #OS_ERRNO_LASTWORDS_REGSHTCB_CBFUNC_ISNULL 0x02001c04��ע��Ĺ��Ӻ���Ϊ�ա�
 *@retval #OS_ERRNO_LASTWORDS_REGSHTCB_CBFUNC_REPEAT 0x02001c05��ע��Ĺ��Ӻ����Ѿ����ڡ�
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C02
 *@see SRE_LastWordsDisHook
 */
extern UINT32 SRE_LastWordsRegHook(SRE_LASTWORDS_FUNC ptrHook);

/**
 *@ingroup SRE_exc
 *@brief �������Թ���ȡ��������
 *
 *@par ����:
 *����ע����������Թ��Ӵ�������ɾ����
 *@attention ��
 *
 *@param ptrHook [IN] ����#SRE_LASTWORDS_FUNC�����Ӻ�����
 *
 *
 *@retval #SRE_OK 0x00000000���ɹ����ء�
 *@retval #OS_ERRNO_LASTWORDS_CANCEL_CBFUNC_ISNULL 0x02001c06��ȡ���Ĺ��Ӻ���Ϊ��
 *@retval #OS_ERRNO_LASTWORDS_CANCEL_CBFUNC_NOEXIST 0x02001c07��ȡ���Ĺ��Ӻ��������ڡ�
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C02
 *@see SRE_LastWordsRegHook
 */
extern UINT32 SRE_LastWordsDisHook(SRE_LASTWORDS_FUNC ptrHook);

/**
 *@ingroup SRE_exc
 *@brief �û�ע���쳣�����ӡ�
 *
 *@par ����:
 *ע���쳣�����ӡ�
 *@attention �����ע��ù���ʱ�����һ��ע��Ĺ�����Ч��
 *
 *@param pfnHook [IN] ����#EXC_PROC_FUNC�����Ӻ�����
 *
 *
 *@retval #SRE_OK 0x00000000���ɹ����ء�
 *@retval #OS_ERRNO_EXC_REG_HOOK_PTR_NULL 0x02001c09�����Ӻ���Ϊ��
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V100R001C02
 *@see ��
 */
extern UINT32 SRE_ExcRegHook(EXC_PROC_FUNC pfnHook);

/**
 *@ingroup SRE_exc
 *@brief �û�ע��Hac��ʱ�����쳣�����û����ӡ�
 *
 *@par ����:
 *ע��Hac��ʱ�����쳣�����û����ӡ�
 *@attention �����ע��ù���ʱ�����һ��ע��Ĺ�����Ч��
 *
 *@param pfnHook [IN] ����#HACTIMEOUT_PROC_FUNC�����Ӻ�����
 *
 *
 *@retval #SRE_OK 0x00000000���ɹ����ء�
 *@retval #OS_ERRNO_EXC_REG_HOOK_PTR_NULL 0x02001c09�����Ӻ���Ϊ��
 *
 *@par ����:
 *sre_exc.h���ýӿ��������ڵ�ͷ�ļ�
 *@since UniDSP V200R003C06
 *@see ��
 */
extern UINT32 SRE_HacTimeOutCheckHook(HACTIMEOUT_PROC_FUNC pfnHook);

/**
 * @ingroup  SRE_exc
 * @brief ��ȡRTOSck�쳣��ڵ�ַ
 *
 * @par ����:
 * SD6181оƬ�ṩ��һ��Ӳ��TraceBuffer���ܣ�����ʵʱѭ����¼ָ��ִ�������������쳣����ʱ��Ϊ����TraceBuffer�е���Ϣ���쳣�������ָ�������ǣ�
 * Ӧ��ֹͣ��¼ָ�������û����������API��ȡ�쳣��ڵ�ַ,���õ�Ӳ��Trace�ṩ��һ�������Ĵ�������PC������Ĵ�����ֵ��ͬʱ��TraceBuffer�Զ�ֹͣ
 * ָ�����ļ�¼��
 * @attention ��
 *
 * <ul>
 * <li>��</li>
 * </ul>
 *
 * @param  ��
 *
 * @retval ������ַ                  ������ַ���쳣����ĺ�����ַ��
 * @par ����:
 * sre_exc.h���ýӿ��������ڵ�ͷ�ļ���
 * @since UniDSP V200R003C05
 * @see ��
 */
extern VOID *SRE_ExcEntryGet(VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SRE_EXC_H */

/**
 * History:
 * 2009-02-23 sdk: Create this file.
 * 2010-12-23 sdk: Modify this file.
 * 2012-09-07 sdk: Modify this file.
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

