/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_hardware.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��2��29��
  ����޸�   :
  ��������   : ����ϵͳӲ����ؽӿڷ�װ
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��29��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LITEOS_HARDWARE_H__
#define __OAL_LITEOS_HARDWARE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
/*lint -e322*/
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/ioport.h>
/*lint +e322*/

#include "oal_util.h"
/*****************************************************************************
  2 �궨��
*****************************************************************************/
/*BEGIN:Added by zhouqingsong/2012/2/15 for SD5115V100*/
#define OAL_HI_TIMER_REG_BASE               (0x10105000)

#define OAL_HI_TIMER_NUM                    2
#define OAL_HI_TIMER_ENABLE                 1
#define OAL_HI_TIMER_DISABLE                0
#define OAL_HI_TIMER_INT_DISABLE            1
#define OAL_HI_TIMER_INT_CLEAR              0
#define OAL_HI_TIMER_DEFAULT_PERIOD         1

#define OAL_HI_TIMER_IRQ_NO                 80            /*5113 : 5   5115:80*/

#define OAL_HI_TIMER_FREE_MODE              0         /* 1101�������� */
#define OAL_HI_TIMER_CYCLE_MODE             1
#define OAL_HI_TIMER_SIZE_32_BIT            1
#define OAL_HI_TIMER_WRAPPING               0
#define OAL_HI_TIMER_INT_OCCUR              1
#define OAL_HI_TIMER_INT_VALID              0x01
#define OAL_HI_TIMER_NO_DIV_FREQ            0x0

#define OAL_HI_SC_REG_BASE                  (0x10100000)
#define OAL_HI_SC_CTRL                      (OAL_HI_SC_REG_BASE + 0x0000)

#define OAL_IRQ_ENABLE                      1  /* �����ж� */
#define OAL_IRQ_FORBIDDEN                   0  /* ��ֹ�ж� */

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
typedef enum
{
    OAL_5115TIMER_ONE,
    OAL_5115TIMER_SEC,

    OAL_5115TIMER_BUTT
}oal_5115timer_enum;
typedef oal_uint8 oal_5115timer_enum_uint8;

typedef enum
{
    OAL_5115IRQ_ADRDSH = 0,
    OAL_5115IRQ_ADUDSH = 1,
    OAL_5115IRQ_ADRT = 2,
    OAL_5115IRQ_DCSWR = 3,
    OAL_5115IRQ_DTSPl0 = 4,
    OAL_5115IRQ_DTSPl1 = 5,
    OAL_5115IRQ_DDERTF = 6,
    OAL_5115IRQ_DMSC = 7,
    OAL_5115IRQ_DRMP = 8,
    OAL_5115IRQ_DTOWT = 9,
    OAL_5115IRQ_FEPE = 10,
    OAL_5115IRQ_FES = 11,
    OAL_5115IRQ_FEA = 12,
    OAL_5115IRQ_FEF = 13,
    OAL_5115IRQ_HIIA = 14,
    OAL_5115IRQ_HRI = 15,
    OAL_5115IRQ_MFSC = 16,
    OAL_5115IRQ_OIS = 17,
    OAL_5115IRQ_OMSNA = 18,
    OAL_5115IRQ_OMSNF = 19,
    OAL_5115IRQ_OSLIS = 20,
    OAL_5115IRQ_WH5RW = 21,
    OAL_5115IRQ_WH5RR = 22,

    OAL_5115IRQ_BUTT
}oal_5115irq_enum;
typedef oal_uint8 oal_5115irq_enum_uint8;

/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
typedef struct cpumask     *oal_cpumask;
typedef oal_uint32          oal_irq_num;

#define IRQF_SHARED		0x00000080
#define OAL_SA_SHIRQ        IRQF_SHARED       /* �ж����� */

typedef oal_uint32        (*oal_irq_intr_func)(void *);

