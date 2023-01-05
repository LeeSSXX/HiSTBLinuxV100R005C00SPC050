/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hwal_ioctl.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��6��16��
  ����޸�   :
  ��������   : hwal ioctl�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��6��16��
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
#include "hwal_event.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HWAL_EVENT_C


/*****************************************************************************
  2 �ⲿ��������
*****************************************************************************/
#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
	oal_uint32 hilink_rx_data(oal_uint8 *puc_data, oal_uint32 ul_len){}
#else
#ifdef _PRE_WLAN_FEATURE_HILINK
extern oal_uint32 hilink_rx_data(oal_uint8 *puc_data, oal_uint32 ul_len);
#endif
#endif
/*****************************************************************************
  3 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : cfg80211_new_sta
 ��������  : �ϱ�new station�¼��ӿ�
 �������  : oal_net_device_stru *pst_dev, const oal_uint8 *mac_addr,
             oal_station_info_stru *sinfo, oal_gfp_enum_uint8 gfp
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void cfg80211_new_sta(oal_net_device_stru *pst_dev, const oal_uint8 *puc_macaddr,
              oal_station_info_stru *pst_sinfo, oal_gfp_enum_uint8 en_gfp)
{
    hisi_new_sta_info_stru st_new_sta_info;

    oal_memset(&st_new_sta_info, 0, OAL_SIZEOF(hisi_new_sta_info_stru));

    /* ��μ�� */
    if ((OAL_PTR_NULL == pst_dev) || (OAL_PTR_NULL == puc_macaddr) || (OAL_PTR_NULL == pst_sinfo))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{cfg80211_new_sta::pst_dev or puc_macaddr or pst_sinfo null ptr error %x,%x,%x.}\r\n", pst_dev, puc_macaddr, pst_sinfo);
        return;
    }

    if ((OAL_PTR_NULL == pst_sinfo->assoc_req_ies) || (0 == pst_sinfo->assoc_req_ies_len))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{cfg80211_new_sta::assoc_req_ies or assoc_req_ies_len null error %x,%d.}\r\n", pst_sinfo->assoc_req_ies, pst_sinfo->assoc_req_ies_len);
        return;
    }

    st_new_sta_info.puc_ie    = OAL_PTR_NULL;
    st_new_sta_info.ul_ielen  = pst_sinfo->assoc_req_ies_len;
    st_new_sta_info.l_reassoc = 0;
    oal_memcopy(st_new_sta_info.auc_macaddr, puc_macaddr, ETH_ALEN);

    st_new_sta_info.puc_ie = oal_memalloc(pst_sinfo->assoc_req_ies_len);
    if(OAL_PTR_NULL == st_new_sta_info.puc_ie)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{cfg80211_new_sta::pst_new_sta_info->puc_ie oal_memalloc error %x.}\r\n", st_new_sta_info.puc_ie);
        return;
    }
    oal_memcopy(st_new_sta_info.puc_ie, pst_sinfo->assoc_req_ies, pst_sinfo->assoc_req_ies_len);

    hisi_driver_send_event(HISI_ELOOP_EVENT_NEW_STA, &st_new_sta_info, OAL_SIZEOF(hisi_new_sta_info_stru));
    return;
}

/*****************************************************************************
 �� �� ��  : cfg80211_del_sta
 ��������  : �ϱ�new station�¼��ӿ�
 �������  : oal_net_device_stru *pst_dev, const oal_uint8 *puc_mac_addr,
            oal_gfp_enum_uint8 en_gfp
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void cfg80211_del_sta(oal_net_device_stru *pst_dev, const oal_uint8 *puc_mac_addr,
                                oal_gfp_enum_uint8 en_gfp)
{
    /* ��μ�� */
    if ((OAL_PTR_NULL == pst_dev) || (OAL_PTR_NULL == puc_mac_addr))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{cfg80211_del_sta::pst_dev or puc_mac_addr ptr NULL %x,%x.}\r\n", pst_dev, puc_mac_addr);
        return;
    }

    hisi_driver_send_event(HISI_ELOOP_EVENT_DEL_STA, puc_mac_addr, ETH_ADDR_LEN);
    return;
}

/*****************************************************************************
 �� �� ��  : cfg80211_rx_mgmt
 ��������  : �ϱ�����֡���ݽӿ�
 �������  : struct wireless_dev *wdev, oal_int32 freq, oal_int32 sig_mbm,
             const oal_uint8 *buf, size_t len, oal_gfp_enum_uint8 gfp
 �������  : ��
 �� �� ֵ  : bool
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_bool_enum_uint8 cfg80211_rx_mgmt(oal_wireless_dev_stru *pst_wdev, oal_int32 l_freq, oal_int32 l_sig_mbm,
              const oal_uint8 *puc_buf, size_t ul_len, oal_gfp_enum_uint8 en_gfp)
{
    hisi_rx_mgmt_stru st_rx_mgmt;

    oal_memset(&st_rx_mgmt, 0, OAL_SIZEOF(hisi_rx_mgmt_stru));
    /* ��μ�� */
    if((OAL_PTR_NULL == pst_wdev) || (OAL_PTR_NULL == puc_buf))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::pst_wdev or puc_buf ptr NULL %x,%x.}\r\n", pst_wdev, puc_buf);
        return OAL_FALSE;
    }

    st_rx_mgmt.puc_buf    = OAL_PTR_NULL;
    st_rx_mgmt.ul_len      = ul_len;
    st_rx_mgmt.l_sig_mbm   = l_sig_mbm;

    if (0 != ul_len)
    {
        st_rx_mgmt.puc_buf = oal_memalloc(ul_len);
        if(OAL_PTR_NULL == st_rx_mgmt.puc_buf)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{cfg80211_rx_mgmt::pst_rx_mgmt->puc_buf oal_memalloc error %x.}\r\n", st_rx_mgmt.puc_buf);
            return OAL_FALSE;
        }
        oal_memcopy(st_rx_mgmt.puc_buf, puc_buf, ul_len);
    }

    hisi_driver_send_event(HISI_ELOOP_EVENT_RX_MGMT, &st_rx_mgmt, OAL_SIZEOF(hisi_rx_mgmt_stru));
    return OAL_TRUE;
}

/*****************************************************************************
 �� �� ��  : cfg80211_mgmt_tx_status
 ��������  : �ϱ�����״̬�ӿ�
 �������  : struct wireless_dev *wdev, oal_uint64 cookie,
             const oal_uint8 *buf, size_t len, bool ack, oal_gfp_enum_uint8 gfp
 �������  : ��
 �� �� ֵ  : bool
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_bool_enum_uint8 cfg80211_mgmt_tx_status(struct wireless_dev *pst_wdev, oal_uint64 ull_cookie,
                 const oal_uint8 *puc_buf, size_t ul_len, oal_bool_enum_uint8 en_ack, oal_gfp_enum_uint8 en_gfp)
{
    hisi_tx_status_stru st_tx_status;

    oal_memset(&st_tx_status, 0, OAL_SIZEOF(hisi_tx_status_stru));
    /* ��μ�� */
    if ((OAL_PTR_NULL == pst_wdev) || (OAL_PTR_NULL == puc_buf))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::pst_wdev or puc_buf ptr NULL %x,%x.}\r\n", pst_wdev, puc_buf);
        return OAL_FALSE;
    }

    st_tx_status.puc_buf  = OAL_PTR_NULL;
    st_tx_status.ul_len    = ul_len;
    st_tx_status.en_ack    = en_ack;

    if (0 != ul_len)
    {
        st_tx_status.puc_buf = oal_memalloc(ul_len);
        if (OAL_PTR_NULL == st_tx_status.puc_buf)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{cfg80211_mgmt_tx_status::pst_tx_status->puc_buf oal_memalloc error %x.}\r\n", st_tx_status.puc_buf);
            return OAL_FALSE;
        }
        oal_memcopy(st_tx_status.puc_buf, puc_buf, ul_len);
    }

    hisi_driver_send_event(HISI_ELOOP_EVENT_TX_STATUS, &st_tx_status, OAL_SIZEOF(hisi_tx_status_stru));
    return OAL_TRUE;
}

/*****************************************************************************
 �� �� ��  : cfg80211_inform_bss_frame
 ��������  : ɨ�����ϱ��ӿ�
 �������  :

 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_cfg80211_bss_stru *cfg80211_inform_bss_frame(
                                            oal_wiphy_stru              *pst_wiphy,
                                            oal_ieee80211_channel_stru  *pst_ieee80211_channel,
                                            oal_ieee80211_mgmt_stru     *pst_mgmt,
                                            oal_uint32                   ul_len,
                                            oal_int32                    l_signal,
                                            oal_gfp_enum_uint8           en_ftp)
{
    hisi_scan_result_stru  st_scan_result;
    oal_uint32             ul_ie_len;
    oal_uint32             ul_beacon_ie_len;

    oal_memset(&st_scan_result, 0, OAL_SIZEOF(hisi_scan_result_stru));

    if ((OAL_PTR_NULL == pst_wiphy) || (OAL_PTR_NULL == pst_ieee80211_channel) || (OAL_PTR_NULL == pst_mgmt))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::pst_wiphy or pst_ieee80211_channel or pst_mgmt null ptr error %x,%x,%x.}\r\n", pst_wiphy, pst_ieee80211_channel, pst_mgmt);
        return OAL_PTR_NULL;
    }

    ul_ie_len           = ul_len - offsetof(oal_ieee80211_mgmt_stru, u.probe_resp.variable);
    ul_beacon_ie_len    = ul_len - offsetof(oal_ieee80211_mgmt_stru, u.beacon.variable);

    st_scan_result.puc_variable = oal_memalloc(ul_ie_len + ul_beacon_ie_len);
    if (OAL_PTR_NULL == st_scan_result.puc_variable)
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{cfg80211_inform_bss_frame::pst_scan_result os_zalloc error.ul_ie_len = %d ul_beacon_ie_len = %d}\r\n", ul_ie_len, ul_beacon_ie_len);
        return OAL_PTR_NULL;
    }

    oal_memset(st_scan_result.puc_variable, 0, ul_ie_len);
    st_scan_result.ul_ie_len          = ul_ie_len;
    st_scan_result.ul_beacon_ie_len   = ul_beacon_ie_len;
    st_scan_result.s_beacon_int       = pst_mgmt->u.probe_resp.beacon_int;
    st_scan_result.s_caps             = pst_mgmt->u.probe_resp.capab_info;
    st_scan_result.l_level            = l_signal;
    st_scan_result.l_freq             = pst_ieee80211_channel->center_freq;
    st_scan_result.l_flags            = pst_ieee80211_channel->flags;
    st_scan_result.ull_tsf            = pst_mgmt->u.probe_resp.timestamp;

    oal_memcopy(st_scan_result.auc_bssid, pst_mgmt->bssid, ETH_ALEN);
    oal_memcopy(st_scan_result.puc_variable, pst_mgmt->u.probe_resp.variable, ul_ie_len);
    oal_memcopy(st_scan_result.puc_variable + ul_ie_len, pst_mgmt->u.beacon.variable, ul_beacon_ie_len);

    hisi_driver_send_event(HISI_ELOOP_EVENT_SCAN_RESULT, &st_scan_result, OAL_SIZEOF(hisi_scan_result_stru));

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : cfg80211_connect_result
 ��������  : ��������ϱ��ӿ�
 �������  :

 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void cfg80211_connect_result(oal_net_device_stru*pst_dev, const oal_uint8 *puc_bssid,
                 const oal_uint8 *puc_req_ie, oal_size_t ul_req_ie_len,
                 const oal_uint8 *puc_resp_ie, oal_size_t ul_resp_ie_len,
                 oal_uint16 us_status, oal_gfp_enum_uint8 en_gfp)
{
    hisi_connect_result_stru st_connect_result;

    oal_memset(&st_connect_result, 0, OAL_SIZEOF(hisi_connect_result_stru));

    if (OAL_PTR_NULL == pst_dev)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{cfg80211_connect_result::pst_dev null ptr error %x.}\r\n", pst_dev);
        return;
    }

    if (OAL_PTR_NULL != puc_bssid)
    {
        oal_memcopy(st_connect_result.auc_bssid, puc_bssid, ETH_ALEN);
    }

    if ((puc_req_ie != OAL_PTR_NULL) && (ul_req_ie_len != 0))
    {
        st_connect_result.puc_req_ie = oal_memalloc(ul_req_ie_len);
        st_connect_result.ul_req_ie_len = ul_req_ie_len;
        if (OAL_PTR_NULL == st_connect_result.puc_req_ie)
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{cfg80211_connect_result::pst_connect_result->puc_req_ie os_zalloc error.}\r\n");
            return;
        }
        oal_memcopy(st_connect_result.puc_req_ie, puc_req_ie, ul_req_ie_len);
    }

    if ((puc_resp_ie != OAL_PTR_NULL) && (ul_resp_ie_len != 0))
    {
        st_connect_result.puc_resp_ie = oal_memalloc(ul_resp_ie_len);
        st_connect_result.ul_resp_ie_len    = ul_resp_ie_len;
        if (OAL_PTR_NULL == st_connect_result.puc_resp_ie)
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{cfg80211_connect_result::pst_connect_result->puc_resp_ie os_zalloc error.}\r\n");
            oal_free(st_connect_result.puc_req_ie);
            return;
        }
        oal_memcopy(st_connect_result.puc_resp_ie, puc_resp_ie, ul_resp_ie_len);
    }

    st_connect_result.us_status = us_status;

    hisi_driver_send_event(HISI_ELOOP_EVENT_CONNECT_RESULT, &st_connect_result, OAL_SIZEOF(hisi_connect_result_stru));
    return;

}

/*****************************************************************************
 �� �� ��  : cfg80211_inform_bss_frame
 ��������  : ȥ�����ϱ��ӿ�
 �������  :

 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  cfg80211_disconnected(
                    oal_net_device_stru        *pst_net_device,
                    oal_uint16                  us_reason,
                    oal_uint8                  *puc_ie,
                    oal_uint32                  ul_ie_len,
                    oal_gfp_enum_uint8          en_gfp)
 {
    hisi_disconnect_stru st_disconnect;

    oal_memset(&st_disconnect, 0, OAL_SIZEOF(hisi_disconnect_stru));

    if ((OAL_PTR_NULL == pst_net_device))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{cfg80211_disconnected::pst_net_device %x.}\r\n", pst_net_device);
        return OAL_FAIL;
    }

    if ((puc_ie != OAL_PTR_NULL) && (ul_ie_len != 0))
    {
        st_disconnect.puc_ie = oal_memalloc(ul_ie_len);
        st_disconnect.ul_ie_len = ul_ie_len;
        if (OAL_PTR_NULL == st_disconnect.puc_ie)
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{cfg80211_disconnected::pst_connect_result->puc_req_ie os_zalloc error.}\r\n");
            return OAL_FAIL;
        }
        oal_memcopy(st_disconnect.puc_ie, puc_ie, ul_ie_len);
    }

    st_disconnect.us_reason = us_reason;

    if (OAL_PTR_NULL != pst_net_device->pst_lwip_netif)
    {
        hwal_lwip_notify(pst_net_device->pst_lwip_netif, NETDEV_DOWN);
    }

    hisi_driver_send_event(HISI_ELOOP_EVENT_DISCONNECT, &st_disconnect, OAL_SIZEOF(hisi_disconnect_stru));
    return OAL_SUCC;
 }

/*****************************************************************************
 �� �� ��  : cfg80211_scan_done
 ��������  : ɨ������ϱ��ӿ�
 �������  :

 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void cfg80211_scan_done(oal_cfg80211_scan_request_stru *pst_request, oal_uint8 uc_aborted)
{
    hisi_driver_send_event(HISI_ELOOP_EVENT_SCAN_DONE, NULL, 0);
}


#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) || (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)

#ifdef _PRE_WLAN_FEATURE_HILINK
/*****************************************************************************
 �� �� ��  : hwal_send_others_bss_data
 ��������  : �ϱ�����BSS�������ݰ�
 �������  : pst_netbuf ���ݰ���netbufָ�롢���ݰ��ĳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��7��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 hwal_send_others_bss_data(oal_netbuf_stru *pst_netbuf)
{
    oal_uint8  *puc_data    = OAL_PTR_NULL;
    oal_uint32  ul_ret;
    oal_uint8  *puc_payload = OAL_PTR_NULL;
    oal_uint16  us_len;
    if (OAL_PTR_NULL == pst_netbuf)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hwal_send_others_bss_data::pst_netbuf is null");
        return OAL_ERR_CODE_PTR_NULL;
    }
    us_len = OAL_NETBUF_LEN(pst_netbuf);
    if (0 != us_len)
    {
        puc_data = oal_memalloc(us_len);
        if (OAL_PTR_NULL == puc_data)
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "hwal_send_others_bss_data::oal_memalloc fail");
            return OAL_FAIL;
        }
        puc_payload = OAL_NETBUF_PAYLOAD(pst_netbuf);
        oal_memcopy(puc_data, puc_payload, us_len);
        hilink_rx_data(puc_data, us_len);
    }
    oal_netbuf_free(pst_netbuf);
    return OAL_SUCC;
}
#endif//#ifdef _PRE_WLAN_FEATURE_HILINK
#endif //#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

