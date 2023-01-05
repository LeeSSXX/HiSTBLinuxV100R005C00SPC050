/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_thread.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2014��1��29��
  ����޸�   :
  ��������   : oal_thread.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��10��13��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LITEOS_THREAD_H__
#define __OAL_LITEOS_THREAD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include <los_task.h>

/*****************************************************************************
  2 STRUCT����
*****************************************************************************/

typedef  LOS_TASK_CB oal_kthread_stru; 

typedef struct _kthread_param_{
    oal_uint32         ul_stacksize;
    oal_int32           l_prio;
    oal_int32           l_policy;
    oal_int32           l_cpuid;
    oal_int32           l_nice;
}oal_kthread_param_stru;

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
#define OAL_CURRENT     oal_get_current()

/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 �궨��
*****************************************************************************/
#define OAL_SCHED_FIFO      (1)
#define OAL_SCHED_RR        (2)

#define NOT_BIND_CPU        (-1)

/*****************************************************************************
  8 UNION����
*****************************************************************************/

/*****************************************************************************
  9 OTHERS����
*****************************************************************************/
typedef int (*oal_thread_func)(void *);

/*****************************************************************************
  10 ��������
*****************************************************************************/

OAL_STATIC OAL_INLINE oal_kthread_stru* oal_kthread_create(char                   *pc_thread_name,
                                                           oal_thread_func        pf_threadfn,
                                                           void                   *p_data,
                                                           oal_kthread_param_stru *pst_thread_param)
{
    oal_int32 uwRet;
    TSK_INIT_PARAM_S stSdTask;
    oal_uint32 ul_taskid;
    oal_kthread_stru *pst_kthread=NULL;

    memset(&stSdTask, 0, sizeof(TSK_INIT_PARAM_S));
    stSdTask.pfnTaskEntry  = (TSK_ENTRY_FUNC)pf_threadfn;
    stSdTask.auwArgs[0]    = (oal_int32)p_data;
    stSdTask.uwStackSize   = pst_thread_param->ul_stacksize;
    stSdTask.pcName        = pc_thread_name;    
    stSdTask.usTaskPrio    = (oal_uint16)pst_thread_param->l_prio;
    stSdTask.uwResved      = LOS_TASK_STATUS_DETACHED;
    
    uwRet = LOS_TaskCreate(&ul_taskid, &stSdTask);
    if (LOS_OK != uwRet)
    {
        dprintf("Failed to create %s thread\n",pc_thread_name);
        return NULL;
    }

    pst_kthread = (oal_kthread_stru *)&g_pstTaskCBArray[ul_taskid];
    return pst_kthread;
}


OAL_STATIC OAL_INLINE oal_void oal_kthread_stop(oal_kthread_stru *pst_kthread)
{
    if (OAL_UNLIKELY(pst_kthread == NULL))
    {
        dprintf("thread can't stop\n");
        return;
    }
    dprintf(">>>>>>>>>%s thread stop\n",pst_kthread->pcTaskName);
    LOS_TaskDelete(pst_kthread->uwTaskPID);
}

OAL_STATIC OAL_INLINE  oal_int32 oal_kthread_should_stop(oal_void)
{
    return 0;
}

OAL_STATIC OAL_INLINE oal_uint32 oal_set_thread_property(oal_kthread_stru        *pst_thread,
                                                         oal_kthread_param_stru  *pst_thread_param)
{
    return LOS_TaskPriSet(pst_thread->uwTaskPID,pst_thread_param->l_prio);
}

OAL_STATIC OAL_INLINE oal_kthread_stru *oal_get_current(oal_void)
{
    return g_stLosTask.pstRunTask;
}

OAL_INLINE static char* oal_get_current_task_name(oal_void)
{
      return g_stLosTask.pstRunTask->pcTaskName;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
