/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hwal_net.c
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
#include "hwal_net.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HWAL_NET_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
netif_flow_ctrl_enum            g_en_netif_flow_ctrl = NETIF_FLOW_CTRL_OFF;

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : netif_set_flow_ctrl_status
 ��������  : LiteOSЭ��ջ���ط�ѹ�ӿ�
 �������  : oal_netif *netif, netif_flow_ctrl_enum_uint8 status
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void  netif_set_flow_ctrl_status(oal_lwip_netif *pst_netif, netif_flow_ctrl_enum_uint8 en_status)
{
    if (OAL_PTR_NULL == pst_netif)
    {
        OAM_ERROR_LOG0(0, 0, "netif parameter NULL.");
        return;
    }

    if (NETIF_FLOW_CTRL_ON == en_status)
    {
        g_en_netif_flow_ctrl = NETIF_FLOW_CTRL_ON;
    }
    else if (NETIF_FLOW_CTRL_OFF == en_status)
    {
        g_en_netif_flow_ctrl = NETIF_FLOW_CTRL_OFF;
    }
    else
    {
        OAM_ERROR_LOG0(0, 0, "netif_set_flow_ctrl_status::status invalid!\r\n");
    }
}

/*****************************************************************************
 �� �� ��  : hwal_netif_rx
 ��������  : liteos���ݽ��սӿڣ�ʵ�������ϱ�lwipЭ��ջ
 �������  : oal_netbuf_stru *pst_netbuf
 �������  : ��
 �� �� ֵ  : oal_int32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32 hwal_netif_rx(oal_net_device_stru *pst_net_dev, oal_netbuf_stru *pst_netbuf)
{
    oal_uint16                  us_ether_type;
    oal_ether_header_stru      *pst_eth_hdr;
    oal_hisi_eapol_stru        *pst_hisi_eapol;

    if ((OAL_PTR_NULL == pst_netbuf) || (OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, 0, "netif_rx_ni parameter NULL.");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ֡���� */
    pst_eth_hdr     = (oal_ether_header_stru *)(pst_netbuf->data);
    us_ether_type   = pst_eth_hdr->us_ether_type;

    /* ���ݲ�ͬ֡���ͣ�������Ӧ���� */
    if ((OAL_HOST2NET_SHORT(ETHER_TYPE_IP) == us_ether_type)
        || (OAL_HOST2NET_SHORT(ETHER_TYPE_ARP) == us_ether_type)
        || (OAL_HOST2NET_SHORT(ETHER_TYPE_RARP) == us_ether_type))
    {
        hwal_lwip_receive(pst_net_dev->pst_lwip_netif, pst_netbuf);

        /*   Hi1131C bug fix  / begin */
        #ifdef _PRE_LWIP_ZERO_COPY
        if(OAL_SUCC != hwal_skb_struct_free(pst_netbuf))
        {
            OAM_ERROR_LOG0(0, 0, "[hwal_lwip_receive] skb_struct free fail");
        }
        #else
        oal_netbuf_free(pst_netbuf);
        #endif
        /*   Hi1131C bug fix  / end */
    }
    else if (OAL_HOST2NET_SHORT(ETHER_TYPE_PAE) == us_ether_type)
    {
        pst_hisi_eapol = &pst_net_dev->st_hisi_eapol;

        /* ���жϽ�������� */
        oal_netbuf_list_tail(&pst_hisi_eapol->st_eapol_skb_head, pst_netbuf);

        /* ��Ӻ����¼���WPA��ȡ */
        if ((OAL_TRUE == pst_hisi_eapol->en_register) && (pst_hisi_eapol->notify_callback != OAL_PTR_NULL))
        {
            pst_hisi_eapol->notify_callback(pst_net_dev->name, pst_hisi_eapol->context);
        }
        else
        {
            OAM_ERROR_LOG0(0,0,"eapol process is not register.\r\n");
        }
    }
    else
    {
        oal_netbuf_free(pst_netbuf);
    }

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : hwal_lwip_skb_alloc
 ��������  : ����netbuf�ռ䣬ȡ����������ͷ�ռ估β�ռ�Ԥ��
 �������  : oal_int32 l_size
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_netbuf_stru *
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru *hwal_lwip_skb_alloc(oal_net_device_stru *pst_net_dev, oal_uint16 us_lwip_buflen)
{
    oal_netbuf_stru        *pst_buf;
    oal_uint32              ul_total_size;

    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_skb_alloc pst_net_dev NULL.");
        return OAL_PTR_NULL;
    }

    /* �����СΪHCC������󳤶ȼ���ͷ�ռ估β�ռ� */
    ul_total_size   = us_lwip_buflen + pst_net_dev->needed_headroom + pst_net_dev->needed_tailroom;
    pst_buf         = oal_netbuf_alloc(ul_total_size, pst_net_dev->needed_headroom, 0);

    return pst_buf;
}

