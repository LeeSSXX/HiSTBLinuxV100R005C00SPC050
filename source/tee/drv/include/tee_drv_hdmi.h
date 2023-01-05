#ifndef _TEE_DRV_HDMI_H__
#define _TEE_DRV_HDMI_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cpluscplus */


#include "hi_type.h"


#define HDMI_HDCP_SRM_MAX_DEVID              128     /**<Max number of Device IDs*/
#define HDMI_HDCP_SRM_DEVID_LEN              5       /**<Each device id length*/
#define HDMI_HDCP_MAX_REPEAT_DEV_NUM         32

typedef enum hiHI_DRV_HDMI_HDCP_STREAM_TYPE_E
{
    HI_DRV_HDMI_HDCP_STREAM_TYPE_0 = 0,        /**< Type 0 Content Stream. May be transmitted by the HDCP Repeater to all HDCP Devices. */
    HI_DRV_HDMI_HDCP_STREAM_TYPE_1,            /**< Type 1 Content Stream(Default value). Must not be transmitte by the HDCP Repeater to HDCP 1.x-complian Devices and HDCP 2.0-compliant Repeaters */
    HI_DRV_HDMI_HDCP_STREAM_TYPE_BUTT,
}HI_DRV_HDMI_HDCP_STREAM_TYPE_E;

typedef enum hiDRV_HDMI_HDCP_LEVEL_E
{
    HI_DRV_HDMI_HDCP_LEVEL_NONE = 0,        /** no hdcp require */
    HI_DRV_HDMI_HDCP_LEVEL_1,               /** hdcp level 1 (hdcp1.4 or hdcp2.2) */
    HI_DRV_HDMI_HDCP_LEVEL_2,               /** hdcp level 2 (only hdcp2.2) */
    HI_DRV_HDMI_HDCP_LEVEL_BUTT
}HI_DRV_HDMI_HDCP_LEVEL_E;


typedef enum hiDRV_HDMI_HDCP_STATUS_E
{
    HI_DRV_HDMI_HDCP_NONE = 0,       /** no hdcp is working     */
    HI_DRV_HDMI_HDCP_14_SUCC,        /** HDCP1.4 auth success   */
    HI_DRV_HDMI_HDCP_14_FAIL,        /** HDCP1.4 auth fail      */
    HI_DRV_HDMI_HDCP_22_SUCC,        /** HDCP2.2 auth success   */
    HI_DRV_HDMI_HDCP_22_FAIL,        /** HDCP2.2 auth fail   */
    HI_DRV_HDMI_HDCP_BUTT
} HI_DRV_HDMI_HDCP_STATUS_E;

typedef enum hiDRV_HDMI_DEVICE_ID_E
{
    HI_DRV_HDMI_DEVICE_0,            /* HDMI device id 0 */
    HI_DRV_HDMI_DEVICE_BUTT,
}HI_DRV_HDMI_DEVICE_ID_E;

typedef struct hiDRV_HDMI_HDCP_CAPS_S
{
    HI_BOOL             bHdcp14Support;
    HI_BOOL             bHdcp22Support;
}HI_DRV_HDMI_HDCP_CAPS_S;

typedef struct
{
    HI_BOOL         bHotPlug;
    HI_U32          u32DownScalW;
    HI_U32          u32DownScalH;
}HI_DRV_HDMI_HADRWARE_STATUS_S;


typedef struct
{
    HI_U8  u8DevId[HDMI_HDCP_SRM_DEVID_LEN];

}HDMI_HDCP_DEVID_S;

typedef struct
{
    HI_U32                  u32RepeatDevNum;
    HDMI_HDCP_DEVID_S       stRepeatIDList[HDMI_HDCP_MAX_REPEAT_DEV_NUM];
}HDMI_HDCP_REPEAT_IDLIST_S;

typedef enum
{
    HI_DRV_HDMI_ID_CHK_NONE,     /* devid or id list is not check */
    HI_DRV_HDMI_ID_CHK_PASS,     /* devid or id list check pass */
    HI_DRV_HDMI_ID_CHK_FAIL,     /* devid or id list check failed */
    HI_DRV_HDMI_ID_CHK_BUTT,
}HI_DRV_HDMI_ID_CHK_STATUS_E;


