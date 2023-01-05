
/************************************************************************
* Copyright(C), 2008-2013, Hisilicon Tech. Co., Ltd.
* All rights reserved.
*
* $Id: imedia_util.c,v 1.138 2010/01/12 09:39:22 songxiaogang Exp $
*
* Description��
*
* Reference Doc��
*
* Modification��//���ļ�β
*
*************************************************************************/

#include "iMedia_error.h"
#include "iMedia_util.h"

#ifndef __KERNEL__
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#endif

#ifdef _MSC_VER
#include  <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif

#define MSG_ERROR   "[ERROR]"
#define MSG_WARNING "[WARNING]"
#define MSG_ALERT   "[ALERT]"
#define MSG_INFO    "[INFO]"
#define MSG_DEBUG   "[DEBUG]"

extern void  IMedia_Default_Print(int level, const char *msg);

#ifndef __KERNEL__
struct STRU_IMEDIA_GLOBAL_INFO g_stGlobalInfo = {FALSE, FALSE, {IMedia_Default_Print, malloc, free}};
#else

#include <linux/slab.h>
void *IMedia_KMalloc(unsigned int size)
{
    return kmalloc(size, GFP_KERNEL);
}

void Imedia_Kfree(void *ptr)
{
    kfree(ptr);
}
struct STRU_IMEDIA_GLOBAL_INFO g_stGlobalInfo = {FALSE, FALSE, {IMedia_Default_Print, IMedia_KMalloc, Imedia_Kfree}};
#endif

#if (defined (ARCH_X86) || defined(_MSC_VER))
extern int   imedia_cpu_cpuid_test(void);
extern int   imedia_cpu_cpuid(UINT32 op, UINT32 *eax, UINT32 *ebx, UINT32 *ecx, UINT32 *edx);
/*========================================================================================
* ����˵������ȡCPU��Ϣ������
* ���������
*     caps       CPU֧��ָ����Ϣ
* ���������
* �� �� ֵ��0-�ɹ�������ֵ������ֵ���塣
=========================================================================================*/
EXPORT_API INT32 IMedia_GetCpuCaps(STRU_IMEDIA_CPU_CAPS *pstCaps)
{
    UINT32 eax, ebx, ecx, edx;
    UINT32 vendor[4] = { 0 };
    //UINT32 max_cpuid = 0;
    //UINT32 max_ext_cpuid = 0;
    //UINT32 cache = 32;

    if (NULL == pstCaps)
    { return IMEDIA_RET_PARAM_NULL; }

    memset(pstCaps, 0, sizeof(STRU_IMEDIA_CPU_CAPS));

#ifndef ARCH_X86_64

    if (!imedia_cpu_cpuid_test())
    { return 0; }

#endif

    // 1. ��ȡCPUIDָ����֧�ֵ����ֵ�ͳ��ҵ������ַ�������
    // EAX CPUIDָ����֧�ֵ����ֵ
    // EBX-EDX-ECX ���ҵ������ַ���������
    (void)imedia_cpu_cpuid(0, &eax, vendor + 0, vendor + 2, vendor + 1);

    if (eax == 0)
    { return 0; }

    IMEDIA_LOG(IMEDIA_INFO, "cpu max_cpuid %d, vendor %s\n", (eax + 1), (const char *)vendor);

    // 2. ��ȡtype/family/model/stepping����ò��ʶ����
    // EAX stepping[0-3] model[4-7] family[8-11] type[12-13] extended model[16-19] extended family[20-27]
    // EBX brand ID[0-7] CLFLUSH[8-15] CPU count[16-23] APIC ID[24-31]
    // ECX feature flags
    // EDX feature flags
    (void)imedia_cpu_cpuid(1, &eax, &ebx, &ecx, &edx);

    if (edx & 0x00800000)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_MMX;
    }
    else
    {
        return IMEDIA_RET_SUCCESS;
    }

    if (edx & 0x02000000)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_MMXEXT | IMEDIA_CPU_SSE;
    }

    if (edx & 0x04000000)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_SSE2;
    }

    if (ecx & 0x00000001)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_SSE3;
    }

    if (ecx & 0x00000200)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_SSSE3;
    }

    if (ecx & 0x00080000)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_SSE4;
    }

    if (ecx & 0x00100000)
    {
        pstCaps->uiCpuFlag |= IMEDIA_CPU_SSE42;
    }

    //cache = (ebx & 0xff00) >> 5; // cflush size
    if (!strncmp((char *)vendor, "GenuineIntel", 13))
    {
        int family = ((eax >> 8) & 0xf) + ((eax >> 20) & 0xff);
        int model  = ((eax >> 4) & 0xf) + ((eax >> 12) & 0xf0);

        //int stepping = eax & 0xf;
        /* 6/9 (pentium-m "banias"), 6/13 (pentium-m "dothan"), and 6/14 (core1 "yonah")
        * theoretically support sse2, but it's significantly slower than mmx for
        * almost all of x264's functions, so let's just pretend they don't. */
        if (family == 6 && (model == 9 || model == 13 || model == 14))
        {
            pstCaps->uiCpuFlag &= ~(IMEDIA_CPU_SSE2 | IMEDIA_CPU_SSE3);
        }
    }

    // 3. ��չָ���֧�ֵ����ֵ�ͳ�������
    //ret = imedia_cpu_cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    //max_ext_cpuid = eax;

    return IMEDIA_RET_SUCCESS;
}
#endif

