/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : mac_device.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2012��10��19��
  ����޸�   :
  ��������   : board��chip��device��Ӧ��������ʵ�ֵ�Դ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��10��19��
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
#include "oam_ext_if.h"
#include "frw_ext_if.h"
#include "wlan_spec.h"
#include "hal_ext_if.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "dmac_reset.h"
#include "mac_vap.h"
#ifdef _PRE_WLAN_ALG_ENABLE
#include "alg_dbac.h"
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_DEVICE_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
extern oal_uint32 band_5g_enabled;
#endif

/* 1131debug: mac_device_init   Ԥ���ӿ� */
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV)
oal_bool_enum_uint8 mac_device_init_adjust(mac_device_stru *, oal_uint32, oal_uint8,oal_uint8);
typedef oal_bool_enum_uint8 (*mac_device_init_adjust_type)(mac_device_stru *pst_mac_device, oal_uint32 ul_chip_ver, oal_uint8 uc_chip_id, oal_uint8 uc_device_id);
mac_device_init_adjust_type g_fn_mac_device_init_adjust_patch_ram = mac_device_init_adjust;
#endif

#if 0
/*****************************************************************************
 �� �� ��  : dmac_dfr_tx_prot_timeout_fn
 ��������  : �޷�������ж϶�ʱ����ʱ����
 �������  : p_arg: ָ��MAC DEVICE��ָ��
 �������  : ��
 �� �� ֵ  : OAL_SUCC������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��7��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  dmac_dfr_tx_prot_timeout_fn(oal_void *p_arg)
{
    mac_device_stru           *pst_mac_device;
    hal_to_dmac_device_stru   *pst_hal_device;
    hal_dfr_tx_prot_stru      *pst_dfr_tx_prot;
    dmac_reset_para_stru       st_reset_param;
    oal_uint16                 us_total_ppdu_num = 0;
    oal_bool_enum_uint8        en_rlst;

    pst_mac_device = (mac_device_stru *)p_arg;
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hal_device = pst_mac_device->pst_device_stru;
    pst_dfr_tx_prot = &(pst_hal_device->st_dfr_tx_prot);

    /* ������Ͷ�����û�����ݣ���ֱ�ӷ��� */
    hal_get_tx_dscr_queue_total_ppdu_num(pst_hal_device, &us_total_ppdu_num);
    if (0 == us_total_ppdu_num)
    {
        pst_dfr_tx_prot->en_tx_prot_timer_step = MAC_DFR_TIMER_STEP_1;
        return OAL_SUCC;
    }

    /* ���������ɶ��л��д������¼���˵�����з�������жϲ�����ֱ�ӷ��� */
    en_rlst = frw_is_event_queue_empty(FRW_EVENT_TYPE_WLAN_TX_COMP);
    if (OAL_FALSE == en_rlst)
    {
        return OAL_SUCC;
    }

    st_reset_param.uc_reset_type    = HAL_RESET_HW_TYPE_ALL;
    st_reset_param.uc_reset_mac_mod = HAL_RESET_MAC_ALL;
    st_reset_param.uc_reset_mac_reg = OAL_FALSE;
    st_reset_param.uc_reset_phy_reg = OAL_FALSE;
    st_reset_param.uc_reason        = DMAC_RESET_REASON_ERROR;

    if (MAC_DFR_TIMER_STEP_1 == pst_dfr_tx_prot->en_tx_prot_timer_step)         /* ��һ�γ�ʱ����λMAC + PHY�߼� */
    {
        pst_dfr_tx_prot->en_tx_prot_timer_step = MAC_DFR_TIMER_STEP_2;
    }
    else if (MAC_DFR_TIMER_STEP_2 == pst_dfr_tx_prot->en_tx_prot_timer_step)    /* �ڶ��γ�ʱ����λMAC + PHY�߼��ͼĴ��� */
    {
        pst_dfr_tx_prot->en_tx_prot_timer_step = MAC_DFR_TIMER_STEP_1;

        st_reset_param.uc_reset_mac_reg = OAL_TRUE;
        st_reset_param.uc_reset_phy_reg = OAL_TRUE;
    }
    else
    {
        pst_dfr_tx_prot->en_tx_prot_timer_step = MAC_DFR_TIMER_STEP_1;
    }

    /* ��λ */
    dmac_reset_hw(pst_mac_device, (oal_uint8 *)&st_reset_param);
    if ((OAL_TRUE == st_reset_param.uc_reset_mac_reg) && (OAL_TRUE == st_reset_param.uc_reset_phy_reg))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY,
                         "{dmac_dfr_tx_prot_timeout_fn:: reset mac phy and register, when second found tx complete isr timeout.}");
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY,
                         "{dmac_dfr_tx_prot_timeout_fn:: reset mac and phy, when first found tx complete isr timeout.}");
    }

    /* ������ʱ�� */
    FRW_TIMER_RESTART_TIMER(&(pst_dfr_tx_prot->st_tx_prot_timer), WLAN_TX_PROT_TIMEOUT, OAL_TRUE);

    return OAL_SUCC;
}
#endif

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


