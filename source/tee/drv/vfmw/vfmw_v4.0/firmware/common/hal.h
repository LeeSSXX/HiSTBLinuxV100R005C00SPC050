/*--------------------------------------------------------------------------------------------------------------------------*/
#ifndef _HAL_HEAD_
#define _HAL_HEAD_
#include "basedef.h"
#include "vfmw.h"

#ifdef __cplusplus
extern "C" {
#endif
//hal
// VDM_MEMORY
typedef struct {
	// VAHB
	UINT8*    VAHB_alloc_vir_addr;
	UINT8*    VAHB_access_vir_addr;
	UINT8*    AHB_alloc_vir_addr;
	UINT8*    AHB_access_vir_addr;

	PHYADDR   VAHB_alloc_phy_addr;
	SINT32    VAHB_alloc_size;
	PHYADDR   VAHB_access_phy_addr;
	SINT32    VAHB_access_size;
	// AHB
	PHYADDR   AHB_alloc_phy_addr;
	SINT32    AHB_alloc_size;
	PHYADDR   AHB_access_phy_addr;
	SINT32    AHB_access_size;
	// VDMREG
	PHYADDR   VDMREG_phy_addr;
	UINT32*   VDMREG_vir_addr;
} VDM_MEMORY;

VOID HiHalInit(VDM_MEMORY *p_Mem);
VOID HiWriteVhb(UINT32 DstPhyAddr, UINT8 *SrcVirAddr, SINT32 Len);
VOID HiReadVhb(UINT8 *DstVirAddr, UINT32 SrcPhyAddr, SINT32 Len);
VOID HiWriteAhb(UINT32 DstPhyAddr, UINT8 *SrcVirAddr, SINT32 Len);
VOID HiReadAhb(UINT8 *DstVirAddr, UINT32 SrcPhyAddr, SINT32 Len);
VOID HiWriteReg(UINT32 Base, UINT32 OffSet, UINT32 Val, UINT32 Mask);
UINT32 HiReadReg(UINT32 Base, UINT32 OffSet, UINT32 Mask);

#define GETAHBPHYADDR(a) (g_Memory.AHB_alloc_phy_addr+((UINT8 *)(a)-g_Memory.AHB_alloc_vir_addr))
#define GETAHBVIRADDR(a) (g_Memory.AHB_alloc_vir_addr+((a)-g_Memory.AHB_alloc_phy_addr))
#define GETVHBPHYADDR(a) (g_Memory.VAHB_alloc_phy_addr+((UINT8 *)(a)-g_Memory.VAHB_alloc_vir_addr))
#define GETVHBVIRADDR(a) (g_Memory.VAHB_alloc_vir_addr+((a)-g_Memory.VAHB_alloc_phy_addr))

#ifdef __cplusplus
}
#endif
#endif

