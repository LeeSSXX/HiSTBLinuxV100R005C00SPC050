/*========================================================================
Open MAX   Component: Video Encoder
This module contains the class definition for openMAX decoder component.
==========================================================================*/
#ifndef __OMX_VENC_H__
#define __OMX_VENC_H__

//khrons standard head file
#include "OMX_Component.h"
#include "OMX_Video.h"
//#include "OMX_Custom.h"


#include "OMX_Types.h"

#include "hi_drv_venc.h"

#include "omx_allocator.h"
#include "omx_venc_queue.h"
#include "omx_venc_drv.h"

//specific-version NO.1.1
#define KHRONOS_1_1                                  //??
#define OMX_SPEC_VERSION            0x00000101       //�汾�ź궨��

#define MAX_PORT_NUM                2

#define MAX_BUF_NUM_INPUT            6//30
#define MAX_BUF_NUM_OUTPUT            20//30

#define MAX_FRAME_WIDTH                1920
#define MAX_FRAME_HEIGHT            1088

#define DEFAULT_FPS                    30
#define DEFAULT_BITRATE             (5*1024*1024)
#define DEFAULT_ALIGN_SIZE            4096                      //?? ��˵�е�4K�߽�߽�?

#define DEF_MAX_IN_BUF_CNT            1  //3
#define DEF_MIN_IN_BUF_CNT            1

#define DEF_MAX_OUT_BUF_CNT            2
#define DEF_MIN_OUT_BUF_CNT            1   //6

#define DEF_OUT_BUFFER_SIZE         (1000000)

#define ENABLE_BUFFER_LOCK
/**
* A pointer to this struct is passed to the OMX_SetParameter when the extension
* index for the 'OMX.google.android.index.enableAndroidNativeBuffers' extension
* is given.
* The corresponding extension Index is OMX_GoogleIndexUseAndroidNativeBuffer.
* This will be used to inform OMX about the presence of gralloc pointers
instead
* of virtual pointers
*/

typedef struct StoreMetaDataInBuffersParams
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bStoreMetaData;
} StoreMetaDataInBuffersParams;

struct PrependSPSPPSToIDRFramesParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_BOOL bEnable;
};

typedef struct AutoRequestIFrmParams
{
    OMX_U32 nSize;
    OMX_U32 nRes;
    OMX_BOOL bEnable;
} AutoRequestIFrmParams;
////////////////////////////////////////////////////////////////////////// add end
#define BITS_PER_LONG                32

//#define ESHUTDOWN                    108
//#define ERESTARTSYS                    512

/*������������е� �汾�źͽṹ��С*/
#define CONFIG_VERSION_SIZE(pParam,Type)  do{\
        pParam->nVersion.nVersion = OMX_SPEC_VERSION;\
        pParam->nSize             = sizeof(Type);\
    }while(0)


#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))                  //��������к��е�Ԫ�ظ���


#define OMX_CHECK_ARG_RETURN(a)  do{if ((a)){DEBUG_PRINT_ERROR("[%s],line: %d", __func__,__LINE__);return OMX_ErrorBadParameter;}}while(0)

#define ALIGN_UP(val, align) ( (val+((align)-1))&~((align)-1) )

#define FRAME_SIZE(w , h)  (((w) * (h) * 2))  //(((w) * (h) * 3) / 2)

// state transactions pending bits
#if 0
enum                                     /*��component˽�нṹ�У���Ϊ���ִ���ı�־����������¼��󣬰Ѹ�bitλ�û�0*/
{
    OMX_STATE_IDLE_PENDING                = 0x1,               //idle(����)�ȴ���־,�ñ�־Ϊ1ʱ������Ӧ�¼��������д���õȴ��Ĳ������������Ѹ�λ �û�0
    OMX_STATE_LOADING_PENDING            = 0x2,
    OMX_STATE_INPUT_ENABLE_PENDING        = 0x3,
    OMX_STATE_OUTPUT_ENABLE_PENDING        = 0x4,
    OMX_STATE_OUTPUT_DISABLE_PENDING    = 0x5,
    OMX_STATE_INPUT_DISABLE_PENDING        = 0x6,
    OMX_STATE_OUTPUT_FLUSH_PENDING        = 0x7,
    OMX_STATE_INPUT_FLUSH_PENDING        = 0x8,
    OMX_STATE_PAUSE_PENDING                = 0x9,
    OMX_STATE_EXECUTE_PENDING            = 0xA
};
#endif

// Deferred callback identifiers
/*enum {                                                        //���ڱ�ʾ���� CMD ����,�Ա��ڲ�����е�ʱ���ж�Ҫ����Ϣ�Ĵ�����Ҫ�����ĸ�����
    OMX_GENERATE_COMMAND_DONE            = 0x1,
    OMX_GENERATE_FTB                    = 0x2,
    OMX_GENERATE_ETB                    = 0x3,
    OMX_GENERATE_COMMAND                = 0x4,
    OMX_GENERATE_EBD                    = 0x5,
    OMX_GENERATE_FBD                    = 0x6,
    OMX_GENERATE_FLUSH_INPUT_DONE        = 0x7,
    OMX_GENERATE_FLUSH_OUTPUT_DONE        = 0x8,
    OMX_GENERATE_START_DONE                = 0x9,
    OMX_GENERATE_PAUSE_DONE                = 0xA,
    OMX_GENERATE_RESUME_DONE            = 0xB,
    OMX_GENERATE_STOP_DONE                = 0xC,
    OMX_GENERATE_EOS_DONE                = 0xD,
    OMX_GENERATE_HARDWARE_ERROR            = 0xE,
    OMX_GENERATE_IMAGE_SIZE_CHANGE        = 0xF,
    OMX_GENERATE_CROP_RECT_CHANGE        = 0x10,
    OMX_GENERATE_UNUSED                    = 0x11
};*/

enum
{
    INPUT_PORT_INDEX    = 0,
    OUTPUT_PORT_INDEX    = 1
};

struct port_property                                           //��Ϊ�˿�˽�нṹ�ĳ�Ա����֮һ,�����˶˿�����
{
    OMX_U32 port_dir;                                     //�˿ڵķ���:in /out
    OMX_U32 min_count;                                    //��Ӧ�ڱ�׼OMX_PARAM_PORTDEFINITIONTYPE���Ͷ����е�nBufferCountMin��������ʶThe minimum number of buffers this port requires
    OMX_U32 max_count;                                    //��Ӧ�ڱ�׼OMX_PARAM_PORTDEFINITIONTYPE���Ͷ����е�nBufferCountActual��������ʶThe actual number of buffers allocated on this port
    OMX_U32 buffer_size;                                  //��Ӧ�ڱ�׼OMX_PARAM_PORTDEFINITIONTYPE���Ͷ����е�nBufferSize����ʶSize, in bytes, for buffers to be used for this channel
    OMX_U32 alignment;                                    //���������ڴ�֮��Ķ���!!4K����
};

/*struct omx_hisi_extern_index
{
    OMX_S8 index_name[OMX_MAX_STRINGNAME_SIZE];
    OMX_HISI_EXTERN_INDEXTYPE index_type;
};*/

struct codec_info                                              //������Э�����ͽṹ��,�������:
{
    const OMX_STRING role_name;                                     //������������(uchar������)
    OMX_VIDEO_CODINGTYPE compress_fmt;                         //OMX��׼ͷ�ļ�omx_video.h�ж������ƵЭ��ö������,���ո�ֵ��OMX_COMPONENT_PRIVATE���͵�m_dec_fmt��OMX_VIDEO_PARAM_PORTFORMATTYPE��OMX_VIDEO_PORTDEFINITIONTYPE�����е�eCompressionFormat
    //enum venc_codec_type codec_type;                           //OMX��׼ͷ�ļ�omx_video.h�ж������ƵЭ��ö������,���ո�ֵ��OMX_COMPONENT_PRIVATE���͵�drv_ctx.venc_chan_attr[VENC_MAX_CHN].chan_cfg.protocol,���嶨����hisi_venc.h��Ӧ��ԭ��hi_unf_common.h�ж���һ��
    HI_UNF_VCODEC_TYPE_E codec_type;
};

struct codec_profile_level                                   //���벻��Ҫ� ��ʱ����
{
    OMX_S32 profile;
    OMX_S32 level;
};/**/


struct frame_info                                            //�Ѱ����ڱ���ͨ��������
{
    OMX_U32 frame_width;
    OMX_U32 frame_height;
    OMX_U32 stride;
    //OMX_U32 scan_lines;
    //OMX_U32 crop_top;
    //OMX_U32 crop_left;
    //OMX_U32 crop_width;
    //OMX_U32 crop_height;
};/**/

// port private for omx
typedef struct OMX_PORT_PRIVATE                                 //�˿ڵ�˽�нṹ�壬��Ϊ�����˽�нṹ��ĳ�Ա֮һ����OMX_COMPONENT_PRIVATE�����У���Ӧ����/����˿ڸ���һ��
{
    OMX_BUFFERHEADERTYPE** m_omx_bufhead;                       //��ӦOMX��׼���壬���ڴ�Ÿö˿�Ҫ�����buffer
    //struct port_property port_pro;                              //�˿����Խṹ�壬���а��������ݶ��ڱ�׼�Ķ˿�����OMX_PARAM_PORTDEFINITIONTYPE�ж�Ӧ

    OMX_PARAM_PORTDEFINITIONTYPE port_def;
    //OMX_U32    m_port_index;                                       //�˿�ID������   ��OMX_PARAM_PORTDEFINITIONTYPE�����ж��壻

    OMX_U32 m_buf_cnt;                                          //�൱�ڱ�ʶ�˿ڵ�i��buffer�������ݵ�flag��m_buf_cnt��ÿһλΪ1�����λ��Ӧ����ŵ�buffer�����ݣ��������ж�port buffer �Ŀ������ҿ�д��Ŀ���buffer�Ȳ���

    OMX_U32 m_usage_bitmap;                                     //������m_buf_cnt���÷����ƣ�ֻ�������� mark buffer to be allocated(��ʶ�Լ����뵽��buffer�������Ժ���free����)
    OMX_U32    m_buf_pend_cnt;                                     //��¼���û������buffer��Ŀ(ʵ��������ڲ��Ѿ�������ɣ�ֻ�ǵȴ���֪�û�)~~��:fill_this_buffer_porxyʱ++,��fill_this_buffer_doneʱ--.

    //OMX_U32    m_port_enabled;                                     //�˿�ʹ�ܱ�־λ��Ҳ��Ӧ�ڶ˿ڱ�׼����OMX_PARAM_PORTDEFINITIONTYPE�е�bEnabledλ
    //OMX_U32    m_port_populated;                                   //�˿ڳ���(?)��־λ��Ҳ��Ӧ�ڶ˿ڱ�׼����OMX_PARAM_PORTDEFINITIONTYPE�е�bPopulatedλ������������bPopulated��ע�͡���ʾ�˿��������buffer��Ŀ�Ѿ��ﵽ���

    OMX_BOOL m_port_reconfig;                                   //�˿��������ñ�־λ��Ĭ��Ϊfalse��������OMX_GENERATE_IMAGE_SIZE_CHANGE�����¼�ʱ���Ѹñ�ʶ��1����ʱ���ܽ���fill/empty_this_buffer�Ȳ������ȵ��ӵ�COMMON_DONE��OMX_CommandPortEnable���¼�����ʱ�Ż�����û�0
    OMX_BOOL m_port_flushing;                                   //�˿�bufferˢ�±�־λ��Ĭ��Ϊfalse�����յ�OMX_CommandFlush����ʱ����handle_command_flush���������Ѹñ�ʶ��1����ʱ���ܽ���fill/empty_this_buffer�Ȳ������ȵ��ӵ�OMX_GENERATE_FLUSH_INPUT_DONE���¼�����ʱ�Ż�����û�0


    //struct frame_info pic_info;               //��venc_driver_context�����������ж�������ȸ߶���Ϣ���ɷ���?   (Ŀǰ�൱�������֡��Ϣ)

    venc_user_info** m_venc_bufhead;                 //??���ڶԱ�׼m_omx_bufhead�Ĳ���buffer??

} OMX_PORT_PRIVATE;

//component private for omx                                     //�����˽�нṹ�壬ʵ��Ӧ�þ��󲿷ֶ�������ṹ�壬���а�����׼�����е�OMX_COMPONENTTYPE��������ָ��
typedef struct OMX_COMPONENT_PRIVATE
{
    pthread_mutex_t m_lock;                                     //���������ڶԹ�����Դ�Ķ�д����ʱ��һ��Ҫ�û��������Ա���������Ĺ�����Դ��Ҫ��:�������У��ܵ���д��
    OMX_COMPONENTTYPE* m_pcomp;                                 //����ṹ��
    OMX_STATETYPE m_state;                                      //���״̬
    OMX_U32 m_flags;                                            //ÿһλ��Ӧ��flags_bit_positions�ж�����¼�
    OMX_VIDEO_CODINGTYPE m_encoder_fmt;                         //��codec_info��Ӧ
    OMX_S8 m_role[OMX_MAX_STRINGNAME_SIZE];                     //�����������ֱ�ʶ
    OMX_S8 m_comp_name[OMX_MAX_STRINGNAME_SIZE];                //����������ָ��
    OMX_PTR m_app_data;                                         //���ⲿӦ�ó���APP��������һ�����������ڸ�APP���ֲ�ͬ��component����Ӧ�ó������OMX_GetHandleʱ��ص������ṹ��ָ��һ����
    OMX_CALLBACKTYPE m_cb;                                      //���ⲿӦ�ó���APP�������ص�����ָ�룬��Ӧ�ó������OMX_GetHandleʱ������m_app_data�ص������ṹ��ָ��һ����

    OMX_TICKS m_pre_timestamp;                                  //pts ʱ���

    OMX_PORT_PRIVATE m_port[MAX_PORT_NUM];                      //port��˽�нṹ
    OMX_BOOL m_use_native_buf;                                  //������buffer��ʶ����get/set_parameter�� OMX_GoogleIndexGetAndroidNativeBufferUsage  �жϷ�֧��Ӧ��

    OMX_BOOL m_store_metadata_buf;                              //������buffer��ʶ����get/set_parameter�� OMX_GoogleIndexStoreMetaDataInBuffers  �жϷ�֧��Ӧ��
    OMX_BOOL m_prepend_sps_pps;                                 //���ڱ�ʾ��ǰ�Ƿ�֧����ÿ��I֡ǰ����SPS/PPS����
    pthread_t msg_thread_id;                                    //��Ϣ�߳�     ->write
    pthread_t event_thread_id;                                  //�¼������߳� ->read

    volatile OMX_BOOL event_thread_exit;                                 //�¼������߳��˳���־
    volatile OMX_BOOL msg_thread_exit;                                   ////��Ϣ�߳��˳���־


    venc_drv_context drv_ctx;         //��ΪComponent��˽�нṹ��������omx_venc_drv.h�а����������豸�ļ���ʶ���š�������ͨ������
    OMX_S32 m_pipe_in;                              //read this pipe
    OMX_S32 m_pipe_out;                             //write this pipe

    sem_t m_async_sem;                          //�첽�ź��� �����ڶ�ȡͨ��ʱ���ź�������
    sem_t m_cmd_lock;                           //�����ź��� (�൱����) ??
    #ifdef ENABLE_BUFFER_LOCK
    sem_t m_buf_lock;
    #endif
    omx_event_queue m_ftb_q;                    //������ݶ���           /*ע��:ÿ��д�����֮��Զ���Թܵ�������Ӧ��д�����¼������߳��жԹܵ����ж�������������Ϣ�������������*/
    omx_event_queue m_cmd_q;                    //�������
    omx_event_queue m_etb_q;                    //����Դ����
    void  *bufferaddr_RGB;     //�����ַ
    HI_U32 bufferaddr_Phy_RGB;
    HI_U32 buffer_size_RGB;    //buffer alloc size

    HI_HANDLE hTDE;
    //allocator_handle_t allocator_handle;        //�����ڴ棬�õ�IOH�豸ʱ�����ڼ�¼ION���~
} OMX_COMPONENT_PRIVATE;

/*==========================================================================*/
// bit operation functions
static inline void bit_set(OMX_U32* addr, OMX_U32 nr)                          // �����ݵ�ĳһλ����Ϊ 1 (�ú�����64λͬ����Ч)
{
    addr[nr / BITS_PER_LONG] |= (1 << (nr % BITS_PER_LONG));
}

static inline void bit_clear(OMX_U32* addr, OMX_U32 nr)                         // �����ݵ�ĳһλ����Ϊ��0 (�ú�����64λͬ����Ч)
{
    addr[nr / BITS_PER_LONG] &= ~((OMX_U32)(1 << (nr % BITS_PER_LONG)));
}

static inline OMX_S32 bit_present(const OMX_U32* addr, OMX_U32 nr)         // �ж�����ĳһλ�Ƿ�Ϊ 1;���Ϊ1,�򷵻�1�����򷵻�0 (�ú�����64λͬ����Ч)
{
    return ((1 << (nr % BITS_PER_LONG)) &
            (((OMX_U32*)addr)[nr / BITS_PER_LONG])) != 0;
}

static inline OMX_S32 bit_absent(const OMX_U32* addr, OMX_U32 nr)          // �ж�����ĳһλ�Ƿ�Ϊ 0�����Ϊ0,�򷵻�1�����򷵻�0
{
    return !bit_present(addr, nr);
}

#endif // __OMX_VENC_H__
