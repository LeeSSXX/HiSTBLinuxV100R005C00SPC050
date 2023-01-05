/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hmac_tcp_udp_keepalive.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��4��18��
  ����޸�   :
  ��������   : hmac��TCP&UDP����ʵ�ֺ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��4��18��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_TCP_UDP_KEEPALIVE
/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oal_types.h"
#include "oal_net.h"
#include "hmac_tcp_udp_keepalive.h"
#include "dmac_ext_if.h"
#include "oam_wdk.h"
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_TCP_UDP_KEEPALIVE_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
hmac_tcp_udp_conn_stru g_st_tcp_udp_conn;
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : hmac_get_tcp_params
 ��������  : ��ȡ���͵�TCP���Ĳ���,
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 hmac_get_tcp_params(oal_netbuf_stru *pst_netbuf)
{
    mac_ether_header_stru   *pst_ether_hdr    = OAL_PTR_NULL; /* ��̫��ͷ */
    oal_uint8                uc_ip_heard_len  = 0;
    oal_uint8               *puc_data         = OAL_PTR_NULL;
    oal_uint8                uc_index         = 0;
    hmac_tcp_params_stru    *pst_tcp_params   = OAL_PTR_NULL;
    mac_ip_header_stru      *pst_ip_head      = OAL_PTR_NULL;
    mac_tcp_header_stru     *pst_tcp_head     = OAL_PTR_NULL;

    if (OAL_PTR_NULL == pst_netbuf)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hmac_get_tcp_params::pst_netbuf is null");
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_ether_hdr   = (mac_ether_header_stru *)oal_netbuf_data(pst_netbuf);
    puc_data        =  (oal_uint8 *)pst_ether_hdr;

/*lint -e778*/
    if (OAL_HOST2NET_SHORT(ETHER_TYPE_IP) == pst_ether_hdr->us_ether_type)
    {
/*lint +e778*/
        uc_ip_heard_len = (puc_data[ETHER_HDR_LEN] & 0x0f) * 4;//IPͷ�ĳ���
        /* IPͷ�ײ�ָ�� */
        pst_ip_head     =(mac_ip_header_stru *)(puc_data + OAL_SIZEOF(mac_ether_header_stru));
        /* �ж��Ƿ�Ϊtcp���� */
        if (MAC_PROTOCOL_TYPE_TCP == pst_ip_head->uc_protocol)
        {
            /* TCPͷ�ײ�ָ�� */
            pst_tcp_head = (mac_tcp_header_stru *)(puc_data + OAL_SIZEOF(mac_ether_header_stru) + uc_ip_heard_len);
            for (uc_index = 0;uc_index < HMAC_TCP_CONNECT_NUM; uc_index++)
            {
                pst_tcp_params = &g_st_tcp_udp_conn.st_tcp_connects[uc_index];
                /* ͨ���˿ں��ж��Ƿ�Ϊָ��TCP���ӵİ� */
                /*lint -e778*/
                if ((OAL_HOST2NET_SHORT(pst_tcp_head->us_sport) == pst_tcp_params->us_src_port)
                    && (OAL_HOST2NET_SHORT(pst_tcp_head->us_dport) == pst_tcp_params->us_dst_port))
                /*lint +e778*/
                {
                    pst_tcp_params->ul_seq_num = OAL_HOST2NET_LONG(pst_tcp_head->ul_seqnum);
                    pst_tcp_params->ul_ack_num = OAL_HOST2NET_LONG(pst_tcp_head->ul_acknum);
                    pst_tcp_params->us_window  = OAL_HOST2NET_SHORT(pst_tcp_head->us_window);
                    oal_set_mac_addr(pst_tcp_params->uc_dst_mac, pst_ether_hdr->auc_ether_dhost);
                }
            }
        }
    }
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : hmac_get_ip_header_checksum
 ��������  : ����IPͷ��У���
 �������  :
             puc_data: ��Ҫ����У��Ͳ��ֵ�����
             us_len  : �ò��ֵ����ݳ���
 �������  :
 �� �� ֵ  : У���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint16 hmac_get_ip_header_checksum(oal_uint8 *puc_data, oal_uint16 us_len)
{
    oal_uint32    ul_sum   = 0;
    oal_uint16    us_index = 0;
    oal_uint16    us_checksum;
    if (OAL_PTR_NULL == puc_data)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hmac_get_ip_header_checksum::puc_data is null");
        return OAL_ERR_CODE_PTR_NULL;
    }
    for (us_index = 0;us_index < us_len;)
    {
        ul_sum += (puc_data[us_index++] << 8);
        ul_sum += puc_data[us_index++];
    }
    while (ul_sum > 0xffff)
    {
        ul_sum = ((ul_sum & 0xffff0000) >> 16) + (ul_sum & 0x0000ffff);
    }
    us_checksum = (oal_uint16)ul_sum;
    us_checksum = ~us_checksum;

    return us_checksum;
}
/*****************************************************************************
 �� �� ��  : hmac_set_ip_header_checksum
 ��������  : ����IPͷ��У���
 �������  :
             puc_payload: payload�ײ���ַ
             us_checksum: ��Ҫ���õ�У���
             *pus_offset: ��ǰƫ����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32 hmac_set_ip_header_checksum(oal_uint8 *puc_payload, oal_uint16 us_checksum, oal_uint16 *pus_offset)
{
    oal_uint16             us_index;
    mac_ip_header_stru    *pst_ip_head = OAL_PTR_NULL;

    if ((OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == pus_offset))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "hmac_set_ip_header_checksum::puc_payload = %x,pus_offset=%x",puc_payload,pus_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }
    us_index = *pus_offset;

    pst_ip_head           = (mac_ip_header_stru *)(puc_payload + us_index);
    pst_ip_head->us_check = OAL_HTON_16(us_checksum);

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : hmac_encap_llc_hearder
 ��������  : ��LLCͷ
 �������  : puc_payload: payload���ײ���ַ
             *pus_offset: ƫ�Ƴ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/

oal_uint32  hmac_encap_llc_hearder(oal_uint8 *puc_payload, oal_uint16 *pus_offset)
{
    oal_uint16         us_index;
    mac_llc_snap_stru *pst_llc_head = OAL_PTR_NULL;

    if ((OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == pus_offset))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "hmac_encap_llc_hearder::puc_payload=%x,pus_offset=%x",puc_payload,pus_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_index = *pus_offset;
    pst_llc_head = (mac_llc_snap_stru *)(puc_payload + us_index);

    pst_llc_head->uc_llc_dsap   = 0xaa;
    pst_llc_head->uc_llc_ssap   = 0xaa;
    pst_llc_head->uc_control    = 0x03;
    /*lint -e778*/
    pst_llc_head->us_ether_type = OAL_HTON_16(ETHER_TYPE_IP);
    /*lint +e778*/
    us_index += OAL_SIZEOF(mac_llc_snap_stru);

    *pus_offset = us_index;
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : hmac_encap_ip_hearder
 ��������  : ��IPͷ
 �������  : puc_payload: payload���ײ���ַ
             puc_params : TCP��UDP�ڵ�ָ��
             *pus_offset: ƫ�Ƴ���
             uc_flag    : ��ʶ�������TCP��������UDP����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  hmac_encap_ip_hearder(oal_uint8 *puc_payload, oal_uint8 *puc_params, oal_uint16 *pus_offset, hmac_netbuf_type_enum_uint8 uc_flag)
{
    oal_uint16             us_index;
    hmac_tcp_params_stru  *pst_tcp_params  = OAL_PTR_NULL;
    hmac_udp_params_stru  *pst_udp_params  = OAL_PTR_NULL;
    oal_uint16             us_checksum     = 0;
    mac_ip_header_stru    *pst_ip_head     = OAL_PTR_NULL;

    if ((OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == puc_params) || (OAL_PTR_NULL == pus_offset))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "hmac_encap_ip_hearder::puc_payload=%x,puc_params = %x,pus_offset = %x",puc_payload,puc_params,pus_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (HMAC_NETBUF_TYPE_TCP == uc_flag)
    {
        pst_tcp_params = (hmac_tcp_params_stru *)puc_params;
    }
    else if(HMAC_NETBUF_TYPE_UDP == uc_flag)
    {
        pst_udp_params = (hmac_udp_params_stru *)puc_params;
    }
    else
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hmac_encap_ip_hearder::uc_flag is invalid");
        return OAL_FAIL;
    }
    us_index = *pus_offset;

    pst_ip_head = (mac_ip_header_stru *)(puc_payload + us_index);

    pst_ip_head->uc_version_ihl = 0x45;
    pst_ip_head->uc_tos         = 0x00;

    /* IPͷ��tcpͷ�̶�Ϊ20���ֽ� */
    if (HMAC_NETBUF_TYPE_TCP == uc_flag)
    {
        if (OAL_PTR_NULL == pst_tcp_params)
        {
            return OAL_ERR_CODE_PTR_NULL;
        }
        pst_ip_head->us_tot_len = OAL_HTON_16(MAC_IP_HEADER_LEN + MAC_TCP_HEADER_LEN + pst_tcp_params->ul_payload_len);
    }
    else
    {
        if (OAL_PTR_NULL == pst_udp_params)
        {
            return OAL_ERR_CODE_PTR_NULL;
        }
        /*lint -e572*/
        pst_ip_head->us_tot_len = OAL_HTON_16(MAC_IP_HEADER_LEN + MAC_UDP_HEADER_LEN + pst_udp_params->ul_payload_len);
        /*lint +e572*/
    }
    /*lint -e778*/
    pst_ip_head->us_id       = OAL_HTON_16(0x11aa);
    pst_ip_head->us_frag_off = OAL_HTON_16(0x4000);
    /*lint +e778*/
    pst_ip_head->uc_ttl      = 0x80;

    /* �����TCP������UDP�� */
    if (HMAC_NETBUF_TYPE_TCP == uc_flag)
    {
        pst_ip_head->uc_protocol = 0x06;
    }
    if(HMAC_NETBUF_TYPE_UDP == uc_flag)
    {
        pst_ip_head->uc_protocol = 0x11;
    }
    pst_ip_head->us_check = 0;
    if (HMAC_NETBUF_TYPE_TCP == uc_flag)
    {
        if (OAL_PTR_NULL == pst_tcp_params)
        {
            return OAL_ERR_CODE_PTR_NULL;
        }
        oal_set_ip_addr((oal_uint8 *)&(pst_ip_head->ul_saddr), pst_tcp_params->uc_src_ip);
        oal_set_ip_addr((oal_uint8 *)&(pst_ip_head->ul_daddr), pst_tcp_params->uc_dst_ip);
    }
    else
    {
        if (OAL_PTR_NULL == pst_udp_params)
        {
            return OAL_ERR_CODE_PTR_NULL;
        }
        oal_set_ip_addr((oal_uint8 *)&(pst_ip_head->ul_saddr), pst_udp_params->uc_src_ip);
        oal_set_ip_addr((oal_uint8 *)&(pst_ip_head->ul_daddr), pst_udp_params->uc_dst_ip);
    }

    us_index += OAL_SIZEOF(mac_ip_header_stru);

    us_checksum = hmac_get_ip_header_checksum(&puc_payload[*pus_offset], us_index - (*pus_offset));
    hmac_set_ip_header_checksum(puc_payload, us_checksum, pus_offset);
    *pus_offset = us_index;
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : hmac_encap_tcp_hearder
 ��������  : ��TCPͷ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  hmac_encap_tcp_hearder(
                         oal_uint8 *puc_payload,
                         hmac_tcp_params_stru *pst_tcp_params,
                         oal_uint16 *pus_offset)
{
    oal_uint16             us_index;
    mac_tcp_header_stru   *pst_tcp_head = OAL_PTR_NULL;

    if ((OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == pst_tcp_params) || (OAL_PTR_NULL == pus_offset))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "hmac_encap_tcp_hearder::puc_payload = 0x%x, pst_tcp_params = 0x%x,pus_offset = 0x%x",puc_payload,pst_tcp_params,pus_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }
    us_index = *pus_offset;
    pst_tcp_head = (mac_tcp_header_stru *)(puc_payload + us_index);
    pst_tcp_head->us_sport  = OAL_HTON_16(pst_tcp_params->us_src_port);
    pst_tcp_head->us_dport  = OAL_HTON_16(pst_tcp_params->us_dst_port);
    /* seq num��ack num����dmac�����ã�����д0 */
    pst_tcp_head->ul_seqnum = 0;
    pst_tcp_head->ul_acknum = 0;
    /* ��4bit��4��ΪTCPͷ�ĳ��ȣ������趨Ϊ�̶�ֵ20�ֽ� */
    pst_tcp_head->uc_offset = 0x50;
    /* bit4Ϊack���λ����������Ϊ1 */
    pst_tcp_head->uc_flags  = 0x18;
    pst_tcp_head->us_window = OAL_HTON_16(pst_tcp_params->us_window);
    /* check��ֵ��dmac����������д0 */
    pst_tcp_head->us_check  = 0;
    pst_tcp_head->us_urgent = 0;
    us_index += OAL_SIZEOF(mac_tcp_header_stru);

    *pus_offset = us_index;
    return OAL_SUCC;
}
/*****************************************************************************
 �� �� ��  : hmac_encap_udp_hearder
 ��������  : ��UDPͷ
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��6��12��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  hmac_encap_udp_hearder(
                        oal_uint8 *puc_payload,
                        hmac_udp_params_stru *pst_udp_params,
                        oal_uint16 *pus_offset)
{
    oal_uint16    us_index     = 0;
    udp_hdr_stru *pst_udp_head = OAL_PTR_NULL;


    if ((OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == pst_udp_params))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "hmac_encap_tcp_hearder::puc_payload = 0x%x, pst_udp_params = 0x%x,pus_offset = 0x%x",puc_payload,pst_udp_params,pus_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }
    us_index = *pus_offset;
    pst_udp_head = (udp_hdr_stru *)(puc_payload + us_index);
    pst_udp_head->us_src_port  = OAL_HTON_16(pst_udp_params->us_src_port);
    pst_udp_head->us_des_port  = OAL_HTON_16(pst_udp_params->us_dst_port);
    /* ����8�ֽڵ�UDPͷ */
    /*lint -e572*/
    pst_udp_head->us_udp_len   = OAL_HTON_16(MAC_UDP_HEADER_LEN + pst_udp_params->ul_payload_len);
    /*lint +e572*/
    /* ��ֵ��dmac������ */
    pst_udp_head->us_check_sum = 0;
    us_index += OAL_SIZEOF(udp_hdr_stru);

    *pus_offset = us_index;

    return OAL_SUCC;
}


/*****************************************************************************
 �� �� ��  : hmac_encap_tcp
 ��������  : ��װTCP֡
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint16 hmac_encap_tcp(
                mac_vap_stru *pst_mac_vap,
                oal_uint8 *puc_data,
                hmac_tcp_params_stru *pst_tcp_params)
{
    oal_uint16 us_frame_len = 0;
    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_data) || (OAL_PTR_NULL == pst_tcp_params))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "hmac_encap_tcp::null point %x %x %x", pst_mac_vap, puc_data, pst_tcp_params);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���LLCͷ */
    hmac_encap_llc_hearder(puc_data,&us_frame_len);
    /* ���IPͷ */
    hmac_encap_ip_hearder(puc_data, (oal_uint8 *)pst_tcp_params, &us_frame_len, HMAC_NETBUF_TYPE_TCP);
    /* ���TCPͷ */
    hmac_encap_tcp_hearder(puc_data, pst_tcp_params, &us_frame_len);
    /* ������� */
    oal_memcopy(&puc_data[us_frame_len], pst_tcp_params->puc_tcp_payload, pst_tcp_params->ul_payload_len);
    us_frame_len += (oal_uint16)pst_tcp_params->ul_payload_len;
    return us_frame_len;
}
/*****************************************************************************
 �� �� ��  : hmac_encap_udp
 ��������  : ��װUDP֡
 �������  : pst_mac_vap: ��ǰ��Ӧ��vapָ��
             puc_data   : ���뵽���ڴ��׵�ַ
             pst_udp_params: UDP�����ڵ�ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint16 hmac_encap_udp(
                mac_vap_stru *pst_mac_vap,
                oal_uint8 *puc_data,
                hmac_udp_params_stru *pst_udp_params)
{
    oal_uint16 us_frame_len = 0;
    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_data) || (OAL_PTR_NULL == pst_udp_params))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "hmac_encap_udp::null point %x %x %x", pst_mac_vap, puc_data, pst_udp_params);
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* ���LLCͷ */
    hmac_encap_llc_hearder(puc_data,&us_frame_len);
    /* ���IPͷ */
    hmac_encap_ip_hearder(puc_data, (oal_uint8 *)pst_udp_params, &us_frame_len, HMAC_NETBUF_TYPE_UDP);
    /* ���UDPͷ */
    hmac_encap_udp_hearder(puc_data, pst_udp_params, &us_frame_len);
    /* ������� */
    oal_memcopy(&puc_data[us_frame_len], pst_udp_params->puc_udp_payload, pst_udp_params->ul_payload_len);
    us_frame_len += (oal_uint16)(pst_udp_params->ul_payload_len);
    return us_frame_len;
}

/*****************************************************************************
 �� �� ��  : hmac_tcp_udp_process
 ��������  : ����Ӧ�ò��·���TCP��UDP������������õİ��Ͳ����·���device��
 �������  : pst_mac_vap: ��ǰ��Ӧ��vapָ��
             us_len     : ��������ĳ���
             puc_param  : ����Ĳ���ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��4��21��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/

oal_uint32 hmac_tcp_udp_process(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    hmac_tcp_params_stru    *pst_host_tcp_params   = OAL_PTR_NULL;
    mac_tcp_params_stru      st_device_tcp_params;
    hmac_udp_params_stru    *pst_host_udp_params   = OAL_PTR_NULL;
    mac_udp_params_stru      st_device_udp_params;
    oal_uint16               us_frame_len;
    mac_tx_ctl_stru         *pst_tx_ctl            = OAL_PTR_NULL;
    oal_netbuf_stru         *pst_frame             = OAL_PTR_NULL;
    frw_event_mem_stru      *pst_event_mem;
    frw_event_stru          *pst_hmac_to_dmac_ctx_event;
    dmac_tx_event_stru      *pst_tx_event;
    oal_uint32               ul_ret;

    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_CFG, "hmac_config_set_tcp_udp_params::pst_mac_vap=%x,puc_param=%x",pst_mac_vap,puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* ����֡�ڴ� */
    pst_frame = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_frame)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_tcp_udp_process::NULL.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_MEM_NETBUF_TRACE(pst_frame, OAL_TRUE);
    OAL_MEMZERO(oal_netbuf_cb(pst_frame), OAL_NETBUF_CB_SIZE());

    OAL_NETBUF_PREV(pst_frame) = OAL_PTR_NULL;
    OAL_NETBUF_NEXT(pst_frame) = OAL_PTR_NULL;
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_frame);
    MAC_GET_CB_TX_USER_IDX(pst_tx_ctl)   = pst_mac_vap->uc_assoc_vap_id;
    mac_set_cb_tid(pst_tx_ctl, 0);
    mac_set_cb_is_amsdu(pst_tx_ctl, OAL_FALSE);

    /*ͨ�������������ж���TCP���Ӳ�������UDP���Ӳ���*/
    if (OAL_SIZEOF(hmac_tcp_params_stru) == us_len)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "hmac_tcp_udp_process::TCP params");
        pst_host_tcp_params   = (hmac_tcp_params_stru*)puc_param;
        /*����ת����dmac�㲻��Ҫplayload������ֻ��Ҫ���Ʋ������ݣ�ע����뱣֤�����ṹ���ڵĲ���˳����ͬ*/
        oal_memcopy(&st_device_tcp_params, pst_host_tcp_params, OAL_SIZEOF(mac_tcp_params_stru));
        us_frame_len = hmac_encap_tcp(pst_mac_vap, oal_netbuf_data(pst_frame), pst_host_tcp_params);
        oal_memcopy((oal_uint8 *)(oal_netbuf_data(pst_frame) + us_frame_len), (oal_uint8 *)&st_device_tcp_params, OAL_SIZEOF(mac_tcp_params_stru));
        oal_netbuf_put(pst_frame, (us_frame_len + OAL_SIZEOF(mac_tcp_params_stru)));
        pst_tx_ctl->us_mpdu_len = us_frame_len + OAL_SIZEOF(mac_tcp_params_stru);
        pst_event_mem           = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
        if (OAL_PTR_NULL == pst_event_mem)
        {
            OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_tcp_udp_process::null, pst_event_mem mem alloc failed.}");
            oal_netbuf_free(pst_frame);

            return OAL_ERR_CODE_PTR_NULL;
        }
        pst_hmac_to_dmac_ctx_event = (frw_event_stru *)pst_event_mem->puc_data;
        FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_ctx_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_TX_HOST_TCP_CRX,
                       OAL_SIZEOF(dmac_tx_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

        /* ��д�¼�payload */
        pst_tx_event = (dmac_tx_event_stru *)(pst_hmac_to_dmac_ctx_event->auc_event_data);

        pst_tx_event->pst_netbuf    = pst_frame;
        pst_tx_event->us_frame_len  = us_frame_len + OAL_SIZEOF(mac_tcp_params_stru);

        ul_ret = frw_event_dispatch_event(pst_event_mem);
        if (ul_ret != OAL_SUCC)
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp::send addba rsp failed, frw_event_dispatch_event failed[%d].}", ul_ret);
            oal_netbuf_free(pst_frame);
        }
        FRW_EVENT_FREE(pst_event_mem);

    }
    else if (OAL_SIZEOF(hmac_udp_params_stru) == us_len)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "hmac_tcp_udp_process::UDP params");

        pst_host_udp_params   = (hmac_udp_params_stru *)puc_param;
        if (OAL_PTR_NULL == pst_host_udp_params)
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "hmac_tcp_udp_process::pst_host_udp_params is null");
        }
        /*����ת����dmac�㲻��Ҫplayload������ֻ��Ҫ���Ʋ������ݣ�ע����뱣֤�����ṹ���ڵĲ���˳����ͬ*/
        oal_memcopy(&st_device_udp_params, pst_host_udp_params, OAL_SIZEOF(mac_udp_params_stru));
        us_frame_len = hmac_encap_udp(pst_mac_vap, oal_netbuf_data(pst_frame), pst_host_udp_params);
        oal_memcopy((oal_uint8 *)(oal_netbuf_data(pst_frame) + us_frame_len), (oal_uint8 *)&st_device_udp_params, OAL_SIZEOF(mac_udp_params_stru));
        oal_netbuf_put(pst_frame, (us_frame_len + OAL_SIZEOF(mac_udp_params_stru)));
        pst_tx_ctl->us_mpdu_len = us_frame_len + OAL_SIZEOF(mac_udp_params_stru);
        pst_event_mem           = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
        if (OAL_PTR_NULL == pst_event_mem)
        {
            OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_tcp_udp_process::null, pst_event_mem mem alloc failed.}");
            oal_netbuf_free(pst_frame);

            return OAL_ERR_CODE_PTR_NULL;
        }
        pst_hmac_to_dmac_ctx_event = (frw_event_stru *)pst_event_mem->puc_data;
        FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_ctx_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_TX_HOST_UDP_CRX,
                       OAL_SIZEOF(dmac_tx_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

        /* ��д�¼�payload */
        pst_tx_event = (dmac_tx_event_stru *)(pst_hmac_to_dmac_ctx_event->auc_event_data);
        pst_tx_event->pst_netbuf    = pst_frame;
        pst_tx_event->us_frame_len  = us_frame_len + OAL_SIZEOF(mac_tcp_params_stru);

        ul_ret = frw_event_dispatch_event(pst_event_mem);
        if (ul_ret != OAL_SUCC)
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp::send addba rsp failed, frw_event_dispatch_event failed[%d].}", ul_ret);
            oal_netbuf_free(pst_frame);
        }

        FRW_EVENT_FREE(pst_event_mem);
    }
    else
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hmac_tcp_udp_process:: params is wrong");
    }
    return OAL_SUCC;
}
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




