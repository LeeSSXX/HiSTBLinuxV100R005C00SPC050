/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : chr_errno.h
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2016��1��19��
  ����޸�   :
  ��������   : CHR�쳣���Ͷ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��1��19��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __CHR_ERRNO_H__
#define __CHR_ERRNO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/

/*****************************************************************************
  2 �궨��
*****************************************************************************/

#define CHR_SYSTEM_OFFSET                   (19)
#define CHR_LAYER_OFFSET                    (16)
#define CHR_EVENT_OFFSET                    (8)
#define CHR_ERROR_OFFSET                    (0)
#define CHR_SYSTEM_MASK                     (0x1F)
#define CHR_LAYER_MASK                      (0x07)
#define CHR_EVENT_MASK                      (0xFF)
#define CHR_ERROR_MASK                      (0xFF)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/****************************************************************************
                        CHR ERRNO ������ṹ
|---------------------------------------------------------------------------|
| 31~24 (8)bits| 23~19 (5)bits | 18~16 (3)bits | 15~8 (8)bits | 7~0 (8)bits |
|---------------------------------------------------------------------------|
|   Reserved    |    system     |     layer     |    event    |   error     |
|---------------------------------------------------------------------------|

|---------------------------------------------------------------------------|
|   λ��   |      ȡֵ       |                    ��;                      |
|---------------------------------------------------------------------------|
| Reserved |      ����       |                    ����                      |
|---------------------------------------------------------------------------|
| system   | CHR_SYSTEM_ENUM | ��ʶ�쳣��ϵͳ plat wifi gnss bt fm nfc ir   |
|---------------------------------------------------------------------------|
| layer    | CHR_LAYER_ENUM  | ��ʶ�쳣����λ��   dev      drv      hal     |
|---------------------------------------------------------------------------|
| event    |CHR_XX_EVENT_ENUM| ��ʶ�쳣�����¼�                             |
|---------------------------------------------------------------------------|
| error    |CHR_XX_ERROR_ENUM| ��ʶ�쳣��������,��ϸ���������쳣�¼���ԭ��  |
|---------------------------------------------------------------------------|
*****************************************************************************/

typedef enum
{
    CHR_SYSTEM_PLAT    = 0x01 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_WIFI    = 0x02 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_GNSS    = 0x03 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_BT      = 0x04 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_FM      = 0x05 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_NFC     = 0x06 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_IR      = 0x07 << CHR_SYSTEM_OFFSET,
    CHR_SYSTEM_MUTT    = 0x08 << CHR_SYSTEM_OFFSET,

}CHR_SYSTEM_ENUM;

typedef enum
{
    CHR_LAYER_DEV      = 0x01 << CHR_LAYER_OFFSET,
    CHR_LAYER_DRV      = 0x02 << CHR_LAYER_OFFSET,
    CHR_LAYER_HAL      = 0x03 << CHR_LAYER_OFFSET,

}CHR_LAYER_ENUM;

typedef enum
{
    /* CHR_WIFI_HAL */
    CHR_WIFI_HAL_EVENT_OPEN               = 0x01,
    CHR_WIFI_HAL_EVENT_SCAN               = 0x02,
    CHR_WIFI_HAL_EVENT_CONNECT            = 0x03,

    /* CHR_WIFI_DRV */
    CHR_WIFI_DRV_EVENT_OPEN               = 0x01,
    CHR_WIFI_DRV_EVENT_SCAN               = 0x02,
    CHR_WIFI_DRV_EVENT_CONNECT            = 0x03,
    CHR_WIFI_DRV_EVENT_DISCONNECT         = 0x04,
    CHR_WIFI_DRV_EVENT_CLOSE              = 0x05,
    CHR_WIFI_DRV_EVENT_EXCEPTION          = 0x06,
    CHR_WIFI_DRV_EVENT_FAKELINK           = 0x07,

    /* CHR_WIFI_DRV platform event */
    CHR_WIFI_DRV_EVENT_PLAT               = 0x08,

}CHR_WIFI_EVENT_ENUM;

typedef enum
{
    /* CHR_GNSS_HAL */
    CHR_GNSS_HAL_EVENT_SYSCALL            = 0x01,
    CHR_GNSS_HAL_EVENT_EXCEPTION          = 0x02,
    CHR_GNSS_HAL_EVENT_TIME               = 0x03,
    CHR_GNSS_HAL_EVENT_INJECT             = 0x04,

    /* CHR_GNSS_DRV platform event */
    CHR_GNSS_DRV_EVENT_PLAT               = 0x05,

}CHR_GNSS_EVENT_ENUM;

