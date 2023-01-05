#ifndef __HI_SMMU_TEST_H
#define __HI_SMMU_TEST_H

#include "hi_type.h"

/*
 * the testcase of smmu interface,such as alloc, free, map, unmap...
 * XXXXX_MORE: means many time for test
 * XXXXX_ERR: means err args for test
 */
typedef enum {
	SEC_CMA_ALLOC_FREE = 0x1,
	SEC_CMA_ALLOC_FREE_MORE,
	SEC_CMA_ALLOC_FREE_ERR,
	SEC_CMA_MAP_UNMAP,
	SEC_CMA_MAP_UNMAP_MORE,
	SEC_CMA_MAP_UNMAP_ERR,
	SEC_CMA_MAP_UNMAP_CACHE,
	SEC_CMA_MAP_UNMAP_SECSMMU,
	SEC_CMA_MAP_UNMAP_SECSMMU_MORE,
	SEC_CMA_MAP_UNMAP_SECSMMU_ERR,
	SEC_SMMU_ALLOC_FREE,
	SEC_SMMU_ALLOC_FREE_MORE,
	SEC_SMMU_ALLOC_FREE_ERR,
	SEC_SMMU_MAP_UNMAP,
	SEC_SMMU_MAP_UNMAP_MORE,
	SEC_SMMU_MAP_UNMAP_ERR,
	SEC_SMMU_MAP_UNMAP_CACHE,
	SEC_NOSEC_CMA_MAP_UNMAP,
	SEC_NOSEC_CMA_MAP_UNMAP_MORE,
	SEC_NOSEC_CMA_MAP_UNMAP_ERR,
	SEC_NOSEC_CMA_MAP_UNMAP_CACHE,
	SEC_NOSEC_CMA_MAP_UNMAP_SECSMMU,
	SEC_NOSEC_CMA_MAP_UNMAP_SECSMMU_MORE,
	SEC_NOSEC_CMA_MAP_UNMAP_SECSMMU_ERR,
	SEC_NOSEC_SMMU_MAP_UNMAP,
	SEC_NOSEC_SMMU_MAP_UNMAP_MORE,
	SEC_NOSEC_SMMU_MAP_UNMAP_ERR,
	SEC_NOSEC_SMMU_MAP_UNMAP_CACHE,
	SEC_NOSEC_SMMU_MAP_UNMAP_SECSMMU,
	SEC_NOSEC_SMMU_MAP_UNMAP_SECSMMU_MORE,
	SEC_NOSEC_SMMU_MAP_UNMAP_SECSMMU_ERR,
	SEC_MMZ_ALLOC_FREE,
	SEC_MMZ_ALLOC_FREE_MORE,
	SEC_MMZ_ALLOC_FREE_ERR,
	SEC_MMZ_MAP_UNMAP,
	SEC_MMZ_MAP_UNMAP_MORE,
	SEC_MMZ_MAP_UNMAP_ERR,
	SEC_MMZ_MAP_UNMAP_CACHE,
	SEC_MMZ_FLUSH,
	SEC_MMZ_MAP_UNMAP_SECSMMU,
	SEC_MMZ_MAP_UNMAP_SECSMMU_MORE,
	SEC_MMZ_MAP_UNMAP_SECSMMU_ERR,
	SEC_FLUSH_MEM,
} TEE_OPERATION_ID; 

typedef struct test_event {
	TEE_OPERATION_ID cmd;
	int (*test_func)(HI_U32 addr, HI_U32 size);
}TEST_EVENT; 

#ifndef SMMU_RELEASE_SUPPORT
/*
 * func:test for smmu drv
 * cmd: input, the test cmd
 * addr: input, the addr of mem
 * size: input, the size of mem
 *
 * the cmd should not be lost,and the cmd indicate what kind of operation of test,
 * it alse indicate how to use the addr and size
 */
int smmu_test(unsigned long cmd, unsigned long addr, unsigned size);
#else
static int smmu_test(unsigned long cmd, unsigned long addr, unsigned size)
{
	return 0;
}
#endif
#endif