typedef struct
{
    HI_DRV_HDMI_ID_CHK_STATUS_E enIdChkStatus;
    HDMI_HDCP_DEVID_S           stDevId;
    HI_DRV_HDMI_ID_CHK_STATUS_E enIdListChkStatus;
    HDMI_HDCP_REPEAT_IDLIST_S   stRepeatId;
}HDMI_HDCP_DEVID_INFO_S;

typedef struct
{
    HI_BOOL                 bHDCP1xAudioMute;
    HI_BOOL                 bHDCP1xVideoMute;
    HI_BOOL                 bHDCP2xAudioMute;
    HI_BOOL                 bHDCP2xVideoMute;
}HDMI_HDCP_MUTE_STATUS_S;

typedef struct hiTEE_DRV_HDMI_SRM_S
{
	  HI_U8 *pu8SrmData;		/**<SRM raw data */
	  HI_U32 u32SrmLen;			/**<Length of SRM */
} HI_TEE_DRV_HDMI_SRM_S;

// support for tee display TA
/**
\brief set srm info.     CNcomment��HDCP2.2��ʼ�����
\param[in]  enHdmiId     hdmi channel id.
\param[in]  pSrm         srm info.
\retval     HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_SetSRM(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_TEE_DRV_HDMI_SRM_S *pstSrm);

/**
\brief set stream type.  CNcomment���������ݱ�������
\param[in]  enHdmiId     hdmi channel id.
\param[in]  enStreamType stream type.
\retval     HI_SUCCESS   success.
*/
HI_S32 DRV_HDMI_SetHDCPStreamType(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_DRV_HDMI_HDCP_STREAM_TYPE_E enStreamType);

/**
\brief set hdcp stratagey. CNcomment������HDCP����
\param[in]  enHdmiId        hdmi channel id.
\param[in]  enHdcpLevel     hdcp level, hdcp2.2 only or hdcpx or non-hdcp.
\retval     HI_SUCCESS      success.
\note: for drv
*/
HI_S32 DRV_HDMI_SetHDCPStrategy(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_DRV_HDMI_HDCP_LEVEL_E enHdcpLevel);


/**
\brief set hdcp stratagey. CNcomment������HDCP����
\param[in]  enHdmiId        hdmi channel id.
\param[in]  enHdcpLevel     hdcp level, hdcp2.2 only or hdcpx or non-hdcp.
\retval     HI_SUCCESS      success.
\note: for ca/ta
*/
HI_S32 HI_DRV_HDMI_SetHDCPStrategy(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_DRV_HDMI_HDCP_LEVEL_E enHdcpLevel);

/**
\brief get current hdcp stratagey. CNcomment����ȡ��ǰHDCP����
\param[in]  enHdmiId         hdmi channel id.
\param[in]  penHdcpLevel     current hdcp level, hdcp2.2 only or hdcpx or non-hdcp.
\retval HI_SUCCESS   success.
\note: for drv
*/
HI_S32 DRV_HDMI_GetHDCPStrategy(HI_DRV_HDMI_DEVICE_ID_E enHdmiId, HI_DRV_HDMI_HDCP_LEVEL_E *penHdcpLevel);


/**
\brief get current hdcp stratagey. CNcomment����ȡ��ǰHDCP����
\param[in]  enHdmiId         hdmi channel id.
\param[in]  penHdcpLevel     current hdcp level, hdcp2.2 only or hdcpx or non-hdcp.
\retval HI_SUCCESS   success.
\note: for ca/ta
*/
HI_S32 HI_DRV_HDMI_GetHDCPStrategy(HI_DRV_HDMI_DEVICE_ID_E enHdmiId, HI_DRV_HDMI_HDCP_LEVEL_E *penHdcpLevel);



/**
\brief get hdcp status. CNcomment����ȡHDCP��֤״̬
\param[in]  enHdmiId      hdmi channel id.
\param[in]  pEnHdcp22     hdcp2.2 authenticate status
\param[in]  pEnHdcp14     hdcp1.4 authenticate status
\retval     HI_SUCCESS    success.
\note: for drv
*/

HI_S32 DRV_HDMI_GetHDCPStatus(HI_DRV_HDMI_DEVICE_ID_E enHdmiId, HI_DRV_HDMI_HDCP_STATUS_E *penHdcpStatus);

