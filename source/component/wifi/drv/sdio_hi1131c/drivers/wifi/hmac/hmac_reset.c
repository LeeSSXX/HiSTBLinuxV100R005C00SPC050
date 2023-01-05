/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hmac_reset.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2014��11��26��
  ����޸�   :
  ��������   : hmac��λ�����ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��11��26��
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
#include "mac_device.h"
#include "mac_resource.h"
#include "hmac_vap.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_RESET_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
extern oal_uint32  hmac_config_send_event(
                    mac_vap_stru                     *pst_mac_vap,
                    wlan_cfgid_enum_uint16            en_cfg_id,
                    oal_uint16                        us_len,
                    oal_uint8                        *puc_param);

#endif
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/*****************************************************************************
 �� �� ��  : hmac_reset_sys_event
 ��������  : device���и�λʱ��ͬ��host�¼�����
 �������  : pst_event_mem�¼�
 �������  : ��
 �� �� ֵ  : OAL_SUCC | OAL_ERR_CODE_PTR_NULL
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��26��
    ��    ��   :  00241943
    �޸�����   : �����ɺ���
*****************************************************************************/
oal_uint32 hmac_reset_sys_event(mac_vap_stru *pst_mac_vap, oal_uint8 uc_len, oal_uint8 *puc_param)
{
    mac_device_stru     *pst_mac_dev;
    mac_reset_sys_stru  *pst_reset_sys;

    pst_reset_sys = (mac_reset_sys_stru *)puc_param;

    pst_mac_dev = mac_res_get_dev(pst_mac_vap->uc_device_id);

    if (OAL_PTR_NULL == pst_mac_dev)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_reset_sys_event::pst_mac_dev null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    switch(pst_reset_sys->en_reset_sys_type)
    {
        case MAC_RESET_STATUS_SYS_TYPE:
            if ((pst_mac_dev->uc_device_reset_in_progress == OAL_TRUE)
               && (pst_reset_sys->uc_value == OAL_FALSE))
            {
                pst_mac_dev->us_device_reset_num++;
            }

#if 0
            pst_mac_dev->uc_device_reset_in_progress = pst_reset_sys->uc_value;
#else
            hmac_config_set_reset_state(pst_mac_vap, uc_len, puc_param);
#endif

            break;
        case MAC_RESET_SWITCH_SYS_TYPE:
            pst_mac_dev->en_reset_switch = pst_reset_sys->uc_value;

            break;
        default:
            break;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : hmac_proc_query_response_event
 ��������  : hmac����dmac�׻����Ĳ�ѯӦ���¼�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��26��
    ��    ��   : z0085449
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  hmac_proc_query_response_event(mac_vap_stru *pst_mac_vap, oal_uint8 uc_len, oal_uint8 *puc_param)
{
    hmac_vap_stru       *pst_hmac_vap;
    dmac_query_station_info_response_event  *pst_query_station_reponse_event;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_query_response::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_query_station_reponse_event = (dmac_query_station_info_response_event *)(puc_param);
    if(OAL_QUERY_STATION_INFO_EVENT == pst_query_station_reponse_event->query_event)
    {
        pst_hmac_vap->station_info.signal     =  (oal_int8)pst_query_station_reponse_event->ul_signal;
        pst_hmac_vap->station_info.rx_packets =  pst_query_station_reponse_event->ul_rx_packets;
        pst_hmac_vap->station_info.tx_packets =  pst_query_station_reponse_event->ul_tx_packets;
        pst_hmac_vap->station_info.rx_bytes   =  pst_query_station_reponse_event->ul_rx_bytes;
        pst_hmac_vap->station_info.tx_bytes   =  pst_query_station_reponse_event->ul_tx_bytes;
        pst_hmac_vap->station_info.tx_retries =  pst_query_station_reponse_event->ul_tx_retries;
        pst_hmac_vap->station_info.rx_dropped_misc =  pst_hmac_vap->station_info.rx_dropped_misc + pst_query_station_reponse_event->ul_rx_dropped_misc;
        pst_hmac_vap->station_info.tx_failed       =  pst_query_station_reponse_event->ul_tx_failed;
        pst_hmac_vap->station_info.txrate          =  pst_query_station_reponse_event->st_txrate;
        pst_hmac_vap->center_freq  = oal_ieee80211_channel_to_frequency(pst_mac_vap->st_channel.uc_chan_number,(enum ieee80211_band)pst_mac_vap->st_channel.en_band);
        pst_hmac_vap->s_free_power = pst_query_station_reponse_event->s_free_power;
    }

   /* ����wal_sdt_recv_reg_cmd�ȴ��Ľ��� */
   pst_hmac_vap->station_info_query_completed_flag = OAL_TRUE;
   OAL_WAIT_QUEUE_WAKE_UP_INTERRUPT(&(pst_hmac_vap->query_wait_q));

   return OAL_SUCC;
}
#endif

/*****************************************************************************
 �� �� ��  : hmac_config_reset_operate
 ��������  : ���������·�
 �������  : pst_event_mem�¼�
 �������  : ��
 �� �� ֵ  : OAL_SUCC | OAL_ERR_CODE_PTR_NULL
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��26��
    ��    ��   :  00241943
    �޸�����   : �����ɺ���
*****************************************************************************/
oal_uint32 hmac_config_reset_operate(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_device_stru      *pst_mac_dev;
    mac_reset_sys_stru    st_reset_sys;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_uint32            ul_ret;
#endif
    oal_int8             *pc_token;
    oal_int8             *pc_end;
    oal_int8             *pc_ctx;
    oal_int8             *pc_sep = " ";

    pst_mac_dev = mac_res_get_dev(pst_mac_vap->uc_device_id);

    if (OAL_PTR_NULL == pst_mac_dev)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_config_reset_operate::pst_mac_dev null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ��λ��Ϣ */
    pc_token = oal_strtok((oal_int8 *)puc_param, pc_sep, &pc_ctx);
    if (NULL == pc_token)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_reset_operate::pc_token null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    st_reset_sys.en_reset_sys_type = (mac_reset_sys_type_enum_uint8)oal_strtol(pc_token, &pc_end, 16);

    if (MAC_RESET_SWITCH_SET_TYPE == st_reset_sys.en_reset_sys_type)
    {
        /* ��ȡChannel List */
        pc_token = oal_strtok(OAL_PTR_NULL, pc_sep, &pc_ctx);
        if (NULL == pc_token)
        {
            OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_reset_operate::pc_token null.}");
            return OAL_ERR_CODE_PTR_NULL;
        }

        st_reset_sys.uc_value = (oal_uint8)oal_strtol(pc_token, &pc_end, 16);
        pst_mac_dev->en_reset_switch = st_reset_sys.uc_value;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_RESET_HW_OPERATE, us_len, (oal_uint8 *)&st_reset_sys);
        if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_reset_operate::hmac_config_send_event failed[%d].}", ul_ret);
        }

        return ul_ret;
#endif
    }
    else if (MAC_RESET_SWITCH_GET_TYPE == st_reset_sys.en_reset_sys_type)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_reset_operate::the reset switch is %d.}", pst_mac_dev->en_reset_switch);
    }
    else if (MAC_RESET_STATUS_GET_TYPE == st_reset_sys.en_reset_sys_type)
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_reset_operate::the reset status is %d, the reset num is %d.}",
                         pst_mac_dev->uc_device_reset_in_progress, pst_mac_dev->us_device_reset_num);
    }
    else
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_config_reset_operate::the reset tpye is %d, out of limit.}", st_reset_sys.en_reset_sys_type);
    }

    return OAL_SUCC;
}

/*lint -e19 */
oal_module_symbol(hmac_config_reset_operate);
/*lint +e19 */




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

