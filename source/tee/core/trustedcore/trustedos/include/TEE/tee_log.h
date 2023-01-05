/**
* @file tee_log.h
*
* Copyright(C), 2008-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
*
* ??:??????log????\n
*/

/** @defgroup grp_log_api ??????
* @ingroup TEE_API
*/
#ifndef __TEE_LOG_H
#define __TEE_LOG_H

#include "tee_internal_api.h"

#define DEBUG_TAG	    "[debug]"
#define INFO_TAG	    "[info]"
#define WARNING_TAG	    "[warning]"
#define ERROR_TAG	    "[error]"

#define LEVEL_DEBUG   2
#define LEVEL_WARNING   1
#define LEVEL_ERROR    0

#define TAG_VERB	    "[verb]"
#define TAG_DEBUG	    "[debug]"
#define TAG_INFO	    "[info]"
#define TAG_WARN	    "[warn]"
#define TAG_ERROR	    "[error]"

typedef enum {
	LOG_LEVEL_ERROR = 0,
	LOG_LEVEL_WARN = 1,
	LOG_LEVEL_INFO = 2,
	LOG_LEVEL_DEBUG = 3,
	LOG_LEVEL_VERBO = 4,
	LOG_LEVEL_ON = 5,
} LOG_LEVEL;

#define LOG_LEVEL_MAX	4

void uart_cprintf(const char *fmt, ...);
void uart_printf_func(const char *fmt, ...);
void hex_dump(const char *des, const char *start, unsigned length);

void tee_print(LOG_LEVEL log_level, const char *fmt, ...);

#ifdef LOG_ON
#define tlogv(fmt, args...) tee_print(LOG_LEVEL_ON, "%s %s:" fmt "", TAG_VERB, __FUNCTION__, ## args)
#define tlogd(fmt, args...) tee_print(LOG_LEVEL_ON, "%s %s:" fmt "", TAG_DEBUG, __FUNCTION__, ## args)
#define tlogi(fmt, args...) tee_print(LOG_LEVEL_ON, "%s %s:" fmt "", TAG_INFO, __FUNCTION__, ## args)
#define tlogw(fmt, args...) tee_print(LOG_LEVEL_ON, "%s %s:" fmt "", TAG_WARN, __FUNCTION__, ## args)
#else
#define tlogv(fmt, args...) tee_print(LOG_LEVEL_VERBO, "%s %s:" fmt "", TAG_VERB, __FUNCTION__, ## args)
#define tlogd(fmt, args...) tee_print(LOG_LEVEL_DEBUG, "%s %s:" fmt "", TAG_DEBUG, __FUNCTION__, ## args)
#define tlogi(fmt, args...) tee_print(LOG_LEVEL_INFO, "%s %s:" fmt "", TAG_INFO, __FUNCTION__, ## args)
#define tlogw(fmt, args...) tee_print(LOG_LEVEL_WARN, "%s %s:" fmt "", TAG_WARN, __FUNCTION__, ## args)
#endif
#define tloge(fmt, args...) tee_print(LOG_LEVEL_ERROR, "%s %s: " fmt " ", TAG_ERROR,__FUNCTION__, ## args)

void ta_print(LOG_LEVEL log_level,const char *fmt, ...);

#ifdef LOG_ON
#define ta_logv(fmt, args...) ta_print(LOG_LEVEL_ON, "%s %s: " fmt "\n", TAG_VERB, __FUNCTION__, ## args)
#define ta_logd(fmt, args...) ta_print(LOG_LEVEL_ON, "%s %s: " fmt "\n", TAG_DEBUG, __FUNCTION__, ## args)
#define ta_logi(fmt, args...) ta_print(LOG_LEVEL_ON, "%s %s: " fmt "\n", TAG_INFO, __FUNCTION__, ## args)
#define ta_logw(fmt, args...) ta_print(LOG_LEVEL_ON, "%s %s: " fmt "\n", TAG_WARN, __FUNCTION__, ## args)
#else
#define ta_logv(fmt, args...) ta_print(LOG_LEVEL_VERBO, "%s %s: " fmt "\n", TAG_VERB, __FUNCTION__, ## args)
#define ta_logd(fmt, args...) ta_print(LOG_LEVEL_DEBUG, "%s %s: " fmt "\n", TAG_DEBUG, __FUNCTION__, ## args)
#define ta_logi(fmt, args...) ta_print(LOG_LEVEL_INFO, "%s %s: " fmt "\n", TAG_INFO, __FUNCTION__, ## args)
#define ta_logw(fmt, args...) ta_print(LOG_LEVEL_WARN, "%s %s: " fmt "\n", TAG_WARN, __FUNCTION__, ## args)
#endif
#define ta_loge(fmt, args...) ta_print(LOG_LEVEL_ERROR, "%s %s: " fmt "\n", TAG_ERROR, __FUNCTION__, ## args)


