/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_main.c
  �� �� ��   : ����
  ��    ��   : ������
  ��������   : 2012��9��18��
  ����޸�   :
  ��������   : OAMģ���ʼ����ж��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ļ�

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION))
/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oam_main.h"
#include "oam_log.h"
#include "oam_event.h"
#include "oam_alarm.h"
#include "oam_trace.h"
#include "oam_statistics.h"
#if (_PRE_PRODUCT_ID !=_PRE_PRODUCT_ID_HI1131C_DEV)
#include "oam_config.h"
#endif
#include "oam_linux_netlink.h"
#include "oam_ext_if.h"

#include "oal_hcc_host_if.h"
#if ((_PRE_OS_VERSION_WIN32 != _PRE_OS_VERSION))
#include "sdt_drv.h"
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* OAMģ��ͳһʹ�õ�ȫ�ֲ������������ģ�����OAM������ģ��ȫ�������� */
oam_mng_ctx_stru    g_st_oam_mng_ctx;

/* ��ӡ���ͺ������� */
OAL_STATIC oal_print_func g_pa_oam_print_type_func[OAM_OUTPUT_TYPE_BUTT]=
{
    oam_print_to_console,   /* OAM_OUTPUT_TYPE_CONSOLE ����̨��� */
    oam_print_to_file,      /* OAM_OUTPUT_TYPE_FS д���ļ�ϵͳ */
    oam_print_to_sdt,       /* OAM_OUTPUT_TYPE_SDT �����SDT,�ϱ��ַ������˴���2048 */
};

/* ���ں�SDT���߽�����ȫ�ֱ��� */
oam_sdt_func_hook_stru          g_st_oam_sdt_func_hook;
oam_wal_func_hook_stru          g_st_oam_wal_func_hook;
oam_sdt_stat_info_stru          g_st_sdt_stat_info;

oal_uint8 g_auc_bcast_addr[WLAN_MAC_ADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#ifdef _PRE_DEBUG_MODE  /* ��������Ĭ�Ͽ���״̬ */
oal_uint32          g_aul_debug_feature_switch[OAM_DEBUG_TYPE_BUTT] =
{
    OAL_SWITCH_OFF,   /* OAM_DEBUG_TYPE_ECHO_REG */
};
#endif

/* �����б� */
oam_software_feature_stru   gst_oam_feature_list[OAM_SOFTWARE_FEATURE_BUTT] =
{
    /*���Ժ�ID                  ��������д*/
    /* 0 */
    {OAM_SF_SCAN,               "scan"},
    {OAM_SF_AUTH,               "auth"},
    {OAM_SF_ASSOC,              "assoc"},
    {OAM_SF_FRAME_FILTER,       "ff"},
    {OAM_SF_WMM,                "wmm"},

    /* 5 */
    {OAM_SF_DFS,                "dfs"},
    {OAM_SF_NETWORK_MEASURE,    "nm"},
    {OAM_SF_ENTERPRISE_VO,      "ev"},
    {OAM_SF_HOTSPOTROAM,        "roam"},
    {OAM_SF_NETWROK_ANNOUNCE,   "11u"},

    /* 10 */
    {OAM_SF_NETWORK_MGMT,       "11k"},
    {OAM_SF_NETWORK_PWS,        "pws"},
    {OAM_SF_PROXYARP,           "proxyarp"},
    {OAM_SF_TDLS,               "tdls"},
    {OAM_SF_CALIBRATE,          "cali"},

    /* 15 */
    {OAM_SF_EQUIP_TEST,         "equip"},
    {OAM_SF_CRYPTO,             "crypto"},
    {OAM_SF_WPA,                "wpa"},
    {OAM_SF_WEP,                "wep"},
    {OAM_SF_WPS,                "wps"},

    /* 20 */
    {OAM_SF_PMF,                "pmf"},
    {OAM_SF_WAPI,               "wapi"},
    {OAM_SF_BA,                 "ba"},
    {OAM_SF_AMPDU,              "ampdu"},
    {OAM_SF_AMSDU,              "amsdu"},

    /* 25 */
    {OAM_SF_STABILITY,          "dfr"},
    {OAM_SF_TCP_OPT,            "tcp"},
    {OAM_SF_ACS,                "acs"},
    {OAM_SF_AUTORATE,           "autorate"},
    {OAM_SF_TXBF,               "txbf"},

    /* 30 */
    {OAM_SF_DYN_RECV,           "weak"},
    {OAM_SF_VIVO,               "vivo"},
    {OAM_SF_MULTI_USER,         "muser"},
    {OAM_SF_MULTI_TRAFFIC,      "mtraff"},
    {OAM_SF_ANTI_INTF,          "anti_intf"},

    /* 35 */
    {OAM_SF_EDCA,               "edca"},
    {OAM_SF_SMART_ANTENNA,      "ani"},
    {OAM_SF_TPC,                "tpc"},
    {OAM_SF_TX_CHAIN,           "txchain"},
    {OAM_SF_RSSI,               "rssi"},

    /* 40 */
    {OAM_SF_WOW,                "wow"},
    {OAM_SF_GREEN_AP,           "green"},
    {OAM_SF_PWR,                "pwr"},
    {OAM_SF_SMPS,               "smps"},
    {OAM_SF_TXOP,               "txop"},

    /* 45 */
    {OAM_SF_WIFI_BEACON,        "beacon"},
    {OAM_SF_KA_AP,              "alive"},
    {OAM_SF_MULTI_VAP,          "mvap"},
    {OAM_SF_2040,               "2040"},
    {OAM_SF_DBAC,               "dbac"},

    /* 50 */
    {OAM_SF_PROXYSTA,           "proxysta"},
    {OAM_SF_UM,                 "um"},
    {OAM_SF_P2P,                "p2p"},
    {OAM_SF_M2U,                "m2u"},
    {OAM_SF_IRQ,                "irq"},

    /* 55 */
    {OAM_SF_TX,                 "tx"},
    {OAM_SF_RX,                 "rx"},
    {OAM_SF_DUG_COEX,           "dugcoex"},
    {OAM_SF_CFG,                "cfg"},
    {OAM_SF_FRW,                "frw"},

    /* 60 */
    {OAM_SF_KEEPALIVE,          "keepalive"},
    {OAM_SF_COEX,               "coex"},
    {OAM_SF_HS20,               "hs20"},
    {OAM_SF_MWO_DET,            "mwodet"},
    {OAM_SF_CCA_OPT,            "cca"},

    {OAM_SF_ROAM,               "roam"},
    {OAM_SF_DFT,                "dft"},
    {OAM_SF_DFR,                "dfr"},
    {OAM_SF_BACKUP,             "backup"},
    {OAM_SF_ANY,                "any"},
};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : oam_print
 ��������  : OAMģ���ṩ���������
 �������  : ��ʽ�����ַ���
 �������  : ��
 �� �� ֵ  : �ɹ�����ʧ��ԭ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  oam_print(oal_int8 *pc_string)
{
    oam_output_type_enum_uint8 en_output_type;
    oal_uint32                 ul_rslt;

    ul_rslt = oam_get_output_type(&en_output_type);
    if (OAL_SUCC != ul_rslt)
    {
        return ul_rslt;
    }

    ul_rslt = g_pa_oam_print_type_func[en_output_type](pc_string);
    if (OAL_SUCC != ul_rslt)
    {
        return ul_rslt;
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_print_to_console
 ��������  : ��ӡ��Ϣ����׼���������
 �������  : pc_string : ��Ҫ��ӡ����׼��������е��ַ�������\0������
 �������  : ��
 �� �� ֵ  : OAL_SUCC�����������롣
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 oam_print_to_console(oal_int8 *pc_string)
{
    if (OAL_UNLIKELY(OAL_PTR_NULL == pc_string))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_IO_PRINT("%s\r\n", pc_string);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_print_to_file
 ��������  : ��ӡ��Ϣ���ļ���
 �������  : pc_string : ��Ҫ��ӡ���ļ��е��ַ�������\0������
 �������  : ��
 �� �� ֵ  : OAL_SUCC�����������롣
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32   oam_print_to_file(oal_int8 *pc_string)
{
#ifdef _PRE_WIFI_DMT

    oal_file_stru            *f_file_ret;                                 /* ���ڱ���д�ļ���ķ���ֵ */
    oal_file_stru            *f_event_file;
    oal_int32                 l_rslt;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pc_string))
    {
        OAM_IO_PRINTK("null param. \r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    f_event_file = oal_file_open(g_st_oam_mng_ctx.ac_file_path,(OAL_O_CREAT | OAL_O_APPEND), 0);

    if (OAL_UNLIKELY(OAL_FILE_FAIL == f_event_file))
    {
        OAM_IO_PRINTK("open file failed. \r\n");
        return OAL_ERR_CODE_OPEN_FILE_FAIL;
    }

    f_file_ret = oal_file_write(f_event_file, pc_string, (OAL_STRLEN(pc_string) + 1));
    if (OAL_FILE_FAIL == f_file_ret)
    {
        l_rslt = oal_file_close(f_event_file);
        if (0 != l_rslt)
        {
            OAM_IO_PRINTK("close file failed. \r\n");
            return OAL_ERR_CODE_CLOSE_FILE_FAIL;
        }

        OAM_IO_PRINTK("write file failed. \r\n");
        return OAL_ERR_CODE_WRITE_FILE_FAIL;
    }

    l_rslt = oal_file_close(f_event_file);

    if (0 != l_rslt)
    {
        OAM_IO_PRINTK("close file failed. \r\n");
        return OAL_ERR_CODE_CLOSE_FILE_FAIL;
    }
#endif
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_print_to_sdt
 ��������  : ��ӡ��Ϣ��PC���ά�ɲ⹤��ƽ̨��
 �������  : pc_string : ��Ҫ��ӡ����ά�ɲ⹤��ƽ̨�е��ַ�������\0������
 �������  : ��
 �� �� ֵ  : OAL_SUCC�����������롣
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 oam_print_to_sdt(oal_int8 *pc_string)
{
    oal_netbuf_stru                *pst_skb;
    oal_uint32                      ul_ret       = OAL_SUCC;
    oal_uint16                      us_strlen;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pc_string))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (OAL_UNLIKELY(OAL_PTR_NULL == g_st_oam_sdt_func_hook.p_sdt_report_data_func))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* �ϱ�sdt�ַ�����'0'���� */
    us_strlen = (oal_uint16)OAL_STRLEN(pc_string);

    us_strlen = (us_strlen > OAM_REPORT_MAX_STRING_LEN) ? OAM_REPORT_MAX_STRING_LEN : us_strlen;

    pst_skb = oam_alloc_data2sdt(us_strlen);
    if (OAL_PTR_NULL == pst_skb)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* COPY��ӡ������ */
    oal_memset(oal_netbuf_data(pst_skb), 0, us_strlen);
    oal_memcopy(oal_netbuf_data(pst_skb), pc_string, (oal_uint32)us_strlen);

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_skb, OAM_DATA_TYPE_STRING, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

/*****************************************************************************
 �� �� ��  : oam_upload_log_to_sdt
 ��������  : ��ӡ��Ϣ��PC���ά�ɲ⹤��ƽ̨��
 �������  : pc_string : ��Ҫ��ӡ����ά�ɲ⹤��ƽ̨�е��ַ�������\0������
 �������  : ��
 �� �� ֵ  : OAL_SUCC�����������롣
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��15��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 oam_upload_log_to_sdt(oal_int8 *pc_string)
{
    oal_netbuf_stru        *pst_skb;
    oal_uint32              ul_ret      = OAL_SUCC;

    if (OAL_UNLIKELY(OAL_PTR_NULL == g_st_oam_sdt_func_hook.p_sdt_report_data_func))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (OAL_PTR_NULL == pc_string)
    {
        OAL_IO_PRINT("oam_upload_log_to_sdt::pc_string is null!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_skb = oam_alloc_data2sdt(OAL_SIZEOF(oam_log_info_stru));
    if (OAL_PTR_NULL == pst_skb)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* COPY��ӡ������ */
    oal_memcopy(oal_netbuf_data(pst_skb), pc_string, OAL_SIZEOF(oam_log_info_stru));

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_skb, OAM_DATA_TYPE_LOG, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

oal_uint32 oam_upload_device_log_to_sdt(oal_uint8 *pc_string, oal_uint16 len)
{
    oal_netbuf_stru        *pst_skb;
    oal_uint32              ul_ret      = OAL_SUCC;

    if (OAL_PTR_NULL == pc_string)
    {
        OAL_IO_PRINT("oam_upload_log_to_sdt::pc_string is null!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_skb = oam_alloc_data2sdt(len);
    if (OAL_PTR_NULL == pst_skb)
    {
        OAL_IO_PRINT("alloc netbuf stru failed!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* COPY��ӡ������ */
    oal_memcopy(oal_netbuf_data(pst_skb), pc_string, len);

    /* �·���sdt���ն��У����������򴮿���� */
    ul_ret = oam_report_data2sdt(pst_skb, OAM_DATA_TYPE_DEVICE_LOG, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}


oal_uint32 oam_send_device_data2sdt(oal_uint8* pc_string, oal_uint16 len)
{
    oal_uint32 ul_ret = OAL_SUCC;
    if (NULL == pc_string)
    {
        return OAL_EFAIL;
    }

    ul_ret = oam_upload_device_log_to_sdt(pc_string, len);

    return ul_ret;
}

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
oal_int32 oam_rx_post_action_function(oal_uint8 stype,
                                             hcc_netbuf_stru* pst_hcc_netbuf, oal_uint8 *pst_context)
{
    oal_uint8   *puc_data;
    OAL_REFERENCE(pst_context);
    OAL_BUG_ON(NULL == pst_hcc_netbuf);

    puc_data = oal_netbuf_data(pst_hcc_netbuf->pst_netbuf);
    if((DUMP_REG == stype) || (DUMP_MEM == stype))
    {
       //AL_IO_PRINT("receive dump msg  = %d\n", stype);
       // exception_bcpu_dump_recv(puc_data,pst_hcc_netbuf->pst_netbuf);
        OAL_IO_PRINT("error , this function is supported in hi1131c\rs\n");
        oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
        return OAL_SUCC;
    }

    /* ����OAM�ӿ�*/
    oam_send_device_data2sdt(puc_data, (oal_uint16)pst_hcc_netbuf->len);

    oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
    return OAL_SUCC;
}
#endif

/*****************************************************************************
 �� �� ��  : oam_get_output_type
 ��������  : ��ȡ��ά�ɲ������ʽ
 �������  : ��
 �������  : ��ά�ɲ������ʽ
 �� �� ֵ  : 1) OAL_ERR_CODE_PTR_NULL: ��ָ��
             2) OAL_SUCC: �ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  oam_get_output_type(oam_output_type_enum_uint8 *pen_output_type)
{
    if (OAL_UNLIKELY(OAL_PTR_NULL == pen_output_type))
    {
        OAM_IO_PRINTK("null param \r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    *pen_output_type = g_st_oam_mng_ctx.en_output_type;

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_set_output_type
 ��������  : ���ÿ�ά�ɲ������ʽ
 �������  : �����ʽ
 �������  : ��
 �� �� ֵ  : OAL_SUCC: ���óɹ�
             OAL_FAIL: ����ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  oam_set_output_type(oam_output_type_enum_uint8 en_output_type)
{
    if (en_output_type >= OAM_OUTPUT_TYPE_BUTT)
    {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    g_st_oam_mng_ctx.en_output_type = en_output_type;

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_set_file_path
 ��������  : 1) �������ÿ�ά�ɲ��ļ�·��
             2) �ļ�·���ַ����ĳ���(����\0)
 �������  : ָ���ά�ɲ��ļ�·����ָ��
 �������  : ��
 �� �� ֵ  : 1) OAL_ERR_CODE_PTR_NULL: ��ָ��
             2) OAL_SUCC: �ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  oam_set_file_path(oal_int8 *pc_file_path, oal_uint32 ul_length)
{
#ifdef _PRE_WIFI_DMT
    oal_file_stru         *f_event_file;
    oal_uint8             *puc_file_path;

    if (OAL_PTR_NULL == pc_file_path)
	{
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (ul_length > OAM_FILE_PATH_LENGTH)
    {
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    puc_file_path = DmtStub_GetDebugFilePath();
    oal_memcopy(g_st_oam_mng_ctx.ac_file_path, puc_file_path, strlen(puc_file_path));

    /* ���²�����Ϊ�˽���һ�ε���־�ļ���� */
    f_event_file = oal_file_open(g_st_oam_mng_ctx.ac_file_path, (OAL_O_CREAT | OAL_O_RDWR), 0);

    if (OAL_FILE_FAIL == f_event_file)
    {
        return OAL_ERR_CODE_WRITE_FILE_FAIL;
    }

    if (0 != oal_file_close(f_event_file))
    {
        return OAL_ERR_CODE_CLOSE_FILE_FAIL;
    }

#else

    if (OAL_PTR_NULL == pc_file_path)
	{
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (ul_length > OAM_FILE_PATH_LENGTH)
    {
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    oal_memcopy(g_st_oam_mng_ctx.ac_file_path, pc_file_path, ul_length);

#endif

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_dump_buff_by_hex
 ��������  : buff��ʮ������ʽ�̶����ȴ�ӡ�����ÿ���l_num������
 �������  : oal_uint8 *puc_buff
             oal_int32 l_len
             oal_int32 l_num
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��10��,����һ
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void  oam_dump_buff_by_hex(oal_uint8 *puc_buff, oal_int32 l_len, oal_int32 l_num)
{
    oal_int32       l_loop;

    for (l_loop = 0; l_loop < l_len; l_loop++)
    {
        OAL_IO_PRINT("%02x ", puc_buff[l_loop]);

        if (0 == (l_loop + 1) % l_num)
        {
            OAL_IO_PRINT("\n");
        }
    }

    OAL_IO_PRINT("\n");
}

/*****************************************************************************
 �� �� ��  : oam_drv_func_hook_init
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_void oam_drv_func_hook_init(oal_void)
{
    /* sdt����⹳�Ӻ�����ʼ�� */
    g_st_oam_sdt_func_hook.p_sdt_report_data_func       = OAL_PTR_NULL;
    g_st_oam_sdt_func_hook.p_sdt_get_wq_len_func        = OAL_PTR_NULL;

    /* wal����⹳�Ӻ�����ʼ�� */
    g_st_oam_wal_func_hook.p_wal_recv_cfg_data_func     = OAL_PTR_NULL;
    g_st_oam_wal_func_hook.p_wal_recv_mem_data_func     = OAL_PTR_NULL;
    g_st_oam_wal_func_hook.p_wal_recv_reg_data_func     = OAL_PTR_NULL;
    g_st_oam_wal_func_hook.p_wal_recv_global_var_func   = OAL_PTR_NULL;
}

/*****************************************************************************
 �� �� ��  : oam_sdt_func_fook_register
 ��������  : oamģ��ע��sdtģ��Ĺ��Ӻ���,������ģ��ͳһ����
 �������  : oam_sdt_func_hook_stru *pfun_st_oam_sdt_hook
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��12��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void oam_sdt_func_fook_register(oam_sdt_func_hook_stru *pfun_st_oam_sdt_hook)
{
    g_st_oam_sdt_func_hook.p_sdt_report_data_func = pfun_st_oam_sdt_hook->p_sdt_report_data_func;
    g_st_oam_sdt_func_hook.p_sdt_get_wq_len_func  = pfun_st_oam_sdt_hook->p_sdt_get_wq_len_func;
}

/*****************************************************************************
 �� �� ��  : oam_wal_func_fook_register
 ��������  : oamģ��ע��walģ��Ĺ��Ӻ���,������ģ��ͳһ����
 �������  : oam_sdt_func_hook_stru *pfun_st_oam_sdt_hook
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��12��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void oam_wal_func_fook_register(oam_wal_func_hook_stru *pfun_st_oam_wal_hook)
{
    g_st_oam_wal_func_hook.p_wal_recv_cfg_data_func     = pfun_st_oam_wal_hook->p_wal_recv_cfg_data_func;
    g_st_oam_wal_func_hook.p_wal_recv_mem_data_func     = pfun_st_oam_wal_hook->p_wal_recv_mem_data_func;
    g_st_oam_wal_func_hook.p_wal_recv_reg_data_func     = pfun_st_oam_wal_hook->p_wal_recv_reg_data_func;
    g_st_oam_wal_func_hook.p_wal_recv_global_var_func   = pfun_st_oam_wal_hook->p_wal_recv_global_var_func;
}

/*****************************************************************************
 �� �� ��  : oam_filter_data2sdt
 ��������  : sdt��Ϣ����Ƿ���Ҫ����
 �������  : oam_data_type_enum_uint8 en_type
 �������  : ��
 �� �� ֵ  : oal_uint32 0 ������ 1����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��26��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  oam_filter_data2sdt(oam_data_type_enum_uint8 en_type)
{
    if (g_st_sdt_stat_info.ul_wq_len < WLAN_SDT_MSG_FLT_HIGH_THD)
    {
        g_st_sdt_stat_info.en_filter_switch = OAL_FALSE;
        return OAM_FLT_PASS;
    }
    else if ((g_st_sdt_stat_info.ul_wq_len >= WLAN_SDT_MSG_FLT_HIGH_THD)
              && (g_st_sdt_stat_info.ul_wq_len < WLAN_SDT_MSG_QUEUE_MAX_LEN))
    {
        /* ��Ϣ���дﵽ�������ޣ����˷���־��Ϣ */
        g_st_sdt_stat_info.en_filter_switch = OAL_TRUE;
        return ((oal_uint8)OAM_DATA_TYPE_LOG == en_type) ? OAM_FLT_PASS : OAM_FLT_DROP;
    }

    /* ��Ϣ������ȫ������ */
    return OAM_FLT_DROP;
}

/*****************************************************************************
 �� �� ��  : oam_alloc_data2sdt
 ��������  : ��װ����sdt app���netlink��Ϣ
             1) ul_data_len��oal_mem_sdt_netbuf_alloc ����ĳ��Ⱥ�����SDTͷ���ɽӿ�����
             2) ����õ�netbufֱ��������ָ�볤�ȼ���

             SDT��NLK��Ϣͷ�ڱ��ӿ��в��ÿ��ǣ����ϱ��ӿ�ͳһ��д

 �������  : oal_uint32  ul_data_len
 �������  : ��
 �� �� ֵ  : oal_netbuf_stru *
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��28��,����һ
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru *oam_alloc_data2sdt(oal_uint16  us_data_len)
{
    oal_netbuf_stru    *pst_netbuf = OAL_PTR_NULL;
#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    pst_netbuf = oal_mem_sdt_netbuf_alloc(us_data_len+WLAN_SDT_SKB_RESERVE_LEN, OAL_TRUE);
    if (OAL_PTR_NULL == pst_netbuf)
    {
        return OAL_PTR_NULL;
    }

    oal_netbuf_reserve(pst_netbuf, WLAN_SDT_SKB_HEADROOM_LEN);

    oal_netbuf_put(pst_netbuf, us_data_len);
#endif
    return pst_netbuf;
}

/*****************************************************************************
 �� �� ��  : oam_report_data2sdt
 ��������  : oam����Ϣ�·���sdt�࣬��sdt��ͳһ�ϱ���app��
 �������  : oal_netbuf_stru *pst_netbuf
             oam_data_type_enum_uint8 en_type
             oam_primid_type_enum_uint8 en_prim
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��11��,���ڶ�
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 oam_report_data2sdt(oal_netbuf_stru *pst_netbuf,
                               oam_data_type_enum_uint8 en_type,
                               oam_primid_type_enum_uint8 en_prim)
{
    /* �ж�sdt������Ϣ�����Ƿ�������������������� */
    if (OAL_LIKELY(OAL_PTR_NULL != g_st_oam_sdt_func_hook.p_sdt_get_wq_len_func))
    {
        g_st_sdt_stat_info.ul_wq_len = (oal_uint32)g_st_oam_sdt_func_hook.p_sdt_get_wq_len_func();
    }

    if (OAM_FLT_PASS != oam_filter_data2sdt(en_type))
    {
        OAM_SDT_STAT_INCR(ul_filter_cnt);
        oal_mem_sdt_netbuf_free(pst_netbuf, OAL_TRUE);
        //oal_netbuf_free(pst_netbuf);

        /* , ��SDT����̫�������ϱ�ʧ��ʱ�򣬼���SDT��ӡ 0x778d WARNING��������ÿ��1000����ӡһ��*/
        /* Note: Ŀǰ�ϲ㺯������ʹ�ø÷���ֵ��ӡwarning��Ϣ����*/
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
        if(0 != ((g_st_sdt_stat_info.ul_filter_cnt) % OAM_DATA2SDT_FAIL_COUNT))
        {
            return OAL_SUCC;
        }
#endif
        return OAL_FAIL;
    }

    if (OAL_UNLIKELY(OAL_PTR_NULL == g_st_oam_sdt_func_hook.p_sdt_report_data_func))
    {
        /*should free netbuf here! by */
        OAL_IO_PRINT("oam_report_data2sdt p_sdt_report_data_func is NULL. \n");
        return OAL_FAIL;
    }

    g_st_oam_sdt_func_hook.p_sdt_report_data_func(pst_netbuf, en_type, en_prim);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_sdt_func_fook_unregister
 ��������  : �ṩ��SDT����ģ���ȥע�ắ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��8��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void oam_sdt_func_fook_unregister(oal_void)
{
    /* ����ָ�븳ֵ */
    g_st_oam_sdt_func_hook.p_sdt_report_data_func           = OAL_PTR_NULL;
    g_st_oam_sdt_func_hook.p_sdt_get_wq_len_func            = OAL_PTR_NULL;
}

/*****************************************************************************
 �� �� ��  : oam_wal_func_fook_unregister
 ��������  : wal���⹳�Ӻ���ȥע��
 �������  : oal_void
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��3��13��,������
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void oam_wal_func_fook_unregister(oal_void)
{
    /* ����ָ�븳ֵ */
    g_st_oam_wal_func_hook.p_wal_recv_cfg_data_func         = OAL_PTR_NULL;
    g_st_oam_wal_func_hook.p_wal_recv_global_var_func       = OAL_PTR_NULL;
    g_st_oam_wal_func_hook.p_wal_recv_mem_data_func         = OAL_PTR_NULL;
    g_st_oam_wal_func_hook.p_wal_recv_reg_data_func         = OAL_PTR_NULL;
}

#if ((_PRE_OS_VERSION_WIN32 != _PRE_OS_VERSION))
extern sdt_drv_mng_stru           g_st_sdt_drv_mng_entry;
oal_void  oam_hisi_kill(void)
{
    struct task_struct *oamhisi_task = NULL;
    oamhisi_task = pid_task(find_vpid(g_st_sdt_drv_mng_entry.ul_usepid), PIDTYPE_PID);
    if (oamhisi_task)
    {
        force_sig(SIGKILL, oamhisi_task);
    }
    else
    {
        OAL_IO_PRINT("[oam_hisi_kill]do not find oam hisi process\n");
    }
}
#else
oal_void  oam_hisi_kill(void)
{
}
#endif

/*****************************************************************************
 �� �� ��  : oam_main_init
 ��������  : OAMģ���ʼ������ڣ�����OAMģ���ڲ��������Եĳ�ʼ����
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  oam_main_init(oal_void)
{
    oal_uint32 ul_rslt;

    /* ��ʼ����ά�ɲ���FILE·�� */
    ul_rslt = oam_set_file_path(WLAN_OAM_FILE_PATH, (OAL_STRLEN(WLAN_OAM_FILE_PATH) + 1));
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_set_file_path fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

    /* ��ʼ����ά�ɲ������ʽ */
    ul_rslt = oam_set_output_type(OAM_OUTPUT_TYPE_SDT);
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_set_output_type fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

    /* ���LOGģ��ĳ�ʼ������ */
    ul_rslt = oam_log_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_log_init fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

    /* ���EVENTģ��ĳ�ʼ������ */
    ul_rslt = oam_event_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_event_init fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

    /* ��ʼ��5115timer�����ڴ����л�ȡ�߾���ʱ��� */
    oal_5115timer_init();

#ifdef _PRE_PROFILING_MODE
    /* ���PROFILINGģ��ĳ�ʼ������ */
    ul_rslt = oam_profiling_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_profiling_init fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }
#endif

    /* ��ʼ��oamģ��Ĺ��Ӻ��� */
    oam_drv_func_hook_init();

    /* ͳ��ģ���ʼ�� */
    oam_statistics_init();

/*TBD������������ƻ���ʼ����*/
#if 0
    /* �������ļ��л�ȡ������Ϣ�����浽OAM�ڲ��ṹ�� */
    for (ul_counter= 0; ul_counter < OM_MODULEID_BUTT; ul_counter++)
    {
        if (OAL_PTR_NULL != g_oam_customize.customize_init[ul_counter])
        {
            g_oam_customize.customize_init[ul_counter]();
        }
    }
#endif

#ifdef _PRE_WLAN_DFT_REG
    oam_reg_init();
#endif

#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
        /* ??netlink */
 #if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    /* ���õĶ˿ں���ϵͳ���ѱ�ռ�ã�02�޷�ʹ������ӿڣ���Ҫ�޸� TBD */
        ul_rslt = oam_netlink_kernel_create();
        if (OAL_SUCC != ul_rslt)
        {
            OAL_IO_PRINT("oam_main_init call oam_netlink_kernel_create fail %d\n",ul_rslt);
            OAL_BUG_ON(1);
            return -OAL_EFAIL;
        }
 #endif
#endif
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
    hcc_rx_register(hcc_get_default_handler(),HCC_ACTION_TYPE_OAM,oam_rx_post_action_function,NULL);
#endif
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_main_exit
 ��������  : OAMģ��ж��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ģ��ж�ط���ֵ���ɹ���ʧ��ԭ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void  oam_main_exit(oal_void)
{

    /* ��ʼ��5115timer�����ڴ����л�ȡ�߾���ʱ��� */
    oal_5115timer_exit();

#ifdef _PRE_WLAN_DFT_REG
    oam_reg_exit();
#endif
    /* ȥע��ȫ�ֱ�����д�ܽӿ� */

#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    #if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
       /* ���õĶ˿ں���ϵͳ���ѱ�ռ�ã�02�޷�ʹ������ӿڣ���Ҫ�޸� TBD */

        oam_netlink_kernel_release();
    #endif
#endif

    /* ж�سɹ��������ӡ */
    OAL_IO_PRINT("oam exit ok!\n");
    return ;
}

/*lint -e578*//*lint -e19*/
#if (_PRE_PRODUCT_ID_HI1151==_PRE_PRODUCT_ID)
oal_module_init(oam_main_init);
oal_module_exit(oam_main_exit);
#endif
/*lint -e19*/
oal_module_symbol(oam_hisi_kill);
oal_module_symbol(oam_main_init);
oal_module_symbol(oam_main_exit);
oal_module_symbol(oam_send_device_data2sdt);
oal_module_symbol(oam_set_file_path);
oal_module_symbol(oam_set_output_type);
oal_module_symbol(oam_get_output_type);
oal_module_symbol(oam_print);
oal_module_symbol(g_st_oam_mng_ctx);
oal_module_symbol(oam_dump_buff_by_hex);
oal_module_symbol(g_st_oam_sdt_func_hook);
oal_module_symbol(g_st_oam_wal_func_hook);
oal_module_symbol(oam_sdt_func_fook_register);
oal_module_symbol(oam_sdt_func_fook_unregister);
oal_module_symbol(oam_wal_func_fook_register);
oal_module_symbol(oam_wal_func_fook_unregister);
oal_module_symbol(oam_report_data2sdt);
oal_module_symbol(g_st_sdt_stat_info);
oal_module_symbol(oam_alloc_data2sdt);
oal_module_symbol(gst_oam_feature_list);
oal_module_symbol(g_auc_bcast_addr);

#ifdef _PRE_DEBUG_MODE
oal_module_symbol(g_aul_debug_feature_switch);
#endif

oal_module_license("GPL");

#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oam_main.h"
#include "oam_log.h"
#include "oam_event.h"
//#include "oam_alarm.h"
#include "oam_trace.h"
#include "plat_firmware.h"
//#include "oam_statistics.h"
#if (_PRE_PRODUCT_ID !=_PRE_PRODUCT_ID_HI1131C_DEV)
#include "oam_config.h"
#endif
//#include "oam_linux_netlink.h"
#include "oam_ext_if.h"

#include "oal_hcc_host_if.h"

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "los_event.h"
#include "los_task.h"
#endif
#include "oal_thread.h"

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
//��־������ʱû��
/* OAMģ��ͳһʹ�õ�ȫ�ֲ������������ģ�����OAM������ģ��ȫ�������� */
//oam_mng_ctx_stru    g_st_oam_mng_ctx;

/*�¼����ƽṹ��*/
EVENT_CB_S oam_event;
oal_kthread_stru        *gst_oam_thread;
OAL_STATIC oal_int32 g_l_oam_bind_cpu = -1;

#define OAM_THREAD_POLICY             OAL_SCHED_FIFO
#define OAM_THREAD_PRIORITY       (11)
#define OAM_THREAD_STACKSIZE  (0x1000)

/* ��ӡ���ͺ������� */
OAL_STATIC oal_print_func g_pa_oam_print_type_func[OAM_OUTPUT_TYPE_BUTT]=
{
    oam_print_to_console,   /* OAM_OUTPUT_TYPE_CONSOLE ����̨��� */
    oam_print_to_file,      /* OAM_OUTPUT_TYPE_FS д���ļ�ϵͳ */
    oam_print_to_sdt,       /* OAM_OUTPUT_TYPE_SDT �����SDT,�ϱ��ַ������˴���2048 */
};

oal_mutex_stru    g_oam_uart_print_mutex;
//oam_stat_info_stru  g_st_stat_info;
oal_wait_queue_head_stru     g_oam_wq;

/*����ȫ�ֱ���*/
OAM_LOG_LUNTI g_st_global ;
oam_rx_sdt_cmd_func_hook_stru g_oam_uart_rx_sdt_cmd_func_hook;

/* ���ں�SDT���߽�����ȫ�ֱ��� */
oam_sdt_func_hook_stru          g_st_oam_sdt_func_hook;
oam_wal_func_hook_stru          g_st_oam_wal_func_hook;
oam_sdt_stat_info_stru          g_st_sdt_stat_info;

oal_uint8 g_auc_bcast_addr[WLAN_MAC_ADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#ifdef _PRE_DEBUG_MODE  /* ��������Ĭ�Ͽ���״̬ */
oal_uint32          g_aul_debug_feature_switch[OAM_DEBUG_TYPE_BUTT] =
{
    OAL_SWITCH_OFF,   /* OAM_DEBUG_TYPE_ECHO_REG */
};
#endif

/* �����б� */
oam_software_feature_stru   gst_oam_feature_list[OAM_SOFTWARE_FEATURE_BUTT] =
{
    /*���Ժ�ID                  ��������д*/
    /* 0 */
    {OAM_SF_SCAN,               "scan"},
    {OAM_SF_AUTH,               "auth"},
    {OAM_SF_ASSOC,              "assoc"},
    {OAM_SF_FRAME_FILTER,       "ff"},
    {OAM_SF_WMM,                "wmm"},

    /* 5 */
    {OAM_SF_DFS,                "dfs"},
    {OAM_SF_NETWORK_MEASURE,    "nm"},
    {OAM_SF_ENTERPRISE_VO,      "ev"},
    {OAM_SF_HOTSPOTROAM,        "roam"},
    {OAM_SF_NETWROK_ANNOUNCE,   "11u"},

    /* 10 */
    {OAM_SF_NETWORK_MGMT,       "11k"},
    {OAM_SF_NETWORK_PWS,        "pws"},
    {OAM_SF_PROXYARP,           "proxyarp"},
    {OAM_SF_TDLS,               "tdls"},
    {OAM_SF_CALIBRATE,          "cali"},

    /* 15 */
    {OAM_SF_EQUIP_TEST,         "equip"},
    {OAM_SF_CRYPTO,             "crypto"},
    {OAM_SF_WPA,                "wpa"},
    {OAM_SF_WEP,                "wep"},
    {OAM_SF_WPS,                "wps"},

    /* 20 */
    {OAM_SF_PMF,                "pmf"},
    {OAM_SF_WAPI,               "wapi"},
    {OAM_SF_BA,                 "ba"},
    {OAM_SF_AMPDU,              "ampdu"},
    {OAM_SF_AMSDU,              "amsdu"},

    /* 25 */
    {OAM_SF_STABILITY,          "dfr"},
    {OAM_SF_TCP_OPT,            "tcp"},
    {OAM_SF_ACS,                "acs"},
    {OAM_SF_AUTORATE,           "autorate"},
    {OAM_SF_TXBF,               "txbf"},

    /* 30 */
    {OAM_SF_DYN_RECV,           "weak"},
    {OAM_SF_VIVO,               "vivo"},
    {OAM_SF_MULTI_USER,         "muser"},
    {OAM_SF_MULTI_TRAFFIC,      "mtraff"},
    {OAM_SF_ANTI_INTF,          "anti_intf"},

    /* 35 */
    {OAM_SF_EDCA,               "edca"},
    {OAM_SF_SMART_ANTENNA,      "ani"},
    {OAM_SF_TPC,                "tpc"},
    {OAM_SF_TX_CHAIN,           "txchain"},
    {OAM_SF_RSSI,               "rssi"},

    /* 40 */
    {OAM_SF_WOW,                "wow"},
    {OAM_SF_GREEN_AP,           "green"},
    {OAM_SF_PWR,                "pwr"},
    {OAM_SF_SMPS,               "smps"},
    {OAM_SF_TXOP,               "txop"},

    /* 45 */
    {OAM_SF_WIFI_BEACON,        "beacon"},
    {OAM_SF_KA_AP,              "alive"},
    {OAM_SF_MULTI_VAP,          "mvap"},
    {OAM_SF_2040,               "2040"},
    {OAM_SF_DBAC,               "dbac"},

    /* 50 */
    {OAM_SF_PROXYSTA,           "proxysta"},
    {OAM_SF_UM,                 "um"},
    {OAM_SF_P2P,                "p2p"},
    {OAM_SF_M2U,                "m2u"},
    {OAM_SF_IRQ,                "irq"},

    /* 55 */
    {OAM_SF_TX,                 "tx"},
    {OAM_SF_RX,                 "rx"},
    {OAM_SF_DUG_COEX,           "dugcoex"},
    {OAM_SF_CFG,                "cfg"},
    {OAM_SF_FRW,                "frw"},

    /* 60 */
    {OAM_SF_KEEPALIVE,          "keepalive"},
    {OAM_SF_COEX,               "coex"},
    {OAM_SF_HS20,               "hs20"},
    {OAM_SF_MWO_DET,            "mwodet"},
    {OAM_SF_CCA_OPT,            "cca"},

    {OAM_SF_ROAM,               "roam"},
    {OAM_SF_DFT,                "dft"},
    {OAM_SF_DFR,                "dfr"},
    {OAM_SF_BACKUP,             "backup"},
    {OAM_SF_ANY,                "any"},
};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : oam_print
 ��������  : OAMģ���ṩ���������
 �������  : ��ʽ�����ַ���
 �������  : ��
 �� �� ֵ  : �ɹ�����ʧ��ԭ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/

extern void oam_printf_wpa(int level, char* pc_fmt, ...);
oal_uint32  oam_print(oal_int8 *pc_string)
{
#if 0
    oam_output_type_enum_uint8 en_output_type;
    oal_uint32                 ul_rslt;

    ul_rslt = oam_get_output_type(&en_output_type);
    if (OAL_SUCC != ul_rslt)
    {
        return ul_rslt;
    }

    ul_rslt = g_pa_oam_print_type_func[en_output_type](pc_string);
    if (OAL_SUCC != ul_rslt)
    {
        return ul_rslt;
    }

#endif
    oam_printf_wpa(MSG_WARNING, pc_string);
    return OAL_SUCC;
}


#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
oal_int32 oam_rx_post_action_function(oal_uint8 stype,
                                             hcc_netbuf_stru* pst_hcc_netbuf, oal_uint8 *pst_context)
{
    oal_uint8   *puc_data;
    OAL_REFERENCE(pst_context);
    OAL_BUG_ON(NULL == pst_hcc_netbuf);

    puc_data = oal_netbuf_data(pst_hcc_netbuf->pst_netbuf);
#if 0
    if((DUMP_REG == stype) || (DUMP_MEM == stype))
    {
       //AL_IO_PRINT("receive dump msg  = %d\n", stype);
        exception_bcpu_dump_recv(puc_data,pst_hcc_netbuf->pst_netbuf);
        oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
        return OAL_SUCC;
    }
#endif

    oam_send_device_data2sdt(puc_data, pst_hcc_netbuf->len);

    oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
    return OAL_SUCC;
}
#endif


int rename_old_file(OAM_LOG_LUNTI * pst_log)
{
    int l_curr_num;
    int l_loop;
    char *ac_old_name = NULL;

    char *ac_new_name = NULL;

    ac_old_name=malloc(LOG_OAM_PATH_LEN + LOG_NAME_LEN + 1);
    memset(ac_old_name, 0, (LOG_OAM_PATH_LEN + LOG_NAME_LEN + 1));
    ac_new_name=malloc(LOG_OAM_PATH_LEN + LOG_NAME_LEN + 1);
    memset(ac_new_name, 0, (LOG_OAM_PATH_LEN + LOG_NAME_LEN + 1));
    char *pc_directory;
    char *pc_prefix;

    if (NULL == pst_log)
    {
        return -EFAIL;
    }
    pc_directory = pst_log->st_path;
    pc_prefix	 = pst_log->ac_prefix;

	l_curr_num = pst_log->l_curr_num;
	if (l_curr_num >= pst_log->st_num)
	{
		l_curr_num = pst_log->st_num - 1;
	}
	pst_log->l_curr_num = l_curr_num;

    for (l_loop = l_curr_num; l_loop >= 1; l_loop--)
    {
        snprintf(ac_new_name, LOG_OAM_PATH_LEN + LOG_NAME_LEN, "%s%s%d", pc_directory, pc_prefix, l_loop + 1);
        snprintf(ac_old_name, LOG_OAM_PATH_LEN + LOG_NAME_LEN, "%s%s%d", pc_directory, pc_prefix, l_loop);
        if(rename(ac_old_name, ac_new_name) < 0)
        {
            //OAM_DEBUG("rename ret < 0");
        }
    }

    free(ac_old_name);
    free(ac_new_name);
    ac_old_name = NULL;
    ac_new_name = NULL;

    return SUCC;
}


int check_file_size(OAM_LOG_LUNTI * pst_log)
{
    FILE* fd = NULL;
    long  size = 0;

    if (NULL == pst_log)
    {
        return -EFAIL;
    }

    fd = pst_log->pst_fd;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    if (pst_log->st_size > size)
    {
        return SUCC;
    }
    fclose(fd);

    rename_old_file(pst_log);
    pst_log->pst_fd = fopen(pst_log->ac_file, "a");//ÿ�δ����ļ����Ǵ�wifi_log_1�����
    if(NULL == pst_log->pst_fd)
    {
        return -EFAIL;
    }
    pst_log->l_curr_num++;
    return SUCC;
}

int oam_get_log_file(OAM_LOG_LUNTI * pst_log)
{
    long  l_size = 0;
    int   l_cnt = 0;
    oal_int8  ac_tmp[LOG_OAM_PATH_LEN + 1] = {0};

    pst_log->pst_fd = fopen(pst_log->ac_file, "a");
    while(NULL == pst_log->pst_fd)
    {
        if (l_cnt > 1)
        {
            return -EFAIL;
        }

        //snprintf(ac_tmp, LOG_OAM_PATH_LEN, "mkdir -p %s", pst_log->st_path);
        //system(ac_tmp);//system ����liteos����Ч����������־��������Ŀ¼��
        pst_log->pst_fd = fopen(pst_log->ac_file, "a");
        l_cnt++;
    }
	fseek(pst_log->pst_fd, 0, SEEK_END);
	l_size = ftell(pst_log->pst_fd);
	if (pst_log->st_size < l_size)
	{
	    if (SUCC != check_file_size(pst_log))
    	    {
            return -EFAIL;
    	    }
	}
	return SUCC;
}


#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

extern oal_uint32    g_uloam_test_cnt;
extern oal_uint32    g_uloam_test_step;
extern oal_uint32    g_uloam_skbuff_list_len;
extern EVENT_CB_S oam_event;
OAL_STATIC oal_int32 oam_task_thread(oal_void* ul_bind_cpu)
{
    oal_int32       ret = 0;
    oal_netbuf_stru  *pst_netbuf;
    oal_allow_signal(SIGTERM);
    #ifdef  _PRE_FRW_EVENT_PROCESS_TRACE_DEBUG
        frw_event_last_pc_trace(__FUNCTION__,__LINE__, (oal_uint32)(oal_ulong)ul_bind_cpu);
    #endif
    for (;;)
    {
        if (oal_kthread_should_stop())
        {
            break;
        }
        /*��д�¼�������pend״̬תready��������ָ�pend״̬����������wifiҵ���߳���Դ*/
        LOS_EventRead(&oam_event, oam_event_wait_bit0, OS_WAITMODE_OR|OS_WAITMODE_CLR, OS_WAIT_FOREVER);
        g_uloam_test_step++;
        pst_netbuf = sdt_drv_netbuf_delist();
        while (NULL != pst_netbuf)
        {
            g_uloam_test_cnt++;
            oam_dev_host_uart_print(OAL_NETBUF_DATA(pst_netbuf), OAL_NETBUF_LEN(pst_netbuf));
            oal_netbuf_free(pst_netbuf);
            pst_netbuf = sdt_drv_netbuf_delist();
        }
#ifdef  _PRE_FRW_EVENT_PROCESS_TRACE_DEBUG
        frw_event_last_pc_trace(__FUNCTION__,__LINE__, (oal_uint32)(oal_ulong)ul_bind_cpu);
#endif
    }
    return SUCC;
}

#endif


oal_uint32  oam_task_init(oal_void)
{
    oal_kthread_param_stru st_thread_param = {0};

    st_thread_param.l_cpuid      = NOT_BIND_CPU;
    st_thread_param.l_policy     = OAM_THREAD_POLICY;
    st_thread_param.l_prio       = OAM_THREAD_PRIORITY;
    st_thread_param.ul_stacksize = OAM_THREAD_STACKSIZE;

    LOS_EventInit(&oam_event);

    gst_oam_thread = oal_kthread_create("hisi_oam_task",oam_task_thread, (void *)g_l_oam_bind_cpu,&st_thread_param);
    if (NULL == gst_oam_thread)
    {
        OAL_IO_PRINT("[OAM][ERROR]failed to create oamthread\n");
        return -OAL_EFAIL;
    }

    //OAL_WAIT_QUEUE_INIT_HEAD(&g_oam_wq);    /*queues init*/

    return OAL_SUCC;
}


int oam_init_roll_log(OAM_LOG_LUNTI * pst_log, int pst_size, int pst_num, char *pc_dir_path, char* pc_prefix)
{
    size_t len = 0;
    int ret = -EFAIL;
    if (NULL == pst_log || 0 == pst_size || 0 == pst_num
      || NULL == pc_dir_path || NULL == pc_prefix)
    {
        return -EFAIL;
    }


    pst_log->st_size = pst_size;
    pst_log->st_size *= 20;//��byteΪ��λ
    pst_log->st_num = pst_num;

    len = strlen(pc_dir_path);
    strncpy(pst_log->st_path, pc_dir_path, len);
    len = 0;

    len = strlen(pc_prefix);
    strncpy(pst_log->ac_prefix, pc_prefix, len);
    len = 0;

    /*ÿ�γ�ʼ�����Ǵ�������־���1��ʼ����02 oam_hisi app����һ��*/
    pst_log->l_curr_num = 1;
    snprintf(pst_log->ac_file, LOG_OAM_PATH_LEN + LOG_NAME_LEN, "%s%s%d", pst_log->st_path, pst_log->ac_prefix, 1);

    return SUCC;
}

typedef unsigned int    uint;

extern volatile oal_uint8            g_uc_uart_ctrl;
extern          oal_uint32           g_ul_file_count;
extern          oal_uint32           g_ul_cnt_statistic;
extern          FILE*                g_l_fopen_fd ;
void write_rx_cmd2file(char* ch_cmd, uint count)
{
    FILE* fd = NULL;
    int write_errno;
    g_ul_cnt_statistic++;
    fwrite(ch_cmd, count, 1, g_l_fopen_fd);
    if(g_ul_file_count <= g_ul_cnt_statistic)
    {
        g_uc_uart_ctrl = SDT_DUMP_CLOSE;
        g_ul_file_count = 0;
        g_ul_cnt_statistic = 0;
        fclose(g_l_fopen_fd);
        g_l_fopen_fd = NULL;
        //OAM_WARNING_LOG0_ACK(1, 1, "suc");
    }
    return;
}

void oam_uart_rx_sdt_cmd_init(void)
{
    g_oam_uart_rx_sdt_cmd_func_hook.uart_rx_sdt_cmd_dispose = write_rx_cmd2file;

}






/*****************************************************************************
 �� �� ��  : oam_main_init
 ��������  : OAMģ���ʼ������ڣ�����OAMģ���ڲ��������Եĳ�ʼ����
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
extern sdt_drv_mng_stru g_st_sdt_drv_mng_entry;

oal_int32  oam_main_init(oal_void)
{
    oal_uint32 ul_rslt;
#if 0
    /* ��ʼ����ά�ɲ���FILE·�� */
    ul_rslt = oam_set_file_path(WLAN_OAM_FILE_PATH, (OAL_STRLEN(WLAN_OAM_FILE_PATH) + 1));
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_set_file_path fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }


    /* ��ʼ����ά�ɲ������ʽ */
    ul_rslt = oam_set_output_type(OAM_OUTPUT_TYPE_SDT);
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_set_output_type fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }
#endif

    /* ���LOGģ��ĳ�ʼ������ */
    dprintf("enter oam_log_init\n");
    ul_rslt = oam_log_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_log_init fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

#if 0
    /* ���EVENTģ��ĳ�ʼ������ */
    ul_rslt = oam_event_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_event_init fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

    /* ��ʼ��5115timer�����ڴ����л�ȡ�߾���ʱ��� */
    oal_5115timer_init();

#ifdef _PRE_PROFILING_MODE
    /* ���PROFILINGģ��ĳ�ʼ������ */
    ul_rslt = oam_profiling_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oam_main_init call oam_profiling_init fail %d\n",ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }
#endif
#endif
    /* ��ʼ��oamģ��Ĺ��Ӻ��� */
//    oam_drv_func_hook_init();

    /* ͳ��ģ���ʼ�� */
    //oam_statistics_init();

/*TBD������������ƻ���ʼ����*/
#if 0
    /* �������ļ��л�ȡ������Ϣ�����浽OAM�ڲ��ṹ�� */
    for (ul_counter= 0; ul_counter < OM_MODULEID_BUTT; ul_counter++)
    {
        if (OAL_PTR_NULL != g_oam_customize.customize_init[ul_counter])
        {
            g_oam_customize.customize_init[ul_counter]();
        }
    }
#endif

#ifdef _PRE_WLAN_DFT_REG
    oam_reg_init();
#endif
    mutex_init(&g_oam_uart_print_mutex);
    mutex_init(&g_st_sdt_drv_mng_entry.st_spin_lock);
    oal_netbuf_list_head_init(&g_st_sdt_drv_mng_entry.rx_wifi_dbg_seq);
    hcc_rx_register(hcc_get_default_handler(), HCC_ACTION_TYPE_OAM, oam_rx_post_action_function, NULL);
    oam_task_init();
    #if 0 //������־����
    oam_init_roll_log(&g_st_global, 10, 10, WIFI_OAM_PATH, WIFI_OAM_PREFIX);
    #endif
    /*��pc��dump�ļ���liteos��������ʼ��*/
    oam_uart_rx_sdt_cmd_init();
    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oam_main_exit
 ��������  : OAMģ��ж��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ģ��ж�ط���ֵ���ɹ���ʧ��ԭ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void  oam_main_exit(oal_void)
{

    /* ��ʼ��5115timer�����ڴ����л�ȡ�߾���ʱ��� */
//    oal_5115timer_exit();

#ifdef _PRE_WLAN_DFT_REG
    oam_reg_exit();
#endif
    /* ȥע��ȫ�ֱ�����д�ܽӿ� */

    /* ж�سɹ��������ӡ */
#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    #if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
       /* ���õĶ˿ں���ϵͳ���ѱ�ռ�ã�02�޷�ʹ������ӿڣ���Ҫ�޸� TBD */

        oam_netlink_kernel_release();
    #endif
#endif

    return ;
}



/*lint -e578*//*lint -e19*/
#if (_PRE_PRODUCT_ID_HI1151==_PRE_PRODUCT_ID)
oal_module_init(oam_main_init);
oal_module_exit(oam_main_exit);
#endif
/*lint -e19*/

oal_module_symbol(oam_main_init);
oal_module_symbol(oam_main_exit);
oal_module_symbol(oam_send_device_data2sdt);
oal_module_symbol(oam_set_file_path);
oal_module_symbol(oam_set_output_type);
oal_module_symbol(oam_get_output_type);
oal_module_symbol(oam_print);
oal_module_symbol(g_st_oam_mng_ctx);
oal_module_symbol(oam_dump_buff_by_hex);
oal_module_symbol(g_st_oam_sdt_func_hook);
oal_module_symbol(g_st_oam_wal_func_hook);
oal_module_symbol(oam_sdt_func_fook_register);
oal_module_symbol(oam_sdt_func_fook_unregister);
oal_module_symbol(oam_wal_func_fook_register);
oal_module_symbol(oam_wal_func_fook_unregister);
oal_module_symbol(oam_report_data2sdt);
oal_module_symbol(g_st_sdt_stat_info);
oal_module_symbol(oam_alloc_data2sdt);
oal_module_symbol(gst_oam_feature_list);
oal_module_symbol(g_auc_bcast_addr);

#ifdef _PRE_DEBUG_MODE
oal_module_symbol(g_aul_debug_feature_switch);
#endif

oal_module_license("GPL");

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

