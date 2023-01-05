
/**************************************************************************************************************
Copyright (C), 2009-2012, Hisilicon Tech. Co., Ltd.
File name: imedia_midprocessor.c
Author & ID: ���+00101841
Version: 1.00
Date:  2009-12-18
Description: ���ļ��������м䴦��ģ��Ľӿں�������ؽṹ����
Function List:
1. IMedia_Image_Create �����㷨ͨ��
2. IMedia_Image_Control �㷨ͨ�����ƺ���
3. IMedia_Image_Process �㷨ͨ��������
4. IMedia_Image_Delete ɾ���㷨ͨ��
History:
1.20091218 ������� ���+00101841
2.20100209 ����review���޸� ���+00101841

* Revision 1.01  2010/03/04 16:0:00  guoshan+00101841
* .R.ģ�������£����������в��÷ֳ����²�������

* Revision 1.02  2010/03/05 16:0:00  guoshan+00101841
* .R.ģ�������£�����resizeģ���ͼ����16������������������WStride����ԭʼWidth

* Revision 1.03  2010/03/19 10:40:00  ���+00101841
* B010�汾���Ժ��޸Ĵ���
*****************************************************************************************************************/

#include "imedia_midprocessor.h"
#include "iMedia_util.h"
#include "deinterlace.h"
#include "resize.h"

typedef struct STRU_MID_PROCESSOR_CTX
{
    INT32 iCrcMask;      // ������
    INT32 iProcessType;  // ͬENUM_IMAGE_PROCESS_TYPE��Ӧ
    void *pSubCtx;       // sub module handler

    STRU_IMEDIA_CPU_CAPS stCpuCaps;     // CPU����������MMX��SSE��
    STRU_IMEDIA_MEMBUF   stMemBuf;
    STRU_IMAGE_PARAMS    stImageParams; // �洢��Ϣ������resize����

} STRU_MID_PROCESSOR_CTX;

/*guoshan+00101841 20100829*/
/*��KeepAspectRatioģʽ�£���ǰ���resizeģ��Ĳ����Ƿ�Ϸ�*/
#define WIDTH_ALIGN 16
#define HEIGHT_ALIGN 2

static INT32 Resize_CheckParam( UINT16 usSrcW, UINT16 usSrcH, UINT16 usSrcPitch, UINT16 usDstW, UINT16 usDstH, UINT16 usDstPitch);
/*=============================================================================================================
* ����˵����
    �����㷨ͨ����
* ���������
    eType      IMAGE_PROCESS_TYPE����������ȥ�����㷨
    params     һЩ������Ϣ�Ľṹ��ָ�룬����ԭʼ��Ŀ��ͼ��Ŀ�ߵ���Ϣ
* ���������
    pctx       �㷨ͨ�����
* �� �� ֵ��
    0-�ɹ�������ֵ������ֵ���塣
===============================================================================================================*/
EXPORT_API INT32 IMedia_Image_Create(ENUM_IMAGE_PROCESS_TYPE eType, STRU_IMAGE_PARAMS *params, IMEDIA_IMAGE_CTX *pctx)
{
    STRU_MID_PROCESSOR_CTX *pstMidProcCtx = NULL;
    INT32 iRet = IMEDIA_RET_SUCCESS;

    /* 2010/03/05 15:25:00 guoshan+00101841*/
    /* �޸Ĵ���resizeģ��ӿڵı������Ա�֤ͼ��usWidth��16�������� */
    /* 2010/03/17 18:55:00 guoshan+00101841*/
    /* Ϊ������SRSһ�£�����ʵ��width���з�Χ���,ȡ���˱��� */
    //     UINT16 usSrcWMaxStride = 0;
    //     UINT16 usDstWMaxStride = 0;

    if (NULL == params)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "image parameters NULL\n");
        return IMEDIA_RET_PARAM_NULL;
    }

    if (NULL == pctx)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "pctx NULL\n");
        return IMEDIA_RET_PARAM_NULL;
    }

    *pctx = NULL;

    g_stGlobalInfo.bMallocFxnUsed = TRUE;

    /*create mid-processor channel handler*/
    pstMidProcCtx = IMedia_Malloc(sizeof(STRU_MID_PROCESSOR_CTX));

    if (NULL == pstMidProcCtx)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "failed to malloc mid_ctx memory\n");
        return  IMEDIA_RET_MEM_MALLOC_FAIL;
    }

    IMEDIA_LOG(IMEDIA_INFO, "malloc mid_ctx = %p succeed\n", pstMidProcCtx);

    memset(pstMidProcCtx, 0, sizeof(STRU_MID_PROCESSOR_CTX));
    memcpy(&pstMidProcCtx->stImageParams, params, sizeof(STRU_IMAGE_PARAMS));
    pstMidProcCtx->iCrcMask = CRC_MASK;
    pstMidProcCtx->iProcessType = eType;

    /* 2010/04/29 18:05:00 songxiaogang+00133955 */
    /* ����������Ż�ʱ���Զ���ȡCPUָ� */
    if (!(params->iFlags & IMEDIA_FLAG_DISABLE_ASM))
    {
        iRet = IMedia_GetCpuCaps(&pstMidProcCtx->stCpuCaps);
    }

    /* 2010/03/04 18:55:00 guoshan+00101841*/
    /* Ϊ��֤����ӿ�RESIZE_QUALITY_LEVEL��һ���ԣ��ⲿ�����ȼ���+1���� */
    pstMidProcCtx->stImageParams.iSubType = params->iSubType + 1;

    /* 2010/03/17 18:55:00 guoshan+00101841*/
    /* Ϊ������SRSһ�£�����ģ�鴫����ʵwidthֵ */
    //     usSrcWMaxStride = IVIDEO_WIDTH_STRIDE(params->usMaxSrcWidth);
    //     usDstWMaxStride = IVIDEO_WIDTH_STRIDE(params->usMaxDstWidth);

    switch (eType)
    {
        case IMAGE_RESIZE:
        {
            /*inquiry about the memory buffer resizing module need*/
            pstMidProcCtx->stMemBuf.uiAlignment = 16;  //16 bit����

            /* 2010/03/05 15:25:00 guoshan+00101841*/
            /* �޸Ĵ���resizeģ��ӿڵı������Ա�֤ͼ��usWidth��16�������� */
            //             iRet = RESIZE_QueryMem((RESIZE_QUALITY_LEVEL)(params->iSubType + 1), params->usMaxSrcWidth, params->usMaxSrcHeight,
            //                                   params->usMaxDstWidth, params->usMaxDstHeight, (INT32 *)&pstMidProcCtx->stMemBuf.uiSize);
            //             iRet = RESIZE_QueryMem((RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType, usSrcWMaxStride, params->usMaxSrcHeight,
            //                                                    usDstWMaxStride, params->usMaxDstHeight, (INT32 *)&pstMidProcCtx->stMemBuf.uiSize);

            /* 2010/03/17 18:55:00 guoshan+00101841*/
            /* Ϊ������SRSһ�£�����ģ�鴫����ʵ��widthֵ */
            iRet = RESIZE_QueryMem((RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType, params->usMaxSrcWidth, params->usMaxSrcHeight,
                                   params->usMaxDstWidth, params->usMaxDstHeight, (INT32 *)&pstMidProcCtx->stMemBuf.uiSize);

            if (iRet)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] RESIZE_QueryMem(%hu,%hu,%hu,%hu) = %d failed\n",
                           pstMidProcCtx, params->usMaxSrcWidth, params->usMaxSrcHeight, params->usMaxDstWidth, params->usMaxDstHeight, iRet);
                IMEDIA_LOG(IMEDIA_ERROR, "pfnfree mid_ctx = %p\n", pstMidProcCtx);
                IMedia_Free(pstMidProcCtx);
                pstMidProcCtx = NULL;
                return  IMEDIA_RET_OTHER;
            }

            IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] RESIZE_QueryMem(%hu,%hu,%hu,%hu) = %lu succeed\n",
                       pstMidProcCtx, params->usMaxSrcWidth, params->usMaxSrcHeight, params->usMaxDstWidth, params->usMaxDstHeight, pstMidProcCtx->stMemBuf.uiSize);

            /*malloc the memory buffer resizing module need*/
            pstMidProcCtx->stMemBuf.pucbase = IMedia_AlignMalloc(pstMidProcCtx->stMemBuf.uiSize);

            if (NULL == pstMidProcCtx->stMemBuf.pucbase)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] malloc(%lu) failed and free ctx\n", pstMidProcCtx, pstMidProcCtx->stMemBuf.uiSize);
                IMedia_Free(pstMidProcCtx);
                pstMidProcCtx = NULL;
                return  IMEDIA_RET_MEM_MALLOC_FAIL;
            }

            IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] malloc subMemBuf(%p) size(%u)\n", pstMidProcCtx, pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

            // create���̷���process�����д���
        }
        break;

        case IMAGE_DEINTERLACE:
        {
            /*inquiry about the memory buffer de-interlacing module needed*/
            pstMidProcCtx->stMemBuf.uiAlignment = 16;  //16 bit����

            iRet = DEINTERLACE_QueryMem(params->usMaxSrcWidth, params->usMaxSrcHeight, &pstMidProcCtx->stMemBuf.uiSize);

            if (iRet)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] DEINTERLACE_QueryMem(%hu, %hu) = %d failed\n", pstMidProcCtx, params->usMaxSrcWidth, params->usMaxSrcHeight, iRet);
                IMEDIA_LOG(IMEDIA_ERROR, "pfnfree mid_ctx = %p\n", pstMidProcCtx);
                IMedia_Free(pstMidProcCtx);
                pstMidProcCtx = NULL;
                return  IMEDIA_RET_OTHER;
            }

            IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] DEINTERLACE_QueryMem(%hu, %hu) = %lu succeed\n", pstMidProcCtx, params->usMaxSrcWidth, params->usMaxSrcHeight, pstMidProcCtx->stMemBuf.uiSize);

            /*malloc the memory buffer de-interlacing module needed*/
            pstMidProcCtx->stMemBuf.pucbase = IMedia_AlignMalloc(pstMidProcCtx->stMemBuf.uiSize);

            if (NULL == pstMidProcCtx->stMemBuf.pucbase)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] malloc(%lu) failed and free ctx\n", pstMidProcCtx, pstMidProcCtx->stMemBuf.uiSize);
                IMedia_Free(pstMidProcCtx);
                pstMidProcCtx = NULL;
                return  IMEDIA_RET_MEM_MALLOC_FAIL;
            }

            IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] malloc subMemBuf(%p) size(%u)\n", pstMidProcCtx, pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

            /*create de-interlace channel handler*/
            iRet = DEINTERLACE_Create(&pstMidProcCtx->pSubCtx, &pstMidProcCtx->stCpuCaps, params->usMaxSrcWidth, params->usMaxSrcHeight,
                                      pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

            if (NULL == pstMidProcCtx->pSubCtx)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] DEINTERLACE_Create = %d failed and free pucbase[%p] & ctx\n", pstMidProcCtx, iRet, pstMidProcCtx->stMemBuf.pucbase);

                /*free malloc buf*/
                IMedia_AlignFree(pstMidProcCtx->stMemBuf.pucbase);
                pstMidProcCtx->stMemBuf.pucbase = NULL;

                IMedia_Free(pstMidProcCtx);
                pstMidProcCtx = NULL;
                return  IMEDIA_RET_OBJ_CREATE_FAIL;
            }

            IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] DEINTERLACE_Create succeed sub_ctx = %p\n", pstMidProcCtx, pstMidProcCtx->pSubCtx);
        }
        break;

        default:
        {
            IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] eType wrong\n", pstMidProcCtx);
            IMedia_Free(pstMidProcCtx);
            pstMidProcCtx = NULL;
            return IMEDIA_RET_PARAM_IN_VALUE_INVALID;
        }

        //    break;
    }

    *pctx = pstMidProcCtx;

    IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] create succeed\n", pstMidProcCtx);

    return IMEDIA_RET_SUCCESS;
}

