����������չʾHDMI���뼰��HDMI IN->HDMI OUT�ػ�����
�����ʽ��sample_hdmi_rx [vo_format]
����˵����vo_format ��Ƶ�����ʽ ȡֵ��Χ 1080P_60|1080P_50|1080i_60|1080i_50|720P_60 ��ѡ Ĭ��ֵ720P_50
��    �ӣ�sample_hdmi_rx 720P_50
ע�����
HI3798ϵ��IC
1.sample����ǰ��Ҫ����hi_hdmirx.ko��
HI3716CV200ϵ��IC��
1.�ֶ�����SIL9293.ko��insmod pub/kmod/hi_sil9293.ko
2.ȷ��HDMI�����ź���SIL9293оƬ����ȷ����
3.audio�޸ģ�
	source\msp\drv\aiao\aiao_v1_1\ao\drv_ao.c
      HI_S32 AOGetSndDefOpenAttr(HI_UNF_SND_ATTR_S *pstSndAttr)
      {
      ...
        #if 0   /*add*/                                       
      	#if defined(HI_UNF_SND_OUTPUTPORT_I2S0_SUPPORT)
      ...
      #endif                
      #endif   /*add*/                 
          return HI_SUCCESS;
      }
      
	sample\common\hi_adp_mpi.c 
			HI_S32 HIADP_AVPlay_SetAdecAttr(HI_HANDLE hAvplay, HI_U32 enADecType, HI_HA_DECODEMODE_E enMode, HI_S32 isCoreOnly)
      { 
      	...
       /* set pcm wav format here base on pcm file */
        stWavFormat.nChannels = 2;    /*1->2*/
        stWavFormat.nSamplesPerSec = 48000;
        stWavFormat.wBitsPerSample = 16;
        ...
       }
4.�μ�sample/vi/readme_cn.txtע�����