/**
\brief get hdcp status. CNcomment����ȡHDCP��֤״̬
\param[in]  enHdmiId      hdmi channel id.
\param[in]  pEnHdcp22     hdcp2.2 authenticate status
\param[in]  pEnHdcp14     hdcp1.4 authenticate status
\retval     HI_SUCCESS    success.
\note: for ca/ta
*/
HI_S32 HI_DRV_HDMI_GetHDCPStatus(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_DRV_HDMI_HDCP_STATUS_E *penHdcpStatus);


/**
\brief set down scale. CNcomment�����ü�طֱ���
\param[in] enHdmiId     hdmi channel id.
\param[in] u32DownScalWidth  scale width
\param[in] u32DownScalHeight scale height
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_SetDownScal(HI_DRV_HDMI_DEVICE_ID_E enHdmiId, HI_U32 u32DownScalWidth,HI_U32 u32DownScalHeight);

/**
\brief get hdcp status. CNcomment������HDCP Mute
\param[in]  enHdmiId     hdmi channel id.
\param[in]  bMute        mute command
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_SetHDCPMute(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_BOOL bMute);

// support for tee hdmi TA
/**
\brief get hdcp status. CNcomment��HDCP2.2��ʼ��
\param[in]  enHdmiId         hdmi channel id.
\param[in]  pstSrmIdList     SRM ID List.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_HDCP22_Init(HI_DRV_HDMI_DEVICE_ID_E enHdmiId);

/**
\brief get hdcp status. CNcomment��HDCP2.2 SRMУ��
\param[in]  enHdmiId     hdmi channel id.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_HDCP22_SrmVerify(HI_DRV_HDMI_DEVICE_ID_E enHdmiId);

/**
\brief view tee status. CNcomment��tee��״̬��Ϣ
\param[in]  enHdmiId    hdmi channel id.
\retval HI_SUCCESS      success.
*/
HI_S32 HI_DRV_HDMI_TeeProc(HI_DRV_HDMI_DEVICE_ID_E enHdmi);

/**
\brief set hdcp capability. CNcomment��HDMI REE����������HDCP����
\param[in]  enHdmiId     hdmi channel id.
\param[in]  stHdcpCaps   hdcp caps:hdcp1.4 and hdcp2.2 support or not.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_SetHDCPCaps(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_DRV_HDMI_HDCP_CAPS_S *pstHdcpCaps);

/**
\brief hdmi ioctl. CNcomment��
\param[in]  enHdmiId    hdmi channel id.
\param[in]  pvData      data.
\param[in]  u32Size     data size.
\retval HI_SUCCESS      success.
*/
HI_S32 HI_DRV_HDMI_IOCTRL(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,const HI_VOID *pvData,HI_U32 u32Size);
/**
\brief get hdcp capability. CNcomment����ȡ��ǰHDCP����(TX��RX��������)
\param[in]  enHdmiId     hdmi channel id.
\param[in]  pstHdcpCaps  hdcp caps:hdcp1.4 and hdcp2.2 support or not.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_GetHDCPCaps(HI_DRV_HDMI_DEVICE_ID_E enHdmiId,HI_DRV_HDMI_HDCP_CAPS_S *pstHdcpCaps);

/**
\brief HDMI init. CNcomment��HDMI ��ʼ��
\param[in]  enHdmiId     hdmi channel id.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_Init(HI_DRV_HDMI_DEVICE_ID_E enHdmiId);

/**
\brief HDMI deinit. CNcomment��HDMI ȥ��ʼ��
\param[in]  enHdmiId     hdmi channel id.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_DeInit(HI_DRV_HDMI_DEVICE_ID_E enHdmiId);

/**
\brief HDMI start. CNcomment��HDMI ����
\param[in]  enHdmiId     hdmi channel id.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_Start(HI_DRV_HDMI_DEVICE_ID_E enHdmiId);

/**
\brief HDMI stop. CNcomment��HDMI ֹͣ
\param[in]  enHdmiId     hdmi channel id.
\retval HI_SUCCESS   success.
*/
HI_S32 HI_DRV_HDMI_Stop(HI_DRV_HDMI_DEVICE_ID_E enHdmiId);


#ifdef __cplusplus
}
#endif /* __cpluscplus */


#endif