typedef struct
{
    volatile oal_uint32 ul_timerx_load;
    volatile oal_uint32 ul_timerx_value;
    volatile oal_uint32 ul_timerx_control;
    volatile oal_uint32 ul_timerx_intclr;
    volatile oal_uint32 ul_timerx_ris;
    volatile oal_uint32 ul_timerx_mis;
    volatile oal_uint32 ul_timerx_bgload;
    volatile oal_uint32 ul_reserve;
} oal_hi_timerx_reg_stru;
/*timer���ƼĴ���*/
typedef union
{
    volatile oal_uint32 ul_value;
    struct
    {
        volatile oal_uint32 ul_oneshot: 1;                 /*ѡ�����ģʽ 0���ؾ���� 1��һ���Լ���*/
        volatile oal_uint32 ul_timersize: 1;               /*16bit|32bit��������ģʽ 0��16bit 1��32bit*/
        volatile oal_uint32 ul_timerpre: 2;                /*Ԥ��Ƶ���� 00������Ƶ 01��4����Ƶ 10��8����Ƶ 11��δ���壬�����൱�ڷ�Ƶ����10*/
        volatile oal_uint32 ul_reserved0: 1;               /*����λ*/
        volatile oal_uint32 ul_intenable: 1;               /*�ж�����λ 0������ 1��������*/
        volatile oal_uint32 ul_timermode: 1;               /*����ģʽ 0������ģʽ 1������ģʽ*/
        volatile oal_uint32 ul_timeren: 1;                 /*��ʱ��ʹ��λ 0����ֹ 1��ʹ��*/
        volatile oal_uint32 ul_reserved1: 24;              /*����λ*/
    } bits_stru;
} oal_hi_timer_control_union;

/*timer2_3�Ĵ���*/
typedef struct
{
    oal_hi_timerx_reg_stru ast_timer[2];
} oal_hi_timer_reg_stru;
typedef struct
{
    oal_hi_timer_control_union  u_timerx_config;
}oal_hi_timerx_config_stru;
typedef struct
{
    oal_uint16  vendor;
    oal_uint16	device;
    oal_uint32  irq;
}oal_pci_dev_stru;

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
/* PCI?y?��?��1?Win32��a���� */
typedef struct
{
    oal_uint32  vendor;         /* Vendor and device ID or PCI_ANY_ID */
    oal_uint32  device;
    oal_uint32  subvendor;      /* Subsystem ID's or PCI_ANY_ID */
    oal_uint32  subdevice;
    oal_uint32  class_;
    oal_uint32  class_mask;
    oal_uint32  driver_data;    /* data private to the driver */
}oal_pci_device_id_stru;

typedef struct
{
    oal_int8                         *pc_name;
    OAL_CONST oal_pci_device_id_stru *id_table;
    oal_int32 (*probe)(oal_pci_dev_stru *dev, OAL_CONST oal_pci_device_id_stru *id);
    void      (*remove)(oal_pci_dev_stru *dev);
}oal_pci_driver_stru;
#endif
/* �ж��豸�ṹ�� */
typedef struct
{
    oal_uint32              ul_irq;                  /* �жϺ� */
    oal_int32               l_irq_type;             /* �ж����ͱ�־ */
    oal_void               *p_drv_arg;              /* �жϴ��������� */
    oal_int8               *pc_name;                /* �ж��豸���� ֻΪ�����Ѻ� */
    oal_irq_intr_func       p_irq_intr_func;        /* �жϴ�������ַ */
}oal_irq_dev_stru;

typedef oal_uint8   oal_hi_timerx_index_enum_uint8;

typedef enum
{
    HI5115_TIMER_INDEX_0 = 0,
    HI5115_TIMER_INDEX_1,
    HI5115_TIMER_INDEX_2,
    HI5115_TIMER_INDEX_3,
    HI5115_TIMER_INDEX_BUTT
}oal_hi_timerx_index_enum;

/*****************************************************************************
  8 UNION����
*****************************************************************************/

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
extern oal_hi_timer_reg_stru *g_pst_reg_timer;
extern oal_uint32 g_aul_irq_save_time[][255];

/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : oal_irq_free
 ��������  : �ͷ��жϴ������
 �������  : st_osdev: �ж��豸�ṹ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_irq_free(oal_irq_dev_stru *st_osdev)
{
    free_irq(st_osdev->ul_irq, st_osdev);
}

/*****************************************************************************
 �� �� ��  : oal_irq_enable
 ��������  : �����ж�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_irq_enable(oal_void)
{
    local_irq_enable();
}

/*****************************************************************************
 �� �� ��  : oal_irq_disable
 ��������  : ��ֹ�����ж�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_irq_disable(oal_void)
{
    local_irq_disable();
}

/*****************************************************************************
 �� �� ��  : oal_irq_interrupt
 ��������  : �жϷ������
 �������  : l_irq: �жϺ�
             p_dev: �ж��豸
 �������  : ��
 �� �� ֵ  : IRQ_HANDLED�жϳ��������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE irqreturn_t  oal_irq_interrupt(oal_int32 l_irq, oal_void *p_dev)
{
    oal_irq_dev_stru *st_osdev = (oal_irq_dev_stru *)p_dev;

    st_osdev->p_irq_intr_func((oal_void *)st_osdev);

    return IRQ_HANDLED;
}

/*****************************************************************************
 �� �� ��  : oal_irq_setup
 ��������  : ע���жϡ�
 �������  : st_osdev: �ж��豸�ṹ��
             fn: �жϺ���ִ�еĺ�����ַ
 �������  : ��
 �� �� ֵ  : �ɹ�����0�� ʧ�ܷ��ط��㣻
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_int32  oal_irq_setup(oal_irq_dev_stru *st_osdev)
{
    oal_int32 l_err = 0;

    l_err = request_irq(st_osdev->ul_irq, oal_irq_interrupt,
                        st_osdev->l_irq_type, st_osdev->pc_name, (oal_void*)st_osdev);

    return l_err;
}

/*****************************************************************************
 �� �� ��  : oal_irq_trigger
 ��������  : �������Ŀ��˵�Ӳ���ж�
 �������  : mask: Ŀ��˺�����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_irq_trigger(oal_uint8 uc_cpuid)
{

}
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
/*****************************************************************************
 �� �� ��  : oal_5115timer_get_10ns
 ��������  : ��ȡ5115Ӳ����ʱ��10ns����ʱ���
 �������  : en_timer: ��������ʱ��ѡһ��
 �������  : ��
 �� �� ֵ  : 10ns���ȵ�ʱ���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32  oal_5115timer_get_10ns(oal_void)
{
    /* 02 ������ʵ������ TBD */
    return 1;
}

#else
/*****************************************************************************
 �� �� ��  : oal_5115timer_get_10ns
 ��������  : ��ȡ5115Ӳ����ʱ��10ns����ʱ���
 �������  : en_timer: ��������ʱ��ѡһ��
 �������  : ��
 �� �� ֵ  : 10ns���ȵ�ʱ���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32  oal_5115timer_get_10ns(oal_void)
{
#if(_PRE_TARGET_PRODUCT_TYPE_WS835DMB == _PRE_CONFIG_TARGET_PRODUCT) //��Ʒ�����˸��жϵĵڶ�����ʱ��
    return g_pst_reg_timer->ast_timer[OAL_5115TIMER_ONE].ul_timerx_value;
#else
    return g_pst_reg_timer->ast_timer[OAL_5115TIMER_SEC].ul_timerx_value;
#endif
}
#endif
/*****************************************************************************
 �� �� ��  : oal_irq_save
 ��������  : ��ֹ�ж�,��ȡ�ж�״̬
 �������  : pui_flags: �ж�״̬�Ĵ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_irq_save(oal_uint *pui_flags, oal_uint32 ul_type)
{
#ifdef _PRE_DEBUG_MODE
/*  start */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)

    oal_uint32 ul_core_id = OAL_GET_CORE_ID();
#endif
#endif
    local_irq_save(*pui_flags);
#ifdef _PRE_DEBUG_MODE
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)

    /* �������һ����������saveʱ�� */
    /* �������ڶ�����������save�����ͣ�����������������͵����save - restore��ʱ��*/
    /* ÿ��restore��ʱ����Ҫ���saveʱ�䣬�����ж������ظ�save */
        if (g_aul_irq_save_time[ul_core_id][254] == 0)
        {
            g_aul_irq_save_time[ul_core_id][254] = oal_5115timer_get_10ns();
            g_aul_irq_save_time[ul_core_id][253] = ul_type;
        }
        else
        {
            /* �ظ�save */
            OAL_IO_PRINT("\n core %d oal_irq_save[%d] failed, already saved by [%d] \n",ul_core_id, ul_type, g_aul_irq_save_time[ul_core_id][253]);
            oal_dump_stack();
        }
#endif
/*  end */

#endif
}

