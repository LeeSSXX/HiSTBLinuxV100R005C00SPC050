/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hal_romcb.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��4��8��
  ����޸�   :
  ��������   : ƽ̨RAM�ӿ�ӳ��,ֻ���������Ƶ��RAM�ӿ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��4��8��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_ROMCB_C

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV)


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "hal_romcb.h"

/*****************************************************************************
  2 ��������
*****************************************************************************/
/*RF�ⲿ���ú���*/
#if (!defined(_PRE_PC_LINT) && !defined(WIN32))
extern oal_void hi1102_get_pwr_comp_val_ram(hal_to_dmac_device_stru *pst_hal_device, oal_uint32 ul_tx_ratio, oal_int16 * ps_pwr_comp_val);
#endif
extern oal_uint32  hi1102_rf_cali_get_scaling_addr(oal_uint8 uc_offset_addr_a);


extern oal_void  hi1102_rf_set_channel_info_ram(hal_device_stru                    *pst_device,
                                                wlan_channel_band_enum_uint8        en_band,
                                                oal_uint8                           uc_channel_idx,
                                                wlan_channel_bandwidth_enum_uint8   en_bandwidth,
                                                oal_uint8                           uc_rf_channel_num);

extern oal_void hi1102_psm_rf_sleep_ram(hal_to_dmac_device_stru * pst_hal_device, oal_uint8 uc_restore_reg);


extern oal_void hi1102_psm_rf_awake_ram(hal_to_dmac_device_stru  *pst_hal_device, oal_uint8 uc_restore_reg);

extern oal_void hi1102_initialize_rf_sys_ram(hal_to_dmac_device_stru *pst_hal_device);

#ifdef _PRE_WLAN_RF_110X_CALI_DPD

extern oal_void hi1102_dpd_cali_func_ram(oal_uint8 u_functype, oal_uint32* pul_status, oal_uint32* hi1102_dpd_cal_data_read, oal_uint32* hi1102_dpd_cal_data_write);
#endif
extern oal_void hi1102_rf_get_rfhw_stat_info_ram(hal_device_stru * pst_hal_device,oal_uint16 *pst_rfhw_stat);



/*****************************************************************************
  3 ȫ�ֱ�������
*****************************************************************************/


wifi_cb_func ROM_CALLBACK g_pfa_wifi_rf_func[WIFI_RF_FUNC_BUTT] =
{
                #if (!defined(_PRE_PC_LINT) && !defined(WIN32))
                    (wifi_cb_func)(hi1102_get_pwr_comp_val_ram),            // BT_PLATFORM_SUBSYS_SEND_MSG
                #endif
                    (wifi_cb_func)(hi1102_rf_set_channel_info_ram),         // hi1102_rf_set_channel_info
                    (wifi_cb_func)(hi1102_psm_rf_sleep_ram),                // hi1102_psm_rf_sleep
                    (wifi_cb_func)(hi1102_psm_rf_awake_ram),                // hi1102_psm_rf_awake
                    (wifi_cb_func)(hi1102_initialize_rf_sys_ram),           // hi1102_initialize_rf_sys
                #ifdef _PRE_WLAN_RF_110X_CALI_DPD
                    (wifi_cb_func)(hi1102_dpd_cali_func_ram),               // hi1102_dpd_cali_func
                #endif
                    (wifi_cb_func)(hi1102_rf_get_rfhw_stat_info_ram)
};

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