/*=============================================================================================================
* ����˵����
    ͨ�����ƺ�����
* ���������
    ctx     �㷨ͨ�����
    cmd     ���Ʋ�����������
    IMEDIA_GET_VERSION    ���ذ汾��Ϣ         wparam(STRU_IMEDIA_VERSION*)          lparam(NULL)
    wparam  ÿ�������Ӧ�������ͣ�����Ϊ��������
    lparam  ÿ�������Ӧ�������ͣ�����Ϊ��������
* ���������
    wparam  ÿ�������Ӧ�������ͣ�����Ϊ��������
    lparam  ÿ�������Ӧ�������ͣ�����Ϊ��������
* �� �� ֵ��
    0-�ɹ�������ֵ������ֵ���塣
===============================================================================================================*/
EXPORT_API INT32 IMedia_Image_Control(IMEDIA_IMAGE_CTX ctx, ENUM_IMEDIA_CMD cmd, void *wparam, void *lparam)
{
    switch (cmd)
    {
            /*���ذ汾��Ϣ*/
        case IMEDIA_GET_VERSION:
        {
            STRU_IMEDIA_VERSION *pstVersion = (STRU_IMEDIA_VERSION *)wparam;

            if (NULL == pstVersion)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "wparam NULL \n");
                return IMEDIA_RET_PARAM_NULL;
            }

            strncpy(pstVersion->cVersionChar, "iMedia MS-MID V100R001P301", IMEDIA_VERSION_LENGTH);
            strncpy(pstVersion->cReleaseTime, __TIME__ " " __DATE__, IMEDIA_TIME_LENGTH);
#if defined(_MSC_VER)
            pstVersion->uiCompileVersion = IMEDIA_FOURCC('V', 'S', '9', '0');
#else
            pstVersion->uiCompileVersion = IMEDIA_FOURCC('G', 'C', 'C', '4');
#endif
        }
        break;

        default:
        {
            IMEDIA_LOG(IMEDIA_ERROR, "invalid cmd\n");
            return IMEDIA_RET_CMD_INVALID;
        }

        //    break;
    }

    return IMEDIA_RET_SUCCESS;
}