/*****************************************************************************
 �� �� ��  : hwal_skb_struct_alloc
 ��������  : ��������struct sk_buff
 �������  : oal_void
 �������  : ��
 �� �� ֵ  : oal_netbuf_stru *
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��7��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_netbuf_stru *hwal_skb_struct_alloc(oal_void)
{
    oal_netbuf_stru *pst_skb = (oal_netbuf_stru *)oal_memalloc(SKB_DATA_ALIGN(OAL_SIZEOF(oal_netbuf_stru)));
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_skb))
    {
        return OAL_PTR_NULL;
    }

    OAL_MEMZERO(pst_skb, OAL_SIZEOF(oal_netbuf_stru));
    oal_atomic_set(&pst_skb->users, 1);

    return pst_skb;
}
/*****************************************************************************
 �� �� ��  : hwal_skb_struct_free
 ��������  : �����ͷ�struct sk_buff
 �������  : oal_netbuf_stru *pst_sk_buf
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��7��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 hwal_skb_struct_free(oal_netbuf_stru *pst_sk_buf)
{
    if (OAL_PTR_NULL == pst_sk_buf)
    {
        return OAL_FAIL;
    }

    oal_free(pst_sk_buf);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : hwal_pbuf_convert_2_skb
 ��������  : ��Lwip��pbuf��skbת��
 �������  : oal_lwip_buf *pst_lwip_buf
             oal_netbuf_stru *pst_sk_buf
 �������  : ��
 �� �� ֵ  : oal_uint32
 ���ú���  :
 ��������  : hwal_lwip_send

 �޸���ʷ      :
  1.��    ��   : 2016��7��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 hwal_pbuf_convert_2_skb(oal_lwip_buf *pst_lwip_buf, oal_netbuf_stru *pst_sk_buf)
{
    oal_net_device_stru *pst_netdev          = oal_dev_get_by_name("wlan0");
    oal_uint32          ul_reserve_head_room = OAL_NETDEVICE_HEADROOM(pst_netdev);
    oal_uint32          ul_reserve_tail_room = OAL_NETDEVICE_TAILROOM(pst_netdev);

    /*
    1.skb/pbufָ��ת��
    2.sk/pbuf����ת��
    3.pbuf->ref������ȷ��������HCC�ɹ����ͺ����ͷſռ�
    */
    if ((OAL_PTR_NULL == pst_lwip_buf) || (OAL_PTR_NULL == pst_sk_buf))
    {
        OAM_ERROR_LOG2(0, 0, "[hwal_pbuf_convert_2_skb] pst_lwip_buf[%x],pst_sk_buf[%x]!", pst_lwip_buf, pst_sk_buf);
        return OAL_FAIL;
    }

    /*
                                 pbuf's memory distribution
    |-----PBUF-----|--------RESERVE--------|----------PAYLOAD---------|---TAILROOM---|

                          converted sk_buff's ptr according to pbuf
    p_mem_head     head                    data                       tail           end
    */

    pst_sk_buf->p_mem_head = (oal_uint8 *)pst_lwip_buf;
    pst_sk_buf->head       = pst_sk_buf->p_mem_head + OAL_NLMSG_ALIGN(sizeof(oal_lwip_buf));
    pst_sk_buf->data       = (oal_uint8 *)pst_lwip_buf->payload;
    skb_reset_tail_pointer(pst_sk_buf);
    pst_sk_buf->tail      += pst_lwip_buf->len;
    /* �ڴ�����ʱ����������ֽڶ��룬�˴����õ����ڴ�Խ�� */
    pst_sk_buf->end        = OAL_NLMSG_ALIGN((oal_uint32)pst_sk_buf->tail) + OAL_NLMSG_ALIGN(ul_reserve_tail_room);

    pst_sk_buf->len        = pst_lwip_buf->len;

    pst_lwip_buf->ref++;

    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : hwal_skb_convert_2_pbuf
 ��������  : ��skb��Lwip��pbufת��
 �������  : oal_netbuf_stru *pst_sk_buf
 �������  : ��
 �� �� ֵ  : oal_lwip_buf *
 ���ú���  :
 ��������  : hwal_lwip_receive

 �޸���ʷ      :
  1.��    ��   : 2016��7��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_lwip_buf *hwal_skb_convert_2_pbuf(oal_netbuf_stru *pst_sk_buf)
{
    /*
    1.skb/pbufָ��ת��
    2.sk/pbuf����ת�� len, tot_len
    */
    oal_lwip_buf       *pst_lwip_buf  = OAL_PTR_NULL;

    if ((OAL_PTR_NULL == pst_sk_buf) || (OAL_PTR_NULL == pst_sk_buf->p_mem_head))
    {
        OAM_ERROR_LOG0(0, 0, "[hwal_skb_convert_2_pbuf] pst_sk_buf or p_mem_head = NULL!");
        return OAL_PTR_NULL;
    }

    pst_lwip_buf          = (oal_lwip_buf *)pst_sk_buf->p_mem_head;
    pst_lwip_buf->payload = pst_sk_buf->data;
    pst_lwip_buf->tot_len = pst_sk_buf->len;
    pst_lwip_buf->len     = pst_sk_buf->len;

    return pst_lwip_buf;
}
/*****************************************************************************
 �� �� ��  : hwal_lwip_receive
 ��������  : �����������ݲ��ϱ�lwip�ӿڣ��������ͷ�netbuf�ڴ�
 �������  :
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���
  2.��    ��   : 2016��7��30��
    ��    ��   : 
    �޸�����   : �����޸�

*****************************************************************************/
oal_void hwal_lwip_receive(oal_lwip_netif *pst_netif, oal_netbuf_stru *pst_drv_buf)
{
    oal_lwip_buf       *pst_lwip_buf;
    if ((OAL_PTR_NULL == pst_netif) || (OAL_PTR_NULL == pst_drv_buf))
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_receive pst_netif is null!");
        return;
    }

    /*   Hi1131C bug fix  / begin */
