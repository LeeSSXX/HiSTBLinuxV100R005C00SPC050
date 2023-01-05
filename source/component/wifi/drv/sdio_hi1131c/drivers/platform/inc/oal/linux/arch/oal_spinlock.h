/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_spinlock.h
  �� �� ��   : ����
  ��    ��   : ds
  ��������   : 2016��7��27��
  ����޸�   :
  ��������   : oal_spinlock.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��7��27��
    ��    ��   : ds
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LINUX_SPINLOCK_H__
#define __OAL_LINUX_SPINLOCK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAL_SPINLOCK_H

#include <linux/spinlock.h>


/*****************************************************************************
  2 �궨��
*****************************************************************************/

#define OAL_SPIN_LOCK_MAGIC_TAG (0xdead4ead)
typedef struct _oal_spin_lock_stru_
{
#ifdef CONFIG_SPIN_LOCK_MAGIC_DEBUG
    oal_uint32  magic;
    oal_uint32  reserved;
#endif
    spinlock_t  lock;
}oal_spin_lock_stru;

#ifdef CONFIG_SPIN_LOCK_MAGIC_DEBUG
#define OAL_DEFINE_SPINLOCK(x) 	 oal_spin_lock_stru x = {\
            .magic = OAL_SPIN_LOCK_MAGIC_TAG,\
            .lock = __SPIN_LOCK_UNLOCKED(x)}
#else
#define OAL_DEFINE_SPINLOCK(x) 	 oal_spin_lock_stru x = {\
            .lock = __SPIN_LOCK_UNLOCKED(x)}
#endif

/* ����ָ�룬����ָ����Ҫ�����������ĵĺ��� */
typedef oal_uint32              (*oal_irqlocked_func)(oal_void *);


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/


/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : oal_spin_lock_init
 ��������  : ��������ʼ����������������Ϊ1��δ��״̬����
 �������  : *pst_lock: ���ĵ�ַ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_spin_lock_init(oal_spin_lock_stru *pst_lock)
{
    spin_lock_init(&pst_lock->lock);
#ifdef CONFIG_SPIN_LOCK_MAGIC_DEBUG	
    pst_lock->magic = OAL_SPIN_LOCK_MAGIC_TAG;
#endif
}

OAL_STATIC OAL_INLINE oal_void  oal_spin_lock_magic_bug(oal_spin_lock_stru *pst_lock)
{
#ifdef CONFIG_SPIN_LOCK_MAGIC_DEBUG
    if(OAL_UNLIKELY((oal_uint32)OAL_SPIN_LOCK_MAGIC_TAG != pst_lock->magic))
    {
#ifdef CONFIG_PRINTK
        /*spinlock never init or memory overwrite?*/
    	printk(KERN_EMERG "[E]SPIN_LOCK_BUG: spinlock:%p on CPU#%d, %s,magic:%08x should be %08x\n", pst_lock,
    	                    raw_smp_processor_id(),current->comm, pst_lock->magic,OAL_SPIN_LOCK_MAGIC_TAG);
        print_hex_dump(KERN_EMERG, "spinlock_magic: ", DUMP_PREFIX_ADDRESS, 16, 1,
	       (oal_uint8 *)((oal_ulong)pst_lock - 32), 32 + sizeof(oal_spin_lock_stru) + 32, true);
        printk(KERN_EMERG"\n");
#endif
        OAL_WARN_ON(1);
    }
#endif
}

/*****************************************************************************
 �� �� ��  : oal_spin_lock
 ��������  : �����������ж��Լ��ں��̵߳Ⱥ���̬�����Ļ����µļ������������
             �ܹ�������������������Ϸ��أ������������������ֱ��������
             ���ı������ͷţ���ʱ��������������ء�
 �������  : *pst_lock:��������ַ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_spin_lock(oal_spin_lock_stru *pst_lock)
{
    oal_spin_lock_magic_bug(pst_lock);
    spin_lock(&pst_lock->lock);
}

/*****************************************************************************
 �� �� ��  : oal_spin_unlock
 ��������  : Spinlock���ں��̵߳Ⱥ���̬�����Ļ����µĽ���������
 �������  : *pst_lock:��������ַ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_spin_unlock(oal_spin_lock_stru *pst_lock)
{
    oal_spin_lock_magic_bug(pst_lock);
    spin_unlock(&pst_lock->lock);
}

/*****************************************************************************
 �� �� ��  : oal_spin_lock_bh
 ��������  : �����������ж��Լ��ں��̵߳Ⱥ���̬�����Ļ����µļ������������
             �ܹ�������������������Ϸ��أ������������������ֱ��������
             ���ı������ͷţ���ʱ��������������ء�
 �������  : pst_lock:��������ַ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void oal_spin_lock_bh(oal_spin_lock_stru *pst_lock)
{
    oal_spin_lock_magic_bug(pst_lock);
    spin_lock_bh(&pst_lock->lock);
}

/*****************************************************************************
 �� �� ��  : oal_spin_unlock_bh
 ��������  : Spinlock�����ж��Լ��ں��̵߳Ⱥ���̬�����Ļ����µĽ���������
 �������  : ��
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void oal_spin_unlock_bh(oal_spin_lock_stru *pst_lock)
{
     oal_spin_lock_magic_bug(pst_lock);
     spin_unlock_bh(&pst_lock->lock);
}

/*****************************************************************************
 �� �� ��  : oal_spin_lock_irq_save
 ��������  : �����������ͬʱ��ñ����־�Ĵ�����ֵ������ʧЧ�����жϡ�
 �������  : *pst_lock:��������ַ
             pui_flags:��־�Ĵ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_spin_lock_irq_save(oal_spin_lock_stru *pst_lock, oal_uint *pui_flags)
{
    oal_spin_lock_magic_bug(pst_lock);
    spin_lock_irqsave(&pst_lock->lock, *pui_flags);
}

/*****************************************************************************
 �� �� ��  : oal_spin_unlock_irq_restore
 ��������  : �ͷ���������ͬʱ���ָ���־�Ĵ�����ֵ���ָ������жϡ�����oal_sp-
             in_lock_irq���ʹ��
 �������  : *pst_lock:��������ַ
             pui_flags:��־�Ĵ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_spin_unlock_irq_restore(oal_spin_lock_stru *pst_lock, oal_uint *pui_flags)
{
    oal_spin_lock_magic_bug(pst_lock);
    spin_unlock_irqrestore(&pst_lock->lock, *pui_flags);
}

/*****************************************************************************
 �� �� ��  : oal_spin_lock_irq_exec
 ��������  : ��ȡ���������ر��жϣ�ִ��ĳ������������֮���ٴ��жϣ��ͷ���
             ������
 �������  : *pst_lock:��������ַ
             func������ָ���ַ
             *p_arg����������
             *pui_flags: �жϱ�־�Ĵ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32  oal_spin_lock_irq_exec(oal_spin_lock_stru *pst_lock, oal_irqlocked_func func, oal_void *p_arg, oal_uint *pui_flags)
{
    oal_uint32  ul_rslt;

    spin_lock_irqsave(&pst_lock->lock, *pui_flags);

    ul_rslt = func(p_arg);

    spin_unlock_irqrestore(&pst_lock->lock, *pui_flags);

    return ul_rslt;
}











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_spinlock.h */