/*=======================================================================================================================
* ����˵����
    �㷨ͨ����������
* ���������
    ctx        �㷨ͨ�����
    pstInArgs  ���������Ϣ������ͼ���ַ��֡����Ϣ�ȣ�
* ���������
    pstOutArgs ���������Ϣ�����ͼ���ַ��֡����Ϣ�ȣ�
* �� �� ֵ��
    0-�ɹ�������ֵ������ֵ���塣
=========================================================================================================================*/
EXPORT_API INT32 IMedia_Image_Process(IMEDIA_IMAGE_CTX ctx, STRU_IMAGE_IN_ARGS *pstInArgs, STRU_IMAGE_OUT_ARGS *pstOutArgs)
{
    STRU_MID_PROCESSOR_CTX *pstMidProcCtx = (STRU_MID_PROCESSOR_CTX *)ctx;

    INT32  iRet = IMEDIA_RET_SUCCESS;

    /*��������Ϸ��Լ��*/
    /*handler�ǿ�*/
    if (NULL == pstMidProcCtx)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "pMidProcCtx NULL \n");
        return IMEDIA_RET_PARAM_NULL;
    }

    /*handler��ȷ*/
    if (CRC_MASK != pstMidProcCtx->iCrcMask)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] iCrcMask[0x%x] wrong\n", pstMidProcCtx, pstMidProcCtx->iCrcMask);
        return IMEDIA_RET_OBJ_BAD_MASK;
    }

    /*����/��������ṹ��ǿ�*/
    if (NULL == pstInArgs || NULL == pstOutArgs)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] inargs or outargs NULL\n", pstMidProcCtx);
        return IMEDIA_RET_PARAM_NULL;
    }

    /* 2010/04/24 17:05:00 songxiaogang+00133955 */
    /* ��֤���������ֵʼ����Ч */
    memset(pstOutArgs, 0, sizeof(STRU_IMAGE_OUT_ARGS));

    /*����ͼ��ǿ�*/
    if (NULL == pstInArgs->pstSource)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] source NULL\n", pstMidProcCtx);
        return IMEDIA_RET_PARAM_IN_VALUE_NULL;
    }

    /*����ͼ��Y,U,Vƽ��ǿ�*/
    /* 2010/03/19 10:15:00 guoshan+00101841*/
    /* ���Ӷ�pstInArgs->pstSource->apucBuf�ķ����� */
    if (NULL == pstInArgs->pstSource->apucBuf || NULL == pstInArgs->pstSource->apucBuf[0] || NULL == pstInArgs->pstSource->apucBuf[1] || NULL == pstInArgs->pstSource->apucBuf[2])
    {
        IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] source Y || U || V buffer NULL\n", pstMidProcCtx);
        return IMEDIA_RET_PARAM_IN_VALUE_NULL;
    }

    /*���ͼ���*/
    if (NULL == pstInArgs->pucTargetBuf)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] inArgs->pucTargetBuf = NULL\n", pstMidProcCtx);
        return IMEDIA_RET_PARAM_IN_VALUE_NULL;
    }

    /*ָ�����ͼ��Y,U,Vƽ���λ��*/
    pstOutArgs->stTarget.apucBuf[0] = pstInArgs->pucTargetBuf;
    pstOutArgs->stTarget.apucBuf[1] = pstOutArgs->stTarget.apucBuf[0] + IVIDEO_Y_SIZE(pstInArgs->usTargetWidth, pstInArgs->usTargetHeight);
    pstOutArgs->stTarget.apucBuf[2] = pstOutArgs->stTarget.apucBuf[1] + IVIDEO_UV_SIZE(pstInArgs->usTargetWidth, pstInArgs->usTargetHeight);
    pstOutArgs->stTarget.eContentType = pstInArgs->pstSource->eContentType;
    pstOutArgs->stTarget.usWidth = pstInArgs->usTargetWidth;
    pstOutArgs->stTarget.usHeight = pstInArgs->usTargetHeight;
    pstOutArgs->stTarget.usWidthPitch = IVIDEO_WIDTH_STRIDE(pstInArgs->usTargetWidth);
    pstOutArgs->stTarget.stAspectRatio.usSarWidth = pstInArgs->pstSource->stAspectRatio.usSarWidth;
    pstOutArgs->stTarget.stAspectRatio.usSarHeight = pstInArgs->pstSource->stAspectRatio.usSarHeight;

    switch (pstMidProcCtx->iProcessType)
    {
        case IMAGE_DEINTERLACE:
        {
            if ((pstInArgs->usTargetWidth != pstInArgs->pstSource->usWidth) || (pstInArgs->usTargetHeight != pstInArgs->pstSource->usHeight))
            {
                IMEDIA_LOG(IMEDIA_ERROR, "outW[%d] != inW[%d]) || outH[%d] != inH[%d])\n",
                           pstInArgs->usTargetWidth, pstInArgs->pstSource->usWidth, pstInArgs->usTargetHeight, pstInArgs->pstSource->usHeight );
                //    return IMEDIA_RET_PARAM_INVALID;
                iRet = IMEDIA_RET_PARAM_INVALID;
                break;
            }

            if (pstInArgs->pstSource->eContentType != IVIDEO_INTERLACED)
            {
                //    IMEDIA_LOG(IMEDIA_WARNING, "non-interlaced frames, de-interlace continue...\n");

                /* 2010/03/17 15:50:00 guoshan+00101841*/
                /* Ӧ��ƷҪ��deinterlaceģ������ʱ�����ַ�interlaced���У�������˳��������� */
                IMEDIA_LOG(IMEDIA_ERROR, "de-interlacing non-interlaced frames...\n");
                iRet = IMEDIA_RET_PARAM_INVALID;
                break;
            }

            /*����deinterlace����������*/
            iRet = DEINTERLACE_Process(pstMidProcCtx->pSubCtx, pstInArgs->pstSource->apucBuf, pstOutArgs->stTarget.apucBuf, pstInArgs->pstSource->usWidth,
                                       pstInArgs->pstSource->usHeight, pstInArgs->pstSource->usWidthPitch, pstOutArgs->stTarget.usWidthPitch);

            /*����de-interlace������ͼ��һ����IVIDEO_PROGRESSIVE*/
            pstOutArgs->stTarget.eContentType = IVIDEO_PROGRESSIVE;
        }
        break;

        case IMAGE_RESIZE:
        {
            INT32 iCpuCap = 0;
            /*20100826 16:50:00 guoshan+00101841*/
            /*����pstInArgs->bKeepSar����resize*/
            UINT16 usDstWidth = pstOutArgs->stTarget.usWidth;
            UINT16 usDstHeight = pstOutArgs->stTarget.usHeight;
            UINT16 usDeltaDstWHalf = 0;
            UINT16 usDeltaDstHHalf = 0;
            UINT8 *apucOutYUVBuf[3];

            apucOutYUVBuf[0] = pstOutArgs->stTarget.apucBuf[0];
            apucOutYUVBuf[1] = pstOutArgs->stTarget.apucBuf[1];
            apucOutYUVBuf[2] = pstOutArgs->stTarget.apucBuf[2];

            /*����WidthStride����resizeģ��*/
            /* 2010/03/19 11:15:00 guoshan+00101841*/
            /* ��֤��SRS��һ���ԣ�ȡ�����޶� */
            //             UINT16 usSrcWMaxStride = IVIDEO_WIDTH_STRIDE(pstInArgs->pstSource->usWidth);
            //             UINT16 usDstWMaxStride = IVIDEO_WIDTH_STRIDE(pstInArgs->usTargetWidth);

            if (pstInArgs->pstSource->usWidth > pstMidProcCtx->stImageParams.usMaxSrcWidth ||
                pstInArgs->pstSource->usHeight > pstMidProcCtx->stImageParams.usMaxSrcHeight ||
                pstInArgs->usTargetWidth > pstMidProcCtx->stImageParams.usMaxDstWidth ||
                pstInArgs->usTargetHeight > pstMidProcCtx->stImageParams.usMaxDstHeight)
            {
                IMEDIA_LOG(IMEDIA_ERROR, "inW[%d] > maxInW[%d] || inH[%d] > maxInH[%d]) || outW[%d] > maxOutW[%d] || outH[%d] > maxOutH[%d]\n",
                           pstInArgs->pstSource->usWidth, pstMidProcCtx->stImageParams.usMaxSrcWidth,
                           pstInArgs->pstSource->usHeight, pstMidProcCtx->stImageParams.usMaxSrcHeight,
                           pstInArgs->usTargetWidth, pstMidProcCtx->stImageParams.usMaxDstWidth,
                           pstInArgs->usTargetHeight, pstMidProcCtx->stImageParams.usMaxDstHeight);
                //    return IMEDIA_RET_PARAM_INVALID;
                iRet = IMEDIA_RET_PARAM_INVALID;
                break;
            }

            // Ŀǰresizeֻ������·���������Ȱ����ַ�ʽ�򵥴�������֧��ָ�������Ļ����ַ����������� sdk
            if (pstMidProcCtx->stCpuCaps.uiCpuFlag & IMEDIA_CPU_SSE4)
            {
                /* 2010/03/08 16:25:00 songxg+00133955 */
                /* ��resize.cģ���еĺ괦��ʽ����һ�� */
                iCpuCap = RESIZE_CPU_MMX | RESIZE_CPU_SSE | RESIZE_CPU_SSE2 | RESIZE_CPU_SSE3 | RESIZE_CPU_SSSE3 | RESIZE_CPU_SSE4;
            }

            if ( TRUE == pstInArgs->bKeepSar)
            {
                /*20100826 16:50:00 guoshan+00101841*/
                UINT32 uiDstW = 0;
                UINT32 uiDstH = 0;
                UINT16 usSarWidth = pstInArgs->pstSource->usWidth;
                UINT16 usSarHeight = pstInArgs->pstSource->usHeight;

                iRet = Resize_CheckParam(pstInArgs->pstSource->usWidth, pstInArgs->pstSource->usHeight, pstInArgs->pstSource->usWidthPitch,
                                         pstOutArgs->stTarget.usWidth, pstOutArgs->stTarget.usHeight, pstOutArgs->stTarget.usWidthPitch);

                if (iRet)
                {
                    IMEDIA_LOG(IMEDIA_ERROR, "Resize params invalid,iRet = [%d]\n", iRet);
                    break;
                }

                memset(pstOutArgs->stTarget.apucBuf[0], 0, IVIDEO_Y_SIZE(pstInArgs->usTargetWidth, pstInArgs->usTargetHeight));
                memset(pstOutArgs->stTarget.apucBuf[1], 128, IVIDEO_UV_SIZE(pstInArgs->usTargetWidth, pstInArgs->usTargetHeight));
                memset(pstOutArgs->stTarget.apucBuf[2], 128, IVIDEO_UV_SIZE(pstInArgs->usTargetWidth, pstInArgs->usTargetHeight));

                uiDstW = usDstHeight * usSarWidth / usSarHeight;

                if ((UINT16)uiDstW > usDstWidth)
                {
                    uiDstH = usDstWidth * usSarHeight / usSarWidth;

                    if ((UINT16)uiDstH <= usDstHeight) //ҪΪ�ϡ��±�Եͬʱ��䣬����Ҫ>=2��
                    {
                        usDeltaDstHHalf = (usDstHeight - (UINT16)uiDstH) >> 1;
                        usDeltaDstHHalf = (usDeltaDstHHalf >> 1) << 1;

                        usDstHeight -= usDeltaDstHHalf << 1;
                    }
                }
                else
                {
                    if ((UINT16)uiDstW <= usDstWidth) //ҪΪ���ұ�Եͬʱ��䣬ÿ������Ҫ>=16��
                    {
                        usDeltaDstWHalf = (usDstWidth - (UINT16)uiDstW) >> 1;
                        usDeltaDstWHalf = (usDeltaDstWHalf >> 4) << 4;
                        usDstWidth -= usDeltaDstWHalf << 1;
                    }
                }

                apucOutYUVBuf[0] += usDeltaDstWHalf + usDeltaDstHHalf * pstOutArgs->stTarget.usWidthPitch;
                apucOutYUVBuf[1] += (usDeltaDstWHalf >> 1) + ((usDeltaDstHHalf * pstOutArgs->stTarget.usWidthPitch) >> 2);
                apucOutYUVBuf[2] += (usDeltaDstWHalf >> 1) + ((usDeltaDstHHalf * pstOutArgs->stTarget.usWidthPitch) >> 2);
            }
            else
            {
                // ����resize���sarΪ1:1
                pstOutArgs->stTarget.stAspectRatio.usSarWidth = 1;
                pstOutArgs->stTarget.stAspectRatio.usSarHeight = 1;
            }

            /*����ʵ������ͼ���Ŀ��ͼ��ĳߴ紴��resizeͨ��handler, ��ʡIMedia_Image_Create�ĵ��ô���*/
            /* 2010/03/05 15:25:00 guoshan+00101841*/
            /* RESIZE_Zoom()������ͼ��߶���2��������Ҫ�󣬷���������ʱ����Ҫͼ��߶���4�������� */
            if ((pstInArgs->pstSource->eContentType == IVIDEO_INTERLACED)
                && (pstInArgs->pstSource->usWidthPitch % 16 == 0)
                && (pstOutArgs->stTarget.usWidthPitch % 16 == 0)
                && (pstInArgs->pstSource->usHeight % 4 == 0)
                && (usDstHeight % 4 == 0))  //2010/08/19 guoshan+00101841 ���Ӷ�Ŀ��߶ȵ��ж�
            {
                UINT8  *apucSrcBottomBuf[3] = {NULL};
                UINT8  *apucDstBottomBuf[3] = {NULL};

                /* 2010/03/05 15:25:00 guoshan+00101841*/
                /* �޸Ĵ���resizeģ��ӿڵı������Ա�֤ͼ��usWidth��16�������� */
                /* 2010/03/19 11:15:00 guoshan+00101841*/
                /* ��֤��SRS��һ���ԣ�ȡ�����޶� */
                //                 iRet = RESIZE_Create(&pstMidProcCtx->pSubCtx,iCpuCap,(RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType,
                //                     pstInArgs->pstSource->usWidth, pstInArgs->pstSource->usHeight>>1, pstInArgs->pstSource->usWidthPitch<<1,
                //                     pstOutArgs->stTarget.usWidth, pstOutArgs->stTarget.usHeight>>1, pstOutArgs->stTarget.usWidthPitch<<1,
                //                     pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

                //                 iRet = RESIZE_Create(&pstMidProcCtx->pSubCtx,iCpuCap,(RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType,
                //                             usSrcWMaxStride, pstInArgs->pstSource->usHeight>>1, pstInArgs->pstSource->usWidthPitch<<1,
                //                             usDstWMaxStride, pstOutArgs->stTarget.usHeight>>1, pstOutArgs->stTarget.usWidthPitch<<1,
                //                             pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

                /* 2010/08/29 guoshan+00101841*/
                /* ͳһ��usDstWidth,usDstHeight����stTarget.usWidth/usHeight���нӿڵ��� */
                iRet = RESIZE_Create(&pstMidProcCtx->pSubCtx, iCpuCap, (RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType,
                                     pstInArgs->pstSource->usWidth, pstInArgs->pstSource->usHeight >> 1, pstInArgs->pstSource->usWidthPitch << 1,
                                     usDstWidth, usDstHeight >> 1, pstOutArgs->stTarget.usWidthPitch << 1,
                                     pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

                if (NULL == pstMidProcCtx->pSubCtx)
                {
                    memset(pstOutArgs, 0, sizeof(STRU_IMAGE_OUT_ARGS));
                    IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] failed to create sub_mid_ctx resize handler,iRet = [%d]\n", pstMidProcCtx, iRet);
                    //    return  IMEDIA_RET_OBJ_CREATE_FAIL;
                    break;
                }

                IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] RESIZE_Create succeed sub_ctx = %p\n", pstMidProcCtx, pstMidProcCtx->pSubCtx);

                // ���ͼ���ǳ���Ҫ�����ε���
                apucSrcBottomBuf[0] = pstInArgs->pstSource->apucBuf[0] +  pstInArgs->pstSource->usWidthPitch;
                apucSrcBottomBuf[1] = pstInArgs->pstSource->apucBuf[1] +  (pstInArgs->pstSource->usWidthPitch >> 1);
                apucSrcBottomBuf[2] = pstInArgs->pstSource->apucBuf[2] +  (pstInArgs->pstSource->usWidthPitch >> 1);

                //                 apucDstBottomBuf[0] = pstOutArgs->stTarget.apucBuf[0] +  pstOutArgs->stTarget.usWidthPitch;
                //                 apucDstBottomBuf[1] = pstOutArgs->stTarget.apucBuf[1] +  (pstOutArgs->stTarget.usWidthPitch >> 1);
                //                 apucDstBottomBuf[2] = pstOutArgs->stTarget.apucBuf[2] +  (pstOutArgs->stTarget.usWidthPitch >> 1);
                apucDstBottomBuf[0] = apucOutYUVBuf[0] +  pstOutArgs->stTarget.usWidthPitch;
                apucDstBottomBuf[1] = apucOutYUVBuf[1] +  (pstOutArgs->stTarget.usWidthPitch >> 1);
                apucDstBottomBuf[2] = apucOutYUVBuf[2] +  (pstOutArgs->stTarget.usWidthPitch >> 1);

                //                iRet = RESIZE_Zoom(pstMidProcCtx->pSubCtx, pstInArgs->pstSource->apucBuf, pstOutArgs->stTarget.apucBuf); // add return value
                /* 2010/08/29 guoshan+00101841*/
                /* ͳһ��apucOutYUVBuf����stTarget.apucBuf���нӿڵ��� */
                iRet = RESIZE_Zoom(pstMidProcCtx->pSubCtx, pstInArgs->pstSource->apucBuf, apucOutYUVBuf);

                if (iRet)
                {
                    IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] failed to resize,iRet = [%d]\n", pstMidProcCtx, iRet);
                    pstMidProcCtx->pSubCtx = NULL;
                    break;
                }

                iRet = RESIZE_Zoom(pstMidProcCtx->pSubCtx, apucSrcBottomBuf, apucDstBottomBuf);

            }
            else
            {
                /* 2010/03/05 15:25:00 guoshan+00101841*/
                /* �޸Ĵ���resizeģ��ӿڵı������Ա�֤ͼ��usWidth��16�������� */
                //                 iRet = RESIZE_Create(&pstMidProcCtx->pSubCtx,iCpuCap,(RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType,
                //                     pstInArgs->pstSource->usWidth, pstInArgs->pstSource->usHeight, pstInArgs->pstSource->usWidthPitch,
                //                     pstOutArgs->stTarget.usWidth, pstOutArgs->stTarget.usHeight, pstOutArgs->stTarget.usWidthPitch,
                //                     pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

                /* 2010/03/19 11:15:00 guoshan+00101841*/
                /* ��֤��SRS��һ���ԣ�ȡ�����޶� */
                //                 iRet = RESIZE_Create(&pstMidProcCtx->pSubCtx,iCpuCap,(RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType,
                //                                       usSrcWMaxStride, pstInArgs->pstSource->usHeight, pstInArgs->pstSource->usWidthPitch,
                //                                       usDstWMaxStride, pstOutArgs->stTarget.usHeight, pstOutArgs->stTarget.usWidthPitch,
                //                                       pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

                /* 2010/08/29 guoshan+00101841*/
                /* ͳһ��usDstWidth,usDstHeight����stTarget.usWidth/usHeight���нӿڵ��� */
                iRet = RESIZE_Create(&pstMidProcCtx->pSubCtx, iCpuCap, (RESIZE_QUALITY_LEVEL)pstMidProcCtx->stImageParams.iSubType,
                                     pstInArgs->pstSource->usWidth, pstInArgs->pstSource->usHeight, pstInArgs->pstSource->usWidthPitch,
                                     usDstWidth, usDstHeight, pstOutArgs->stTarget.usWidthPitch,
                                     pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);

                if (NULL == pstMidProcCtx->pSubCtx)
                {
                    memset(pstOutArgs, 0, sizeof(STRU_IMAGE_OUT_ARGS));
                    IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] failed to create sub_mid_ctx resize handler,iRet = [%d]\n", pstMidProcCtx, iRet);
                    //return  IMEDIA_RET_OBJ_CREATE_FAIL;
                    break;
                }

                IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] RESIZE_Create succeed sub_ctx = %p\n", pstMidProcCtx, pstMidProcCtx->pSubCtx);

                // ���ͼ����֡����Ҫ����1��
                //                iRet = RESIZE_Zoom(pstMidProcCtx->pSubCtx, pstInArgs->pstSource->apucBuf, pstOutArgs->stTarget.apucBuf); // add return value
                /* 2010/08/29 guoshan+00101841*/
                /* ͳһ��apucOutYUVBuf����stTarget.apucBuf���нӿڵ��� */
                iRet = RESIZE_Zoom(pstMidProcCtx->pSubCtx, pstInArgs->pstSource->apucBuf, apucOutYUVBuf);
            }

            pstMidProcCtx->pSubCtx = NULL;

            if (iRet)
            {
                /* 2010/03/16 12:00:00 guoshan+00101841*/
                /* ����ʧ��ʱ��ӡ��������Ϣ */
                IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] failed to resize, iRet = [%d]\n", pstMidProcCtx, iRet);
                //    return iRet;
            }

        }
        break;

        default:
        {
            iRet = IMEDIA_RET_OBJ_INVALID;
        }
        break;
    }

    if (IMEDIA_RET_SUCCESS != iRet)
    {
        memset(pstOutArgs, 0, sizeof(STRU_IMAGE_OUT_ARGS));
    }

    return iRet;
}


