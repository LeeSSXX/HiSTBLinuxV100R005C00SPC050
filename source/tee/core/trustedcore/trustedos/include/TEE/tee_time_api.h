#ifndef __TEE_TIME_API_H
#define __TEE_TIME_API_H

#include "tee_internal_api.h"

#define TEE_TIMEOUT_INFINITE (0xFFFFFFFF)

typedef struct {
	uint32_t seconds;
	uint32_t millis;
} TEE_Time;


typedef struct {
	int32_t seconds;
	int32_t millis;
	int32_t min;
	int32_t hour;
	int32_t day;
	int32_t month;
	int32_t year;
} TEE_Date_Time;

typedef struct {
	uint32_t type;
	uint32_t timer_id;
	uint32_t timer_class;
	uint32_t reserved2;
} TEE_timer_property;

typedef enum {
	ANTI_ROOT_TIMER = 1,
} TEE_Anti_Root_Timer_Type;

void get_sys_rtc_time(TEE_Time *time);

void TEE_GetSystemTime(TEE_Time *time);

TEE_Result TEE_Wait(uint32_t timeout);

TEE_Result TEE_GetTAPersistentTime(TEE_Time *time);

TEE_Result TEE_SetTAPersistentTime(TEE_Time *time);

void TEE_GetREETime(TEE_Time *time);
void TEE_GetREETimeStr(char *time_str);

/**
 * @ingroup  TEE_EXT_TIMER_API
 * @brief ����һ����ȫʱ��
 *
 * @par ����:
 * ����һ����ȫʱ��
 *
 * @attention ��
 * @param time_seconds [IN]   ��ȫʱ��ʱ��
 * @param timer_property [IN]  ��ȫʱ������type
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS:   ��δ���
 * @retval #TEE_ERROR_OUT_OF_MEMORY �ڴ治��
 * @retval #TEE_ERROR_TIMER_CREATE_FAILED timer����ʧ��
 *
 * @par ����:
 * @li tee_timer_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result  TEE_EXT_CreateTimer(uint32_t time_seconds,   TEE_timer_property *timer_property);

/**
 * @ingroup  TEE_EXT_TIMER_API
 * @brief ����һ����ȫʱ��
 *
 * @par ����:
 * ����һ����ȫʱ��
 *
 * @attention ��
 * @param timer_property [IN]  ��ȫʱ������type
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_TIMER_NOT_FOUND:   timer������
 * @retval #TEE_ERROR_TIMER_DESTORY_FAILED timer����ʧ��
 *
 * @par ����:
 * @li tee_timer_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result  TEE_EXT_DestoryTimer(TEE_timer_property *timer_property);

/**
 * @ingroup  TEE_EXT_TIMER_API
 * @brief ��ȡtimer��ʱʱ��
 *
 * @par ����:
 * ��ȡtimer��ʱʱ��
 *
 * @attention ��
 * @param timer_property [IN]  ��ȫʱ������type
 * @param time_seconds [OUT]  ��ȫʱ��
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_TIMER_NOT_FOUND:   timer������
 *
 * @par ����:
 * @li tee_timer_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result  TEE_EXT_GetTimerExpire(TEE_timer_property *timer_property, uint32_t *time_seconds);

/**
 * @ingroup  TEE_EXT_TIMER_API
 * @brief ��ȡtimer��ʱʣ��ʱ��
 *
 * @par ����:
 * ��ȡtimer��ʱʣ��ʱ��
 *
 * @attention ��
 * @param timer_property [IN]  ��ȫʱ������type
 * @param time_seconds [OUT]  ��ȫʱ��
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_TIMER_NOT_FOUND:   timer������
 *
 * @par ����:
 * @li tee_timer_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R005C00
*/
TEE_Result  TEE_EXT_GetTimerRemain(TEE_timer_property *timer_property, uint32_t *time_seconds);

TEE_Result  TEE_ANTI_ROOT_CreateTimer(uint32_t time_seconds);
TEE_Result  TEE_ANTI_ROOT_DestoryTimer();
#endif
