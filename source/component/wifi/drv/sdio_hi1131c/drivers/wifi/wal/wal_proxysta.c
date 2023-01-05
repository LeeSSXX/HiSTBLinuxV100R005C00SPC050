/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : wal_proxysta.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2014��4��21��
  ����޸�   :
  ��������   : Proxy STA ����������غ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��4��21��
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
#include "oam_ext_if.h"
#include "frw_ext_if.h"
#include "wal_linux_bridge.h"
#include "mac_device.h"
#include "hmac_vap.h"
#include "mac_vap.h"
#include "wal_main.h"
#include "wal_proxysta.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_PROXYSTA_C

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
/*****************************************************************************
*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_pre_route(oal_uint32 ul_hooknum,
                                                     oal_netbuf_stru *pst_netbuf,
                                                     OAL_CONST oal_net_device_stru *pst_in,
                                                     OAL_CONST oal_net_device_stru *pst_out,
                                                     oal_int32 (*p_okfn_func)(oal_netbuf_stru *));

OAL_STATIC oal_uint32  wal_proxysta_handle_forward(oal_uint32 ul_hooknum,
                                                   oal_netbuf_stru *pst_netbuf,
                                                   OAL_CONST oal_net_device_stru *pst_in,
                                                   OAL_CONST oal_net_device_stru *pst_out,
                                                   oal_int32 (*p_okfn_func)(oal_netbuf_stru *));

OAL_STATIC oal_uint32  wal_proxysta_handle_post_route(oal_uint32 ul_hooknum,
                                                      oal_netbuf_stru *pst_netbuf,
                                                      OAL_CONST oal_net_device_stru *pst_in,
                                                      OAL_CONST oal_net_device_stru *pst_out,
                                                      oal_int32 (*p_okfn_func)(oal_netbuf_stru *));

OAL_STATIC oal_uint32  wal_proxysta_handle_local_in(oal_uint32 ul_hooknum,
                                                    oal_netbuf_stru *pst_netbuf,
                                                    OAL_CONST oal_net_device_stru *pst_in,
                                                    OAL_CONST oal_net_device_stru *pst_out,
                                                    oal_int32 (*p_okfn_func)(oal_netbuf_stru *));

/*****************************************************************************/
/*2.�궨��                                                                   */
/*****************************************************************************/
/* port types */
#define WAL_PROXYSTA_PTYPE_ETH      0x1000  /*4096 Port type ethernet*/
#define WAL_PROXYSTA_PTYPE_VAP      0x1001  /*4097 Port type VAP*/
#define WAL_PROXYSTA_PTYPE_PETH     0x1003  /*4099 Port type proxy ethernet*/
#define WAL_PROXYSTA_PTYPE_PVAP     0x1004  /*4100 type proxy VAP*/
/*  proxysta begin */
#define WAL_PROXYSTA_PTYPE_MPVAP	0x1005  /*4101 Port type main proxy VAP*/

/*  proxysta end */

/*Port flags*/
#define WAL_PROXYSTA_PTYPE_FLAG_ISO         1<<0    /*Port isolation enabled/disabled*/

#define WAL_PROXYSTA_PFLAG_SHIFT            16
#define WAL_PROXYSTA_PTYPE_MASK             ((1 << WAL_PROXYSTA_PFLAG_SHIFT) - 1)
#define WAL_PROXYSTA_PTYPE(_type)           (_type & WAL_PROXYSTA_PTYPE_MASK)
#define WAL_PROXYSTA_PFLAG(_type)           (_type >> WAL_PROXYSTA_PFLAG_SHIFT)

#define WAL_PROXYSTA_PFLAG_IS_ISO(_flag)    (_flag & WAL_PROXYSTA_PTYPE_FLAG_ISO)
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
OAL_STATIC oal_nf_hook_ops_stru proxysta_brhooks[] __read_mostly = {
    {
        .hook     = wal_proxysta_handle_pre_route,
        .owner    = OAL_THIS_MODULE,
        .pf       = OAL_PF_BRIDGE,
        .hooknum  = OAL_NF_BR_PRE_ROUTING,
        .priority = NF_BR_PRI_NAT_DST_BRIDGED
    },
    {
        .hook     = wal_proxysta_handle_forward,
        .owner    = OAL_THIS_MODULE,
        .pf       = OAL_PF_BRIDGE,
        .hooknum  = OAL_NF_BR_FORWARD,
        .priority = NF_BR_PRI_FILTER_BRIDGED
    },
    {
        .hook     = wal_proxysta_handle_post_route,
        .owner    = OAL_THIS_MODULE,
        .pf       = OAL_PF_BRIDGE,
        .hooknum  = OAL_NF_BR_POST_ROUTING,
        .priority = NF_BR_PRI_NAT_SRC
    },
    {
        .hook     = wal_proxysta_handle_local_in,
        .owner    = OAL_THIS_MODULE,
        .pf       = OAL_PF_BRIDGE,
        .hooknum  = OAL_NF_BR_LOCAL_IN,
        .priority = NF_BR_PRI_FILTER_BRIDGED
    },
};
#elif (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
OAL_STATIC oal_nf_hook_ops_stru proxysta_brhooks[] = {
    {
        wal_proxysta_handle_pre_route,
        OAL_THIS_MODULE,
        OAL_PF_BRIDGE,
        {0},
        OAL_NF_BR_PRE_ROUTING,
        NF_BR_PRI_NAT_DST_BRIDGED
    },
    {
        wal_proxysta_handle_forward,
        OAL_THIS_MODULE,
        OAL_PF_BRIDGE,
        {0},
        OAL_NF_BR_FORWARD,
        NF_BR_PRI_FILTER_BRIDGED
    },
    {
        wal_proxysta_handle_post_route,
        OAL_THIS_MODULE,
        OAL_PF_BRIDGE,
        {0},
        OAL_NF_BR_POST_ROUTING,
        NF_BR_PRI_NAT_SRC
    },
    {
        wal_proxysta_handle_local_in,
        OAL_THIS_MODULE,
        OAL_PF_BRIDGE,
        {0},
        OAL_NF_BR_LOCAL_IN,
        NF_BR_PRI_FILTER_BRIDGED
    },
};
#endif

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


/*****************************************************************************
 �� �� ��  : wal_proxysta_brroute_hook
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        /*TBD*/
#else

OAL_STATIC oal_int32 wal_proxysta_brroute_hook(oal_netbuf_stru *pst_netbuf)
{
	if (OAL_PROXYSTA_MARK_ROUTE == pst_netbuf->mark)
	{
		OAM_INFO_LOG0(0, OAM_SF_PROXYSTA, "{wal_proxysta_brroute_hook::Allowing local delivery.}");
		return OAL_TRUE; /* route it */
	}

	return OAL_FALSE; /* bridge it */
}
#endif
/*****************************************************************************
 �� �� ��  : wal_proxysta_dev_xmit
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_void  wal_proxysta_dev_xmit(oal_netbuf_stru *pst_netbuf)
{
    if (pst_netbuf->len > pst_netbuf->dev->mtu)
    {
        oal_netbuf_free(pst_netbuf);
    }
    else
    {
        oal_netbuf_push(pst_netbuf, ETHER_HDR_LEN);
        #ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
            wal_vap_start_xmit(pst_netbuf, pst_netbuf->dev);
        #else
            wal_bridge_vap_xmit(pst_netbuf, pst_netbuf->dev);
        #endif
    }
}

/*****************************************************************************
 �� �� ��  : wal_proxysta_find_net_dev
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
OAL_STATIC oal_uint32  wal_proxysta_find_net_dev(mac_device_proxysta_stru *pst_dev_psta,
                                                 oal_uint8                *puc_mac_addr,
                                                 oal_net_device_stru     **ppst_net_device)
{
    oal_uint8              uc_hash_value;
    mac_vap_stru          *pst_mav_vap;
    hmac_vap_stru         *pst_hmac_vap;
    oal_dlist_head_stru   *pst_entry;

    uc_hash_value = MAC_PROXYSTA_CALCULATE_HASH_VALUE(puc_mac_addr);

    OAL_DLIST_SEARCH_FOR_EACH(pst_entry, &(pst_dev_psta->ast_proxysta_hash[uc_hash_value]))
    {
        pst_mav_vap = OAL_DLIST_GET_ENTRY(pst_entry, mac_vap_stru, st_entry);

        if (0 == oal_compare_mac_addr(pst_mav_vap->st_vap_proxysta.auc_oma, puc_mac_addr))
        {
            pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mav_vap->uc_vap_id);
            if (OAL_PTR_NULL == pst_hmac_vap)
            {
                OAM_ERROR_LOG0(pst_mav_vap->uc_vap_id, OAM_SF_PROXYSTA, "{wal_proxysta_find_net_dev::pst_hmac_vap is null.}");
                return OAL_ERR_CODE_PTR_NULL;
            }

            if (OAL_PTR_NULL == pst_hmac_vap->pst_net_device)
            {
                return OAL_FAIL;
            }

            *ppst_net_device = pst_hmac_vap->pst_net_device;

            return OAL_SUCC;
        }
    }

    return OAL_FAIL;
}

/*****************************************************************************
 �� �� ��  : wal_proxysta_find_mainsta_net_dev
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��9��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_find_mainsta_net_dev(mac_device_proxysta_stru *pst_dev_psta,
                                                 oal_uint8                *puc_mac_addr,
                                                 oal_net_device_stru     **ppst_net_device)
{
    hmac_vap_stru         *pst_hmac_vap;

    /* ������ĵ�Դmac��ַ��main sta��mac��ַ��ȣ������mainsta�տڷ��� */
    if (0 == oal_compare_mac_addr(pst_dev_psta->auc_mac_addr, puc_mac_addr))
    {
        /* ��ȡmain sta��hmac vap */
        pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(1);
        if (OAL_PTR_NULL == pst_hmac_vap)
        {
            OAM_ERROR_LOG0(0, OAM_SF_PROXYSTA, "{wal_proxysta_find_net_dev::pst_hmac_vap is null.}");
            return OAL_ERR_CODE_PTR_NULL;
        }

        if (OAL_PTR_NULL == pst_hmac_vap->pst_net_device)
        {
            return OAL_FAIL;
        }

        *ppst_net_device = pst_hmac_vap->pst_net_device;

        return OAL_SUCC;
    }

    return OAL_FAIL;
}

/*****************************************************************************
 �� �� ��  : wal_proxysta_get_psta_dev
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_get_psta_dev(OAL_CONST oal_net_device_stru *pst_net_device, mac_device_proxysta_stru **ppst_dev_psta)
{
    mac_device_stru   *pst_mac_device;
    mac_vap_stru      *pst_mac_vap;

    pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_net_device);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    *ppst_dev_psta = &(pst_mac_device->st_device_proxysta);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_proxsysta_add_dev
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_proxysta_add_vap(frw_event_mem_stru *pst_event_mem)
{
    frw_event_stru              *pst_event;
    mac_cfg_set_oma_param_stru  *pst_mac_cfg_set_oma;
    oal_uint8                    auc_proxysta_oma[WLAN_MAC_ADDR_LEN];
    mac_device_stru             *pst_mac_device;
    oal_int32                    l_ret;
    oal_uint32                   uc_hash_value;
    mac_vap_stru                *pst_mac_vap;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_PROXYSTA, "{wal_proxysta_add_vap::pst_event_mem is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event           = (frw_event_stru *)pst_event_mem->puc_data;
    pst_mac_cfg_set_oma = (mac_cfg_set_oma_param_stru *)pst_event->auc_event_data;

    /* ��ȡproxysta��oma��ַ */
    oal_memcopy(auc_proxysta_oma, pst_mac_cfg_set_oma, WLAN_MAC_ADDR_LEN);

    /* ��ȡmac_device_stru */
    pst_mac_device = mac_res_get_dev(pst_event->st_event_hdr.uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG0(pst_event->st_event_hdr.uc_vap_id, OAM_SF_PROXYSTA,
                       "{wal_proxysta_add_vap::get mac_device ptr is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡmac_vap_stru */
    pst_mac_vap = mac_res_get_mac_vap(pst_event->st_event_hdr.uc_vap_id);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(pst_event->st_event_hdr.uc_vap_id, OAM_SF_PROXYSTA,
                       "{wal_proxysta_add_vap::get mac_vap ptr is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����ǵ�һ��Proxy STA�� ע�����Ź��Ӻ��� */
    if (0 == pst_mac_device->st_device_proxysta.uc_proxysta_cnt)
    {
        l_ret = oal_nf_register_hooks(proxysta_brhooks, OAL_SIZEOF(proxysta_brhooks)/OAL_SIZEOF(proxysta_brhooks[0]));

        if (l_ret < 0)
        {
            OAM_ERROR_LOG0(0, OAM_SF_PROXYSTA,
                       "{wal_proxysta_add_vap::Failed to register br hooks.}");
            return (oal_uint32)l_ret;
        }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* TBD  br_should_route_hook did't export symbol in new kernel.*/
#else
    /* register hook to intercept pkt to support local delivery */
    OAL_RCU_ASSIGN_POINTER(oal_br_should_route_hook, wal_proxysta_brroute_hook);
#endif
    }

    /* ���Proxy STA�� hash����*/
    uc_hash_value  = MAC_PROXYSTA_CALCULATE_HASH_VALUE(auc_proxysta_oma);
    oal_dlist_add_head(&(pst_mac_vap->st_entry), &(pst_mac_device->st_device_proxysta.ast_proxysta_hash[uc_hash_value]));
    pst_mac_device->st_device_proxysta.uc_proxysta_cnt++;

    return  OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_proxysta_remove_vap
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��24��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  wal_proxysta_remove_vap(mac_vap_stru *pst_mac_vap)
{
    oal_uint32                   uc_hash_value;
    oal_uint8                    auc_proxysta_oma[WLAN_MAC_ADDR_LEN];
    oal_dlist_head_stru         *pst_hash_head;
    oal_dlist_head_stru         *pst_entry;
    mac_device_stru             *pst_mac_device;
    mac_vap_stru                *pst_vap;

    /* ��ȡmac_device_stru */
    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                       "{wal_proxysta_remove_vap::get mac_device ptr is null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ��Ҫɾ����proxysta oma��ַ */
    oal_memcopy(auc_proxysta_oma, pst_mac_vap->st_vap_proxysta.auc_oma, WLAN_MAC_ADDR_LEN);

    uc_hash_value  = MAC_PROXYSTA_CALCULATE_HASH_VALUE(auc_proxysta_oma);
    pst_hash_head = &(pst_mac_device->st_device_proxysta.ast_proxysta_hash[uc_hash_value]);

    OAL_DLIST_SEARCH_FOR_EACH(pst_entry, pst_hash_head)
    {
        pst_vap = OAL_DLIST_GET_ENTRY(pst_entry, mac_vap_stru, st_entry);

        if (0 == oal_compare_mac_addr(pst_vap->st_vap_proxysta.auc_oma, auc_proxysta_oma))
        {
            oal_dlist_delete_entry(pst_entry);
            pst_mac_device->st_device_proxysta.uc_proxysta_cnt--;

            if (0 == pst_mac_device->st_device_proxysta.uc_proxysta_cnt)
            {
                oal_nf_unregister_hooks(proxysta_brhooks, OAL_SIZEOF(proxysta_brhooks)/OAL_SIZEOF(proxysta_brhooks[0]));
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA, "{wal_proxysta_remove_vap::Remov hooks.}");

        #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
                /*TBD*/
        #else
                /* reset the br route hook */
                OAL_RCU_ASSIGN_POINTER(oal_br_should_route_hook, NULL);
        #endif
            }
            return OAL_SUCC;
        }
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : wal_proxysta_handle_pre_route
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_pre_route(oal_uint32 ul_hooknum,
                                                     oal_netbuf_stru *pst_netbuf,
                                                     OAL_CONST oal_net_device_stru *pst_in,
                                                     OAL_CONST oal_net_device_stru *pst_out,
                                                     oal_int32 (*p_okfn_func)(oal_netbuf_stru *))
{
    oal_bool_enum_uint8         en_ismcast;
    mac_device_proxysta_stru   *pst_dev_psta;
    oal_net_device_stru        *pst_net_device;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        /* TBD  br_port is useless in new kernel.*/
        oal_uint32                  ul_type = ~0UL;
#else
        oal_uint32                  ul_type  = oal_br_get_port_type(pst_in->br_port);
#endif

    oal_uint32                  ul_ptype = WAL_PROXYSTA_PTYPE(ul_type);
    oal_netbuf_stru            *pst_netbuf_clone;
    oal_uint32                  ul_ret;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;

    OAM_INFO_LOG1(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_pre_route:ul_ptype:%d}",ul_ptype);

    en_ismcast = ETHER_IS_MULTICAST(oal_eth_hdr(pst_netbuf)->h_dest);

    ul_ret = wal_proxysta_get_psta_dev(pst_in, &pst_dev_psta);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG0(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_pre_route::get proxysta dev failed.}");
        return OAL_NF_DROP;
    }

    switch (ul_ptype)
    {
        case WAL_PROXYSTA_PTYPE_ETH:

        /* AP0���յ�����֡�Ĵ����������:
           1.AP0�յ�STA1���͵�DHCP�㲥�����鲥��ֱ��OAL_NF_ACCEPT;
           2.AP0�յ�STA1���͵�ARP�㲥�����鲥��ֱ��OAL_NF_ACCEPT;
           3.AP0�յ�STA1���͵�ARP������ֱ��ͨ��STA1��Ӧ��STA1'�Ķ˿ڽ����ķ��͸�Root��������OAL_NF_ACCEPT;
           4.AP0�յ�STA1���͵�ICMP ping��ֱ��OAL_NF_ACCEPT;
           5.AP0�յ�STA1���͵ļ�����֤EAPOL֡��ֱ��OAL_NF_ACCEPT. */
        case WAL_PROXYSTA_PTYPE_VAP:
        //if (WAL_PROXYSTA_PFLAG_IS_ISO(ul_pflag))
        //{
            OAM_INFO_LOG0(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_pre_route::received packet, start into AP0 process.}");

            pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_in);
            if (OAL_PTR_NULL == pst_mac_vap)
            {
                OAM_ERROR_LOG0(0, OAM_SF_PROXYSTA,
                               "{wal_proxysta_handle_pre_route::get mac_vap failed.}");
                return OAL_NF_DROP;
            }

            /* Ŀ�ĵ�ַΪAP0��Ŀ�ĵ�ַ��Ӧ���Ƿ��͸�AP0�ļ�����֤֡������ */
            if (0 == oal_compare_mac_addr(oal_eth_hdr(pst_netbuf)->h_dest, mac_mib_get_StationID(pst_mac_vap)))
            {
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                              "{wal_proxysta_handle_pre_route::sk_buff dest address is AP0.}");
                return OAL_NF_ACCEPT;
            }

            /* AP0�յ�STA1���͵�DHCP����ARP�㲥�����鲥��ֱ��OAL_NF_ACCEPT */
            if (en_ismcast)
            {
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                              "{wal_proxysta_handle_pre_route::AP0 received multicast process.}");
                return OAL_NF_ACCEPT;
            }

            /* AP0�յ�STA1���͸�sta0�İ�����local in ���͸��ϲ�Э��ջ�������ϲ���ת�� ������ֹ */
            if (0 == oal_compare_mac_addr(oal_eth_hdr(pst_netbuf)->h_dest, pst_dev_psta->auc_mac_addr))
            {
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                              "{wal_proxysta_handle_pre_route::sk_buff dest address is STA0.}");
                return OAL_NF_ACCEPT;
            }

            /* ����ΪAP0�յ���STA1���͸�Root AP�ĵ������Ĵ��� */
            /* AP0�յ�STA1���͵ĵ�����������ͨ��ת���Ķ˿�STA1'���Ӷ�����ת�� */
            ul_ret = wal_proxysta_find_net_dev(pst_dev_psta, oal_eth_hdr(pst_netbuf)->h_source, &pst_net_device);
            if (OAL_SUCC != ul_ret)
            {
                OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_pre_route::wal_proxysta_find_net_dev don't find proxysta.}");
                return OAL_NF_DROP;
            }

            /* Cloneһ��skb����skb2ͨ��STA1��Ӧ��STA1'����ת�� */
            pst_netbuf_clone = oal_netbuf_clone(pst_netbuf);
            if (OAL_PTR_NULL == pst_netbuf_clone)
            {
                OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                               "{wal_proxysta_handle_pre_route::clone sk_buff failed.}");
                return OAL_NF_DROP;
            }

            pst_netbuf_clone->dev = pst_net_device;

            /* ԭskb���ö�����ǣ������Ž���ԴMAC��ַѧϰ��ѧϰ��forwardʱ���� */
            pst_netbuf->mark = OAL_PROXYSTA_MARK_DROPPED;

            OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                               "{wal_proxysta_handle_pre_route::start send cloned sk_buff by proxysta.}");

            /* ֱ��ͨ��STA1��Ӧ��STA1'��clone�ı��ķ��͸�Root AP */
            wal_proxysta_dev_xmit(pst_netbuf_clone);

            return OAL_NF_ACCEPT;
        //}

        case WAL_PROXYSTA_PTYPE_PETH:

        /* proxysta���յ�����֡�Ĵ����������:(����Ӳ��ʵ��ֻ��main sta�յ���root ap���͵Ĺ㲥�����鲥���Ĳű��ϱ�)
           1.proxysta�յ�root_ap���͵�ARP������ֱ��OAL_NF_ACCEPT;
           2.proxysta�յ�root_ap���͵�ICMP ping��ֱ��OAL_NF_ACCEPT. */
        case WAL_PROXYSTA_PTYPE_PVAP:
            OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                          "{wal_proxysta_handle_pre_route::received packet, start into proxysta process.}");

            /* proxysta�յ��Լ����͵ı���ֱ�Ӷ��� */
            if (OAL_PROXYSTA_MARK_DROPPED == pst_netbuf->mark)
            {
                OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_pre_route::received packet which src address is equal with own address, dropped.}");

                /* sk_buff mark�ֶ����� */
                pst_netbuf->mark = 0;
                return OAL_NF_DROP;
            }
            break;

        /* main sta���յ�����֡�Ĵ����������:
           1.main sta�յ�root_ap���͵�DHCP�㲥�����鲥��ֱ��OAL_NF_ACCEPT;
           2.main sta�յ�root_ap���͵�ARP�㲥�����鲥��ֱ��OAL_NF_ACCEPT;
           3.main sta�յ�root_ap���͵�ARP������ֱ��OAL_NF_ACCEPT;
           4.main sta�յ�root_ap���͵�ICMP ping��ֱ��OAL_NF_ACCEPT. */
        case WAL_PROXYSTA_PTYPE_MPVAP:
            OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                          "{wal_proxysta_handle_pre_route::received packet, start into main sta process.}");

            /* main sta�յ��Լ����͵ı���ֱ�Ӷ��� */
            if (OAL_PROXYSTA_MARK_DROPPED == pst_netbuf->mark)
            {
                OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_pre_route::received packet which src address is equal with own address, dropped.}");
                /* sk_buff mark�ֶ����� */
                pst_netbuf->mark = 0;
                return OAL_NF_DROP;
            }
            break;

        default:
            break;
    }

    return OAL_NF_ACCEPT;
}

#if 0
/*****************************************************************************
 �� �� ��  : wal_proxysta_handle_pre_route
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_pre_route(oal_uint32 ul_hooknum,
                                                     oal_netbuf_stru *pst_netbuf,
                                                     OAL_CONST oal_net_device_stru *pst_in,
                                                     OAL_CONST oal_net_device_stru *pst_out,
                                                     oal_int32 (*p_okfn_func)(oal_netbuf_stru *))
{
    oal_bool_enum_uint8         en_ismcast;
    mac_device_proxysta_stru   *pst_dev_psta;
    oal_net_device_stru        *pst_net_device;
    oal_uint32                  ul_type  = oal_br_get_port_type(pst_in->br_port);
    oal_uint32                  ul_ptype = WAL_PROXYSTA_PTYPE(ul_type);
    oal_uint32                  ul_pflag = WAL_PROXYSTA_PFLAG(ul_type);
    oal_netbuf_stru            *pst_netbuf_clone;
    oal_uint32                  ul_ret;

    en_ismcast = ETHER_IS_MULTICAST(oal_eth_hdr(pst_netbuf)->h_dest);

    ul_ret = wal_proxysta_get_psta_dev(pst_in, &pst_dev_psta);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        return OAL_NF_DROP;
    }

    switch (ul_ptype)
    {
        case WAL_PROXYSTA_PTYPE_ETH:
        case WAL_PROXYSTA_PTYPE_VAP:
            if (WAL_PROXYSTA_PFLAG_IS_ISO(ul_pflag))
            {


                if (en_ismcast)
                {
                    return OAL_NF_ACCEPT;
                }

                /* ���͸�sta0�İ�����local in ���͸��ϲ�Э��ջ�������ϲ���ת�� ������ֹ */
                if (0 == oal_compare_mac_addr(oal_eth_hdr(pst_netbuf)->h_dest, pst_dev_psta->auc_mac_addr))
                {
                    return OAL_NF_ACCEPT;
                }

                ul_ret = wal_proxysta_find_net_dev(pst_dev_psta, oal_eth_hdr(pst_netbuf)->h_source, &pst_net_device);
                if (OAL_SUCC != ul_ret)
                {
                    return OAL_NF_DROP;
                }

                pst_netbuf_clone = oal_netbuf_clone(pst_netbuf);
                if (OAL_PTR_NULL == pst_netbuf_clone)
                {
                    return OAL_NF_DROP;
                }

                pst_netbuf_clone->dev = pst_net_device;

                /*let this pkt to go through the bridge for learning */
                /*  will be dropped in the forward hook so mark it*/
                pst_netbuf->mark = OAL_PROXYSTA_MARK_DROPPED;
                wal_proxysta_dev_xmit(pst_netbuf_clone);

                return OAL_NF_ACCEPT;
            }
            break;


        case WAL_PROXYSTA_PTYPE_PETH:
        case WAL_PROXYSTA_PTYPE_PVAP:
            if (OAL_PROXYSTA_MARK_DROPPED == pst_netbuf->mark)
            {
                return OAL_NF_DROP;
            }
            break;

        default:
            break;
    }

    return OAL_NF_ACCEPT;
}
#endif

/*****************************************************************************
 �� �� ��  : wal_proxysta_handle_forward
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_forward(oal_uint32 ul_hooknum,
                                                   oal_netbuf_stru *pst_netbuf,
                                                   OAL_CONST oal_net_device_stru *pst_in,
                                                   OAL_CONST oal_net_device_stru *pst_out,
                                                   oal_int32 (*p_okfn_func)(oal_netbuf_stru *))
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        /*TBD*/
        oal_uint32             ul_ptype = ~0UL;
#else
        oal_uint32             ul_ptype = WAL_PROXYSTA_PTYPE(oal_br_get_port_type(pst_in->br_port));
#endif
    oal_bool_enum_uint8    en_ismcast;

    OAM_INFO_LOG1(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_forward:ul_ptype:%d}",ul_ptype);
    en_ismcast = ETHER_IS_MULTICAST(oal_eth_hdr(pst_netbuf)->h_dest);

    if (en_ismcast)
    {
        OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                      "{wal_proxysta_handle_forward::received multicast packet.}");
        return OAL_NF_ACCEPT;
    }

    switch (ul_ptype)
    {
        case WAL_PROXYSTA_PTYPE_ETH:
        /* AP0�յ��ĵ�������֡����pre_route�����Ϊ�������˴����ж��� */
        case WAL_PROXYSTA_PTYPE_VAP:
            if (OAL_PROXYSTA_MARK_DROPPED == pst_netbuf->mark)
            {

                /* sk_buff mark�ֶ����� */
                pst_netbuf->mark = 0;
                return OAL_NF_DROP;
            }

            break;

        default:
            break;
    }

    return OAL_NF_ACCEPT;
}

#if 0
/*****************************************************************************
 �� �� ��  : wal_proxysta_handle_post_route
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_post_route(oal_uint32 ul_hooknum,
                                                      oal_netbuf_stru *pst_netbuf,
                                                      OAL_CONST oal_net_device_stru *pst_in,
                                                      OAL_CONST oal_net_device_stru *pst_out,
                                                      oal_int32 (*p_okfn_func)(oal_netbuf_stru *))
{
    mac_device_proxysta_stru   *pst_dev_psta;
    oal_net_device_stru        *pst_net_device;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))
        /*TBD? 1102*/
        oal_uint32                  ul_ptype = ~0UL;
#else
        oal_uint32                  ul_ptype = WAL_PROXYSTA_PTYPE(oal_br_get_port_type(pst_out->br_port));
#endif
    mac_vap_stru               *pst_mac_vap;
    oal_bool_enum_uint8         en_ismcast;
    oal_uint32                  ul_ret;
    oal_uint8                  *puc_h_source;

    en_ismcast = ETHER_IS_MULTICAST(oal_eth_hdr(pst_netbuf)->h_dest);

    ul_ret = wal_proxysta_get_psta_dev(pst_out, &pst_dev_psta);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        return OAL_NF_DROP;
    }

    OAL_IO_PRINT("\n-------------wal_proxysta_handle_post_route--------------\n");
    OAL_IO_PRINT("\n  test 0: ul_ptype is %d \n",ul_ptype);

    puc_h_source = oal_eth_hdr(pst_netbuf)->h_source;
    switch (ul_ptype)
    {
        case WAL_PROXYSTA_PTYPE_PETH:
        case WAL_PROXYSTA_PTYPE_PVAP:
        /*  proxysta begin */
        case WAL_PROXYSTA_PTYPE_MPVAP:
        /*  proxysta end */
            pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_out);

            /*  proxysta begin */
            OAL_IO_PRINT("\n test 1: %s:%d [%p]\n", pst_out->name, ul_ptype, pst_netbuf);
            /*  proxysta end */

            /* ��sta1���͹����İ���Դ��ַ(sta1)��sta1'��oma(sta1)���,��ͨ��sta1�����㲥֡���͸�root_ap���ڵ���wal_bridge_vap_xmit����ʱ,�ὫԴ��ַ��sta1�滻Ϊsta1�� */
            if ((0 == oal_compare_mac_addr(puc_h_source, pst_mac_vap->st_vap_proxysta.auc_oma)) ||
                (0 == oal_compare_mac_addr(puc_h_source, mac_mib_get_StationID(pst_mac_vap))))
            {
                OAL_IO_PRINT("\n test 2 :%s:%d [%p]\n", pst_out->name, ul_ptype, pst_netbuf);
                return OAL_NF_ACCEPT;
            }

            if (en_ismcast || (pst_netbuf->mark == OAL_WRAP_BR_MARK_FLOOD))
            {
                OAL_IO_PRINT("\n test 3: it is a mcast packet, OAL_NF_DROP.\n");
                return OAL_NF_DROP;
            }
            else
            {
                OAL_IO_PRINT("\n test 4: it is a unicast packet.\n");
                ul_ret = wal_proxysta_find_net_dev(pst_dev_psta, oal_eth_hdr(pst_netbuf)->h_source, &pst_net_device);
                if (OAL_SUCC != ul_ret)
                {
                    return OAL_NF_DROP;
                }

                pst_netbuf->dev = pst_net_device;
                wal_proxysta_dev_xmit(pst_netbuf);

                return OAL_NF_STOLEN;
            }

        default:
            break;
    }

    return OAL_NF_ACCEPT;
}
#endif

/*****************************************************************************
 �� �� ��  : wal_proxysta_handle_post_route
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��28��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_post_route(oal_uint32 ul_hooknum,
                                                      oal_netbuf_stru *pst_netbuf,
                                                      OAL_CONST oal_net_device_stru *pst_in,
                                                      OAL_CONST oal_net_device_stru *pst_out,
                                                      oal_int32 (*p_okfn_func)(oal_netbuf_stru *))
{
    mac_device_proxysta_stru   *pst_dev_psta;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44)) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /*TBD? 1102*/
    oal_uint32                  ul_ptype = ~0UL;
#else
    oal_uint32                  ul_ptype = WAL_PROXYSTA_PTYPE(oal_br_get_port_type(pst_out->br_port));
#endif
    mac_vap_stru               *pst_mac_vap;
    oal_uint32                  ul_ret;
    oal_uint8                  *puc_h_source;
    oal_uint8                  *puc_d_source;
    oal_net_device_stru        *pst_net_device;
    oal_bool_enum_uint8         en_ismcast;

    ul_ret = wal_proxysta_get_psta_dev(pst_out, &pst_dev_psta);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_ERROR_LOG0(0, OAM_SF_PROXYSTA,
                       "{wal_proxysta_handle_post_route::get proxysta dev failed.}");
        return OAL_NF_DROP;
    }
    OAM_INFO_LOG1(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_post_route:ul_ptype:%d}",ul_ptype);

    en_ismcast = ETHER_IS_MULTICAST(oal_eth_hdr(pst_netbuf)->h_dest);
    puc_h_source = oal_eth_hdr(pst_netbuf)->h_source;
    puc_d_source = oal_eth_hdr(pst_netbuf)->h_dest;

    /* ��ӡ����Դ��ַ����ȷ�ϴ˰������ĸ��˿ڷ����� */
    OAM_INFO_LOG4(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_post_route:h_source_mac:%02X:XX:XX:%02X:%02X:%02X}",
                  puc_h_source[0],puc_h_source[3],puc_h_source[4],puc_h_source[5]);

    OAM_INFO_LOG4(0, OAM_SF_PROXYSTA, "{wal_proxysta_handle_post_route:d_source_mac:%02X:XX:XX:%02X:%02X:%02X}",
                  puc_d_source[0],puc_d_source[3],puc_d_source[4],puc_d_source[5]);

    switch (ul_ptype)
    {
        /* AP0ͨ������ת������֡�Ĵ����������:
           1.AP0ת��STA0���͵�DHCP�㲥�����鲥��;
           2.AP0ת��STA0���͵�ARP�㲥�����鲥��;
           3.AP0ת��STA1'���͵�ARP��������������ĵ�Դ��ַ����proxysta��oma��ַ��OAL_NF_ACCEPT;
           4.AP0ת��STA1'���͵�ICMP ping����������ĵ�Դ��ַ����proxysta��oma��ַ��OAL_NF_ACCEPT. */
        case WAL_PROXYSTA_PTYPE_VAP:
            OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                          "{wal_proxysta_handle_post_route::start into AP0 process.}");

            pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_out);
            if (OAL_PTR_NULL == pst_mac_vap)
            {
                OAM_WARNING_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_post_route::get mac vap failed.}");
                return OAL_NF_DROP;
            }

            return OAL_NF_ACCEPT;

        case WAL_PROXYSTA_PTYPE_PETH:

        /* proxystaͨ������ת������֡�Ĵ����������:
           1.proxystaת��STA1���͵�DHCP�㲥�����鲥����������ĵ�Դ��ַ����proxysta��oma��ַ��OAL_NF_ACCEPT;
           2.proxystaת��STA1���͵�ARP�㲥�����鲥����������ĵ�Դ��ַ����proxysta��oma��ַ��OAL_NF_ACCEPT;
           3.proxystaת��STA1���͵�ARP��������������ĵ�Դ��ַ����proxysta��oma��ַ��OAL_NF_ACCEPT;
           4.proxystaת��STA1���͵�ICMP ping����������ĵ�Դ��ַ����proxysta��oma��ַ��OAL_NF_ACCEPT. */
        case WAL_PROXYSTA_PTYPE_PVAP:
            OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                          "{wal_proxysta_handle_post_route::start into proxysta process.}");

            pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_out);
            if (OAL_PTR_NULL == pst_mac_vap)
            {
                OAM_WARNING_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_post_route::get mac vap failed.}");
                return OAL_NF_DROP;
            }

            /* �����sta1���͹����İ���Դ��ַ(sta1)��sta1'��oma(sta1)��ȣ����۰���ʲô����
               ��ͨ��sta1�������ķ��͸�root_ap���ڵ���wal_bridge_vap_xmit����ʱ,�ὫԴ��ַ��sta1�滻Ϊsta1�� */
            if ((0 == oal_compare_mac_addr(puc_h_source, pst_mac_vap->st_vap_proxysta.auc_oma)) ||
                (0 == oal_compare_mac_addr(puc_h_source, mac_mib_get_StationID(pst_mac_vap))))
            {
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                              "{wal_proxysta_handle_post_route::source mac == oma mac.}");
                return OAL_NF_ACCEPT;
            }

            if (en_ismcast || (pst_netbuf->mark == OAL_WRAP_BR_MARK_FLOOD))
            {
                OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_post_route::multicast or flood, dropped.}");
                return OAL_NF_DROP;
            }
            else
            {
                OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_post_route::proxysta tx ucast data}");

                ul_ret = wal_proxysta_find_mainsta_net_dev(pst_dev_psta, oal_eth_hdr(pst_netbuf)->h_source, &pst_net_device);
                if (OAL_SUCC != ul_ret)
                {
                    return OAL_NF_DROP;
                }

                pst_netbuf->dev = pst_net_device;

                OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_post_route::send unicast packet by main sta in proxysta process.}");

                wal_proxysta_dev_xmit(pst_netbuf);

                return OAL_NF_STOLEN;
            }

        /* main staͨ������ת������֡�Ĵ����������:
           1.main staֻ�������ű���(main sta)���͵ı���. */
        case WAL_PROXYSTA_PTYPE_MPVAP:
            OAM_INFO_LOG0(0, OAM_SF_PROXYSTA,
                          "{wal_proxysta_handle_post_route::start into main sta process.}");

            pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_out);
            if (OAL_PTR_NULL == pst_mac_vap)
            {
                OAM_WARNING_LOG0(0, OAM_SF_PROXYSTA,
                                 "{wal_proxysta_handle_post_route::get mac vap failed.}");
                return OAL_NF_DROP;
            }

            /* ������ͱ��ĵ�Դmac��main sta����ĵ�ַ�����ۺ��ֱ��ģ�ֱ�ӽ�����ת���������� */
            if (0 == oal_compare_mac_addr(puc_h_source, mac_mib_get_StationID(pst_mac_vap)))
            {
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                              "{wal_proxysta_handle_post_route::send packet, which src address is equal with mainsta's.}");
                return OAL_NF_ACCEPT;
            }
            else
            {
                OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PROXYSTA,
                              "{wal_proxysta_handle_post_route::dropped packet, which src address is not equal with mainsta's.}");
                return OAL_NF_DROP;
            }

        default:
            break;
    }

    return OAL_NF_ACCEPT;
}

/*****************************************************************************
 �� �� ��  : wal_proxysta_handle_local_in
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC oal_uint32  wal_proxysta_handle_local_in(oal_uint32 ul_hooknum,
                                                    oal_netbuf_stru *pst_netbuf,
                                                    OAL_CONST oal_net_device_stru *pst_in,
                                                    OAL_CONST oal_net_device_stru *pst_out,
                                                    oal_int32 (*p_okfn_func)(oal_netbuf_stru *))
{
    return OAL_NF_ACCEPT;
}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