typedef enum
{
    /* CHR_WIFI_HAL_EVENT_OPEN */
    CHR_WIFI_HAL_ERROR_CONFIG_READ             = 0x01,
    CHR_WIFI_HAL_ERROR_SOCKET_FAIL             = 0x02,
    CHR_WIFI_HAL_ERROR_AP_CONFIG_SET_FAT       = 0x03,

    /* CHR_WIFI_HAL_EVENT_SCAN */
    CHR_WIFI_HAL_ERROR_SCAN_TIME_OUT           = 0x01,

    /* CHR_WIFI_HAL_EVENT_CONNECT */
    CHR_WIFI_HAL_ERROR_CMD_SEND_FAIL           = 0x01,
    CHR_WIFI_HAL_ERROR_CONNECT_FAIL            = 0x02,
    CHR_WIFI_HAL_ERROR_DISCONNECT_EVENT_RECV   = 0x03,
    CHR_WIFI_HAL_ERROR_WPS_FAIL                = 0x04,
    CHR_WIFI_HAL_ERROR_MODE_CHANGE_FAIL        = 0x05,

    /* CHR_WIFI_DRV_EVENT_OPEN */
    CHR_WIFI_DRV_ERROR_INSMOD_KO               = 0x01,
    CHR_WIFI_DRV_ERROR_SYS_VERSION             = 0x02,
    CHR_WIFI_DRV_ERROR_POWER_ON                = 0x03,
    CHR_WIFI_DRV_ERROR_CUSTOM_CALL             = 0x04,

    /* CHR_WIFI_DRV_EVENT_SCAN */
    CHR_WIFI_DRV_ERROR_SCAN_REFUSED            = 0x01,
    CHR_WIFI_DRV_ERROR_SCAN_TIMEOUT            = 0x02,

    /* CHR_WIFI_DRV_EVENT_CONNECT */
    CHR_WIFI_DRV_ERROR_CONNECT_CMD             = 0x01,
    CHR_WIFI_DRV_ERROR_AUTH_TIMEOUT            = 0x02,
    CHR_WIFI_DRV_ERROR_ASSOC_TIMEOUT           = 0x03,

    /* CHR_WIFI_DRV_EVENT_DISCONNECT */
    CHR_WIFI_DRV_ERROR_LINKLOSS                = 0x01,
    CHR_WIFI_DRV_ERROR_KEEPALIVE_TIMEOUT       = 0x02,
    CHR_WIFI_DRV_ERROR_CHANNEL_CHANGE          = 0x03,

    /* CHR_WIFI_DRV_EVENT_CLOSE */
    CHR_WIFI_DRV_ERROR_POWER_OFF               = 0x01,

    /* CHR_WIFI_DRV_EVENT_EXCEPTION */
    CHR_WIFI_DRV_ERROR_BEAT_HEART_TIMEOUT      = 0x01,
    CHR_WIFI_DRV_ERROR_WATCHDOG_TIMEOUT        = 0x02,
    CHR_WIFI_DRV_ERROR_WAKEUP_FAIL             = 0x03,
    CHR_WIFI_DRV_ERROR_DEVICE_PANIC            = 0x04,
    CHR_WIFI_DRV_ERROR_SDIO_TRANS_FAIL         = 0x05,

    /* CHR_WIFI_DRV_EVENT_FAKELINK */
    CHR_WIFI_DRV_ERROR_RX_NO_BUFFER            = 0x01,
    CHR_WIFI_DRV_ERROR_ARP_TX_FAIL             = 0x02,
    CHR_WIFI_DRV_ERROR_EAPOL_TX_FAIL           = 0x03,
    CHR_WIFI_DRV_ERROR_DHCP_TX_FAIL            = 0x04,
    CHR_WIFI_DRV_ERROR_RF_OVERHEAT_EXCEPTION   = 0x05,
    CHR_WIFI_DRV_ERROR_INTERFERENCE            = 0x06,

}CHR_WIFI_ERROR_ENUM;


typedef enum
{
    /*CHR_GNSS_HAL_EVENT_SYSCALL*/
    CHR_GNSS_HAL_ERROR_OPEN_CMD           = 0x01,
    CHR_GNSS_HAL_ERROR_READ_CMD           = 0x02,
    CHR_GNSS_HAL_ERROR_WRITE_CMD          = 0x03,
    CHR_GNSS_HAL_ERROR_FLUSH_CMD          = 0x04,
    CHR_GNSS_HAL_ERROR_CLOSE_CMD          = 0x05,
    CHR_GNSS_HAL_ERROR_CHMOD_CMD          = 0x06,
    CHR_GNSS_HAL_ERROR_SOCKET_CREATE_CMD  = 0x07,
    CHR_GNSS_HAL_ERROR_SOCKET_CONNECT_CMD = 0x08,
    CHR_GNSS_HAL_ERROR_PIPE_CREATE_CMD    = 0x09,
    CHR_GNSS_HAL_ERROR_EPOLL_REGISTER_CMD = 0x0A,
    CHR_GNSS_HAL_ERROR_EPOLL_HUP_CMD      = 0x0B,
    CHR_GNSS_HAL_ERROR_THREAD_CREATE_CMD  = 0x0C,

    /*CHR_GNSS_HAL_EVENT_EXCEPTION*/
    CHR_GNSS_HAL_ERROR_REBOOT_CMD         = 0x01,
    CHR_GNSS_HAL_ERROR_TIMEOUT_CMD        = 0x02,
    CHR_GNSS_HAL_ERROR_DATA_LOST_CMD      = 0x03,
    CHR_GNSS_HAL_ERROR_DATA_WRONG_CMD     = 0x04,
    CHR_GNSS_HAL_ERROR_ACK_LOST_CMD       = 0x05,

    /*CHR_GNSS_HAL_EVENT_TIME*/
    CHR_GNSS_HAL_ERROR_RTC_CMD            = 0x01,

    /*CHR_GNSS_HAL_EVENT_INJECT*/
    CHR_GNSS_HAL_ERROR_TIME_INJECT_CMD    = 0x01,
    CHR_GNSS_HAL_ERROR_LOC_INJECT_CMD     = 0x02,
    CHR_GNSS_HAL_ERROR_EPH_INJECT_CMD     = 0x03,

}CHR_GNSS_ERROR_ENUM;

