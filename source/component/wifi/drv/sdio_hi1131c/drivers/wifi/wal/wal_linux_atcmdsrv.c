/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : wal_linux_atcmdsrv.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2015��10��10��
  ����޸�   :
  ��������   : װ��������������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��10��10��
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
#include "oal_ext_if.h"
#include "oal_profiling.h"
#include "oal_kernel_file.h"

#include "oam_ext_if.h"
#include "frw_ext_if.h"

#include "wlan_spec.h"
#include "wlan_types.h"

#include "mac_vap.h"
#include "mac_resource.h"
#include "mac_ie.h"

#include "hmac_ext_if.h"
#include "hmac_chan_mgmt.h"

#include "wal_main.h"
#include "wal_config.h"
#include "wal_regdb.h"
#include "wal_linux_scan.h"
#include "wal_linux_atcmdsrv.h"
#include "wal_linux_bridge.h"
#include "wal_linux_flowctl.h"
#include "wal_linux_event.h"
#include "plat_cali.h"
#include "oal_hcc_host_if.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#endif



#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_ATCMDSRV_C

/*****************************************************************************
  2 �ṹ�嶨��
*****************************************************************************/
#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST))
typedef enum
{
    CHECK_LTE_GPIO_INIT            = 0,    /* ��ʼ�� */
    CHECK_LTE_GPIO_LOW             = 1,    /* ����Ϊ�͵�ƽ */
    CHECK_LTE_GPIO_HIGH            = 2,    /*����Ϊ�ߵ�ƽ */
    CHECK_LTE_GPIO_RESUME          = 3,    /*�ָ��Ĵ������� */
    CHECK_LTE_GPIO_BUTT
}check_lte_gpio_step;

typedef struct
{
    oal_uint8                     uc_mode;          /* ģʽ*/
    oal_uint8                     uc_band;          /* Ƶ�� */
}wal_atcmdsrv_mode_stru;

typedef struct
{
    oal_uint32                   ul_datarate;          /* at�������õ�����ֵ */
    oal_int8                    *puc_datarate;          /* �����ַ���*/
}wal_atcmdsrv_datarate_stru;

OAL_CONST wal_atcmdsrv_mode_stru g_ast_atcmdsrv_mode_table[] =
{
    {WLAN_LEGACY_11B_MODE, WLAN_BAND_2G},    /* 11b, 2.4G */
    {WLAN_LEGACY_11G_MODE, WLAN_BAND_2G},    /* �ɵ�11g only�ѷ���, 2.4G, OFDM */
    {WLAN_MIXED_ONE_11G_MODE, WLAN_BAND_2G},    /* 11bg, 2.4G */
    {WLAN_MIXED_TWO_11G_MODE, WLAN_BAND_2G},    /* 11g only, 2.4G */
    {WLAN_VHT_MODE, WLAN_BAND_2G},    /* 11ac */
    {WLAN_HT_11G_MODE, WLAN_BAND_2G},    /* 11ng,������11b*/
    {WLAN_HT_ONLY_MODE_2G, WLAN_BAND_2G},/* 11nonlg 2Gmode*/
    {WLAN_PROTOCOL_BUTT,WLAN_BAND_2G},

#ifdef _PRE_WLAN_FEATURE_5G
    {WLAN_LEGACY_11A_MODE, WLAN_BAND_5G},    /* 11a, 5G, OFDM */
    {WLAN_HT_MODE, WLAN_BAND_5G},    /* 11n(11bgn����11an������Ƶ���ж�) */
    {WLAN_HT_ONLY_MODE, WLAN_BAND_5G},    /* 11n only 5Gmode,ֻ�д�HT���豸�ſ��Խ��� */
    {WLAN_VHT_ONLY_MODE, WLAN_BAND_5G},    /* 11ac only mode ֻ�д�VHT���豸�ſ��Խ��� */
#endif /* _PRE_WLAN_FEATURE_5G */
};

OAL_STATIC OAL_CONST wal_atcmdsrv_datarate_stru   past_atcmdsrv_non_ht_rate_table[] =
{
    {1," 1 "},
    {2," 2 "},
    {5," 5.5 "},
    {6," 6 "},
    {7," 7 "},
    {8," 8 "},
    {9," 9 "},
    {11," 11 "},
    {12," 12 "},
    {18," 18 "},
    {24," 24 "},
    {36," 36 "},
    {48," 48 "},
    {54," 54 "},
};
oal_uint64                      ul_chipcheck_total_time;

#endif


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
#if ((_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST))
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_freq
 ��������  : ����Ƶ��/�ŵ�
 �������  : Ƶ��1~24 36~165
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

