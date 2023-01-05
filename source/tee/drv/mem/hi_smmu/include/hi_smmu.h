#ifndef _HI_SMMU_H
#define _HI_SMMU_H

#include "hi_list.h"
#include "tee_os_hal.h"

#define HIL_MAX_NAME_LEN	16
#define UUID_LENGTH	16

typedef enum{
        HISI_MEM_ALLOC = 0x0,
        HISI_MEM_FREE,
        HISI_MEM_GET_MEMINFO,
	HISI_MEM_PUT_MEMINFO,
	HISI_AGENT_CLOSE,
} smmu_cmd_t;

typedef struct tz_pageinfo {
	unsigned int phys_addr;
	unsigned int npages;
} TEE_PAGEINFO;

typedef struct tz_memblocks {
	unsigned int sec_smmu;
	unsigned int phys_addr;
	unsigned int normal_smmu;
	unsigned int total_size;
	unsigned int pageinfoaddr;
	unsigned int nblocks;
} TEE_MEMBLOCKS;

typedef struct tz_page {
	TEE_PAGEINFO *tz_pageinfo;
	struct list_head list;
} TZ_PAGEINFO;

typedef struct tz_smmu {
	TEE_MEMBLOCKS *tz_mblock;
	struct list_head tz_page_list;
} TZ_SMMUINFO;

typedef struct ta_map_data {
	TEE_UUID uuid;
	int tcached;
	void *ta_virt;
	int ta_map_ref;
	struct list_head list;
}TA_MAP;

typedef struct sec_mmb {
	char name[HIL_MAX_NAME_LEN];
        unsigned long sec_smmu;
        unsigned long phys_addr;
        unsigned long meminfo_addr; /* page info address  */
	unsigned int nblocks; /* nr blocks for sg pages */
	int memtype;  /* 1: system mem  0:cma mem  */
        unsigned long size;
        void *sec_virt;
        int flag;       /* indicate whether the mem is secure or not */
        int t_ref;      /* t_ref = smmu_ref + map_ref   */
        int smmu_ref;
        int map_ref;
	int kmap_ref;
        char *uuid;
	unsigned long nosec_smmu; /* for normal mem to map sec cpu or sec smmu*/
	int cached;
        struct list_head list;
        struct list_head t_list;
	int is_smmu_map_clear; /*indicate the smmu map if clear. 1: clear; 0 : no clear  */
} SEC_MMB;

typedef struct smmu_ctrl_t{
	smmu_cmd_t cmd;
	int memtype; //0:cma mem; 1: system mem;
	unsigned int size;
	char name[HIL_MAX_NAME_LEN];
	unsigned int     phys_addr;
	unsigned int     normal_smmu;
	unsigned int	sec_smmu;
	unsigned int tz_mblock_phys;
} SMMU_AGENT_CTRL;

#define SMMU_AGENT_ID (0x1234)

#define HI_MEM_PROC 0x2005

typedef struct {
	char *bufname;
	unsigned long phys_addr;
	unsigned long sec_addr;
	unsigned long nonsec_addr;
	unsigned long buf_phys;
	unsigned long buf_size;
	unsigned long smmu_addr;
	unsigned long align;
	unsigned long cmd;
	unsigned long arg0;
	unsigned long arg1;
	int memtype;
	int share_mem;
} HI_TEE_SMMU_DRV_IOCTRL_BUF;

enum SMMU_DRV_IOCTL_FUNC_ID
{
	HISI_SEC_MAPTOSMMU = 0x0,
	HISI_SEC_UNMAPFROMSMMU,
	HISI_SEC_MAPTOSMMU_AND_SETFLAG,
	HISI_SEC_UNMAPFROMSMMU_AND_CLRFLG,
	HISI_SEC_ALLOC,
	HISI_SEC_FREE,
	HISI_SEC_MAP_TO_SEC_SMMU,
	HISI_SEC_UNMAP_FROM_SEC_SMMU,
	HISI_NOSEC_MEM_MAP_TO_SEC_SMMU,
	HISI_NOSEC_MEM_UNMAP_FROM_SEC_SMMU,
	AGENT_CLOSED,
	SEC_IOCTL,
	CHECK_SEC_SMMU,
	CHECK_SEC_MMZ,
};


int smmu_hardware_resume(void);

int smmu_common_init_s(unsigned long smmu_e_raddr,
		unsigned long smmu_e_waddr, unsigned long smmu_pgtbl);

/*
 * func: map to sec-smmu
 * buf_phys: input, buffer phy_addr from no secure world, which store the mem info
 * buf_size: input, buffer size
 * return: sec-smmu addr if exec success
 *	   0 if exec failed
 */
unsigned int hisi_sec_maptosmmu(unsigned long buf_phys, unsigned long buf_size);

/*
 * func: unmap from sec-smmu
 * buf_phys: input, buffer phy_addr from no secure world, which store the mem info
 * buf_size: input, buffer size
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_sec_unmapfromsmmu(unsigned long buf_phys, unsigned long buf_size);

/*
 * func: map to sec-smmu, and set sec flag
 * buf_phys: input, buffer phy_addr from no secure world, which store the mem info
 * buf_size: input, buffer size
 * return: sec-smmu if exec success
 *	   0 if exec failed
 */
unsigned int hisi_sec_maptosmmu_and_setflag(unsigned long buf_phys,
				      unsigned long buf_size);

/*
 * func: clear sec flag and unmap from sec-smmu
 * buf_phys: input, buffer phy_addr from no secure world, which store the mem info
 * buf_size: input, buffer size
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_sec_unmapfromsmmu_and_clrflg(unsigned long buf_phys,
				      unsigned long buf_size);

/*
 * func: alloc a mem(cma of system) and set sec flag
 * bufname: input, the name of mem
 * size: input, the size of mem with which it will be allocated
 * alogn: input, resverd
 * memtype: input, mem type,1 system(smmu), 0 cma
 * return:  phys_addr or sec-smmu if exec success
 *	    0 if exec failed
 */
unsigned int hisi_sec_alloc(const char *bufname, unsigned long size,
		      unsigned long align, int memtype);

/*
 * func: free the mem and clear the sec flag
 * sec_addr: input, sec addr, sec-smmu or phys_addr
 * memtype: input, the address type, 1 sec_addr == sec_smmu, 0 sec_addr == phys_addr
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_sec_free(unsigned long sec_addr, int memtype);

/*
 * func: map to sec cpu in user space
 * sec_addr: input, sec addr of sec mem
 * memtype: input, the address type, 1 sec_addr == sec_smmu, 0 sec_addr == phys_addr
 * cached: input, the cache attr when to map cpu, 1 map with cache, 0 map with no cache
 * return: sec cpu virt address if exec success
 *	   NULL if exec failed
 */
unsigned int hisi_sec_map_to_cpu(unsigned long sec_addr, int memtype, int cached);


/*
 * func: unmap from sec cpu in user space
 * sec_addr: input, sec addr of sec mem
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_sec_unmap_from_cpu(void *sec_virt);

/*
 * func: map to sec cpu in kernel space
 * sec_addr: input, sec addr of sec mem
 * memtype: input, the address type, 1 sec_addr == sec_smmu, 0 sec_addr == phys_addr
 * datalen: input, the user mem map len
 * cached: input, the cache attr when mapping cpu, 1 cached map, 0 no cache map
 * return: sec cpu virt address if exec success
 *	   NULL if exec failed
 */
unsigned int hisi_sec_kmap_to_cpu(unsigned long sec_addr, unsigned int datalen,
					int memtype, int cached);

/*
 * func: unmap from sec cpu in kernel space
 * sec_addr: input, sec addr of sec mem
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_sec_kunmap_from_cpu(void *sec_virt);

/*
 * func: map to sec smmu:(sec share mem(cma) or sec mmz)
 * phys_addr: input, phys addr of sec mem(cma or sec mmz)
 * size: input, the size of mem
 * share_mem: input, the address type, 1 phys_addr is sec share mem(cma), 0 phys_addr is sec-mmz
 * return: sec smmu if exec success
 *	   0 if exec failed
 */
unsigned int hisi_sec_map_to_sec_smmu(unsigned long phys_addr, unsigned long size,
							int share_mem);

/*
 * func: unmap from sec smmu:(sec share mem(cma) or sec mmz)
 * sec_addr: input, sec smmu addr of sec mem(cma or sec mmz)
 * share_mem: input, the address type, 1 sec share mem(cma), 0  sec-mmz
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_sec_unmap_from_sec_smmu(unsigned long sec_smmu, int share_mem);

/*
 * func: normal mem map to sec cpu in user space
 * nonsec_addr: input, normal mem addr (nosec smmu or phys)
 * memtype: input, the address type of nonsec_addr, 1 nosec smmu, 0  phys addr
 * cached: input, the cache attr when to map cpu, 1 map with cached, 0 map with no cached
 * return: sec cpu virt addr if exec success
 *	   NULL if exec failed
 */
unsigned int hisi_nonsec_mem_map_to_sec_cpu(unsigned long nonsec_addr, int memtype,
								int cached);

/*
 * func: normal mem unmap from sec cpu in user space
 * va_addr: input, sec cpu virt addr of mem
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_nosec_mem_unmap_from_sec_cpu(void *va_addr);

/*
 * func: normal mem map to sec cpu in kernel space
 * nonsec_addr: input, normal mem addr (nosec smmu or phys)
 * memtype: input, the address type of nonsec_addr, 1 nosec smmu, 0  phys addr
 * datalen: input, the user mem len
 * cached: input, the cache attr when to map, 1 mapped with cache, 0 mapped with no cache
 * return: sec cpu virt addr if exec success
 *	   NULL if exec failed
 */
unsigned int hisi_nonsec_mem_kmap_to_sec_cpu(unsigned long nonsec_addr, unsigned int datalen, int memtype,
								int cached);

/*
 * func: normal mem unmap from sec cpu in kernel space
 * va_addr: input, sec cpu virt addr of mem
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_nosec_mem_kunmap_from_sec_cpu(void *va_addr);

/*
 * func: normal mem map to sec smmu
 * nonsec_addr: input, normal mem addr (nosec smmu or phys)
 * memtype: input, the address type of nonsec_addr, 1 nosec smmu, 0  phys addr
 * return: sec smmu addr if exec success
 *	   0 if exec failed
 */
unsigned int hisi_nosec_mem_map_to_sec_smmu(unsigned long nonsec_addr, int memtype);

/*
 * func: normal mem unmap from sec smmu
 * sec_smmu: input, sec smmu of mem
 * return: 0 if exec success
 *	   -1 if exec failed
 */
int hisi_nosec_mem_unmap_from_sec_smmu(unsigned long sec_smmu);

/*
 * func: get smmu pgtbl base and dustbin base of smmu write/read
 * smmu_e_raddr: output, the dustbin base of smmu read
 * smmu_e_waddr: output, the dustbin base of smmu write
 * smmu_pgtbl: output, the smmu pgtbl base
 */
void get_sec_smmu_pgtblbase(unsigned long *smmu_e_raddr,
				unsigned long *smmu_e_waddr,
				unsigned long *smmu_pgtbl);

/*
 * func: closed agent
 * when unregister agent from REE, TEE should give a message to agent in REE
 * to finish the thread.
 */
int agent_closed(void);

/*
 * func: a general system call
 * cma:input, means the command
 * arg0:input
 * arg1:input
 */
int sec_ioctl(unsigned long cmd, unsigned long arg0, unsigned long arg1);

/*
 * func: flush mem via virt and size
 * virt: input, the virt addr of mem
 * size: input, the size of mem
 */
void hisi_mem_flush(void *virt, unsigned long size);

/*
 * func: check if the addr is sec or not
 * addr: input, the addr need to check
 * iommu: input, the type of addr, 1 smmu , 0 phys
 * is_sec: output, the sec attr of addr, legal value only return success
 *
 */
int is_sec_mem(unsigned int addr, int iommu, bool *is_sec);

/*
 * func: smmu module init function
 */
int smmu_init(void);
#endif