/* in debug version users can dynamically modify the loglevel ,in release version, users have to modify the level by compile */
#ifndef DEBUG_VERSION

//#define TA_DEBUG
#ifdef TA_DEBUG
#define ta_debug(fmt, args...) uart_printf_func("%s %s: " fmt "", DEBUG_TAG,__FUNCTION__, ## args)
#else
#define ta_debug(fmt, args...)
#endif

#else

#define ta_debug(fmt, args...) \
    do {\
        uint32_t level;\
        level = get_value();\
        if(level>=LEVEL_DEBUG){\
           uart_printf_func("%s %s: " fmt "", DEBUG_TAG,__FUNCTION__, ## args); \
        }\
    } while (0)

#define ta_warning(fmt, args...)\
    do {\
        uint32_t level;\
        level = get_value();\
        if(level>=LEVEL_WARNING){\
            uart_printf_func("%s %s: " fmt "", DEBUG_TAG,__FUNCTION__, ## args); \
        }\
    } while (0)

#endif

#define ta_info(fmt, args...) uart_printf_func("%s %s: " fmt "", INFO_TAG,__FUNCTION__, ## args)
#define ta_error(fmt, args...) uart_printf_func("%s %s: " fmt " ", ERROR_TAG,__FUNCTION__, ## args)
#define TA_LOG
#ifdef TA_LOG
/**
 * @ingroup grp_log_api
 * ?????Trace
 */
#define TRACE "[Trace]"
/**
 * @ingroup grp_log_api
 * ?????Warning
 */
#define WARNING "[Warning]"
/**
 * @ingroup grp_log_api
 * ?????Error
 */
#define ERROR "[Error]"

/**
 * @ingroup grp_log_api
 * ??Trace?????
 */
#define SLogTrace(fmt, args...) SLog("%s %s: " fmt "\n", TRACE, __FUNCTION__, ## args)
/**
 * @ingroup grp_log_api
 * ??Warning?????
 */
#define SLogWarning(fmt, args...) SLog("%s %s: " fmt "\n", WARNING, __FUNCTION__, ## args)
/**
 * @ingroup grp_log_api
 * ??Error?????
 */
#define SLogError(fmt, args...) SLog("%s %s: " fmt "\n", ERROR, __FUNCTION__, ## args)

//TODO: SHOULD call Panic to deal, here just return
/**
 * @ingroup grp_log_api
 * ????
 */
#define SAssert(exp) \
    do {    \
        if (!(exp)) {   \
            SLog("Assertion [ %s ] Failed: File %s, Line %d, Function %s\n",   \
            #exp, "__FILE__", __LINE__, __FUNCTION__);    \
            return 0xFFFF0001;    \
        }   \
    } while(0);
#else
#define SLogTrace(fmt, args...) ((void)0)
#define SLogWarning(fmt, args...) ((void)0)
#define SLogError(fmt, args...) ((void)0)
#define SAssert(exp)  ((void)0)
#endif

/**
* @ingroup  grp_log_api
* @brief ????????
*
* @par ??:
* ?
*
* @attention ?
* @param fmt [IN] ????????
*
* @retval #?
*
* @par ??:
* @li tee_internal_api.h:????????
**/
void SLog(const char *fmt, ...);
uint32_t get_value();

/**
* @ingroup  grp_log_api
* @brief ??????????
*
* @par ??:
* ?????????????????,????????????,
* ??????????
*
* @attention ?
* @param fmt [IN] ?
*
* @retval #?
*
* @par ??: ?
**/
void __dump_backtraces(void);
#endif
