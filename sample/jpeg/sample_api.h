/******************************************************************************

  Copyright (C), 2013-2020, Hisilicon. Co., Ltd.

******************************************************************************
File Name        : sample_api.h
Version            : Initial Draft
Author            :
Created            : 2013/08/01
Description        : ��ͬ������ʹ�ð���
                  (1) ��������:  �ļ�              ���buffer:  �����ڴ�
                  (2) ��������:  �ļ�              ���buffer:  �����ڴ�
                  (3) ��������:  �ļ�              ���buffer:  �������
                  (4) ��������:  ���������ڴ�      ���buffer:  �����ڴ�
                  (5) ��������:  ���������ڴ�      ���buffer:  �����ڴ�
                  (6) ��������:  ���������ڴ�      ���buffer:  �������
                  (7) ��������:  �����ڴ�          ���buffer:  �����ڴ�
                  (8) ��������:  �����ڴ�          ���buffer:  �����ڴ�
                  (9) ��������:  �����ڴ�          ���buffer:  �������

Function List     :


History           :
Date                Author                Modification
2013/08/01            y00181162              Created file
******************************************************************************/


#ifndef __SAMPLE_API_H__
#define __SAMPLE_API_H__


/*********************************add include here******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <setjmp.h>

#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "jpeglib.h"
#include "jerror.h"
#include "hifb.h"


#include "hi_unf_disp.h"
#include "hi_adp_mpi.h"

/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
       extern "C"
    {
#endif
#endif /* __cplusplus */


         /***************************** Macro Definition ******************************/

        #define SAMPLE_TRACE( fmt, args... )\
        do { \
                fprintf(stderr,fmt, ##args );\
        } while (0)


        #define SAMPLE_DEC_SETJMP_BUFFER           setjmp_buffer
        #define SAMPLE_DEC_ERR_FUCNTION            test_dec_err
        #define DECLARE_SAMPLE_DEC_SETJMP() \
                static jmp_buf  SAMPLE_DEC_SETJMP_BUFFER

        #define DECLARE_SAMPLE_DEC_ERR_FUNC() \
            static HI_VOID SAMPLE_DEC_ERR_FUCNTION(j_common_ptr cinfo)\
            {\
                 (*cinfo->err->output_message) (cinfo);  \
                longjmp(SAMPLE_DEC_SETJMP_BUFFER, 1);\
            }


        #define SAMPLE_DEC_JPEG_FILE_DIR     "./../res/jpg/"
        #define SAMPLE_DEC_JPEG_FILE         "./../res/jpg/image1.jpg"

        //#define SAMPLE_DEC_DIR

        /*************************** Enum Definition ****************************/

        /*************************** Structure Definition ****************************/

        typedef struct tagJPEG_MYERR_S
        {
            struct jpeg_error_mgr pub;

        }JPEG_MYERR_S,*JPEG_MYERR_S_PTR;



        /********************** Global Variable declaration **************************/


        static struct fb_var_screeninfo fb_vinfo_BGR888 =
        {
            1280,720,1280,720,0,0,
            24,
            0,
            { 16, 8, 0},
            { 8, 8, 0},
            { 0, 8, 0},
            {0, 0, 0},
            0,FB_ACTIVATE_FORCE,0,0,0,-1,-1,-1,-1,-1,-1,-1,
        };

        /******************************* API declaration *****************************/


        /*****************************************************************************
        * func            : sample_get_file_size
        * description    : CNcomment: ��ȡ�ļ��������ȴ�С  CNend\n
        * param[in]     : pInFile            CNcomment: �򿪵��ļ���       CNend\n
        * param[out]    : *u64FileSize        CNcomment: �ļ�����           CNend\n
        * retval        : HI_SUCCESS
        * retval        : HI_FAILURE
        * others:        : NA
        *****************************************************************************/
        static inline HI_S32 sample_get_file_size(FILE* pInFile,HI_U64 *u64FileSize)
        {
              /** ��ȡ�ļ��Ĵ�С **/
              if (fseek(pInFile, 0, SEEK_END) != 0)
              {
                    return HI_FAILURE;
              }
              /** ��ȡ��ǰ�ļ����ڵ�λ��**/
              *u64FileSize = ftell(pInFile);
              /* �ж�֡�����Ƿ����ļ���,��Ϊ�����ļ�����Ƶ��,�кö�֡ */
              if (*u64FileSize <= 0)
              {
                    return HI_FAILURE;
              }
              /** �ص��ļ�ͷ  **/
              if (fseek(pInFile, 0, SEEK_SET) != 0)
              {
                    return HI_FAILURE;
              }
              return HI_SUCCESS;

        }

        /*****************************************************************************
        * func            : sample_getjpegfilename
        * description    : CNcomment: ��ȡ�ļ�����  CNend\n
        * param[in]     : pInFile            CNcomment: �򿪵��ļ���       CNend\n
        * param[out]    : *u64FileSize        CNcomment: �ļ�����           CNend\n
        * retval        : HI_SUCCESS
        * retval        : HI_FAILURE
        * others:        : NA
        *****************************************************************************/
        static inline HI_S32 sample_getjpegfilename(HI_CHAR *pFileName, HI_CHAR *pDstName)
        {
             HI_CHAR *pPos;
             HI_CHAR *pName = pFileName;

             pPos = strrchr(pFileName, '.');
             if (NULL == pPos)
             {
                return HI_FAILURE;
             }
             pPos++;
             if (strncasecmp(pPos,"jpeg",2) != 0)
             {
                return HI_FAILURE;
             }

             memset(pDstName,0,256);
             strncpy(pDstName, SAMPLE_DEC_JPEG_FILE_DIR,strlen(SAMPLE_DEC_JPEG_FILE_DIR));
             strncat(pDstName,pName,strlen(pName));

             return HI_SUCCESS;
        }


        /*****************************************************************************
        * func            : test_dec_show
        * description    : CNcomment: ��ʾ��������  CNend\n
        * retval        : HI_SUCCESS
        * retval        : HI_FAILURE
        * others:        : NA
        *****************************************************************************/
        static inline HI_S32 test_dec_show(HI_U32 u32Width,HI_U32 u32Height,HI_S32 s32Stride,HI_CHAR * pBuffer,J_COLOR_SPACE ColorSpace)
        {

                int ret = 0;
                struct fb_fix_screeninfo finfo;
                struct fb_var_screeninfo vinfo;
                HIFB_ALPHA_S stAlpha;
                int console_fd;
                const char* file = "/dev/fb0";

                HI_VOID* mapped_mem  = NULL;
                unsigned long mapped_offset = 0;
                unsigned long mapped_memlen = 0;

                HI_U32 u32Offset   = 0;
                HI_U32 u32YOffset  = 0;

                HI_S32 s32DstStride = 0;

                console_fd = open(file, O_RDWR, 0);
                if (console_fd < 0)
                {
                    SAMPLE_TRACE("Unable to open %s\n", file);
                    return (-1);
                }
                switch(ColorSpace)
                {

                        //case JCS_RGB:
                        //case JCS_YCbCr:
                        case JCS_BGR:
                        //case JCS_CrCbY:
                        //case  JCS_RGB_565:
                        //case  JCS_BGR_565:
                        //case  JCS_ARGB_1555:
                        //case  JCS_ABGR_1555:
                        //case  JCS_ARGB_8888:
                        //case    JCS_ABGR_8888:
                            if (ioctl(console_fd, FBIOPUT_VSCREENINFO, &fb_vinfo_BGR888) < 0)
                            {
                                SAMPLE_TRACE("Unable to set variable screeninfo!\n");
                                ret = -1;
                                goto CLOSEFD;
                            }
                            break;
                      default:
                            SAMPLE_TRACE("not this color space show\n");
                            goto CLOSEFD;
                            break;

                }

                /* Get the type of video hardware */
                if (ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0)
                {
                    SAMPLE_TRACE("Couldn't get console hardware info\n");
                    ret = -1;
                    goto CLOSEFD;
                }

                mapped_memlen = finfo.smem_len + mapped_offset;
                mapped_mem = mmap(NULL, mapped_memlen,
                                  PROT_READ | PROT_WRITE, MAP_SHARED, console_fd, 0);
                if (mapped_mem == (char *)-1)
                {
                    SAMPLE_TRACE("Unable to memory map the video hardware\n");
                    mapped_mem = NULL;
                    ret = -1;
                    goto CLOSEFD;
                }

                /* Determine the current screen depth */
                if (ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0)
                {
                    SAMPLE_TRACE("Couldn't get console pixel format\n");
                    ret = -1;
                    goto UNMAP;
                }

                /* set alpha */
                stAlpha.bAlphaEnable  = HI_TRUE;
                stAlpha.bAlphaChannel = HI_FALSE;
                stAlpha.u8Alpha0 = 0xff;
                stAlpha.u8Alpha1 = 0xff;
                if (ioctl(console_fd, FBIOPUT_ALPHA_HIFB, &stAlpha) < 0)
                {
                    SAMPLE_TRACE("Couldn't set alpha\n");
                    ret = -1;
                    goto UNMAP;
                }

                memset(mapped_mem, 0x00, finfo.smem_len);


                /*****************************************************/

                switch(ColorSpace)
                {
                   case JCS_RGB:
                   case JCS_BGR:
                   case JCS_YCbCr:
                   case JCS_CrCbY:
                        s32DstStride = 1280*3;
                        u32Offset  = (100 * 1280 * 3);
                        u32YOffset = (100 * 3);
                        break;
                    default:
                        SAMPLE_TRACE("not this color space show\n");
                        break;

                }

                int ii = 0;
                HI_UCHAR *pBufferSrc = (HI_UCHAR*)pBuffer;
                HI_UCHAR *pBufferDst = mapped_mem;

                pBufferDst = pBufferDst + u32Offset + u32YOffset;

                if(    (finfo.smem_len - u32Offset - u32YOffset) >= (s32Stride * u32Height)
                    && (finfo.smem_len - u32Offset - u32YOffset) >= (s32DstStride * u32Height))
                {
                    for (ii = 0; ii < u32Height; ii++)
                    {
                       memcpy(pBufferDst, pBufferSrc, s32Stride);
                       pBufferDst += s32DstStride;
                       pBufferSrc += s32Stride;
                    }
               }

                if (ioctl(console_fd, FBIOPAN_DISPLAY, &vinfo) < 0)
                    {
                        printf("pan_display failed!\n");
                        close(console_fd);
                        return -1;
                    }

               sleep(5);

            UNMAP:
                munmap(mapped_mem, mapped_memlen);

            CLOSEFD:
                if(console_fd > 0)
                {
                    close(console_fd);
                }
                return ret;

        }

        /*****************************************************************************
        * func            : Display_Init
        * description    : VO��ʼ��
        * param[in]     :
        * retval        : NA
        * others:        : NA
        *****************************************************************************/
        static inline HI_S32 Display_Init()
        {
            HI_SYS_Init();
            HIADP_MCE_Exit();
            HIADP_Disp_Init(HI_UNF_ENC_FMT_720P_50);
            return HI_SUCCESS;
        }

        /*****************************************************************************
        * func            : Display_DeInit
        * description    : VOȥ��ʼ��
        * param[in]     :
        * retval        : NA
        * others:        : NA
        *****************************************************************************/
        static inline HI_S32 Display_DeInit()
        {
            HIADP_Disp_DeInit();
            HI_SYS_DeInit();
            return  0;
        }


#ifdef __cplusplus

#if __cplusplus

        }
#endif
#endif /* __cplusplus */


#endif /* __SAMPLE_API_H__*/