#ifdef _PRE_LWIP_ZERO_COPY
    pst_lwip_buf = hwal_skb_convert_2_pbuf(pst_drv_buf);
    if (pst_lwip_buf != pst_drv_buf->p_mem_head)
    {
        OAM_ERROR_LOG0(0, 0, "[hwal_lwip_receive] skb_convert_2_pbuf, addr not match!");
        return;
    }
#else
    /* ����LWIPЭ��ջ�����ڴ� */
    pst_lwip_buf = pbuf_alloc(PBUF_RAW, (OAL_NETBUF_LEN(pst_drv_buf) + ETH_PAD_SIZE), PBUF_RAM);
    if (OAL_PTR_NULL == pst_lwip_buf)
    {
        OAM_ERROR_LOG1(0, 0, "hwal_lwip_receive pbuf_alloc failed! len = %d", OAL_NETBUF_LEN(pst_drv_buf));
        return;
    }

    /* ��payload��ַ����ƫ��2�ֽ� */
#if ETH_PAD_SIZE
    pbuf_header(pst_lwip_buf, -ETH_PAD_SIZE);
#endif

    /* ���ڴ渴�Ƶ�LWIPЭ��ջ�����ڴ� */
    oal_memcopy(pst_lwip_buf->payload, OAL_NETBUF_DATA(pst_drv_buf), OAL_NETBUF_LEN(pst_drv_buf));

#endif
    /*   Hi1131C bug fix  / end */
    /* ��payload��ַǰ��2�ֽ� */
#if ETH_PAD_SIZE
    pbuf_header(pst_lwip_buf, ETH_PAD_SIZE);
#endif

    /* �ϱ�Э��ջ */
    driverif_input(pst_netif, pst_lwip_buf);
}