/*========================================================================================
* ����˵��������ϵͳ��Ϣ��ֻ�����ڴ���亯����δ����ǰ������Ч
* ���������
*     pstSysInfo ϵͳ��Ϣ
* ���������
* �� �� ֵ��0-�ɹ�������ֵ������ֵ���塣
=========================================================================================*/
EXPORT_API INT32 IMedia_SetSysInfo(struct STRU_IMEDIA_SYSINFO *pstSysInfo)
{
    // ���������������Ƿ�Ϸ�
    if (NULL == pstSysInfo)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "pstSysInfo NULL\n");
        return IMEDIA_RET_PARAM_NULL;
    }

    if (NULL == pstSysInfo->pfnMalloc)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "pfnMalloc NULL\n");
        return IMEDIA_RET_PARAM_IN_VALUE_NULL;
    }

    if (NULL == pstSysInfo->pfnFree)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "pfnFree NULL\n");
        return IMEDIA_RET_PARAM_IN_VALUE_NULL;
    }

    if (g_stGlobalInfo.bMallocFxnUsed)
    {
        IMEDIA_LOG(IMEDIA_ERROR, "g_stGlobalInfo.bMallocFxnUsed = true\n");
        return IMEDIA_RET_OBJ_INVALID;
    }

    g_stGlobalInfo.stSysInfo.pfnPrintf = pstSysInfo->pfnPrintf;
    g_stGlobalInfo.stSysInfo.pfnMalloc = pstSysInfo->pfnMalloc;
    g_stGlobalInfo.stSysInfo.pfnFree = pstSysInfo->pfnFree;

    g_stGlobalInfo.bIsExternalSet = TRUE;
    return IMEDIA_RET_SUCCESS;
}

#ifndef __KERNEL__
EXPORT_API const char  *IMedia_OS_TimeString()
{
    static char acSystimeString[64] = { 0 };

    time_t curSystime = time(NULL);
    struct tm *pLocalTime = localtime(&curSystime);

    snprintf(acSystimeString, 64, "%04d-%02d-%02d %02d:%02d:%02d",
             pLocalTime->tm_year + 1900,
             pLocalTime->tm_mon + 1,
             pLocalTime->tm_mday,
             pLocalTime->tm_hour,
             pLocalTime->tm_min,
             pLocalTime->tm_sec);

    return acSystimeString;
}
#endif

EXPORT_API UINT64  IMedia_OS_Microseconds(void)
{
    UINT64 ret = 0;

#if defined(_MSC_VER)
    UINT64 counter;
    UINT64 frequency;

    //ret = (INT64)timeGetTime();
    QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
    QueryPerformanceCounter((LARGE_INTEGER *)&counter);
    ret = counter * 1000 / (frequency / 1000);
#elif defined(__GNUC__)
    struct timeval t;
    //struct timezone tz;

#if defined(__KERNEL__)
    do_gettimeofday(&t);
#else
    gettimeofday(&t, NULL /* &tz */);
#endif

    ret = (UINT64)t.tv_sec * 1000000 + t.tv_usec;
#endif

    return ret;
}

EXPORT_API UINT64 IMedia_OS_Milliseconds(void)
{
    UINT64 ret = 0;

#if defined(_MSC_VER)
    ret = (UINT64)timeGetTime();
#elif defined(__GNUC__)
    struct timeval t;
    //struct timezone tz;

#if defined(__KERNEL__)
    do_gettimeofday(&t);
#else
    gettimeofday(&t, NULL /* &tz */);
#endif

    ret = (UINT64)t.tv_sec * 1000 + t.tv_usec / 1000;
#endif

    return ret;
}

EXPORT_API UINT32 IMedia_OS_Milliseconds32(void)
{
    return (UINT32)(IMedia_OS_Milliseconds() & 0xFFFFFFFF);
}

EXPORT_API UINT32 IMedia_ValueOffset32(UINT32 begin, UINT32 end)
{
    if (end >= begin)
    { return end - begin; }

    /*ƫ���С�����*/
    if ((begin - end) < 0x1FFFF)
    { return begin - end; }

    /* �����*/
    return 0xFFFFFFFF - begin + end + 1;
}

EXPORT_API void IMedia_Sleep(UINT32 ms)
{
    if (ms == 0)
    { return; }

    /* >10 s*/
    if (ms > 10000)
    { ms = 10000; }

#if defined( _MSC_VER)
    Sleep(ms);
#elif defined(__GNUC__)
    //usleep(ms * 1000);
    /*
    {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = ms * 1000000;
        nanosleep(&ts, NULL);
    }*/
    /*taskDelay(sysClkRateGet()*ms/1000);*/
#endif
}