/*==================================================================================================================
* ����˵����
    ɾ���㷨ͨ����
* ���������
    ctx    �㷨ͨ�����
* ���������
    ��
* �� �� ֵ��
    0-�ɹ�������ֵ������ֵ���塣
====================================================================================================================*/
EXPORT_API INT32 IMedia_Image_Delete(IMEDIA_IMAGE_CTX ctx)
{
    STRU_MID_PROCESSOR_CTX *pstMidProcCtx = (STRU_MID_PROCESSOR_CTX *)ctx;

    /*check pMidProcCtx validity*/
    if (NULL == pstMidProcCtx)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "pMidProcCtx NULL\n");
        return  IMEDIA_RET_PARAM_NULL;
    }

    if (CRC_MASK != pstMidProcCtx->iCrcMask)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "mid_ctx[%p] iCrcMask[0x%x] wrong\n", pstMidProcCtx, pstMidProcCtx->iCrcMask);
        return IMEDIA_RET_OBJ_BAD_MASK;
    }

    /*free algorithm buffer*/
    if (NULL != pstMidProcCtx->stMemBuf.pucbase)
    {
        IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] free subMemBuf(%p) size(%u)\n", pstMidProcCtx, pstMidProcCtx->stMemBuf.pucbase, pstMidProcCtx->stMemBuf.uiSize);
        IMedia_AlignFree(pstMidProcCtx->stMemBuf.pucbase);
        pstMidProcCtx->stMemBuf.pucbase = NULL;
        pstMidProcCtx->stMemBuf.uiSize = 0;
        pstMidProcCtx->pSubCtx = NULL;
    }

    /*free pMidProcCtx structure buffer*/
    IMEDIA_LOG(IMEDIA_INFO, "mid_ctx[%p] free ctx\n", pstMidProcCtx);
    pstMidProcCtx->iCrcMask = 0;
    IMedia_Free(pstMidProcCtx);
    pstMidProcCtx = NULL;

    return IMEDIA_RET_SUCCESS;
}