/*****************************************************************************
 �� �� ��  : hwal_lwip_send
 ��������  : ��LWIPЭ��ջע��ķ��ͻص�����
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_int32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���
  2.��    ��   : 2016��7��30��
    ��    ��   : 
    �޸�����   : �����޸�

*****************************************************************************/
oal_int32 hwal_lwip_send(oal_lwip_netif *pst_netif, oal_lwip_buf *pst_lwip_buf)
{
    oal_netbuf_stru        *pst_converted_skb;
    oal_lwip_buf           *pst_buf_tmp;
    mac_vap_stru           *pst_vap;
    oal_net_device_stru    *pst_net_dev;
    oal_uint32              ul_lwip_buf_index   = 0;
    oal_uint32              ul_drv_buf_offset   = 0;

    if ((OAL_PTR_NULL == pst_lwip_buf) || (OAL_PTR_NULL == pst_netif))
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_send parameter NULL.");
        return -OAL_EFAIL;
    }

    pst_net_dev = (oal_net_device_stru *)pst_netif->state;
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_ERROR_LOG0(0, 0, "pst_netif->state parameter NULL.");
        return -OAL_EFAIL;
    }

    /* ��ȡVAP�ṹ�� */
    pst_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_net_dev);

    /* ���VAP�ṹ�岻���ڣ���������, hwal�����һ���пմ�����ֹhostapd�������ʱ�Ĵ����ӡ */
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_vap))
    {
        OAM_WARNING_LOG0(0, OAM_SF_TX, "{hwal_lwip_send::pst_vap = OAL_PTR_NULL!}\r\n");
        return OAL_SUCC;
    }

    /* Flow_ctl */
    if (OAL_TRUE == NETIF_IS_NOT_READY())
    {
        // TODO: ��������֡���� DHCP ARP RARP EAPOL
        //printf("[hwal_lwip_wifi_drv_send] stop transfer data to driver!\r\n");

        return OAL_SUCC;
    }

    /*   Hi1131C bug fix  / begin */
#ifdef _PRE_LWIP_ZERO_COPY
    /* HCC���ͳɹ�֮ǰ,Lwip�ش��������ٴ��·�,ֱ�ӷ���,������ڴ��ظ����� */
    if (pst_lwip_buf->ref >= 2)
    {
        return OAL_SUCC;
    }

    pst_converted_skb = hwal_skb_struct_alloc();
    if (OAL_PTR_NULL == pst_converted_skb)
    {
        OAM_ERROR_LOG0(0, 0, "[hwal_lwip_send] skb_buff alloc fail!");
        return -OAL_EFAIL;
    }

    if (OAL_SUCC != hwal_pbuf_convert_2_skb(pst_lwip_buf, pst_converted_skb))
    {
        OAM_ERROR_LOG0(0, 0, "[hwal_lwip_send] pbuf convert 2 skb fail!");
        return -OAL_EFAIL;
    }
    /*   Hi1131C bug fix  / end */
#else
#if ETH_PAD_SIZE
    pst_converted_skb = hwal_lwip_skb_alloc(pst_net_dev, pst_lwip_buf->tot_len + ETH_PAD_SIZE);
    oal_netbuf_pull(pst_converted_skb, ETH_PAD_SIZE);
#else
    pst_converted_skb = hwal_lwip_skb_alloc(pst_net_dev, pst_lwip_buf->tot_len);
#endif
    if (OAL_PTR_NULL == pst_converted_skb)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_skb_alloc NULL.");
        return -OAL_EFAIL;
    }

    for (pst_buf_tmp = pst_lwip_buf; pst_buf_tmp != OAL_PTR_NULL; pst_buf_tmp = pst_buf_tmp->next)
    {
        ul_lwip_buf_index++;
        if (oal_netbuf_tailroom(pst_converted_skb) < pst_buf_tmp->len)
        {
            OAM_ERROR_LOG3(0, 0, "oal_netbuf_tailroom.tail = %d, len = %d, idx = %d", oal_netbuf_tailroom(pst_converted_skb), pst_buf_tmp->len, ul_lwip_buf_index);
            oal_netbuf_free(pst_converted_skb);

            return -OAL_EFAIL;
        }

        oal_netbuf_put(pst_converted_skb, pst_buf_tmp->len);
        oal_memcopy(OAL_NETBUF_DATA(pst_converted_skb) + ul_drv_buf_offset, pst_buf_tmp->payload, pst_buf_tmp->len);

        /* ��ƫ�������£�����pbuf������һ��buf */
        ul_drv_buf_offset += pst_buf_tmp->len;
    }
#endif

    if ((OAL_PTR_NULL == pst_net_dev->netdev_ops) || (OAL_PTR_NULL == pst_net_dev->netdev_ops->ndo_start_xmit))
    {
        OAM_ERROR_LOG0(0, 0, "pst_net_dev->netdev_ops NULL.");
        oal_netbuf_free(pst_converted_skb);
        return -OAL_EFAIL;
    }

    pst_converted_skb->queue_mapping = pst_net_dev->netdev_ops->ndo_select_queue(pst_net_dev, pst_converted_skb);

    pst_net_dev->netdev_ops->ndo_start_xmit(pst_converted_skb, pst_net_dev);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : hwal_lwip_init
 ��������  : LWIPЭ��ջ��ʼ��
 �������  :
 �������  : ��
 �� �� ֵ  : oal_int32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32 hwal_lwip_init(oal_net_device_stru *pst_net_dev)
{
    oal_ip_addr_t       st_gw;
    oal_ip_addr_t       st_ipaddr;
    oal_ip_addr_t       st_netmask;
    oal_lwip_netif     *pst_netif;

    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_init parameter NULL.");
        return OAL_FAIL;
    }

    if (0 != oal_strcmp(pst_net_dev->name, "wlan0"))
    {
        return OAL_SUCC;
    }
    /* ��ʼ��skb list */
    oal_netbuf_head_init(&pst_net_dev->st_hisi_eapol.st_eapol_skb_head);

    pst_netif = oal_memalloc(OAL_SIZEOF(oal_lwip_netif));
    if (OAL_PTR_NULL == pst_netif)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_init failed mem alloc NULL.");
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }
    oal_memset(pst_netif, 0, OAL_SIZEOF(oal_lwip_netif));

    OAL_IP4_ADDR(&st_gw, 192, 168, 43, 1);
    OAL_IP4_ADDR(&st_ipaddr, 192, 168, 43, 1);
    OAL_IP4_ADDR(&st_netmask, 255, 255, 255, 0);

    /* �����豸ע�� */
    pst_netif->state            = (oal_void *)pst_net_dev;
    pst_netif->drv_send         = hwal_lwip_send;
    pst_netif->drv_notify       = hwal_lwip_notify;
    pst_netif->link_layer_type  = WIFI_DRIVER_IF;
    pst_netif->hwaddr_len       = ETHARP_HWADDR_LEN;

    if (OAL_PTR_NULL == netif_add(pst_netif, &st_ipaddr, &st_netmask, &st_gw))
    {
        oal_free(pst_netif);

        OAM_ERROR_LOG0(0, 0, "hwal_lwip_init failed netif_add NULL.");
        return OAL_FAIL;
    }

    oal_memcopy(pst_netif->hwaddr, pst_net_dev->dev_addr, ETHER_ADDR_LEN);
    pst_net_dev->pst_lwip_netif = pst_netif;

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : hwal_lwip_deinit
 ��������  : LWIPЭ��ջȥ��ʼ��
 �������  :
 �������  : ��
 �� �� ֵ  : oal_int32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��22��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void hwal_lwip_deinit(oal_net_device_stru *pst_net_dev)
{
    if (OAL_PTR_NULL == pst_net_dev)
    {
        return;
    }

    if (pst_net_dev->pst_lwip_netif != OAL_PTR_NULL)
    {
        oal_free(pst_net_dev->pst_lwip_netif);
    }

    pst_net_dev->pst_lwip_netif = OAL_PTR_NULL;

    return;
}

