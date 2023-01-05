/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_file.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2016��07��29��
  ����޸�   :
  ��������   : oal_file.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��07��29��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __OAL_LITEOS_FILE_H__
#define __OAL_LITEOS_FILE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include <linux/fs.h>
#include <fs/fs.h>
#include <fcntl.h>
#include "oal_mm.h"
#include <oal_types.h>

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/*�ļ�����*/
#define OAL_O_ACCMODE           O_ACCMODE
#define OAL_O_RDONLY            O_RDONLY
#define OAL_O_WRONLY            O_WRONLY
#define OAL_O_RDWR              O_RDWR
#define OAL_O_CREAT             O_CREAT
#define OAL_O_TRUNC             O_TRUNC
#define OAL_O_APPEND            O_APPEND
#define OAL_O_EXCL              O_EXCL  /* not fcntl */
#define OAL_O_NOCTTY            O_NOCTTY    /* not fcntl */
#define OAL_O_NOCTTY            O_NONBLOCK  
#define OAL_O_DSYNC             O_DSYNC          /* used to be O_SYNC, see below */
#define OAL_FASYNC              FASYNC            /* fcntl, for BSD compatibility */
#define OAL_O_DIRECT            O_DIRECT        /* direct disk access hint */
#define OAL_O_LARGEFILE         O_LARGEFILE  
#define OAL_O_DIRECTORY         O_DIRECTORY      /* must be a directory */
#define OAL_O_NOFOLLOW          O_NOFOLLOW        /* don't follow links */
#define OAL_O_NOATIME           O_NOATIME  
#define OAL_O_CLOEXEC           O_CLOEXEC      /* set close_on_exec */


#define OAL_SEEK_SET      SEEK_SET     /* Seek from beginning of file.  */
#define OAL_SEEK_CUR     SEEK_CUR    /* Seek from current position.  */
#define OAL_SEEK_END     SEEK_END    /* Set file pointer to EOF plus "offset" */

#define OAL_FILE_POS(pst_file)      oal_get_file_pos(pst_file)


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

typedef struct _oal_file_stru_{
    int fd;
}oal_file_stru;


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
 �� �� ��  : oal_file_open
 ��������  : ���ļ�
 �������  : pc_path: �ļ�·����flags:�򿪷�ʽ,rights:��Ȩ��
 �������  : ��
 �� �� ֵ  : �ļ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��8��3��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_file_stru* oal_file_open(const oal_int8 *pc_path,oal_int32 flags,oal_int32 rights)
{
    oal_file_stru   *pst_file = NULL;
    pst_file = calloc(1, sizeof(oal_file_stru));
    if (pst_file == NULL)
        return NULL;
    pst_file->fd = open(pc_path, flags, rights);
    if (pst_file->fd < 0)
    {
        free(pst_file);
        return NULL;
    }
    return pst_file;
}

/*****************************************************************************
 �� �� ��  : oal_file_write
 ��������  : д�ļ�
 �������  : file: �ļ����
           : pc_string: �������ݵ�ַ
           : ul_length: �������ݳ���
 �������  : ��
 �� �� ֵ  : �ļ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_uint32 oal_file_write(oal_file_stru *pst_file, oal_int8 *pc_string, oal_uint32 ul_length)
{
    return write(pst_file->fd,pc_string,ul_length);
}

/*****************************************************************************
 �� �� ��  : oal_file_close
 ��������  : �ر��ļ�
 �������  : pc_path: �ļ�·��
 �������  : ��
 �� �� ֵ  : �ļ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��14��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_void oal_file_close(oal_file_stru *pst_file)
{
    close(pst_file->fd);
    free(pst_file);
    pst_file= NULL;
}

/*****************************************************************************
 �� �� ��  : oal_file_read
 ��������  : �ں˶��ļ�����ͷ��ʼ��
 �������  : file:ָ��Ҫ��ȡ���ļ���ָ��
             puc_buf:���ļ��������ݺ��ŵ�buf
             ul_count:ָ��Ҫ��ȡ�ĳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��10��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_int32  oal_file_read(oal_file_stru *pst_file,
                                                 oal_int8 *pc_buf,
                                                 oal_uint32 ul_count)
{
    return read(pst_file->fd, pc_buf, ul_count);
}

OAL_STATIC OAL_INLINE oal_int64  oal_file_lseek(oal_file_stru *pst_file,oal_int64 offset,oal_int32 whence)
{
    return lseek(pst_file->fd, offset, whence);
}

/*****************************************************************************
 �� �� ��  : oal_file_size
 ��������  : ��ȡ�ļ���С
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��4��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
OAL_STATIC OAL_INLINE oal_int32  oal_file_size(const oal_int8 *pc_path, oal_uint32   *pul_file_size)
{
    oal_file_stru     *p_file;

    p_file = oal_file_open(pc_path,(OAL_O_RDONLY),0);
    if (OAL_PTR_NULL == p_file)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }
    *pul_file_size = oal_file_lseek(p_file,0,OAL_SEEK_END);
    oal_file_close(p_file);

    return OAL_SUCC;
}


OAL_STATIC OAL_INLINE oal_int32  oal_get_file_pos(oal_file_stru *pst_file)
{
    struct file *fp = NULL;
    fp = fs_getfilep(pst_file->fd);
    return (int)fp->f_pos;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_file.h */