/*****************************************************************************
 �� �� ��  : dmac_device_exit
 ��������  : ж��device�Ĳ�������(����֮ǰ���Ѵ�chip��ɾ��)
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ɹ�����ʧ��ԭ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_exit(mac_device_stru *pst_device)
{
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_device))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{dmac_device_exit::pst_device null.}");

        return OAL_ERR_CODE_PTR_NULL;
    }

#if 0
	/* ע���޷�������жϼ�ⶨʱ�� */
    FRW_TIMER_UNREGISTER_TIMER(&(pst_device->pst_device_stru->st_dfr_tx_prot.st_tx_prot_timer));
#endif



    pst_device->uc_vap_num = 0;
    pst_device->uc_sta_num = 0;
    pst_device->st_p2p_info.uc_p2p_device_num   = 0;
    pst_device->st_p2p_info.uc_p2p_goclient_num = 0;

    mac_res_free_dev(pst_device->uc_device_id);

#if 0
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    pst_device->en_device_state = OAL_FALSE;
#endif
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_device_set_state(pst_device, OAL_FALSE);
#endif

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : mac_chip_exit
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_chip_exit(mac_board_stru *pst_board, mac_chip_stru *pst_chip)
{
#if 0
    mac_device_stru   *pst_dev;
    oal_uint32         ul_ret;
    oal_uint8          uc_device;
#endif
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_chip || OAL_PTR_NULL == pst_board))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_chip_init::param null.}");

        return OAL_ERR_CODE_PTR_NULL;
    }

    /*����Device����ṹ�ͷ�*/
#if 0
    for (uc_device = 0; uc_device < pst_chip->uc_device_nums; uc_device++)
    {
         pst_dev = mac_res_get_dev(pst_chip->auc_device_id[uc_device]);
         ul_ret = pst_board->p_device_destroy_fun(pst_dev);
         if (OAL_SUCC != ul_ret)
         {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{mac_chip_exit::p_device_destroy_fun failed[%d].}", ul_ret);

             return ul_ret;
         }
    }
#endif

    pst_chip->uc_device_nums = 0;

    /* destroy�������״̬��ΪFALSE */
    pst_chip->en_chip_state  = OAL_FALSE;

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : mac_board_exit
 ��������  : �ͷ�ȫ�ֱ��������Ľṹ��
 �������  : board����ָ��
 �������  : ��
 �� �� ֵ  : �ɹ���ʧ��
 ���ú���  : mac_chip_destroy��oal_bit_find_first_bit_one_byte��oal_bit_clear_bit_one_byte
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��19��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_board_exit(mac_board_stru *pst_board)
{
#if 0
    oal_uint8  uc_chip_idx;
    oal_uint32 ul_ret;
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_board))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{mac_board_exit::pst_board null.}");

        return OAL_ERR_CODE_PTR_NULL;
    }
#if 0
    while (0 != pst_board->uc_chip_id_bitmap)
    {
        /* ��ȡ���ұ�һλΪ1��λ������ֵ��Ϊchip�������±� */
        uc_chip_idx = oal_bit_find_first_bit_one_byte(pst_board->uc_chip_id_bitmap);
        if (OAL_UNLIKELY(uc_chip_idx >= WLAN_CHIP_MAX_NUM_PER_BOARD))
        {
            MAC_ERR_LOG2(0, "uc_chip_idx is exceeded support spec.", uc_chip_idx, pst_board->uc_chip_id_bitmap);
            OAM_ERROR_LOG2(0, OAM_SF_ANY, "{mac_board_exit::invalid uc_chip_idx[%d] uc_chip_id_bitmap=%d.}",
                           uc_chip_idx, pst_board->uc_chip_id_bitmap);

            return OAL_ERR_CODE_ARRAY_OVERFLOW;
        }

        ul_ret = mac_chip_exit(pst_board, &pst_board->ast_chip[uc_chip_idx]);
        if (OAL_SUCC != ul_ret)
        {
            MAC_WARNING_LOG1(0, "mac_chip_destroy return fail.", ul_ret);
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{mac_board_exit::mac_chip_exit failed[%d].}", ul_ret);

            return ul_ret;
        }

        /* �����Ӧ��bitmapλ */
        oal_bit_clear_bit_one_byte(&pst_board->uc_chip_id_bitmap, uc_chip_idx);
    }
#endif

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : mac_device_max_band
 ��������  : FPGAֻ֧�����40M�Ĵ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
wlan_bw_cap_enum_uint8 mac_device_max_band(oal_void)
{
#if ((_PRE_WLAN_CHIP_ASIC != _PRE_WLAN_CHIP_VERSION))
    return WLAN_BW_CAP_40M;
#else
    return WLAN_BW_CAP_80M;
#endif
}

/*****************************************************************************
 �� �� ��  : mac_device_init
 ��������  : ��ʼ��device�������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_init(mac_device_stru *pst_mac_device, oal_uint32 ul_chip_ver, oal_uint8 uc_chip_id, oal_uint8 uc_device_id)
{
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    oal_uint32                 ul_loop = 0;
#endif

    if (OAL_PTR_NULL == pst_mac_device)
    {
       OAM_ERROR_LOG0(0, OAM_SF_ANY, "{mac_device_init::pst_mac_device null.}");

       return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ʼ��device������ */
    pst_mac_device->uc_chip_id   = uc_chip_id;
    pst_mac_device->uc_device_id = uc_device_id;

    /* ��ʼ��device�����һЩ���� */
    pst_mac_device->en_max_bandwidth = WLAN_BAND_WIDTH_BUTT;
    pst_mac_device->en_max_band      = WLAN_BAND_BUTT;
    pst_mac_device->uc_max_channel   = 0;
    pst_mac_device->ul_beacon_interval = WLAN_BEACON_INTVAL_DEFAULT;

    pst_mac_device->uc_tx_chain  = 0xf;
    pst_mac_device->uc_rx_chain  = 0xf;

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    pst_mac_device->st_cap_flag.bit_proxysta = 0; /* Ĭ�Ϲر�proxy sta ���� */
#endif


#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    pst_mac_device->st_bss_id_list.us_num_networks = 0;
#endif


#ifdef _PRE_WLAN_FEATURE_SMPS
    pst_mac_device->en_smps = OAL_FALSE;
    pst_mac_device->uc_dev_smps_mode = WLAN_MIB_MIMO_POWER_SAVE_MIMO;
    pst_mac_device->uc_no_smps_user_cnt = 0;
#endif

    pst_mac_device->en_device_state = OAL_TRUE;

#ifdef _PRE_WALN_FEATURE_LUT_RESET
    pst_mac_device->en_reset_switch = OAL_TRUE;
#else
    pst_mac_device->en_reset_switch = OAL_FALSE;
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)

    /* ���ݳ�ʼ��ͨ����������֧�ֵĿռ����� */
    if ((WITP_RF_CHANNEL_NUMS == g_l_rf_channel_num) && (WITP_RF_CHANNEL_ZERO == g_l_rf_single_tran))
    {
        pst_mac_device->en_nss_num  = WLAN_DOUBLE_NSS;

        /* ����ͨ��Ϊ˫ͨ����ͨ��0 & ͨ��1 */
        pst_mac_device->uc_tx_chain = WITP_TX_CHAIN_DOUBLE;
    }
    else
    {
        pst_mac_device->en_nss_num = WLAN_SINGLE_NSS;

        if (WITP_RF_CHANNEL_ZERO == g_l_rf_single_tran)
        {
            /* ����ͨ��Ϊ˫ͨ����ͨ��0 */
            pst_mac_device->uc_tx_chain =  WITP_TX_CHAIN_ZERO;
        }
        else if(WITP_RF_CHANNEL_ONE == g_l_rf_single_tran)
        {
            /* ����ͨ��Ϊ˫ͨ����ͨ��1 */
            pst_mac_device->uc_tx_chain =  WITP_TX_CHAIN_ONE;
        }
    }
#else
    pst_mac_device->uc_tx_chain =  WITP_TX_CHAIN_ZERO;
#endif

    /* Ĭ�Ϲر�wmm,wmm��ʱ��������Ϊ0 */
    pst_mac_device->en_wmm = OAL_TRUE;

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    /* ��ʼ��device �ṹ��Proxy STA �����Ϣ */
    for (ul_loop = 0; ul_loop < MAC_VAP_PROXY_STA_HASH_MAX_VALUE; ul_loop++)
    {
        oal_dlist_init_head(&(pst_mac_device->st_device_proxysta.ast_proxysta_hash[ul_loop]));
    }

    /* ����Device������Repeaterģʽ */
    pst_mac_device->en_dev_work_mode = MAC_REPEATER_MODE;
#endif

    /* ����оƬ�汾��ʼ��device������Ϣ */
   switch(ul_chip_ver)
   {
    case WLAN_CHIP_VERSION_HI1151V100H:
        pst_mac_device->en_protocol_cap  = WLAN_PROTOCOL_CAP_VHT;
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
        pst_mac_device->en_bandwidth_cap = (band_5g_enabled) ? mac_device_max_band() : WLAN_BW_CAP_40M;
    #ifdef _PRE_WLAN_FEATURE_5G
        pst_mac_device->en_band_cap      = (band_5g_enabled) ? WLAN_BAND_CAP_2G_5G : WLAN_BAND_CAP_2G;
    #else
        pst_mac_device->en_band_cap      = WLAN_BAND_CAP_2G;
    #endif /* _PRE_WLAN_FEATURE_5G */
#else
        pst_mac_device->en_bandwidth_cap = mac_device_max_band();
    #ifdef _PRE_WLAN_FEATURE_5G
        pst_mac_device->en_band_cap      = WLAN_BAND_CAP_2G_5G;
    #else
        pst_mac_device->en_band_cap      = WLAN_BAND_CAP_2G;
    #endif /* _PRE_WLAN_FEATURE_5G */
#endif
        break;

    case WLAN_CHIP_VERSION_HI1151V100L:
        pst_mac_device->en_protocol_cap  = WLAN_PROTOCOL_CAP_VHT;
        pst_mac_device->en_bandwidth_cap = WLAN_BW_CAP_40M;
        pst_mac_device->en_band_cap      = WLAN_BAND_CAP_2G;

        break;

    default:
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{mac_device_init::ul_chip_ver is not supportted[0x%x].}", ul_chip_ver);
        return OAL_ERR_CODE_CONFIG_UNSUPPORT;


    }
    pst_mac_device->bit_ldpc_coding  = OAL_FALSE;   //1131debug  1131c��֧��
#ifdef _PRE_WLAN_FEATURE_TXBF
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    pst_mac_device->bit_tx_stbc      = (WITP_TX_CHAIN_DOUBLE == pst_mac_device->uc_tx_chain) ? OAL_TRUE : OAL_FALSE;
    pst_mac_device->bit_su_bfmer     = (WITP_TX_CHAIN_DOUBLE == pst_mac_device->uc_tx_chain) ? OAL_TRUE : OAL_FALSE;
#else
    pst_mac_device->bit_tx_stbc      =  OAL_FALSE;
    pst_mac_device->bit_su_bfmer     =  OAL_FALSE;
#endif
    pst_mac_device->bit_su_bfmee     = OAL_TRUE;
    pst_mac_device->bit_rx_stbc      = 1;                       /* ֧��2���ռ��� */

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
    pst_mac_device->bit_mu_bfmee     = OAL_TRUE;
#else
    pst_mac_device->bit_mu_bfmee     = OAL_FALSE;
#endif

#else
    pst_mac_device->bit_tx_stbc      = OAL_FALSE;
    pst_mac_device->bit_su_bfmer     = OAL_FALSE;
    pst_mac_device->bit_su_bfmee     = OAL_FALSE;
    pst_mac_device->bit_mu_bfmee     = OAL_FALSE;
    pst_mac_device->bit_rx_stbc      = 1;
#endif

    /* ��ʼ��vap numͳ����Ϣ */
    pst_mac_device->uc_vap_num = 0;
    pst_mac_device->uc_sta_num = 0;
#ifdef _PRE_WLAN_FEATURE_P2P
    pst_mac_device->st_p2p_info.uc_p2p_device_num   = 0;
    pst_mac_device->st_p2p_info.uc_p2p_goclient_num = 0;
    pst_mac_device->st_p2p_info.pst_primary_net_device = OAL_PTR_NULL;/* ��ʼ����net_device Ϊ��ָ�� */
#endif

    /* ��ʼ��Ĭ�Ϲ����� */
    mac_init_regdomain();

    /* ��ʼ���ŵ��б� */
    mac_init_channel_list();

    /* ��ʼ����λ״̬*/
    MAC_DEV_RESET_IN_PROGRESS(pst_mac_device, OAL_FALSE);
    pst_mac_device->us_device_reset_num = 0;

    /* Ĭ�Ϲر�DBAC���� */
#ifdef _PRE_WLAN_FEATURE_DBAC
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    pst_mac_device->en_dbac_enabled = OAL_FALSE;
#else
    pst_mac_device->en_dbac_enabled = OAL_TRUE;
#endif
#endif


#ifdef _PRE_SUPPORT_ACS
    oal_memset(&pst_mac_device->st_acs_switch, 0, OAL_SIZEOF(pst_mac_device->st_acs_switch));
#endif

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    mac_set_2040bss_switch(pst_mac_device, OAL_TRUE);
#else
    mac_set_2040bss_switch(pst_mac_device, OAL_FALSE);
#endif
#endif
    pst_mac_device->uc_in_suspend       = OAL_FALSE;
    pst_mac_device->uc_arpoffload_switch   = OAL_FALSE;
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
    pst_mac_device->uc_rekeyoffload_switch   = OAL_FALSE;
#endif
    pst_mac_device->uc_wapi = OAL_FALSE;

    /* AGC��ͨ��Ĭ��Ϊ����Ӧ   */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    pst_mac_device->uc_lock_channel = 0x02;
#endif
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV)
    pst_mac_device->uc_scan_count    = 0;
#endif

    /*1131debug: mac_device_init    �ӿ�Ԥ�� */
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV)
    if (NULL != g_fn_mac_device_init_adjust_patch_ram)
    {
        if (OAL_TRUE == g_fn_mac_device_init_adjust_patch_ram(pst_mac_device, ul_chip_ver, uc_chip_id, uc_device_id))
        {
            return OAL_SUCC;
        }
    }
#endif
    return OAL_SUCC;
}

oal_uint32  mac_chip_init(mac_chip_stru *pst_chip, oal_uint8 uc_chip_id)
{
    return OAL_SUCC;
}

oal_uint32  mac_board_init(mac_board_stru *pst_board)
{
    return OAL_SUCC;
}


/*****************************************************************************
 �� �� ��  : mac_device_find_up_vap
 ��������  : Ѱ�Ҵ���UP״̬��VAP
 �������  : pst_mac_device: device
 �������  : ppst_mac_vap  : vap
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_find_up_vap(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap)
{
    oal_uint8       uc_vap_idx;
    mac_vap_stru   *pst_mac_vap;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_mac_vap = mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
        {
            OAM_WARNING_LOG1(0, OAM_SF_SCAN, "vap is null! vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);

            *ppst_mac_vap = OAL_PTR_NULL;

            return OAL_ERR_CODE_PTR_NULL;
        }

        if (MAC_VAP_STATE_UP == pst_mac_vap->en_vap_state || MAC_VAP_STATE_PAUSE == pst_mac_vap->en_vap_state ||
            (MAC_VAP_STATE_STA_LISTEN == pst_mac_vap->en_vap_state && pst_mac_vap->us_user_nums > 0))
        {
            *ppst_mac_vap = pst_mac_vap;

            return OAL_SUCC;
        }
    }

    *ppst_mac_vap = OAL_PTR_NULL;

    return OAL_FAIL;
}

/*****************************************************************************
 �� �� ��  : mac_device_find_2up_vap
 ��������  : Ѱ�Ҵ���UP״̬������VAP
 �������  : pst_mac_device: device
 �������  : ppst_mac_vap  : vap
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_find_2up_vap(
                mac_device_stru *pst_mac_device,
                mac_vap_stru   **ppst_mac_vap1,
                mac_vap_stru   **ppst_mac_vap2)
{
    mac_vap_stru                  *pst_vap;
    oal_uint8                      uc_vap_idx;
    oal_uint8                      ul_up_vap_num = 0;
    mac_vap_stru                  *past_vap[2] = {0};

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_PTR_NULL == pst_vap)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "vap is null, vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);
            continue;
        }

        if (MAC_VAP_STATE_UP == pst_vap->en_vap_state || MAC_VAP_STATE_PAUSE == pst_vap->en_vap_state ||
            (MAC_VAP_STATE_STA_LISTEN == pst_vap->en_vap_state && pst_vap->us_user_nums > 0))
        {
            past_vap[ul_up_vap_num] = pst_vap;
            ul_up_vap_num++;

            if (ul_up_vap_num >=2)
            {
                break;
            }
        }
    }

    if (ul_up_vap_num < 2)
    {
        return OAL_FAIL;
    }

    *ppst_mac_vap1 = past_vap[0];
    *ppst_mac_vap2 = past_vap[1];

    return OAL_SUCC;
}

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
/*****************************************************************************
 �� �� ��  : mac_device_find_up_vap
 ��������  : Ѱ�Ҵ���UP״̬��AP VAP
 �������  : pst_mac_device: device
 �������  : ppst_mac_vap  : vap
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��4��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_find_up_ap(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap)
{
    oal_uint8       uc_vap_idx;
    mac_vap_stru   *pst_mac_vap;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_mac_vap = mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
        {
            OAM_WARNING_LOG1(0, OAM_SF_SCAN, "vap is null! vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);
            return OAL_ERR_CODE_PTR_NULL;
        }

        if ((MAC_VAP_STATE_UP == pst_mac_vap->en_vap_state || MAC_VAP_STATE_PAUSE == pst_mac_vap->en_vap_state)&&
            (WLAN_VAP_MODE_BSS_AP == pst_mac_vap->en_vap_mode))
        {
            *ppst_mac_vap = pst_mac_vap;

            return OAL_SUCC;
        }
    }

    *ppst_mac_vap = OAL_PTR_NULL;

    return OAL_FAIL;
}
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)

/*****************************************************************************
 �� �� ��  : mac_device_find_up_sta
 ��������  : Ѱ�Ҵ���UP״̬��STA
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��6��23��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_find_up_sta(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap)
{
    oal_uint8       uc_vap_idx;
    mac_vap_stru   *pst_mac_vap;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_mac_vap = mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
        {
            OAM_WARNING_LOG1(0, OAM_SF_SCAN, "vap is null! vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);

            *ppst_mac_vap = OAL_PTR_NULL;

            return OAL_ERR_CODE_PTR_NULL;
        }

        if ((MAC_VAP_STATE_UP == pst_mac_vap->en_vap_state || MAC_VAP_STATE_PAUSE == pst_mac_vap->en_vap_state)&&
            (WLAN_VAP_MODE_BSS_STA == pst_mac_vap->en_vap_mode))
        {
            *ppst_mac_vap = pst_mac_vap;

            return OAL_SUCC;
        }
    }

    *ppst_mac_vap = OAL_PTR_NULL;

    return OAL_FAIL;
}

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
/*****************************************************************************
 �� �� ��  : mac_device_find_up_p2p_go
 ��������  : Ѱ�Ҵ���UP״̬�� P2P_GO
 �������  : pst_mac_device: device
 �������  : ppst_mac_vap  : vap
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_find_up_p2p_go(mac_device_stru *pst_mac_device, mac_vap_stru **ppst_mac_vap)
{
    oal_uint8       uc_vap_idx;
    mac_vap_stru   *pst_mac_vap;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_mac_vap = mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
        {
            OAM_WARNING_LOG1(0, OAM_SF_SCAN, "vap is null! vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);
            continue;
        }

        if ((MAC_VAP_STATE_UP == pst_mac_vap->en_vap_state || MAC_VAP_STATE_PAUSE == pst_mac_vap->en_vap_state)&&
            (WLAN_P2P_GO_MODE == pst_mac_vap->en_p2p_mode))
        {
            *ppst_mac_vap = pst_mac_vap;

            return OAL_SUCC;
        }
    }

    *ppst_mac_vap = OAL_PTR_NULL;

    return OAL_FAIL;
}
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)

/*****************************************************************************
 �� �� ��  : mac_device_calc_up_vap_num
 ��������  : ����up��vap����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_calc_up_vap_num(mac_device_stru *pst_mac_device)
{
    mac_vap_stru                  *pst_vap;
    oal_uint8                      uc_vap_idx;
    oal_uint8                      ul_up_ap_num = 0;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_PTR_NULL == pst_vap)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "vap is null, vap id is %d",
                           pst_mac_device->auc_vap_id[uc_vap_idx]);
            continue;
        }

        if (MAC_VAP_STATE_UP == pst_vap->en_vap_state || MAC_VAP_STATE_PAUSE == pst_vap->en_vap_state)
        {
            ul_up_ap_num++;
        }
    }

    return ul_up_ap_num;
}

#if 0
/*****************************************************************************
�� �� �� : mac_device_calc_work_vap_num
�������� : ����work��vap����
������� : ��
������� : ��
�� �� ֵ :
���ú��� :
�������� :

�޸���ʷ :
1.�� �� : 2015��12��2��
�� �� : 
�޸����� : �����ɺ���

*****************************************************************************/
oal_uint32 mac_device_calc_work_vap_num(mac_device_stru *pst_mac_device)
{
    mac_vap_stru *pst_vap;
    oal_uint8 uc_vap_idx;
    oal_uint8 ul_work_vap_num = 0;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_PTR_NULL == pst_vap)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "mac_device_calc_work_vap_numv::vap[%d] is null",
            pst_mac_device->auc_vap_id[uc_vap_idx]);
            continue;
        }

        if ((MAC_VAP_STATE_INIT != pst_vap->en_vap_state) && (MAC_VAP_STATE_BUTT != pst_vap->en_vap_state))
        {
            ul_work_vap_num++;
        }
    }

    return ul_work_vap_num;
}
#endif //#if 0



#if 0
#ifdef _PRE_WLAN_FEATURE_DBAC
/*****************************************************************************
 �� �� ��  : mac_dbac_update_chl_config
 ��������  : DBAC�����ṩ�ӿڣ���DBACģʽ���л������ŵ���
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��7��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_dbac_update_chl_config(mac_device_stru *pst_mac_device, mac_channel_stru *pst_chl)
{
    alg_dbac_mgr_stru   *pst_dbac_mgr;
    oal_uint8            uc_vap_idx;

    if (OAL_PTR_NULL == pst_mac_device || OAL_PTR_NULL == pst_chl)
    {
        OAM_ERROR_LOG2(0, OAM_SF_DBAC, "pst_mac_device or pst_chl is null ptr, %u, %u", pst_mac_device, pst_chl);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_dbac_mgr = alg_dbac_get_mgr_stru(pst_mac_device);
    if (OAL_PTR_NULL == pst_dbac_mgr)
    {
        OAM_WARNING_LOG0(0, OAM_SF_DBAC, "pst_dbac_mgr == NULL");
        return OAL_FAIL;
    }

    /* ����DBAC �ŵ���Ϣ */
    for (uc_vap_idx = 0; uc_vap_idx < ALG_DBAC_CFG_VAP_CNT; uc_vap_idx++)
    {
        if (pst_dbac_mgr->ast_fcs_cfg[uc_vap_idx].st_src_chl.en_band == pst_chl->en_band)
        {
            pst_dbac_mgr->ast_fcs_cfg[uc_vap_idx].st_src_chl = *pst_chl;
            OAM_INFO_LOG4(0, OAM_SF_DBAC, "update dbac channel config: cfg idx is %d, src chl update to band %d, ch num %d, bw %d",
                            uc_vap_idx,
                            pst_chl->en_band,
                            pst_chl->uc_chan_number,
                            pst_chl->en_bandwidth);
        }
        else
        {
            pst_dbac_mgr->ast_fcs_cfg[uc_vap_idx].st_dst_chl = *pst_chl;
            OAM_INFO_LOG4(0, OAM_SF_DBAC, "update dbac channel config: cfg idx is %d, dst chl update to band %d, ch num %d, bw %d",
                            uc_vap_idx,
                            pst_chl->en_band,
                            pst_chl->uc_chan_number,
                            pst_chl->en_bandwidth);
        }
    }

    return OAL_SUCC;
}
#endif
#endif


/*****************************************************************************
 �� �� ��  : mac_device_is_p2p_connected
 ��������  : �ж�p2p�豸�Ƿ����
 �������  : pst_mac_device: device
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��4��1��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  mac_device_is_p2p_connected(mac_device_stru *pst_mac_device)
{
    oal_uint8       uc_vap_idx;
    mac_vap_stru   *pst_mac_vap;

    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_mac_vap = mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
        {
            OAM_WARNING_LOG1(0, OAM_SF_P2P, "vap is null! vap id is %d", pst_mac_device->auc_vap_id[uc_vap_idx]);
            return OAL_ERR_CODE_PTR_NULL;
        }
        if((IS_P2P_GO(pst_mac_vap)||IS_P2P_CL(pst_mac_vap))&&
           (pst_mac_vap->us_user_nums > 0))
        {
            return OAL_SUCC;
        }
    }
    return OAL_FAIL;
}


/*****************************************************************************
 �� �� ��  : mac_device_set_vap_id
 ��������  : ����device��������ɾ��һ��vap������ά��vap����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��7��
    ��    ��   : ��� 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void mac_device_set_vap_id(mac_device_stru *pst_mac_device, mac_vap_stru *pst_mac_vap,  oal_uint8 uc_vap_idx, wlan_vap_mode_enum_uint8 en_vap_mode, wlan_p2p_mode_enum_uint8 en_p2p_mode, oal_uint8 is_add_vap)
{
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_uint8                       uc_vap_tmp_idx = 0;
    mac_vap_stru                   *pst_tmp_vap;
#endif

    if (is_add_vap)
    {
        /* ?offload???,????HMAC????? */
        pst_mac_device->auc_vap_id[pst_mac_device->uc_vap_num++] = uc_vap_idx;

        /* device?sta???1 */
        if (WLAN_VAP_MODE_BSS_STA == en_vap_mode)
        {
            pst_mac_device->uc_sta_num++;

    #ifdef _PRE_WLAN_FEATURE_PROXYSTA
            if (1 == pst_mac_device->st_cap_flag.bit_proxysta)
            {
                if ((OAL_TRUE == pst_mac_vap->st_vap_proxysta.en_is_proxysta) &&
                    (OAL_FALSE == pst_mac_vap->st_vap_proxysta.en_is_main_proxysta))
                {
                    pst_mac_device->uc_proxysta_num++;
                }
            }
    #endif
            /* ???uc_assoc_vap_id??????ap??? */
            pst_mac_vap->uc_assoc_vap_id = 0xff;
        }

    #ifdef _PRE_WLAN_FEATURE_P2P
        pst_mac_vap->en_p2p_mode = en_p2p_mode;
        mac_inc_p2p_num(pst_mac_vap);
        /*  ??GO?????????0,???WLAN??????GO?????? */
        if (IS_P2P_GO(pst_mac_vap))
        {
            for (uc_vap_tmp_idx = 0; uc_vap_tmp_idx < pst_mac_device->uc_vap_num; uc_vap_tmp_idx++)
            {
                pst_tmp_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_tmp_idx]);
                if (OAL_PTR_NULL == pst_tmp_vap)
                {
                    OAM_ERROR_LOG1(0, OAM_SF_SCAN, "{dmac_config_add_vap::pst_mac_vap null,vap_idx=%d.}",
                                   pst_mac_device->auc_vap_id[uc_vap_tmp_idx]);
                    continue;
                }

                if ((MAC_VAP_STATE_UP == pst_tmp_vap->en_vap_state) && (pst_tmp_vap != pst_mac_vap))
                {
                    pst_mac_vap->st_channel.en_band        = pst_tmp_vap->st_channel.en_band;
                    pst_mac_vap->st_channel.en_bandwidth   = pst_tmp_vap->st_channel.en_bandwidth;
                    pst_mac_vap->st_channel.uc_chan_number = pst_tmp_vap->st_channel.uc_chan_number;
                    pst_mac_vap->st_channel.uc_idx         = pst_tmp_vap->st_channel.uc_idx;
                    break;
                }
            }
        }
    #endif
    }
    else
    {
        /* ?offload???,????HMAC????? */
        pst_mac_device->auc_vap_id[pst_mac_device->uc_vap_num--] = 0;

        /* device?sta???1 */
        if (WLAN_VAP_MODE_BSS_STA == en_vap_mode)
        {
            pst_mac_device->uc_sta_num--;

    #ifdef _PRE_WLAN_FEATURE_PROXYSTA
            if (1 == pst_mac_device->st_cap_flag.bit_proxysta)
            {
                if ((OAL_TRUE == pst_mac_vap->st_vap_proxysta.en_is_proxysta) &&
                    (OAL_FALSE == pst_mac_vap->st_vap_proxysta.en_is_main_proxysta))
                {
                    pst_mac_device->uc_proxysta_num--;
                }
            }
    #endif
            /* ???uc_assoc_vap_id??????ap??? */
            pst_mac_vap->uc_assoc_vap_id = 0xff;
        }

    #ifdef _PRE_WLAN_FEATURE_P2P
        pst_mac_vap->en_p2p_mode = en_p2p_mode;
        mac_dec_p2p_num(pst_mac_vap);
    #endif
    }
}

oal_void mac_device_set_dfr_reset(mac_device_stru *pst_mac_device, oal_uint8 uc_device_reset_in_progress)
{
    pst_mac_device->uc_device_reset_in_progress = uc_device_reset_in_progress;
}

oal_void  mac_device_set_state(mac_device_stru *pst_mac_device, oal_uint8 en_device_state)
{
    pst_mac_device->en_device_state = en_device_state;
}

oal_void  mac_device_set_channel(mac_device_stru *pst_mac_device, mac_cfg_channel_param_stru * pst_channel_param)
{
    pst_mac_device->uc_max_channel = pst_channel_param->uc_channel;
    pst_mac_device->en_max_band = pst_channel_param->en_band;
    pst_mac_device->en_max_bandwidth = pst_channel_param->en_bandwidth;
}

oal_void  mac_device_get_channel(mac_device_stru *pst_mac_device, mac_cfg_channel_param_stru * pst_channel_param)
{
    pst_channel_param->uc_channel = pst_mac_device->uc_max_channel;
    pst_channel_param->en_band = pst_mac_device->en_max_band;
    pst_channel_param->en_bandwidth = pst_mac_device->en_max_bandwidth;
}


oal_void  mac_device_set_txchain(mac_device_stru *pst_mac_device, oal_uint8 uc_tx_chain)
{
    pst_mac_device->uc_tx_chain = uc_tx_chain;
}

oal_void  mac_device_set_rxchain(mac_device_stru *pst_mac_device, oal_uint8 uc_rx_chain)
{
    pst_mac_device->uc_rx_chain = uc_rx_chain;
}

oal_void  mac_device_set_beacon_interval(mac_device_stru *pst_mac_device, oal_uint32 ul_beacon_interval)
{
    pst_mac_device->ul_beacon_interval = ul_beacon_interval;
}

oal_void  mac_device_inc_active_user(mac_device_stru *pst_mac_device)
{
    /* ?????+1 */
    pst_mac_device->uc_active_user_cnt++;
}

oal_void  mac_device_dec_active_user(mac_device_stru *pst_mac_device)
{
    if (pst_mac_device->uc_active_user_cnt)
    {
        pst_mac_device->uc_active_user_cnt--;
    }
}

#if 0
oal_void  mac_device_set_dfs(mac_device_stru *pst_mac_device, oal_bool_enum_uint8 en_dfs_switch, oal_uint8 uc_debug_level)
{
    /*������dfs������ ��Ч*/
#if 0
    pst_mac_device->en_dfs_switch = en_dfs_switch;
    pst_mac_device->uc_debug_level = uc_debug_level;
#endif
}
#endif
/*****************************************************************************
 �� �� ��  : mac_device_get_all_rates
 ��������  : ��ȡ��������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void* mac_device_get_all_rates(mac_device_stru *pst_dev)
{
    return (oal_void *)pst_dev->st_mac_rates_11g;
}


/*lint -e19*/
oal_module_symbol(mac_device_set_vap_id);
oal_module_symbol(mac_device_set_dfr_reset);
oal_module_symbol(mac_device_set_state);
oal_module_symbol(mac_device_get_channel);
oal_module_symbol(mac_device_set_channel);
oal_module_symbol(mac_device_set_txchain);
oal_module_symbol(mac_device_set_rxchain);
oal_module_symbol(mac_device_set_beacon_interval);
oal_module_symbol(mac_device_inc_active_user);
oal_module_symbol(mac_device_dec_active_user);
#if 0
oal_module_symbol(mac_device_inc_assoc_user);
oal_module_symbol(mac_device_dec_assoc_user);
oal_module_symbol(mac_device_set_dfs);
#endif

oal_module_symbol(mac_device_init);
oal_module_symbol(mac_chip_init);
oal_module_symbol(mac_board_init);

oal_module_symbol(mac_device_exit);
oal_module_symbol(mac_chip_exit);
oal_module_symbol(mac_board_exit);

oal_module_symbol(mac_device_find_up_vap);
oal_module_symbol(mac_device_find_up_ap);
oal_module_symbol(mac_device_calc_up_vap_num);
#if 0
oal_module_symbol(mac_device_calc_work_vap_num);
#endif
oal_module_symbol(mac_device_is_p2p_connected);
oal_module_symbol(mac_device_find_2up_vap);
oal_module_symbol(mac_device_find_up_p2p_go);
/*lint +e19*/
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV)
#pragma arm section rwdata = "patch", code = "patch", zidata = "patch", rodata = "patch"
oal_bool_enum_uint8 mac_device_init_adjust(mac_device_stru *pst_mac_device, oal_uint32 ul_chip_ver, oal_uint8 uc_chip_id, oal_uint8 uc_device_id)
{
    return OAL_FALSE;
}
#pragma arm section rodata, code, rwdata, zidata
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

