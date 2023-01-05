/******************************************************************************
*
* Copyright (C) 2016 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name       : drv_hifb_scrolltext.c
Version         : Initial Draft
Author          :
Created         : 2016/02/02
Description     :
Function List   :


History         :
Date                Author                Modification
2016/02/02            sdk              Created file
******************************************************************************/


/*********************************add include here******************************/

#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/spinlock.h>

#include <linux/fb.h>
#include <linux/interrupt.h>

#include "hifb_drv.h"
#include "hifb.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_mem.h"
#include "drv_hifb_scrolltext.h"
#include "hi_gfx_sys_k.h"


#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT


/***************************** Macro Definition ******************************/


/*************************** Structure Definition ****************************/


/********************** Global Variable declaration **************************/

static spinlock_t scrolltextLock = __SPIN_LOCK_UNLOCKED(scrolltextLock);

/******************************* API declaration *****************************/

static inline HI_S32 DRV_SCROLLTEXT_SwitchTextBuffer(HIFB_SCROLLTEXT_S *pstScrollText, HI_U32 BufNum);

/******************************* API release *****************************/


/***************************************************************************************
* func            : hifb_alloscrolltext_handle
* description    : �жϹ�����Ļ�����Ƿ�Ϸ�
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_U32 hifb_alloscrolltext_handle(HI_U32 u32LayerId)
{
    HI_U32 u32ScrollTextHandle;
    HIFB_SCROLLTEXT_INFO_S *pstScrollTextInfo;
    pstScrollTextInfo = &s_stTextLayer[u32LayerId];

    /** ���ֻ����������Ļ **/
    u32ScrollTextHandle = pstScrollTextInfo->u32ScrollTextId++;
    if(pstScrollTextInfo->u32ScrollTextId > 1)
    {
        pstScrollTextInfo->u32ScrollTextId = 0;
    }
    if (!pstScrollTextInfo->bAvailable)
    {
        HIFB_ERROR("the scroll text was invalid!\n");
        return HIFB_SCROLLTEXT_BUTT_HANDLE;
    }

   /** HIFB_SCROLLTEXT_HANDLE :
    **    HIFB_SCROLLTEXT_HD0_HANDLE1 0x21
    **                                  2 = HIFB_LAYER_HD0
    **                                  1 = u32textnum(2) - 1
    ** u32ScrollTextHandle = 0 �� 1
    ** (0x0f & u32ScrollTextHandle) = 0 �� 1
    ** u32LayerId�������¼���ֵ
    **HIFB_LAYER_HD_0 = 0x0,
    **HIFB_LAYER_HD_1 = 0x1,
    **HIFB_LAYER_HD_2 = 0x2,
    **HIFB_LAYER_HD_3 = 0x3,
    **
    **HIFB_LAYER_SD_0 = 0x4,
    **HIFB_LAYER_SD_1 = 0x5,
    **HIFB_LAYER_SD_2 = 0x6,
    **HIFB_LAYER_SD_3 = 0x7,
    **
    **HIFB_LAYER_AD_0 = 0x8,
    **HIFB_LAYER_AD_1 = 0x9,
    **HIFB_LAYER_AD_2 = 0x10,
    **HIFB_LAYER_AD_3 = 0x11
    **��4λ��ʾͼ��ID������λ��ʾ�ĸ���Ļ
    **����HIFB_SCROLLTEXT_HD1_HANDLE0 ��ʾHIFB_LAYER_HD_1 = fb1��ĸ���һ����Ļ0x10
    **/
    return ((0xf0 & (u32LayerId << 4)) | (0x0f & u32ScrollTextHandle));

}

