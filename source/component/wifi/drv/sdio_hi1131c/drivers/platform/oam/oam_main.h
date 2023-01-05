/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oam_main
  �� �� ��   : ����
  ��    ��   : ������
  ��������   : 2012��9��18��
  ����޸�   :
  ��������   : oam_main.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAM_MAIN_H__
#define __OAM_MAIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION))
/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "oal_types.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"

#ifdef _PRE_WIFI_DMT
#include "dmt_stub.h"
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define OAM_BEACON_HDR_LEN              24
#define OAM_TIMER_MAX_LEN               36
#define OAM_PRINT_CRLF_NUM              20              /* ������з��ĸ��� */

#define OAM_FEATURE_NAME_ABBR_LEN            (12)           /* ����������д��󳤶� */

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
#define OAM_DATA2SDT_FAIL_COUNT     1000
#endif

/* ��ӡ��ʽ���� */
typedef oal_uint32  (* oal_print_func)(oal_int8 *pc_string);

/* ������ݸ�ʽ���� */
typedef oal_int8*   oam_va_list;

/*****************************************************************************
  3 ö�ٶ���
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
/* ��־�����б�ṹ�嶨�� */
typedef struct
{
    oam_feature_enum_uint8              en_feature_id;
    oal_uint8                           auc_feature_name_abbr[OAM_FEATURE_NAME_ABBR_LEN]; /* ��0���� */
}oam_software_feature_stru;

/* ÿ��ul_interval������ul_burst����־��� */
typedef enum
{
    OAM_RATELIMIT_OUTPUT        = 0,
    OAM_RATELIMIT_NOT_OUTPUT,
}oam_ratelimit_output_enum;
typedef oal_uint8   oam_ratelimit_output_enum_uint8;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
//extern oam_mng_ctx_stru             g_st_oam_mng_ctx;
extern oam_software_feature_stru    gst_oam_feature_list[];

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern oal_int32  oam_main_init(oal_void);
extern oal_void   oam_main_exit(oal_void);
extern oal_uint32   oam_send_device_data2sdt(oal_uint8* pc_string, oal_uint16 len);
extern oal_uint32  oam_set_file_path(oal_int8 *pc_file_path, oal_uint32 ul_length);
extern oal_uint32  oam_set_output_type(oam_output_type_enum_uint8 en_output_type);
extern oal_uint32  oam_print(oal_int8 *pc_string);
extern oal_uint32  oam_print_to_file(oal_int8 *pc_string);
extern oal_uint32  oam_print_to_sdt(oal_int8 *pc_string);
extern oal_uint32  oam_print_to_console(oal_int8 *pc_string);
extern oal_uint32  oam_upload_log_to_sdt(oal_int8 *pc_string);
extern oal_void oam_dump_buff_by_hex(oal_uint8 *puc_buff, oal_int32 l_len, oal_int32 l_num);
extern oam_ratelimit_output_enum_uint8 oam_log_ratelimit(oam_ratelimit_type_enum_uint8 en_ratelimit_type );

#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "oal_types.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"

#ifdef _PRE_WIFI_DMT
#include "dmt_stub.h"
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define OAM_BEACON_HDR_LEN              24
#define OAM_TIMER_MAX_LEN               36
#define OAM_PRINT_CRLF_NUM              20              /* ������з��ĸ��� */

#define OAM_FEATURE_NAME_ABBR_LEN            (12)           /* ����������д��󳤶� */

#define LOG_NAME_LEN    20
#define LOG_OAM_PATH_LEN    30
#define WIFI_OAM_PREFIX    "wifi_oam_log_"
#define WIFI_OAM_PATH       "/jffs0/data/oam_wifi/"

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
#define OAM_DATA2SDT_FAIL_COUNT     1000
#endif

/* ��ӡ��ʽ���� */
typedef oal_uint32  (* oal_print_func)(oal_int8 *pc_string);

/* ������ݸ�ʽ���� */
typedef oal_int8*   oam_va_list;

/*�ȴ����¼�����*/
#define oam_event_wait_bit0  1<<0
/*�¼����ƽṹ��*/
extern EVENT_CB_S oam_event;

/*****************************************************************************
  3 ö�ٶ���
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
/*������־�����ṹ��*/
typedef struct oam_log_save
{
    FILE*   pst_fd;                             /* ��ǰ����������־�ļ�fd*/
    oal_int8    ac_prefix[LOG_NAME_LEN];            /* ��־�ļ�ǰ׺����"wifi_log_"*/
    oal_int32 st_size;               /*ÿ��������־��С(��kbΪ��λ)*/
    oal_int32 st_num;                /* ������־������*/
    oal_int8  st_path[LOG_OAM_PATH_LEN];               /* log config path in *.ini file */
    oal_int32   l_curr_num;                           /* log file num in curr path */
    oal_int8    ac_file[LOG_OAM_PATH_LEN + LOG_NAME_LEN + 1]; /* abs path log file */
}OAM_LOG_LUNTI;

typedef unsigned int	uint;
typedef struct oam_rx_sdt_cmd
{
    int (*uart_rx_sdt_cmd_dispose)(char* ch, uint ch_cnt);
}oam_rx_sdt_cmd_func_hook_stru;

extern oam_rx_sdt_cmd_func_hook_stru g_oam_uart_rx_sdt_cmd_func_hook;

/* ��־�����б�ṹ�嶨�� */
typedef struct
{
    oam_feature_enum_uint8              en_feature_id;
    oal_uint8                           auc_feature_name_abbr[OAM_FEATURE_NAME_ABBR_LEN]; /* ��0���� */
}oam_software_feature_stru;

/* ÿ��ul_interval������ul_burst����־��� */
typedef enum
{
    OAM_RATELIMIT_OUTPUT        = 0,
    OAM_RATELIMIT_NOT_OUTPUT,
}oam_ratelimit_output_enum;
typedef oal_uint8   oam_ratelimit_output_enum_uint8;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
//extern oam_mng_ctx_stru             g_st_oam_mng_ctx;
extern oam_software_feature_stru    gst_oam_feature_list[];

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern oal_int32  oam_main_init(oal_void);
extern oal_void   oam_main_exit(oal_void);
extern oal_uint32   oam_send_device_data2sdt(oal_uint8* pc_string, oal_uint16 len);
extern oal_uint32  oam_set_file_path(oal_int8 *pc_file_path, oal_uint32 ul_length);
//extern oal_uint32  oam_set_output_type(oam_output_type_enum_uint8 en_output_type);
extern oal_uint32  oam_print(oal_int8 *pc_string);
extern oal_uint32  oam_print_to_file(oal_int8 *pc_string);
extern oal_uint32  oam_print_to_sdt(oal_int8 *pc_string);
extern oal_uint32  oam_print_to_console(oal_int8 *pc_string);
extern oal_uint32  oam_upload_log_to_sdt(oal_int8 *pc_string);
extern oal_void oam_dump_buff_by_hex(oal_uint8 *puc_buff, oal_int32 l_len, oal_int32 l_num);
extern oam_ratelimit_output_enum_uint8 oam_log_ratelimit(oal_uint8 en_ratelimit_type );
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oam_main */
