/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hwal_event.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��6��16��
  ����޸�   :
  ��������   : hwal_ioctl.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��6��16��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __HWAL_EVENT_H__
#define __HWAL_EVENT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "driver_hisi_common.h"
#include "oam_ext_if.h"
#include "oal_net.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HWAL_EVENT_H
/*****************************************************************************
  2 �궨��
*****************************************************************************/

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
oal_void            cfg80211_new_sta(oal_net_device_stru *pst_dev, const oal_uint8 *puc_macaddr, oal_station_info_stru *pst_sinfo, oal_gfp_enum_uint8 en_gfp);
oal_void            cfg80211_del_sta(oal_net_device_stru *pst_dev, const oal_uint8 *puc_mac_addr, oal_gfp_enum_uint8 en_gfp);
oal_bool_enum_uint8 cfg80211_rx_mgmt(oal_wireless_dev_stru *pst_wdev, oal_int32 l_freq, oal_int32 l_sig_mbm, const oal_uint8 *puc_buf, size_t ul_len, oal_gfp_enum_uint8 en_gfp);
oal_bool_enum_uint8 cfg80211_mgmt_tx_status(struct wireless_dev *pst_wdev, oal_uint64 ull_cookie, const oal_uint8 *puc_buf, size_t ul_len, oal_bool_enum_uint8 en_ack, oal_gfp_enum_uint8 en_gfp);
oal_cfg80211_bss_stru *cfg80211_inform_bss_frame(oal_wiphy_stru *pst_wiphy,oal_ieee80211_channel_stru *pst_ieee80211_channel,oal_ieee80211_mgmt_stru     *pst_mgmt,
                                                  oal_uint32 ul_len,oal_int32 l_signal,oal_gfp_enum_uint8 en_ftp);
oal_void            cfg80211_connect_result(oal_net_device_stru*pst_dev, const oal_uint8 *puc_bssid,const oal_uint8 *puc_req_ie, oal_size_t ul_req_ie_len,const oal_uint8 *puc_resp_ie, oal_size_t ul_resp_ie_len,
			     							oal_uint16 us_status, oal_gfp_enum_uint8 en_gfp);
oal_uint32          cfg80211_disconnected(oal_net_device_stru *pst_net_device,oal_uint16 us_reason,oal_uint8 *puc_ie,oal_uint32 ul_ie_len,oal_gfp_enum_uint8 en_gfp);
oal_void            cfg80211_scan_done(oal_cfg80211_scan_request_stru *pst_request, oal_uint8 uc_aborted);

#ifdef _PRE_WLAN_FEATURE_HILINK
oal_uint32 hwal_send_others_bss_data(oal_netbuf_stru *pst_netbuf);
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif

