
/******************************************************************************

  ��Ȩ���� (C), 2001-2011, ��˼�뵼�����޹�˾

******************************************************************************
    �� �� ��   : vdec_firmware.c
    �� �� ��   : ����
    ��    ��   :
    ��������   :
    ����޸�   :
    ��������   : firmware�Ķ���ӿ�ʵ��


 �޸���ʷ   :
    1.��    �� : 2009-01-08
    ��    ��   :
    �޸�����   :

******************************************************************************/
#include "vfmw.h"

#ifdef VFMW_VC1_SUPPORT
#include "vc1.h"
#include "bitplane.h"
#endif

#include "vdm_hal.h"

#ifdef VFMW_VDH_V100_SUPPORT
#include "vdm_hal_v100_api.h"
#endif

#ifdef VFMW_VDH_V200R004_SUPPORT
#include "vdm_hal_v200r004_api.h"
#endif

#include "mem_manage.h"
#include "public.h"

#ifdef VFMW_FOD_SUPPORT
#include "fod_drv.h"
#endif

#include "vfmw_ctrl.h"
#include "syntax.h"
//#include "vfmw_osal.h"

#ifdef VFMW_DNR_SUPPORT
#include "dnr_hal.h"
#include "dnr_offline.h"
#endif
#ifdef VFMW_BTL_SUPPORT
#include "btl_hal_v100.h"
#include "btl_drv.h"
#endif
//#include "vfmw_rawbuf.h"
#include "hi_reg_common.h"

#ifdef ENV_ARMLINUX_KERNEL
#include "hi_kernel_adapt.h"
#include "hi_drv_stat.h"
#include "hi_drv_sys.h"
#endif

#include "vfmw_osal_ext.h"

Vfmw_Osal_Func_Ptr vfmw_Osal_Func_Ptr_S;


/*======================================================================*/
/*   ��                                                                 */
/*======================================================================*/
#define  VDEC_ASSERT_RET( cond, else_print )                    \
    do {                                                            \
        if( !(cond) )                                               \
        {                                                           \
            dprint(PRN_FATAL,"vfmw.c,L%d: %s\n", __LINE__, else_print ); \
            return VDEC_ERR;                                        \
        }                                                           \
    }while(0)

#define  VDEC_ASSERT( cond, else_print )                        \
    do {                                                            \
        if( !(cond) )                                               \
        {                                                           \
            dprint(PRN_FATAL,"vfmw.c,L%d: %s\n", __LINE__, else_print ); \
            return;                                                 \
        }                                                           \
    }while(0)

/*======================================================================*/
/*   ȫ�ֹ�������                                                       */
/*======================================================================*/
//extern OSAL_IRQ_SPIN_LOCK g_TaskLock;
//extern OSAL_IRQ_SPIN_LOCK g_SpinLock;
//extern OSAL_IRQ_SPIN_LOCK g_SpinLock_fsp;
//extern OSAL_IRQ_SPIN_LOCK g_SpinLockIrq_Destroy;
//extern OSAL_IRQ_SPIN_LOCK g_ActivateLock;

#if defined(VFMW_DNR_SUPPORT) || defined(VFMW_BTL_SUPPORT)
extern OSAL_IRQ_SPIN_LOCK g_SpinLockIsr_Image2D;
#endif

//extern SINT32  g_SleepTime;

SINT32 g_VfmwEnableFlag = 0;
SINT32 g_VfmwInitCount = 0;
SINT32 g_ChipMaxWidth = 8192;   // chip support max width   //sdk
SINT32 g_ChipMaxHeight = 8192;  // chip support max height
VDMHAL_FUN_PTR_S g_vdm_hal_fun_ptr = {0};
#ifdef VFMW_DNR_SUPPORT
DNRHAL_FUN_PTR_S g_dnr_hal_fun_ptr = {0};
#endif
#ifdef VFMW_BTL_SUPPORT
BTLHAL_FUN_PTR_S g_btl_hal_fun_ptr = {0};
#endif

#ifdef VFMW_MODULE_LOWDLY_SUPPORT
UINT8 g_LowDelayCalcStat[MAX_CHAN_NUM] = {0};
SINT32 g_LowDelayCalcHandle[MAX_CHAN_NUM] = {0};
UINT32 g_LowDelaySeqIndex[MAX_CHAN_NUM] = {0};
#endif
extern VFMW_CHAN_S  *s_pstVfmwChan[MAX_CHAN_NUM];

/*======================================================================*/
/*   ���ú�������                                                       */
/*======================================================================*/
VOID  GetVdecCapability( VDEC_CAP_S *pCap );
//SINT32 GetVdecChipType(UINT32 *ChipType);


/*======================================================================*/
/*   ����ʵ��                                                           */
/*======================================================================*/
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
/************************************************************************
    ��/�˳�VDEC���: ��insmod/rmmodʱ�������º�����
    ��Ҫְ���Ǵ���/����proc�ļ�ϵͳ
 ************************************************************************/
VOID VDEC_Lowdelay_Event_Time(SINT32 ChanId, HI_LD_Event_ID_E eventId, HI_U32 u32FrameId)
{
    HI_LD_Event_S evt;
    HI_U32 TmpTime = 0;
    HI_DRV_SYS_GetTimeStampMs(&TmpTime);

    evt.evt_id = eventId;
    evt.frame = u32FrameId;
    evt.handle = g_LowDelayCalcHandle[ChanId];
    evt.time = TmpTime;

    if (1 == g_LowDelayCalcStat[ChanId])
    {
        HI_DRV_LD_Notify_Event(&evt);
    }
}
#endif

VOID VDEC_OpenModule(VOID)
{
    //OSAL_SEMA_INTIT();
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SemaInit();

#ifdef ENV_ARMLINUX_KERNEL
    //vfmw_proc_init();
    vfmw_Osal_Func_Ptr_S.pfun_Osal_vfmw_proc_init();
#endif
}

VOID VDEC_ExitModule(VOID)
{
#ifdef ENV_ARMLINUX_KERNEL
    //vfmw_proc_exit();
    vfmw_Osal_Func_Ptr_S.pfun_Osal_vfmw_proc_exit();
#endif
}

VOID VDM_ResetClock(VOID)
{
    U_PERI_CRG30 u32TmpValue;
    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;
    u32TmpValue.bits.vdhclk_skipcfg = 0;

    /*ʹ�ܿ�����Ĳ�����Ҫ�������ʱ���λҪ����0����1*/
    u32TmpValue.bits.vdhclk_loaden = 0x0;
    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;
    u32TmpValue.bits.vdhclk_loaden = 0x1;
    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;

    return;
}

VOID VDM_OpenHardware(SINT32 VdhId)
{
    U_PERI_CRG30 u32TmpValue;

    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;

    u32TmpValue.bits.vdh_cken         = 0x1;
    u32TmpValue.bits.vdh_all_srst_req = 0x0;
    u32TmpValue.bits.vdh_mfd_srst_req = 0x0;
    u32TmpValue.bits.vdh_clk_sel      = 0x0;
    u32TmpValue.bits.vdhclk_skipcfg   = 0x0;
    u32TmpValue.bits.vdhclk_loaden    = 0x0;

    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;

    //udelay(30);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_udelay(30);

    return;
}

VOID DSP_OpenHardware(SINT32 VdhId)
{
    U_PERI_CRG30 u32TmpValue;

    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;

    u32TmpValue.bits.vdhdsp_cken = 0x1;

    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;

    //udelay(30);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_udelay(30);

    return;
}


VOID VDM_CloseHardware(SINT32 VdhId)
{
    U_PERI_CRG30 u32TmpValue;
    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;

    /* ȫ�ָ�λ */
    VDMHAL_GlbReset();

    u32TmpValue.bits.vdh_all_srst_req = 0x1;
    u32TmpValue.bits.vdh_mfd_srst_req = 0x1;
    u32TmpValue.bits.vdh_cken         = 0x0;
    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;

    //udelay(30);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_udelay(30);

    return;
}

VOID DSP_CloseHardware(SINT32 VdhId)
{
    U_PERI_CRG30 u32TmpValue;

    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;

    u32TmpValue.bits.vdhdsp_cken = 0x0;

    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;

    //udelay(30);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_udelay(30);

    return;
}


VOID SCD_OpenHardware(SINT32 ScdId)
{
    U_PERI_CRG30 u32TmpValue;
    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;
    u32TmpValue.bits.vdh_scd_srst_req = 0x0;
    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;

    return;
}

VOID SCD_CloseHardware(SINT32 ScdId)
{
    U_PERI_CRG30 u32TmpValue;
    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG30.u32;
    u32TmpValue.bits.vdh_scd_srst_req = 0x1;
    g_pstRegCrg->PERI_CRG30.u32 = u32TmpValue.u32;
    return;
}

#ifdef VFMW_DNR_SUPPORT
VOID DNR_OpenHardware(VOID)
{
    /*X6V300������DNRģ��*/
    return;
}
#endif

#ifdef VFMW_BTL_SUPPORT
VOID BTL_OpenHardware(VOID)
{
    UINT32  *p32 = (UINT32 *)MEM_Phy2Vir(BTL_RESET_REG_PHY_ADDR);

    if ( NULL != p32)
    {
#if 0
        /* ��ʱ��ʼ�� */
        p32[0] |= 0x100;   // PERI_CRG8[8:8] = btl_clken
        /* ������λ�ź� */
        p32[0] &= (~1);    // PERI_CRG8[0:0] = btl_srst_req
#endif
        p32[0] |= 0x1;
        /* ������λ�ź� */
        p32[0] &= (~0x10);
    }

    return;
}

VOID BTL_CloseHardware(VOID)
{
    UINT32  *p32 = (UINT32 *)MEM_Phy2Vir(BTL_RESET_REG_PHY_ADDR);

    if ( NULL != p32)
    {
        /* ������λ�ź� */
        p32[0] |= 0x10;
        /*�ر�ʼ��*/
        p32[0] &= (~0x1);

    }

    return;
}
#endif

SINT32 BPD_OpenHardware(VOID)
{
    U_PERI_CRG34 u32TmpValue;
    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG34.u32;
    u32TmpValue.bits.bpd_cken = 0x1;
    u32TmpValue.bits.bpd_srst_req = 0x0;
    g_pstRegCrg->PERI_CRG34.u32 = u32TmpValue.u32;

    return VCTRL_OK;
}

SINT32 BPD_CloseHardware(VOID)
{
    U_PERI_CRG34 u32TmpValue;
    u32TmpValue.u32 = g_pstRegCrg->PERI_CRG34.u32;
    u32TmpValue.bits.bpd_cken = 0x0;
    u32TmpValue.bits.bpd_srst_req = 0x1;
    g_pstRegCrg->PERI_CRG34.u32 = u32TmpValue.u32;

    return VCTRL_OK;
}


/************************************************************************
    VDEC ��VFMW���е������г�ʼ��
 ************************************************************************/
VOID VDEC_SpinLockInit(VOID)
{

    //OSAL_SpinLockIRQInit(&g_TaskLock);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_TASKLOCK);

    //OSAL_SpinLockIRQInit(&g_RecordLock);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_RECORDLOCK);

    //OSAL_SpinLockIRQInit(&g_SpinLock);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_SPINLOCK);

    //OSAL_SpinLockIRQInit(&g_SpinLock_fsp);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_SPINLOCK_FSP);


    //OSAL_SpinLockIRQInit(&g_SpinLockIrq_Destroy);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_SPINLOCKIRQ_DESTROY);

    //OSAL_SpinLockIRQInit(&g_ActivateLock);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_ACTIVATELOCK);

#if defined(VFMW_DNR_SUPPORT) || defined(VFMW_BTL_SUPPORT)
    //OSAL_SpinLockIRQInit(&g_SpinLockIsr_Image2D);
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLockInit(G_SPINLOCKISR_IMAGE2D);
#endif

    return;
}


/************************************************************************
    VDECȫ�ֳ�ʼ�����ϲ���ô˺���ʹVDEC�ص���ʼ״̬��ͬʱע��ص�����.
    VdecCallback: ���������Ӵ˴��ݻص�������VDEC
 ************************************************************************/
