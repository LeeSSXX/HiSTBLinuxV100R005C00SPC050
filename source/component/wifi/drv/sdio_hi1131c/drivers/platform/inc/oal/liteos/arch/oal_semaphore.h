/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_semaphore.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2012��12��10��
  ����޸�   :
  ��������   : oal_semaphore.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��12��10��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LITEOS_SEMAPHORE_H__
#define __OAL_LITEOS_SEMAPHORE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include <los_sem.ph>
#include <los_sem.h>

/*****************************************************************************
  2 STRUCT����
*****************************************************************************/
typedef SEM_CB_S oal_semaphore_stru;

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
  7 �궨��
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

OAL_STATIC OAL_INLINE oal_void oal_sema_init(oal_semaphore_stru *pst_sem, oal_uint16 us_val)
{
    oal_uint32 ul_semhandle;
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return;
    }

    if (LOS_OK != LOS_SemCreate(us_val, &ul_semhandle))
    {
        OAL_IO_PRINT("LOS_SemCreate failed!\n");
        return;
    }
    *pst_sem = *(GET_SEM(ul_semhandle));    
}


OAL_STATIC OAL_INLINE oal_void oal_up(oal_semaphore_stru *pst_sem)
{
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return;
    }

    if (LOS_OK != LOS_SemPost(pst_sem->usSemID))
    {
        OAL_IO_PRINT("LOS_SemPost failed!\n");
        return;
    }    
}

OAL_STATIC OAL_INLINE oal_void oal_down(oal_semaphore_stru *pst_sem)
{   
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return;
    }

    if (LOS_OK != LOS_SemPend(pst_sem->usSemID, OS_WAIT_FOREVER))
    {
        OAL_IO_PRINT("LOS_SemPend failed!\n");
        return;
    }  
}

OAL_STATIC OAL_INLINE oal_int32 oal_down_timeout(oal_semaphore_stru *pst_sem,oal_uint32 ul_timeout)
{
    oal_uint32 ul_reval = 0;
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return -OAL_EFAIL;
    }

    ul_reval = LOS_SemPend(pst_sem->usSemID, ul_timeout);
    if (LOS_OK != ul_reval)
    {
        return ul_reval;
    }
    return OAL_SUCC;
}

OAL_STATIC OAL_INLINE oal_int32 oal_down_interruptible(oal_semaphore_stru *pst_sem)
{
    oal_uint32 ul_reval = 0;
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return -OAL_EFAIL;
    }

    ul_reval = LOS_SemPend(pst_sem->usSemID, OS_WAIT_FOREVER);
    if (LOS_OK != ul_reval)
    {
        return ul_reval;
    }
    return OAL_SUCC;
}

 OAL_STATIC OAL_INLINE oal_int32 oal_down_trylock(oal_semaphore_stru *pst_sem)
{
    oal_uint32 ul_reval = 0;
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return -OAL_EFAIL;
    }

    ul_reval = LOS_SemPend(pst_sem->usSemID, OS_NO_WAIT);
    if (LOS_OK != ul_reval)
    {
        return ul_reval;
    }
    return OAL_SUCC;
}

 OAL_STATIC OAL_INLINE oal_int32 oal_sema_destroy(oal_semaphore_stru *pst_sem)
 {
    oal_uint32 ul_reval = 0;
    if (NULL == pst_sem)
    {
        OAL_IO_PRINT("[%s]pst_sem is null!\n",__func__);
        return -OAL_EFAIL;
    }

    ul_reval = LOS_SemDelete(pst_sem->usSemID);
    if (LOS_OK != ul_reval)
    {
        return ul_reval;
    }
    return OAL_SUCC;
 }


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_semaphore.h */