/***************************************************************************************
* func            : hifb_parse_scrolltexthandle
* description    : ��ȡͼ��ID����Ļ��ID
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_U32 hifb_parse_scrolltexthandle(HI_U32 u32Handle, HI_U32 *pU32LayerId, HI_U32 *pScrollTextId)
{


    if (u32Handle >= HIFB_SCROLLTEXT_BUTT_HANDLE)
    {
        HIFB_ERROR("invalid scrolltext handle!\n");
        return HI_FAILURE;
    }
    /** �������ԭ������Ϊ������Ļ��ʱ�����Ǹ���ͼ��ID����ĻID��ϵ�**/
    *pU32LayerId   = (u32Handle & 0xf0) >> 4;
    *pScrollTextId = u32Handle & 0x0f;

    if(   (HIFB_LAYER_CURSOR == *pU32LayerId)
        ||(*pU32LayerId >= HIFB_LAYER_ID_BUTT))
    {
        HIFB_ERROR("invalid scrolltext handle!\n");
        *pU32LayerId = HIFB_LAYER_ID_BUTT;
        return HI_FAILURE;
    }
    /** ���ֻ֧��������Ļ **/
    if (*pScrollTextId >= SCROLLTEXT_NUM)
    {
        HIFB_ERROR("invalid scrolltext handle!\n");
        *pScrollTextId = SCROLLTEXT_NUM;
        return HI_FAILURE;
    }

    return HI_SUCCESS;

}

/***************************************************************************************
* func            : hifb_check_scrolltext_para
* description    : �жϹ�����Ļ�����Ƿ�Ϸ�
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 hifb_check_scrolltext_para(HI_U32 u32LayerId, HIFB_SCROLLTEXT_ATTR_S *stAttr)
{

    HI_U32 i;
    HIFB_RECT stScrollTextRect, stSrcRect;
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar   = (HIFB_PAR_S *)info->par;

    stScrollTextRect = stAttr->stRect;
    if (0 == stAttr->u16CacheNum || SCROLLTEXT_CACHE_NUM < stAttr->u16CacheNum)
    {
        HIFB_ERROR("the cachenum u applied was invalid!\n");
        return HI_FAILURE;
    }
    /**
     **һ��ͼ�����ֻ֧������������Ļ,Ҫ��ʹ��һ����Ļ�㴴��������Ļ�������
     **Ҫ��ʹ������ͼ��ֱ𴴽�������Ļ����Ͳ�����������
     **/
    if(s_stTextLayer[u32LayerId].u32textnum >= SCROLLTEXT_NUM)
    {
        HIFB_ERROR("the scrolltext num created by hifb%d reached the maxinum!\n", u32LayerId);
        return HI_FAILURE;
    }

    if (  (0 > stScrollTextRect.x)
        ||(0 > stScrollTextRect.y)
        ||(pstPar->stExtendInfo.stPos.s32XPos > stScrollTextRect.x )
        ||(pstPar->stExtendInfo.stPos.s32YPos > stScrollTextRect.y))
    {
        HIFB_ERROR("failed to create the scrolltext because of wrong pos info!\n");
        return HI_FAILURE;
    }

    if (   (0 > stScrollTextRect.w)
         ||(0 > stScrollTextRect.h)
         ||(pstPar->stExtendInfo.u32DisplayWidth  < stScrollTextRect.w)
         ||(pstPar->stExtendInfo.u32DisplayHeight < stScrollTextRect.h))
    {
        HIFB_ERROR("failed to create the scrolltext because of wrong width or height!\n");
        return HI_FAILURE;
    }

   if (  (pstPar->stExtendInfo.stPos.s32XPos + pstPar->stExtendInfo.u32DisplayWidth) < (stScrollTextRect.w + stScrollTextRect.x)
        ||(pstPar->stExtendInfo.stPos.s32YPos + pstPar->stExtendInfo.u32DisplayHeight) < (stScrollTextRect.h + stScrollTextRect.y))
    {
        HIFB_ERROR("failed to create the scrolltext because of wrong width or height!\n");
        return HI_FAILURE;
    }

    /**
     ** whether the scroll text overlayed with each other
     ** ������Ļ�������ص���������
     **/
    for (i = 0; i < s_stTextLayer[u32LayerId].u32textnum; i++)
    {
        if(s_stTextLayer[u32LayerId].stScrollText[i].bAvailable)
        {
            stSrcRect = s_stTextLayer[u32LayerId].stScrollText[i].stRect;
            if(hifb_isoverlay(&stSrcRect, &stScrollTextRect))
            {
                HIFB_ERROR("failed to create the scrolltext because the scrolltext overlayed with another!\n");
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;

}

/***************************************************************************************
* func            : hifb_freescrolltext_cachebuf
* description    : �ͷŹ�����Ļbuffer
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 hifb_freescrolltext_cachebuf(HIFB_SCROLLTEXT_S *pstScrollText)
{
    HI_U32 i;
    HI_CHAR *pBuf;

    for (i = 0; i < pstScrollText->u32cachebufnum; i++)
    {
        pBuf = pstScrollText->stCachebuf[i].pVirAddr;
        if (HI_NULL != pBuf)
        {
            hifb_buf_ummap(pBuf);
        }
        pstScrollText->stCachebuf[i].pVirAddr = HI_NULL;

        if (pstScrollText->stCachebuf[i].u32PhyAddr != 0)
        {
            hifb_buf_freemem(pstScrollText->stCachebuf[i].u32PhyAddr,HI_TRUE);
        }
        pstScrollText->stCachebuf[i].u32PhyAddr = 0;

        pstScrollText->stCachebuf[i].bInusing   = HI_FALSE;
    }

    return HI_SUCCESS;
}


/***************************************************************************************
* func            : hifb_allocscrolltext_buf
* description    : ����������Ļbuffer
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 hifb_allocscrolltext_buf(HI_U32 u32LayerId, HIFB_SCROLLTEXT_ATTR_S *stAttr)
{

    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HI_U32 u32StartAddr;
    HI_U32 i, u32cacheSize, u32Pitch;
    HI_CHAR name[32];
    HI_CHAR *pBuf;

    /**
     ** ��������������Ļ��ʹ�õ��ǵڼ���������Ļ
     ** Ҫôһ��ͼ������������Ļ
     ** Ҫô����ͼ��ֱ�ʹ��һ����Ļ
     **/
    HI_U32 u32Index  = s_stTextLayer[u32LayerId].u32ScrollTextId;
    HIFB_SCROLLTEXT_S *pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32Index]);
    /** ��Ϣ��������s_stTextLayerȫ�ֱ����� **/

    /** if with old cache buffer **/
    if(pstScrollText->bAvailable)
    {/** ����Ļ�Ѿ��ù��ˣ������Ļ��buffer **/
        /** free old buffer*/
        HIFB_INFO("free old scrolltext cache buffer\n");
        hifb_freescrolltext_cachebuf(pstScrollText);
    }

    /** Modify 16 to 32, preventing out of bound. **/
    /** 16 bytes aligmn **/
    u32Pitch = ((stAttr->stRect.w * info->var.bits_per_pixel >> 3) + 15)>>4;
    u32Pitch = u32Pitch << 4;

    u32cacheSize =  u32Pitch * stAttr->stRect.h;

    /** ��bufferģʽ���û�̬���õ� **/
    for (i = 0; i < stAttr->u16CacheNum; i++)
    {/** ˫bufferģʽ **/
        snprintf(name, sizeof(name), "HIFB_Layer%d_Scroll%d", u32LayerId, i);
        u32StartAddr = hifb_buf_allocmem(name, "iommu", u32cacheSize);

        if (HI_NULL == u32StartAddr)
        {
            HIFB_ERROR("failed to alloc cache buffer for the scrolltext!\n");
            return HI_FAILURE;
        }
        pstScrollText->stCachebuf[i].u32PhyAddr = u32StartAddr;

        pBuf = (HI_CHAR *)hifb_buf_map(pstScrollText->stCachebuf[i].u32PhyAddr);
        if (pBuf == HI_NULL)
        {
            HIFB_ERROR("map cache buffer failed!\n");
            hifb_buf_freemem(pstScrollText->stCachebuf[i].u32PhyAddr,HI_TRUE);
            return HI_FAILURE;
        }
        HI_GFX_Memset(pBuf, 0, u32cacheSize);
        /** �շ����껹û����ʹ�� **/
        pstScrollText->stCachebuf[i].bInusing   = HI_FALSE;
        pstScrollText->stCachebuf[i].pVirAddr   = pBuf;

    }
    pstScrollText->bAvailable     = HI_TRUE; /** �������ˣ�˵������Ļ����ʹ���� **/
    pstScrollText->u32Stride      = u32Pitch;
    pstScrollText->u32cachebufnum = stAttr->u16CacheNum;
    pstScrollText->bDeflicker     = stAttr->bDeflicker;
    pstScrollText->ePixelFmt      = stAttr->ePixelFmt;
    HI_GFX_Memcpy(&(pstScrollText->stRect), &(stAttr->stRect), sizeof(HIFB_RECT));

    return HI_SUCCESS;

}

