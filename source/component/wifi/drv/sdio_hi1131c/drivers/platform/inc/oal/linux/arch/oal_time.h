/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_time.h
  �� �� ��   : ����
  ��    ��   : ds
  ��������   : 2016��7��27��
  ����޸�   :
  ��������   : oal_time.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��7��27��
    ��    ��   : ds
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LINUX_TIME_H__
#define __OAL_LINUX_TIME_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/rtc.h>

#include <oal_types.h>

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#if (_PRE_CHIP_BITS_MIPS32 == _PRE_CHIP_BITS)
/* 32λ�Ĵ�����󳤶� */
#define OAL_TIME_US_MAX_LEN  (0xFFFFFFFF - 1)

#elif (_PRE_CHIP_BITS_MIPS64 == _PRE_CHIP_BITS)
/* 64λ�Ĵ�����󳤶� */
#define OAL_TIME_US_MAX_LEN  (0xFFFFFFFFFFFFFFFF - 1)

#endif


/* ��ȡ���뼶ʱ���*/
#define OAL_TIME_GET_STAMP_MS() jiffies_to_msecs(jiffies)

/* ��ȡ�߾��Ⱥ���ʱ���,����1ms*/
#define OAL_TIME_GET_HIGH_PRECISION_MS()  oal_get_time_stamp_from_timeval()

#define OAL_ENABLE_CYCLE_COUNT()
#define OAL_DISABLE_CYCLE_COUNT()
#define OAL_GET_CYCLE_COUNT() 0

/* �Ĵ�����תģ������ʱ����� */
#define OAL_TIME_CALC_RUNTIME(_ul_start, _ul_end)   ((((OAL_TIME_US_MAX_LEN) / HZ) * 1000) + ((OAL_TIME_US_MAX_LEN) % HZ) * (1000 / HZ) - (_ul_start) + (_ul_end))

#define OAL_TIME_JIFFY    jiffies

#define OAL_TIME_HZ       HZ

#define OAL_MSECS_TO_JIFFIES(_msecs)    msecs_to_jiffies(_msecs)

#define OAL_JIFFIES_TO_MSECS(_jiffies)      jiffies_to_msecs(_jiffies)

/* ��ȡ��_ul_start��_ul_end��ʱ��� */
#define OAL_TIME_GET_RUNTIME(_ul_start, _ul_end) \
    (((_ul_start) > (_ul_end))?(OAL_TIME_CALC_RUNTIME((_ul_start), (_ul_end))):((_ul_end) - (_ul_start)))


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


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
    oal_int i_sec;
    oal_int i_usec;
}oal_time_us_stru;

typedef ktime_t oal_time_t_stru;
typedef struct timeval oal_timeval_stru;
typedef struct rtc_time oal_rtctime_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : oal_time_get_stamp_us
 ��������  : ��ȡ΢��ȼ���ʱ���
 �������  : pst_usec: ʱ��ṹ��ָ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��30��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void  oal_time_get_stamp_us(oal_time_us_stru *pst_usec)
{
    struct timespec ts;

    getnstimeofday(&ts);

    pst_usec->i_sec     = ts.tv_sec;

    pst_usec->i_usec    = ts.tv_nsec /1000;

}

/*****************************************************************************
 �� �� ��  : oal_ktime_get
 ��������  : �����ں˺�����ȡ��ǰʱ���
 �������  : oal_void
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_time_t_stru oal_ktime_get(oal_void)
{
    return ktime_get();
}

/*****************************************************************************
 �� �� ��  : oal_ktime_sub
 ��������  : �����ں˺�����ȡʱ���ֵ
 �������  : const oal_time_t_stru lhs,
             const oal_time_t_stru rhs
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��27��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_time_t_stru oal_ktime_sub(const oal_time_t_stru lhs, const oal_time_t_stru rhs)
{
    return ktime_sub(lhs, rhs);
}


/*****************************************************************************
 �� �� ��  : oal_get_time_stamp_from_timeval
 ��������  : ��ȡʱ�侫��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��2��18��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint64 oal_get_time_stamp_from_timeval(oal_void)
{
    struct timeval tv;
    oal_uint64 curr_time;

    do_gettimeofday(&tv);
    curr_time = tv.tv_usec;
    do_div(curr_time, 1000);
    curr_time = curr_time + tv.tv_sec * 1000;

    return curr_time;

}

OAL_STATIC OAL_INLINE oal_void oal_do_gettimeofday(oal_timeval_stru *tv)
{
    do_gettimeofday(tv);
}
OAL_STATIC OAL_INLINE oal_void oal_rtc_time_to_tm(oal_ulong time, oal_rtctime_stru *tm)
{
    rtc_time_to_tm(time, tm);
}

/*****************************************************************************
 �� �� ��  : oal_time_is_before
 ��������  : �ж�ul_time�Ƿ�ȵ�ǰʱ����
             ���磬��ʾ��ʱʱ���ѹ��������磬������δ��ʱ
 �������  : oal_uint ui_time
 �������  : ��
 �� �� ֵ  : OAL_STATIC OAL_INLINE oal_uint32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��20��,���ڶ�
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32 oal_time_is_before(oal_uint ui_time)
{
    return (oal_uint32)time_is_before_jiffies(ui_time);
}

/*****************************************************************************
 �� �� ��  : oal_time_after
 ��������  : �ж�ʱ���ul_time_a�Ƿ���ul_time_b֮��:
 �������  : oal_uint ui_time
 �������  : ��
 �� �� ֵ  : Return: 1 ul_time_a��ul_time_b֮��; ���� Return: 0
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32 oal_time_after(oal_uint32 ul_time_a, oal_uint32 ul_time_b)
{
    return (oal_uint32)time_after((oal_ulong)ul_time_a, (oal_ulong)ul_time_b);
}

OAL_STATIC OAL_INLINE oal_ulong oal_ktime_to_us(const oal_time_t_stru kt)
{
    return ktime_to_us(kt);
}

OAL_STATIC OAL_INLINE oal_uint32 oal_time_before_eq(oal_uint32 ul_time_a, oal_uint32 ul_time_b)
{
    return time_before_eq((oal_ulong)ul_time_a, (oal_ulong)ul_time_b);
}

OAL_STATIC OAL_INLINE oal_uint32 oal_time_before(oal_uint32 ul_time_a, oal_uint32 ul_time_b)
{
    return time_before((oal_ulong)ul_time_a, (oal_ulong)ul_time_b);
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_time.h */
