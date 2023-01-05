/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : heartbeat_host.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 
  ����޸�   :
  ��������   : 
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/


#include <linux/slab.h>
#include "heartbeat_host.h"
#include "plat_debug.h"
#include "plat_pm.h"
#include "oal_sdio_host_if.h"
#include "exception_rst.h"
#include "oal_timer.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HEARTBEAT_HOST_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
#define BEAT_TIME_MESC             (2000)      /* ������ʱʱ��Ϊ2����*/

typedef struct _heart_beat
{
    struct timer_list timer;

    #define HEARTBEATEN     (1)
    #define HEARTBEATDIS    (0)
    int32 heartbeat_en;
    
}heart_beat_stru;

heart_beat_stru *g_pst_heartbeat = NULL;

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

void set_heartbeat_cfg(int32 cfg)
{
    if(NULL == g_pst_heartbeat)
    {
        PS_PRINT_ERR("g_pst_heartbeat is  NULL\r\n");
        return;
    }
    g_pst_heartbeat->heartbeat_en = cfg?HEARTBEATEN:HEARTBEATDIS;
}

int32 start_heartbeat(void)
{
    if(NULL == g_pst_heartbeat)
    {
        PS_PRINT_ERR("g_pst_heartbeat is  NULL\r\n");
        return -EINVAL;
    }
    
    PS_PRINT_INFO("add beat timer\n");
    //mod_timer(&(g_pst_heartbeat->timer), jiffies + BEAT_TIME * HZ);
    oal_timer_start(&(g_pst_heartbeat->timer), BEAT_TIME_MESC);
    g_pst_heartbeat->heartbeat_en = HEARTBEATEN;
    return SUCC;
}

int32 stop_heartbeat (void)
{
    if(NULL == g_pst_heartbeat)
    {
        PS_PRINT_ERR("g_pst_heartbeat is  NULL\r\n");
        return -EINVAL;
    }
    
    PS_PRINT_INFO("delete beat timer\n");
    g_pst_heartbeat->heartbeat_en = HEARTBEATDIS;
    //del_timer_sync(&(g_pst_heartbeat->timer));
    oal_timer_delete_sync(&(g_pst_heartbeat->timer));
    return SUCC;
}

int32 update_heartbeat(void)
{
    if( (NULL == g_pst_heartbeat) || (HEARTBEATDIS == g_pst_heartbeat->heartbeat_en) )
    {
        PS_PRINT_ERR("g_pst_heartbeat is  NULL, or HEARTBEATDIS == g_pst_heartbeat->heartbeat_en\r\n");
        return -EINVAL;
    }
        
 //   PS_PRINT_DBG("update beat timer\n");
    //mod_timer(&(g_pst_heartbeat->timer), jiffies + BEAT_TIME * HZ);
    oal_timer_start(&(g_pst_heartbeat->timer), BEAT_TIME_MESC);
    return SUCC;
}

    
/***********************************************************************************
 �� �� ��  : heartbeat_expire_func
 ��������  : ������ʱ���������ú������������ж��������У�����������˯�ߵĲ���
 �������  : uint64 data������Ҫ��������Ϊ�������ں˵ĺ�������
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 
    ��    ��   : 
    �޸�����   : �����ɺ���

***********************************************************************************/
static void heartbeat_expire_func(uint64 data)
{
    OAL_REFERENCE(data);
    
    PS_PRINT_ERR("enter heartbeat_expire_func\r\n");
    if(NULL == g_pst_heartbeat)
    {
        PS_PRINT_ERR("g_pst_heartbeat is NULL\r\n");
        return;
    }
    
    if(HEARTBEATDIS == g_pst_heartbeat->heartbeat_en)
    {
        PS_PRINT_WARNING("g_pst_heartbeat->heartbeat_en disable\r\n");
        update_heartbeat();
        return;
    }

    /*�µ�ʱ��û��������Ϣ�ϱ�*/
    if (wlan_is_shutdown())
    {
        PS_PRINT_INFO("device has power off!\r\n");
        return;
    }

    /*˯��ʱ��û��������Ϣ�ϱ�*/
    if (HOST_ALLOW_TO_SLEEP == wlan_pm_state_get())
    {
        PS_PRINT_INFO("device has sleep!\r\n");
        return;
    }

    //  todo...    heartbeat exception
    //  in soft interrupt, cannot sleep
    oal_exception_submit(TRANS_FAIL);
    
    return;
}


int32 heart_beat_init(void)
{
    if(NULL != g_pst_heartbeat)
    {
        PS_PRINT_ERR("g_pst_heartbeat is not NULL\r\n");
        return -EINVAL;
    }
    g_pst_heartbeat = kzalloc(sizeof(heart_beat_stru), GFP_KERNEL);
    if(NULL == g_pst_heartbeat)
    {
        PS_PRINT_ERR("malloc fail, g_pst_heartbeat is NULL\r\n");
        return -ENOMEM;
    }

    //init_timer( &(g_pst_heartbeat->timer) );
    //(g_pst_heartbeat->timer).function = heartbeat_expire_func;
    //(g_pst_heartbeat->timer).expires  = jiffies + BEAT_TIME*HZ;
    //(g_pst_heartbeat->timer).data     = 0;
    
    oal_timer_init(&(g_pst_heartbeat->timer), BEAT_TIME_MESC, heartbeat_expire_func, 0);

    g_pst_heartbeat->heartbeat_en = HEARTBEATDIS;
    return SUCC;
}

void heart_beat_release(void)
{
    if(NULL == g_pst_heartbeat)
    {
        PS_PRINT_WARNING("g_pst_heartbeat is  NULL\r\n");
        return;
    }
    stop_heartbeat();
    kfree(g_pst_heartbeat);
    g_pst_heartbeat = NULL;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