/*****************************************************************************
 �� �� ��  : oal_irq_restore
 ��������  : �ָ��ж�,�ָ��ж�״̬
 �������  : *pui_flags: �ж�״̬��ַ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_irq_restore(oal_uint *pui_flags, oal_uint32 ul_type)
{
#ifdef _PRE_DEBUG_MODE
/*  start */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    oal_uint32 ul_restore_time;
    oal_uint32 ul_core_id = OAL_GET_CORE_ID();

    if (g_aul_irq_save_time[ul_core_id][254] != 0)
    {
        /* restoreʱ����Ҫ�ж��ϴ�save��type�Ƿ���ͬ������ͬΪ�Ƿ� */
        if ((ul_type < 253) && (g_aul_irq_save_time[ul_core_id][253] == ul_type))
        {
            ul_restore_time = g_aul_irq_save_time[ul_core_id][254] - oal_5115timer_get_10ns();

            /* ��¼������͵�save - restore ���ֵ */
            if (g_aul_irq_save_time[ul_core_id][ul_type] < ul_restore_time)
            {
                g_aul_irq_save_time[ul_core_id][ul_type] = ul_restore_time;
            }
        }
        else
        {
            /* restore���� */
            OAL_IO_PRINT("\n core %d oal_irq_restore[%d] failed, should be [%d] \n",ul_core_id, ul_type, g_aul_irq_save_time[ul_core_id][253]);
            oal_dump_stack();
        }
        g_aul_irq_save_time[ul_core_id][254] = 0;
        g_aul_irq_save_time[ul_core_id][253] = -1;
    }
    else
    {
        /* �ظ�restore */
        printk("\n core %d oal_irq_restore[%d] failed, already restored \n",ul_core_id, ul_type);
        oal_dump_stack();
    }
#endif
/*  end */
#endif
    local_irq_restore(*pui_flags);
}

/*****************************************************************************
 �� �� ��  : oal_irq_set_affinity
 ��������  : ��ָ�����жϺŰ󶨵�ָ����cpu��ִ��
 �������  : ul_irq: �жϺ�
             mask: cpu����
 �������  : ��
 �� �� ֵ  : �ɹ�����OAL_SUCC,ʧ�ܷ���OAL_FAIL
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_int32  oal_irq_set_affinity(oal_irq_num irq, oal_uint32 ul_cpu)
{
    oal_int32 l_ret;
#if 0
    struct cpumask mask;

    cpumask_clear(&mask);
    cpumask_set_cpu(ul_cpu, &mask);

    l_ret = irq_set_affinity(irq, &mask);

    if (l_ret != 0)
    {
        return OAL_FAIL;
    }
#endif
    return OAL_SUCC;
}
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)

/*****************************************************************************
 �� �� ��  : oal_5115timer_init
 ��������  : ��ʼ��5115Ӳ����ʱ��
 �������  : en_timer:Ҫ�õĶ�ʱ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_5115timer_init(oal_void)
{
    oal_hi_timer_control_union u_reg_control;

    g_pst_reg_timer = (oal_hi_timer_reg_stru *)ioremap(OAL_HI_TIMER_REG_BASE, sizeof(oal_hi_timer_reg_stru));

    /*��timer������*/
#if(_PRE_TARGET_PRODUCT_TYPE_WS835DMB == _PRE_CONFIG_TARGET_PRODUCT) //��Ʒ�����˸��жϵĵڶ�����ʱ��
    u_reg_control.ul_value = g_pst_reg_timer->ast_timer[OAL_5115TIMER_ONE].ul_timerx_control;
#else
    u_reg_control.ul_value = g_pst_reg_timer->ast_timer[OAL_5115TIMER_SEC].ul_timerx_control;
#endif
    /* ����ģʽΪ����ģʽ */
    u_reg_control.bits_stru.ul_timermode = OAL_HI_TIMER_FREE_MODE;

    /*����Ƶ*/
    u_reg_control.bits_stru.ul_timerpre = OAL_HI_TIMER_NO_DIV_FREQ;

    /* �����ж� */
    u_reg_control.bits_stru.ul_intenable = OAL_HI_TIMER_INT_CLEAR;

    /*����Ϊ32bit��������ģʽ*/
    u_reg_control.bits_stru.ul_timersize = OAL_HI_TIMER_SIZE_32_BIT;

    /*����Ϊ�ؾ����*/
    u_reg_control.bits_stru.ul_oneshot = OAL_HI_TIMER_WRAPPING;

    /*ʹ�ܼĴ���*/
    u_reg_control.bits_stru.ul_timeren = OAL_TRUE;       /* HI_TRUE_E */

    /*д��timer������*/
#if(_PRE_TARGET_PRODUCT_TYPE_WS835DMB == _PRE_CONFIG_TARGET_PRODUCT) //��Ʒ�����˸��жϵĵڶ�����ʱ��
    g_pst_reg_timer->ast_timer[OAL_5115TIMER_ONE].ul_timerx_control = u_reg_control.ul_value;
