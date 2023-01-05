
typedef enum {
       HI_MMZ_NEW = 0x1000,
       HI_MMZ_DEL,
       HI_MMZ_CP_CA2TA,
       HI_MMZ_CP_TA2CA,
       HI_MMZ_CP_TA2TA,
	HI_MMZ_TEST,
	HI_MMZ_DEBUG
} E_HI_MMZ_CommandId;

/**
\brief Allocate secure memory block.
CNcomment:\brief ���䰲ȫ�ڴ�顣CNend
\attention \n
N/A
\param[in]  size        The size of the memory block to be allocated.             CNcomment:�ڴ��ߴ� CNend
\param[in]  mmz_name    The name of the memory pool to allocate from.             CNcomment:�ڴ������ CNend
\param[in]  mmb_name    The name of the memory block.                             CNcomment:�ڴ�����֡�CNend
\retval ::non-zero   Success, return the phisical address of the block.           CNcomment:�ɹ�, �����ڴ�������ַ CNend
\retval ::NULL       Calling this API fails.                                      CNcomment:API����ʧ�� CNend
\see \n
N/A
*/
void *HI_SEC_MMZ_New(unsigned long size , char *mmz_name, char *mmb_name);

/**
\brief Relieve the secure memory block.
CNcomment:\brief �ͷŰ�ȫ�ڴ�顣CNend
\attention \n
N/A
\param[in]  phys_addr    The phisical address of the secure memory block.  CNcomment:���ͷ��ڴ��������ַ CNend
\retval ::0   Success                                                      CNcomment:�ɹ� CNend
\see \n
N/A
*/
int HI_SEC_MMZ_Delete(unsigned long phys_addr);

/**
\brief Memory copy from client side to trusted side.
CNcomment:\brief �ǰ�ȫ����ȫ�Ŀ����ӿڡ�CNend
\attention \n
N/A
\param[in]  CAphyaddr    The phisical address of client side.       CNcomment:�ǰ�ȫ�ڴ�������ַ CNend
\param[in]  TAphyaddr    The phisical address of the trusted side.  CNcomment:��ȫ�ڴ��������ַ CNend
\param[in]  CopySize     The size to be copyed.                     CNcomment:�������ڴ��С CNend
\retval ::0   Success                                               CNcomment:�ɹ� CNend
\see \n
N/A
*/
int HI_SEC_MMZ_CA2TA(unsigned long CAphyaddr, unsigned long TAphyaddr, unsigned long CopySize);

//int HI_SEC_MMZ_TA2CA(unsigned long TAphyaddr, unsigned long CAphyaddr, unsigned long CopySize);

/**
\brief Memory copy from secure mem1 to secure mem2.
CNcomment:\brief ��ȫ����ȫ�Ŀ����ӿڡ�CNend
\attention \n
N/A
\param[in]  phyaddr1    The phisical address of secure mem1.        CNcomment:��ȫ�ڴ�1�������ַ CNend
\param[in]  phyaddr2    The phisical address of secure mem2.        CNcomment:��ȫ�ڴ�2�������ַ CNend
\param[in]  CopySize     The size to be copyed.                     CNcomment:�������ڴ��С CNend
\retval ::0   Success                                               CNcomment:�ɹ� CNend
\see \n
N/A
*/
int HI_SEC_MMZ_TA2TA(unsigned long phyaddr1, unsigned long phyaddr2, unsigned long CopySize);


