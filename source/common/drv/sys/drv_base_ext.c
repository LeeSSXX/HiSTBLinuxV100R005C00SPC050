/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : common_base.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/01/25
  Description   : common driver moudle define
  History       :
  1.Date        : 2010/01/25
    Author      : jianglei
    Modification: Created file

******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>

#include "hi_type.h"
#include "hi_drv_log.h"
#include "hi_drv_struct.h"
#include "hi_drv_sys.h"
#include "drv_log.h"
#include "hi_drv_proc.h"
#include "hi_drv_stat.h"
#include "drv_sys_ioctl.h"
#include "drv_stat_ioctl.h"
#include "hi_drv_module.h"
#include "drv_base_ext_k.h"
#include "hi_drv_mmz.h"
#include "hi_drv_memdev.h"
#include "hi_drv_userproc.h"
#include "hi_drv_dev.h"


static struct platform_device *hi_virt_dev = NULL;

int hi_get_irq_byname(char *name)
{
    struct resource* res = NULL;

    if (!hi_virt_dev)
    {
        HI_ERR_SYS("module %s is not registered.\n",name);
        return -1;
    }

    res = platform_get_resource_byname(hi_virt_dev, IORESOURCE_IRQ, name);
    if (!res)
    {
        HI_ERR_SYS("module %s  platform_get_resource_byname fail!\n", name);
        return -1;
    }

    //HI_PRINT("module %s, res->start:%d.\n",name,res->start);

    return res->start;
}

static int hi_virt_drv_probe(struct platform_device *pdev)
{
    HI_PRINT("virtual device initialization.\n");
    hi_virt_dev = pdev;
    return 0;
}

static const struct of_device_id hi_virt_dev_match[] __maybe_unused = {
    { .compatible = "virt-device", },
    {},
};
MODULE_DEVICE_TABLE(of, hi_virt_dev_match);

struct platform_driver hi_virt_pltm_driver = {
    .probe  = hi_virt_drv_probe,
    .driver = {
        .name = "virt-device",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(hi_virt_dev_match),
    },
};

static int __INIT__ COMMON_DRV_ModInit(void)
{
    HI_S32 ret;

#ifdef MODULE
    ret = platform_driver_register(&hi_virt_pltm_driver);
    if(ret)
    {
        HI_ERR_SYS("register driver failed:%#x!\n", ret);
        return ret;
    }
#endif

#ifndef MODULE
    ret = DRV_PM_ModInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("DRV_PM_ModInit failed:%#x!\n", ret);
        return HI_FAILURE;
    }

    ret = DRV_MMZ_ModInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("DRV_MMZ_ModInit failed:%#x!\n", ret);
        goto ErrorExit_MMZ;
    }
#endif

#if !defined (HI_MCE_SUPPORT) && !defined(HI_KEYLED_CT1642_KERNEL_SUPPORT)
    ret = HI_DRV_CommonInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_CommonInit failed:%#x!\n", ret);
        goto ErrorExit_Common;
    }
#endif

    ret = HI_DRV_PROC_Init();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_PROC_Init failed:%#x!\n", ret);
        goto ErrorExit_PROC;
    }

    ret = HI_DRV_LOG_Init();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_LOG_Init failed:%#x!\n", ret);
        goto ErrorExit_LOG;
    }

    ret = HI_DRV_SYS_Init();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_SYS_Init failed:%#x!\n", ret);
        goto ErrorExit_SYS;
    }

    ret = HI_DRV_STAT_Init();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_STAT_Init failed:%#x!\n", ret);
        goto ErrorExit_STAT;
    }

    ret = MMNGR_DRV_ModInit(HI_KMODULE_MAX_COUNT, HI_KMODULE_MEM_MAX_COUNT);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_SYS("KModuleMgr_Init failed:%#x!\n", ret);
        goto ErrorExit_Module;
    }

    ret = MEMDEV_DRV_ModInit();
    if (HI_SUCCESS != ret)
    {
        HI_ERR_SYS("memdev init failed:%#x!\n", ret);
        goto ErrorExit_MEMDEV;
    }

    ret = USRPROC_DRV_ModInit();
    if (HI_SUCCESS != ret)
    {
        HI_ERR_SYS("userproc init failed:%#x!\n", ret);
        goto ErrorExit_USRPROC;
    }

#if defined(MODULE)
    HI_PRINT("Load hi_common.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

ErrorExit_USRPROC:
    MEMDEV_DRV_ModExit();

ErrorExit_MEMDEV:
    MMNGR_DRV_ModExit();

ErrorExit_Module:
    HI_DRV_STAT_Exit();

ErrorExit_STAT:
    HI_DRV_SYS_Exit();

ErrorExit_SYS:
    HI_DRV_LOG_Exit();

ErrorExit_LOG:
    HI_DRV_PROC_Exit();

ErrorExit_PROC:
#if !defined (HI_MCE_SUPPORT) && !defined(KEYLED_CT1642_KERNEL_SUPPORT)
    HI_DRV_CommonExit();

ErrorExit_Common:
#endif

#ifndef MODULE
    DRV_MMZ_ModExit();

ErrorExit_MMZ:
    DRV_PM_ModExit();
#endif

    return HI_FAILURE;
}

#ifdef MODULE
static HI_VOID COMMON_DRV_ModExit (HI_VOID)
{
    USRPROC_DRV_ModExit();

    MEMDEV_DRV_ModExit();

    MMNGR_DRV_ModExit();

    HI_DRV_STAT_Exit();

    HI_DRV_SYS_Exit();

    HI_DRV_LOG_Exit();

    HI_DRV_PROC_Exit();

#ifndef MODULE
    DRV_MMZ_ModExit();
    DRV_PM_ModExit();
#endif

#if !defined (HI_MCE_SUPPORT) && !defined(HI_KEYLED_CT1642_KERNEL_SUPPORT)
    HI_DRV_CommonExit();
#endif

#ifdef MODULE
    platform_driver_unregister(&hi_virt_pltm_driver);
#endif

#if defined(MODULE)
    HI_PRINT("remove hi_common.ko success.\n");
#endif
    return;
}
#endif

#ifdef MODULE
module_init(COMMON_DRV_ModInit);
module_exit(COMMON_DRV_ModExit);
#else
device_initcall(COMMON_DRV_ModInit);
#endif

MODULE_AUTHOR("HISILION");
MODULE_LICENSE("GPL");
