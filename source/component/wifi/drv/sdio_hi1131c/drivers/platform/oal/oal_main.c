/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

 ******************************************************************************
  �� �� ��   : oal_main.c
  �� �� ��   : ����
  ��    ��   : ������
  ��������   : 2012��9��18��
  ����޸�   :
  ��������   : OALģ���ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ļ�

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oal_main.h"
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
#include "oal_pci_if.h"
#endif
#include "oal_mem.h"
#include "oal_schedule.h"
#include "oal_net.h"

#include "oal_hcc_host_if.h"
#include "oal_kernel_file.h"

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
#ifndef WIN32
#include "plat_firmware.h"
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)||(_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
void __iomem *g_l2cache_base;
#endif

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : oal_main_init
 ��������  : OALģ���ʼ������ڣ�����OALģ���ڲ��������Եĳ�ʼ����
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ʼ������ֵ���ɹ���ʧ��ԭ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_int32  ATTR_OAL_NO_FUNC_TRACE oal_main_init(oal_void)
{
    oal_uint32  ul_rslt;
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    if(NULL == oal_conn_sysfs_root_obj_init())
    {
        OAL_IO_PRINT("hisi root sysfs init failed\n");
    }
#endif

    /* Ϊ�˽��ģ�������ʱ�䣬����ʱ�����ӡ */

    /* �ڴ�س�ʼ�� */
    ul_rslt = oal_mem_init_pool();
    if (ul_rslt != OAL_SUCC)
    {
        OAL_IO_PRINT("oal_main_init: oal_mem_init_pool return error code: %d", ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

#if (_PRE_PRODUCT_ID_HI1151 == _PRE_PRODUCT_ID)
    /* pci����ע�� */
    ul_rslt = oal_pci_init();
    if (OAL_SUCC != ul_rslt)
    {
        /* �ڴ��ж�� */
        oal_mem_exit();
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }

    ul_rslt = oal_5115_pci_init();
    if (OAL_SUCC != ul_rslt)
    {
        OAL_IO_PRINT("oal_main_init: oal_5115_pci_init return error code: %d", ul_rslt);
        OAL_BUG_ON(1);
        return -OAL_EFAIL;
    }
#elif ((_PRE_PRODUCT_ID_HI1131C_HOST == _PRE_PRODUCT_ID)||(_PRE_PRODUCT_ID_HI1131C_DEV ==_PRE_PRODUCT_ID))
    /* Hi1102 SDIO���߳�ʼ���ӿ� TBD */

    /* ��ʼ��: �����ϵ�chip��������1 */
    oal_bus_init_chip_num();
    ul_rslt = oal_bus_inc_chip_num();
    if(OAL_SUCC != ul_rslt)
    {
         OAL_IO_PRINT("oal_pci_probe: oal_bus_inc_chip_num failed!\n");
         OAL_BUG_ON(1);
         return -OAL_EIO;
    }
#endif

    /* �����ɹ� */
#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
    OAL_MEMZERO(g_past_net_device, WLAN_VAP_SUPPORT_MAX_NUM_LIMIT * OAL_SIZEOF(oal_net_device_stru *));
#endif

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
    /* HCC��ʼ�� */
    if (OAL_UNLIKELY(NULL ==hcc_module_init()))
    {
        OAL_IO_PRINT("[ERROR]hcc_module_init return err null\n");
        return -OAL_EFAIL;
    }
#endif

#ifdef _PRE_CONFIG_HISI_CONN_SOFTWDFT
    if(OAL_UNLIKELY(OAL_SUCC != oal_softwdt_init()))
    {
        OAL_IO_PRINT("oal_softwdt_init init failed!\n");
        return -OAL_EFAIL;
    }
#endif

#ifdef _PRE_OAL_FEATURE_KEY_PROCESS_TRACE
    if(OAL_UNLIKELY(OAL_SUCC != oal_dft_init()))
    {
        OAL_IO_PRINT("oal_dft_init init failed!\n");
        return -OAL_EFAIL;
    }
#endif

    return OAL_SUCC;
}

/*****************************************************************************
 �� �� ��  : oal_main_exit
 ��������  : OALģ��ж��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ģ��ж�ط���ֵ���ɹ���ʧ��ԭ��
 ���ú���  : ��
 ��������  : ��

 �޸���ʷ      :
  1.��    ��   : 2012��9��18��
    ��    ��   : ������
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_void  ATTR_OAL_NO_FUNC_TRACE oal_main_exit(oal_void)
{
#ifdef _PRE_OAL_FEATURE_KEY_PROCESS_TRACE
    oal_dft_exit();
#endif

#ifdef _PRE_CONFIG_HISI_CONN_SOFTWDFT
    oal_softwdt_exit();
#endif

    hcc_module_exit(hcc_get_default_handler());

#if (_PRE_PRODUCT_ID_HI1151 == _PRE_PRODUCT_ID)

    /* pci����ж�� */
    oal_pci_exit();

    oal_5115_pci_exit();

#elif ((_PRE_PRODUCT_ID_HI1131C_HOST == _PRE_PRODUCT_ID)||(_PRE_PRODUCT_ID_HI1131C_DEV ==_PRE_PRODUCT_ID))
    /* Hi1102 SDIO����exit�ӿ�(���µ�) TBD */

    /* chip num��ʼ��:0 */
    oal_bus_init_chip_num();
#endif

    /* �ڴ��ж�� */
    oal_mem_exit();
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    oal_conn_sysfs_root_boot_obj_exit();
    oal_conn_sysfs_root_obj_exit();
#endif

    return ;
}

#if 0
/*****************************************************************************
 �� �� ��  : __cyg_profile_func_enter
 ��������  : �����滻����ԭ��
 �������  : �̶�Ϊ������ַ��call site
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
void ATTR_OAL_NO_FUNC_TRACE __cyg_profile_func_enter(void *this_func, void *call_site)
{

}

/*****************************************************************************
 �� �� ��  : __cyg_profile_func_exit
 ��������  : �˳��滻����ԭ��
 �������  : �̶�Ϊ������ַ��call site
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��20��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
void ATTR_OAL_NO_FUNC_TRACE __cyg_profile_func_exit(void *this_func, void *call_site)
{

}
#endif
/*****************************************************************************
 �� �� ��  : oal_chip_get_version
 ��������  : ��ȡchip version,��ƽ̨�ṩ�ܵ�version���,device��host�ٷֱ����
 �������  : oal_void
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint32  oal_chip_get_version(oal_void)
{
    oal_uint32 ul_chip_ver = 0;

#if (_PRE_WLAN_REAL_CHIP == _PRE_WLAN_CHIP_SIM)

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    ul_chip_ver   = WLAN_CHIP_VERSION_HI1151V100H;
#elif (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
    /* 1102 02��ҪSOC�ṩ�Ĵ�����ʵ�� */
    ul_chip_ver   = WLAN_CHIP_VERSION_HI1151V100H;
#endif

#else /* else _PRE_WLAN_REAL_CHIP != _PRE_WLAN_CHIP_SIM*/

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    ul_chip_ver   = WLAN_CHIP_VERSION_HI1151V100H;
#elif (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_DEV) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1131C_HOST)
    /* 1102 02��ҪSOC�ṩ�Ĵ�����ʵ�� */
    ul_chip_ver   = WLAN_CHIP_VERSION_HI1151V100H;
#endif

#endif
    return ul_chip_ver;

}
/*****************************************************************************
 �� �� ��  : oal_chip_get_version
 ��������  : ����chip version��ȡdevice num
 �������  : chip version
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��25��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
oal_uint8 oal_chip_get_device_num(oal_uint32   ul_chip_ver)
{
    oal_uint8   uc_device_nums = 0;

    switch  (ul_chip_ver)
    {
        case WLAN_CHIP_VERSION_HI1151V100H:
        case WLAN_CHIP_VERSION_HI1151V100L:
            uc_device_nums = WLAN_CHIP_DBSC_DEVICE_NUM;
        break;

        default:
            uc_device_nums = 0;
        break;
    }

    return uc_device_nums;
}

/*lint -e578*//*lint -e19*/
#if (_PRE_PRODUCT_ID_HI1151 ==_PRE_PRODUCT_ID)
oal_module_init(oal_main_init);
oal_module_exit(oal_main_exit);

#endif
oal_module_symbol(oal_chip_get_version);
oal_module_symbol(oal_chip_get_device_num);
oal_module_symbol(oal_main_init);
oal_module_symbol(oal_main_exit);
oal_module_symbol(g_l2cache_base);
//oal_module_symbol(__cyg_profile_func_enter);
//oal_module_symbol(__cyg_profile_func_exit);




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