SINT32 VDEC_Init( SINT32 (*VdecCallback)( SINT32 ChanID, SINT32 eEventID, VOID *pArgs ) )
{
    SINT32 ret;
    VDEC_OPERATION_S Args;

    //memset(&Args, 0, sizeof(VDEC_OPERATION_S));
    vfmw_Osal_Func_Ptr_S.pfun_Osal_MemSet(&Args, 0, sizeof(VDEC_OPERATION_S));

    /* ��׼���� */
    Args.VdecCallback = VdecCallback;
    Args.mem_malloc = NULL;
    Args.mem_free = NULL;

    ret = VDEC_InitWithOperation(&Args);

    return  (ret == VCTRL_OK) ? VDEC_OK : VDEC_ERR;
}

SINT32 VDEC_InitWithOperation(VDEC_OPERATION_S *pArgs)
{
    SINT32 ret;

    if (NULL == pArgs)
    {
        dprint(PRN_ERROR, "VDEC_InitWithOperation has NULL parameter, error!\n");
        return VDEC_ERR;
    }

    vfmw_Osal_Func_Ptr_S.pfun_Osal_SemaDown();

    /* ����: mem_malloc & mem_free���������ÿ�����Ч*/
    /* ��ΪOMX ��VDEC ����ͬʱ���棬����һ����ʼ������һ��ֱ�ӷ���OK������ᱨ��ʼ��ʧ�� sdk */
    if (1 == g_VfmwEnableFlag)
    {
        g_VfmwInitCount++;
        ret = VCTRL_OK;
        goto Exit;
    }

    AcceleratorCharacter = NULL;

#ifdef VFMW_BTL_SUPPORT
    g_btl_hal_fun_ptr.pfun_BTLDRV_OpenHardware           = BTL_OpenHardware;
    g_btl_hal_fun_ptr.pfun_BTLDRV_CloseHardware          = BTL_CloseHardware;
    g_btl_hal_fun_ptr.pfun_BTLHAL_Start                  = BTLHAL_V100_Start;
    g_btl_hal_fun_ptr.pfun_BTLHAL_GetInfo	            = BTLHAL_V100_GetInfo;
    g_btl_hal_fun_ptr.pfun_BTLHAL_GetInfo_Capture	    = BTLHAL_V100_GetInfo_Capture;
    g_btl_hal_fun_ptr.pfun_BTLHAL_Reset                  = BTLHAL_V100_Reset;
    g_btl_hal_fun_ptr.pfun_BTLHAL_ClearIntState          = BTLHAL_V100_ClearIntState;
    g_btl_hal_fun_ptr.pfun_BTLHAL_MaskInt                = BTLHAL_V100_MaskInt;
    g_btl_hal_fun_ptr.pfun_BTLHAL_EnableInt              = BTLHAL_V100_EnableInt;
    g_btl_hal_fun_ptr.pfun_BTLHAL_CfgReg                 = BTLHAL_V100_CfgReg;
#endif
    g_vdm_hal_fun_ptr.pfun_VDMDRV_OpenHardware           = VDM_OpenHardware;
    g_vdm_hal_fun_ptr.pfun_VDMDRV_CloseHardware          = VDM_CloseHardware;

    g_vdm_hal_fun_ptr.pfun_DSPDRV_OpenHardware           = DSP_OpenHardware;
    g_vdm_hal_fun_ptr.pfun_DSPDRV_CloseHardware          = DSP_CloseHardware;

    g_vdm_hal_fun_ptr.pfun_SCDDRV_OpenHardware           = SCD_OpenHardware;
    g_vdm_hal_fun_ptr.pfun_SCDDRV_CloseHardware          = SCD_CloseHardware;
    g_vdm_hal_fun_ptr.pfun_BPDDRV_OpenHardware           = BPD_OpenHardware;
    g_vdm_hal_fun_ptr.pfun_BPDDRV_CloseHardware          = BPD_CloseHardware;

    g_vdm_hal_fun_ptr.pfun_VDMHAL_GetHalMemSize          = VDMHAL_V200R004_GetHalMemSize;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_OpenHAL                = VDMHAL_V200R004_OpenHAL;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_CloseHAL               = VDMHAL_V200R004_CloseHAL;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_ArrangeMem             = VDMHAL_V200R004_ArrangeMem;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_ArrangeMem_BTL         = VDMHAL_V200R004_ArrangeMem_BTL;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_ResetVdm               = VDMHAL_V200R004_ResetVdm;
#ifdef VFMW_BVT_SUPPORT
    g_vdm_hal_fun_ptr.pfun_VDMHAL_GlbReset               = VDMHAL_V200R004_ResetGlb;
#else
    g_vdm_hal_fun_ptr.pfun_VDMHAL_GlbReset               = VDMHAL_V200R004_GlbReset;
#endif
    g_vdm_hal_fun_ptr.pfun_VDMHAL_ClearIntState          = VDMHAL_V200R004_ClearIntState;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_MaskInt                = VDMHAL_V200R004_MaskInt;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_EnableInt              = VDMHAL_V200R004_EnableInt;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_CheckReg               = VDMHAL_V200R004_CheckReg;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_StartHwRepair          = VDMHAL_V200R004_StartHwRepair;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_StartHwDecode          = VDMHAL_V200R004_StartHwDecode;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_PrepareDec             = VDMHAL_V200R004_PrepareDec;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_IsVdmReady             = VDMHAL_V200R004_IsVdmReady;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_IsVdmRun               = VDMHAL_V200R004_IsVdmRun;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_PrepareRepair          = VDMHAL_V200R004_PrepareRepair;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_MakeDecReport          = VDMHAL_V200R004_MakeDecReport;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_BackupInfo             = VDMHAL_V200R004_BackupInfo;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_GetCharacter           = VDMHAL_V200R004_GetCharacter;
    g_vdm_hal_fun_ptr.pfun_VDMHAL_Write1DYuv             = VDMHAL_V200R004_WriteBigTitle1DYuv;
#ifdef VFMW_DNR_SUPPORT
    g_dnr_hal_fun_ptr.pfun_DNRDRV_OpenHardware           = DNR_OpenHardware;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_Init					 = DNRHAL_V200_Init;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_CfgReg				 = DNRHAL_V200_CfgReg;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_Reset					 = DNRHAL_V200_Reset;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_Start					 = DNRHAL_V200_Start;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_ClearIntState			 = DNRHAL_V200_ClearIntState;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_MaskInt                = DNRHAL_V200_MaskInt;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_EnableInt              = DNRHAL_V200_EnableInt;
    g_dnr_hal_fun_ptr.pfun_DNRHAL_PrintDnrPhyRegDat		 = DNRHAL_V200_PrintDnrPhyRegDat;
#endif

    // �������г�ʼ��
    VDEC_SpinLockInit();

    VDMHAL_GetCharacter();
    MEM_ManagerWithOperation(pArgs);
    ret = VCTRL_OpenVfmw();
    dprint(PRN_CTRL, "VCTRL_OpenVfmw() return %d\n", ret);

    if (ret == VCTRL_OK)
    {
        g_VfmwEnableFlag = 1;
        g_VfmwInitCount = 1;
    }
    else
    {
        g_VfmwEnableFlag = 0;
        g_VfmwInitCount = 0;
    }

    /* ȫ�ָ�λ */
    VDMHAL_GlbReset();

Exit:
    VCTRL_SetCallBack(pArgs->eAdapterType, pArgs->VdecCallback);

    //OSAL_UP();
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SemaUp();

    return  (ret == VCTRL_OK) ? VDEC_OK : VDEC_ERR;
}
/************************************************************************
    VDECȫ��ȥ��ʼ�����ϲ���ô˺���ʹVDEC�ر�����Ӳ�����٣��ͷ�������Դ
 ************************************************************************/