static HI_VOID hifb_scrolltext_blit_work(struct work_struct *work);

/***************************************************************************************
* func            : hifb_create_scrolltext
* description    : ����������Ļ
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 hifb_create_scrolltext(HI_U32 u32LayerId, HIFB_SCROLLTEXT_CREATE_S *stScrollText)
{


    HI_S32 s32Ret;
    HI_U32 u32Index;
    HIFB_SCROLLTEXT_ATTR_S stAttr = stScrollText->stAttr; /** �û�����������Ϣ **/

    /** fb�ڲ���Ϣ **/
    HIFB_SCROLLTEXT_INFO_S *pstTextInfo = &s_stTextLayer[u32LayerId];
    HIFB_SCROLLTEXT_S *stText;

    /**
     **u32LayerId����ڴ�����Ļ���ʱ���Ѿ���ֵ�ˡ�
     **/
    /*check the parameter of scrolltext struct*/
    s32Ret = hifb_check_scrolltext_para(u32LayerId, &stAttr);
    if (HI_SUCCESS != s32Ret)
    {
        HIFB_ERROR("failed to create scrolltext!\n");
        return HI_FAILURE;
    }

    s32Ret = hifb_allocscrolltext_buf(u32LayerId, &stAttr);
    if (HI_SUCCESS != s32Ret)
    {
        HIFB_ERROR("failed to create scrolltext!\n");
        return HI_FAILURE;
    }

    /**
     ** return the handle ,that usr can manipulate it
     ** to fill data to scrolltext
     **/
    pstTextInfo->u32textnum++;          /** ����һ����Ļ���Լ�һ�� **/

    if (!pstTextInfo->bAvailable)
        INIT_WORK(&(s_stTextLayer[u32LayerId].blitScrollTextWork), hifb_scrolltext_blit_work);

    pstTextInfo->bAvailable  = HI_TRUE; /** ����Ļ���� **/

    /** �����Ļ����Ǻ�ͼ��ID�Լ�ÿ��ͼ����������Ļ��ҹ��� **/
    stScrollText->u32Handle = hifb_alloscrolltext_handle(u32LayerId);
    /** ���ֻ��������0��1 **/
    u32Index = stScrollText->u32Handle & 0x0f;
    /** ����Ҳ�ᱣ�浽ȫ�ֱ����� **/
    stText = &(pstTextInfo->stScrollText[u32Index]);
    stText->enHandle = stScrollText->u32Handle; /** �����������ĸ�ͼ�β�� **/
    stText->u32IdleFlag = 1;

    init_waitqueue_head(&(stText->wbEvent));

    return HI_SUCCESS;

}

/***************************************************************************************
* func            : hifb_fill_scrolltext
* description    : ���ݴ�������ط�����ͷϷ������Ƚϸ���
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 hifb_fill_scrolltext(HIFB_SCROLLTEXT_DATA_S *stScrollTextData)
{

    HI_U32 i, u32LayerId, u32TextId, u32Handle;
    HI_S32 s32Ret;
    HIFB_SCROLLTEXT_CACHE stCacheBuf;
    HIFB_BUFFER_S stTempBuf, stCanvasBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    struct fb_info *info;
    HIFB_PAR_S *pstPar;
    HIFB_SCROLLTEXT_S *pstScrollText;


    u32Handle = stScrollTextData->u32Handle;
    HI_GFX_Memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));

    s32Ret = hifb_parse_scrolltexthandle(u32Handle, &u32LayerId, &u32TextId);
    if (HI_SUCCESS != s32Ret)
    {
        HIFB_ERROR("fill data to scrolltext failed because of invalid scrolltext handle!\n");
        return HI_FAILURE;
    }

    info   = s_stLayer[u32LayerId].pstInfo;
    pstPar = (HIFB_PAR_S *)info->par;
    pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32TextId]);

    if(HI_FALSE == pstScrollText->bAvailable)
    {
        HIFB_ERROR("the scrolltext was invalid!\n");
        return HI_FAILURE;
    }

    if (HI_TRUE == pstScrollText->bPause)
    {
        return HI_SUCCESS;
    }

    if (HI_NULL != stScrollTextData->u32PhyAddr)
    {
        if (!pstScrollText->u32IdleFlag)
        {
            wait_event_interruptible_timeout(pstScrollText->wbEvent, pstScrollText->u32IdleFlag, 100*HZ);
        }

        stCanvasBuf.stCanvas.u32PhyAddr = stScrollTextData->u32PhyAddr;
        stCanvasBuf.stCanvas.enFmt      = pstScrollText->ePixelFmt;
        stCanvasBuf.stCanvas.u32Width   = pstScrollText->stRect.w;
        stCanvasBuf.stCanvas.u32Height  = pstScrollText->stRect.h;
        stCanvasBuf.stCanvas.u32Pitch   = stScrollTextData->u32Stride;
        stCanvasBuf.UpdateRect.x        = 0;
        stCanvasBuf.UpdateRect.y        = 0;
        stCanvasBuf.UpdateRect.w        = pstScrollText->stRect.w;
        stCanvasBuf.UpdateRect.h        = pstScrollText->stRect.h;

        for (i = 0; i < pstScrollText->u32cachebufnum; i++)
        {
            stCacheBuf = pstScrollText->stCachebuf[i];
            if (HI_FALSE == stCacheBuf.bInusing)
            {
                stTempBuf.stCanvas.u32PhyAddr = stCacheBuf.u32PhyAddr;
                stTempBuf.stCanvas.enFmt      = pstPar->stExtendInfo.enColFmt;
                stTempBuf.stCanvas.u32Width   = pstScrollText->stRect.w;
                stTempBuf.stCanvas.u32Height  = pstScrollText->stRect.h;
                stTempBuf.stCanvas.u32Pitch   = pstScrollText->u32Stride;
                stTempBuf.UpdateRect.x        = 0;
                stTempBuf.UpdateRect.y        = 0;
                stTempBuf.UpdateRect.w        = pstScrollText->stRect.w;
                stTempBuf.UpdateRect.h        = pstScrollText->stRect.h;

                if (stTempBuf.stCanvas.u32Pitch != stCanvasBuf.stCanvas.u32Pitch)
                {
                    stBlitOpt.bScale = HI_TRUE;
                }

                if (pstScrollText->bDeflicker && (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE))
                {
                    stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
                }
                stBlitOpt.bBlock           = HI_TRUE;
                stBlitOpt.bCallBack        = HI_FALSE;
                s32Ret = g_stGfx2dCallBackFunction.HIFB_DRV_Blit(&stCanvasBuf, &stTempBuf, &stBlitOpt, HI_TRUE);
                if (s32Ret <= 0){
                    HIFB_ERROR("hifb_fill_scrolltext blit err !\n");
                    return HI_FAILURE;
                }

                i = DRV_SCROLLTEXT_SwitchTextBuffer(pstScrollText,i);

                return HI_SUCCESS;
            }
        }
    }
    else if (HI_NULL != stScrollTextData->pu8VirAddr)
    {
        HI_CHAR *pBuf = NULL;
        if (pstPar->stExtendInfo.enColFmt != pstScrollText->ePixelFmt)
        {
            HIFB_ERROR("invalid virtual address!\n");
            return HI_FAILURE;
        }

        if (!pstScrollText->u32IdleFlag)
        {
            wait_event_interruptible_timeout(pstScrollText->wbEvent, pstScrollText->u32IdleFlag, 100*HZ);
        }

        for (i = 0; i < pstScrollText->u32cachebufnum; i++)
        {
            HI_U32 u32LineNum = 0;
            stCacheBuf = pstScrollText->stCachebuf[i];
            if (HI_FALSE == stCacheBuf.bInusing)
            {
                pBuf = pstScrollText->stCachebuf[i].pVirAddr;
                for (u32LineNum = 0; u32LineNum < pstScrollText->stRect.h; u32LineNum++)
                {
                    HI_GFX_Memcpy(pBuf + u32LineNum*pstScrollText->u32Stride,
                           stScrollTextData->pu8VirAddr + u32LineNum*stScrollTextData->u32Stride,pstScrollText->u32Stride);
                }
                i = DRV_SCROLLTEXT_SwitchTextBuffer(pstScrollText,i);
                return HI_SUCCESS;
            }
        }
    }

    return HI_FAILURE;
}



static inline HI_S32 DRV_SCROLLTEXT_SwitchTextBuffer(HIFB_SCROLLTEXT_S *pstScrollText, HI_U32 BufNum)
{
    HI_ULONG flags = 0;
    HI_S32 s32Cnt  = 0;

    spin_lock_irqsave(&scrolltextLock, flags);

    pstScrollText->stCachebuf[BufNum].bInusing = HI_TRUE;
    pstScrollText->u32IdleFlag = 0;

    while (BufNum < pstScrollText->u32cachebufnum)
    {
         HI_U32 u32LastUsingCache = BufNum;
         HI_U32 j = BufNum + 1;

         while (j < pstScrollText->u32cachebufnum)
         {
              if (HI_TRUE == pstScrollText->stCachebuf[j].bInusing)
              {
                    HI_BOOL bInusing   =  pstScrollText->stCachebuf[j].bInusing;
                    HI_U32  u32PhyAddr = pstScrollText->stCachebuf[j].u32PhyAddr;
                    HI_U8   *pVirAddr  = pstScrollText->stCachebuf[j].pVirAddr;
                    pstScrollText->stCachebuf[j].bInusing = pstScrollText->stCachebuf[u32LastUsingCache].bInusing;
                    pstScrollText->stCachebuf[j].u32PhyAddr = pstScrollText->stCachebuf[u32LastUsingCache].u32PhyAddr;
                    pstScrollText->stCachebuf[j].pVirAddr = pstScrollText->stCachebuf[u32LastUsingCache].pVirAddr;
                    pstScrollText->stCachebuf[u32LastUsingCache].bInusing = bInusing;
                    pstScrollText->stCachebuf[u32LastUsingCache].u32PhyAddr = u32PhyAddr;
                    pstScrollText->stCachebuf[u32LastUsingCache].pVirAddr = pVirAddr;
                    u32LastUsingCache = j;
              }
              else
              {
                    pstScrollText->u32IdleFlag = 1;
              }

              j++;
         }

        BufNum++;
    }

    if (HI_FALSE == pstScrollText->stCachebuf[0].bInusing)
    {
        pstScrollText->u32IdleFlag = 1;
        for (s32Cnt = 1; s32Cnt < pstScrollText->u32cachebufnum; s32Cnt++)
        {
             HI_BOOL bInusing   =  pstScrollText->stCachebuf[s32Cnt - 1].bInusing;
             HI_U32  u32PhyAddr = pstScrollText->stCachebuf[s32Cnt - 1].u32PhyAddr;
             HI_U8   *pVirAddr  = pstScrollText->stCachebuf[s32Cnt - 1].pVirAddr;
             pstScrollText->stCachebuf[s32Cnt - 1].bInusing   = pstScrollText->stCachebuf[s32Cnt].bInusing;
             pstScrollText->stCachebuf[s32Cnt - 1].u32PhyAddr = pstScrollText->stCachebuf[s32Cnt].u32PhyAddr;
             pstScrollText->stCachebuf[s32Cnt - 1].pVirAddr   = pstScrollText->stCachebuf[s32Cnt].pVirAddr;
             pstScrollText->stCachebuf[s32Cnt].bInusing   = bInusing;
             pstScrollText->stCachebuf[s32Cnt].u32PhyAddr = u32PhyAddr;
             pstScrollText->stCachebuf[s32Cnt].pVirAddr   = pVirAddr;
        }
    }

    spin_unlock_irqrestore(&scrolltextLock, flags);

    return BufNum;
}


/***************************************************************************************
* func            : hifb_destroy_scrolltext
* description    : ���ٹ�����Ļ
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
HI_S32 hifb_destroy_scrolltext(HI_U32 u32LayerID, HI_U32 u32ScrollTextID)
{
    HI_S32 s32Ret;
    HIFB_SCROLLTEXT_S       *pstScrollText;

    if (!s_stTextLayer[u32LayerID].stScrollText[u32ScrollTextID].bAvailable)
    {
        HIFB_ERROR("invalid scrolltext handle!\n");
        return HI_FAILURE;
    }

    s_stTextLayer[u32LayerID].u32textnum--;
    if(0 == s_stTextLayer[u32LayerID].u32textnum){
        s_stTextLayer[u32LayerID].bAvailable = HI_FALSE;
        flush_work(&s_stTextLayer[u32LayerID].blitScrollTextWork);
    }

    pstScrollText = &(s_stTextLayer[u32LayerID].stScrollText[u32ScrollTextID]);
    /*wait tde blit job done*/
    if (pstScrollText->s32TdeBlitHandle){
        s32Ret = g_stGfx2dCallBackFunction.HIFB_DRV_WaitForDone(pstScrollText->s32TdeBlitHandle, 1000);
        if (s32Ret < 0){
            HIFB_ERROR("HIFB_DRV_WaitForDone failed!ret=%x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    hifb_freescrolltext_cachebuf(pstScrollText);

    s_stTextLayer[u32LayerID].u32ScrollTextId = u32ScrollTextID;
    HI_GFX_Memset(pstScrollText,0,sizeof(HIFB_SCROLLTEXT_S));

    return HI_SUCCESS;
}

/***************************************************************************************
* func            : hifb_scrolltext_callback
* description    : ������Ļ�ص���TDE blit֮��ص���
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/
static HI_S32 hifb_scrolltext_callback(HI_VOID *pParaml, HI_VOID *pParamr)
{
    HI_S32 s32Ret;
    HI_U32 u32TextLayerId, u32TextId, u32Handle;
    HIFB_SCROLLTEXT_S *pstScrollText;
    u32Handle = *(HI_U32 *)pParaml;

    s32Ret = hifb_parse_scrolltexthandle(u32Handle, &u32TextLayerId, &u32TextId);
    if (HI_SUCCESS != s32Ret)
    {
        HIFB_ERROR("failed to parse the scrolltext handle!\n");
        return HI_FAILURE;
    }

    pstScrollText = &(s_stTextLayer[u32TextLayerId].stScrollText[u32TextId]);
    pstScrollText->stCachebuf[0].bInusing = HI_FALSE;

    if (pstScrollText->bAvailable)
    {
        pstScrollText->s32TdeBlitHandle = HI_NULL;
        pstScrollText->bBliting         = HI_FALSE;
       /** wake up ���ݴ����ʱ����������� **/
       pstScrollText->u32IdleFlag = 1;
       wake_up_interruptible(&(pstScrollText->wbEvent));
    }

    return HI_SUCCESS;
}


/***************************************************************************************
* func            : hifb_scrolltext_blit
* description    : ������Ļblit
* param[in]     :
* retval        : NA
* others:        : NA
***************************************************************************************/

static HI_VOID hifb_scrolltext_blit_work(struct work_struct *work)
{
    HI_S32 s32Ret;
    HI_U32 i, j;
    HI_U32 u32StartAddr;
    HIFB_SCROLLTEXT_INFO_S *pstScrollTextInfo;
    HIFB_SCROLLTEXT_S      *pstScrollText;
    struct fb_info *info;
    HIFB_PAR_S *pstPar;
    HI_U32 u32LayerID;

    pstScrollTextInfo = (HIFB_SCROLLTEXT_INFO_S *)container_of(work, HIFB_SCROLLTEXT_INFO_S, blitScrollTextWork);
    u32LayerID = pstScrollTextInfo - &s_stTextLayer[0];
    info = s_stLayer[u32LayerID].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    //pstScrollTextInfo = &(s_stTextLayer[u32LayerID]);
    //u32StartAddr = pstPar->stBufInfo.u32DisplayAddr[0];
    u32StartAddr = pstPar->stRunInfo.CurScreenAddr;
    /*blit the cache buffer of scrolltext to the display buffer*/
    if (s_stTextLayer[u32LayerID].bAvailable)
    {
        for (i = 0; i < SCROLLTEXT_NUM; i++)
        {
            j = 0;
            pstScrollText = &(pstScrollTextInfo->stScrollText[i]);

            if (pstScrollText->bAvailable
                && !pstScrollText->bPause
                && pstScrollText->stCachebuf[j].bInusing
                && !pstScrollText->bBliting)
            {
                HIFB_BUFFER_S stTempBuf, stCanvasBuf;
                HIFB_BLIT_OPT_S stBlitOpt;

                HI_GFX_Memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));

                stCanvasBuf.stCanvas.u32PhyAddr = pstScrollText->stCachebuf[j].u32PhyAddr;
                stCanvasBuf.stCanvas.enFmt      = pstPar->stExtendInfo.enColFmt;
                stCanvasBuf.stCanvas.u32Width   = pstScrollText->stRect.w;
                stCanvasBuf.stCanvas.u32Height  = pstScrollText->stRect.h;
                stCanvasBuf.stCanvas.u32Pitch   = pstScrollText->u32Stride;

                stCanvasBuf.UpdateRect.x        = 0;
                stCanvasBuf.UpdateRect.y        = 0;
                stCanvasBuf.UpdateRect.w        = pstScrollText->stRect.w;
                stCanvasBuf.UpdateRect.h        = pstScrollText->stRect.h;

                stTempBuf.stCanvas.u32PhyAddr   = u32StartAddr;
                stTempBuf.stCanvas.enFmt        = pstPar->stExtendInfo.enColFmt;
                stTempBuf.stCanvas.u32Width     = pstPar->stExtendInfo.u32DisplayWidth;
                stTempBuf.stCanvas.u32Height    = pstPar->stExtendInfo.u32DisplayHeight;
                stTempBuf.stCanvas.u32Pitch   = info->fix.line_length;

                stTempBuf.UpdateRect.x          = pstScrollText->stRect.x - pstPar->stExtendInfo.stPos.s32XPos;
                stTempBuf.UpdateRect.y          = pstScrollText->stRect.y - pstPar->stExtendInfo.stPos.s32YPos;;
                stTempBuf.UpdateRect.w          = pstScrollText->stRect.w;
                stTempBuf.UpdateRect.h          = pstScrollText->stRect.h;

                if (stTempBuf.stCanvas.u32Width != stCanvasBuf.stCanvas.u32Width
                    || stTempBuf.stCanvas.u32Height != stCanvasBuf.stCanvas.u32Height)
                {
                    stBlitOpt.bScale = HI_TRUE;
                }

                if (pstScrollText->bDeflicker
                    && pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
                {
                    stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
                }

                //stBlitOpt.bRegionDeflicker = HI_TRUE;
                stBlitOpt.bBlock           = HI_FALSE;
                stBlitOpt.bCallBack        = HI_TRUE;
                stBlitOpt.pfnCallBack      = (IntCallBack)hifb_scrolltext_callback;
                stBlitOpt.pParam           = &(pstScrollText->enHandle);
                /** TDE������֮���ص� hifb_scrolltext_callback ������� **/
                pstScrollText->bBliting = HI_TRUE;
                s32Ret = g_stGfx2dCallBackFunction.HIFB_DRV_Blit(&stCanvasBuf, &stTempBuf, &stBlitOpt, HI_TRUE);
                if (s32Ret <= 0){
                    HIFB_ERROR("hifb_scrolltext_blit blit err !\n");
                    return;
                }

                pstScrollText->s32TdeBlitHandle = s32Ret;

            }
        }
    }

    return;
}
HI_S32 hifb_scrolltext_blit(HI_U32 u32LayerId)
{
    if (s_stTextLayer[u32LayerId].bAvailable)
        schedule_work(&(s_stTextLayer[u32LayerId].blitScrollTextWork));
    return HI_SUCCESS;
}
#endif
