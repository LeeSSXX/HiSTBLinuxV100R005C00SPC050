/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

******************************************************************************
�� �� ��   : test
�� �� ��   : ����
��    ��   : 
��������   : 2014��10��10��
����޸�   :
��������   : test.c �ļ�
�����б�   :
�޸���ʷ   :
1.��    ��   : 2008��8��8��
 ��    ��   : z00888888
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

#include "board.h"
#include "oal_types.h"
#include "oal_schedule.h"
#include "plat_debug.h"
#include "hisi_ini.h"

int32 g_plat_loglevel = PLAT_LOG_DEBUG;

oal_int32  sdio_detect_init(oal_void)
{
    oal_int32 l_return = 0; 

#ifndef CONFIG_DEV_CLK_40M
    l_return = board_clk_init();
    if (OAL_SUCC != l_return)
    {
        PS_PRINT_ERR("board_clk_init failedS\n");
        goto done;
    }
#endif
  
    l_return = board_cfg_init();
    if (OAL_SUCC != l_return)
    {
        PS_PRINT_ERR("board_cfg_init failedS\n");
        goto done;
    }
    
    l_return = board_power_gpio_init();
    if (OAL_SUCC != l_return)
    {
        PS_PRINT_ERR("board_power_gpio_init failedS\n");
        goto done;
    }    
    board_power_on();    
    himciv200_sdio_card_detect_change();
    msleep(500);
    
done:
    return l_return;
}

oal_void sdio_detect_exit(oal_void)
{
    board_power_off();
    board_power_gpio_exit();
    himciv200_sdio_card_detect_change();
    msleep(500);
}


oal_module_init(sdio_detect_init);
oal_module_exit(sdio_detect_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sdio detect");


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

