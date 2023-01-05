/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : dmac_tcp_udp_keepalive.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��4��12��
  ����޸�   :
  ��������   : dmac_tcp_udp_keepalive.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��4��12��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __DMAC_TCP_UDP_KEEPALIVE_H__
#define __DMAC_TCP_UDP_KEEPALIVE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "mac_vap.h"
#include "dmac_vap.h"
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_DMAC_TCP_UDP_KEEPALIVE_H

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define DMAC_TCP_CONNECT_NUM  4   /* ����TCP�������� */
#define DMAC_UDP_CONNECT_NUM  4   /* ����UDP�������� */

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
typedef enum
{
    DMAC_TCP_OFFSET_SEQ_NUM  = 4, /* TCP��seq num�ֶδ�TCPͷ�ײ�ƫ��4�ֽ� */
    DMAC_TCP_OFFSET_ACK_NUM  = 8, /* TCP��ack num�ֶδ�TCPͷ�ײ�ƫ��8�ֽ� */
    DMAC_TCP_OFFSET_CHECKSUM = 12,/* TCP��У����ֶδ�TCPͷ�ײ�ƫ��12�ֽ� */
    DMAC_TCP_OFFSET_BUTT
}dmac_tcp_offset_enum;
typedef oal_uint8 dmac_tcp_offset_enum_uint8;

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
    mac_tcp_params_stru           *pst_tcp_params;        /* TCP���ӵĲ�����Ϣ */
    oal_netbuf_stru               *pst_tcp_netbuf;        /* ��Ҫ���͵�TCP������,������MACͷ */
    mac_vap_stru                  *pst_mac_vap;           /* ��Ӧ��VAP */
    oal_uint32                     ul_payload_len;        /* ��������payload���� */ 
    oal_uint32                     ul_tcp_wait_time;      /* ���ϴη�����������ʱ�� */
    oal_uint16                     us_retry_count;        /* �Ѿ��ش�����*/
    oal_uint8                      uc_get_ack_flag;       /* �Ƿ���յ�TCP ACK���յ���ֵΪ1��û�յ���ֵΪ0 */
    oal_uint8                      auc_resv[1];
}dmac_tcp_conn_stru;

typedef struct
{
    mac_udp_params_stru            *pst_udp_params;        /* UDP���ӵĲ�����Ϣ */
    oal_netbuf_stru                *pst_udp_netbuf;       /* ��Ҫ���͵�UDP������ */
    oal_netbuf_stru                *pst_udp_ack_netbuf;   /* �豸�յ���������UDP����ظ���UDP��Ӧ�� */
    mac_vap_stru                   *pst_mac_vap;          /* ��Ӧ��VAP */    
    oal_uint32                      ul_payload_len;       /* ��������payload���� */ 
    oal_uint32                      ul_udp_wait_time;     /* ���ϴη�����������ʱ�� */
    oal_uint16                      us_retry_count;        /* �Ѿ��ش����� */
    oal_uint8                       uc_get_ack_flag;       /* �Ƿ���յ�UDP��Ӧ����1���յ���0û�н��յ� */
    oal_uint8                       auc_resv[1];
}dmac_udp_conn_stru;

typedef struct
{
    dmac_tcp_conn_stru        st_tcp_connect[DMAC_TCP_CONNECT_NUM];      /* TCP������ز���ָ�� */
    dmac_udp_conn_stru        st_udp_connect[DMAC_UDP_CONNECT_NUM];      /* UDP������ز���ָ�� */
    oal_uint8                 uc_tcp_conn_num;      /* �����TCP���Ӹ��� */
    oal_uint8                 uc_udp_conn_num;      /* �����UDP���Ӹ��� */
    oal_uint8                 uc_keepalive_switch;  /* TCP&UDP keepalive���Կ��� */
    oal_uint8                 auc_resv[1];   
}dmac_tcp_udp_conn_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/
typedef enum
{
    DMAC_NETBUF_TYPE_TCP,
    DMAC_NETBUF_TYPE_UDP,
    DMAC_NETBUF_TYPE_BUTT
}dmac_netbuf_type_enum;
typedef oal_uint8 dmac_netbuf_type_enum_uint8;


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern oal_uint32 dmac_rx_tcp_ack(oal_netbuf_stru *pst_netbuf);
extern oal_uint32 dmac_rx_udp_ack(oal_netbuf_stru *pst_netbuf);
extern oal_uint32 dmac_tcp_udp_conn_process(dmac_vap_stru *pst_dmac_vap);
extern oal_uint32 dmac_tx_process_tcp_event(frw_event_mem_stru *pst_event_mem);
extern oal_uint32 dmac_tx_process_udp_event(frw_event_mem_stru *pst_event_mem);
extern oal_uint32 dmac_set_tcp_seq_num(oal_uint8 *puc_netbuf, oal_uint32 ul_seq_num);
extern oal_uint32 dmac_destory_tcp_udp_connects(oal_void);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif



