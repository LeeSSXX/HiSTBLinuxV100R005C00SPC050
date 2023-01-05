/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : hmac_tcp_udp_keepalive.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��4��18��
  ����޸�   :
  ��������   : hmac_tcp_udp_keepalive.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��4��18��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __HMAC_TCP_UDP_KEEPALIVE_H__
#define __HMAC_TCP_UDP_KEEPALIVE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "mac_vap.h"
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_TCP_UDP_KEEPALIVE_H

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define HMAC_TCP_CONNECT_NUM  4   /* ����TCP�������� */
#define HMAC_UDP_CONNECT_NUM  4   /* ����UDP�������� */


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
typedef enum
{
    HMAC_NETBUF_TYPE_TCP,
    HMAC_NETBUF_TYPE_UDP,
    HMAC_NETBUF_TYPE_BUTT
}hmac_netbuf_type_enum;
typedef oal_uint8 hmac_netbuf_type_enum_uint8;


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
typedef struct
{
    oal_uint32      ul_sess_id;
    oal_uint8       uc_dst_mac[6];  /* Ŀ��MAC��ַ */
    oal_uint8       auc_resv[2];
    oal_uint8       uc_src_ip[4];   /* ԴIP��ַ */
    oal_uint8       uc_dst_ip[4];   /* Ŀ��IP��ַ */
    oal_uint16      us_src_port;    /* Դ�˿ں� */
    oal_uint16      us_dst_port;    /* Ŀ�Ķ˿ں� */
    oal_uint32      ul_seq_num;     /* ���к� */
    oal_uint32      ul_ack_num;     /* ȷ�Ϻ� */
    oal_uint16      us_window;      /* TCP���ڴ�С */
    oal_uint16      us_interval_timer;       /* �������������� */
    oal_uint16      us_retry_interval_timer; /* �ش�ʱ�������������� */
    oal_uint16      us_retry_max_count;      /* ����ش����� */
    oal_uint32      ul_time_value;
    oal_uint32      ul_time_echo;
    oal_uint8      *puc_tcp_payload;
    oal_uint32      ul_payload_len;
}hmac_tcp_params_stru;

typedef struct
{
    oal_uint32     ul_sess_id;
    oal_uint8      uc_dst_mac[6];    /* Ŀ��MAC��ַ */
    oal_uint8      auc_resv[2];
    oal_uint8      uc_src_ip[4];     /* ԴIP��ַ */
    oal_uint8      uc_dst_ip[4];     /* Ŀ��IP��ַ */
    oal_uint16     us_src_port;      /* Դ�˿ں� */
    oal_uint16     us_dst_port;      /* Ŀ�Ķ˿ں� */
    oal_uint16     us_interval_timer;       /* �������������� */
    oal_uint16     us_retry_interval_timer; /* �ش�ʱ�������������� */
    oal_uint16     us_retry_max_count;      /* ����ش����� */
    oal_uint32     ul_time_value;
    oal_uint32     ul_time_echo;
    oal_uint8     *puc_udp_payload;
    oal_uint8      ul_payload_len;
}hmac_udp_params_stru;

typedef struct
{
    hmac_tcp_params_stru st_tcp_connects[HMAC_TCP_CONNECT_NUM];
    hmac_udp_params_stru st_udp_connects[HMAC_UDP_CONNECT_NUM];
}hmac_tcp_udp_conn_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern oal_uint16 hmac_encap_tcp(mac_vap_stru *pst_mac_vap, oal_uint8 *puc_data, hmac_tcp_params_stru *pst_tcp_params);
extern oal_uint16 hmac_encap_udp(mac_vap_stru *pst_mac_vap, oal_uint8 *puc_data, hmac_udp_params_stru *pst_udp_params);
extern oal_uint32 hmac_tcp_udp_process(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
extern oal_uint32 hmac_get_tcp_params(oal_netbuf_stru *pst_netbuf);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif




