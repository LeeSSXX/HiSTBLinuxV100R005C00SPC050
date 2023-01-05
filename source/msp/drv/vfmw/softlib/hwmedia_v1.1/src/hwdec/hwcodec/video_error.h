
/*******************************************************************************
Copyright (C), 2009-2012, Hisilicon Tech. Co., Ltd.
File name: VIDEO_error.h
Author & ID: ��С��+00133955
Version: 1.00
Date:  2010-01-18
Description: ���ļ������˷���ֵ�������붨��
Function List:
History:
1��20100118 ������� ��С��+00133955
2��**
3������
*******************************************************************************/
#ifndef __VIDEO_ERROR_H__
#define __VIDEO_ERROR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif    /* __cpluscplus*/
#endif    /* __cpluscplus*/

/** ģ�鷵��ֵö�����Ͷ��� */
typedef enum ENUM_VIDEO_RETURN
{
    VIDEO_RET_SUCCESS = 0,             /**< �������� */

    /** ���������ⷵ�� */
    VIDEO_RET_PARAM_NULL = 0x0100,     /**< ����ָ��Ϊ�� */
    VIDEO_RET_PARAM_INVALID,           /**< ������Ч */
    VIDEO_RET_PARAM_IN_VALUE_NULL,     /**< �������ֵָ��Ϊ�� */
    VIDEO_RET_PARAM_IN_VALUE_INVALID,  /**< �������ֵ��Ч */
    VIDEO_RET_PARAM_OUT_VALUE_NULL,    /**< �������ֵָ��Ϊ�� */
    VIDEO_RET_PARAM_OUT_VALUE_INVALID, /**< �������ֵ��Ч */
    VIDEO_RET_PARAM_NOT_ENOUGH,        /**< �������� */

    VIDEO_RET_PARAM_OTHER = 0x01FF,    /**< �������������� */

    /** ���������ⷵ�� */
    VIDEO_RET_OBJ_NULL = 0x0200,       /**< ����ָ��Ϊ�� */
    VIDEO_RET_OBJ_BAD_MASK,            /**< ���������ٻ� */
    VIDEO_RET_OBJ_INVALID,             /**< ��Ч���� */
    VIDEO_RET_OBJ_NOT_FOUND,           /**< ����û�ҵ� */
    VIDEO_RET_OBJ_CREATE_FAIL,         /**< ���󴴽�ʧ�� */
    VIDEO_RET_OBJ_NOT_INIT,            /**< ����δ��ʼ�� */
    VIDEO_RET_OBJ_DELETE_FAIL,         /**< ����ɾ��ʧ�� */

    VIDEO_RET_OBJ_OTHER = 0x02FF,      /**< �������������� */

    /** ���棨�����������ⷵ�� */
    VIDEO_RET_MEM_MALLOC_FAIL = 0x0400,/**< �ڴ����ʧ�� */
    VIDEO_RET_MEM_FREE_FAIL,           /**< �ڴ��ͷ�ʧ�� */
    VIDEO_RET_BUF_NULL,                /**< ����ָ��Ϊ�� */
    VIDEO_RET_BUF_BAD_MASK,            /**< ���������ٻ� */
    VIDEO_RET_BUF_INVALID,             /**< ������Ч���޷�ʶ���ʽ */
    VIDEO_RET_BUF_OVER_FLOW,           /**< ������� */
    VIDEO_RET_BUF_ADDR_NOT_ALIGN,      /**< �����ַδ���� */
    VIDEO_RET_BUF_SIZE_NOT_ALIGN,      /**< �����Сδ���� */
    VIDEO_RET_STREAM_ERROR,			/**< ������������ */

    VIDEO_RET_BUF_OTHER = 0x04FF,      /**< �������������� */

    /** �������������ⷵ�� */
    VIDEO_RET_CMD_INVALID = 0x1000,

    /** �������ⷵ�� */
    VIDEO_RET_OTHER = 0x8000,
} ENUM_VIDEO_RETURN;

/** �㷨�����ö�����Ͷ��� */
typedef enum ENUM_VIDEO_CODEC_ERROR_CODE
{
    VIDEO_CODEC_ERR_NOTHING = 0,    	/**< �������� */

    /** ͨ�ô������� */
    VIDEO_CODEC_ERR_UNSUPPORTED_PROFILE,
    VIDEO_CODEC_ERR_UNSUPPORTED_LEVEL,
    VIDEO_CODEC_ERR_UNSUPPORT_COLOR_SPACE,
    VIDEO_CODEC_ERR_INVALID_WIDTH,
    VIDEO_CODEC_ERR_INVALID_HEIGHT,
    VIDEO_CODEC_ERR_INVALID_WIDTH_PITCH,
    VIDEO_CODEC_ERR_INVALID_HEIGHT_PITCH,
    VIDEO_CODEC_ERR_INVALID_FRAMERATE,
    VIDEO_CODEC_ERR_INVALID_BITRATE,

    /** H264�������� */
    VIDEO_CODEC_ERR_INVALID_SPS = 0x0100,
    VIDEO_CODEC_ERR_INVALID_PPS,
    //...

    /** MPEG2�������� */

    /** H263�������� */

    /** MPEG4�������� */

    /** WMV/VC1�������� */

    /** VP6�������� */

} ENUM_VIDEO_CODEC_ERROR_CODE;

#ifdef __cplusplus
#if __cplusplus
}
#endif    /* __cpluscplus*/
#endif    /* __cpluscplus*/

#endif /**< __VIDEO_ERROR_H__ */


