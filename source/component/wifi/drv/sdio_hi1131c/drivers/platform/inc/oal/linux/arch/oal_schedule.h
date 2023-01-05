/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_schedule.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2012��11��29��
  ����޸�   :
  ��������   : oal_types.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��11��29��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LINUX_SCHEDULE_H__
#define __OAL_LINUX_SCHEDULE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
/*lint -e322*/

#include <linux/interrupt.h>
#include <asm/param.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <asm/uaccess.h>



#include "oal_mutex.h"
#include "oal_semaphore.h"
#include "oal_task.h"
#include "oal_rw_lock.h"
#include "oal_spinlock.h"
#include "oal_wakelock.h"
#include "oal_timer.h"
#include "oal_time.h"
#include "oal_atomic.h"
#include "oal_wait.h"
#include "oal_completion.h"
#include "oal_mm.h"

/*lint +e322*/

/*****************************************************************************
  2 �궨��
*****************************************************************************/



/**
 * wait_event_interruptible_timeout - sleep until a condition gets true or a timeout elapses
 * @_wq: the waitqueue to wait on
 * @_condition: a C expression for the event to wait for
 * @_timeout: timeout, in jiffies
 *
 * The process is put to sleep (TASK_INTERRUPTIBLE) until the
 * @condition evaluates to true or a signal is received.
 * The @condition is checked each time the waitqueue @wq is woken up.
 *
 * wake_up() has to be called after changing any variable that could
 * change the result of the wait condition.
 *
 * The function returns 0 if the @timeout elapsed, -ERESTARTSYS if it
 * was interrupted by a signal, and the remaining jiffies otherwise
 * if the condition evaluated to true before the timeout elapsed.
 */





#define oal_in_interrupt()  in_interrupt()

#define oal_in_atomic()     in_atomic()


typedef oal_uint32 (*oal_module_func_t)(oal_void);

/* ģ����� */
#define oal_module_init(_module_name)   module_init(_module_name)

#define oal_module_license(_license_name) MODULE_LICENSE(_license_name)

#define oal_module_param    module_param

#define OAL_S_IRUGO         S_IRUGO

/* ģ����� */
#define oal_module_exit(_module_name)   module_exit(_module_name)

/* ģ����ŵ��� */
#define oal_module_symbol(_symbol)      EXPORT_SYMBOL(_symbol)
#define OAL_MODULE_DEVICE_TABLE(_type, _name) MODULE_DEVICE_TABLE(_type, _name)

#ifdef HI1131C_SDIO_DETECT_SUPPORT/*sdio_detect.ko not need export symbol*/
    #define OAL_EXPORT_SYMBOL(_symbol)
#else
    #define OAL_EXPORT_SYMBOL(_symbol)      EXPORT_SYMBOL(_symbol)
#endif

#define oal_smp_call_function_single(core, task, info, wait) smp_call_function_single(core, task, info, wait)

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
typedef struct proc_dir_entry       oal_proc_dir_entry_stru;



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
 �� �� ��  : oal_copy_from_user
 ��������  : ���û�̬���ݿ������ں�̬
 �������  : p_to: Ŀ�ĵ�
             p_from : Դ
             ul_size : ��Ҫ�����ĳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32  oal_copy_from_user(oal_void *p_to, const oal_void *p_from, oal_uint32 ul_size)
{
    return (oal_uint32)copy_from_user(p_to, p_from, (oal_uint)ul_size);
}

/*****************************************************************************
 �� �� ��  : oal_copy_to_user
 ��������  : ���ں�̬���ݿ������û�̬
 �������  : p_to: Ŀ�ĵ�
             p_from : Դ
             ul_size : ��Ҫ�����ĳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��27��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32  oal_copy_to_user(oal_void *p_to, const oal_void *p_from, oal_uint32 ul_size)
{
    return (oal_uint32)copy_to_user(p_to, p_from, (oal_uint)ul_size);
}

/*****************************************************************************
 �� �� ��  : oal_create_proc_entry
 ��������  : ����proc_entry�ṹ��
 �������  : pc_name: ������proc_entry������
             us_mode: ����ģʽ
             pst_parent: ĸproc_entry�ṹ�壬�̳�����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_proc_dir_entry_stru* oal_create_proc_entry(const oal_int8 *pc_name, oal_uint16 us_mode, oal_proc_dir_entry_stru *pst_parent)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))
    return NULL;
#else
    return create_proc_entry(pc_name, us_mode, pst_parent);
#endif
}

/*****************************************************************************
 �� �� ��  : oal_remove_proc_entry
 ��������  : ����proc_entry�ṹ��
 �������  : pc_name: ������proc_entry������
             pst_parent: ĸproc_entry�ṹ�壬�̳�����
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE void oal_remove_proc_entry(const oal_int8 *pc_name, oal_proc_dir_entry_stru *pst_parent)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))
#else
    return remove_proc_entry(pc_name, pst_parent);
#endif
}



#ifdef _PRE_OAL_FEATURE_TASK_NEST_LOCK
/*****************************************************************************
 �� �� ��  : oal_smp_task_lock
 ��������  : lock the task, the lock can be double locked by the same process
 �������  : 
             
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
extern oal_void _oal_smp_task_lock_(oal_task_lock_stru* pst_lock,oal_ulong  claim_addr);
#define oal_smp_task_lock(lock)    _oal_smp_task_lock_(lock, (oal_ulong)_THIS_IP_)

/*****************************************************************************
 �� �� ��  : oal_smp_task_unlock
 ��������  : unlock the task
 �������  : 
             
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void oal_smp_task_unlock(oal_task_lock_stru* pst_lock)
{
	oal_ulong flags;

	if(OAL_WARN_ON(in_interrupt() || in_atomic()))
	{
	    return;
	}

    if(OAL_UNLIKELY(!pst_lock->claimed))
    {
        OAL_WARN_ON(1);
        return;
    }

    oal_spin_lock_irq_save(&pst_lock->lock, &flags);
	if(--pst_lock->claim_cnt)
	{
	    oal_spin_unlock_irq_restore(&pst_lock->lock, &flags);
	}
	else
	{
		pst_lock->claimed = 0;
		pst_lock->claimer = NULL;
		oal_spin_unlock_irq_restore(&pst_lock->lock, &flags);
		wake_up(&pst_lock->wq);
	}
}

/*****************************************************************************
 �� �� ��  : oal_smp_task_lock_init
 ��������  : task struct init
 �������  : 
             
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void oal_smp_task_lock_init(oal_task_lock_stru* pst_lock)
{
    oal_memset((oal_void*)pst_lock,0,sizeof(oal_task_lock_stru));

	oal_spin_lock_init(&pst_lock->lock);
	OAL_WAIT_QUEUE_INIT_HEAD(&pst_lock->wq);
	pst_lock->claimed = 0;
	pst_lock->claim_cnt = 0;
}
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_schedule.h */