/*guoshan+00101841 20100829*/
/*��KeepAspectRatioģʽ�£���ǰ���resizeģ��Ĳ����Ƿ�Ϸ�*/
static INT32 Resize_CheckParam( UINT16 usSrcW, UINT16 usSrcH, UINT16 usSrcPitch, UINT16 usDstW, UINT16 usDstH, UINT16 usDstPitch)
{
    // ���������
    if (usSrcW < IVIDEO_MIN_WIDTH || usSrcW > IVIDEO_MAX_WIDTH || usSrcW % WIDTH_ALIGN != 0)
    {
        IMEDIA_LOG( IMEDIA_ERROR, "usSrcW = %d is wrong!\n", usSrcW);
        return IMEDIA_RET_PARAM_INVALID;
    }

    // ���������
    if (usSrcPitch < usSrcW || usSrcPitch % WIDTH_ALIGN != 0)
    {
        IMEDIA_LOG( IMEDIA_ERROR, " usSrcPitch = %d!\n", usSrcPitch);
        return IMEDIA_RET_PARAM_INVALID;
    }

    // �������߶�
    if (usSrcH < IVIDEO_MIN_HEIGHT || usSrcH > IVIDEO_MAX_HEIGHT || usSrcH % HEIGHT_ALIGN != 0)
    {
        IMEDIA_LOG( IMEDIA_ERROR, "usSrcH = %d is wrong!\n", usSrcH);
        return IMEDIA_RET_PARAM_INVALID;
    }

    // ���������
    if (usDstW < IVIDEO_MIN_WIDTH || usDstW > IVIDEO_MAX_WIDTH || usDstW % WIDTH_ALIGN != 0)
    {
        IMEDIA_LOG( IMEDIA_ERROR, "usDstW = %d is wrong!\n", usDstW);
        return IMEDIA_RET_PARAM_INVALID;
    }

    // ���������
    if (usDstPitch < usDstW || usDstPitch % WIDTH_ALIGN != 0)
    {
        IMEDIA_LOG( IMEDIA_ERROR, " usDstPitch = %d is wrong!\n", usDstPitch);
        return IMEDIA_RET_PARAM_INVALID;
    }

    // �������߶�
    if (usDstH < IVIDEO_MIN_HEIGHT || usDstH > IVIDEO_MAX_HEIGHT || usDstH % HEIGHT_ALIGN != 0)
    {
        IMEDIA_LOG( IMEDIA_ERROR, "usDstH = %d is wrong!\n", usDstH);
        return IMEDIA_RET_PARAM_INVALID;
    }

    // ����Ƿ����²���
    if (usSrcW < usDstW || usSrcH < usDstH)
    {
        IMEDIA_LOG( IMEDIA_ERROR, "usSrcW[%d] < usDstW[%d] || usSrcH[%d] < usDstH[%d]!\n", usSrcW, usDstW, usSrcH, usDstH);
        return IMEDIA_RET_PARAM_INVALID;
    }

    return IMEDIA_RET_SUCCESS;
}

/************************************************************************************************************
* $Log$
************************************************************************************************************/