SINT32 VDEC_Exit(VOID)
{
    SINT32 ret;

    //ret = OSAL_DOWN_INTERRUPTIBLE();
    ret = vfmw_Osal_Func_Ptr_S.pfun_Osal_SemaDown();

    if (0 != ret)
    {
        dprint(PRN_FATAL, "OSAL_DOWN_INTERRUPTIBLE ERR\n");
    }

    /* ��ΪOMX ��VDEC ����ͬʱ���棬������һ���������ȫ��ȥ��ʼ�� sdk */
    g_VfmwInitCount--;

    if (g_VfmwInitCount > 0)
    {
        //OSAL_UP();
        vfmw_Osal_Func_Ptr_S.pfun_Osal_SemaUp();

        return VCTRL_OK;
    }

    ret = VCTRL_CloseVfmw();
    MEM_ManagerClearOperation();

    g_VfmwEnableFlag = 0;

    //OSAL_UP();
    vfmw_Osal_Func_Ptr_S.pfun_Osal_SemaUp();

    return  (ret == VCTRL_OK) ? VDEC_OK : VDEC_ERR;
}

/************************************************************************
    VDEC��������
    ChanID:  ��Ҫ������ͨ����(����ͨ���޹ز������˲�����Ϊ����ֵ)
    eCmdID:  ������룬ָ����ҪVDECִ�к��ֶ���
    pArgs:   ������������ʽ��eCmdID���
 ************************************************************************/