#else
    g_pst_reg_timer->ast_timer[OAL_5115TIMER_SEC].ul_timerx_control = u_reg_control.ul_value;
#endif
}

/*****************************************************************************
 �� �� ��  : oal_5115timer_exit
 ��������  : �ͷ�Ӳ����ʱ�������ڴ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_5115timer_exit(oal_void)
{
    iounmap(g_pst_reg_timer);
}
#else

/*****************************************************************************
 �� �� ��  : oal_5115timer_init
 ��������  : ��ʼ��5115Ӳ����ʱ��
 �������  : en_timer:Ҫ�õĶ�ʱ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_5115timer_init(oal_void)
{

}

/*****************************************************************************
 �� �� ��  : oal_5115timer_exit
 ��������  : �ͷ�Ӳ����ʱ�������ڴ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_5115timer_exit(oal_void)
{
}

#endif

/* ����һ���µı�ʹ����Դ�� */
#define oal_request_mem_region(start, n, name)  request_mem_region(start, n, name)
#define oal_release_mem_region(start, n)        release_mem_region(start, n)

#define oal_ioremap(cookie, size)               ioremap(cookie, size)
#define oal_ioremap_nocache(_cookie, _size)     ioremap_nocache(_cookie, _size)
#define oal_iounmap(cookie)                     iounmap(cookie)

#define oal_writel(_ul_val, _ul_addr)           writel(_ul_val, _ul_addr)

#define oal_readl(_ul_addr)                     readl(_ul_addr)

OAL_STATIC  OAL_INLINE oal_hi_timerx_reg_stru  *hi_timerx_request(oal_hi_timerx_index_enum_uint8  en_timer_idx)
{
    oal_hi_timerx_reg_stru      *pst_timer  = NULL;

    if (en_timer_idx < HI5115_TIMER_INDEX_BUTT)
    {
        pst_timer = (oal_hi_timerx_reg_stru *)oal_ioremap(OAL_HI_TIMER_REG_BASE + sizeof(oal_hi_timerx_reg_stru) * en_timer_idx,
                                                            sizeof(oal_hi_timerx_reg_stru));
    }

    return pst_timer;
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_load(oal_hi_timerx_reg_stru  *pst_timer, oal_uint32 ul_load)
{
    if(NULL != pst_timer)
    {
        pst_timer->ul_timerx_load = ul_load;
    }
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_start(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        ((oal_hi_timer_control_union *)&pst_timer->ul_timerx_control)->bits_stru.ul_timeren = OAL_TRUE;
    }
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_stop(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        ((oal_hi_timer_control_union *)&pst_timer->ul_timerx_control)->bits_stru.ul_timeren = OAL_FALSE;
    }
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_release(oal_hi_timerx_reg_stru  *pst_timer)
{
    if(NULL != pst_timer)
    {
        hi_timerx_stop(pst_timer);
        oal_iounmap(pst_timer);
    }
}

OAL_STATIC  OAL_INLINE  oal_uint32    hi_timerx_read(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        return  pst_timer->ul_timerx_value;
    }

    return  0xDEAD; // TODO
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_enable_intr(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        ((oal_hi_timer_control_union *)&pst_timer->ul_timerx_control)->bits_stru.ul_intenable = OAL_TRUE;
    }
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_disable_intr(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        ((oal_hi_timer_control_union *)&pst_timer->ul_timerx_control)->bits_stru.ul_intenable = OAL_FALSE;
    }
}

OAL_STATIC  OAL_INLINE  oal_void    hi_timerx_clear_intr(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        pst_timer->ul_timerx_intclr = 0x0001; // write any value will clear intr
    }
}
OAL_STATIC  OAL_INLINE  oal_bool_enum_uint8  hi_timerx_intr_hit(oal_hi_timerx_reg_stru *pst_timer)
{
    if(NULL != pst_timer)
    {
        return pst_timer->ul_timerx_ris & 0x0001 ? OAL_TRUE : OAL_FALSE;
    }

    return OAL_FALSE;
}
OAL_STATIC  OAL_INLINE oal_void    hi_timerx_control(oal_hi_timerx_reg_stru        *pst_timer,
                                                     oal_hi_timerx_config_stru     *pst_config)
{
    if ((NULL != pst_timer) && (NULL != pst_config))
    {
        pst_timer->ul_timerx_control = pst_config->u_timerx_config.ul_value;
    }
}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_hardware.h */