typedef enum
{
    CHR_PLAT_DRV_ERROR_FIRMWARE_DOWN      = 0x01,
    CHR_PLAT_DRV_ERROR_SDIO_INIT          = 0x02,
    CHR_PLAT_DRV_ERROR_OPEN_UART          = 0x03,
    CHR_PLAT_DRV_ERROR_CFG_UART           = 0x04,
    CHR_PLAT_DRV_ERROR_OPEN_BCPU          = 0x05,
    CHR_PLAT_DRV_ERROR_BCPU_BOOTUP        = 0x06,
    CHR_PLAT_DRV_ERROR_CLOSE_BCPU         = 0x07,
    CHR_PLAT_DRV_ERROR_OPEN_WCPU          = 0x08,
    CHR_PLAT_DRV_ERROR_WCPU_BOOTUP        = 0x09,
    CHR_PLAT_DRV_ERROR_CLOSE_WCPU         = 0x0A,
    CHR_PLAT_DRV_ERROR_OPEN_THREAD        = 0x0B,
    CHR_PLAT_DRV_ERROR_CLOSE_THREAD       = 0x0C,
    CHR_PLAT_DRV_ERROR_WAKEUP_DEV         = 0x0D,
    CHR_PLAT_DRV_ERROR_RECV_LASTWORD      = 0x0E,
    CHR_PLAT_DRV_ERROR_BEAT_TIMEOUT       = 0x0F,

}CHR_PLAT_ERROR_ENUM;

/************************ CHR ERRNO �������λ����װ *************************/
#define CHR_COMBINE_EVENT_ERROR(event, error)   \
    (((event & CHR_EVENT_MASK) << CHR_EVENT_OFFSET) | ((error & CHR_ERROR_MASK) << CHR_ERROR_OFFSET))

#define CHR_PLAT_DEV(event,error) \
    (CHR_SYSTEM_PLAT | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_PLAT_DRV(event,error) \
    (CHR_SYSTEM_PLAT | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_PLAT_HAL(event,error) \
    (CHR_SYSTEM_PLAT | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

#define CHR_WIFI_DEV(event,error) \
    (CHR_SYSTEM_WIFI | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_WIFI_DRV(event,error) \
    (CHR_SYSTEM_WIFI | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_WIFI_HAL(event,error) \
    (CHR_SYSTEM_WIFI | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

#define CHR_GNSS_DEV(event,error) \
    (CHR_SYSTEM_GNSS | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_GNSS_DRV(event,error) \
    (CHR_SYSTEM_GNSS | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_GNSS_HAL(event,error) \
    (CHR_SYSTEM_GNSS | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

#define CHR_BT_DEV(event,error) \
    (CHR_SYSTEM_BT | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_BT_DRV(event,error) \
    (CHR_SYSTEM_BT | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_BT_HAL(event,error) \
    (CHR_SYSTEM_BT | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

#define CHR_FM_DEV(event,error) \
    (CHR_SYSTEM_FM | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_FM_DRV(event,error) \
    (CHR_SYSTEM_FM | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_FM_HAL(event,error) \
    (CHR_SYSTEM_FM | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

#define CHR_NFC_DEV(event,error) \
    (CHR_SYSTEM_NFC | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_NFC_DRV(event,error) \
    (CHR_SYSTEM_NFC | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_NFC_HAL(event,error) \
    (CHR_SYSTEM_NFC | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

#define CHR_IR_DEV(event,error) \
    (CHR_SYSTEM_IR | CHR_LAYER_DEV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_IR_DRV(event,error) \
    (CHR_SYSTEM_IR | CHR_LAYER_DRV | CHR_COMBINE_EVENT_ERROR(event, error))
#define CHR_IR_HAL(event,error) \
    (CHR_SYSTEM_IR | CHR_LAYER_HAL | CHR_COMBINE_EVENT_ERROR(event, error))

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

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of chr_errno.h */