************************** ***************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_freq(oal_net_device_stru *pst_net_dev, oal_int32 l_freq)
{
    wal_msg_write_stru          st_write_msg;

    oal_int32                   l_ret;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_set_freq:l_freq[%d]", l_freq);
    /***************************************************************************
        ���¼���wal�㴦��
    ***************************************************************************/
    /* ��д��Ϣ */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_CURRENT_CHANEL, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_freq;

    /* ������Ϣ */
    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_freq::return err code %d!}", l_ret);
        return l_ret;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_txpower
 ��������  : ����Ƶ��/�ŵ�
 �������  : Ƶ��1~24 36~165
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_txpower(oal_net_device_stru *pst_net_dev, oal_int32 l_txpower)
{
    wal_msg_write_stru          st_write_msg;

    oal_int32                   l_ret;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_set_txpower:l_txpower[%d]", l_txpower);

    /***************************************************************************
        ���¼���wal�㴦��
    ***************************************************************************/
    /* ��д��Ϣ */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_TX_POWER, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_txpower;

    /* ������Ϣ */
    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_txpower::return err code %d!}", l_ret);
        return l_ret;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_mode
 ��������  : ����ģʽ
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_mode(oal_net_device_stru *pst_net_dev, oal_int32 l_mode)
{
    wal_msg_write_stru          st_write_msg;
    mac_cfg_mode_param_stru    *pst_mode_param;
    oal_uint8                   uc_prot_idx;

    oal_int32                   l_ret = 0;

    /*��ȡģʽ��Ӧ��band*/
    #if 0

    for (uc_prot_idx = 0; uc_prot_idx < WAL_ATCMDSRV_IOCTL_MODE_NUM; uc_prot_idx++)
    #else

    for (uc_prot_idx = 0; uc_prot_idx < sizeof(g_ast_atcmdsrv_mode_table)/sizeof(wal_atcmdsrv_mode_stru); uc_prot_idx++)
    #endif
    {
        if (g_ast_atcmdsrv_mode_table[uc_prot_idx].uc_mode == (oal_uint8)l_mode)
        {
            break;
        }
    }

    /***************************************************************************
        ���¼���wal�㴦��
    ***************************************************************************/
    /* ��д��Ϣ */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_MODE, OAL_SIZEOF(mac_cfg_mode_param_stru));

    /*����ģʽ��������ģʽ��ʱ�򽫴���Ĭ�ϳ�20M*/
    pst_mode_param = (mac_cfg_mode_param_stru *)(st_write_msg.auc_value);
    if(WLAN_HT_ONLY_MODE_2G == l_mode)
    {
        pst_mode_param->en_protocol  = WLAN_HT_ONLY_MODE;
    }
    else
    {
        pst_mode_param->en_protocol  = (oal_uint8)l_mode;
    }
    if (uc_prot_idx >= sizeof(g_ast_atcmdsrv_mode_table)/sizeof(wal_atcmdsrv_mode_stru))
    {
        OAM_ERROR_LOG1(0,OAM_SF_ANY,"{wal_atcmsrv_ioctl_set_mode:err code[%u]}",uc_prot_idx);
        return l_ret;
    }
    pst_mode_param->en_band      = (wlan_channel_band_enum_uint8)g_ast_atcmdsrv_mode_table[uc_prot_idx].uc_band;
    pst_mode_param->en_bandwidth = WLAN_BAND_WIDTH_20M;
    /*δ��ʹ�ã�������ɾ��*/
    OAM_WARNING_LOG3(0, OAM_SF_CFG, "{wal_atcmsrv_ioctl_set_mode::protocol[%d],band[%d],bandwidth[%d]!}\r\n",
                            pst_mode_param->en_protocol, pst_mode_param->en_band, pst_mode_param->en_bandwidth);

    /* ������Ϣ */
    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_mode_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_mode::return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_datarate
 ��������  : ���÷�������
 �������  : ����ֵ
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_datarate(oal_net_device_stru *pst_net_dev, oal_int32 l_datarate)
{
    oal_uint8                   uc_prot_idx;
    oal_uint32                  ul_ret;
    mac_vap_stru                   *pst_mac_vap;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_datarate::OAL_NET_DEV_PRIV, return null!}");
        return -OAL_EINVAL;
    }


    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_set_datarate:l_datarate[%d]", l_datarate);

    /*��ȡ���ʶ�Ӧ���ַ�����������������ʵ���Ӧ�ӿ�*/
    for (uc_prot_idx = 0; uc_prot_idx < WAL_ATCMDSRV_IOCTL_DATARATE_NUM; uc_prot_idx++)
    {
        if (past_atcmdsrv_non_ht_rate_table[uc_prot_idx].ul_datarate == (oal_uint32)l_datarate)
        {
            break;
        }
    }
    if (uc_prot_idx >= WAL_ATCMDSRV_IOCTL_DATARATE_NUM)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY,"uc_prot_idx Overrunning!");
        return -OAL_EINVAL;
    }
    if(WLAN_HT_ONLY_MODE == pst_mac_vap->en_protocol)/*����������Ϊ7ʱ��ʾMCS7*/
    {
        ul_ret = wal_hipriv_set_mcs(pst_net_dev,(oal_int8 *)past_atcmdsrv_non_ht_rate_table[uc_prot_idx].puc_datarate);
    }
    else if(WLAN_VHT_MODE == pst_mac_vap->en_protocol)
    {
        ul_ret = wal_hipriv_set_mcsac(pst_net_dev,(oal_int8 *)past_atcmdsrv_non_ht_rate_table[uc_prot_idx].puc_datarate);
    }
    else
    {
        ul_ret = wal_hipriv_set_rate(pst_net_dev,(oal_int8 *)past_atcmdsrv_non_ht_rate_table[uc_prot_idx].puc_datarate);
    }
    if (OAL_SUCC != ul_ret)
    {
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_bandwidth
 ��������  : ���ô���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_bandwidth(oal_net_device_stru *pst_net_dev, oal_int32 l_bandwidth)
{
    wal_msg_write_stru          st_write_msg;
    mac_cfg_mode_param_stru    *pst_mode_param;

    oal_int32                   l_ret;


    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_set_bandwidth:l_bandwidth[%d]", l_bandwidth);

    /***************************************************************************
        ���¼���wal�㴦��
    ***************************************************************************/
    /* ��д��Ϣ */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_BANDWIDTH, OAL_SIZEOF(oal_int32));

    /*���ô���ʱ��ģʽ�����޸ģ����ǰ���֮ǰ��ֵ����*/
    pst_mode_param = (mac_cfg_mode_param_stru *)(st_write_msg.auc_value);

    pst_mode_param->en_bandwidth = (oal_uint8)l_bandwidth;

    /* ������Ϣ */
    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_mode_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_mode::return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    return OAL_SUCC;

}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_always_tx
 ��������  : ���ó���ģʽ
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_always_tx(oal_net_device_stru *pst_net_dev,oal_int32 l_always_tx)
{
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_bcast_param;
    oal_int8                          pc_param;
    oal_uint8                         auc_param[] = {"all"};
    oal_uint16                        us_len;


    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_set_always_tx:l_always_tx[%d]", l_always_tx);

    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_TX_1102, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ��������������������� */
    pst_set_bcast_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* װ�����Ե������ֱ�ӽ������������ú� */
    pst_set_bcast_param->en_payload_flag = RF_PAYLOAD_ALL_ONE;
    pst_set_bcast_param->ul_payload_len = WAL_ATCMDSRB_IOCTL_AL_TX_LEN;
    pst_set_bcast_param->uc_param = (oal_uint8)l_always_tx;

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_always_tx::return err code [%d]!}", l_ret);
        return l_ret;
    }

    /*��ӡδ����Ϣ*/
    l_ret = (oal_int32)wal_hipriv_vap_info(pst_net_dev,&pc_param);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_always_tx::return err code [%d]!}", l_ret);
    }
    /*��ӡ���мĴ���*/
    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    oal_memcopy(st_write_msg.auc_value, auc_param, OAL_STRLEN((oal_int8*)auc_param));

    us_len = (oal_uint16)(OAL_STRLEN((oal_int8*)auc_param));

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REG_INFO, us_len);

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_always_tx::return err code [%d]!}\r\n", l_ret);
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_atcmdsrv_ioctl_convert_dbb_num
 ��������  : ת��dbb�汾��
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/

OAL_STATIC oal_void wal_atcmdsrv_ioctl_convert_dbb_num(oal_uint32 ul_dbb_num,oal_uint8 *pc_dbb_num)
{
    oal_uint8  uc_temp          = 0;

    /* MAC H/w version register format                  */
    /* ------------------------------------------------ */
    /* | 31 - 24 | 23 - 16 | 15 - 12 | 11 - 0 | */
    /* ------------------------------------------------ */
    /* | BN      | Y1      | Y2      |   Y3   | */
    /* ------------------------------------------------ */

    /* Format the version as BN.Y1.Y2.Y3 with all values in hex i.e. the  */
    /* version string would be XX.XX.X.XXX.                                 */
    /* For e.g. 0225020A saved in the version register would translate to */
    /* the configuration interface version number 02.25.0.20A             */

    uc_temp = (ul_dbb_num & 0xF0000000) >> 28;
    pc_dbb_num[0] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);
    uc_temp = (ul_dbb_num & 0x0F000000) >> 24;
    pc_dbb_num[1] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);

    pc_dbb_num[2] = '.';

    uc_temp = (ul_dbb_num & 0x00F00000) >> 20;
    pc_dbb_num[3] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);
    uc_temp = (ul_dbb_num & 0x000F0000) >> 16;
    pc_dbb_num[4] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);
    pc_dbb_num[5] = '.';

    uc_temp = (ul_dbb_num & 0x0000F000) >> 12;
    pc_dbb_num[6] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);
    pc_dbb_num[7] = '.';

    uc_temp = (ul_dbb_num & 0x00000F00) >> 8;
    pc_dbb_num[8] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);
    uc_temp = (ul_dbb_num & 0x000000F0) >> 4;
    pc_dbb_num[9] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);
    uc_temp = (ul_dbb_num & 0x0000000F) >> 0;
    pc_dbb_num[10] = WAL_ATCMDSRV_GET_HEX_CHAR(uc_temp);


    return ;
}

/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_dbb_num
 ��������  : ��ȡdbb�汾��
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_get_dbb_num(oal_net_device_stru *pst_net_dev, oal_int8 *pc_dbb_num)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_int32                       i_leftime;
    mac_vap_stru                   *pst_mac_vap;
    hmac_vap_stru                  *pst_hmac_vap;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_dbb_num::OAL_NET_DEV_PRIV, return null!}");
        return -OAL_EINVAL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY,"{wal_atcmsrv_ioctl_get_dbb_num::mac_res_get_hmac_vap failed!}");
        return OAL_FAIL;
    }

    /***************************************************************************
                              ���¼���wal�㴦��
    ***************************************************************************/
    pst_hmac_vap->st_atcmdsrv_get_status.uc_get_dbb_completed_flag = OAL_FALSE;
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_GET_VERSION, 0);

    l_ret = wal_send_cfg_event(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH,
                             (oal_uint8 *)&st_write_msg,
                             OAL_FALSE,
                             OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_dbb_num::wal_send_cfg_event return err_code [%d]!}", l_ret);
        return l_ret;
    }
    /*�����ȴ�dmac�ϱ�*/
    /*lint -e730*/
    i_leftime = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(OAL_TRUE == pst_hmac_vap->st_atcmdsrv_get_status.uc_get_dbb_completed_flag),WAL_ATCMDSRB_DBB_NUM_TIME);
    /*lint +e730*/
    if ( 0 == i_leftime)
    {
        /* ��ʱ��û���ϱ�ɨ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_dbb_num::dbb_num wait for %ld ms timeout!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else if (i_leftime < 0)
    {
        /* ��ʱ���ڲ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_dbb_num::dbb_num wait for %ld ms error!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else
    {
        /* ��������  */
        OAM_INFO_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_dbb_num::dbb_num wait for %ld ms error!}",
                      ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        if(0x0225020a != pst_hmac_vap->st_atcmdsrv_get_status.ul_dbb_num)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_get_dbb_num:ul_dbb_num[0x%x],not match 0x0225020a", pst_hmac_vap->st_atcmdsrv_get_status.ul_dbb_num);
            return -OAL_EINVAL;
        }
        wal_atcmdsrv_ioctl_convert_dbb_num(pst_hmac_vap->st_atcmdsrv_get_status.ul_dbb_num,(oal_uint8 *)pc_dbb_num);
        return OAL_SUCC;
    }


}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_lte_gpio_mode
 ��������  : LTE���������麸���ģʽ����
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_lte_gpio_mode(oal_net_device_stru *pst_net_dev, oal_int32 l_check_lte_gpio_step)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_int32                       i_leftime;
    mac_vap_stru                   *pst_mac_vap;
    hmac_vap_stru                  *pst_hmac_vap;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_lte_gpio_mode::OAL_NET_DEV_PRIV, return null!}");
        return -OAL_EINVAL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY,"{wal_atcmsrv_ioctl_lte_gpio_mode::mac_res_get_hmac_vap failed!}");
        return -OAL_EINVAL;
    }

    pst_hmac_vap->st_atcmdsrv_get_status.uc_lte_gpio_check_flag = OAL_FALSE;

    /***************************************************************************
         ���¼���wal�㴦��
     ***************************************************************************/
     /* ��д��Ϣ */
     WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_CHECK_LTE_GPIO, OAL_SIZEOF(oal_int32));

     /*����LTE�麸����ģʽ*/
     *(oal_int32 *)(st_write_msg.auc_value) = l_check_lte_gpio_step;

     /* ������Ϣ */
     l_ret = wal_send_cfg_event(pst_net_dev,
                                WAL_MSG_TYPE_WRITE,
                                WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                                (oal_uint8 *)&st_write_msg,
                                OAL_FALSE,
                                OAL_PTR_NULL);

     if (OAL_UNLIKELY(OAL_SUCC != l_ret))
     {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_lte_gpio_mode::return err code %d!}\r\n", l_ret);
         return l_ret;
     }
    /*�����ȴ�dmac�ϱ�*/
    /*lint -e730*/
    i_leftime = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(OAL_TRUE == pst_hmac_vap->st_atcmdsrv_get_status.uc_lte_gpio_check_flag),WAL_ATCMDSRB_DBB_NUM_TIME);
    /*lint +e730*/
    if ( 0 == i_leftime)
    {
        /* ��ʱ��û���ϱ�ɨ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_lte_gpio_mode:: wait for %ld ms timeout!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else if (i_leftime < 0)
    {
        /* ��ʱ���ڲ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_lte_gpio_mode:: wait for %ld ms error!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else
    {
        return OAL_SUCC;
    }
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_lte_gpio_set
 ��������  : LTE���������麸���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_lte_gpio_set(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_int32       l_ret = -OAL_EFAIL;

    l_ret = gpio_request_one(WAL_ATCMDSRV_LTE_ISM_PRIORITY, GPIOF_IN, WAL_ATCMDSRV_LTE_ISM_PRIORITY_NAME);
    if (l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_set:set LTE_ISM_PRIORITY mode gpio fail");
        return -OAL_EFAIL;
    }

    l_ret = gpio_request_one(WAL_ATCMDSRV_LTE_RX_ACT, GPIOF_IN, WAL_ATCMDSRV_LTE_RX_ACT_NAME);
    if (l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_set:set LTE_RX_ACT mode gpio fail");
        gpio_free(WAL_ATCMDSRV_LTE_ISM_PRIORITY);
        return -OAL_EFAIL;
    }

    l_ret = gpio_request_one(WAL_ATCMDSRV_LTE_TX_ACT, GPIOF_IN, WAL_ATCMDSRV_LTE_TX_ACT_NAME);
    if (l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_set:set LTE_TX_ACT mode gpio fail");
        gpio_free(WAL_ATCMDSRV_LTE_ISM_PRIORITY);
        gpio_free(WAL_ATCMDSRV_LTE_RX_ACT);
        return -OAL_EFAIL;
    }
#endif
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_lte_gpio_free
 ��������  : LTE���������麸���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_void  wal_atcmsrv_ioctl_lte_gpio_free(oal_net_device_stru *pst_net_dev)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;

    gpio_free(WAL_ATCMDSRV_LTE_ISM_PRIORITY);

    gpio_free(WAL_ATCMDSRV_LTE_RX_ACT);

    gpio_free(WAL_ATCMDSRV_LTE_TX_ACT);
    /***************************************************************************
         ���¼���wal�㴦��
     ***************************************************************************/
     /* ��д��Ϣ */
     WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_CHECK_LTE_GPIO, OAL_SIZEOF(oal_int32));

     /*����LTE�麸����ģʽ*/
     *(oal_int32 *)(st_write_msg.auc_value) = CHECK_LTE_GPIO_RESUME;

     /* ������Ϣ */
     l_ret = wal_send_cfg_event(pst_net_dev,
                                WAL_MSG_TYPE_WRITE,
                                WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                                (oal_uint8 *)&st_write_msg,
                                OAL_FALSE,
                                OAL_PTR_NULL);

     if (OAL_UNLIKELY(OAL_SUCC != l_ret))
     {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_lte_gpio_mode::return err code %d!}\r\n", l_ret);
     }
#endif
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_lte_gpio_free
 ��������  : LTE���������麸���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_lte_gpio_get(oal_int32 l_check_lte_gpio)
{
    oal_int32   l_fail_gpio_cnt = 0;
 #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if(l_check_lte_gpio != gpio_get_value(WAL_ATCMDSRV_LTE_ISM_PRIORITY))
    {
        OAM_ERROR_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_get:LTE_ISM_PRIORITY FAIL");
        l_fail_gpio_cnt++;
    }

    if(l_check_lte_gpio != gpio_get_value(WAL_ATCMDSRV_LTE_RX_ACT))
    {
        OAM_ERROR_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_get:LTE_RX_ACT FAIL");
        l_fail_gpio_cnt++;
    }

    if(l_check_lte_gpio != gpio_get_value(WAL_ATCMDSRV_LTE_TX_ACT))
    {
        OAM_ERROR_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_get:LTE_TX_ACT FAIL");
        l_fail_gpio_cnt++;
    }
    OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_get:ALL GPIO IS OK");
 #endif
    return l_fail_gpio_cnt;
}

/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_lte_gpio_check
 ��������  : LTE���������麸���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_lte_gpio_check(oal_net_device_stru *pst_net_dev)
{
    oal_int32                       l_ret;
    /*********step1 ���ùܽ�Ϊgpioģʽ********/
    OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_check:enter lte gpio check!");
    /*��ʼ��host�ܽ�*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_set();
    if(OAL_SUCC != l_ret)
    {
        return l_ret;
    }
    /*��ʼ��device lte�������ż��*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_mode(pst_net_dev,CHECK_LTE_GPIO_INIT);

    if(OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_check:CHECK_LTE_GPIO_INIT FAIL!");
        wal_atcmsrv_ioctl_lte_gpio_free(pst_net_dev);
        return l_ret;
    }
    /*********step2 ����device�ܽ�Ϊ�ͣ���ȡhost���********/
    /*��gpioȫ������Ϊ��*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_mode(pst_net_dev,CHECK_LTE_GPIO_LOW);

    if(OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_check:CHECK_LTE_GPIO_LOW FAIL!");
        wal_atcmsrv_ioctl_lte_gpio_free(pst_net_dev);
        return l_ret;
    }
    /*��ȡGPIO�ܽŵ�ƽ*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_get(0);
    if(0 != l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_check:check gpio low mode FAIL!");
        wal_atcmsrv_ioctl_lte_gpio_free(pst_net_dev);
        return l_ret;
    }

    /*********step3 ����device�ܽ�Ϊ�ߣ���ȡhost���********/
    /*��gpioȫ������Ϊ��*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_mode(pst_net_dev,CHECK_LTE_GPIO_HIGH);

    if(OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_check:CHECK_LTE_GPIO_HIGH FAIL!");
        wal_atcmsrv_ioctl_lte_gpio_free(pst_net_dev);
        return l_ret;
    }
    /*��ȡGPIO�ܽŵ�ƽ*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_get(1);
    if(0 != l_ret)
    {
        OAM_WARNING_LOG0(0, 0, "wal_atcmsrv_ioctl_lte_gpio_check:check gpio high mode FAIL!");
        wal_atcmsrv_ioctl_lte_gpio_free(pst_net_dev);
        return l_ret;
    }
    wal_atcmsrv_ioctl_lte_gpio_free(pst_net_dev);
    return OAL_SUCC;
 }
 oal_uint64                      ul_gpio_wakeup_host_int_get_save;
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_hw_status
 ��������  : ��ȡӲ�������������fem��pa��host wakeup device���š�lte����ܽ�
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_atcmsrv_ioctl_get_hw_status(oal_net_device_stru *pst_net_dev, oal_int32 *pl_fem_pa_status)
{
    oal_cali_param_stru            *pst_cali_data;
    oal_uint64                      ul_gpio_wakeup_host_int_get;
    oal_uint32                      ul_check_gpio_wakeup_host_status = 0;
    oal_int32                       l_ret = 0;
    oal_int8                        auc_dbb[WAL_ATCMDSRV_IOCTL_DBB_LEN];
    oal_uint32                      ul_lte_status = 0;
    /*device�ϱ���Ϣ�����������жϣ���⻽������*/
    l_ret = wal_atcmsrv_ioctl_get_dbb_num(pst_net_dev,auc_dbb);
    if(OAL_SUCC != l_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmsrv_ioctl_get_fem_pa_status:Failed to get dbb num !");
    }

    /*device����host gpio���ż��*/
    ul_gpio_wakeup_host_int_get = oal_get_gpio_int_count_para();

    if(ul_gpio_wakeup_host_int_get_save == ul_gpio_wakeup_host_int_get)
    {
        ul_check_gpio_wakeup_host_status = 1;

        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_get_fem_pa_status:check wl_host_wake_up gpio fail!");
    }
    ul_gpio_wakeup_host_int_get_save = ul_gpio_wakeup_host_int_get;


    pst_cali_data = (oal_cali_param_stru *)get_cali_data_buf_addr();
    /*��ȡlte����ܽŽ��*/
    l_ret = wal_atcmsrv_ioctl_lte_gpio_check(pst_net_dev);

    if(OAL_SUCC != l_ret)
    {
        ul_lte_status = 1;
    }
    *pl_fem_pa_status = (oal_int32)(pst_cali_data->ul_check_hw_status|(ul_check_gpio_wakeup_host_status << 4)|(ul_lte_status << 5));

    if(0 != *pl_fem_pa_status)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_fem_pa_status::fem_pa_status[bit0-bit1],lna_status[bit2-bit3],gpio[bit4],lte_gpio[bit5];ul_check_hw_status[0x%x]}", *pl_fem_pa_status);
    }


    return OAL_SUCC;


}