void  IMedia_Default_Print(int level, const char *msg)
{
}

/*========================================================================================
* ����˵������־��ӡ�������
* ���������
*     level ����־�ȼ�����error��warning��info��
*     format����ӡ�ַ�������
* ���������
* �� �� ֵ��
=========================================================================================*/
#if defined(__GNUC__)
static char *FindFileShortName(const char *filename)
{
    const char *pret = NULL;
    const char *pnext = NULL;

    if (NULL == filename)
    { return NULL; }

    pret = filename;

    while ((pnext = strrchr(pret, '/')) != NULL)
    { pret = pnext + 1; }

    return (char *)pret;
}

EXPORT_API void IMedia_msg_format(const char *file, int line, const char *funcname, int level, const char *format, ...)
#else
EXPORT_API void IMedia_msg_format(int level, const char *format, ...)
#endif
{
    char msg[512] = { 0 };
    va_list ap;

#if defined(__GNUC__)
    static const char *pLastFile = NULL;
    static int   print_prefix_flag = 1;

    if (print_prefix_flag)
    { snprintf(msg, 512, "<%s:%d::%s> ", FindFileShortName(file), line, funcname); }

#endif

    va_start(ap, format);
    vsnprintf(msg + strlen(msg), 512 - strlen(msg), format, ap);
    va_end(ap);

#if defined(__GNUC__)
    print_prefix_flag = (pLastFile != file || msg[strlen(msg) - 1] == '\n');
    pLastFile = file;
#endif

    if (NULL != g_stGlobalInfo.stSysInfo.pfnPrintf)
    {
        g_stGlobalInfo.stSysInfo.pfnPrintf(level, msg);
    }
}

#if 0
/*****************************************************************************************
* ����˵���������ַ�����ȡ������
* ���������
*     iErrorCode   ģ���ڲ�������
* ���������
* �� �� ֵ��������Ӧ���ַ�����
*****************************************************************************************/
EXPORT_API INT32 IMedia_GetErrorString(INT32 iErrorCode, char *pszRet, INT32 iLen)
{
    UINT8 ucCodecID = 0;
    UINT8 ucSEQ = 0;
    UINT8 ucPic = 0;
    UINT8 ucSlice = 0;
    UINT8 ucMB = 0;
    UINT8 ucResidual = 0;
    UINT8 ucReserved = 0;

    ucCodecID = IMEDIA_GET_ERR_CODEC_ID(iErrorCode);
    ucSEQ = IMEDIA_GET_ERR_SEQ(iErrorCode);
    ucPic = IMEDIA_GET_ERR_PIC(iErrorCode);
    ucSlice = IMEDIA_GET_ERR_SLICE(iErrorCode);
    ucMB = IMEDIA_GET_ERR_MB(iErrorCode);
    ucResidual = IMEDIA_GET_ERR_RESIDUAL(iErrorCode);
    ucReserved = IMEDIA_GET_ERR_RESERVED(iErrorCode);

    if (iLen > 63)
    {
        snprintf(pszRet, 66, "id:%d seq:%d pic:%d slice:%d MB:%d Residual:%d Reserved:%d",
                 ucCodecID, ucSEQ, ucPic, ucSlice, ucMB, ucResidual, ucReserved);
    }

    return IMEDIA_RET_SUCCESS;
}
#endif

/*****************************************************************************************
* ����˵�����ڴ���亯����
* ���������
*     size  �����ڴ泤��
* ���������
* �� �� ֵ��ʵ���ڴ�ռ��ַ����ʧ��-NULL��
*****************************************************************************************/
EXPORT_API void *IMedia_Malloc(unsigned int size)
{
    return g_stGlobalInfo.stSysInfo.pfnMalloc(size);
}

EXPORT_API void *IMedia_AlignMalloc(unsigned int size)
{
    void *ptr = NULL;
    char diff = 0;

    /* let's disallow possible ambiguous cases */
    if (size > (INT_MAX - 16) )
    { return NULL; }

    ptr = IMedia_Malloc(size + 16);

    if (NULL == ptr)
    { return ptr; }

    diff = ((-(long)ptr - 1) & 15) + 1;
    ptr  = (char *)ptr + diff;
    ((char *)ptr)[-1] = diff;

    return ptr;
}

/*****************************************************************************************
* ����˵�����ڴ��ͷź�����
* ���������
*     ptr   �ڴ��ַ
* ���������
* �� �� ֵ���ޡ�
*****************************************************************************************/
EXPORT_API void  IMedia_Free(void *ptr)
{
    if (ptr)
    {
        g_stGlobalInfo.stSysInfo.pfnFree(ptr);
    }
}

EXPORT_API void  IMedia_AlignFree(void *ptr)
{
    if (ptr)
    {
        IMedia_Free((char *)ptr - ((char *)ptr)[-1]);
    }
}

/************************************************************************
* $Log$
************************************************************************/
