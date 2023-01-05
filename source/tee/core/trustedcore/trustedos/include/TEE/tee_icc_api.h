#ifndef __TEE_ICC_API_H
#define __TEE_ICC_API_H
#include "tee_internal_api.h"
#include "tee_ext_api.h"
#include "tee_log.h"

#ifndef UNUSED
#define UNUSED(a) (a=a)
#endif

#define BSP_ERR_ICC_BASE              (int)(0x80000000 | 0x10180000)
#define BSP_ERR_ICC_CCORE_RESETTING   (BSP_ERR_ICC_BASE + 0x12)      /* modem������λ */
#define ICC_INVALID_NO_FIFO_SPACE     (BSP_ERR_ICC_BASE + 0x13)      /* ͨ���� */
#define MODEM_CALL_IS_STUB            (BSP_ERR_ICC_BASE + 0xff)      /* Ϊ׮�ӿڣ���ƽ̨��֧�ְ�ȫͨ�� */
#define ICC_CHANNEL_IS_OPEN           (0x80000000 + 0x0b)            /* ��ͨ���Ѿ����� */
/**
 * ���ð�ȫICC����֧�ֵ�����ID
 */
typedef enum FUNC_MODEM_CMD_ID {
    /*0x600-0x800:TA-->����*/
    FUNC_ICC_CA_CFG_GET = 0x600,       /* ��ȡCA session��ʼ�����ò��� */
    FUNC_MDRV_ICC_OPEN,                /* ��һ����ȫICCͨ��*/
    FUNC_MDRV_ICC_CLOSE,               /* �ر�һ����ȫICCͨ�� */
    FUNC_MDRV_ICC_WRITE,               /* ͨ����ȫICCͨ���������� */
    FUNC_MDRV_ICC_READ,                /* ͨ����ȫICCͨ��������ȡ���� */
    FUNC_MDRV_ICC_TRYREAD,             /* ͨ����ȫICCͨ�����Զ�ȡ���� */

}FUNC_CMD_ID;

/* ͨ��id���� */
enum ICC_S_CHN_ID
{
    ICC_CHN_SEC_IFC  = 17,        /* ����IPC������ֱ�ӻص�����������ȫOS��ccore��������ͨ�� */
    ICC_CHN_SEC_VSIM = 18,        /* ˽��IPC������ֱ�ӻص������������ͨ��ȫOS��ccore����ͨ�� */

    ICC_CHN_ID_MAX
};

/* ˵��: ���ջص�����ID,��ͨ�����������, "ͨ������_xxx=0��ͨ������_RECV_FUNC_ID_MAX֮�� */
enum ICC_S_RECV_FUNC_ID{
    IFC_RECV_FUNC_MODULE_VERIFY = 0,

    /* ��Ҫ������ͨ���϶�����ͨ��,���ڸ�ע����֮ǰ���� */
    IFC_RECV_FUNC_ID_MAX,

    VSIM_RECV_FUNC_SUB0 = 0,

    /* ��Ҫ������ͨ���϶�����ͨ��,���ڸ�ע����֮ǰ���� */
    VSIM_RECV_FUNC_ID_MAX  

};

typedef struct tag_CA_ICC_ATTR_S {
    TEE_UUID     svc_uuid;        /* TA��UUID */
    uint8_t      svc_name[32];    /* TA������ */
    uint32_t     session_cmd_id;  /* CMD_ID */
    uint32_t     task_priority;   /* ��AP��session������������ȼ� */
 }CA_ICC_ATTR_S;

/*****************************************************************************
* �� �� ��  : mdrv_icc_open
* ��������  : ��iccͨ���ӿ�
* �������  : unsigned int channel_id  channel_id = ͨ��id << 16 | function_id, ʹ��Լ��:
                1) channel_id��16bitΪͨ����ʶID��ʹ��enum ICC_CHN_ID
                2) ��16bitΪ�ص�������ʶID��ʹ��ICC_RECV_FUNC_ID��Ӧͨ��id��ö��ֵ
              *pCaAttr�����NULL,�����û��Լ���CA��session�Ͷ��̣߳�
                      ����и�ֵ��������CA��session�Ͷ��̣߳����ڰ�ȫOS�̻߳������⣬�ù����ݲ���
* �������  : ��
* �� �� ֵ  : ��ȷ: 0;
              ����: ������;
              ���ⷵ��ֵ:MODEM_CALL_IS_STUB:openΪ׮�ӿڣ�����ƽ̨��֧�ְ�ȫͨ��
              ICC_CHANNEL_IS_OPEN�������ʾ�ظ�open
* ˵    ��  : 
*****************************************************************************/
static inline int mdrv_icc_open(unsigned int channel_id, CA_ICC_ATTR_S *pCaAttr)
{
    UNUSED(pCaAttr);
    return __bsp_modem_call(FUNC_MDRV_ICC_OPEN, channel_id, NULL, 0);
}

/*****************************************************************************
* �� �� ��  : mdrv_icc_close
* ��������  : �ر�iccͨ���ӿ�
* �������  : unsigned int channel_id  channel_id = ͨ��id << 16 | function_id, ʹ��Լ��:
                1) channel_id��16bitΪͨ����ʶID��ʹ��enum ICC_CHN_ID
                2) ��16bitΪ�ص�������ʶID��ʹ��ICC_RECV_FUNC_ID��Ӧͨ��id��ö��ֵ
* �������  : ��
* �� �� ֵ  : ��ȷ: 0;
              ����: ������;
* ˵    ��  : 
*****************************************************************************/
static inline int mdrv_icc_close(unsigned int channel_id)
{
    return __bsp_modem_call(FUNC_MDRV_ICC_CLOSE, channel_id, NULL, 0);
}

/*****************************************************************************
* �� �� ��  : mdrv_icc_write
* ��������  : icc�첽�������ݽӿ�
* �������  : unsigned int cpuid       ����Ҫ���͵�cpu�ı�ţ�ʹ��Լ��: Ҫʹ��enum CPU_ID�����Ա��ö��ֵ
*             unsigned int channel_id  channel_id = ͨ��id << 16 | function_id, ʹ��Լ��:
                               1) channel_id��16bitΪͨ����ʶID��ʹ��enum ICC_CHN_ID��ö��ֵ
                               2) ��16bitΪ�ص�������ʶID��ʹ��ICC_RECV_FUNC_ID��Ӧͨ��id��ö��ֵ
*             u8 *buffer      Ҫ���͵�����buffer��ָ��
*             unsigned int buf_len    Ҫ���͵����ݵĴ�С, ʹ��Լ��: ���ֵ < fifo_size - ICC_CHANNEL_PACKET_LEN
* �������  : ��
* �� �� ֵ  : ��ȷ: ʵ��д��fifo�Ĵ�С;
              ����: ��ֵ;
              ���ⷵ��ֵ: BSP_ERR_ICC_CCORE_RESETTING ����modem���ڸ�λ���˽׶ΰ�ȫͨ�����رգ�ֱ��modem�����ָ���
                          ICC_INVALID_NO_FIFO_SPACE ����ͨ��������
* ˵    ��  : ���Ͷ˱����з�ѹ��ͨ�����ݷ��ͻ�����������ơ�

*****************************************************************************/
static inline int mdrv_icc_write(unsigned int channel_id, unsigned char *buffer, unsigned int buf_len)
{
    return __bsp_modem_call(FUNC_MDRV_ICC_WRITE, channel_id, buffer, buf_len);
}

/*****************************************************************************
* �� �� ��  : mdrv_icc_read
* ��������  : icc��fifo��ȡ���ݽӿ�
* �������  : unsigned int channel_id  channel_id = ͨ��id << 16 | function_id, ʹ��Լ��:
                1) channel_id��16bitΪͨ����ʶID��ʹ��enum ICC_CHN_ID
                2) ��16bitΪ�ص�������ʶID��ʹ��ICC_RECV_FUNC_ID��Ӧͨ��id��ö��ֵ
*             unsigned char *buffer    Ҫ��ȡfifo������buffer��ָ��
*             unsigned int  buf_len    ����buffer�Ĵ�С
* �������  : ��
* �� �� ֵ  : ��ȷ: ʵ�ʴ�ͨ����ȡ���ݴ�С;
              0:    ͨ���������ݣ�
              ����: ��ֵ
* ˵    ��  : 1) �����openʱ������uuid��sessionid��task_priority�ȣ�mdrv_icc_read�������openʱ�������Ϊ���session�У�
              2) �����openʱ�����*pCaAttrΪNULL��mdrv_icc_read�������ʹ�����Լ����session�С�
              ���ڰ�ȫOS�̻߳������⣬�ù����ݲ���,��ʹ��mdrv_icc_tryread�ӿ������
*****************************************************************************/
static inline int mdrv_icc_read(unsigned int channel_id, unsigned char *buffer, unsigned int buf_len)
{
    UNUSED(channel_id);
    UNUSED(buffer);
    UNUSED(buf_len);
    return 0;
    //return __bsp_modem_call(FUNC_MDRV_ICC_READ, channel_id, buffer, buf_len);
}

/*****************************************************************************
* �� �� ��  : mdrv_icc_tryread
* ��������  : icc��fifo��ȡ���ݽӿ�
* �������  : unsigned int channel_id  channel_id = ͨ��id << 16 | function_id, ʹ��Լ��:
                1) channel_id��16bitΪͨ����ʶID��ʹ��enum ICC_CHN_ID
                2) ��16bitΪ�ص�������ʶID��ʹ��ICC_RECV_FUNC_ID��Ӧͨ��id��ö��ֵ
*             unsigned char *buffer    Ҫ��ȡfifo������buffer��ָ��
*             unsigned int  buf_len    ����buffer�Ĵ�С

* �������  : ��
* �� �� ֵ  : ��ȷ: ʵ�ʴ�ͨ����ȡ���ݴ�С;
              0:    ͨ���������ݣ�
              ����: ��ֵ
* ˵    ��  :
*****************************************************************************/
static inline int mdrv_icc_tryread(unsigned int channel_id, unsigned char *buffer, unsigned int buf_len)
{
    return __bsp_modem_call(FUNC_MDRV_ICC_TRYREAD, channel_id, buffer, buf_len);
}
#endif
