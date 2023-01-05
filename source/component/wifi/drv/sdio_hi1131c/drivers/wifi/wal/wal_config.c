/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : wal_config.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2012��11��6��
  ����޸�   :
  ��������   : WAL���ò���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��11��6��
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
#include "oal_types.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "oal_util.h"
#include "wlan_types.h"

#include "mac_device.h"
#include "mac_vap.h"
#include "mac_resource.h"
//#include "mac_11i.h"

#include "hmac_resource.h"
#include "hmac_device.h"
#include "hmac_scan.h"
#include "hmac_ext_if.h"
#include "hmac_config.h"
#include "wal_ext_if.h"
#include "wal_main.h"
#include "wal_config.h"
#include "wal_linux_bridge.h"
#include "oam_ext_if.h"
#if(_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)

#include "hmac_cali_mgmt.h"
#endif

#ifdef _PRE_WLAN_CHIP_TEST
#include "hmac_test_main.h"
#endif

#ifdef _PRE_WLAN_DFT_REG
#include "hal_witp_debug.h"
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
#include "wal_proxysta.h"
#endif

#ifdef _PRE_WLAN_FEATURE_MCAST
#include "hmac_m2u.h"
#endif

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
#include "hmac_proxy_arp.h"
#endif

#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION))
#include "plat_pm_wlan.h"
#endif

#ifdef _PRE_WLAN_FEATURE_TCP_UDP_KEEPALIVE
#include "hmac_tcp_udp_keepalive.h"
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_CONFIG_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/* ��̬�������� */
#ifdef _PRE_WLAN_FEATURE_PM
OAL_STATIC oal_uint32  wal_config_wifi_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_pm_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_pm_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