/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_fem_pa_status
 ��������  : ��ȡfem��pa�Ƿ��ջ�
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void  wal_atcmsrv_ioctl_get_fem_pa_status(oal_net_device_stru *pst_net_dev, oal_int32 *pl_fem_pa_status)
{
    oal_cali_param_stru            *pst_cali_data;
    oal_uint64                      ul_gpio_wakeup_host_int_get;
    oal_uint32                      ul_check_gpio_wakeup_host_status = 0;
    oal_int32                       l_ret = 0;
    oal_int8                        auc_dbb[WAL_ATCMDSRV_IOCTL_DBB_LEN];
    /*device�ϱ���Ϣ�����������жϣ���⻽������*/
    l_ret = wal_atcmsrv_ioctl_get_dbb_num(pst_net_dev,auc_dbb);
    if(OAL_SUCC != l_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmsrv_ioctl_get_fem_pa_status:Failed to get dbb num !");
    }

    /*device����host gpio���ż��*/
    ul_gpio_wakeup_host_int_get = oal_get_gpio_int_count_para();

    if(ul_gpio_wakeup_host_int_get_save == ul_gpio_wakeup_host_int_get)
    {
        ul_check_gpio_wakeup_host_status = 1;

        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_get_fem_pa_status:check wl_host_wake_up gpio fail!");
    }
    ul_gpio_wakeup_host_int_get_save = ul_gpio_wakeup_host_int_get;


    pst_cali_data = (oal_cali_param_stru *)get_cali_data_buf_addr();

    *pl_fem_pa_status = (oal_int32)(pst_cali_data->ul_check_hw_status|(ul_check_gpio_wakeup_host_status << 4));

    if(0 != *pl_fem_pa_status)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_fem_pa_status::fem_pa_status[bit0-bit1],lna_status[bit2-bit3],gpio[bit4];ul_check_hw_status[0x%x]}", *pl_fem_pa_status);
    }


}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_always_rx
 ��������  : �򿪳��չ���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_always_rx(oal_net_device_stru *pst_net_dev, oal_int32 l_always_rx)
{
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    oal_uint8                        auc_param[] = {"all"};
    oal_uint16                       us_len;

     /*��״̬��ֵ*/
     *(oal_uint8 *)(st_write_msg.auc_value) = (oal_uint8)l_always_rx;

    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_RX, OAL_SIZEOF(oal_uint8));

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_rx::return err code [%d]!}\r\n", l_ret);
        return l_ret;
    }

     /*��ӡ���мĴ���*/
    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    oal_memcopy(st_write_msg.auc_value, auc_param, OAL_STRLEN((oal_int8*)auc_param));

    us_len = (oal_uint16)(OAL_STRLEN((oal_int8*)auc_param));

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REG_INFO, us_len);

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_always_rx::return err code [%d]!}\r\n", l_ret);
    }
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_hw_addr
 ��������  : ���ý���mac��ַ
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_hw_addr(oal_net_device_stru *pst_net_dev, oal_uint8 *pc_hw_addr)
{
    oal_int32                       l_ret;
    mac_cfg_staion_id_param_stru   *pst_mac_cfg_para;
    wal_msg_write_stru              st_write_msg;


    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_STATION_ID, OAL_SIZEOF(mac_cfg_staion_id_param_stru));

    /* ��������������� */
    pst_mac_cfg_para = (mac_cfg_staion_id_param_stru *)(st_write_msg.auc_value);
    /*������������02�Ѿ�û������*/
    pst_mac_cfg_para->en_p2p_mode = WLAN_LEGACY_VAP_MODE;
    oal_set_mac_addr(pst_mac_cfg_para->auc_station_id, pc_hw_addr);

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_staion_id_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::return err code %d!}\r\n", l_ret);
        return l_ret;
    }
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_rx_pckg
 ��������  : ��ѯ�������ݰ��ĸ���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  wal_atcmsrv_ioctl_get_rx_pckg(oal_net_device_stru *pst_net_dev, oal_int32 *pl_rx_pckg_succ_num)
{
    oal_int32                   l_ret;
    mac_cfg_rx_fcs_info_stru   *pst_rx_fcs_info;
    wal_msg_write_stru          st_write_msg;
    mac_vap_stru               *pst_mac_vap;
    hmac_vap_stru              *pst_hmac_vap;
    oal_int32                   i_leftime;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_dbb_num::OAL_NET_DEV_PRIV, return null!}");
        return -OAL_EINVAL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY,"{wal_atcmsrv_ioctl_get_dbb_num::mac_res_get_hmac_vap failed!}");
        return OAL_FAIL;
    }

    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    pst_hmac_vap->st_atcmdsrv_get_status.uc_get_rx_pkct_flag = OAL_FALSE;
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_RX_FCS_INFO, OAL_SIZEOF(mac_cfg_rx_fcs_info_stru));

    /* ��������������� */
    pst_rx_fcs_info = (mac_cfg_rx_fcs_info_stru *)(st_write_msg.auc_value);
    /*������������02�Ѿ�û������*/
    pst_rx_fcs_info->ul_data_op    = 0;
    pst_rx_fcs_info->ul_print_info = 0;

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_rx_fcs_info_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /*�����ȴ�dmac�ϱ�*/
    i_leftime = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(oal_uint32)(OAL_TRUE == pst_hmac_vap->st_atcmdsrv_get_status.uc_get_rx_pkct_flag),WAL_ATCMDSRB_GET_RX_PCKT);

    if ( 0 == i_leftime)
    {
        /* ��ʱ��û���ϱ�ɨ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_pckg::dbb_num wait for %ld ms timeout!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else if (i_leftime < 0)
    {
        /* ��ʱ���ڲ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_pckg::dbb_num wait for %ld ms error!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else
    {
        /* ��������  */
        OAM_INFO_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_pckg::dbb_num wait for %ld ms error!}",
                      ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        *pl_rx_pckg_succ_num = (oal_int)pst_hmac_vap->st_atcmdsrv_get_status.ul_rx_pkct_succ_num;
        return OAL_SUCC;
    }
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_pm_switch
 ��������  :
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��12��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_pm_switch(oal_net_device_stru *pst_net_dev, oal_int32 l_pm_switch)
{
    wal_msg_write_stru          st_write_msg;

    oal_int32                   l_ret;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_set_pm_switch:l_pm_switch[%d]", l_pm_switch);

    *(oal_uint8 *)(st_write_msg.auc_value) = (oal_uint8)l_pm_switch;

    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_PM_SWITCH, OAL_SIZEOF(oal_int32));

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_set_pm_switch::return err code [%d]!}\r\n", l_ret);
        return l_ret;
    }

    return OAL_SUCC;

}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_rx_rssi
 ��������  : ��ѯ����RSSIֵ
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_get_rx_rssi(oal_net_device_stru *pst_net_dev, oal_int32 *pl_rx_rssi)
{
    oal_int32                   l_ret;
    mac_cfg_rx_fcs_info_stru   *pst_rx_fcs_info;
    wal_msg_write_stru          st_write_msg;
    mac_vap_stru               *pst_mac_vap;
    hmac_vap_stru              *pst_hmac_vap;
    oal_int32                   i_leftime;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_rssi::OAL_NET_DEV_PRIV, return null!}");
        return -OAL_EINVAL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY,"{wal_atcmsrv_ioctl_get_rx_rssi::mac_res_get_hmac_vap failed!}");
        return OAL_FAIL;
    }

    /***************************************************************************
                                ���¼���wal�㴦��
    ***************************************************************************/
    pst_hmac_vap->st_atcmdsrv_get_status.uc_get_rx_pkct_flag = OAL_FALSE;
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_RX_FCS_INFO, OAL_SIZEOF(mac_cfg_rx_fcs_info_stru));

    /* ��������������� */
    pst_rx_fcs_info = (mac_cfg_rx_fcs_info_stru *)(st_write_msg.auc_value);
    /*������������02�Ѿ�û������*/
    pst_rx_fcs_info->ul_data_op    = 0;
    pst_rx_fcs_info->ul_print_info = 0;

    l_ret = wal_send_cfg_event(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_rx_fcs_info_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_rssi::return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /*�����ȴ�dmac�ϱ�*/
    i_leftime = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(oal_uint32)(OAL_TRUE == pst_hmac_vap->st_atcmdsrv_get_status.uc_get_rx_pkct_flag),WAL_ATCMDSRB_GET_RX_PCKT);

    if ( 0 == i_leftime)
    {
        /* ��ʱ��û���ϱ�ɨ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_rssi::dbb_num wait for %ld ms timeout!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else if (i_leftime < 0)
    {
        /* ��ʱ���ڲ����� */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_rssi::dbb_num wait for %ld ms error!}",
                         ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        return -OAL_EINVAL;
    }
    else
    {
        /* ��������  */
        OAM_INFO_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_atcmsrv_ioctl_get_rx_rssi::dbb_num wait for %ld ms error!}",
                      ((WAL_ATCMDSRB_DBB_NUM_TIME * 1000)/OAL_TIME_HZ));
        *pl_rx_rssi = (oal_int)pst_hmac_vap->st_atcmdsrv_get_status.s_rx_rssi;
        return OAL_SUCC;
    }
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_chipcheck
 ��������  : ����оƬ�Լ죬��ȡ�Լ���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_chipcheck(oal_net_device_stru *pst_net_dev, oal_int32 *l_chipcheck_result)
{
    oal_int32                ul_ret;
    ul_ret = wlan_device_mem_check();

    return ul_ret;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_chipcheck_result
 ��������  : ����оƬ�Լ죬��ȡ�Լ���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_get_chipcheck_result(oal_net_device_stru *pst_net_dev, oal_int32 *l_chipcheck_result)
{

    *l_chipcheck_result = wlan_device_mem_check_result(&ul_chipcheck_total_time);

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_get_chipcheck_result:result[%d]",*l_chipcheck_result);
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_get_chipcheck_time
 ��������  : ��ѯchipcheck����ʱ��
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_get_chipcheck_time(oal_net_device_stru *pst_net_dev, oal_uint64 *ul_chipcheck_time)
{
    *ul_chipcheck_time = ul_chipcheck_total_time;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_atcmsrv_ioctl_get_chipcheck_time:[%d]",ul_chipcheck_total_time);
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_uart_loop
 ��������  : uart���ز���
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_uart_loop(oal_net_device_stru *pst_net_dev, oal_int32 *l_uart_loop_set)
{
    return 0;//conn_test_uart_loop((oal_int8 *)&l_uart_loop_set);
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_set_sdio_loop
 ��������  : ��ѯchipcheck����ʱ��
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_set_sdio_loop(oal_net_device_stru *pst_net_dev, oal_int32 *l_sdio_loop_set)
{
    return 0;//conn_test_sdio_loop((oal_int8 *)&l_sdio_loop_set);
}
/*****************************************************************************
 �� �� ��  : wal_atcmsrv_ioctl_fetch_caldata
 ��������  : ��ȡУ׼����
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��10��
    ��    ��   :  
    �޸�����   : �����ɺ���

*****************************************************************************/
extern oal_int32 hwifi_fetch_ori_caldata(oal_uint8* auc_caldata, oal_int32 l_nvm_len);
OAL_STATIC oal_int32  wal_atcmsrv_ioctl_fetch_caldata(oal_uint8* auc_caldata)
{
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    return hwifi_fetch_ori_caldata(auc_caldata, WAL_ATCMDSRV_NV_WINVRAM_LENGTH);
#else
    return 0;
#endif
}
/*****************************************************************************
 �� �� ��  : wal_atcmdsrv_wifi_priv_cmd
 ��������  : ˽�нӿں���
 �������  : net deviceָ��
 �������  : ��
 �� �� ֵ  : ͳ�ƽ��ָ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��06��25��
    ��    ��   : 
    �޸�����   : ��������

*****************************************************************************/
oal_int32 wal_atcmdsrv_wifi_priv_cmd(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int32 ul_cmd)
{
    wal_atcmdsrv_wifi_priv_cmd_stru  st_priv_cmd;
    oal_int32    l_ret              = OAL_SUCC;


    if ((OAL_PTR_NULL == pst_ifr->ifr_data)||(OAL_PTR_NULL == pst_net_dev))
    {
        l_ret = -OAL_EINVAL;
        return l_ret;
    }
    /*���û�̬���ݿ������ں�̬*/
    if (oal_copy_from_user(&st_priv_cmd, pst_ifr->ifr_data, sizeof(wal_android_wifi_priv_cmd_stru)))
    {
		l_ret = -OAL_EINVAL;
		return l_ret;
	}
    if(st_priv_cmd.l_verify != WAL_ATCMDSRV_IOCTL_VERIFY_CODE)
    {
        OAM_WARNING_LOG2(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:ioctl verify failed,verify code is:%d(not equal %d)", st_priv_cmd.l_verify, WAL_ATCMDSRV_IOCTL_VERIFY_CODE);
        return -OAL_EINVAL;
    }
    switch(st_priv_cmd.ul_cmd)
    {
        case WAL_ATCMDSRV_IOCTL_CMD_WI_FREQ_SET:
             l_ret = wal_atcmsrv_ioctl_set_freq(pst_net_dev,st_priv_cmd.pri_data.l_freq);
             break;
        case WAL_ATCMDSRV_IOCTL_CMD_WI_POWER_SET:
             l_ret = wal_atcmsrv_ioctl_set_txpower(pst_net_dev,st_priv_cmd.pri_data.l_pow);
             break;
        case WAL_ATCMDSRV_IOCTL_CMD_MODE_SET:
             l_ret = wal_atcmsrv_ioctl_set_mode(pst_net_dev,st_priv_cmd.pri_data.l_mode);
             break;
        case WAL_ATCMDSRV_IOCTL_CMD_DATARATE_SET:
             l_ret = wal_atcmsrv_ioctl_set_datarate(pst_net_dev,st_priv_cmd.pri_data.l_datarate);
             break;
        case WAL_ATCMDSRV_IOCTL_CMD_BAND_SET:
             l_ret = wal_atcmsrv_ioctl_set_bandwidth(pst_net_dev,st_priv_cmd.pri_data.l_bandwidth);
             break;
        case WAL_ATCMDSRV_IOCTL_CMD_ALWAYS_TX_SET:
             l_ret = wal_atcmsrv_ioctl_set_always_tx(pst_net_dev,st_priv_cmd.pri_data.l_awalys_tx);
             break;
        case WAL_ATCMDSRV_IOCTL_CMD_DBB_GET:
             l_ret = wal_atcmsrv_ioctl_get_dbb_num(pst_net_dev,st_priv_cmd.pri_data.auc_dbb);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_HW_STATUS_GET:
            l_ret = wal_atcmsrv_ioctl_get_hw_status(pst_net_dev,&st_priv_cmd.pri_data.l_fem_pa_status);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_ALWAYS_RX_SET:
            l_ret = wal_atcmsrv_ioctl_set_always_rx(pst_net_dev,st_priv_cmd.pri_data.l_awalys_rx);
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_HW_ADDR_SET:
            l_ret = wal_atcmsrv_ioctl_set_hw_addr(pst_net_dev,st_priv_cmd.pri_data.auc_mac_addr);
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_RX_PCKG_GET:
            l_ret = wal_atcmsrv_ioctl_get_rx_pckg(pst_net_dev,&st_priv_cmd.pri_data.l_rx_pkcg);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_PM_SWITCH:
            l_ret = wal_atcmsrv_ioctl_set_pm_switch(pst_net_dev,st_priv_cmd.pri_data.l_pm_switch);
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_RX_RSSI:
            l_ret = wal_atcmsrv_ioctl_get_rx_rssi(pst_net_dev,&st_priv_cmd.pri_data.l_rx_rssi);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_SET:
            l_ret = wal_atcmsrv_ioctl_set_chipcheck(pst_net_dev,&st_priv_cmd.pri_data.l_chipcheck_result);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
         case WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_RESULT:
            l_ret = wal_atcmsrv_ioctl_get_chipcheck_result(pst_net_dev,&st_priv_cmd.pri_data.l_chipcheck_result);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
         case WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_TIME:
            l_ret = wal_atcmsrv_ioctl_get_chipcheck_time(pst_net_dev,&st_priv_cmd.pri_data.l_chipcheck_time);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
         case WAL_ATCMDSRV_IOCTL_CMD_UART_LOOP_SET:
            l_ret = wal_atcmsrv_ioctl_set_uart_loop(pst_net_dev,&st_priv_cmd.pri_data.l_uart_loop_set);
            break;
         case WAL_ATCMDSRV_IOCTL_CMD_SDIO_LOOP_SET:
            l_ret = wal_atcmsrv_ioctl_set_sdio_loop(pst_net_dev,&st_priv_cmd.pri_data.l_sdio_loop_set);
            break;
        case WAL_ATCMDSRV_IOCTL_CMD_RD_CALDATA:
            l_ret = wal_atcmsrv_ioctl_fetch_caldata(st_priv_cmd.pri_data.auc_caldata);
            if(oal_copy_to_user(pst_ifr->ifr_data,&st_priv_cmd,sizeof(wal_atcmdsrv_wifi_priv_cmd_stru)))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY,"wal_atcmdsrv_wifi_priv_cmd:Failed to copy ioctl_data(caldata) to user !");
                l_ret = -OAL_EINVAL;
            }
            break;
        default:
             break;
    }
    return l_ret;
}
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

