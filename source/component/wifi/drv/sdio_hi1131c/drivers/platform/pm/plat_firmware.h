/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : plat_firmware.h
  �� �� ��   : ����
  ��    ��   :
  ��������   :
  ����޸�   :
  ��������   : plat_firmware.cͷ�ļ�
  �����б�   :

  �޸���ʷ   :
  1.��    ��   :
    ��    ��   :
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __PLAT_FIRMWARE_H__
#define __PLAT_FIRMWARE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "plat_type.h"
#include "oal_net.h"
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "exception_rst.h"
#endif
/*****************************************************************************
  2 �궨��
*****************************************************************************/
    
#define CMD_JUMP_EXEC_RESULT_SUCC   (0)
#define CMD_JUMP_EXEC_RESULT_FAIL   (1)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
    
enum FIRMWARE_CFG_FILE_ENUM
{
    WIFI_CFG = 0,                      /*ֻ����wifi firmware����������������ִ�ж���У׼*/
    RAM_REG_TEST_CFG,              /*���߲���mem reg����ʹ��*/
    CFG_FILE_TOTAL
};

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

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
extern int32 firmware_download(uint32 ul_index);
extern int32 plat_firmware_init(void);
extern int32 plat_firmware_clear(void);
extern int32 device_mem_dump(p_st_wifi_demp_mem_info * pst_mem_dump_info);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "stdlib.h"
#include "exception_rst.h"
/*****************************************************************************
  2 �궨��
*****************************************************************************/

/* string */
#define OS_MEM_SET(dst, data, size)         memset(dst, data, size)
#define OS_MEM_CPY(dst, src, size)          memcpy(dst, src, size)
#define OS_MEM_CMP(dst, src, size)          memcmp(dst, src, size)
#define OS_STR_CMP(dst, src)                strcmp(dst, src)
#define OS_STR_LEN(s)                       strlen(s)
#define OS_STR_CHR(str, chr)                strchr(str, chr)
#define OS_STR_STR(dst, src)                strstr(dst, src)

#define SKB_DATA_ALIGN(X)                   ALIGN(X, 32)

/* memory */
#define OS_MEM_KFREE(p)                     free(p)
#define OS_KMALLOC_GFP(size)                memalign(32, SKB_DATA_ALIGN(size))
#define OS_VMALLOC_GFP(size)                malloc(size)
#define OS_MEM_VFREE(p)                     free(p)

#define READ_MEG_TIMEOUT            (200)      /* 200ms */
#define READ_MEG_JUMP_TIMEOUT       (100000)   /* 100s */

#define FILE_CMD_WAIT_TIME_MIN      (5000)     /* 5000us */
#define FILE_CMD_WAIT_TIME_MAX      (5100)     /* 5100us */


#define SEND_BUF_LEN                (520)
#define RECV_BUF_LEN                (512)
#define VERSION_LEN                 (64)
#define CMD_BUFF_LEN                (256)

#define READ_CFG_BUF_LEN            (2048) 

#define DOWNLOAD_CMD_LEN            (32)
#define DOWNLOAD_CMD_PARA_LEN       (800)

#define HOST_DEV_TIMEOUT            (3)
#define INT32_STR_LEN               (32)

#define SHUTDOWN_TX_CMD_LEN         (64)

#define CMD_JUMP_EXEC_RESULT_SUCC   (0)
#define CMD_JUMP_EXEC_RESULT_FAIL   (1)

#define SOFT_WCPU_EN_ADDR           "0x50000c00"
#define SOFT_BCPU_EN_ADDR           "0x50000c04"
#define BCPU_DE_RESET_ADDR          "0x50000094"

/*�����Ƿ���device����Ĺؼ���*/
#define VER_CMD_KEYWORD             "VERSION"
#define JUMP_CMD_KEYWORD            "JUMP"
#define FILES_CMD_KEYWORD           "FILES"
#define SETPM_CMD_KEYWORD           "SETPM"
#define SETBUCK_CMD_KEYWORD         "SETBUCK"
#define SETSYSLDO_CMD_KEYWORD       "SETSYSLDO"
#define SETNFCRETLDO_CMD_KEYWORD    "SETNFCRETLDO"
#define SETPD_CMD_KEYWORD           "SETPD"
#define SETNFCCRG_CMD_KEYWORD       "SETNFCCRG"
#define SETABB_CMD_KEYWORD          "SETABB"
#define SETTCXODIV_CMD_KEYWORD      "SETTCXODIV"
#define RMEM_CMD_KEYWORD            "READM"
#define WMEM_CMD_KEYWORD            "WRITEM"
#define QUIT_CMD_KEYWORD            "QUIT"

/*���������ֲ��ᷢ��device�����ڿ���У׼���̣����ǻ�������cfg�ļ���*/
#define CALI_COUNT_CMD_KEYWORD      "CALI_COUNT"
#define CALI_BFGX_DATA_CMD_KEYWORD  "CALI_BFGX_DATA"
#define SHUTDOWN_WIFI_CMD_KEYWORD   "SHUTDOWN_WIFI"
#define SHUTDOWN_BFGX_CMD_KEYWORD   "SHUTDOWN_BFGX"

/*������device������ִ�гɹ����صĹؼ��֣�host�յ�һ�¹ؼ���������ִ�гɹ�*/
#define MSG_FROM_DEV_WRITEM_OK      "WRITEM OK"
#define MSG_FROM_DEV_READM_OK       ""
#define MSG_FROM_DEV_FILES_OK       "FILES OK"
#define MSG_FROM_DEV_READY_OK       "READY"
#define MSG_FROM_DEV_JUMP_OK        "JUMP OK"
#define MSG_FROM_DEV_SET_OK         "SET OK"
#define MSG_FROM_DEV_QUIT_OK        ""

/*������cfg�ļ���������Ĳ���ͷ��һ���Ϸ���cfg�����ʽΪ:����ͷ+����ؼ���(QUIT�������)*/
#define FILE_TYPE_CMD_KEY           "ADDR_FILE_"
#define NUM_TYPE_CMD_KEY            "PARA_"

#define COMPART_KEYWORD             ' '
#define CMD_LINE_SIGN               ';'



#define HISDIO_BLOCK_SIZE              (512) 

#define HISDIO_ALIGN_4_OR_BLK(len)     ((len) < HISDIO_BLOCK_SIZE ? ALIGN((len), 4) : ALIGN((len), HISDIO_BLOCK_SIZE))

#define HIUSB_ALIGN_32(len)             ( ALIGN((len), 32) )

#ifndef _PRE_FEATURE_USB
#define HISI_CHANNEL_ALIGN(len)        HISDIO_ALIGN_4_OR_BLK(len)
#else
#define HISI_CHANNEL_ALIGN(len)        HIUSB_ALIGN_32(len)
#endif

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

enum FIRMWARE_CFG_CMD_ENUM
{
    ERROR_TYPE_CMD = 0,            /* ��������� */
    FILE_TYPE_CMD,                 /* �����ļ������� */
    NUM_TYPE_CMD,                  /* �������ò��������� */
    QUIT_TYPE_CMD,                 /* �˳����� */
    SHUTDOWN_WIFI_TYPE_CMD,        /* SHUTDOWN WCPU���� */
};

enum FIRMWARE_CFG_FILE_ENUM
{
    WIFI_CFG = 0, 
    CFG_FILE_TOTAL
};

enum FIRMWARE_SUBSYS_ENUM
{
    DEV_WCPU = 0,
    DEV_BCPU,
    DEV_CPU_BUTT,
};


/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/



/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

extern uint8_t *g_aus_recv_cmd_buff;
extern uint8_t *g_aus_send_cmd_buff;

/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
typedef struct cmd_type_st
{
    int32_t       cmd_type;
    uint8_t       cmd_name[DOWNLOAD_CMD_LEN];
    uint8_t       cmd_para[DOWNLOAD_CMD_PARA_LEN];

}CMD_TYPE_STRUCT;

typedef struct firmware_globals_st
{
    int32_t            al_count[CFG_FILE_TOTAL];      /*�洢ÿ��cfg�ļ���������Ч���������*/
    CMD_TYPE_STRUCT *apst_cmd[CFG_FILE_TOTAL];      /*�洢ÿ��cfg�ļ�����Ч����*/
    uint8_t            auc_CfgVersion[VERSION_LEN];   /*�洢cfg�ļ������õİ汾����Ϣ*/
    uint8_t            auc_DevVersion[VERSION_LEN];   /*�洢����ʱdevice���ϱ��İ汾����Ϣ*/
}FIRMWARE_GLOBALS_STRUCT;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/
typedef int OS_KERNEL_FILE_STRU;

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern int32_t firmware_download(uint32_t ul_index);
extern int32_t firmware_cfg_init(void);
extern int32_t firmware_cfg_clear(void);
extern int32_t sdio_device_mem_dump(struct st_wifi_dump_mem_info *pst_mem_dump_info, uint32_t count);
extern uint32_t usb_max_req_size(void);

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of plat_firmware.h */