OAL_STATIC oal_uint32  wal_config_add_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_del_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_down_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_start_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_bandwidth(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_get_fem_pa_status(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

OAL_STATIC oal_uint32  wal_config_set_mac_addr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_concurrent(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_concurrent(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_bss_type(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_bss_type(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_ssid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_ssid(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_shortgi20(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_shortgi20(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_shortgi40(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_shortgi40(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_shortgi80(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_shortgi80(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_shpreamble(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_shpreamble(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_MONITOR
OAL_STATIC oal_uint32  wal_config_get_addr_filter(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_addr_filter(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_get_prot_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_prot_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_auth_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_auth_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_bintval(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_bintval(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_nobeacon(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_nobeacon(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_txchain(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_txchain(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_rxchain(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_rxchain(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_txpower(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_txpower(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_freq(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_freq(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_wmm_params(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_vap_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_event_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_eth_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_80211_ucast_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_DEBUG_MODE_USER_TRACK
OAL_STATIC oal_uint32  wal_config_report_thrput_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_TXOPPS
OAL_STATIC oal_uint32  wal_config_set_txop_ps_machw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
OAL_STATIC oal_uint32 wal_config_print_btcoex_status(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_LTECOEX
OAL_STATIC oal_uint32 wal_config_ltecoex_mode_set(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

OAL_STATIC oal_uint32  wal_config_80211_mcast_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_probe_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_rssi_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_config_lte_gpio_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_report_vap_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_wfa_cfg_aifsn(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_wfa_cfg_cw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#endif

OAL_STATIC oal_uint32  wal_config_get_mpdu_num(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#if 0
OAL_STATIC oal_uint32  wal_config_ota_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_CHIP_TEST
OAL_STATIC oal_uint32  wal_config_beacon_offload_test(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_ota_beacon_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_ota_rx_dscr_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_all_ota(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_dhcp_arp_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_random_mac_addr_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_oam_output(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_add_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_del_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_ampdu_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_amsdu_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_ampdu_end(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_auto_ba_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_profiling_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_addba_req(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_delba_req(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_list_ap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_list_sta(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_sta_list(mac_vap_stru *pst_mac_vap, oal_uint16 *us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_list_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_rd_pwr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_reduce_sar(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_start_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_start_join(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_start_deauth(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dump_timer(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_kick_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_ampdu_tx_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_pause_tid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_user_vip(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_vap_host(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_send_bar(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_packet_xmit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dump_ba_bitmap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_vap_pkt_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_dtimperiod(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_dtimperiod(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_amsdu_ampdu_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_amsdu_tx_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dump_all_rx_dscr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
OAL_STATIC oal_uint32  wal_config_get_hipkt_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_flowctl_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_flowctl_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_RF_110X_CALI_DPD
OAL_STATIC oal_uint32  wal_config_start_dpd(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_SMPS
OAL_STATIC oal_uint32  wal_config_set_smps_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_smps_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_smps_en(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
#endif


#ifdef _PRE_WLAN_FEATURE_UAPSD
OAL_STATIC oal_uint32  wal_config_set_uapsd_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_uapsd_en(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);

#endif

OAL_STATIC oal_uint32  wal_config_set_country(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_DFS    //1131_debug
OAL_STATIC oal_uint32  wal_config_set_country_for_dfs(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_get_country(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_11D
OAL_STATIC oal_uint32  wal_config_set_rd_by_ie_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_get_tid(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param);
oal_uint32  wal_config_set_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_beacon(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_reset_hw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_reset_operate(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_UAPSD
OAL_STATIC oal_uint32  wal_config_uapsd_debug(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_DFT_STAT
OAL_STATIC oal_uint32  wal_config_set_phy_stat_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dbb_env_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_usr_queue_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_report_vap_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_report_all_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_config_show_device_memleak(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_show_device_meminfo(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32 wal_config_add_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_get_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_remove_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_set_default_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_timer_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_scan_abort(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_cfg80211_start_sched_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_cfg80211_stop_sched_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_cfg80211_start_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_cfg80211_start_join(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_alg_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_beacon_chain_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#if 0
OAL_STATIC oal_uint32  wal_config_tdls_prohibited(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_tdls_channel_switch_prohibited(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_2040_channel_switch_prohibited(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_FortyMHzIntolerant(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_2040_coext_support(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_rx_fcs_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_DFS
OAL_STATIC oal_uint32  wal_config_dfs_radartool(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_SUPPORT_ACS
OAL_STATIC oal_uint32  wal_config_acs(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

OAL_STATIC oal_uint32  wal_config_show_profiling(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dump_rx_dscr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dump_tx_dscr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dump_memory(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_txbf_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_frag_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_rts_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_user_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_dscr_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
//#ifdef    _PRE_WLAN_CHIP_TEST

OAL_STATIC oal_uint32  wal_config_set_log_level(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_set_feature_log(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32 wal_config_set_log_lowpower(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_set_pm_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_set_rate(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_mcs(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_mcsac(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_nss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_rfch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_bw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#if defined (_PRE_WLAN_CHIP_TEST) || defined (_PRE_WLAN_FEATURE_ALWAYS_TX)
OAL_STATIC oal_uint32  wal_config_always_tx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_bcast_pkt(mac_vap_stru *pst_mac_vap, oal_uint32 ul_payload_len);
#endif  /* _PRE_WLAN_CHIP_TEST */
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
OAL_STATIC oal_uint32 wal_config_always_tx_1102(mac_vap_stru * pst_mac_vap, oal_uint16 us_len, oal_uint8 * puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_always_rx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_dync_txpower(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_get_thruput(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_freq_skew(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_adjust_ppm(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_reg_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dbb_scaling_amend(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST))
OAL_STATIC oal_uint32  wal_config_sdio_flowctrl(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_WOW
OAL_STATIC oal_uint32  wal_config_host_sleep_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_wow(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_wow_activate_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_wow_pattern(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif /* end of _PRE_WLAN_FEATURE_WOW */

#ifdef _PRE_WLAN_FEATURE_HILINK
OAL_STATIC oal_uint32  wal_config_set_monitor_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_TCP_UDP_KEEPALIVE
OAL_STATIC oal_uint32  wal_config_set_tcp_udp_params(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_mips_cycle_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_reg_write(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_config_resume_rx_intr_fifo(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

OAL_STATIC oal_uint32  wal_config_alg(mac_vap_stru *pst_mac_vap, oal_uint16 pus_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_PERFORM_STAT
OAL_STATIC oal_uint32  wal_config_pfm_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_pfm_display(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
OAL_STATIC oal_uint32  wal_config_set_edca_opt_weight_sta(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_edca_opt_switch_sta(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_edca_opt_switch_ap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_edca_opt_cycle_ap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_CHIP_TEST
OAL_STATIC oal_uint32  wal_config_lpm_tx_data(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_lpm_tx_probe_request(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_chip_state(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_soc_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_psm_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_smps_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_smps_stub(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_txop_ps(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_txop_tx_stub(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_lpm_wow_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_remove_user_lut(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_send_frame(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_rx_pn(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_set_soft_retry(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_open_addr4(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_open_wmm_test(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_chip_test_open(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_set_coex(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_dfx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_send_pspoll(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_send_nulldata(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_send_action(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_clear_all_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)
OAL_STATIC oal_uint32 wal_config_enable_pmf(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#endif /* #ifdef _PRE_WLAN_CHIP_TEST */

OAL_STATIC oal_uint32  wal_config_open_wmm(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_hide_ssid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_mib(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_mib(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_thruput_bypass(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_auto_protection(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_send_2040_coext(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_2040_coext_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_version(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_get_all_reg_value(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_DAQ
OAL_STATIC oal_uint32  wal_config_data_acq(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_SMPS
#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_get_smps_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
OAL_STATIC oal_uint32  wal_config_set_oma(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_proxysta_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_DFT_REG
oal_uint32  wal_config_dump_reg(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32 wal_config_set_wps_p2p_ie(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_set_wps_ie(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
OAL_STATIC oal_uint32  wal_config_blacklist_add(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_blacklist_add_only(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_blacklist_del(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_blacklist_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_blacklist_show(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_autoblacklist_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_autoblacklist_aging(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_autoblacklist_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_autoblacklist_reset_time(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_isolation_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_isolation_type(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_isolation_forward(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_isolation_clear(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_isolation_show(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
OAL_STATIC oal_uint32 wal_config_set_opmode_notify(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_get_user_rssbw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#endif

OAL_STATIC oal_uint32 wal_config_set_vap_nss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_WLAN_FEATURE_MCAST
OAL_STATIC oal_uint32  wal_config_m2u_snoop_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_add_m2u_deny_table(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_m2u_deny_table(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_show_m2u_snoop_table(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_igmp_packet_xmit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);


#endif

#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_rx_filter_val(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_rx_filter_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_get_rx_filter_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_report_ampdu_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#endif

OAL_STATIC oal_uint32  wal_config_set_ampdu_aggr_num(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_config_freq_adjust(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

OAL_STATIC oal_uint32  wal_config_set_stbc_cap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_ldpc_cap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
OAL_STATIC oal_uint32  wal_config_proxyarp_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_proxyarp_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#endif

#ifdef _PRE_WLAN_FEATURE_WAPI
//OAL_STATIC oal_uint32  wal_config_wapi_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_WAPI_DEBUG
OAL_STATIC oal_uint32  wal_config_wapi_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */

OAL_STATIC oal_uint32 wal_config_remain_on_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_cancel_remain_on_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_mgmt_tx(mac_vap_stru * pst_mac_vap, oal_uint16 us_len, oal_uint8 * puc_param);

OAL_STATIC oal_uint32  wal_config_vap_classify_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_vap_classify_tid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_scan_test(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_bgscan_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
OAL_STATIC oal_uint32 wal_config_query_station_stats(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_HS20
OAL_STATIC oal_uint32  wal_config_set_qos_map(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
OAL_STATIC oal_uint32  wal_config_set_p2p_ps_ops(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_p2p_ps_noa(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_p2p_ps_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
OAL_STATIC oal_uint32  wal_config_set_sta_pm_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#ifdef _PRE_PSM_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_show_pm_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_set_pm_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_sta_pm_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
OAL_STATIC oal_uint32 wal_config_set_uapsd_para(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_PROFLING_MIPS
OAL_STATIC oal_uint32  wal_config_set_mips(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_show_mips(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
OAL_STATIC oal_uint32  wal_config_set_max_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_DFT_STAT
OAL_STATIC oal_uint32  wal_config_set_performance_log_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_config_set_ampdu_mmss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
OAL_STATIC oal_uint32  wal_config_enable_arp_offload(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_show_arpoffload_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32 wal_config_cfg_vap_h2d(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_host_dev_init(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32 wal_config_host_dev_exit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

oal_uint32 wal_send_cali_data(oal_net_device_stru *pst_net_dev);

#endif
#ifdef _PRE_WLAN_TCP_OPT
OAL_STATIC oal_uint32  wal_config_get_tcp_ack_stream_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_tx_tcp_ack_opt_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_rx_tcp_ack_opt_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_tx_tcp_ack_limit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_rx_tcp_ack_limit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#endif

#ifdef _PRE_WLAN_FEATURE_ROAM
OAL_STATIC oal_uint32  wal_config_roam_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_roam_band(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_roam_org(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_roam_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_roam_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif //_PRE_WLAN_FEATURE_ROAM
#ifdef _PRE_WLAN_FEATURE_11R
OAL_STATIC oal_uint32  wal_config_set_ft_ies(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif //_PRE_WLAN_FEATURE_11R

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
OAL_STATIC oal_uint32  wal_config_enable_2040bss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
OAL_STATIC oal_uint32  wal_config_set_auto_freq_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
OAL_STATIC oal_uint32  wal_config_get_lauch_cap(mac_vap_stru *pst_mac_vap, oal_uint16 *us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_linkloss_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_all_log_level(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_d2h_hcc_assemble_cnt(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_chn_est_ctrl(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_power_ref(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_pm_cfg_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_cus_rf(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_cus_dts_cali(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_set_cus_nvram_params(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
OAL_STATIC oal_uint32  wal_config_dev_customize_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */
OAL_STATIC oal_uint32  wal_config_vap_destroy(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
OAL_STATIC oal_uint32  wal_config_set_tx_classify_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif  /* _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN */

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
OAL_STATIC oal_uint32 wal_config_set_rekey_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_atomic g_wal_config_seq_num = ATOMIC_INIT(0);
oal_module_symbol(g_wal_config_seq_num);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
oal_atomic g_wal_config_seq_num = OAL_ATOMIC_INIT(0);
#else
oal_atomic g_wal_config_seq_num = 0;
#endif

/* cfgid����ȫ�ֱ��� */
OAL_STATIC OAL_CONST wal_wid_op_stru g_ast_board_wid_op[] =
{
     /* cfgid                   �Ƿ�λmac  ����һ�ֽ�   get����              set���� */
    {WLAN_CFGID_BSS_TYPE,          OAL_TRUE,   {0},   wal_config_get_bss_type,   wal_config_set_bss_type},
    {WLAN_CFGID_ADD_VAP,           OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_add_vap},
    {WLAN_CFGID_START_VAP,         OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_start_vap},
    {WLAN_CFGID_DEL_VAP,           OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_del_vap},
    {WLAN_CFGID_DOWN_VAP,          OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_down_vap},
    {WLAN_CFGID_MODE,              OAL_FALSE,  {0},   wal_config_get_mode,       wal_config_set_mode},
    {WLAN_CFGID_BANDWIDTH,         OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_set_bandwidth},
    {WLAN_CFGID_CHECK_FEM_PA,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_fem_pa_status},

    {WLAN_CFGID_CURRENT_CHANEL,    OAL_FALSE,  {0},   wal_config_get_freq,       wal_config_set_freq},
    {WLAN_CFGID_STATION_ID,        OAL_TRUE,   {0},   OAL_PTR_NULL,              wal_config_set_mac_addr},
    {WLAN_CFGID_CONCURRENT,        OAL_TRUE,   {0},   wal_config_get_concurrent, wal_config_set_concurrent},
    {WLAN_CFGID_SSID,              OAL_FALSE,  {0},   wal_config_get_ssid,       wal_config_set_ssid},
    {WLAN_CFGID_SHORTGI,           OAL_FALSE,  {0},   wal_config_get_shortgi20,  wal_config_set_shortgi20},
    {WLAN_CFGID_SHORTGI_FORTY,     OAL_FALSE,  {0},   wal_config_get_shortgi40,  wal_config_set_shortgi40},
    {WLAN_CFGID_SHORTGI_EIGHTY,    OAL_FALSE,  {0},   wal_config_get_shortgi80,  wal_config_set_shortgi80},

    {WLAN_CFGID_SHORT_PREAMBLE,    OAL_FALSE,  {0},   wal_config_get_shpreamble, wal_config_set_shpreamble},
#ifdef _PRE_WLAN_FEATURE_MONITOR
    {WLAN_CFGID_ADDR_FILTER,       OAL_FALSE,  {0},   wal_config_get_addr_filter,wal_config_set_addr_filter},
#endif
    {WLAN_CFGID_PROT_MODE,         OAL_FALSE,  {0},   wal_config_get_prot_mode,  wal_config_set_prot_mode},
    {WLAN_CFGID_AUTH_MODE,         OAL_FALSE,  {0},   wal_config_get_auth_mode,  wal_config_set_auth_mode},
    {WLAN_CFGID_BEACON_INTERVAL,   OAL_FALSE,  {0},   wal_config_get_bintval,    wal_config_set_bintval},
    {WLAN_CFGID_NO_BEACON,         OAL_FALSE,  {0},   wal_config_get_nobeacon,   wal_config_set_nobeacon},
    {WLAN_CFGID_TX_CHAIN,          OAL_FALSE,  {0},   wal_config_get_txchain,    wal_config_set_txchain},
    {WLAN_CFGID_RX_CHAIN,          OAL_FALSE,  {0},   wal_config_get_rxchain,    wal_config_set_rxchain},
    {WLAN_CFGID_TX_POWER,          OAL_FALSE,  {0},   wal_config_get_txpower,    wal_config_set_txpower},
#ifdef _PRE_WLAN_FEATURE_SMPS
    {WLAN_CFGID_SMPS_MODE,         OAL_FALSE,  {0},   wal_config_get_smps_mode,  wal_config_set_smps_mode},
    {WLAN_CFGID_SMPS_EN,           OAL_FALSE,  {0},   wal_config_get_smps_en,    OAL_PTR_NULL},
#endif
#ifdef _PRE_WLAN_FEATURE_UAPSD
    {WLAN_CFGID_UAPSD_EN,          OAL_FALSE,  {0},   wal_config_get_uapsd_en,   wal_config_set_uapsd_en},
#endif
    {WLAN_CFGID_DTIM_PERIOD,       OAL_FALSE,  {0},   wal_config_get_dtimperiod, wal_config_set_dtimperiod},

    {WLAN_CFGID_EDCA_TABLE_CWMIN,          OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_CWMAX,          OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_AIFSN,          OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT,     OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME,  OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_EDCA_TABLE_MANDATORY,      OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_CWMIN,         OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_CWMAX,         OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_AIFSN,         OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT,    OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME, OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},
    {WLAN_CFGID_QEDCA_TABLE_MANDATORY,     OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_set_wmm_params},

    {WLAN_CFGID_VAP_INFO,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_vap_info},
    {WLAN_CFGID_EVENT_SWITCH,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_event_switch},
    {WLAN_CFGID_ETH_SWITCH,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_eth_switch},
    {WLAN_CFGID_80211_UCAST_SWITCH,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_80211_ucast_switch},

#ifdef _PRE_DEBUG_MODE_USER_TRACK
    {WLAN_CFGID_USR_THRPUT_STAT,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_report_thrput_stat},
#endif
#ifdef _PRE_WLAN_FEATURE_TXOPPS
    {WLAN_CFGID_TXOP_PS_MACHW,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_txop_ps_machw},
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    {WLAN_CFGID_BTCOEX_STATUS_PRINT,    OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_print_btcoex_status},
#endif
#ifdef _PRE_WLAN_FEATURE_LTECOEX
    {WLAN_CFGID_LTECOEX_MODE_SET,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ltecoex_mode_set},
#endif
    {WLAN_CFGID_80211_MCAST_SWITCH,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_80211_mcast_switch},
    {WLAN_CFGID_PROBE_SWITCH,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_probe_switch},
    {WLAN_CFGID_RSSI_SWITCH,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_rssi_switch},
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_REPORT_VAP_INFO,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_report_vap_info},
    {WLAN_CFGID_WFA_CFG_AIFSN,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_wfa_cfg_aifsn},
    {WLAN_CFGID_WFA_CFG_CW,        OAL_FALSE,  {0},    OAL_PTR_NULL,                wal_config_wfa_cfg_cw},
    {WLAN_CFGID_CHECK_LTE_GPIO,              OAL_FALSE,  {0},   OAL_PTR_NULL,        wal_config_lte_gpio_mode},

#endif
    {WLAN_CFGID_GET_MPDU_NUM,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_get_mpdu_num},
#if 0
    {WLAN_CFGID_OTA_SWITCH,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ota_switch},
#endif
#ifdef _PRE_WLAN_CHIP_TEST
    {WLAN_CFGID_SET_BEACON_OFFLOAD_TEST,   OAL_FALSE,  {0},    OAL_PTR_NULL,         wal_config_beacon_offload_test},
#endif
    {WLAN_CFGID_OTA_BEACON_SWITCH,      OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ota_beacon_switch},
    {WLAN_CFGID_OTA_RX_DSCR_SWITCH,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ota_rx_dscr_switch},
    {WLAN_CFGID_SET_ALL_OTA,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_all_ota},
    {WLAN_CFGID_SET_DHCP_ARP,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_dhcp_arp_switch},
    {WLAN_CFGID_SET_RANDOM_MAC_ADDR_SCAN, OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_set_random_mac_addr_scan},

#ifdef _PRE_WLAN_RF_110X_CALI_DPD
    {WLAN_CFGID_START_DPD,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_start_dpd},
#endif
    {WLAN_CFGID_OAM_OUTPUT_TYPE,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_oam_output},
    {WLAN_CFGID_ADD_USER,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_add_user},
    {WLAN_CFGID_DEL_USER,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_del_user},
    {WLAN_CFGID_AMPDU_START,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ampdu_start},
    {WLAN_CFGID_AMSDU_START,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_amsdu_start},
    {WLAN_CFGID_AMPDU_END,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ampdu_end},
    {WLAN_CFGID_AUTO_BA_SWITCH,         OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_auto_ba_switch},
    {WLAN_CFGID_PROFILING_SWITCH,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_profiling_switch},
    {WLAN_CFGID_ADDBA_REQ,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_addba_req},
    {WLAN_CFGID_DELBA_REQ,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_delba_req},
    {WLAN_CFGID_SET_LOG_LEVEL,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_log_level},
    {WLAN_CFGID_SET_FEATURE_LOG,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_feature_log},
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_SET_LOG_PM,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_log_lowpower},
    {WLAN_CFGID_SET_PM_SWITCH,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_pm_switch},
#endif
    {WLAN_CFGID_LIST_AP,                OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_list_ap},
    {WLAN_CFGID_LIST_STA,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_list_sta},
    {WLAN_CFGID_DUMP_ALL_RX_DSCR,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dump_all_rx_dscr},
    {WLAN_CFGID_START_SCAN,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_start_scan},
    {WLAN_CFGID_START_JOIN,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_start_join},
    {WLAN_CFGID_START_DEAUTH,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_start_deauth},
    {WLAN_CFGID_DUMP_TIEMR,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dump_timer},
    {WLAN_CFGID_KICK_USER,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_kick_user},
    {WLAN_CFGID_PAUSE_TID,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_pause_tid},
    {WLAN_CFGID_SET_USER_VIP,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_user_vip},
    {WLAN_CFGID_SET_VAP_HOST,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_vap_host},
    {WLAN_CFGID_AMPDU_TX_ON,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_ampdu_tx_on},
    {WLAN_CFGID_AMSDU_TX_ON,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_amsdu_tx_on},
    {WLAN_CFGID_SEND_BAR,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_send_bar},
    {WLAN_CFGID_PACKET_XMIT,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_packet_xmit},
    {WLAN_CFGID_DUMP_BA_BITMAP,         OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dump_ba_bitmap},
    {WLAN_CFGID_VAP_PKT_STAT,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_vap_pkt_stat},
    {WLAN_CFGID_TIMER_START,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_timer_start},
    {WLAN_CFGID_SHOW_PROFILING,         OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_show_profiling},
    {WLAN_CFGID_AMSDU_AMPDU_SWITCH,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_amsdu_ampdu_switch},
    {WLAN_CFGID_COUNTRY,                OAL_FALSE,  {0},    wal_config_get_country,  wal_config_set_country},
#ifdef _PRE_WLAN_FEATURE_DFS    //1131_debug
    {WLAN_CFGID_COUNTRY_FOR_DFS,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_country_for_dfs},
#endif
    {WLAN_CFGID_TID,                    OAL_FALSE,  {0},    wal_config_get_tid,      OAL_PTR_NULL},
    {WLAN_CFGID_RESET_HW,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_reset_hw},
    {WLAN_CFGID_RESET_HW_OPERATE,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_reset_operate},

#ifdef _PRE_WLAN_FEATURE_UAPSD
    {WLAN_CFGID_UAPSD_DEBUG,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_uapsd_debug},
#endif
#ifdef _PRE_WLAN_DFT_STAT
    {WLAN_CFGID_PHY_STAT_EN,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_phy_stat_en},
    {WLAN_CFGID_DBB_ENV_PARAM,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dbb_env_param},
    {WLAN_CFGID_USR_QUEUE_STAT,         OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_usr_queue_stat},
    {WLAN_CFGID_VAP_STAT,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_report_vap_stat},
    {WLAN_CFGID_ALL_STAT,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_report_all_stat},
#endif


    {WLAN_CFGID_DUMP_RX_DSCR,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dump_rx_dscr},
    {WLAN_CFGID_DUMP_TX_DSCR,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dump_tx_dscr},
    {WLAN_CFGID_DUMP_MEMORY,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dump_memory},
    {WLAN_CFGID_ALG,                    OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_alg},
    {WLAN_CFGID_BEACON_CHAIN_SWITCH,    OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_beacon_chain_switch},
#if 0
    {WLAN_CFGID_TDLS_PROHI,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_tdls_prohibited},
    {WLAN_CFGID_TDLS_CHASWI_PROHI,      OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_tdls_channel_switch_prohibited},
#endif
    {WLAN_CFGID_2040_CHASWI_PROHI,      OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_2040_channel_switch_prohibited},
    {WLAN_CFGID_2040_INTOLERANT,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_FortyMHzIntolerant},
    {WLAN_CFGID_2040_COEXISTENCE,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_2040_coext_support},
    {WLAN_CFGID_FRAG_THRESHOLD_REG,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_frag_threshold},
    {WLAN_CFGID_RX_FCS_INFO,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_rx_fcs_info},

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_RESUME_RX_INTR_FIFO,    OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_resume_rx_intr_fifo},
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
    {WLAN_CFGID_RADARTOOL,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dfs_radartool},
#endif
#ifdef _PRE_SUPPORT_ACS
    {WLAN_CFGID_ACS_CONFIG,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_acs},
#endif

    {WLAN_CFGID_LIST_CHAN,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_list_channel},
    {WLAN_CFGID_REGDOMAIN_PWR,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_rd_pwr},
    {WLAN_CFGID_USER_INFO,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_user_info},
    {WLAN_CFGID_SET_DSCR,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_dscr_param},
/*#ifdef _PRE_WLAN_CHIP_TEST  */
    {WLAN_CFGID_SET_RATE,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_rate},
    {WLAN_CFGID_SET_MCS,                OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_mcs},
    {WLAN_CFGID_SET_MCSAC,              OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_mcsac},
    {WLAN_CFGID_SET_NSS,                OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_nss},
    {WLAN_CFGID_SET_RFCH,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_rfch},
    {WLAN_CFGID_SET_BW,                 OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_bw},
#if defined (_PRE_WLAN_CHIP_TEST) || defined (_PRE_WLAN_FEATURE_ALWAYS_TX)
    {WLAN_CFGID_SET_ALWAYS_TX,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_always_tx},
#endif
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    {WLAN_CFGID_SET_ALWAYS_TX_1102,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_always_tx_1102},
#endif
    {WLAN_CFGID_SET_ALWAYS_RX,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_always_rx},
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_DYNC_TXPOWER,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dync_txpower},
#endif
    {WLAN_CFGID_GET_THRUPUT,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_get_thruput},
    {WLAN_CFGID_SET_FREQ_SKEW,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_freq_skew},
    {WLAN_CFGID_ADJUST_PPM,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_adjust_ppm},

    {WLAN_CFGID_REG_INFO,               OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_reg_info},
    {WLAN_CFGID_DBB_SCALING_AMEND,      OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_dbb_scaling_amend},
#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST))
    {WLAN_CFGID_SDIO_FLOWCTRL,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_sdio_flowctrl},
#endif

#ifdef _PRE_WLAN_FEATURE_WOW
    {WLAN_CFGID_HOST_SLEEP_EN,          OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_host_sleep_switch},
    {WLAN_CFGID_SET_WOW,                OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_wow},
    {WLAN_CFGID_WOW_ACTIVATE_EN,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_wow_activate_switch},
    {WLAN_CFGID_SET_WOW_PATTERN,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_wow_pattern},
#endif /* end of _PRE_WLAN_FEATURE_WOW */

#ifdef _PRE_WLAN_FEATURE_HILINK
    {WLAN_CFGID_MONITOR_EN ,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_monitor_switch},
#endif
#ifdef _PRE_WLAN_FEATURE_TCP_UDP_KEEPALIVE
    {WLAN_CFGID_TCP_UDP_PARAMS ,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_tcp_udp_params},
#endif
    {WLAN_MIPS_CYCLE_SWITCH,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_mips_cycle_switch},
    {WLAN_CFGID_REG_WRITE,               OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_reg_write},
    {WLAN_CFGID_TXBF_SWITCH,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_txbf_switch},

    {WLAN_CFGID_SCAN_ABORT,               OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_scan_abort},
    /* ����Ϊ�ں�cfg80211���õ����� */
    {WLAN_CFGID_CFG80211_START_SCHED_SCAN,OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_cfg80211_start_sched_scan},
    {WLAN_CFGID_CFG80211_STOP_SCHED_SCAN, OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_cfg80211_stop_sched_scan},
    {WLAN_CFGID_CFG80211_START_SCAN,      OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_cfg80211_start_scan},
    {WLAN_CFGID_CFG80211_START_CONNECT,   OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_cfg80211_start_join},
    {WLAN_CFGID_CFG80211_SET_CHANNEL,     OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_set_channel},
    {WLAN_CFGID_CFG80211_CONFIG_BEACON,   OAL_FALSE,  {0},    OAL_PTR_NULL,      wal_config_set_beacon},


    {WLAN_CFGID_ADD_KEY,           OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_add_key},
    {WLAN_CFGID_GET_KEY,           OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_get_key},
    {WLAN_CFGID_REMOVE_KEY,        OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_remove_key},

    {WLAN_CFGID_ALG_PARAM,         OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_alg_param},

#ifdef _PRE_WLAN_PERFORM_STAT
    {WLAN_CFGID_PFM_STAT,          OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_pfm_stat},
    {WLAN_CFGID_PFM_DISPLAY,       OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_pfm_display},
#endif
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
    {WLAN_CFGID_SET_REKEY,        OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_set_rekey_info},
#endif

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    {WLAN_CFGID_EDCA_OPT_SWITCH_STA,  OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_set_edca_opt_switch_sta},
    {WLAN_CFGID_EDCA_OPT_WEIGHT_STA,  OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_set_edca_opt_weight_sta},
    {WLAN_CFGID_EDCA_OPT_SWITCH_AP,   OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_set_edca_opt_switch_ap},
    {WLAN_CFGID_EDCA_OPT_CYCLE_AP,    OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_set_edca_opt_cycle_ap},
#endif

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
    {WLAN_CFGID_GET_HIPKT_STAT,  	  OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_get_hipkt_stat},
    {WLAN_CFGID_SET_FLOWCTL_PARAM,    OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_set_flowctl_param},
    {WLAN_CFGID_GET_FLOWCTL_STAT,     OAL_FALSE,  {0},   OAL_PTR_NULL,           wal_config_get_flowctl_stat},
#endif

    /* START:��ԴAPP �����·���˽������ */
    {WLAN_CFGID_GET_ASSOC_REQ_IE,  OAL_FALSE,  {0},   wal_config_get_assoc_req_ie,   OAL_PTR_NULL},
    {WLAN_CFGID_SET_WPS_IE,        OAL_FALSE,  {0},   OAL_PTR_NULL,             wal_config_set_wps_ie},
    {WLAN_CFGID_SET_RTS_THRESHHOLD,OAL_FALSE,  {0},   OAL_PTR_NULL,             wal_config_rts_threshold},
    /* END:��ԴAPP �����·���˽������ */

#ifdef _PRE_WLAN_CHIP_TEST
    {WLAN_CFGID_LPM_TX_DATA,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_lpm_tx_data},
    {WLAN_CFGID_LPM_TX_PROBE_REQUEST,        OAL_FALSE,  {0},   OAL_PTR_NULL,   wal_config_lpm_tx_probe_request},
    {WLAN_CFGID_LPM_CHIP_STATE,     OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_chip_state},
    {WLAN_CFGID_LPM_SOC_MODE,      OAL_FALSE,  {0},   OAL_PTR_NULL,             wal_config_set_lpm_soc_mode},
    {WLAN_CFGID_LPM_PSM_PARAM,      OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_psm_param},
    {WLAN_CFGID_LPM_SMPS_MODE,      OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_smps_mode},
    {WLAN_CFGID_LPM_SMPS_STUB,      OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_smps_stub},
    {WLAN_CFGID_LPM_TXOP_PS_SET,    OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_txop_ps},
    {WLAN_CFGID_LPM_TXOP_TX_STUB,   OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_txop_tx_stub},
    {WLAN_CFGID_LPM_WOW_EN,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_lpm_wow_en},
    {WLAN_CFGID_REMOVE_LUT,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_remove_user_lut},
    {WLAN_CFGID_SEND_FRAME,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_send_frame},
    {WLAN_CFGID_SET_RX_PN_REG,      OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_rx_pn},
    {WLAN_CFGID_SET_SOFT_RETRY,     OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_soft_retry},
    {WLAN_CFGID_OPEN_ADDR4,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_open_addr4},
    {WLAN_CFGID_OPEN_WMM_TEST,      OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_open_wmm_test},

    {WLAN_CFGID_CHIP_TEST_OPEN,     OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_chip_test_open},
    {WLAN_CFGID_SET_COEX,           OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_coex},
    {WLAN_CFGID_DFX_SWITCH,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_dfx},
#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)
    {WLAN_CFGID_PMF_ENABLE,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_enable_pmf},
#endif
    {WLAN_CFGID_SEND_ACTION,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_send_action},
    {WLAN_CFGID_CLEAR_ALL_STAT,         OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_clear_all_stat},
    {WLAN_CFGID_SEND_PSPOLL,        OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_send_pspoll},
    {WLAN_CFGID_SEND_NULLDATA,      OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_send_nulldata},
#endif /* #ifdef _PRE_WLAN_CHIP_TEST */
    {WLAN_CFGID_DEFAULT_KEY,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_default_key},

    {WLAN_CFGID_WMM_SWITCH,         OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_open_wmm},
    {WLAN_CFGID_HIDE_SSID,          OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_hide_ssid},

    {WLAN_CFGID_SET_MIB,            OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_mib},
    {WLAN_CFGID_GET_MIB,            OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_mib},
    {WLAN_CFGID_SET_THRUPUT_BYPASS, OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_thruput_bypass},
    {WLAN_CFGID_SET_AUTO_PROTECTION,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_auto_protection},

    {WLAN_CFGID_SEND_2040_COEXT,    OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_send_2040_coext},
    {WLAN_CFGID_2040_COEXT_INFO,    OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_2040_coext_info},
    {WLAN_CFGID_GET_VERSION,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_version},
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_GET_ALL_REG_VALUE,  OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_all_reg_value},
#endif
#ifdef _PRE_WLAN_FEATURE_DAQ
    {WLAN_CFGID_DATA_ACQ,           OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_data_acq},
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    {WLAN_CFGID_SET_OMA,            OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_oma},
    {WLAN_CFGID_PROXYSTA_SWITCH,    OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_proxysta_switch},
#endif

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    {WLAN_CFGID_SET_OPMODE_NOTIFY,  OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_opmode_notify},
    {WLAN_CFGID_GET_USER_RSSBW,     OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_user_rssbw},
#endif

    {WLAN_CFGID_SET_VAP_NSS,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_vap_nss},

#ifdef _PRE_WLAN_DFT_REG
    {WLAN_CFGID_DUMP_REG,       OAL_FALSE,  {0},   OAL_PTR_NULL,                wal_config_dump_reg},
#endif

#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_RX_FILTER_VAL,      OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_rx_filter_val},
    {WLAN_CFGID_SET_RX_FILTER_EN,   OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_rx_filter_en},
    {WLAN_CFGID_GET_RX_FILTER_EN,   OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_rx_filter_en},
    {WLAN_CFGID_REPORT_AMPDU_STAT,  OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_report_ampdu_stat},
#endif

    {WLAN_CFGID_SET_AGGR_NUM,       OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_ampdu_aggr_num},

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_FREQ_ADJUST,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_freq_adjust},
#endif

    {WLAN_CFGID_SET_STBC_CAP,       OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_stbc_cap},
    {WLAN_CFGID_SET_LDPC_CAP,       OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_ldpc_cap},

#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY

    /* ���������� */
    {WLAN_CFGID_ADD_BLACK_LIST          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_blacklist_add},
    {WLAN_CFGID_DEL_BLACK_LIST          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_blacklist_del},
    {WLAN_CFGID_BLACKLIST_MODE          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_blacklist_mode},
    {WLAN_CFGID_BLACKLIST_SHOW          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_blacklist_show},

    /* �Զ��������������� */
    {WLAN_CFGID_AUTOBLACKLIST_ON        ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_autoblacklist_enable},
    {WLAN_CFGID_AUTOBLACKLIST_AGING     ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_autoblacklist_aging},
    {WLAN_CFGID_AUTOBLACKLIST_THRESHOLD ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_autoblacklist_threshold},
    {WLAN_CFGID_AUTOBLACKLIST_RESET     ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_autoblacklist_reset_time},

    /* �û������������ */
    {WLAN_CFGID_ISOLATION_MODE          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_isolation_mode},
    {WLAN_CFGID_ISOLATION_TYPE          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_isolation_type},
    {WLAN_CFGID_ISOLATION_FORWARD       ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_isolation_forward},
    {WLAN_CFGID_ISOLATION_CLEAR         ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_isolation_clear},
    {WLAN_CFGID_ISOLATION_SHOW          ,OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_isolation_show},

#endif
#ifdef _PRE_WLAN_FEATURE_MCAST
    {WLAN_CFGID_M2U_SNOOP_ON,      OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_m2u_snoop_on},
    {WLAN_ADD_M2U_DENY_TABLE,      OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_add_m2u_deny_table},
    {WLAN_CFGID_M2U_DENY_TABLE,    OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_m2u_deny_table},
    {WLAN_SHOW_M2U_SNOOP_TABLE,    OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_show_m2u_snoop_table},
    {WLAN_CFGID_IGMP_PACKET_XMIT,  OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_igmp_packet_xmit},
#endif

#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
    {WLAN_CFGID_ADD_BLACK_LIST_ONLY    ,OAL_FALSE,  {0},   OAL_PTR_NULL,      wal_config_blacklist_add_only},
#endif

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
    {WLAN_CFGID_PROXYARP_EN             ,OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_proxyarp_en},
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_PROXYARP_INFO           ,OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_proxyarp_info},
#endif /* #ifdef _PRE_DEBUG_MODE */
#endif

#ifdef _PRE_WLAN_FEATURE_WAPI
    //{WLAN_CFGID_WAPI_EN             ,OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_wapi_en},
#ifdef _PRE_WAPI_DEBUG
    {WLAN_CFGID_WAPI_INFO           ,OAL_FALSE,  {0},    OAL_PTR_NULL,          wal_config_wapi_info},
#endif /* #ifdef _PRE_DEBUG_MODE */
#endif


#ifdef _PRE_WLAN_FEATURE_SMPS
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_GET_SMPS_INFO,           OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_get_smps_info},
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_PM
    {WLAN_CFGID_WIFI_EN,            OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_wifi_enable},
    {WLAN_CFGID_PM_INFO,            OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_pm_info},
    {WLAN_CFGID_PM_EN,              OAL_FALSE,  {0},   OAL_PTR_NULL,              wal_config_pm_enable},
#endif

    {WLAN_CFGID_SET_WPS_P2P_IE,                    OAL_FALSE,  {0},    OAL_PTR_NULL,     wal_config_set_wps_p2p_ie},
    {WLAN_CFGID_CFG80211_REMAIN_ON_CHANNEL,        OAL_FALSE,  {0},    OAL_PTR_NULL,     wal_config_remain_on_channel},
    {WLAN_CFGID_CFG80211_CANCEL_REMAIN_ON_CHANNEL, OAL_FALSE,  {0},    OAL_PTR_NULL,     wal_config_cancel_remain_on_channel},
    {WLAN_CFGID_CFG80211_MGMT_TX,   OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_mgmt_tx},

    {WLAN_CFGID_VAP_CLASSIFY_EN,    OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_vap_classify_en},
    {WLAN_CFGID_VAP_CLASSIFY_TID,   OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_vap_classify_tid},
#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_SCAN_TEST,          OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_scan_test},
#endif
    {WLAN_CFIGD_BGSCAN_ENABLE,      OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_bgscan_enable},
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
    {WLAN_CFGID_QUERY_STATION_STATS,   OAL_FALSE,  {0},    OAL_PTR_NULL,           wal_config_query_station_stats},
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_DEVICE_MEM_LEAK,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_show_device_memleak},
    {WLAN_CFGID_DEVICE_MEM_INFO,       OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_show_device_meminfo},
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
    {WLAN_CFGID_SET_PS_MODE,           OAL_FALSE,  {0},   OAL_PTR_NULL,         wal_config_set_sta_pm_mode},
#ifdef _PRE_PSM_DEBUG_MODE
    {WLAN_CFGID_SHOW_PS_INFO,          OAL_FALSE,  {0},   OAL_PTR_NULL,         wal_config_show_pm_info},
#endif
    {WLAN_CFGID_SET_PSM_PARAM,         OAL_FALSE,  {0},   OAL_PTR_NULL,          wal_config_set_pm_param},
    {WLAN_CFGID_SET_STA_PM_ON,          OAL_FALSE,  {0},   OAL_PTR_NULL,         wal_config_set_sta_pm_on},
#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
    {WLAN_CFGID_SET_UAPSD_PARA,        OAL_FALSE, {0},      OAL_PTR_NULL,       wal_config_set_uapsd_para},
#endif
#ifdef _PRE_WLAN_FEATURE_11D
    {WLAN_CFGID_SET_RD_IE_SWITCH,      OAL_FALSE, {0},      OAL_PTR_NULL,       wal_config_set_rd_by_ie_switch},
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    {WLAN_CFGID_SET_P2P_PS_OPS,     OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_p2p_ps_ops},
    {WLAN_CFGID_SET_P2P_PS_NOA,     OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_p2p_ps_noa},
    {WLAN_CFGID_SET_P2P_PS_STAT,    OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_p2p_ps_stat},
#endif
#ifdef _PRE_WLAN_FEATURE_HS20
    {WLAN_CFGID_SET_QOS_MAP,        OAL_FALSE,  {0},   OAL_PTR_NULL,            wal_config_set_qos_map},
#endif
#ifdef _PRE_WLAN_PROFLING_MIPS
    {WLAN_CFGID_SET_MIPS,            OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_mips},
    {WLAN_CFGID_SHOW_MIPS,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_show_mips},
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_AMPDU_MMSS,          OAL_FALSE,  {0},    OAL_PTR_NULL,             wal_config_set_ampdu_mmss},
#endif

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
    {WLAN_CFGID_ENABLE_ARP_OFFLOAD,             OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_enable_arp_offload},
    {WLAN_CFGID_SHOW_ARPOFFLOAD_INFO,           OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_show_arpoffload_info},
#endif

#ifdef _PRE_WLAN_TCP_OPT
    {WLAN_CFGID_GET_TCP_ACK_STREAM_INFO,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_get_tcp_ack_stream_info},
    {WLAN_CFGID_TX_TCP_ACK_OPT_ENALBE,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_tx_tcp_ack_opt_enable},
    {WLAN_CFGID_RX_TCP_ACK_OPT_ENALBE,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_rx_tcp_ack_opt_enable},
    {WLAN_CFGID_TX_TCP_ACK_OPT_LIMIT,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_tx_tcp_ack_limit},
    {WLAN_CFGID_RX_TCP_ACK_OPT_LIMIT,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_rx_tcp_ack_limit},
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {WLAN_CFGID_CFG_VAP_H2D,  OAL_FALSE,    {0},    OAL_PTR_NULL,            wal_config_cfg_vap_h2d},
    {WLAN_CFGID_HOST_DEV_INIT,  OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_host_dev_init},
    {WLAN_CFGID_HOST_DEV_EXIT,  OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_host_dev_exit},
#endif
    {WLAN_CFGID_SET_MAX_USER,        OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_set_max_user},
    {WLAN_CFGID_GET_STA_LIST,        OAL_FALSE,  {0},    wal_config_get_sta_list, OAL_PTR_NULL},
#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_DFT_STAT
    {WLAN_CFGID_SET_PERFORMANCE_LOG_SWITCH,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_set_performance_log_switch},
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_ROAM
    {WLAN_CFGID_ROAM_ENABLE,  OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_roam_enable},
    {WLAN_CFGID_ROAM_ORG,     OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_roam_org},
    {WLAN_CFGID_ROAM_BAND,    OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_roam_band},
    {WLAN_CFGID_ROAM_START,   OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_roam_start},
    {WLAN_CFGID_ROAM_INFO,    OAL_FALSE,  {0},    OAL_PTR_NULL,            wal_config_roam_info},
#endif  //_PRE_WLAN_FEATURE_ROAM
#ifdef _PRE_WLAN_FEATURE_11R
    {WLAN_CFGID_SET_FT_IES,  OAL_FALSE,  {0},    OAL_PTR_NULL,             wal_config_set_ft_ies},
#endif //_PRE_WLAN_FEATURE_11R
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    {WLAN_CFGID_2040BSS_ENABLE,  OAL_FALSE,  {0},    OAL_PTR_NULL,         wal_config_enable_2040bss},
#endif

#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
    {WLAN_CFGID_SET_AUTO_FREQ_ENABLE,         OAL_FALSE,  {0},  OAL_PTR_NULL,          wal_config_set_auto_freq_enable},
#endif

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    {WLAN_CFGID_LAUCH_CAP,                 OAL_FALSE,  {0},  wal_config_get_lauch_cap, OAL_PTR_NULL},
    {WLAN_CFGID_SET_LINKLOSS_THRESHOLD,    OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_linkloss_threshold},
    {WLAN_CFGID_SET_ALL_LOG_LEVEL,         OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_all_log_level},
    {WLAN_CFGID_SET_D2H_HCC_ASSEMBLE_CNT,  OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_d2h_hcc_assemble_cnt},
    {WLAN_CFGID_SET_CHN_EST_CTRL,          OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_chn_est_ctrl},
    {WLAN_CFGID_SET_POWER_REF,             OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_power_ref},
    {WLAN_CFGID_SET_PM_CFG_PARAM,          OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_pm_cfg_param},
    {WLAN_CFGID_SET_CUS_RF,                OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_cus_rf},
    {WLAN_CFGID_SET_CUS_DTS_CALI,          OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_cus_dts_cali},
    {WLAN_CFGID_SET_CUS_NVRAM_PARAM,       OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_set_cus_nvram_params},
    /* SHOW DEIVCE CUSTOMIZE INFOS */
    {WLAN_CFGID_SHOW_DEV_CUSTOMIZE_INFOS,  OAL_FALSE,  {0},  OAL_PTR_NULL,        wal_config_dev_customize_info},
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

    {WLAN_CFGID_DESTROY_VAP,     OAL_FALSE,  {0},    OAL_PTR_NULL,         wal_config_vap_destroy},

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
    {WLAN_CFGID_TX_CLASSIFY_LAN_TO_WLAN_SWITCH,     OAL_FALSE,  {0},   OAL_PTR_NULL,        wal_config_set_tx_classify_switch},
#endif
    {WLAN_CFGID_REDUCE_SAR,     OAL_FALSE,  {0},    OAL_PTR_NULL,       wal_config_reduce_sar},
    {WLAN_CFGID_BUTT,                  OAL_FALSE,  {0},    0,                       0},
};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : wal_config_add_vap
 ��������  : ����VAP
 �������  : pst_mac_vap  : ָ��mac_vap��ָ��
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_add_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32 ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_add_vap(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        return ul_ret;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_config_del_vap
 ��������  : ɾ��vap
 �������  : pst_mac_vap  : ָ��mac_vap��ָ��
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_del_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32         ul_ret;
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    mac_device_stru   *pst_mac_device;
#endif

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_del_vap::pst_mac_vap or puc_param null ptr error [%d], [%d]}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA, "{wal_config_del_vap::mac_res_get_dev is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (OAL_TRUE == pst_mac_device->st_cap_flag.bit_proxysta)
    {
        /* �����ɾ����sta��Proxy STA������Ҫ��hash���Ƴ���main Proxy STA����Ҫ�Ƴ� */
        if ((OAL_TRUE == pst_mac_vap->st_vap_proxysta.en_is_proxysta) && (OAL_FALSE == pst_mac_vap->st_vap_proxysta.en_is_main_proxysta))
        {
            OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA, "{wal_config_del_vap::Proxy STA is removed from hash table}");
            wal_proxysta_remove_vap(pst_mac_vap);
        }
    }

#endif
    ul_ret = hmac_config_del_vap(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_del_vap:: return error code [%d].}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_start_vap
 ��������  : ����VAP
 �������  : pst_mac_vap : ָ��vap
             us_len      : ��������
             puc_param   : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_start_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_start_vap::pst_mac_vap or puc_param null ptr error %d,%d.}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_start_vap(pst_mac_vap, us_len, puc_param);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_start_vap:: return error code %d.}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_config_down_vap
 ��������  : ͣ��vap
 �������  : pst_mac_vap : ָ��vap
             us_len      : ��������
             puc_param   : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_down_vap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32    ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_down_vap::pst_mac_vap or puc_param null ptr error [%x],[%x].}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_down_vap(pst_mac_vap, us_len, puc_param);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_down_vap:: return error code [%d].}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
/*****************************************************************************
 �� �� ��  : wal_config_get_hipkt_stat
 ��������  : ��ȡ�����ȼ����ĵ�ͳ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��2��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_hipkt_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_hipkt_stat(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_flowctl_param
 ��������  : ����flowctl��ز���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��08��25��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_flowctl_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_flowctl_param(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_flowctl_stat
 ��������  : ����flowctl��ز���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��08��25��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_flowctl_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_flowctl_stat(pst_mac_vap, us_len, puc_param);
}

#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_bss_type
 ��������  : ����bss type
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_bss_type(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_bss_type(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_bss_type
 ��������  : ��ȡbss type
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_bss_type(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_bss_type(pst_mac_vap, pus_len,  puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_mode
 ��������  : ����ģʽ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_mode(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_mode
 ��������  : ����ģʽ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_mode(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_set_mode
 ��������  : ����ģʽ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_bandwidth(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_mode_param_stru    *pst_prot_param;

    pst_prot_param = (mac_cfg_mode_param_stru *)puc_param;

    pst_prot_param->en_protocol  = pst_mac_vap->en_protocol;
    pst_prot_param->en_band      = pst_mac_vap->st_channel.en_band;

    return hmac_config_set_mode(pst_mac_vap, us_len, (oal_uint8 *)pst_prot_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_get_fem_pa_status
 ��������  : ��ȡ�汾
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_fem_pa_status(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_fem_pa_status(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_mac_addr
 ��������  : ����mac��ַ
 �������  : pst_mac_vap: ָ��vap��ָ��
             ul_len     : ��������
             puc_param  : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_mac_addr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                     ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_set_mac_addr::pst_mac_vap or puc_param null ptr error %d,%d.}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_set_mac_addr(pst_mac_vap, us_len, puc_param);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_set_mac_addr:: return error code %d.}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_set_concurrent
 ��������  : �����豸�����û���
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_concurrent(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_set_concurrent::pst_mac_vap or puc_param null ptr error %d,%d.}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_set_concurrent(pst_mac_vap, us_len, puc_param);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_set_concurrent:: return error code %d.}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_get_concurrent
 ��������  : ����ssid
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_concurrent(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_concurrent(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_ssid
 ��������  : ����ssid
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_ssid(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_ssid(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_ssid
 ��������  : ����ssid
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_ssid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_ssid(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_shpreamble
 ��������  : ����ǰ��������λ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_shpreamble(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_shpreamble(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_shpreamble
 ��������  : ��ȡǰ��������λ
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_shpreamble(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_shpreamble(pst_mac_vap, pus_len, puc_param);
}
#ifdef _PRE_WLAN_FEATURE_MONITOR
/*****************************************************************************
 �� �� ��  : wal_config_set_addr_filter
 ��������  : ���õ�ַ��������λ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_addr_filter(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_addr_filter(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_addr_filter
 ��������  : ��ȡǰ��ַ��������λ
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_addr_filter(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_addr_filter(pst_mac_vap, pus_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_shortgi20
 ��������  : ����shortgi
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_shortgi20(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_shortgi20(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_shortgi40
 ��������  : ����shortgi
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_shortgi40(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_shortgi40(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_shortgi80
 ��������  : ����shortgi
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_shortgi80(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_shortgi80(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_get_shortgi20
 ��������  : ��ȡshortgi
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_shortgi20(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_shortgi20(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_shortgi40
 ��������  : ��ȡshortgi
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_shortgi40(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_shortgi40(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_shortgi80
 ��������  : ��ȡshortgi
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_shortgi80(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_shortgi80(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_prot_mode
 ��������  : ���ñ���ģʽ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_prot_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_prot_mode(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_prot_mode
 ��������  : ��ȡ����ģʽ
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_prot_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_prot_mode(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_auth_mode
 ��������  : ������֤ģʽ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_auth_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_auth_mode(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_auth_mode
 ��������  : ��ȡ��֤ģʽ
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_auth_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_auth_mode(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_bintval
 ��������  : ����beacon interval
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_bintval(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_bintval(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_bintval
 ��������  : ��ȡbeacon interval
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_bintval(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_bintval(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_dtimperiod
 ��������  : ����dtim_period
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_dtimperiod(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_dtimperiod(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_dtimperiod
 ��������  : ��ȡdtim_period
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_dtimperiod(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_dtimperiod(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_nobeacon
 ��������  : ������ʽbeacon
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_nobeacon(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_nobeacon(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_nobeacon
 ��������  : ��ȡ��ʽbeacon
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_nobeacon(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_nobeacon(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_txchain
 ��������  : ���÷���ͨ��
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_txchain(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_txchain(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_txchain
 ��������  : ��ȡ����ͨ��
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_txchain(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_txchain(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_rxchain
 ��������  : ���ý���ͨ��
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_rxchain(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_rxchain(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_rxchain
 ��������  : ��ȡ����ͨ��
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_rxchain(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_rxchain(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_txpower
 ��������  : ���÷��书��
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_txpower(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_txpower(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_txpower
 ��������  : ��ȡ���书��
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_txpower(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_txpower(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_freq
 ��������  : ����Ƶ��/�ŵ�
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_freq(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_freq(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_freq
 ��������  : ��ȡƵ��/�ŵ�
 �������  : pst_event_hdr: �¼�ͷ
 �������  : pus_len      : ��������
             puc_param    : ����
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_freq(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_freq(pst_mac_vap, pus_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_cwmin
 ��������  : ����EDCA�������ڲ���
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��23��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_wmm_params(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{

    return hmac_config_set_wmm_params(pst_mac_vap, us_len, puc_param);
}



/*****************************************************************************
 �� �� ��  : wal_config_get_wmm_params
 ��������  : ����EDCA�������ڲ���
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��23��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_config_get_wmm_params(oal_net_device_stru *pst_net_dev, oal_uint8 *puc_param)
{
    mac_vap_stru               *pst_vap;

    pst_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_config_get_wmm_params::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_config_get_wmm_params(pst_vap, puc_param);
}

#ifdef _PRE_WLAN_FEATURE_SMPS
/*****************************************************************************
 �� �� ��  : wal_config_set_smps_mode
 ��������  : ����smpsģʽ
 �������  : pst_mac_vap  : vap
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_smps_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_smps_mode(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_smps_mode
 ��������  : ��ȡsmpsģʽ
 �������  : pst_mac_vap  : vap
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_smps_mode(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_smps_mode(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_smps_en
 ��������  : ��ȡsmpsģʽ
 �������  : pst_mac_vap  : vap
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_smps_en(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_smps_en(pst_mac_vap, pus_len, puc_param);
}

#endif

#ifdef _PRE_WLAN_FEATURE_UAPSD

/*****************************************************************************
 �� �� ��  : wal_config_set_uapsd_en
 ��������  : ����vap uapsdʹ��
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_uapsd_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_uapsden(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_uapsd_en
 ��������  : ��ȡvap uapsdʹ��
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_uapsd_en(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{

    return hmac_config_get_uapsden(pst_mac_vap, pus_len, puc_param);
}

#endif


/*****************************************************************************
 �� �� ��  : wal_config_set_channel
 ��������  : �����ŵ���Ϣ������Ƶ�Σ��ŵ��ţ�����
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��26��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_config_set_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_channel(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_beacon
 ��������  : �����豸�������������RTS ����ֵ����Ƭ����ֵ
 �������  : pst_event_hdr: �¼�ͷ
             us_len       : ��������
             puc_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��24��
    ��    ��   : zex287476
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_beacon(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                          ul_ret;


    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap) || OAL_UNLIKELY(OAL_PTR_NULL == puc_param) )
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_set_beacon::pst_mac_vap or puc_param is null [%x],[%x].}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_set_beacon(pst_mac_vap, us_len, puc_param);

    return ul_ret;
}

/*****************************************************************************
 �� �� ��  : wal_config_vap_info
 ��������  : ��ӡvap������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_vap_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_vap_info(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_event_switch
 ��������  : ����OAM eventģ����ܿ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_event_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_event_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_eth_switch
 ��������  : ������̫���շ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_eth_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_eth_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_80211_ucast_switch
 ��������  : ����80211����֡�ϱ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_80211_ucast_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_80211_ucast_switch(pst_mac_vap, us_len, puc_param);
}


#ifdef _PRE_DEBUG_MODE_USER_TRACK
/*****************************************************************************
 �� �� ��  : wal_config_report_thrput_stat
 ��������  : �ϱ�Ӱ���û�ʵʱ���µ�ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_report_thrput_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_report_thrput_stat(pst_mac_vap, us_len, puc_param);
}

#endif


#ifdef _PRE_WLAN_FEATURE_TXOPPS

/*****************************************************************************
 �� �� ��  : wal_config_set_txop_ps_machw
 ��������  : ����mac txop psʹ�ܼĴ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��16��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_txop_ps_machw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_txop_ps_machw(pst_mac_vap, us_len, puc_param);
}

#endif

#ifdef _PRE_WLAN_FEATURE_BTCOEX
/*****************************************************************************
 �� �� ��  : wal_config_print_btcoex_status
 ��������  : ��ӡ����ά����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_print_btcoex_status(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_print_btcoex_status(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_LTECOEX
/*****************************************************************************
 �� �� ��  : wal_config_ltecoex_mode_set
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_ltecoex_mode_set(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_ltecoex_mode_set(pst_mac_vap, us_len, puc_param);
}
#endif


/*****************************************************************************
 �� �� ��  : wal_config_80211_mcast_switch
 ��������  : ����80211�鲥\�㲥֡�ϱ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_80211_mcast_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_80211_mcast_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_probe_switch
 ��������  : ����probe request��probe response�ϱ��Ŀ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_probe_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_probe_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_rssi_switch
 ��������  : ���ô�ӡ���ձ�����rssi��Ϣ�Ŀ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_rssi_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_rssi_switch(pst_mac_vap, us_len, puc_param);
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/*****************************************************************************
 �� �� ��  : wal_config_report_vap_info
 ��������  : ����flagλ�ϱ���Ӧ��vap��Ϣ
 �������  : mac_vap_stru *pst_mac_vap,
             oal_uint16 us_len,
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_report_vap_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_report_vap_info(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_wfa_cfg_aifsn
 ��������  : ����aifsn�̶�ֵ
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_wfa_cfg_aifsn(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_wfa_cfg_aifsn(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_wfa_cfg_cw
 ��������  : ����cw�̶�ֵ
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_wfa_cfg_cw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_wfa_cfg_cw(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_lte_gpio_mode
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_lte_gpio_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_lte_gpio_mode(pst_mac_vap, us_len, puc_param);
}

#endif

/*****************************************************************************
 �� �� ��  : wal_config_get_mpdu_num
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_mpdu_num(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_mpdu_num(pst_mac_vap, us_len, puc_param);
}

#if 0
/*****************************************************************************
 �� �� ��  : wal_config_ota_switch
 ��������  : ����ĳһ�־����ota�ϱ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

  2.��    ��   : 2015��1��23��
    ��    ��   : 
    �޸�����   : ʵ��ת�ӿ�hmac_config_ota_switch

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_ota_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_ota_switch(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_RF_110X_CALI_DPD
OAL_STATIC oal_uint32  wal_config_start_dpd(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
   return hmac_config_start_dpd(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_CHIP_TEST
/*****************************************************************************
 �� �� ��  : wal_config_host_fore_sleep
 ��������  : ����ĳһ�־����ota�ϱ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��3��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_beacon_offload_test(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_beacon_offload_test(pst_mac_vap, us_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_ota_beacon_switch
 ��������  : �����Ƿ��ϱ�beacon֡�Ŀ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

  2.��    ��   : 2015��1��23��
    ��    ��   : 
    �޸�����   : ʵ��ת�ӿ�wal_config_ota_beacon_switch

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_ota_beacon_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_ota_beacon_switch(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_ota_rx_dscr_switch
 ��������  : �����Ƿ��ϱ�beacon֡�Ŀ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��19��
    ��    ��   : 
    �޸�����   : �����ɺ���


*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_ota_rx_dscr_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_ota_rx_dscr_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_all_ota
 ��������  : ���������û�֡�ϱ������п��أ������1�����ϱ���������֡��֡���ݣ�
             cb�ֶΣ��������������0����ʲô�����ϱ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_all_ota(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_all_ota(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_dhcp_arp_switch
 ��������  : ���÷��͹㲥dhcp��arp����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_dhcp_arp_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_dhcp_arp_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_random_mac_addr_scan
 ��������  : �������mac addrɨ�迪��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_random_mac_addr_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_random_mac_addr_scan(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_oam_output
 ��������  : ����oamģ������λ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��3��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_oam_output(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_oam_output(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_add_user
 ��������  : ����û���������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_add_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_add_user(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_del_user
 ��������  : ɾ���û���������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_del_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_del_user(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_ampdu_start
 ��������  : ����AMPDU��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_ampdu_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_ampdu_start(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_amsdu_start
 ��������  : ����AMPDU��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_amsdu_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_amsdu_start(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_ampdu_end
 ��������  : �ر�AMPDU����������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_ampdu_end(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_ampdu_end(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_auto_ba_switch
 ��������  : �����Զ�����BA�Ự�Ŀ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_auto_ba_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_auto_ba_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_auto_ba_switch
 ��������  : ����amsdu+ampdu���ϾۺϵĿ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_amsdu_ampdu_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_amsdu_ampdu_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_auto_ba_switch
 ��������  : �����Զ�����BA�Ự�Ŀ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_profiling_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_profiling_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_addba_req
 ��������  : ����BA�Ự����������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_addba_req(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_addba_req(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_delba_req
 ��������  : ɾ��BA�Ự����������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_delba_req(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_delba_req(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_list_ap
 ��������  : ��ʾSTAɨ�赽��AP
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_list_ap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_list_ap(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_list_ap
 ��������  : ��ʾSTAɨ�赽��AP
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_list_sta(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_list_sta(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_sta_list
 ��������  : ȡ�ù���sta list
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_sta_list(mac_vap_stru *pst_mac_vap, oal_uint16 *us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_sta_list(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_list_channel
 ��������  : ��ʾ֧�ֵ��ŵ��б�
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��27��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_list_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_list_channel(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_rd_pwr
 ��������  : ���ù�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_rd_pwr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_regdomain_pwr(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_reduce_sar
 ��������  : ��SAR
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_reduce_sar(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_reduce_sar(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_dump_all_rx_dscr
 ��������  : ��ӡ���н���������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_config_dump_all_rx_dscr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dump_all_rx_dscr(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_start_scan
 ��������  : ������ʼɨ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_start_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_sta_initiate_scan(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_cfg80211_start_sched_scan
 ��������  : ����PNO����ɨ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_cfg80211_start_sched_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_cfg80211_start_sched_scan(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_cfg80211_stop_sched_scan
 ��������  : ֹͣPNO����ɨ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_cfg80211_stop_sched_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_cfg80211_stop_sched_scan(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_scan_abort
 ��������  : ɨ����ֹ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_scan_abort(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_scan_abort(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_cfg80211_start_scan
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_cfg80211_start_scan(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_cfg80211_start_scan_sta(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_start_join
 ��������  : ����join
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_start_join(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_bss_dscr_stru   *pst_bss_dscr;
    oal_uint32           ul_bss_idx;

    ul_bss_idx = (oal_uint8)oal_atoi((oal_int8 *)puc_param);

    /* ??bss???,??????bss dscr??? */
    pst_bss_dscr = hmac_scan_find_scanned_bss_dscr_by_index(pst_mac_vap->uc_device_id, ul_bss_idx);
    if (OAL_PTR_NULL == pst_bss_dscr)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_start_join::find bss failed by index!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_sta_initiate_join(pst_mac_vap, pst_bss_dscr);
}

/*****************************************************************************
 �� �� ��  : wal_config_cfg80211_start_join
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��30��
    ��    ��   : 
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��10��24��
    ��    ��   : 
    �޸�����   : ���Ӽ�����֤��صĴ���
  3.��    ��   : 2015��5��5��
    ��    ��   : 
    �޸�����   : �����߼�Ų��hmac
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_cfg80211_start_join(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_connect(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_start_scan
 ��������  : ������ʼɨ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_start_deauth(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    hmac_config_send_deauth(pst_mac_vap, pst_mac_vap->auc_bssid);
    return OAL_SUCC;
}


/*****************************************************************************
 �� �� ��  : wal_config_start_scan
 ��������  : ������ʼɨ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dump_timer(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dump_timer(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_kick_user
 ��������  : ȥ����1���û�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_pause_tid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_pause_tid(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_user_vip
 ��������  : ����ĳ���û�Ϊvip
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��22��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_user_vip(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_user_vip(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_vap_host
 ��������  : ����ĳ��VAPΪhost vap
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��22��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_vap_host(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_vap_host(pst_mac_vap, us_len, puc_param);
}



/*****************************************************************************
 �� �� ��  : wal_config_send_bar
 ��������  : ȥ����1���û�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_send_bar(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_send_bar(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_create_packet
 ��������  : �����һ������
 �������  : size��ʾ���ĳ��ȣ� ������̫��ͷ���� ������FCS�� ȡֵ��ΧӦ��Ϊ60~1514
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru*  wal_config_create_packet_ap(oal_uint32    ul_size,
                                              oal_uint8     uc_tid,
                                              oal_uint8    *puc_mac_ra,
                                              oal_uint8    *puc_mac_ta)
{
    oal_netbuf_stru         *pst_buf;
    mac_ether_header_stru   *pst_ether_header;
    mac_ip_header_stru      *pst_ip;
    oal_uint32               ul_loop = 0;
    oal_uint32               l_reserve = 256;
    pst_buf = oal_netbuf_alloc(ul_size+l_reserve, (oal_int32)l_reserve, 4);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_buf))
    {
        return OAL_PTR_NULL;
    }

    oal_netbuf_put(pst_buf, ul_size);
    oal_set_mac_addr(&pst_buf->data[0], puc_mac_ra);
    oal_set_mac_addr(&pst_buf->data[6], puc_mac_ta);

    /* ֡������ ���6���ֽڱ���Ϊ0x00*/
    for (ul_loop = 0; ul_loop < ul_size - 20; ul_loop++)
    {
        pst_buf->data[14 + ul_loop] = (oal_uint8)ul_loop;
    }

    pst_ether_header = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);

    /*lint -e778*/
    pst_ether_header->us_ether_type = OAL_HOST2NET_SHORT(ETHER_TYPE_IP);
    /*lint +e778*/
    pst_ip = (mac_ip_header_stru *)(pst_ether_header + 1);      /* ƫ��һ����̫��ͷ��ȡipͷ */

    pst_ip->uc_tos = (oal_uint8)(uc_tid << WLAN_IP_PRI_SHIFT);

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    pst_ip->uc_protocol = MAC_UDP_PROTOCAL;
#endif

    pst_buf->next = OAL_PTR_NULL;
    pst_buf->prev = OAL_PTR_NULL;

    OAL_MEMZERO(oal_netbuf_cb(pst_buf), OAL_NETBUF_CB_SIZE());

    return pst_buf;

}

/*****************************************************************************
 �� �� ��  : wal_config_create_al_tx_packet
 ��������  : �����һ������
 �������  : size��ʾ���ĳ��ȣ� ������̫��ͷ���� ������FCS�� ȡֵ��ΧӦ��Ϊ60~1514
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru*  wal_config_create_al_tx_packet(oal_uint32 ul_size,
                                                    oal_uint8 uc_tid,
                                                    mac_rf_payload_enum_uint8        en_payload_flag,
                                                    oal_bool_enum_uint8 en_init_flag)
{
    static oal_netbuf_stru         *pst_buf = OAL_PTR_NULL;
    oal_uint32               ul_loop = 0;
    oal_uint32               l_reserve = 256;

    if (OAL_TRUE == en_init_flag)
    {
        pst_buf = oal_netbuf_alloc(ul_size+l_reserve, (oal_int32)l_reserve, 4);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_buf))
        {
            OAM_ERROR_LOG0(0, OAM_SF_TX, "wal_config_create_al_tx_packet::alloc Fail");
            return OAL_PTR_NULL;
        }
        oal_netbuf_put(pst_buf, ul_size);
    }

    if (OAL_PTR_NULL == pst_buf)
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "wal_config_create_al_tx_packet::pst_buf is not initiate");
        return OAL_PTR_NULL;
    }

    switch(en_payload_flag)
    {
        case RF_PAYLOAD_ALL_ZERO:
            OAL_MEMZERO(pst_buf->data, ul_size);
            break;
        case RF_PAYLOAD_ALL_ONE:
            oal_memset(pst_buf->data, 0xFF, ul_size);
            break;
        case RF_PAYLOAD_RAND:
            pst_buf->data[0] = oal_gen_random(18, 1);
            for (ul_loop = 1; ul_loop < ul_size; ul_loop++)
            {
                pst_buf->data[ul_loop] = oal_gen_random(18, 0);
            }
            break;
        default:
            break;
    }

    pst_buf->next = OAL_PTR_NULL;
    pst_buf->prev = OAL_PTR_NULL;

    OAL_MEMZERO(oal_netbuf_cb(pst_buf), OAL_NETBUF_CB_SIZE());
    return pst_buf;
}

/*****************************************************************************
 �� �� ��  : wal_config_create_igmp_packet
 ��������  : ��һ��igmpv2 reprt����
 �������  : size��ʾ���ĳ��ȣ� ������̫��ͷ���� ������FCS�� ȡֵ��ΧӦ��Ϊ60~1514
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru*  wal_config_create_igmp_packet(oal_uint32 ul_size,
                                                    oal_uint8 uc_tid,
                                                    oal_uint8 *puc_mac_ra,
                                                    oal_uint8 *puc_mac_ta)
{
    oal_netbuf_stru         *pst_buf;
    mac_ether_header_stru   *pst_ether_header;
    mac_ip_header_stru      *pst_ip;
    oal_uint32               ul_loop = 0;
    oal_uint32               l_reserve = 256;
    mac_igmp_header_stru     *pst_igmp_hdr;                         /* igmp header for v1 v2 */
    pst_buf = oal_netbuf_alloc(ul_size+l_reserve, (oal_int32)l_reserve, 4);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_buf))
    {
        return OAL_PTR_NULL;
    }

    oal_netbuf_put(pst_buf, ul_size);
    oal_set_mac_addr(&pst_buf->data[0], puc_mac_ra);
    oal_set_mac_addr(&pst_buf->data[6], puc_mac_ta);

    /* ֡������ ���6���ֽڱ���Ϊ0x00*/
    for (ul_loop = 0; ul_loop < ul_size - 50; ul_loop++)
    {
        pst_buf->data[14 + ul_loop] = (oal_uint8)ul_loop;
    }

    pst_ether_header = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);

    /*lint -e778*/
    pst_ether_header->us_ether_type = OAL_HOST2NET_SHORT(ETHER_TYPE_IP);
    /*lint +e778*/
    pst_ip = (mac_ip_header_stru *)(pst_ether_header + 1);      /* ƫ��һ����̫��ͷ��ȡipͷ */
    pst_ip->uc_version_ihl = 0x45;
    pst_ip->uc_protocol = IPPROTO_IGMP;
    //pst_ip->ul_saddr = oal_byteorder_host_to_net_uint32(0xc064642c);     /* STA��IP��ַ 192.100.100.44 */
    //pst_ip->ul_daddr = oal_byteorder_host_to_net_uint32(0xe0804020);     /* �鲥���IP��ַ 224.128.64.32 */

    pst_ip->uc_tos = (oal_uint8)(uc_tid << WLAN_IP_PRI_SHIFT);
    /* ָ��igmpͷָ�� */
    pst_igmp_hdr = (mac_igmp_header_stru *)(pst_ip + 1);
    pst_igmp_hdr->uc_type = MAC_IGMPV2_REPORT_TYPE;
    pst_igmp_hdr->ul_group = oal_byteorder_host_to_net_uint32(0xe0804020);

    pst_buf->next = OAL_PTR_NULL;
    pst_buf->prev = OAL_PTR_NULL;

    OAL_MEMZERO(oal_netbuf_cb(pst_buf), OAL_NETBUF_CB_SIZE());

    return pst_buf;

}

/*****************************************************************************
 �� �� ��  : wal_config_create_packet
 ��������  : �����һ������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru*  wal_config_create_packet_sta(oal_uint32 ul_size,
                                                oal_int32 l_reserve,
                                                oal_uint32 ul_put_len,
                                                oal_bool_enum_uint8 en_ismcast,
                                                oal_uint8 uc_tid)
{
    oal_netbuf_stru *pst_buf;
    mac_ether_header_stru  *pst_ether_header;
    mac_ip_header_stru     *pst_ip;
    oal_uint32             ul_loop = 0;

    pst_buf = oal_netbuf_alloc(ul_size, l_reserve, 4);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_buf))
    {
        return OAL_PTR_NULL;
    }

    oal_netbuf_put(pst_buf, ul_put_len);

    if (en_ismcast)
    {
        pst_buf->data[0] = 0xFF;
    }
    else
    {
        pst_buf->data[0] = 0x28;
    }

    pst_buf->data[0] = 0x28;
    pst_buf->data[1] = 0x6E;
    pst_buf->data[2] = 0xD4;
    pst_buf->data[3] = 0x89;
    pst_buf->data[4] = 0x00;
    pst_buf->data[5] = 0xC2;

    pst_buf->data[6] = 0x06;
    pst_buf->data[7] = 0x05;
    pst_buf->data[8] = 0x04;
    pst_buf->data[9] = 0x03;
    pst_buf->data[10] = 0x02;
    pst_buf->data[11] = 0x02;


    pst_buf->data[12] = 0x08;
    pst_buf->data[13] = 0x00;

    for (ul_loop = 0; ul_loop < ul_put_len - 20; ul_loop++)
    {
        pst_buf->data[14 + ul_loop] = (oal_uint8)ul_loop;
    }

    pst_ether_header = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);

    /*lint -e778*/
    pst_ether_header->us_ether_type = OAL_HOST2NET_SHORT(ETHER_TYPE_IP);
    /*lint +e778*/
    pst_ip = (mac_ip_header_stru *)(pst_ether_header + 1);      /* ƫ��һ����̫��ͷ��ȡipͷ */

    pst_ip->uc_tos = (oal_uint8)(uc_tid << WLAN_IP_PRI_SHIFT);

    pst_buf->next = OAL_PTR_NULL;
    pst_buf->prev = OAL_PTR_NULL;

    OAL_MEMZERO(oal_netbuf_cb(pst_buf), OAL_NETBUF_CB_SIZE());

    return pst_buf;

}

OAL_STATIC oal_uint32  wal_config_alg(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_alg(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_packet_xmit
 ��������  : ������������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_packet_xmit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_mpdu_ampdu_tx_param_stru *pst_ampdu_tx_on_param;
    oal_uint8                       uc_skb_num;
    oal_uint8                       uc_skb_idx;
    oal_netbuf_stru                *pst_netbuf;
    oal_uint8                       uc_tid;
    oal_uint16                      us_packet_len;
    oal_net_device_stru            *pst_dev;
    oal_netbuf_stru                *past_netbuf[32] = {OAL_PTR_NULL};

    pst_ampdu_tx_on_param = (mac_cfg_mpdu_ampdu_tx_param_stru *)puc_param;
    uc_skb_num            = pst_ampdu_tx_on_param->uc_packet_num;
    uc_tid                = pst_ampdu_tx_on_param->uc_tid;
    us_packet_len         = pst_ampdu_tx_on_param->us_packet_len;

    pst_dev = hmac_vap_get_net_device(pst_mac_vap->uc_vap_id);

    /*Ϊprofiling�����޸ģ�ֻ����xmitʱ��*/
    if (uc_skb_num <= 32)
    {
        for (uc_skb_idx = 0; uc_skb_idx < uc_skb_num; uc_skb_idx++)
        {
            past_netbuf[uc_skb_idx] = wal_config_create_packet_ap(us_packet_len, uc_tid,
                                                    pst_ampdu_tx_on_param->auc_ra_mac,
                                                    pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.auc_dot11StationID);

        }

        OAM_PROFILING_TX_STATISTIC(OAL_PTR_NULL, OAM_PROFILING_FUNC_CONFIG_XMIT_START);

        for (uc_skb_idx = 0; uc_skb_idx < uc_skb_num; uc_skb_idx++)
        {
            #ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
                wal_vap_start_xmit(past_netbuf[uc_skb_idx], pst_dev);
            #else
                wal_bridge_vap_xmit(past_netbuf[uc_skb_idx], pst_dev);
            #endif
        }

        OAM_PROFILING_TX_STATISTIC(OAL_PTR_NULL, OAM_PROFILING_FUNC_CONFIG_XMIT_END);

    }
    else
    {
        for (uc_skb_idx = 0; uc_skb_idx < uc_skb_num; uc_skb_idx++)
        {
            pst_netbuf = wal_config_create_packet_ap(us_packet_len, uc_tid,
                                                    pst_ampdu_tx_on_param->auc_ra_mac,
                                                    pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.auc_dot11StationID);
            #ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
                wal_vap_start_xmit(pst_netbuf, pst_dev);
            #else
                wal_bridge_vap_xmit(pst_netbuf, pst_dev);
            #endif
        }
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_dump_ba_bitmap
 ��������  : ������������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dump_ba_bitmap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dump_ba_bitmap(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_timer_start
 ��������  : �����жϴ�����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_timer_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint8           uc_timer_switch;

    uc_timer_switch = *(puc_param);

    if (OAL_TRUE == uc_timer_switch)
    {
        oal_5115timer_init();
    }
    else
    {
        oal_5115timer_exit();
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_show_profiling
 ��������  : ��ӡprofiling��Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_show_profiling(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
#ifdef _PRE_PROFILING_MODE

    oal_uint32          ul_value;

    ul_value = *((oal_uint32 *)puc_param);

    if (OAM_PROFILING_RX == ul_value)
    {
        oam_profiling_rx_show_offset();
    }
    else if (OAM_PROFILING_TX == ul_value)
    {
        oam_profiling_tx_show_offset();
    }
    else if (OAM_PROFILING_ALG == ul_value)
    {
        oam_profiling_alg_show_offset();
    }
    else if (OAM_PROFILING_CHIPSTART == ul_value)
    {
        oam_profiling_starttime_show_offset();
    }
    else if (OAM_PROFILING_CHSWTICH == ul_value)
    {
        oam_profiling_chswitch_show_offset();
    }
    else
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_show_profiling:: invalide profiling type %d!}\r\n", ul_value);
    }
#endif

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_ampdu_tx_on
 ��������  : ȥ����1���û�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_amsdu_tx_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_amsdu_tx_on(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_ampdu_tx_on
 ��������  : ȥ����1���û�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_ampdu_tx_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_ampdu_tx_on(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_ampdu_tx_on
 ��������  : ȥ����1���û�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_txbf_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{

    mac_cfg_ampdu_tx_on_param_stru *pst_txbf_on_param;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_txbf_switch:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_txbf_on_param = (mac_cfg_ampdu_tx_on_param_stru *)puc_param;

    if (1 == pst_txbf_on_param->uc_aggr_tx_on || 0 == pst_txbf_on_param->uc_aggr_tx_on)
    {
        //pst_mac_vap->st_cap_flag.bit_bfer = pst_txbf_on_param->uc_aggr_tx_on;
    }
    else
    {

        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_txbf_switch:: pst_ampdu_tx_on_param->uc_aggr_tx_on %d!}\r\n", pst_txbf_on_param->uc_aggr_tx_on);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_frag_threshold
 ��������  : ���÷�Ƭ����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_frag_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_frag_threshold_stru *pst_frag_threshold;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_frag_threshold:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_frag_threshold = (mac_cfg_frag_threshold_stru *)puc_param;

    if (OAL_PTR_NULL == pst_mac_vap->pst_mib_info)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_frag_threshold:pst_mib_info[%d],ul_frag_threshold[%d] !}\r\n",
                    pst_mac_vap->pst_mib_info);
        return OAL_ERR_CODE_PTR_NULL;
    }

    //pst_mac_vap->pst_mib_info->st_wlan_mib_operation.ul_dot11FragmentationThreshold = pst_frag_threshold->ul_frag_threshold;
    mac_mib_set_frag_threshold(pst_mac_vap, pst_frag_threshold->ul_frag_threshold);
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_rts_threshold
 ��������  : ���÷�Ƭ����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��20��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_rts_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_rts_threshold_stru *pst_rts_threshold;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_CFG, "{wal_config_rts_threshold:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_rts_threshold = (mac_cfg_rts_threshold_stru *)puc_param;

    if (OAL_PTR_NULL == pst_mac_vap->pst_mib_info)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_rts_threshold: pst_mac_vap->pst_mib_info is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    mac_mib_set_rts_threshold(pst_mac_vap, pst_rts_threshold->ul_rts_threshold);

    OAM_INFO_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_rts_threshold: mib rts %d!}\r\n",
                    pst_mac_vap->pst_mib_info->st_wlan_mib_operation.ul_dot11RTSThreshold);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_kick_user
 ��������  : ȥ����1���û�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_kick_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_kick_user(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_country
 ��������  : ���ù�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_country(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_country(pst_mac_vap, us_len, puc_param);
}

#ifdef _PRE_WLAN_FEATURE_DFS    //1131_debug
/*****************************************************************************
 �� �� ��  : wal_config_set_country_for_dfs
 ��������  : ���ù������е�dfs��Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_country_for_dfs(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_country_for_dfs(pst_mac_vap, us_len, puc_param);
}
#endif
/*****************************************************************************
 �� �� ��  : wal_config_set_country
 ��������  : ���ù�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_reset_hw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_reset_hw(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_country
 ��������  : ���ù�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_reset_operate(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_reset_operate(pst_mac_vap, us_len, puc_param);
}


#ifdef _PRE_WLAN_FEATURE_UAPSD
OAL_STATIC oal_uint32  wal_config_uapsd_debug(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
     return  hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_UAPSD_DEBUG,us_len, puc_param);
}

#endif

#ifdef _PRE_WLAN_DFT_STAT

/*****************************************************************************
 �� �� ��  : wal_config_set_phy_stat_en
 ��������  : ����phyͳ��ʹ�ܽڵ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_phy_stat_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_phy_stat_en(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_dbb_env_param
 ��������  : �ϱ�����ֹͣ�ϱ��տڻ�����ά�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dbb_env_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dbb_env_param(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_usr_queue_stat
 ��������  : �ϱ����������û�����ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_usr_queue_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_usr_queue_stat(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_report_vap_stat
 ��������  : �ϱ�����ֹͣ�ϱ�vap����ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_report_vap_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_report_vap_stat(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_report_all_stat
 ��������  : �ϱ�������������ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_report_all_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_report_all_stat(pst_mac_vap, us_len, puc_param);
}


#endif

/*****************************************************************************
 �� �� ��  : wal_config_dump_rx_dscr
 ��������  : dump����������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��2��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dump_rx_dscr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dump_rx_dscr(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_dump_tx_dscr
 ��������  : dump��������������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��2��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dump_tx_dscr(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dump_tx_dscr(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_dump_memory
 ��������  : dump��������������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��2��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dump_memory(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_dump_memory_stru *pst_param;
    oal_uint8                *puc_addr;
    oal_uint32                ul_i;

    pst_param = (mac_cfg_dump_memory_stru *)puc_param;
    puc_addr  = (oal_uint8 *)(pst_param->ul_addr);

    for (ul_i = 0; ul_i < pst_param->ul_len; ul_i++)
    {
        OAL_IO_PRINT("%02x ", puc_addr[ul_i]);
        if (ul_i && (ul_i % 20 == 0))
        {
            OAL_IO_PRINT("\n");
        }
    }

    OAL_IO_PRINT("\n");

    return OAL_SUCC;
}


/*****************************************************************************
 �� �� ��  : wal_config_get_country
 ��������  : ��ȡ������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_country(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_country(pst_mac_vap, pus_len, puc_param);
}
#ifdef _PRE_WLAN_FEATURE_11D
/*****************************************************************************
 �� �� ��  : wal_config_set_rd_by_ie_switch
 ��������  : ��ȡ������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_rd_by_ie_switch(mac_vap_stru *pst_mac_vap,oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_rd_by_ie_switch(pst_mac_vap, us_len, puc_param);
}
#endif
/*****************************************************************************
 �� �� ��  : wal_config_get_tid
 ��������  : ��ȡ��������֡��tid
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_tid(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_tid(pst_mac_vap, pus_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_vap_pkt_stat
 ��������  : �ϱ�ĳһ��vap�µ��շ���ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_vap_pkt_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_vap_pkt_stat(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_user_info
 ��������  : ��ӡuser��Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_user_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
   return hmac_config_user_info(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_dscr_param
 ��������  : ��ӡuser��Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_dscr_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
   return hmac_config_set_dscr_param(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_log_level
 ��������  : ������־�����¼�����hmac
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_log_level(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_log_level(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_log_level
 ��������  : ������־�����¼�����hmac
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_feature_log(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32          ul_ret;

    /***************************************************************************
        ���¼���DMAC��, ͬ��DMAC����
    ***************************************************************************/
    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_SET_FEATURE_LOG, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_set_feature_log::hmac_config_send_event failed[%d].}", ul_ret);
    }

    return ul_ret;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/*****************************************************************************
 �� �� ��  : wal_config_set_log_lowpower
 ��������  : ������־�͹���ģʽ������buff������־�����ǻ���host
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_log_lowpower(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32          ul_ret;

    /***************************************************************************
        ���¼���DMAC��, ͬ��DMAC����
    ***************************************************************************/
    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_SET_LOG_PM, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_set_feature_log::hmac_config_send_event failed[%d].}", ul_ret);
    }

    return ul_ret;

}

OAL_STATIC oal_uint32 wal_config_set_pm_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32          ul_ret;
    oal_uint8           uc_en;


    uc_en = (oal_uint8)(*puc_param);

    OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_config_set_pm_switch:[%d]}\r\n", uc_en);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if(uc_en)
    {
        g_wlan_pm_switch = OAL_TRUE;
        wlan_pm_enable();
    }
    else
    {
        wlan_pm_disable();
        g_wlan_pm_switch = OAL_FALSE;
    }
#endif
    /***************************************************************************
        ���¼���DMAC��, ͬ��DMAC����
    ***************************************************************************/
    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_SET_PM_SWITCH, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_set_feature_log::hmac_config_send_event failed[%d].}", ul_ret);
    }

    return ul_ret;

}
#endif
/*****************************************************************************
 �� �� ��  : wal_config_set_rate
 ��������  : ����non-HT����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
//#ifdef    _PRE_WLAN_CHIP_TEST
OAL_STATIC oal_uint32  wal_config_set_rate(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;
    mac_cfg_tx_comp_stru            st_event_set_bcast;

    oal_memset(&st_event_set_bcast, 0, OAL_SIZEOF(mac_cfg_tx_comp_stru));
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_config_set_rate::pst_mac_vap/puc_param is null ptr %d %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ò��� */
    ul_ret = hmac_config_set_rate(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {

        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_rate::hmac_config_set_rate error!}\r\n");
        return ul_ret;
    }
#if 0
    /* ���³��� */
    if (OAL_SWITCH_ON == pst_mac_vap->bit_al_tx_flag)
    {
       st_event_set_bcast.uc_param = OAL_SWITCH_ON;
       st_event_set_bcast.en_protocol_mode = WLAN_PHY_PROTOCOL_BUTT;
       wal_config_always_tx(pst_mac_vap, us_len, (oal_uint8 *)&st_event_set_bcast);
    }
#endif
    return ul_ret;
}

/*****************************************************************************
 �� �� ��  : wal_config_set_mcs
 ��������  : ����HT����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_mcs(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY,"{wal_config_set_mcs::pst_mac_vap/puc_param is null ptr}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ò��� */
    ul_ret = hmac_config_set_mcs(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_mcs::hmac_config_set_mcs error.}\r\n");
        return ul_ret;
    }

    return ul_ret;
}


/*****************************************************************************
 �� �� ��  : wal_config_set_mcsac
 ��������  : ����VHT����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_mcsac(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_set_mcsac::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ò��� */
    ul_ret = hmac_config_set_mcsac(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_mcsac::hmac_config_set_mcsac error!}\r\n");
        return ul_ret;
    }

    return ul_ret;
}

/*****************************************************************************
 �� �� ��  : wal_config_set_nss
 ��������  : ���ÿռ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_nss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_set_nss::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ò��� */
    ul_ret = hmac_config_set_nss(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_nss::hmac_config_set_nss error!}\r\n");
        return ul_ret;
    }

    return ul_ret;
}

/*****************************************************************************
 �� �� ��  : wal_config_set_rfch
 ��������  : ���÷���ͨ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_rfch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_set_rfch::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ò��� */
    ul_ret = hmac_config_set_rfch(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_rfch::hmac_config_set_rfch error!}\r\n");
        return ul_ret;
    }

    return ul_ret;
}


/*****************************************************************************
 �� �� ��  : wal_config_set_bw
 ��������  : ���ô���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_bw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32  ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_set_bw::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ò��� */
    ul_ret = hmac_config_set_bw(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_bw::hmac_config_set_bw error!}\r\n");
        return ul_ret;
    }

    return ul_ret;
}

#if defined (_PRE_WLAN_CHIP_TEST) || defined (_PRE_WLAN_FEATURE_ALWAYS_TX)
/*****************************************************************************
 �� �� ��  : wal_config_always_tx
 ��������  : �㲥���ݰ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_always_tx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;
    mac_cfg_tx_comp_stru            *pst_event_set_bcast;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_always_tx::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event_set_bcast = (mac_cfg_tx_comp_stru *)puc_param;

    ul_ret = hmac_config_always_tx(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_always_tx::hmac_config_always_tx failed!}\r\n");
        return ul_ret;
    }

    if (OAL_SWITCH_ON == pst_event_set_bcast->uc_param)
    {
        ul_ret = wal_config_bcast_pkt(pst_mac_vap, pst_event_set_bcast->ul_payload_len);
        if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
        {
            OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_always_tx::wal_config_bcast_pkt failed!}\r\n");
            return ul_ret;
        }
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_bcast_pkt
 ��������  : �㲥���ݰ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_bcast_pkt(mac_vap_stru *pst_mac_vap, oal_uint32 ul_payload_len)
{
    oal_netbuf_stru                *pst_netbuf;
    hmac_vap_stru                  *pst_hmac_vap;
    oal_uint8                       uc_tid;
    oal_uint32                      ul_ret = OAL_SUCC;

    /* ��μ�� */
    if (OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == pst_mac_vap->pst_mib_info)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_bcast_pkt::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_bcast_pkt::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    uc_tid = 0;

    /* ��� */
    pst_netbuf = wal_config_create_al_tx_packet(ul_payload_len, uc_tid, (oal_uint8)pst_mac_vap->bit_payload_flag, (oal_uint8)pst_hmac_vap->bit_init_flag);

    if (OAL_PTR_NULL == pst_netbuf)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_bcast_pkt::return null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_MEMZERO(OAL_NETBUF_CB(pst_netbuf), OAL_NETBUF_CB_SIZE());

    ul_ret = hmac_tx_lan_to_wlan(pst_mac_vap, pst_netbuf);

    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_bcast_pkt::hmac_tx_lan_to_wlan return error %d!}\r\n", ul_ret);
    }
    return ul_ret;
}

#endif  /* _PRE_WLAN_CHIP_TEST */

#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
/*****************************************************************************
 �� �� ��  : wal_config_always_tx
 ��������  : �㲥���ݰ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_always_tx_1102(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_always_tx::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }


    ul_ret = hmac_config_always_tx_1102(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_always_tx::hmac_config_always_tx failed!}\r\n");
        return ul_ret;
    }

    return OAL_SUCC;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_ALWAYS_TX */


/*****************************************************************************
 �� �� ��  : wal_config_always_rx
 ��������  : ���ó���ģʽ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_always_rx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_always_rx::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_always_rx(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_always_rx::hmac_config_always_rx failed!}\r\n");
        return ul_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_DEBUG_MODE
/*****************************************************************************
 �� �� ��  : wal_config_dync_txpower
 ��������  : ���ö�̬����У׼����
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dync_txpower(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_dync_txpower::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_dync_txpower(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_dync_txpower::hmac_config_dync_txpower failed!}\r\n");
        return ul_ret;
    }

    return OAL_SUCC;
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_get_thruput
 ��������  : ��ȡоƬ������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_thruput(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_get_thruput::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_get_thruput(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_get_thruput::hmac_config_get_thruput failed!}\r\n");
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_set_freq_skew
 ��������  : ����Ƶƫ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_freq_skew(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_set_freq_skew::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_set_freq_skew(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_set_freq_skew::hmac_config_set_freq_skew failed!}\r\n");
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_adjust_ppm
 ��������  : ����Ƶƫ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��9��
    ��    ��   : pengdunmen
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_adjust_ppm(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32                      ul_ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_adjust_ppm::pst_mac_vap/puc_param is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_adjust_ppm(pst_mac_vap, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_adjust_ppm::hmac_config_set_freq_skew failed!}\r\n");
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_reg_info
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_reg_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
     return hmac_config_reg_info(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_dbb_scaling
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��1��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dbb_scaling_amend(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
     return hmac_config_dbb_scaling_amend(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_reg_write
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_reg_write(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_reg_write(pst_mac_vap, us_len, puc_param);
}

#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST))

/*****************************************************************************
 �� �� ��  : wal_config_sdio_flowctrl
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_sdio_flowctrl(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sdio_flowctrl(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_WOW
/*****************************************************************************
 �� �� ��  : wal_config_host_sleep_switch
 ��������  : host sleep ����
 �������  : mac_vap_stru  *pst_mac_vap
             oal_uint16     us_len
             oal_uint8     *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_host_sleep_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_host_sleep_switch(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_set_wow
 ��������  : Enable/disable WOW events
 �������  : mac_vap_stru  *pst_mac_vap
             oal_uint16     us_len
             oal_uint8     *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_wow(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_wow(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_wow_activate_switch
 ��������  : activate/deactivate wow hipriv
 �������  : mac_vap_stru  *pst_mac_vap
             oal_uint16     us_len
             oal_uint8     *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_wow_activate_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_wow_activate_switch(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_set_wow_pattern
 ��������  : set wow pattern
 �������  : mac_vap_stru  *pst_mac_vap
             oal_uint16     us_len
             oal_uint8     *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_wow_pattern(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_wow_pattern(pst_mac_vap, us_len, puc_param);
}
#endif /* end of _PRE_WLAN_FEATURE_WOW */

#ifdef _PRE_WLAN_FEATURE_HILINK
/*****************************************************************************
 �� �� ��  : wal_config_set_monitor_switch
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_monitor_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_config_set_monitor_switch:: in");
    return hmac_config_set_monitor_switch(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_TCP_UDP_KEEPALIVE
/*****************************************************************************
 �� �� ��  : wal_config_set_tcp_udp_params
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_tcp_udp_params(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_config_set_tcp_udp_params:: in");
    return hmac_tcp_udp_process(pst_mac_vap, us_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_mips_cycle_switch
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_mips_cycle_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_mips_cycle_switch(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_alg_param
 ��������  : �㷨����ʾ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_alg_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{

    return hmac_config_alg_param(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_beacon_chain_switch
 ��������  : beacon֡ͨ��(0/1)�л�����
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��13��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_beacon_chain_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_beacon_chain_switch(pst_mac_vap, us_len, puc_param);
}
#if 0
/*****************************************************************************
 �� �� ��  : wal_config_tdls_prohibited
 ��������  : TDLS���ÿ���
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��26��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_tdls_prohibited(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_tdls_prohibited(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_tdls_channel_switch_prohibited
 ��������  : TDLS�ŵ��л����ÿ���
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��26��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_tdls_channel_switch_prohibited(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_tdls_channel_switch_prohibited(pst_mac_vap, us_len, puc_param);
}
#endif
/*****************************************************************************
 �� �� ��  : wal_config_2040_channel_switch_prohibited
 ��������  : 20/40�����ŵ��л����ÿ���(0: ����ֹ; 1: ��ֹ)
 �������  : pst_mac_vap: MAC VAPָ��
             us_len     :
             puc_param  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_2040_channel_switch_prohibited(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_2040_channel_switch_prohibited(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_FortyMHzIntolerant
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_FortyMHzIntolerant(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_FortyMHzIntolerant(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_2040_coext_support
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_2040_coext_support(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_2040_coext_support(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_rx_fcs_info
 ��������  : ��ӡ����֡��FCS��Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_rx_fcs_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_rx_fcs_info(pst_mac_vap, us_len, puc_param);
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
/*****************************************************************************
 �� �� ��  : wal_config_resume_rx_intr_fifo
 ��������  : ʹ�ָܻ�rx intr fifo����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_resume_rx_intr_fifo(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_resume_rx_intr_fifo(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
/*****************************************************************************
 �� �� ��  : wal_config_dfs_radartool
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_dfs_radartool(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dfs_radartool(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_SUPPORT_ACS
/*****************************************************************************
 �� �� ��  : wal_config_acs
 ��������  : ACS���ýӿ�
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��6��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_acs(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_acs(pst_mac_vap, us_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_process_query
 ��������  : �������ò�ѯ����
 �������  : pst_event_hdr   : �¼�ͷ
             puc_req_msg     : ������Ϣ
             us_req_msg_len  : ������Ϣ����
 �������  : puc_rsp_msg     : ������Ϣ
             pus_rsp_msg_len : ������Ϣ����
 �� �� ֵ  : OAL_SUCC������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_process_query(
                mac_vap_stru       *pst_mac_vap,
                oal_uint8          *puc_req_msg,
                oal_uint16          us_req_msg_len,
                oal_uint8          *puc_rsp_msg,
                oal_uint8          *puc_rsp_msg_len)
{
    oal_uint16          us_req_idx = 0;      /* ������Ϣ���� */
    oal_uint16          us_rsp_idx = 0;      /* ������Ϣ���� */
    oal_uint16          us_len     = 0;      /* WID��Ӧ����ֵ�ĳ��� */
    wal_msg_query_stru *pst_query_msg;
    wal_msg_write_stru *pst_rsp_msg;
    oal_uint32          ul_ret;
    oal_uint16          us_cfgid;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap)
     || (OAL_PTR_NULL == puc_req_msg)
     || (OAL_PTR_NULL == puc_rsp_msg)
     || (OAL_PTR_NULL == puc_rsp_msg_len)))
    {
        OAM_ERROR_LOG4(0, OAM_SF_ANY, "{wal_config_process_query::pst_mac_vap/puc_req_msg/puc_rsp_msg/puc_rsp_msg_len null ptr error: %d, %d, %d, %d!}\r\n",
                       pst_mac_vap, puc_req_msg, puc_rsp_msg, puc_rsp_msg_len);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ѯ��Ϣ��ʽ����:                                                     */
    /* +-------------------------------------------------------------------+ */
    /* | WID0          | WID1         | WID2         | ................... | */
    /* +-------------------------------------------------------------------+ */
    /* |     2 Bytes   |    2 Bytes   |    2 Bytes   | ................... | */
    /* +-------------------------------------------------------------------+ */

    /* ������Ϣ��ʽ����:                                                     */
    /* +-------------------------------------------------------------------+ */
    /* | WID0      | WID0 Length | WID0 Value  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 2 Byte      | WID Length  | ......................... | */
    /* +-------------------------------------------------------------------+ */

    while (us_req_idx < us_req_msg_len)
    {
        /* �Ӳ�ѯ��Ϣ�еõ�һ��WIDֵ   */
        pst_query_msg = (wal_msg_query_stru *)(&puc_req_msg[us_req_idx]);
        us_req_idx   += WAL_MSG_WID_LENGTH;                       /* ָ����һ��WID */

        /* ��ȡ������Ϣ�ڴ� */
        pst_rsp_msg = (wal_msg_write_stru *)(&puc_rsp_msg[us_rsp_idx]);

        /* Ѱ��cfgid ��Ӧ��get���� */
        for (us_cfgid = 0; WLAN_CFGID_BUTT != g_ast_board_wid_op[us_cfgid].en_cfgid; us_cfgid++)
        {
            if (g_ast_board_wid_op[us_cfgid].en_cfgid == pst_query_msg->en_wid)
            {
                break;
            }
        }

        /* �쳣�����cfgid���ڲ����������� */
        if (WLAN_CFGID_BUTT == g_ast_board_wid_op[us_cfgid].en_cfgid)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_query::cfgid not invalid %d!}\r\n", pst_query_msg->en_wid);
            continue;
        }

        /* �쳣�����cfgid��Ӧ��get����Ϊ�� */
        if (OAL_PTR_NULL == g_ast_board_wid_op[us_cfgid].p_get_func)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_query::g_ast_board_wid_op get_func ptr is null, wid is %d!}\r\n", pst_query_msg->en_wid);
            continue;
        }

        ul_ret = g_ast_board_wid_op[us_cfgid].p_get_func(pst_mac_vap, &us_len, pst_rsp_msg->auc_value);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_query::g_ast_board_wid_op func return no SUCC. wid and ret value is:%d, %d!}\r\n", pst_query_msg->en_wid, ul_ret);
            continue;
        }

        pst_rsp_msg->en_wid = pst_query_msg->en_wid;            /* ���÷�����Ϣ��WID */
        pst_rsp_msg->us_len = us_len;

        us_rsp_idx += us_len + WAL_MSG_WRITE_MSG_HDR_LENGTH;    /* ��Ϣ��ĳ��� �ټ�����Ϣͷ�ĳ��� */

        /*��ϢResponse �ӿ������õ�����ʹ�ó�����Ϣ����ռ䳤�ȣ�
          ������Ҫ���жϣ���鳤�Ⱥ͹��ƣ�������Ҫ����*/
        if(OAL_UNLIKELY(us_rsp_idx + OAL_SIZEOF(wal_msg_hdr_stru) > HMAC_RSP_MSG_MAX_LEN))
        {
            OAM_ERROR_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_query::us_cfgid:%d reponse msg len:%u over limit:%u}",
                                us_cfgid,us_rsp_idx + OAL_SIZEOF(wal_msg_hdr_stru),HMAC_RSP_MSG_MAX_LEN);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
            oal_print_hex_dump((oal_uint8*)puc_rsp_msg, HMAC_RSP_MSG_MAX_LEN,
                                        32, "puc_rsp_msg: ");
            OAL_BUG_ON(1);
#endif
        }
    }

    *puc_rsp_msg_len = (oal_uint8)us_rsp_idx;

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_process_write
 ��������  : ����������������
 �������  : pst_event_hdr: �¼�ͷ
             puc_req_msg  : ������Ϣ
             us_msg_len   : ������Ϣ����
 �������  : OAL_SUCC������������
             puc_rsp_msg     : ������Ϣ
             pus_rsp_msg_len : ������Ϣ����
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_process_write(
                mac_vap_stru    *pst_mac_vap,
                oal_uint8       *puc_req_msg,
                oal_uint16       us_msg_len,
                oal_uint8       *puc_rsp_msg,
                oal_uint8       *puc_rsp_msg_len)
{
    oal_uint16              us_req_idx = 0;
    oal_uint16              us_rsp_idx = 0;
    oal_uint16              us_cfgid;
    wal_msg_write_stru     *pst_write_msg;
    wal_msg_write_rsp_stru *pst_rsp_msg;
    oal_uint32              ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_req_msg)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_process_write::pst_mac_vap/puc_req_msg null ptr error %d, %d!}\r\n", pst_mac_vap, puc_req_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ������Ϣ�ĸ�ʽ����:                                                   */
    /* +-------------------------------------------------------------------+ */
    /* | WID0      | WID0 Length | WID0 Value  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 2 Byte      | WID Length  | ......................... | */
    /* +-------------------------------------------------------------------+ */

    /* ������Ϣ�ĸ�ʽ����:                                                   */
    /* +-------------------------------------------------------------------+ */
    /* | WID0     | resv    | WID0 ������ |  WID1   | resv | WID1������ |  | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes  | 2 Bytes | 4 Byte      | 2 Bytes | 2 B  |  4 Bytes   |  | */
    /* +-------------------------------------------------------------------+ */

    while (us_req_idx < us_msg_len)
    {
        /* ��ȡһ������WID��Ϣ   */
        pst_write_msg = (wal_msg_write_stru *)(&puc_req_msg[us_req_idx]);

        /* ��ȡ������Ϣ�ڴ� */
        pst_rsp_msg = (wal_msg_write_rsp_stru *)(&puc_rsp_msg[us_rsp_idx]);

        us_req_idx += pst_write_msg->us_len + WAL_MSG_WRITE_MSG_HDR_LENGTH;   /* ָ����һ��WID������Ϣ */

        /* Ѱ��cfgid ��Ӧ��write���� */
        for (us_cfgid = 0; WLAN_CFGID_BUTT != g_ast_board_wid_op[us_cfgid].en_cfgid; us_cfgid++)
        {
            if (g_ast_board_wid_op[us_cfgid].en_cfgid == pst_write_msg->en_wid)
            {
                break;
            }
        }

        /* �쳣�����cfgid���ڲ����������� */
        if (WLAN_CFGID_BUTT == g_ast_board_wid_op[us_cfgid].en_cfgid)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_write::cfgid not invalid %d!}\r\n", pst_write_msg->en_wid);
            continue;
        }

        ul_ret = g_ast_board_wid_op[us_cfgid].p_set_func(pst_mac_vap, pst_write_msg->us_len, pst_write_msg->auc_value);

        /* �����ش��������õ�rsp��Ϣ�� */
        pst_rsp_msg->en_wid = pst_write_msg->en_wid;
        pst_rsp_msg->ul_err_code = ul_ret;
        us_rsp_idx += OAL_SIZEOF(wal_msg_write_rsp_stru);

        /*��ϢResponse �ӿ������õ�����ʹ�ó�����Ϣ����ռ䳤�ȣ�
          ������Ҫ���жϣ���鳤�Ⱥ͹��ƣ�������Ҫ����*/
        if(OAL_UNLIKELY(us_rsp_idx + OAL_SIZEOF(wal_msg_hdr_stru) > HMAC_RSP_MSG_MAX_LEN))
        {
            OAM_ERROR_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_write::us_cfgid:%d reponse msg len:%u over limit:%u}",
                                us_cfgid,us_rsp_idx + OAL_SIZEOF(wal_msg_hdr_stru),HMAC_RSP_MSG_MAX_LEN);
            OAL_BUG_ON(1);
        }

        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_write::g_ast_board_wid_op func return no SUCC. wid and ret value is %d, %d!}\r\n", pst_write_msg->en_wid, ul_ret);
        }

    }

    *puc_rsp_msg_len = (oal_uint8)us_rsp_idx;

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_process_pkt
 ��������  : ���ð��¼�����
 �������  : pst_event: �¼�
 �������  : ��
 �� �� ֵ  : OAL_SUCC������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��6��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_config_process_pkt(frw_event_mem_stru *pst_event_mem)
{
    frw_event_stru         *pst_event;
    wal_msg_stru           *pst_msg;
    wal_msg_stru           *pst_rsp_msg = NULL;
    frw_event_hdr_stru     *pst_event_hdr;
    mac_vap_stru           *pst_mac_vap;
    oal_uint16              us_msg_len;
    oal_uint8               uc_rsp_len = 0;
    oal_uint8               uc_rsp_toal_len = 0;
    oal_uint32              ul_ret;
    oal_ulong               ul_request_address;
    oal_uint8               ac_rsp_msg[HMAC_RSP_MSG_MAX_LEN] = {0};

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_config_process_pkt::pst_event_mem null ptr error!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event     = (frw_event_stru *)pst_event_mem->puc_data;
    pst_event_hdr = &(pst_event->st_event_hdr);
    ul_request_address = ((wal_msg_rep_hdr*)pst_event->auc_event_data)->ul_request_address;
    pst_msg       = (wal_msg_stru *)(frw_get_event_payload(pst_event_mem) + OAL_SIZEOF(wal_msg_rep_hdr));

    pst_mac_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_event_hdr->uc_vap_id);

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_WARNING_LOG0(pst_event_hdr->uc_vap_id, OAM_SF_ANY, "{wal_config_process_pkt::hmac_get_vap_by_id return err code!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }


    /* ȡ������Ϣ */
    pst_rsp_msg  = (wal_msg_stru *)ac_rsp_msg;

    /* ȡ������Ϣ�ĳ��� */
    us_msg_len = pst_msg->st_msg_hdr.us_msg_len;

    OAM_INFO_LOG0(pst_event_hdr->uc_vap_id, OAM_SF_ANY, "{wal_config_process_pkt::a config event occur!}\r\n");

    switch (pst_msg->st_msg_hdr.en_msg_type)
    {
        case WAL_MSG_TYPE_QUERY:

            ul_ret = wal_config_process_query(pst_mac_vap, pst_msg->auc_msg_data, us_msg_len, pst_rsp_msg->auc_msg_data, &uc_rsp_len);
            if (OAL_SUCC != ul_ret)
            {
                OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_pkt::wal_config_process_query return error code %d!}\r\n", ul_ret);
                return ul_ret;
            }

            break;

        case WAL_MSG_TYPE_WRITE:
            ul_ret = wal_config_process_write(pst_mac_vap, pst_msg->auc_msg_data, us_msg_len, pst_rsp_msg->auc_msg_data, &uc_rsp_len);
            if (OAL_SUCC != ul_ret)
            {
                OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_pkt::wal_config_process_write return error code %d!}\r\n", ul_ret);
                return ul_ret;
            }

            break;

        default:
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_pkt::pst_msg->st_msg_hdr.en_msg_type error, msg_type is %d!}\r\n", pst_msg->st_msg_hdr.en_msg_type);

            return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /*response ����Ҫ����ͷ��*/
    uc_rsp_toal_len = uc_rsp_len + OAL_SIZEOF(wal_msg_hdr_stru);

    if(OAL_UNLIKELY(uc_rsp_toal_len > HMAC_RSP_MSG_MAX_LEN))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_process_pkt::invaild response len %u!}\r\n", uc_rsp_toal_len);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        OAL_BUG_ON(1);
#endif
    }

    /* ��䷵����Ϣͷ */
    pst_rsp_msg->st_msg_hdr.en_msg_type = WAL_MSG_TYPE_RESPONSE;
    pst_rsp_msg->st_msg_hdr.uc_msg_sn   = pst_msg->st_msg_hdr.uc_msg_sn;
    pst_rsp_msg->st_msg_hdr.us_msg_len  = uc_rsp_len;

    if(ul_request_address)
    {
        /*need response*/
        oal_uint8* pst_rsp_msg_tmp = oal_memalloc(uc_rsp_toal_len);
        if(NULL == pst_rsp_msg_tmp)
        {
            /*no mem*/
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_config_process_pkt::wal_config_process_pkt msg alloc %u failed!", uc_rsp_toal_len);
            wal_set_msg_response_by_addr(ul_request_address, NULL, OAL_ERR_CODE_PTR_NULL, uc_rsp_toal_len);
        }
        else
        {
            oal_memcopy((oal_void*)pst_rsp_msg_tmp, (oal_void*)ac_rsp_msg, uc_rsp_toal_len);
            if(OAL_SUCC != wal_set_msg_response_by_addr(ul_request_address, (oal_void*)pst_rsp_msg_tmp, OAL_SUCC, uc_rsp_toal_len))
            {
                OAL_IO_PRINT("wal_config_process_pkt did't found the request msg, request addr:0x%lx\n", ul_request_address);
                OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_config_process_pkt::wal_config_process_pkt did't found the request msg!");
                oal_free(pst_rsp_msg_tmp);
            }
        }
    }

    /* ����WAL�ȴ��Ľ��� */
    wal_cfg_msg_task_sched();

    return OAL_SUCC;
}


/*****************************************************************************
 �� �� ��  : wal_config_add_key
 ��������  : WAL�ڶ���,����hmac��ʵ��������Կ
 �������  :
 �������  : oal_uint32
 �� �� ֵ  : 0:�ɹ�,����:ʧ��
 ���ú���  : ��
 ��������  : ��


�޸���ʷ      :
  1.��    ��   : 2013��12��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_add_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_11i_add_key(pst_mac_vap, us_len, puc_param);
}
 /*****************************************************************************
 �� �� ��  : wal_config_get_key
 ��������  : WAL�ڶ���,����hmac��ʵ��
 �������  : frw_event_mem_stru *pst_event_mem
 �������  : oal_uint32
 �� �� ֵ  : 0:�ɹ�,����:ʧ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2014��1��4��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_get_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    /*ͨ���������ã�hmac����ʵ��*/
    return (hmac_config_11i_get_key(pst_mac_vap, us_len, puc_param));
}

/*****************************************************************************
 �� �� ��  : wal_config_remove_key
 ��������  : WAL�ڶ��㣬���¼���HMAC
 �������  : frw_event_mem_stru *pst_event_mem
 �������  : oal_uint32
 �� �� ֵ  : 0:�ɹ�,����:ʧ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2014��1��4��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_remove_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    /*ͨ���������ã�hmac����ʵ��*/
    return (hmac_config_11i_remove_key(pst_mac_vap, us_len, puc_param));
}

/*****************************************************************************
 �� �� ��  : wal_set_default_key
 ��������  : WAL�ڶ��㣬����HMAC
 �������  : frw_event_mem_stru *pst_event_mem
 �������  : oal_uint32
 �� �� ֵ  : 0:�ɹ�,����:ʧ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2014��1��4��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_default_key(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    /*ͨ���������ã�hmac����ʵ��*/
    return (hmac_config_11i_set_default_key(pst_mac_vap, us_len, puc_param));
}


/*****************************************************************************
 �� �� ��  : wal_config_get_assoc_req_ie
 ��������  : AP ��ȡSTA ��������IE ��Ϣ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 *pus_len
             oal_uint8 *puc_param
 �������  : oal_uint32
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��30��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 wal_config_get_assoc_req_ie(mac_vap_stru *pst_mac_vap, oal_uint16 *pus_len, oal_uint8 *puc_param)
{
    return hmac_config_get_assoc_req_ie(pst_mac_vap, pus_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_wps_ie
 ��������  : AP ����WPS ��ϢԪ�ص�VAP �ṹ��
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 *pus_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��16��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 wal_config_set_wps_ie(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_wps_ie(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_wps_p2p_ie
 ��������  : AP ����WPS ��ϢԪ�ص�VAP �ṹ��
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 *pus_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��26��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 wal_config_set_wps_p2p_ie(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_wps_p2p_ie(pst_mac_vap, us_len, puc_param);
}

#ifdef _PRE_WLAN_PERFORM_STAT
/*****************************************************************************
 �� �� ��  : wal_config_pfm_stat
 ��������  : ��ʼ����ͳ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_pfm_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_pfm_stat(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_pfm_display
 ��������  : ��ʾ����ͳ����־
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_pfm_display(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_pfm_display(pst_mac_vap, us_len, puc_param);
}

#endif

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
/*****************************************************************************
 �� �� ��  : wal_config_set_edca_opt_switch_sta
 ��������  : ����edca��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��2��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_edca_opt_switch_sta(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_edca_opt_switch_sta(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_edca_opt_weight_sta
 ��������  : ����STA��edca��������Ȩ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��2��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_edca_opt_weight_sta(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_edca_opt_weight_sta(pst_mac_vap, us_len, puc_param);
}



/*****************************************************************************
 �� �� ��  : wal_config_set_edca_opt_switch_ap
 ��������  : ����edca��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��2��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_edca_opt_switch_ap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_edca_opt_switch_ap(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_edca_opt_cycle
 ��������  : ����edca��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��2��
    ��    ��   : xiechunhui
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_edca_opt_cycle_ap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_edca_opt_cycle_ap(pst_mac_vap, us_len, puc_param);
}

#endif


#ifdef _PRE_WLAN_CHIP_TEST
/*****************************************************************************
 �� �� ��  : wal_config_lpm_tx_data
 ��������  : �����������ָ�����������ȡ���ַ�ı���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_lpm_tx_data(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_lpm_tx_data(pst_mac_vap,us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_lpm_tx_probe_request
 ��������  : ���������probe requst֡
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/

OAL_STATIC oal_uint32  wal_config_lpm_tx_probe_request(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return  hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_TX_PROBE_REQUEST,us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_chip_state
 ��������  : �����������chip˯�߻���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_chip_state(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_CHIP_STATE ,us_len, puc_param);
}

OAL_STATIC oal_uint32  wal_config_set_lpm_soc_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_SOC_MODE ,us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_psm_param
 ��������  : �����������psm�Ĵ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_psm_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_PSM_PARAM ,us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_smps_mode
 ��������  : �����������psm�Ĵ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_smps_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_SMPS_MODE ,us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_smps_stub
 ��������  : �������TXOP������׮
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_smps_stub(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_SMPS_STUB ,us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_txop_ps
 ��������  : �����������psm�Ĵ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_txop_ps(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_TXOP_PS_SET ,us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_txop_tx_stub
 ��������  : �������TXOP������׮
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_txop_tx_stub(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_TXOP_TX_STUB ,us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_lpm_wow_en
 ��������  : �������TXOP������׮
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_lpm_wow_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_LPM_WOW_EN ,us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_remove_user_lut
 ��������  : �������ɾ���ظ��û�lut
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_remove_user_lut(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_REMOVE_LUT, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_send_frame
 ��������  : ����������Ϳ���֡/����֡
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_send_frame(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_SEND_FRAME, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_rx_pn
 ��������  : �����������RX_PN_LUT_CONFIG�Ĵ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_rx_pn(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_SET_RX_PN_REG, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_sft_retry
 ��������  : �����������Software Retry������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_soft_retry(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_SET_SOFT_RETRY, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_open_addr4
 ��������  : �����������macͷΪ4��ַ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_open_addr4(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_OPEN_ADDR4, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_open_wmm_test
 ��������  : ���������/�ر�WMM
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_open_wmm_test(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_OPEN_WMM_TEST, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_chip_test_open
 ��������  : ���������оƬ��֤����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_chip_test_open(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
#ifdef _PRE_WLAN_CHIP_TEST
        /* ����һ��hmac��оƬ��֤���� */
        hmac_test_set_chip_test(*(oal_uint8*)puc_param);
#endif

    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_CHIP_TEST_OPEN, us_len, puc_param);
}


/*****************************************************************************
�� �� ��  : wal_config_set_coex
��������  : WAL�ڶ���,����hmac��ʵ��
�������  : frw_event_mem_stru *pst_event_mem
�������  : oal_uint32
�� �� ֵ  : 0:�ɹ�,����:ʧ��
���ú���  : ��
��������  : ��

�޸���ʷ      :
1.��    ��   : 2014��3��4��
��    ��   : 
�޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_coex(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    /*ͨ���������ã�hmac����ʵ��*/

#ifdef _PRE_WLAN_CHIP_TEST
    return (hmac_config_set_coex(pst_mac_vap, us_len, puc_param));
#else
    return OAL_SUCC;
#endif
}

/*****************************************************************************
 �� �� ��  : wal_config_set_dfx
 ��������  : ����DFX���Կ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_dfx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
#ifdef _PRE_WLAN_CHIP_TEST
    return hmac_config_set_dfx(pst_mac_vap, us_len, puc_param);
#else
     return OAL_SUCC;
#endif
}



#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)

/*****************************************************************************
 �� �� ��  : wal_config_enable_pmf
 ��������  : chip test��ǿ�ƴ򿪡��ر�pmf��ʹ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_enable_pmf(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    /* ����һ��hmac��оƬ��֤���� */
    hmac_enable_pmf(pst_mac_vap, puc_param);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_PMF_ENABLE, us_len, puc_param);
#else
    return OAL_SUCC;
#endif
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_send_action
 ��������  : ���÷���action֡�ӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_send_action(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_test_send_action(pst_mac_vap, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_send_pspoll
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_send_pspoll(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_send_pspoll(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_send_pspoll
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_send_nulldata(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_send_nulldata(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_clear_all_stat
 ��������  : ��ȡ������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_clear_all_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_clear_all_stat(pst_mac_vap, us_len, puc_param);
}

#endif /* #ifdef _PRE_WLAN_CHIP_TEST */

/*****************************************************************************
 �� �� ��  : wal_config_open_wmm
 ��������  : ���������/�ر�WMM
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_open_wmm(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_open_wmm::pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* �������vap������ */
    if (WLAN_VAP_MODE_CONFIG == pst_mac_vap->en_vap_mode)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_open_wmm::this is config vap! can't get info.}");
        return OAL_FAIL;
    }

    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_WMM_SWITCH, us_len, puc_param);

}

/*****************************************************************************
 �� �� ��  : wal_config_hide_ssid
 ��������  : ���������/�ر�����ssid
 �������  : pst_mac_vap
             us_len
             puc_param
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_hide_ssid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_hide_ssid(pst_mac_vap, us_len, puc_param);
}



/*****************************************************************************
 �� �� ��  : wal_config_set_mib
 ��������  : ����VAP mibֵ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_mib(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_mib(pst_mac_vap, WLAN_CFGID_SET_MIB, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_mib
 ��������  : ��ȡVAP mibֵ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_mib(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_mib(pst_mac_vap, WLAN_CFGID_GET_MIB, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_thruput_bypass
 ��������  : ����thruput_bypassά���
 �������  :
 �������  :
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��8��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_thruput_bypass(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_thruput_bypass(pst_mac_vap, WLAN_CFGID_SET_THRUPUT_BYPASS, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_auto_protection
 ��������  : ����auto protection ����
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_auto_protection(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint8 uc_auto_protection_flag;

    if (OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    uc_auto_protection_flag = (oal_uint8)*((oal_uint32 *)puc_param);

    return hmac_config_set_auto_protection(pst_mac_vap, uc_auto_protection_flag);
}

/*****************************************************************************
 �� �� ��  : wal_config_send_2040_coext
 ��������  : ����20/40�������֡
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_send_2040_coext(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_send_2040_coext(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_2040_coext_info
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_2040_coext_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_2040_coext_info(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_version
 ��������  : ��ȡ�汾
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_version(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_version(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_get_all_reg_value
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��1��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/

#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_get_all_reg_value(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_all_reg_value(pst_mac_vap, us_len, puc_param);
}
#endif
#ifdef _PRE_WLAN_FEATURE_DAQ
/*****************************************************************************

 �� �� ��  : wal_config_data_acq
 ��������  : �������ݲɼ�
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_data_acq(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_data_acq(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_SMPS
#ifdef _PRE_DEBUG_MODE
/*****************************************************************************

�� �� ��  : wal_config_get_smps_info
��������  : �������ݲɼ�
�������  : pst_mac_vap: MAC VAP�ṹ��ָ��
            us_len     : ��������
            puc_param  : ����ָ��
�������  : ��
�� �� ֵ  :
���ú���  :
��������  :

�޸���ʷ      :
 1.��    ��   : 2014��4��18��
   ��    ��   : 
   �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_smps_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_smps_info(pst_mac_vap, us_len, puc_param);
}
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
/*****************************************************************************
 �� �� ��  : wal_config_set_oma
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_config_set_oma(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_oma(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_proxysta_switch
 ��������  : ����proxystaģ����ܿ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_proxysta_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_proxysta_switch(pst_mac_vap, us_len, puc_param);
}

#endif

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY

/*****************************************************************************
 �� �� ��  : wal_config_set_opmode_notify
 ��������  : ���ù���ģʽ֪ͨ����
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��12��
    ��    ��   : 
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_opmode_notify(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_opmode_notify(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_user_rssbw
 ��������  : ���ù���ģʽ֪ͨ����
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��12��
    ��    ��   : 
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_user_rssbw(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_user_rssbw(pst_mac_vap, us_len, puc_param);
}

#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_vap_rx_nss
 ��������  : ����VAP��RX NSS
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��27��
    ��    ��   : 
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_vap_nss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_vap_nss(pst_mac_vap, us_len, puc_param);
}

#ifdef _PRE_WLAN_DFT_REG
/*****************************************************************************
 �� �� ��  : wal_config_dump_reg
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_config_dump_reg(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_device_stru            *pst_mac_dev = OAL_PTR_NULL;
    hal_to_dmac_device_stru    *pst_hal_dmac_dev = OAL_PTR_NULL;

    pst_mac_dev = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_dev))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_config_dump_reg::pst_device[id:%d] is NULL!}",
                pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_hal_dmac_dev = pst_mac_dev->pst_device_stru;
    if(OAL_SUCC != hi1151_debug_refresh_reg(pst_hal_dmac_dev, OAM_REG_EVT_USR))
    {
        return OAL_SUCC;
    }
    hi1151_debug_frw_evt(pst_hal_dmac_dev);
    return OAL_SUCC;
}
#endif


#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY

/*****************************************************************************
 �� �� ��  : wal_config_blacklist_add
 ��������  : ���Ӻ�����
 �������  : pst_net_dev: net device
             p_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_blacklist_add(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_blacklist_add(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_blacklist_add_only
 ��������  : ���Ӻ�����
 �������  : pst_net_dev: net device
             p_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��08��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_blacklist_add_only(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_blacklist_add_only(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_blacklist_del
 ��������  : ����mac��ַ
 �������  : pst_dev: �����豸
             p_addr : ��ַ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ�??

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_blacklist_del(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_blacklist_del(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_set_blacklist_mode
 ��������  : ���ú�����ģʽ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_blacklist_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_blacklist_mode(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_blacklist_show
 ��������  : ��������Ϣ��ӡ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_blacklist_show(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_show_blacklist(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_isolation_show
 ��������  : ������Ϣ��ӡ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_isolation_show(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_show_isolation(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_autoblacklist_enable
 ��������  : �Զ�������ȥ/ʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_autoblacklist_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_autoblacklist_enable(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_autoblacklist_aging
 ��������  : �����û�����ģʽ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_autoblacklist_aging(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_autoblacklist_aging(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_autoblacklist_aging
 ��������  : �����û�����ģʽ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_autoblacklist_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_autoblacklist_threshold(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_autoblacklist_aging
 ��������  : �����û�����ģʽ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_autoblacklist_reset_time(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_autoblacklist_reset_time(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_isolation_mode
 ��������  : �����û�����ģʽ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_isolation_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_isolation_mode(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_isolation_type
 ��������  : �����û���������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_isolation_type(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_isolation_type(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_isolation_forward
 ��������  : �����û�����forward
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_isolation_forward(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_isolation_forword(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_isolation_clear
 ��������  : ����ͳ����0
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_isolation_clear(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_isolation_clear(pst_mac_vap, us_len, puc_param);
}
#endif  /* #ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY */

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
/*****************************************************************************
 �� �� ��  : wal_config_proxyarp_en
 ��������  : ��proxyarp���Կ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_proxyarp_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_proxyarp_en_stru *pst_proxyarp_en_param;

    pst_proxyarp_en_param = (mac_proxyarp_en_stru *)puc_param;

    hmac_proxyarp_on(pst_mac_vap, pst_proxyarp_en_param->en_proxyarp);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_proxyarp_info
 ��������  : ��ӡproxyarp������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_proxyarp_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    hmac_proxy_display_info(pst_mac_vap);
    return OAL_SUCC;
}
#endif/* #ifdef _PRE_DEBUG_MODE */

#endif

#ifdef _PRE_WLAN_FEATURE_WAPI
/*****************************************************************************
 �� �� ��  : wal_config_wapi_info
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_WAPI_DEBUG
OAL_STATIC oal_uint32  wal_config_wapi_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_user_info_param_stru *pst_user_info;
    pst_user_info = (mac_cfg_user_info_param_stru *)puc_param;

    hmac_wapi_display_info(pst_mac_vap, pst_user_info->us_user_idx);
    return OAL_SUCC;
}
#endif/* #ifdef _PRE_DEBUG_MODE */

#endif /* _PRE_WLAN_FEATURE_WAPI */


/*****************************************************************************
 �� �� ��  : wal_recv_config_cmd
 ��������  : ע�ᵽsdt����������ص�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  : SDT����
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_recv_config_cmd(oal_uint8 *puc_buf, oal_uint16 us_len)
{
    oal_int8                ac_vap_name[OAL_IF_NAME_SIZE];
    oal_net_device_stru    *pst_net_dev;
    mac_vap_stru           *pst_mac_vap;
    frw_event_mem_stru     *pst_event_mem;
    frw_event_stru         *pst_event;
    oal_uint32              ul_ret      = OAL_SUCC;
    wal_msg_stru           *pst_msg;
    oal_netbuf_stru        *pst_netbuf;
    oal_uint16              us_netbuf_len; /* ����sdt��skb������������ͷβ�ռ�ĳ��� */
    wal_msg_stru           *pst_rsp_msg;
    wal_msg_rep_hdr        *pst_rep_hdr = NULL;

    oal_uint16              us_msg_size = us_len;
    oal_uint16              us_need_response = OAL_FALSE;

    DECLARE_WAL_MSG_REQ_STRU(st_msg_request);

    WAL_MSG_REQ_STRU_INIT(st_msg_request);

    oal_memcopy(ac_vap_name, puc_buf, OAL_IF_NAME_SIZE);
    ac_vap_name[OAL_IF_NAME_SIZE - 1] = '\0';   /* ��ֹ�ַ����쳣 */

    /* ����dev_name�ҵ�dev */
    pst_net_dev = oal_dev_get_by_name(ac_vap_name);
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_recv_config_cmd::oal_dev_get_by_name return null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_dev_put(pst_net_dev);   /* ����oal_dev_get_by_name�󣬱������oal_dev_putʹnet_dev�����ü�����һ */

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);    /* ��ȡmac vap */
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_recv_config_cmd::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_msg_size -= OAL_IF_NAME_SIZE;

    /* �����ڴ� */
    pst_event_mem = FRW_EVENT_ALLOC(us_msg_size + OAL_SIZEOF(wal_msg_rep_hdr));
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_recv_config_cmd::request %d mem failed}\r\n", us_msg_size);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = (frw_event_stru *)pst_event_mem->puc_data;

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CRX,
                       WAL_HOST_CRX_SUBTYPE_CFG,
                       (oal_uint16)(us_msg_size),
                       FRW_EVENT_PIPELINE_STAGE_0,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    /* ��д�¼�payload */
    oal_memcopy(frw_get_event_payload(pst_event_mem) + OAL_SIZEOF(wal_msg_rep_hdr), puc_buf + OAL_IF_NAME_SIZE, us_msg_size);
    pst_msg = (wal_msg_stru *)(puc_buf + OAL_IF_NAME_SIZE);
    pst_rep_hdr = (wal_msg_rep_hdr*)pst_event->auc_event_data;

    if (WAL_MSG_TYPE_QUERY == pst_msg->st_msg_hdr.en_msg_type)
    {
        /*need response*/
        us_need_response = OAL_TRUE;
    }

    if(OAL_TRUE == us_need_response)
    {
        pst_rep_hdr->ul_request_address = (oal_ulong)&st_msg_request;
        wal_msg_request_add_queue(&st_msg_request);
    }
    else
    {
        pst_rep_hdr->ul_request_address = 0;
    }

    ul_ret = wal_config_process_pkt(pst_event_mem);

    if(OAL_TRUE == us_need_response)
    {
        wal_msg_request_remove_queue(&st_msg_request);
    }

    if (OAL_SUCC != ul_ret)
    {
        FRW_EVENT_FREE(pst_event_mem);
        if(NULL != st_msg_request.pst_resp_mem)
        {
            /*�쳣ʱ�ڴ���Ҫ�ͷ�*/
            oal_free(st_msg_request.pst_resp_mem);
        }
        return (oal_int32)ul_ret;
    }

    /* �ͷ��ڴ� */
    FRW_EVENT_FREE(pst_event_mem);

    /* ����ǲ�ѯ��Ϣ���ͣ�����ϱ� */
    if(OAL_TRUE == us_need_response)
    {
        if (OAL_UNLIKELY(OAL_PTR_NULL == g_st_oam_sdt_func_hook.p_sdt_report_data_func))
        {
            if(NULL != st_msg_request.pst_resp_mem)
            {
                /*�쳣ʱ�ڴ���Ҫ�ͷ�*/
                oal_free(st_msg_request.pst_resp_mem);
            }
            return OAL_ERR_CODE_PTR_NULL;
        }

        if (NULL == st_msg_request.pst_resp_mem)
        {
            OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_recv_config_cmd::get response ptr failed!}\r\n");
            return (oal_int32)ul_ret;
        }

        pst_rsp_msg  = (wal_msg_stru *)st_msg_request.pst_resp_mem;

        us_netbuf_len = pst_rsp_msg->st_msg_hdr.us_msg_len + 1; /* +1��sdt���ߵ���Ҫ */

        us_netbuf_len = (us_netbuf_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) ? WLAN_SDT_NETBUF_MAX_PAYLOAD : us_netbuf_len;

        pst_netbuf = oam_alloc_data2sdt(us_netbuf_len);
        if (OAL_PTR_NULL == pst_netbuf)
        {
            if(NULL != st_msg_request.pst_resp_mem)
            {
                /*�쳣ʱ�ڴ���Ҫ�ͷ�*/
                oal_free(st_msg_request.pst_resp_mem);
            }
            return OAL_ERR_CODE_PTR_NULL;
        }

        oal_netbuf_data(pst_netbuf)[0] = 'M';     /* sdt��Ҫ */
        oal_memcopy(oal_netbuf_data(pst_netbuf) + 1, (oal_uint8 *)pst_rsp_msg->auc_msg_data, us_netbuf_len - 1);
        oal_free(st_msg_request.pst_resp_mem);
        st_msg_request.pst_resp_mem = NULL;

        oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_CFG, OAM_PRIMID_TYPE_DEV_ACK);
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_recv_memory_cmd
 ��������  : ���յ�SDT��ȡ����д�ڴ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ɹ����ߴ�����
 ���ú���  : SDT����
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_recv_memory_cmd(oal_uint8 *puc_buf, oal_uint16 us_len)
{
    oal_netbuf_stru            *pst_netbuf;
    wal_sdt_mem_frame_stru     *pst_mem_frame;
    oal_uint                    ul_mem_addr;    /* ��ȡ�ڴ��ַ */
    oal_uint16                  us_mem_len;     /* ��Ҫ��ȡ�ĳ��� */
    oal_uint8                   uc_offload_core_mode; /* offload�£���ʾ��һ���� */

    pst_mem_frame        = (wal_sdt_mem_frame_stru *)puc_buf;
    ul_mem_addr          = pst_mem_frame->ul_addr;
    us_mem_len           = pst_mem_frame->us_len;
    uc_offload_core_mode = pst_mem_frame->en_offload_core_mode;

    if (WAL_OFFLOAD_CORE_MODE_DMAC == uc_offload_core_mode)
    {
        /* �����offload���Σ�����Ҫ��ȡ���ڴ���wifiоƬ�࣬��Ҫ���¼����������� */
        return OAL_SUCC;
    }

    if (OAL_PTR_NULL == ul_mem_addr)            /* ��д��ַ������ */
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (us_mem_len > WAL_SDT_MEM_MAX_LEN)
    {
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }



    if (MAC_SDT_MODE_READ== pst_mem_frame->uc_mode)
    {
        if (OAL_UNLIKELY(OAL_PTR_NULL == g_st_oam_sdt_func_hook.p_sdt_report_data_func))
        {
            return OAL_ERR_CODE_PTR_NULL;
        }

        us_mem_len = (us_mem_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) ?  WLAN_SDT_NETBUF_MAX_PAYLOAD : us_mem_len;

        pst_netbuf = oam_alloc_data2sdt(us_mem_len);
        if (OAL_PTR_NULL == pst_netbuf)
        {
            return OAL_ERR_CODE_PTR_NULL;
        }

        oal_memcopy(oal_netbuf_data(pst_netbuf), (oal_void *)ul_mem_addr, us_mem_len);

        oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_MEM_RW, OAM_PRIMID_TYPE_DEV_ACK);
    }
    else if (MAC_SDT_MODE_WRITE == pst_mem_frame->uc_mode)
    {
        oal_memcopy((oal_void *)ul_mem_addr, pst_mem_frame->auc_data, us_mem_len);
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_parse_global_var_cmd
 ��������  : ����ȫ�ֱ�����д�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_parse_global_var_cmd(
                wal_sdt_global_var_stru    *pst_global_frame,
                oal_uint                   ul_global_var_addr)
{
    oal_netbuf_stru            *pst_netbuf;
    oal_uint16                  us_skb_len;

    if (MAC_SDT_MODE_WRITE == pst_global_frame->uc_mode)
    {
        oal_memcopy((oal_void *)ul_global_var_addr, (oal_void *)(pst_global_frame->auc_global_value), pst_global_frame->us_len);
    }
    else if (MAC_SDT_MODE_READ == pst_global_frame->uc_mode)
    {
        if (OAL_UNLIKELY(OAL_PTR_NULL == g_st_oam_sdt_func_hook.p_sdt_report_data_func))
        {
            return OAL_ERR_CODE_PTR_NULL;
        }

        us_skb_len = pst_global_frame->us_len;

        us_skb_len = (us_skb_len > WLAN_SDT_NETBUF_MAX_PAYLOAD) ? WLAN_SDT_NETBUF_MAX_PAYLOAD : us_skb_len;

        pst_netbuf = oam_alloc_data2sdt(us_skb_len);
        if (OAL_PTR_NULL == pst_netbuf)
        {
            return OAL_ERR_CODE_PTR_NULL;
        }

        oal_memcopy(oal_netbuf_data(pst_netbuf), (oal_void *)ul_global_var_addr, us_skb_len);
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
        oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_GVAR_RW, OAM_PRIMID_TYPE_DEV_ACK);
#else
        oam_report_data2sdt(pst_netbuf, OAM_DATA_TYPE_MEM_RW, OAM_PRIMID_TYPE_DEV_ACK);
#endif
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_recv_global_var_cmd
 ��������  : ���ն�дȫ�ֱ������������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  : SDT����
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_recv_global_var_cmd(oal_uint8 *puc_buf, oal_uint16 us_len)
{
    wal_sdt_global_var_stru        *pst_global_frame;
    oal_uint                       ul_global_var_addr;

    if (OAL_PTR_NULL == puc_buf)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_global_frame = (wal_sdt_global_var_stru *)puc_buf;

    if (WAL_OFFLOAD_CORE_MODE_DMAC == pst_global_frame->en_offload_core_mode)
    {
        /* offload���Σ�����Ҫ��ȡ��ȫ�ֱ�����wifiоƬ�࣬��Ҫ���¼����������� */
        return OAL_SUCC;
    }

    ul_global_var_addr = oal_kallsyms_lookup_name(pst_global_frame->auc_global_value_name);
    if (0 == ul_global_var_addr)    /* not found */
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_recv_global_var_cmd::kernel lookup global var address returns 0!}\r\n");
        return OAL_FAIL;
    }

    return wal_parse_global_var_cmd(pst_global_frame, ul_global_var_addr);
}

/*****************************************************************************
 �� �� ��  : wal_recv_reg_cmd
 ��������  : �յ�SDT�·��Ķ�д�Ĵ����������Ƕ��Ĵ���������Ҫ���������Ľ�
             ��ͨ���ص����ظ�SDT�������д�Ĵ������������һ��㷢��ȥ��hal
             ��д�Ϳ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ɹ����ߴ�����
 ���ú���  : SDT����
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��10��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_recv_reg_cmd(oal_uint8 *puc_buf, oal_uint16 us_len)
{
    oal_int8                     ac_vap_name[OAL_IF_NAME_SIZE];
    oal_net_device_stru         *pst_net_dev;
    mac_vap_stru                *pst_mac_vap;
    wal_sdt_reg_frame_stru      *pst_reg_frame;
    oal_int32                    l_ret;
    hmac_vap_cfg_priv_stru      *pst_cfg_priv;
    oal_netbuf_stru             *pst_net_buf;
    oal_uint32                   ul_ret;

    oal_memcopy(ac_vap_name, puc_buf, OAL_IF_NAME_SIZE);
    ac_vap_name[OAL_IF_NAME_SIZE - 1] = '\0';   /* ��ֹ�ַ����쳣 */

    /* ����dev_name�ҵ�dev */
    pst_net_dev = oal_dev_get_by_name(ac_vap_name);
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_recv_reg_cmd::oal_dev_get_by_name return null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    oal_dev_put(pst_net_dev);   /* ����oal_dev_get_by_name�󣬱������oal_dev_putʹnet_dev�����ü�����һ */

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);    /* ��ȡmac vap */

    ul_ret = hmac_vap_get_priv_cfg(pst_mac_vap, &pst_cfg_priv);      /* ȡ����˽�нṹ�� */
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_recv_reg_cmd::hmac_vap_get_priv_cfg return null_ptr_err!}\r\n");
        return (oal_int32)ul_ret;
    }

    pst_cfg_priv->en_wait_ack_for_sdt_reg = OAL_FALSE;

    ul_ret = hmac_sdt_recv_reg_cmd(pst_mac_vap, puc_buf, us_len);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_recv_reg_cmd::hmac_sdt_recv_reg_cmd return error code!}\r\n");

        return (oal_int32)ul_ret;
    }

    pst_reg_frame = (wal_sdt_reg_frame_stru *)puc_buf;

    if (MAC_SDT_MODE_READ == pst_reg_frame->uc_mode)
    {
        wal_wake_lock();
        /*lint -e730*//* info, boolean argument to function */
        l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_cfg_priv->st_wait_queue_for_sdt_reg,
                                                     (OAL_TRUE == pst_cfg_priv->en_wait_ack_for_sdt_reg),
                                                     2 * OAL_TIME_HZ);
        /*lint +e730*/
        /*lint -e774*//* ���WIN32�汾�µ�PC LINT���� */
        if (0 == l_ret)
        {
            /* ��ʱ */
            OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_recv_reg_cmd::wal_netdev_open: wait queue timeout!}\r\n");
            wal_wake_unlock();
            return -OAL_EINVAL;
        }
        else if (l_ret < 0)
        {
            /* �쳣 */
            OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_recv_reg_cmd::wal_netdev_open: wait queue error!}\r\n");
            wal_wake_unlock();
            return -OAL_EINVAL;
        }
        wal_wake_unlock();
        /*lint +e774*/

        /* ��ȡ���صļĴ���ֵ */
        pst_reg_frame->ul_reg_val = *((oal_uint32 *)(pst_cfg_priv->ac_rsp_msg));

        if (OAL_UNLIKELY(OAL_PTR_NULL != g_st_oam_sdt_func_hook.p_sdt_report_data_func))
        {
            pst_net_buf = oam_alloc_data2sdt((oal_uint16)OAL_SIZEOF(wal_sdt_reg_frame_stru));
            if (OAL_PTR_NULL == pst_net_buf)
            {
                return OAL_ERR_CODE_PTR_NULL;
            }

            oal_memcopy(oal_netbuf_data(pst_net_buf), (oal_uint8 *)pst_reg_frame, (oal_uint16)OAL_SIZEOF(wal_sdt_reg_frame_stru));

            oam_report_data2sdt(pst_net_buf, OAM_DATA_TYPE_REG_RW, OAM_PRIMID_TYPE_DEV_ACK);
        }
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_drv_cfg_func_hook_init
 ��������  : wal���⹳�Ӻ�����ʼ��
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void wal_drv_cfg_func_hook_init(oal_void)
{
    g_st_wal_drv_func_hook.p_wal_recv_cfg_data_func     = wal_recv_config_cmd;
    g_st_wal_drv_func_hook.p_wal_recv_mem_data_func     = wal_recv_memory_cmd;
    g_st_wal_drv_func_hook.p_wal_recv_reg_data_func     = wal_recv_reg_cmd;
    g_st_wal_drv_func_hook.p_wal_recv_global_var_func   = wal_recv_global_var_cmd;
}


#ifdef _PRE_DEBUG_MODE
/*****************************************************************************
 �� �� ��  : wal_config_rx_filter_val
 ��������  : wal����/��ȡָ��״̬��֡����ֵ
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_rx_filter_val(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_rx_filter_val(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_rx_filter_en
 ��������  : wal����֡����ʹ�ܿ���
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_rx_filter_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_rx_filter_en(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_get_rx_filter_en
 ��������  : wal��ȡ֡����ʹ�ܿ���״̬
 �������  : pst_mac_vap: MAC VAP�ṹ��ָ��
             us_len     : ��������
             puc_param  : ����ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_rx_filter_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_rx_filter_en(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_report_ampdu_stat
 ��������  : �ϱ���ȡ����ĳһ��tid��ampduҵ��ͳ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_report_ampdu_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_report_ampdu_stat(pst_mac_vap, us_len, puc_param);
}

#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_ampdu_aggr_num
 ��������  : ���þۺ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��10��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_ampdu_aggr_num(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_ampdu_aggr_num(pst_mac_vap, us_len, puc_param);
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
/*****************************************************************************
 �� �� ��  : wal_config_set_ampdu_mmss
 ��������  : ���þۺϵ�mmss
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_ampdu_mmss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_ampdu_mmss(pst_mac_vap, us_len, puc_param);
}
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
/*****************************************************************************
 �� �� ��  : wal_config_freq_adjust
 ��������  : ����Ƶƫ��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��10��29��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_freq_adjust(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_freq_adjust(pst_mac_vap, us_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_stbc_cap
 ��������  : ����AP��STBC����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_stbc_cap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_stbc_cap(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_ldpc_cap
 ��������  : ����AP��LDPC����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_ldpc_cap(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_ldpc_cap(pst_mac_vap, us_len, puc_param);
}

#ifdef _PRE_WLAN_FEATURE_MCAST
/*****************************************************************************
 �� �� ��  : wal_config_m2u_snoop_on
 ��������  : ���鲥ת������snoop����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_m2u_snoop_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_m2u_snoop_on_param_stru *pst_m2u_snoop_on_param;
    hmac_vap_stru *pst_hmac_vap;
    hmac_m2u_stru *pst_m2u;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    pst_m2u = (hmac_m2u_stru *)(pst_hmac_vap->pst_m2u);

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_M2U, "{wal_config_m2u_snoop_on:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_m2u_snoop_on_param = (mac_cfg_m2u_snoop_on_param_stru *)puc_param;

    /* uc_m2u_snoop_on */
    if (1 == pst_m2u_snoop_on_param->uc_m2u_snoop_on || 0 == pst_m2u_snoop_on_param->uc_m2u_snoop_on)
    {
        pst_m2u->en_snoop_enable = pst_m2u_snoop_on_param->uc_m2u_snoop_on;
    }
    else
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_M2U, "{wal_config_m2u_snoop_on::pst_m2u_snoop_on_param->uc_m2u_snoop_on %d!}\r\n", pst_m2u_snoop_on_param->uc_m2u_snoop_on);
        return OAL_FAIL;
    }

    /* uc_m2u_mcast_mode */
    if (1 == pst_m2u_snoop_on_param->uc_m2u_mcast_mode || 0 == pst_m2u_snoop_on_param->uc_m2u_mcast_mode ||
                2 == pst_m2u_snoop_on_param->uc_m2u_mcast_mode)
    {
        pst_m2u->en_mcast_mode = pst_m2u_snoop_on_param->uc_m2u_mcast_mode;
    }
    else
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_M2U, "{wal_config_m2u_snoop_on::pst_m2u_snoop_on_param->uc_m2u_mcast_mode %d!}\r\n", pst_m2u_snoop_on_param->uc_m2u_mcast_mode);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_add_m2u_deny_table
 ��������  : �����鲥�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_add_m2u_deny_table(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_add_m2u_deny_table_stru *pst_m2u_deny_table_param;
    hmac_vap_stru *pst_hmac_vap;
    oal_uint32  ul_deny_group_addr1;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_M2U, "{wal_add_m2u_deny_table:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_m2u_deny_table_param = (mac_add_m2u_deny_table_stru *)puc_param;

    ul_deny_group_addr1 = oal_byteorder_host_to_net_uint32(pst_m2u_deny_table_param->ul_deny_group_addr);

    if (HMAC_M2U_MIN_DENY_GROUP <= ul_deny_group_addr1 && HMAC_M2U_MAX_DENY_GROUP >= ul_deny_group_addr1)
    {
        hmac_m2u_add_snoop_deny_entry(pst_hmac_vap, &ul_deny_group_addr1);
    }
    else
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_M2U, "{wal_add_m2u_deny_table::pst_m2u_deny_table_param->ul_deny_group_addr %x!}\r\n", pst_m2u_deny_table_param->ul_deny_group_addr);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_m2u_deny_table
 ��������  : �����鲥����������� 1 0
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_m2u_deny_table(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_clg_m2u_deny_table_stru *pst_clg_m2u_deny_table_param;
    hmac_vap_stru *pst_hmac_vap;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_M2U, "{wal_config_m2u_deny_table:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_clg_m2u_deny_table_param = (mac_clg_m2u_deny_table_stru *)puc_param;

    /* ����鲥������� */
    if (1 == pst_clg_m2u_deny_table_param->uc_m2u_clear_deny_table)
    {
        hmac_m2u_clear_deny_table(pst_hmac_vap);
    }
    /* show�鲥������� */
    if (1 == pst_clg_m2u_deny_table_param->uc_m2u_show_deny_table)
    {
        hmac_m2u_show_snoop_deny_table(pst_hmac_vap);
    }
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_show_m2u_snoop_table
 ��������  : ��ʾsnoop��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_show_m2u_snoop_table(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_show_m2u_snoop_table_stru *pst_show_m2u_snoop_table_param;
    hmac_vap_stru *pst_hmac_vap;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_M2U, "{wal_show_m2u_snoop_table:: pst_mac_vap/puc_param is null ptr %d, %d!}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_show_m2u_snoop_table_param = (mac_show_m2u_snoop_table_stru *)puc_param;

    /* show snoop �� */
    if (1 == pst_show_m2u_snoop_table_param->uc_m2u_show_snoop_table)
    {
        hmac_m2u_print_all_snoop_list(pst_hmac_vap);
    }
    else
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_M2U, "{wal_show_m2u_snoop_table::pst_m2u_snoop_on_param->uc_m2u_snoop_on %d!}\r\n", pst_show_m2u_snoop_table_param->uc_m2u_show_snoop_table);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_config_igmp_packet_xmit
 ��������  : igmp���ķ�����������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_igmp_packet_xmit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_cfg_mpdu_ampdu_tx_param_stru *pst_ampdu_tx_on_param;
    oal_uint8                       uc_skb_num;
    oal_uint8                       uc_skb_idx;
    oal_uint8                       uc_tid;
    oal_uint16                      us_packet_len;
    oal_net_device_stru            *pst_dev;
    oal_netbuf_stru                *past_netbuf[32] = {OAL_PTR_NULL};

    pst_ampdu_tx_on_param = (mac_cfg_mpdu_ampdu_tx_param_stru *)puc_param;
    uc_skb_num            = pst_ampdu_tx_on_param->uc_packet_num;
    uc_tid                = pst_ampdu_tx_on_param->uc_tid;
    us_packet_len         = pst_ampdu_tx_on_param->us_packet_len;

    pst_dev = hmac_vap_get_net_device(pst_mac_vap->uc_vap_id);

    if (uc_skb_num <= 32)
    {
        for (uc_skb_idx = 0; uc_skb_idx < uc_skb_num; uc_skb_idx++)
        {
            past_netbuf[uc_skb_idx] = wal_config_create_igmp_packet(us_packet_len, uc_tid,
                                                    pst_ampdu_tx_on_param->auc_ra_mac,
                                                    pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.auc_dot11StationID);
            #ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
                wal_vap_start_xmit(past_netbuf[uc_skb_idx], pst_dev);
            #else
                wal_bridge_vap_xmit(past_netbuf[uc_skb_idx], pst_dev);
            #endif

        }


    }
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_wifi_enable
 ��������  : ʹ��/ȥʹ��wifiоƬ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_PM
OAL_STATIC oal_uint32  wal_config_wifi_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32    ul_ret;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_config_wifi_enable::pst_mac_vap or puc_param null ptr error [%x],[%x].}\r\n", pst_mac_vap, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_wifi_enable(pst_mac_vap, us_len, puc_param);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_config_wifi_enable:: return error code [%d].}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_config_pm_info
 ��������  : PM��Ϣ���������ά��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/

OAL_STATIC oal_uint32  wal_config_pm_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_PM_INFO,us_len, puc_param);
}

OAL_STATIC oal_uint32  wal_config_pm_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap,WLAN_CFGID_PM_EN,us_len, puc_param);
}

#endif


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_config_show_device_memleak(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_DEVICE_MEM_LEAK, us_len, puc_param);
}

OAL_STATIC oal_uint32  wal_config_show_device_meminfo(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_DEVICE_MEM_INFO, us_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_remain_on_channel
 ��������  : ������ָ���ŵ�
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��22��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_remain_on_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_remain_on_channel(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_cancel_remain_on_channel
 ��������  : ֹͣ������ָ���ŵ�
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��22��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_cancel_remain_on_channel(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_cancel_remain_on_channel(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_set_sta_pm_mode
 ��������  : ����sta���ܵ�ģʽ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_STA_PM
OAL_STATIC oal_uint32  wal_config_set_sta_pm_mode(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_config_set_sta_pm_mode(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_show_pm_info
 ��������  : sta psm��ͳ��ά����Ϣ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��16��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_PSM_DEBUG_MODE
OAL_STATIC oal_uint32  wal_config_show_pm_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_SHOW_PS_INFO, us_len, puc_param);
}
#endif

OAL_STATIC oal_uint32  wal_config_set_pm_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_sync_cmd_common(pst_mac_vap, WLAN_CFGID_SET_PSM_PARAM, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_set_sta_pm_on
 ��������  : ��sta device �͹���
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_sta_pm_on(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    hmac_vap_stru                *pst_hmac_vap;
    mac_cfg_ps_open_stru         *pst_sta_pm_open;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        OAM_WARNING_LOG0(0, OAM_SF_PWR, "{wal_config_set_sta_pm_on::pst_mac_vap / puc_param null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap    = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_config_set_sta_pm_on::pst_hmac_vap null,vap state[%d].}",pst_mac_vap->en_vap_state);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_sta_pm_open = (mac_cfg_ps_open_stru *)puc_param;

    /* ����ϲ�����dhcp�ɹ���ʱȡ����ʱ���͹��ĵĶ�ʱ�� */
    if((OAL_TRUE == pst_hmac_vap->st_ps_sw_timer.en_is_registerd) && (pst_sta_pm_open->uc_pm_enable > MAC_STA_PM_SWITCH_OFF))
    {
        FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&(pst_hmac_vap->st_ps_sw_timer));
    }

    return hmac_config_set_sta_pm_on(pst_mac_vap, us_len, puc_param);
}

#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_uapsd_para
 ��������  : ����sta uapsd�Ĳ���
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
OAL_STATIC oal_uint32 wal_config_set_uapsd_para(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param)))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_config_set_uapsd_para(pst_mac_vap, us_len, puc_param);
}
#endif
/*****************************************************************************
 �� �� ��  : wal_config_mgmt_tx
 ��������  : ���͹���֡
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_mgmt_tx(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_wpas_mgmt_tx(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_vap_classify_en
 ��������  : ����vap���ȼ��Ƿ�ʹ��
 �������  :
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_vap_classify_en(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_vap_classify_en(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_vap_classify_tid
 ��������  : ����vap���ȼ���tid
 �������  :
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_vap_classify_tid(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_vap_classify_tid(pst_mac_vap, us_len, puc_param);
}
#ifdef _PRE_DEBUG_MODE
/*****************************************************************************
 �� �� ��  : wal_config_scan_test
 ��������  : ɨ��ģ���������
 �������  :
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_scan_test(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_scan_test(pst_mac_vap, us_len, puc_param);
}
#endif

/*****************************************************************************
 �� �� ��  : wal_config_bgscan_enable
 ��������  : ɨ��ģ���������
 �������  :
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��2��
    ��    ��   : W00346925
    �޸�����   : �����ɺ���
*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_bgscan_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_bgscan_enable(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_query_station_stats
 ��������  : ��ѯstation����Ϣ
 �������  : mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��3��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
OAL_STATIC oal_uint32 wal_config_query_station_stats(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_query_station_info(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_HS20
/*****************************************************************************
 �� �� ��  : wal_config_set_qos_map
 ��������  : ����QoSMap����
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_qos_map(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_qos_map(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
/*****************************************************************************
 �� �� ��  : wal_config_set_p2p_ps_ops
 ��������  : ����P2P OPS ����
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��14��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_p2p_ps_ops(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_p2p_ps_ops(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_p2p_ps_noa
 ��������  : ����P2P NOA ����
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��14��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_p2p_ps_noa(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_p2p_ps_noa(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_set_p2p_ps_stat
 ��������  : ����P2P ����ͳ�ƿ���
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��1��14��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_p2p_ps_stat(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_p2p_ps_stat(pst_mac_vap, us_len, puc_param);
}
#endif

#ifdef _PRE_WLAN_PROFLING_MIPS
/*****************************************************************************
 �� �� ��  : wal_config_set_mips
 ��������  :
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_mips(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_mips(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_show_mips
 ��������  :
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_show_mips(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_show_mips(pst_mac_vap, us_len, puc_param);
}
#endif


#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
/*****************************************************************************
 �� �� ��  : wal_config_arp_offload_enable
 ��������  : ��/�ر�ARP offload����
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_enable_arp_offload(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_enable_arp_offload(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_show_arpoffload_info
 ��������  : ��ʾDevice���¼��IP��ַ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��8��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_show_arpoffload_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_show_arpoffload_info(pst_mac_vap, us_len, puc_param);
}
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32 wal_config_cfg_vap_h2d(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_cfg_vap_h2d(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_host_dev_init
 ��������  : ��ʼ��host device_struc�ṹ�壬Ŀǰ�������µ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_host_dev_init(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_host_dev_init(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_host_dev_exit
 ��������  : ȥ��ʼ��host device_struc�ṹ�壬Ŀǰ�������µ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_host_dev_exit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_host_dev_exit(pst_mac_vap);
}



oal_uint32 wal_send_cali_data(oal_net_device_stru *pst_net_dev)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

    mac_vap_stru               *pst_mac_vap;
    oal_uint8                  *puc_param = OAL_PTR_NULL;

    mac_device_stru            *pst_mac_device;

    pst_mac_device      = mac_res_get_dev(0);
    if(NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap::pst_mac_device is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->uc_cfg_vap_id);

    hmac_send_cali_data(pst_mac_vap, 0, puc_param);
#endif
    return OAL_SUCC;
}

#endif
#ifdef _PRE_WLAN_TCP_OPT
/*****************************************************************************
 �� �� ��  : wal_config_get_tcp_ack_stream_info
 ��������  : ��ѯTCP ACK����ͳ��ֵ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_get_tcp_ack_stream_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_tcp_ack_stream_info(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_tx_tcp_ack_opt_enable
 ��������  : ���÷���TCP ACK�Ż�ʹ��
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_tx_tcp_ack_opt_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_tx_tcp_ack_opt_enable(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_rx_tcp_ack_opt_enable
 ��������  : ���ý���TCP ACK�Ż�ʹ��
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_rx_tcp_ack_opt_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_rx_tcp_ack_opt_enable(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_tx_tcp_ack_limit
 ��������  : ���÷���ack limit
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_tx_tcp_ack_limit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_tx_tcp_ack_limit(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_rx_tcp_ack_limit
 ��������  : ���ý���ack limit
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_rx_tcp_ack_limit(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_rx_tcp_ack_limit(pst_mac_vap, us_len, puc_param);
}
#endif

#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_DFT_STAT
/*****************************************************************************
 �� �� ��  : wal_config_set_performance_log_switch
 ��������  : �������ܴ�ӡ���ƿ���
 �������  :
 �������  :
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��8��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_performance_log_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_performance_log_switch(pst_mac_vap, WLAN_CFGID_SET_PERFORMANCE_LOG_SWITCH, us_len, puc_param);
}
#endif
#endif

/*****************************************************************************
 �� �� ��  : wal_config_set_max_user
 ��������  : ��������û���
 �������  : pst_net_dev: net device
             p_param    : ����
 �������  : ��
 �� �� ֵ  : ������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_max_user(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_uint32          ul_max_user;

    ul_max_user = *((oal_uint32 *)puc_param);
    if((IS_P2P_GO(pst_mac_vap) && (ul_max_user > WLAN_P2P_GO_ASSOC_USER_MAX_NUM_SPEC))||
        (ul_max_user > WLAN_ASSOC_USER_MAX_NUM_SPEC))
    {
        /*
        OAM_ERROR_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_config_set_max_user:: AP/GO want to set the max_user_value[%d] more than the threshold[%d]!}\r\n",
                ul_max_user, (IS_P2P_GO(pst_mac_vap) ? WLAN_P2P_GO_ASSOC_USER_MAX_NUM_SPEC : WLAN_ASSOC_USER_MAX_NUM_SPEC));
        return OAL_FAIL;
        */
        ul_max_user = WLAN_ASSOC_USER_MAX_NUM_SPEC;
    }
    return hmac_config_set_max_user(pst_mac_vap, us_len, ul_max_user);
}
#ifdef _PRE_WLAN_FEATURE_ROAM
/*****************************************************************************
�� �� ��  : wal_config_roam_enable
��������  : ��/�ر����ι���
�������  : mac_vap_stru *pst_mac_vap
         oal_uint16 us_len
         oal_uint8 *puc_param
�������  : ��
�� �� ֵ  : OAL_STATIC oal_uint32
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2015��6��11��
��    ��   : 
�޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_roam_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_roam_enable(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
�� �� ��  : wal_config_roam_band
��������  : ��������Ƶ��
�������  : mac_vap_stru *pst_mac_vap
         oal_uint16 us_len
         oal_uint8 *puc_param
�������  : ��
�� �� ֵ  : OAL_STATIC oal_uint32
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2015��6��11��
��    ��   : 
�޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_roam_band(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_roam_band(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
�� �� ��  : wal_config_roam_org
��������  : �������������ŵ�
�������  : mac_vap_stru *pst_mac_vap
         oal_uint16 us_len
         oal_uint8 *puc_param
�������  : ��
�� �� ֵ  : OAL_STATIC oal_uint32
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2015��6��11��
��    ��   : 
�޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_roam_org(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_roam_org(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_roam_start
 ��������  : �������ι���
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_roam_start(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_roam_start(pst_mac_vap, us_len, puc_param);
}


/*****************************************************************************
 �� �� ��  : wal_config_roam_info
 ��������  : �������ι���
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_roam_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_roam_info(pst_mac_vap, us_len, puc_param);
}
#endif //_PRE_WLAN_FEATURE_ROAM

#ifdef _PRE_WLAN_FEATURE_11R
/*****************************************************************************
 �� �� ��  : wal_config_set_ft_ies
 ��������  : ����ft ies
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��8��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_ft_ies(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_ft_ies(pst_mac_vap, us_len, puc_param);
}
#endif //_PRE_WLAN_FEATURE_11R

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
/*****************************************************************************
 �� �� ��  : wal_config_enable_2040bss
 ��������  : 20/40 bss��⿪��
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��8��31��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_enable_2040bss(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_enable_2040bss(pst_mac_vap, us_len, puc_param);
}
#endif /* _PRE_WLAN_FEATURE_20_40_80_COEXIST */
/*****************************************************************************
 �� �� ��  : wal_config_set_auto_freq_enable
 ��������  : �����Զ���Ƶʹ��
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��16��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
OAL_STATIC oal_uint32  wal_config_set_auto_freq_enable(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_auto_freq_enable(pst_mac_vap, us_len, puc_param);
}
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
/*****************************************************************************
 �� �� ��  : wal_config_get_lauch_cap
 ��������  : ʵ�ʷ���������ѯ
 �������  : mac_vap_stru *pst_mac_vap
             oal_uint16 us_len
             oal_uint8 *puc_param
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��27��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_get_lauch_cap(mac_vap_stru *pst_mac_vap, oal_uint16 *us_len, oal_uint8 *puc_param)
{
    return hmac_config_get_lauch_cap(pst_mac_vap, us_len, puc_param);
}

/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_linkloss_threshold(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_linkloss_threshold(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_all_log_level(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_all_log_level(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_d2h_hcc_assemble_cnt(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_d2h_hcc_assemble_cnt(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_chn_est_ctrl(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_chn_est_ctrl(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_power_ref(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_power_ref(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_pm_cfg_param(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_pm_cfg_param(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_cus_rf(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_cus_rf(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_cus_dts_cali(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_cus_dts_cali(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_cus_nvram_params(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_cus_nvram_params(pst_mac_vap, us_len, puc_param);
}
/*****************************************************************************
 �� �� ��  : wal_config_dev_customize_info
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_SUCC �� ʧ�ܴ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
/* show dev customize info */
OAL_STATIC oal_uint32  wal_config_dev_customize_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_dev_customize_info(pst_mac_vap, us_len, puc_param);
}
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

/*****************************************************************************
 �� �� ��  : wal_config_vap_destroy
 ��������  : down����ɾ��vap
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_vap_destroy(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    hmac_vap_stru *pst_hmac_vap;
    pst_hmac_vap  = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    return hmac_vap_destroy(pst_hmac_vap);
}

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
/*****************************************************************************
 �� �� ��  : wal_config_set_tx_classify_switch
 ��������  : ����ҵ��ʶ���ܿ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��23��
    ��    ��   : ()
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_config_set_tx_classify_switch(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_tx_classify_switch(pst_mac_vap, us_len, puc_param);
}
#endif  /* _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN */

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
/*****************************************************************************
 �� �� ��  : wal_config_set_rekey_info
 ��������  : WAL�ڶ���,����hmac��ʵ������rekey offload
 �������  :
 �������  : oal_uint32
 �� �� ֵ  : 0:�ɹ�,����:ʧ��
 ���ú���  : ��
 ��������  : ��


�޸���ʷ      :
  1.��    ��   : 2016��8��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32 wal_config_set_rekey_info(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    return hmac_config_set_rekey_info(pst_mac_vap, us_len, puc_param);
}
#endif

/*lint -e19*/
oal_module_symbol(wal_drv_cfg_func_hook_init);
/*lint +e19*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