SINT32 VDEC_Control( SINT32 ChanID, VDEC_CID_E eCmdID, VOID *pArgs )
{
    SINT32 ret = VDEC_OK;
    IMAGE_INTF_S *pImgIntf;
    SINT32 VdmMem[3], ScdMem[3];
    SINT32 *p32 = NULL;
    UINT64 *p64 = NULL;
    DETAIL_MEM_SIZE DetailMemSize;
    SINT32 TaskLockFlag = 0;
    SINT32 s32Loop = 500;

    //    UINT32 *pu32 = NULL;
    //OSAL_SpinLock(&g_TaskLock);

    if (g_VfmwEnableFlag != 1)
    {
        dprint(PRN_FATAL, "VFMW has been destroyed, so any operation is unuseful!\n");
        return VDEC_ERR;
    }

    switch (eCmdID)
    {
        case VDEC_CID_GET_GLOBAL_STATE: /* ��ȡȫ��״̬��Ϣ */
            break;

        case VDEC_CID_EXTRA_WAKEUP_THREAD:
            break;

        case VDEC_CID_GET_CAPABILITY:   /* ��ȡ���������� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_GLOBAL_CFG:   /* ��ȡȫ��������Ϣ */
            break;

        case VDEC_CID_CFG_DECODER:      /* ����ȫ�ֽ����� */
            break;

        case VDEC_CID_CFG_EXTRA:      /* ���ý����� extra_ref & extra_disp */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c, L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_CREATE_CHAN:      /* ����ͨ�� */

            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }
            else
            {
                p32 = (SINT32 *)pArgs;

                if (p32[0] > CAP_LEVEL_BUTT)
                {
                    dprint(PRN_FATAL, "vfmw.c,L%d: capability level out of range\n", __LINE__);
                    TaskLockFlag = 1;
                }
            }

            break;

        case VDEC_CID_CREATE_CHAN_WITH_OPTION:  /* �����ض���С�Ľ���ͨ�� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }
            else
            {
                p32 = (SINT32 *)pArgs;

                if (p32[0] > CAP_LEVEL_BUTT)
                {
                    dprint(PRN_FATAL, "vfmw.c,L%d: capability level out of range\n", __LINE__);
                    TaskLockFlag = 1;
                }
            }

            break;

#if 0

        case VDEC_CID_CREATE_CHAN_WITH_MEM:  /* �����ض���С�Ľ���ͨ�� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }
            else
            {
                p32 = (SINT32 *)pArgs;

                if (p32[0] > CAP_LEVEL_BUTT)
                {
                    dprint(PRN_FATAL, "vfmw.c,L%d: capability level out of range\n", __LINE__);
                    TaskLockFlag = 1;
                }
            }

            break;
#endif

        case VDEC_CID_DESTROY_CHAN:     /* ����ͨ�� */
            break;

        case VDEC_CID_DESTROY_CHAN_WITH_OPTION:     /* ����ͨ������Դ�ⲿ�ͷ� */
            break;

#if 0

        case VDEC_CID_DESTROY_CHAN_WITH_MEM:     /* ����ͨ������Դ�ⲿ�ͷ� */
            break;
#endif

        case VDEC_CID_GET_CHAN_CFG:     /* ��ȡͨ������ */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_CFG_CHAN:         /* ����ͨ�� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_CHAN_STATE:   /* ��ȡͨ��״̬��Ϣ */
            break;

        case VDEC_CID_START_CHAN:       /* ����ͨ�� */
            break;


        case VDEC_CID_STOP_CHAN:        /* ֹͣͨ�� */
            break;

        case VDEC_CID_RESET_CHAN:       /* ��λͨ�� */
            break;

        case VDEC_CID_RELEASE_STREAM:        /* �ͷ�raw buffer */
            break;

        case VDEC_CID_SET_STREAM_INTF:  /* ����ͨ���������ӿ� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_IMAGE_INTF:   /* ��ȡͨ����ͼ��ӿ� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_STREAM_SIZE:     /* ��ȡͨ����VDH����������(byte) */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_CHAN_MEMSIZE:    /* ��ȡͨ�����ض�������������Ҫռ�õ��ڴ����� */
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_CHAN_DETAIL_MEMSIZE_WITH_OPTION:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_GET_CHAN_ID_BY_MEM:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

            //add by sdk, 2012.11.20
        case VDEC_CID_SET_DISCARDPICS_PARAM:   /*��ȡ��֡����ز���*/
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_SET_DBG_OPTION:   /* ���õ���ѡ�� */
            break;

        case VDEC_CID_GET_DGB_OPTION:   /* ��ȡ����ѡ�� */
            break;

            /* ���������������STD_USERͨ���²ſ���ʹ�� */
        case VDEC_CID_GET_USRDEC_FRAME:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_PUT_USRDEC_FRAME:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_SET_PTS_TO_SEEK:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_SET_TRICK_MODE:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_SET_CTRL_INFO:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_RESET_CHAN_WITH_OPTION:       /* ��λͨ����ѡ��ر���ĳЩͨ������ */
            break;

        case VDEC_CID_FRAME_BTL:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_SET_FRAME_RATE:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_START_LOWDLAY_CALC:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_STOP_LOWDLAY_CALC:
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;

        case VDEC_CID_ALLOC_MEM_TO_CHANNEL: //sdk
        {
            if ( NULL == pArgs )
            {
                dprint(PRN_FATAL, "vfmw.c,L%d: pArgs is NULL!\n", __LINE__);
                TaskLockFlag = 1;
            }

            break;
        }

#if 0

        case VDEC_CID_OPEN_RAWBUF:
        case VDEC_CID_OPEN_MOUNT_RAWBUF:
        case VDEC_CID_CLOSE_RAWBUF:
        case VDEC_CID_MOUNT_RAWBUF:
        case VDEC_CID_UNMOUNT_RAWBUF:
        case VDEC_CID_RESET_RAWBUF:
        case VDEC_CID_GET_STREAM_BUF:
        case VDEC_CID_PUT_STREAM_BUF:

        case VDEC_CID_GET_CHIP_TYPE:
        case VDEC_CID_SLEEP_TIME:

            break;
#endif

        default:
            TaskLockFlag = 1;
    }

    if (1 == TaskLockFlag)
    {
        return VDEC_ERR;
    }

    switch (eCmdID)
    {

        case VDEC_CID_EXTRA_WAKEUP_THREAD:
        {
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            VCTRL_ExtraWakeUpThread();
            ret = VDEC_OK;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);
            break;
        }

        case VDEC_CID_GET_GLOBAL_STATE: /* ��ȡȫ��״̬��Ϣ */
            break;

        case VDEC_CID_GET_CAPABILITY:   /* ��ȡ���������� */

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            GetVdecCapability((VDEC_CAP_S *)pArgs);
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_GET_GLOBAL_CFG:   /* ��ȡȫ��������Ϣ */
            break;

        case VDEC_CID_CFG_DECODER:      /* ����ȫ�ֽ����� */
            break;

        case VDEC_CID_CFG_EXTRA:      /* ���ý����� extra_ref & extra_disp */

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            p32 = (SINT32 *)pArgs;
            VCTRL_SetVdecExtra(p32[0], p32[1]);
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_CREATE_CHAN:      /* ����ͨ�� */
            p32 = (SINT32 *)pArgs;
            ret = VCTRL_CreateChan(p32[0], (MEM_DESC_S *)(&p32[1]));
            dprint(PRN_CTRL, "VCTRL_CreateChan() return %d\n", ret);
            p32[0] = ret;  /* ע�⣬��������pArgs�ȴ����������(������),�ִ����������(ͨ����) */
            ret = (ret >= 0) ? VDEC_OK : VDEC_ERR;
            break;

        case VDEC_CID_CREATE_CHAN_WITH_OPTION:  /* �����ض���С�Ľ���ͨ�� */
            p32 = (SINT32 *)pArgs;
            ret = VCTRL_CreateChanWithOption(p32[0], (VDEC_CHAN_OPTION_S *)(p32[1]), VCTRL_IGNOR_EXTRA, 0);
            p32[0] = ret;  /* ע�⣬��������pArgs�ȴ����������(������),�ִ����������(ͨ����) */
            ret = (ret >= 0) ? VDEC_OK : VDEC_ERR;
            break;

        case VDEC_CID_DESTROY_CHAN:     /* ����ͨ�� */
            while ((VDEC_ERR == (VCTRT_SetChanCtrlState(ChanID, CHAN_DESTORY))) && (s32Loop != 0))
            {
                //msleep(1);
                vfmw_Osal_Func_Ptr_S.pfun_Osal_msleep(1);
                s32Loop--;
            }

            if (s32Loop <= 0)
            {
                ret = VDEC_ERR;
                break;
            }

            ret = VCTRL_DestroyChan(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            break;

        case VDEC_CID_DESTROY_CHAN_WITH_OPTION:     /* ����ͨ������Դ�ⲿ�ͷ� */
            while ((VDEC_ERR == (VCTRT_SetChanCtrlState(ChanID, CHAN_DESTORY))) && (s32Loop != 0))
            {
                //msleep(1);
                vfmw_Osal_Func_Ptr_S.pfun_Osal_msleep(1);
                s32Loop--;
            }

            if (s32Loop <= 0)
            {
                ret = VDEC_ERR;
                dprint(PRN_FATAL, "VCTRT_SetChanCtrlState ERR: %d\n", ret);
                break;
            }

            ret = VCTRL_DestroyChanWithOption(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            break;

        case VDEC_CID_GET_CHAN_CFG:     /* ��ȡͨ������ */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_GetChanCfg(ChanID, (VDEC_CHAN_CFG_S *)pArgs);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_CFG_CHAN:         /* ����ͨ�� */

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_ConfigChan(ChanID, (VDEC_CHAN_CFG_S *)pArgs);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;

#if defined(VFMW_SCD_LOWDLY_SUPPORT) || defined(VFMW_AVSPLUS_SUPPORT)

            if (ret == VDEC_OK)
            {
                ret = VCTRL_LoadDspCode(ChanID);
                ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            }

#endif
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK); // LoadDspCode()���������߲���
            break;

        case VDEC_CID_GET_CHAN_STATE:   /* ��ȡͨ��״̬��Ϣ */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            VCTRL_GetChanState(ChanID, (VDEC_CHAN_STATE_S *)pArgs);
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;
#if 0

        case VDEC_CID_GET_CHAN_FRMSTATE:/*��ȡ֡��״̬*/
            OSAL_SpinLockIRQ(&g_TaskLock);
            VCTRL_GetChanFrmState(ChanID, (VDEC_CHAN_FRMSTATUSINFO_S *)pArgs);
            VCTRL_EnableAllInt();
            OSAL_SpinUnLockIRQ(&g_TaskLock);
            break;
#endif

        case VDEC_CID_START_CHAN:       /* ����ͨ�� */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_StartChan(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_STOP_CHAN:        /* ֹͣͨ�� */
            ret = VCTRL_StopChanWithCheck(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            break;

        case VDEC_CID_RESET_CHAN:       /* ��λͨ�� */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_ResetChan(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_RELEASE_STREAM:       /* �ͷ�raw buffer */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_ReleaseStream(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_SET_STREAM_INTF:  /* ����ͨ���������ӿ� */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_SetStreamInterface(ChanID, pArgs);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;

            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_GET_IMAGE_INTF:   /* ��ȡͨ����ͼ��ӿ� */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            pImgIntf = (IMAGE_INTF_S *)pArgs;
            pImgIntf->image_provider_inst_id = ChanID;

#if defined(OFF_LINE_DNR_ENABLE)
            pImgIntf->read_image = DNR_ReceiveImage;
            pImgIntf->release_image = DNR_ReleaseImage;
#elif defined(BTL_ENABLE)
            pImgIntf->read_image = BTL_ReceiveImage;
            pImgIntf->release_image = BTL_ReleaseImage;
#else
            pImgIntf->read_image = VCTRL_GetChanImage;
            pImgIntf->release_image = VCTRL_ReleaseChanImage;
#endif
            ret = VDEC_OK;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_GET_STREAM_SIZE:     /* ��ȡͨ����VDH����������(byte) */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_GetStreamSize(ChanID, (SINT32 *)pArgs);
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_GET_CHAN_MEMSIZE:    /* ��ȡͨ�����ض�������������Ҫռ�õ��ڴ����� */
            p32 = (SINT32 *)pArgs;

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            if ( VCTRL_OK == VCTRL_GetChanMemSize( p32[0], VdmMem, ScdMem) )
            {
                CHAN_MEM_BUDGET_S *pMem = (CHAN_MEM_BUDGET_S *)pArgs;
                pMem->MinMemSize    = VdmMem[0] + ScdMem[0];
                pMem->NormalMemSize = VdmMem[1] + ScdMem[1];
                pMem->FluentMemSize = VdmMem[2] + ScdMem[2];
                ret = VDEC_OK;
            }
            else
            {
                ret = VDEC_ERR;
            }

            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_GET_CHAN_DETAIL_MEMSIZE_WITH_OPTION:
            p32 = (SINT32 *)pArgs;

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            if ( VCTRL_OK == VCTRL_GetChanMemSizeWithOption( p32[0], (VDEC_CHAN_OPTION_S *)p32[1], &DetailMemSize, VCTRL_ADD_EXTRA) )
            {
                p32[0] = DetailMemSize.VdhDetailMem;
                p32[1] = DetailMemSize.ScdDetailMem;
                p32[2] = DetailMemSize.ChanCtxDetailMem;
                ret = VDEC_OK;
            }
            else
            {
                ret = VDEC_ERR;
            }

            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;
#if 0

        case VDEC_CID_GET_CHAN_DETAIL_MEMSIZE:
            p32 = (SINT32 *)pArgs;

            if ( VCTRL_OK == VCTRL_GetChanDetailMemSize( p32[0], p32[1], p32[2], &DetailMemSize) )
            {
                p32[0] = DetailMemSize.VdhDetailMem;
                p32[1] = DetailMemSize.ScdDetailMem;
                p32[2] = DetailMemSize.ChanCtxDetailMem;
                ret = VDEC_OK;
            }
            else
            {
                ret = VDEC_ERR;
            }

            break;
#endif

        case VDEC_CID_GET_CHAN_ID_BY_MEM:

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            p32 = (SINT32 *)pArgs;
            p32[0] = VCTRL_GetChanIDByMemAddr(p32[0]);
            ret = (-1 != p32[0]) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;
#if 0

        case VDEC_CID_OPEN_RAWBUF:           /* ��ָ��ͨ���ϴ�raw buf */
            OSAL_SpinLock(&g_TaskLock);
            p32 = (SINT32 *)pArgs;
            ret = VCTRL_OpenRawBuf(ChanID, p32[0]);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_OPEN_MOUNT_RAWBUF:    /* ��ָ��ͨ���ϴ�raw buf, ������raw buf��װ��ͨ��(ʹ��"��"ģʽ������) */
            OSAL_SpinLock(&g_TaskLock);
            p32 = (SINT32 *)pArgs;
            ret = VCTRL_OpenRawBuf(ChanID, p32[0]);

            if (VCTRL_OK != ret)
            {
                ret = VDEC_ERR;
            }
            else
            {
                ret = VCTRL_MountRawBuf(ChanID);
                ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            }

            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_CLOSE_RAWBUF:          /* ��ָ��ͨ���Ϲر�raw buf */
            OSAL_SpinLock(&g_TaskLock);
            ret = VCTRL_CloseRawBuf(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_MOUNT_RAWBUF:          /* Ϊָ��ͨ����װraw buf */
            OSAL_SpinLock(&g_TaskLock);
            ret = VCTRL_MountRawBuf(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_UNMOUNT_RAWBUF:        /* Ϊָ��ͨ��ж��raw buf */
            OSAL_SpinLock(&g_TaskLock);
            ret = VCTRL_UnMountRawBuf(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_RESET_RAWBUF:          /* Ϊָ��ͨ����λraw buf */
            OSAL_SpinLock(&g_TaskLock);
            ret = VCTRL_ResetRawBuf(ChanID);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_GET_STREAM_BUF:        /* ��ȡ��������Ŀռ� */
            OSAL_SpinLock(&g_TaskLock);
            {
                SINT32 D32;

                p32 = (SINT32 *)pArgs;
                ret = RAW_GetFree(ChanID, p32[0], &D32);

                if (RAW_OK == ret)
                {
                    p32[0] = D32;
                    ret = VDEC_OK;
                }
                else
                {
                    ret = VDEC_ERR;
                }
            }
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;

        case VDEC_CID_PUT_STREAM_BUF:        /* �������� */
            OSAL_SpinLock(&g_TaskLock);
            {
                MEM_DESC_S *pMem;
                UINT64 *p32_1;
                p32_1 = (UINT64 *)(pArgs + sizeof(MEM_DESC_S)); //PTS
                pMem = (MEM_DESC_S *)pArgs;
                ret = RAW_PutBusy(ChanID, pMem->PhyAddr, pMem->Length, p32_1[0]);
                ret = (RAW_OK == ret) ? VDEC_OK : VDEC_ERR;
            }
            VCTRL_EnableAllInt();
            OSAL_SpinUnLock(&g_TaskLock);
            break;
#endif

            //add by sdk, 2012.11.20
        case VDEC_CID_SET_DISCARDPICS_PARAM:     /*��ȡ��֡����ز���*/
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_SetDiscardPicParam(ChanID, (VDEC_DISPIC_PARAM_S *)pArgs);
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_SET_DBG_OPTION:   /* ���õ���ѡ�� */
            break;

        case VDEC_CID_GET_DGB_OPTION:   /* ��ȡ����ѡ�� */
            break;

            /* ���������������STD_USERͨ���²ſ���ʹ�� */
#ifdef VFMW_USER_SUPPORT

        case VDEC_CID_GET_USRDEC_FRAME:

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);
            {
                MEM_DESC_S  stMem;  /* phy_addr, length */
                p32 = (SINT32 *)pArgs;
                ret = VCTRL_GetUsrdecFrame(ChanID, &stMem);
                p32[0] = stMem.PhyAddr;
                p32[1] = stMem.Length;
                ret = (ret == VCTRL_OK) ? VDEC_OK : VDEC_ERR;
            }
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_PUT_USRDEC_FRAME:
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_PutUsrdecFrame(ChanID, (USRDEC_FRAME_DESC_S *)pArgs);
            ret = (ret == VCTRL_OK) ? VDEC_OK : VDEC_ERR;

            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;
#endif

        case VDEC_CID_SET_PTS_TO_SEEK:
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);
            {
                VDEC_CHAN_RESET_OPTION_S option;
                option.s32KeepBS = 1;
                option.s32KeepSPSPPS = 0;
                option.s32KeepFSP    = 0;
                ret = VCTRL_ResetChanWithOption(ChanID, &option);
                ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;

                if (ret == VDEC_OK)
                {
                    p64 = (UINT64 *)pArgs;
                    ret = VCTRL_SeekPts(ChanID, p64);
                    ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
                }
            }
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_SET_TRICK_MODE:
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

#if defined(OFF_LINE_DNR_ENABLE)
            ret = DNR_ConfigFFFBSpeed(ChanID, (SINT32 *)pArgs);
            ret = (DNR_OK == ret) ? VDEC_OK : VDEC_ERR;

            if (VDEC_OK == ret)
            {
                ret = VCTRL_ConfigFFFBSpeed(ChanID, (SINT32 *)pArgs);
                ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            }

#endif

#if defined(BTL_ENABLE)
            ret = BTL_ConfigFFFBSpeed(ChanID, (SINT32 *)pArgs);
            ret = (BTL_OK == ret) ? VDEC_OK : VDEC_ERR;

            if (VDEC_OK == ret)
            {
                ret = VCTRL_ConfigFFFBSpeed(ChanID, (SINT32 *)pArgs);
                ret = (BTL_OK == ret) ? VDEC_OK : VDEC_ERR;
            }

#else
            ret = VCTRL_ConfigFFFBSpeed(ChanID, (SINT32 *)pArgs);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
#endif

            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_SET_CTRL_INFO:

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_ConfigPVRInfo(ChanID, (VFMW_CONTROLINFO_S *)pArgs);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;

        case VDEC_CID_RESET_CHAN_WITH_OPTION:       /* ��λͨ����ѡ��ر���ĳЩͨ������ */
            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = VCTRL_ResetChanWithOption(ChanID, (VDEC_CHAN_RESET_OPTION_S *)pArgs);
            ret = (VCTRL_OK == ret) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;
#if 0

        case VDEC_CID_GET_CHIP_TYPE:
            pu32 = (UINT32 *)pArgs;
            ret = GetVdecChipType(&(pu32[0]));   /* ��ȡоƬ���� */
            break;

        case VDEC_CID_SLEEP_TIME:
            p32 = (SINT32 *)pArgs;
            g_SleepTime = p32[0];
            ret = VDEC_OK;
            break;
#endif
            //add by sdk
#ifdef VFMW_BTL_SUPPORT

        case VDEC_CID_FRAME_BTL:

            //OSAL_SpinLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

            ret = BTL_SetDataSource(ChanID, 1, (IMAGE *)pArgs);
            ret = (ret == BTL_OK) ? VDEC_OK : VDEC_ERR;
            VCTRL_EnableAllInt();

            //OSAL_SpinUnLockIRQ(&g_TaskLock);
            vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            break;
#endif

        case VDEC_CID_SET_FRAME_RATE:
            s_pstVfmwChan[ChanID]->s32FrameRate = *(SINT32 *)pArgs;
            break;
#ifdef VFMW_MODULE_LOWDLY_SUPPORT

        case VDEC_CID_START_LOWDLAY_CALC:
            g_LowDelayCalcStat[ChanID] = 1;
            g_LowDelayCalcHandle[ChanID] = *(SINT32 *)pArgs;
            break;

        case VDEC_CID_STOP_LOWDLAY_CALC:
            g_LowDelayCalcStat[ChanID] = 0;
            g_LowDelayCalcHandle[ChanID] = 0;
            break;
#endif

        case VDEC_CID_ALLOC_MEM_TO_CHANNEL:        //sdk
            ret = VCTRT_SetChanCtrlState(ChanID, CHAN_FRAME_STORE_ALLOC);

            if (VDEC_OK != ret)
            {
                ret = VDEC_ERR;
                break;
            }

            ret = VCTRL_RlsAllFrameNode(ChanID); //�ͷ�֮ǰ�Ѿ���̬�����֡��

            if (VDEC_OK != ret)
            {
                dprint(PRN_FS, "DFS, rls chan %d frame store fail!\n", ChanID);
                break;
            }

            dprint(PRN_FS, "DFS, rls chan %d frame store success!\n", ChanID);
            VCTRL_SetFsParamToChan(ChanID, (VDEC_CHAN_FRAME_STORE_PARAM_S *)pArgs);
            ret = VCTRL_RequestChanMem(ChanID);

            if (VDEC_OK == ret)
            {

                //OSAL_SpinLockIRQ(&g_TaskLock);
                vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

                VCTRL_SetChanFsPartitionState(ChanID, FSP_PARTITION_STATE_SUCCESS);
                dprint(PRN_FS, "DFS, set decoding thread state to success!\n ");

                //OSAL_SpinUnLockIRQ(&g_TaskLock);
                vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            }
            else if (0 != VCTRL_GetCurChanFsSize(ChanID))
            {
                dprint(PRN_FS, "DFS, request frame store fail!, chanid: %d\n", ChanID);

                //OSAL_SpinLockIRQ(&g_TaskLock);
                vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinLock(G_TASKLOCK);

                VCTRL_SetChanFsPartitionState(ChanID, FSP_PARTITION_STATE_FAIL);
                dprint(PRN_FS, "DFS, set decoding thread state to fail\n");

                //OSAL_SpinUnLockIRQ(&g_TaskLock);
                vfmw_Osal_Func_Ptr_S.pfun_Osal_SpinUnlock(G_TASKLOCK);

            }

            ret = VCTRT_SetChanCtrlState(ChanID, CHAN_NORMAL);

            if (VDEC_OK != ret)
            {
                ret = VDEC_ERR;
            }

            break;

        default:
            ret = VDEC_ERR;
    }

    return ret;
}


VOID GetVdecCapability( VDEC_CAP_S *pCap )
{
    SINT32  StdCnt = 0;
    UINT32 ChipID;

    //memset( pCap, 0, sizeof(VDEC_CAP_S) );
    vfmw_Osal_Func_Ptr_S.pfun_Osal_MemSet( pCap, 0, sizeof(VDEC_CAP_S) );

    pCap->s32MaxChanNum = MAX_CHAN_NUM;
    pCap->s32MaxBitRate = 50;
    pCap->s32MaxFrameWidth  = 1920;// 1920;
    pCap->s32MaxFrameHeight = 1088;// 1088;
    pCap->s32MaxPixelPerSec = 1920 * 1088 * 30 * 3 / 2; //8192*8192*30*3/2;// 1920*1088*30*3/2;   /* FHD*1.5 */

    /* �ں�̬��Ҫ��оƬID������оƬ���;���������ЩЭ�� */
    ChipID = VFMW_CHIP_TYPE_Hi3716CV200;

    switch (ChipID)
    {
        case VFMW_CHIP_TYPE_Hi3716CV200:
            pCap->SupportedStd[StdCnt++] = VFMW_H264;
            pCap->SupportedStd[StdCnt++] = VFMW_MPEG2;
            pCap->SupportedStd[StdCnt++] = VFMW_MPEG4;
            pCap->SupportedStd[StdCnt++] = VFMW_AVS;
#ifdef    VFMW_REAL8_SUPPORT
            pCap->SupportedStd[StdCnt++] = VFMW_REAL8;
#endif
#ifdef    VFMW_REAL9_SUPPORT
            pCap->SupportedStd[StdCnt++] = VFMW_REAL9;
#endif
            pCap->SupportedStd[StdCnt++] = VFMW_VC1;
            pCap->SupportedStd[StdCnt++] = VFMW_DIVX3;
            pCap->SupportedStd[StdCnt++] = VFMW_H263;
            pCap->SupportedStd[StdCnt++] = VFMW_VP6;
            pCap->SupportedStd[StdCnt++] = VFMW_SORENSON;
            pCap->SupportedStd[StdCnt++] = VFMW_VP6F;
            pCap->SupportedStd[StdCnt++] = VFMW_VP6A;
            pCap->SupportedStd[StdCnt++] = VFMW_VP8;
            pCap->SupportedStd[StdCnt++] = VFMW_MVC;
            pCap->SupportedStd[StdCnt++] = VFMW_RAW;
            pCap->SupportedStd[StdCnt++] = VFMW_USER;
            pCap->SupportedStd[StdCnt++] = VFMW_END_RESERVED;
            break;

        default:
            dprint(PRN_FATAL, "****** fatal: UNKNOWN CHIP TYPE(0x%x), no thing can be done on this chip!\n", ChipID);
            break;
    }


    return;
}

SINT32 VDEC_Suspend(VOID)
{
    VCTRL_Suspend();
    dprint(PRN_FATAL, "VDEC_Suspend ok !\n");

    return VDEC_OK;
}

SINT32 VDEC_Resume(VOID)
{

    VCTRL_Resume();

    dprint(PRN_FATAL, "VDEC_Resume ok !\n");
    return VDEC_OK;
}


VOID VFMW_set_dbgopt( UINT32 opt, UINT8 *p_args )
{
    VCTRL_SetDbgOption( opt, p_args );

    return;
}

#if 0
/* module use vfmw functions */
EXPORT_SYMBOL(VDEC_Init);
EXPORT_SYMBOL(VDEC_Exit);
EXPORT_SYMBOL(VDEC_Control);
EXPORT_SYMBOL(VDEC_InitWithOperation);
EXPORT_SYMBOL(VDEC_Suspend);
EXPORT_SYMBOL(VDEC_Resume);
EXPORT_SYMBOL(VDM_ResetClock);
#endif
