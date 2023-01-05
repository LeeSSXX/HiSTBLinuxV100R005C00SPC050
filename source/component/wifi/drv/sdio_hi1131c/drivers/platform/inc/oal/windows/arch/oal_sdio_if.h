/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_sdio.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2012��11��7��
  ����޸�   :
  ��������   : oal_sdio.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��10��20��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_WINDOWS_SDIO_IF_H__
#define __OAL_WINDOWS_SDIO_IF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "oal_types.h"

struct sdio_func {
    unsigned        enable_timeout; /* max enable timeout in msec */
};

OAL_STATIC OAL_INLINE oal_int32 oal_sdio_memcpy_fromio(struct sdio_func *func, oal_void *dst,
    oal_uint32 addr, oal_int32 count)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(dst);
    OAL_REFERENCE(addr);
    OAL_REFERENCE(count);
    return -OAL_EBUSY;
}

OAL_STATIC OAL_INLINE oal_int32 oal_sdio_readsb(struct sdio_func *func, oal_void *dst, oal_uint32 addr,
    oal_int32 count)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(dst);
    OAL_REFERENCE(addr);
    OAL_REFERENCE(count);
    return -OAL_EBUSY;
}

OAL_STATIC OAL_INLINE oal_int32 oal_sdio_writesb(struct sdio_func *func, oal_uint32 addr, oal_void *src,
    int count)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(src);
    OAL_REFERENCE(addr);
    OAL_REFERENCE(count);
    return -OAL_EBUSY;
}

OAL_STATIC OAL_INLINE oal_uint8 oal_sdio_readb(struct sdio_func *func, oal_uint32 addr, oal_int32 *err_ret)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(addr);
    *err_ret = -OAL_EBUSY;;
    return 0;
}

OAL_STATIC OAL_INLINE void oal_sdio_writeb(struct sdio_func *func, oal_uint8 b, oal_uint32 addr, oal_int32 *err_ret)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(addr);
    OAL_REFERENCE(b);
    *err_ret = -OAL_EBUSY;
}

OAL_STATIC OAL_INLINE oal_uint32 oal_sdio_readl(struct sdio_func *func, oal_uint32 addr, oal_int32 *err_ret)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(addr);
    *err_ret = -OAL_EBUSY;;
    return 0;
}

OAL_STATIC OAL_INLINE oal_void oal_sdio_writel(struct sdio_func *func, oal_uint32 b, oal_uint32 addr, oal_int32 *err_ret)
{
    OAL_REFERENCE(func);
    OAL_REFERENCE(addr);
    OAL_REFERENCE(b);
    *err_ret = -OAL_EBUSY;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_sdio_if.h */

