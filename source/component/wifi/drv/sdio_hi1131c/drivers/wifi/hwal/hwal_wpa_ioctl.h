/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hwal_ioctl.h
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

#ifndef __HWAL_IOCTL_H__
#define __HWAL_IOCTL_H__

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
#include "wal_main.h"
#include "mac_user.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HWAL_IOCTL_H
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
typedef oal_int32 (*hwal_ioctl_handler)(oal_int8 *puc_ifname, oal_void *p_buf);

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
oal_int32   hwal_ioctl_set_key(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_new_key(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_del_key(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_set_ap(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_change_beacon(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_send_mlme(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_send_eapol(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_set_mode(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_set_power(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_receive_eapol(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_enable_eapol(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_disable_eapol(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_get_addr(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_get_hw_feature(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_stop_ap(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_del_virtual_intf(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_set_ap_wps_p2p_ie(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_wpa_ioctl(oal_int8 *puc_ifname, hisi_ioctl_command_stru *pst_cmd);
oal_int32   hwal_ioctl_scan(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_disconnect(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_assoc(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_set_netdev(oal_int8 *puc_ifname, oal_void *p_buf);
oal_uint8   hwal_is_valid_ie_attr(oal_uint8 *puc_ie, oal_uint32 ul_ie_len);
oal_int32   hwal_ioctl_dhcp_start(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_dhcp_stop(oal_int8 *puc_ifname, oal_void *p_buf);
oal_int32   hwal_ioctl_dhcp_succ_check(oal_int8 *puc_ifname, oal_void *p_buf);

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
oal_int32           hwal_ioctl_set_rekey_info(oal_int8 *puc_ifname, oal_void *p_buf);
extern oal_int32    wal_cfg80211_set_rekey_info(oal_net_device_stru *pst_net_dev, mac_rekey_offload_stru *pst_rekey_offload);
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwal_ioctl.h */