/*****************************************************************************
 �� �� ��  : hwal_lwip_notify
 ��������  : ��LWIPЭ��ջע��� LWIP ֪ͨ�ص�����
 �������  :
 �������  : ��
 �� �� ֵ  : OAL_STATIC oal_int32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��09��09��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32 hwal_lwip_notify(oal_lwip_netif *pst_netif, oal_uint32 ul_notify_type)
{
    oal_net_device_stru    *pst_net_dev    = OAL_PTR_NULL;
    mac_vap_stru           *pst_vap        = OAL_PTR_NULL;
    oal_net_notify_stru     st_notify_stru = {0};
    oal_int32               l_ret         = OAL_FAIL;

    if (OAL_PTR_NULL == pst_netif)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_notify: pst_netif NULL.");
        return -OAL_EFAIL;
    }

    pst_net_dev = (oal_net_device_stru *)pst_netif->state;
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_lwip_notify: pst_net_dev NULL.");
        return -OAL_EFAIL;
    }

    st_notify_stru.ul_ip_addr     = *(oal_uint32 *)(&pst_netif->ip_addr);
    st_notify_stru.ul_notify_type = ul_notify_type;

    if ((OAL_PTR_NULL != pst_net_dev->netdev_ops)
     && (OAL_PTR_NULL != pst_net_dev->netdev_ops->ndo_netif_notify))
    {
        l_ret = pst_net_dev->netdev_ops->ndo_netif_notify(pst_net_dev, &st_notify_stru);
    }

    OAM_WARNING_LOG4(0, 0, "{hwal_lwip_notify::pst_net_dev[0x%08X],ndo_netif_notify[0x%08X],ul_notify_type[0x%08X], ul_ret[%d].}\n",
                    pst_net_dev,pst_net_dev->netdev_ops->ndo_netif_notify, ul_notify_type,l_ret);

    return l_ret;
}

/*****************************************************************************
 �� �� ��  : hwal_tcpip_init
 ��������  : ��ʼ��������tcpip�߳�
 �������  : oal_void
 �������  : ��
 �� �� ֵ  : oal_void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��5��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void hwal_tcpip_init(oal_void)
{
    tcpip_init(OAL_PTR_NULL, OAL_PTR_NULL);
}

/*****************************************************************************
 �� �� ��  : hwal_dhcp_start
 ��������  : ����dhcp
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��8��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32 hwal_dhcp_start(oal_net_device_stru *pst_netdev)
{
    oal_int32  l_ret;
    if (OAL_PTR_NULL == pst_netdev)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_dhcp_start pst_netdev is null.");
        return -HISI_EFAIL;
    }
    oal_netif_set_up(pst_netdev->pst_lwip_netif);
    l_ret = oal_dhcp_start(pst_netdev->pst_lwip_netif);
    if (l_ret != OAL_SUCC)
    {
        OAM_ERROR_LOG1(0, 0, "dhcp_start fail %d.",l_ret);
    }
    return l_ret;
}

/*****************************************************************************
 �� �� ��  : hwal_dhcp_stop
 ��������  : �ر�dhcp
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��8��26��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32 hwal_dhcp_stop(oal_net_device_stru *pst_netdev)
{
    if (OAL_PTR_NULL == pst_netdev)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_dhcp_stop pst_netdev is null.");
        return -HISI_EFAIL;
    }
    oal_dhcp_stop(pst_netdev->pst_lwip_netif);
    oal_netif_set_down(pst_netdev->pst_lwip_netif);
    return HISI_SUCC;
}

/*****************************************************************************
 �� �� ��  : hwal_dhcp_succ_check
 ��������  : ���dhcp�Ƿ�ɹ�
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��10��13��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32 hwal_dhcp_succ_check(oal_net_device_stru *pst_netdev)
{
    if (OAL_PTR_NULL == pst_netdev)
    {
        OAM_ERROR_LOG0(0, 0, "hwal_dhcp_succ_check pst_netdev is null.");
        return -HISI_EFAIL;
    }

    return oal_dhcp_succ_check(pst_netdev->pst_lwip_netif);
}

/*****************************************************************************
 �� �� ��  : hwal_lwip_get_netif
 ��������  : ��ȡnetdev�б����netif
 �������  : const oal_int8 *pc_name
 �������  :
 �� �� ֵ  : oal_lwip_netif
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��9��17��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_lwip_netif *hwal_lwip_get_netif(const oal_int8 *pc_name)
{
    oal_net_device_stru *pst_net_dev;

    pst_net_dev = oal_dev_get_by_name(pc_name);

    if (OAL_PTR_NULL == pst_net_dev)
    {
        return OAL_PTR_NULL;
    }

    return pst_net_dev->pst_lwip_netif;
}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

