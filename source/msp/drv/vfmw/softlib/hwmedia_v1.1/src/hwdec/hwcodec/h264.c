
/*
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file libavcodec/h264.c
 * H.264 / AVC / MPEG4 part10 codec.
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#include "iMedia_error.h"


#include "dsputil.h"
#include "avcodec.h"
#include "mpegvideo.h"
#include "internal.h"
#include "h264.h"
#include "h264data.h"
#include "h264_parser.h"
#include "golomb.h"
#include "mathops.h"
#include "rectangle.h"
//#include "vdpau_internal.h"

#include "cabac.h"

#if ARCH_X86
#include "x86/h264_i386.h"
#endif

//#undef NDEBUG
#include <assert.h>

//#define LXW_DEBUG
#ifdef LXW_DEBUG
#define TARGET_POC -8196
#define MB_X 1
#define MB_Y 0
#define MB_Y2 MB_Y
#define TARGET_FRAME 27
int frame_num = 0;
int output_num = 0;
#endif
/**
 * Value of Picture.reference when Picture is not a reference picture, but
 * is held for delayed output.
 */
#define DELAYED_PIC_REF 4

/* 2010/05/4 19:00:00 liuxw+00139685 */
/* �����ӵĺ� */
#define MAX_PIC_HEIGHT 1088
#define MIN_PIC_HEIGHT 16
#define MAX_PIC_WIDTH  1920
#define MIN_PIC_WIDTH  32
#define MB_WIDTH 16
#define MB_HEIGHT 16

static VLC coeff_token_vlc[4];
static VLC_TYPE coeff_token_vlc_tables[520 + 332 + 280 + 256][2];
static const int coeff_token_vlc_tables_size[4] = {520, 332, 280, 256};

static VLC chroma_dc_coeff_token_vlc;
static VLC_TYPE chroma_dc_coeff_token_vlc_table[256][2];
static const int chroma_dc_coeff_token_vlc_table_size = 256;

static VLC total_zeros_vlc[15];
static VLC_TYPE total_zeros_vlc_tables[15][512][2];
static const int total_zeros_vlc_tables_size = 512;

static VLC chroma_dc_total_zeros_vlc[3];
static VLC_TYPE chroma_dc_total_zeros_vlc_tables[3][8][2];
static const int chroma_dc_total_zeros_vlc_tables_size = 8;

static VLC run_vlc[6];
static VLC_TYPE run_vlc_tables[6][8][2];
static const int run_vlc_tables_size = 8;

static VLC run7_vlc;
static VLC_TYPE run7_vlc_table[96][2];
static const int run7_vlc_table_size = 96;

static void svq3_luma_dc_dequant_idct_c(DCTELEM *block, int qp);
static void svq3_add_idct_c(uint8_t *dst, DCTELEM *block, int stride, int qp, int dc);
static void filter_mb( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize);
static void filter_mb_fast( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize);
static Picture *remove_long(H264Context *h, int i, int ref_mask);

static av_always_inline uint32_t pack16to32(int a, int b)
{
#ifdef WORDS_BIGENDIAN
    return (b & 0xFFFF) + (a << 16);
#else
    return (a & 0xFFFF) + (b << 16);
#endif
}

static const uint8_t rem6[52] =
{
    0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3,
};

static const uint8_t div6[52] =
{
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
};

static const uint8_t left_block_options[4][8] =
{
    {0, 1, 2, 3, 7, 10, 8, 11},
    {2, 2, 3, 3, 8, 11, 8, 11},
    {0, 0, 1, 1, 7, 10, 7, 10},
    {0, 2, 0, 2, 7, 10, 7, 10}
};

#define LEVEL_TAB_BITS 8
static int8_t cavlc_level_tab[7][1 << LEVEL_TAB_BITS][2];

static void fill_caches(H264Context *h, int mb_type, int for_deblock)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = h->mb_xy;
    int topleft_xy, top_xy, topright_xy, left_xy[2];
    int topleft_type, top_type, topright_type, left_type[2];
    const uint8_t *left_block;
    int topleft_partition = -1;
    int i;

    top_xy     = mb_xy  - (s->mb_stride << FIELD_PICTURE);

    //FIXME deblocking could skip the intra and nnz parts.
    if (for_deblock && (h->slice_num == 1 || h->slice_table[mb_xy] == h->slice_table[top_xy]) && !FRAME_MBAFF)
    {
        /* 2010/08/07 19:30:00 liuxw+00139685 */
        /* ����ɴ��˳��Ļ�����ô���������top_mb_xy */
        h->top_mb_xy = top_xy;
        return;
    }

    /* Wow, what a mess, why didn't they simplify the interlacing & intra
     * stuff, I can't imagine that these complex rules are worth it. */

    topleft_xy = top_xy - 1;
    topright_xy = top_xy + 1;
    left_xy[1] = left_xy[0] = mb_xy - 1;
    left_block = left_block_options[0];

    if (FRAME_MBAFF)
    {
        const int pair_xy          = s->mb_x     + (s->mb_y & ~1) * s->mb_stride;
        const int top_pair_xy      = pair_xy     - s->mb_stride;
        const int topleft_pair_xy  = top_pair_xy - 1;
        const int topright_pair_xy = top_pair_xy + 1;
        const int topleft_mb_field_flag  = IS_INTERLACED(s->current_picture.mb_type[topleft_pair_xy]);
        const int top_mb_field_flag      = IS_INTERLACED(s->current_picture.mb_type[top_pair_xy]);
        const int topright_mb_field_flag = IS_INTERLACED(s->current_picture.mb_type[topright_pair_xy]);
        const int left_mb_field_flag     = IS_INTERLACED(s->current_picture.mb_type[pair_xy - 1]);
        const int curr_mb_field_flag     = IS_INTERLACED(mb_type);
        const int bottom = (s->mb_y & 1);
        tprintf(s->avctx, "fill_caches: curr_mb_field_flag:%d, left_mb_field_flag:%d, topleft_mb_field_flag:%d, top_mb_field_flag:%d, topright_mb_field_flag:%d\n", curr_mb_field_flag, left_mb_field_flag, topleft_mb_field_flag, top_mb_field_flag, topright_mb_field_flag);

        if (curr_mb_field_flag && (bottom || top_mb_field_flag))
        {
            top_xy -= s->mb_stride;
        }

        if (curr_mb_field_flag && (bottom || topleft_mb_field_flag))
        {
            topleft_xy -= s->mb_stride;
        }
        else if (bottom && !curr_mb_field_flag && left_mb_field_flag)
        {
            topleft_xy += s->mb_stride;
            // take top left mv from the middle of the mb, as opposed to all other modes which use the bottom right partition
            topleft_partition = 0;
        }

        if (curr_mb_field_flag && (bottom || topright_mb_field_flag))
        {
            topright_xy -= s->mb_stride;
        }

        if (left_mb_field_flag != curr_mb_field_flag)
        {
            left_xy[1] = left_xy[0] = pair_xy - 1;

            if (curr_mb_field_flag)
            {
                left_xy[1] += s->mb_stride;
                left_block = left_block_options[3];
            }
            else
            {
                left_block = left_block_options[2 - bottom];
            }
        }
    }

    h->top_mb_xy = top_xy;
    h->left_mb_xy[0] = left_xy[0];
    h->left_mb_xy[1] = left_xy[1];

    if (for_deblock)
    {
        topleft_type = 0;
        topright_type = 0;
        top_type     = h->slice_table[top_xy     ] < 0xFFFF ? s->current_picture.mb_type[top_xy]     : 0;
        left_type[0] = h->slice_table[left_xy[0] ] < 0xFFFF ? s->current_picture.mb_type[left_xy[0]] : 0;
        left_type[1] = h->slice_table[left_xy[1] ] < 0xFFFF ? s->current_picture.mb_type[left_xy[1]] : 0;

        if (MB_MBAFF && !IS_INTRA(mb_type))
        {
            int list;

            for (list = 0; list < (int)h->list_count; list++)
            {
                //These values where changed for ease of performing MC, we need to change them back
                //FIXME maybe we can make MC and loop filter use the same values or prevent
                //the MC code from changing ref_cache and rather use a temporary array.
                if (USES_LIST(mb_type, list))
                {
                    int8_t *ref = &s->current_picture.ref_index[list][h->mb2b8_xy[mb_xy]];
                    *(uint32_t *)&h->ref_cache[list][scan8[ 0]] =
                        *(uint32_t *)&h->ref_cache[list][scan8[ 2]] = (pack16to32(ref[0], ref[1]) & 0x00FF00FF) * 0x0101;
                    ref += h->b8_stride;
                    *(uint32_t *)&h->ref_cache[list][scan8[ 8]] =
                        *(uint32_t *)&h->ref_cache[list][scan8[10]] = (pack16to32(ref[0], ref[1]) & 0x00FF00FF) * 0x0101;
                }
            }
        }
    }
    else
    {
        topleft_type = h->slice_table[topleft_xy ] == h->slice_num ? s->current_picture.mb_type[topleft_xy] : 0;
        top_type     = h->slice_table[top_xy     ] == h->slice_num ? s->current_picture.mb_type[top_xy]     : 0;
        topright_type = h->slice_table[topright_xy] == h->slice_num ? s->current_picture.mb_type[topright_xy] : 0;
        left_type[0] = h->slice_table[left_xy[0] ] == h->slice_num ? s->current_picture.mb_type[left_xy[0]] : 0;
        left_type[1] = h->slice_table[left_xy[1] ] == h->slice_num ? s->current_picture.mb_type[left_xy[1]] : 0;

        if (IS_INTRA(mb_type))
        {
            int type_mask = h->pps.constrained_intra_pred ? IS_INTRA(-1) : -1;
            h->topleft_samples_available =
                h->top_samples_available =
                    h->left_samples_available = 0xFFFF;
            h->topright_samples_available = 0xEEEA;

            if (!(top_type & type_mask))
            {
                h->topleft_samples_available = 0xB3FF;
                h->top_samples_available = 0x33FF;
                h->topright_samples_available = 0x26EA;
            }

            if (IS_INTERLACED(mb_type) != IS_INTERLACED(left_type[0]))
            {
                if (IS_INTERLACED(mb_type))
                {
                    if (!(left_type[0] & type_mask))
                    {
                        h->topleft_samples_available &= 0xDFFF;
                        h->left_samples_available &= 0x5FFF;
                    }

                    if (!(left_type[1] & type_mask))
                    {
                        h->topleft_samples_available &= 0xFF5F;
                        h->left_samples_available &= 0xFF5F;
                    }
                }
                else
                {
                    int left_typei = h->slice_table[left_xy[0] + s->mb_stride ] == h->slice_num
                                     ? s->current_picture.mb_type[left_xy[0] + s->mb_stride] : 0;
                    assert(left_xy[0] == left_xy[1]);

                    if (!((left_typei & type_mask) && (left_type[0] & type_mask)))
                    {
                        h->topleft_samples_available &= 0xDF5F;
                        h->left_samples_available &= 0x5F5F;
                    }
                }
            }
            else
            {
                if (!(left_type[0] & type_mask))
                {
                    h->topleft_samples_available &= 0xDF5F;
                    h->left_samples_available &= 0x5F5F;
                }
            }

            if (!(topleft_type & type_mask))
            { h->topleft_samples_available &= 0x7FFF; }

            if (!(topright_type & type_mask))
            { h->topright_samples_available &= 0xFBFF; }

            if (IS_INTRA4x4(mb_type))
            {
                if (IS_INTRA4x4(top_type))
                {
                    h->intra4x4_pred_mode_cache[4 + 8 * 0] = h->intra4x4_pred_mode[top_xy][4];
                    h->intra4x4_pred_mode_cache[5 + 8 * 0] = h->intra4x4_pred_mode[top_xy][5];
                    h->intra4x4_pred_mode_cache[6 + 8 * 0] = h->intra4x4_pred_mode[top_xy][6];
                    h->intra4x4_pred_mode_cache[7 + 8 * 0] = h->intra4x4_pred_mode[top_xy][3];
                }
                else
                {
                    int pred;

                    if (!(top_type & type_mask))
                    { pred = -1; }
                    else
                    {
                        pred = 2;
                    }

                    h->intra4x4_pred_mode_cache[4 + 8 * 0] =
                        h->intra4x4_pred_mode_cache[5 + 8 * 0] =
                            h->intra4x4_pred_mode_cache[6 + 8 * 0] =
                                h->intra4x4_pred_mode_cache[7 + 8 * 0] = pred;
                }

                for (i = 0; i < 2; i++)
                {
                    if (IS_INTRA4x4(left_type[i]))
                    {
                        h->intra4x4_pred_mode_cache[3 + 8 * 1 + 2 * 8 * i] = h->intra4x4_pred_mode[left_xy[i]][left_block[0 + 2 * i]];
                        h->intra4x4_pred_mode_cache[3 + 8 * 2 + 2 * 8 * i] = h->intra4x4_pred_mode[left_xy[i]][left_block[1 + 2 * i]];
                    }
                    else
                    {
                        int pred;

                        if (!(left_type[i] & type_mask))
                        { pred = -1; }
                        else
                        {
                            pred = 2;
                        }

                        h->intra4x4_pred_mode_cache[3 + 8 * 1 + 2 * 8 * i] =
                            h->intra4x4_pred_mode_cache[3 + 8 * 2 + 2 * 8 * i] = pred;
                    }
                }
            }
        }
    }


    /*
    0 . T T. T T T T
    1 L . .L . . . .
    2 L . .L . . . .
    3 . T TL . . . .
    4 L . .L . . . .
    5 L . .. . . . .
    */
    //FIXME constraint_intra_pred & partitioning & nnz (let us hope this is just a typo in the spec)
    if (top_type)
    {
        h->non_zero_count_cache[4 + 8 * 0] = h->non_zero_count[top_xy][4];
        h->non_zero_count_cache[5 + 8 * 0] = h->non_zero_count[top_xy][5];
        h->non_zero_count_cache[6 + 8 * 0] = h->non_zero_count[top_xy][6];
        h->non_zero_count_cache[7 + 8 * 0] = h->non_zero_count[top_xy][3];

        h->non_zero_count_cache[1 + 8 * 0] = h->non_zero_count[top_xy][9];
        h->non_zero_count_cache[2 + 8 * 0] = h->non_zero_count[top_xy][8];

        h->non_zero_count_cache[1 + 8 * 3] = h->non_zero_count[top_xy][12];
        h->non_zero_count_cache[2 + 8 * 3] = h->non_zero_count[top_xy][11];

    }
    else
    {
        h->non_zero_count_cache[4 + 8 * 0] =
            h->non_zero_count_cache[5 + 8 * 0] =
                h->non_zero_count_cache[6 + 8 * 0] =
                    h->non_zero_count_cache[7 + 8 * 0] =

                        h->non_zero_count_cache[1 + 8 * 0] =
                            h->non_zero_count_cache[2 + 8 * 0] =

                                h->non_zero_count_cache[1 + 8 * 3] =
                                    h->non_zero_count_cache[2 + 8 * 3] = h->pps.cabac && !IS_INTRA(mb_type) ? 0 : 64;

    }

    for (i = 0; i < 2; i++)
    {
        if (left_type[i])
        {
            h->non_zero_count_cache[3 + 8 * 1 + 2 * 8 * i] = h->non_zero_count[left_xy[i]][left_block[0 + 2 * i]];
            h->non_zero_count_cache[3 + 8 * 2 + 2 * 8 * i] = h->non_zero_count[left_xy[i]][left_block[1 + 2 * i]];
            h->non_zero_count_cache[0 + 8 * 1 +   8 * i] = h->non_zero_count[left_xy[i]][left_block[4 + 2 * i]];
            h->non_zero_count_cache[0 + 8 * 4 +   8 * i] = h->non_zero_count[left_xy[i]][left_block[5 + 2 * i]];
        }
        else
        {
            h->non_zero_count_cache[3 + 8 * 1 + 2 * 8 * i] =
                h->non_zero_count_cache[3 + 8 * 2 + 2 * 8 * i] =
                    h->non_zero_count_cache[0 + 8 * 1 +   8 * i] =
                        h->non_zero_count_cache[0 + 8 * 4 +   8 * i] = h->pps.cabac && !IS_INTRA(mb_type) ? 0 : 64;
        }
    }

    if ( h->pps.cabac )
    {
        // top_cbp
        if (top_type)
        {
            h->top_cbp = h->cbp_table[top_xy];
        }
        else if (IS_INTRA(mb_type))
        {
            h->top_cbp = 0x1C0;
        }
        else
        {
            h->top_cbp = 0;
        }

        // left_cbp
        if (left_type[0])
        {
            h->left_cbp = h->cbp_table[left_xy[0]] & 0x1f0;
        }
        else if (IS_INTRA(mb_type))
        {
            h->left_cbp = 0x1C0;
        }
        else
        {
            h->left_cbp = 0;
        }

        if (left_type[0])
        {
            h->left_cbp |= ((h->cbp_table[left_xy[0]] >> ((left_block[0] & (~1)) + 1)) & 0x1) << 1;
        }

        if (left_type[1])
        {
            h->left_cbp |= ((h->cbp_table[left_xy[1]] >> ((left_block[2] & (~1)) + 1)) & 0x1) << 3;
        }
    }

#if 1

    if (IS_INTER(mb_type) || IS_DIRECT(mb_type))
    {
        int list;

        /* 2010/08/23 11:30:00 liuxw+00139685 */
        /* ��h->list_countֱ���滻Ϊ2,��ĳ������0/1������ʱ��������mv��0��ref��-1 */
        //      for(list=0; list<(int)h->list_count; list++)
        for (list = 0; list < 2; list++)
        {
            if (!USES_LIST(mb_type, list) && !IS_DIRECT(mb_type) && !h->deblocking_filter)
            {
                /*if(!h->mv_cache_clean[list]){
                    memset(h->mv_cache [list],  0, 8*5*2*sizeof(int16_t)); //FIXME clean only input? clean at all?
                    memset(h->ref_cache[list], PART_NOT_AVAILABLE, 8*5*sizeof(int8_t));
                    h->mv_cache_clean[list]= 1;
                }*/
                continue;
            }

            h->mv_cache_clean[list] = 0;

            /* 2010/08/23 11:30:00 liuxw+00139685 */
            /* �������룺����ǰ����list0/1������ʱ��������mv��0��ref��-1 */
            if (!USES_LIST(mb_type, list))
            {
                memset(h->mv_cache [list],  0, 8 * 5 * 2 * sizeof(int16_t));
                memset(h->ref_cache[list], LIST_NOT_USED, 8 * 5 * sizeof(int8_t));
            }

            if (USES_LIST(top_type, list))
            {
                const int b_xy = h->mb2b_xy[top_xy] + 3 * h->b_stride;
                const int b8_xy = h->mb2b8_xy[top_xy] + h->b8_stride;
                *(uint32_t *)h->mv_cache[list][scan8[0] + 0 - 1 * 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy + 0];
                *(uint32_t *)h->mv_cache[list][scan8[0] + 1 - 1 * 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy + 1];
                *(uint32_t *)h->mv_cache[list][scan8[0] + 2 - 1 * 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy + 2];
                *(uint32_t *)h->mv_cache[list][scan8[0] + 3 - 1 * 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy + 3];
                h->ref_cache[list][scan8[0] + 0 - 1 * 8] =
                    h->ref_cache[list][scan8[0] + 1 - 1 * 8] = s->current_picture.ref_index[list][b8_xy + 0];
                h->ref_cache[list][scan8[0] + 2 - 1 * 8] =
                    h->ref_cache[list][scan8[0] + 3 - 1 * 8] = s->current_picture.ref_index[list][b8_xy + 1];
            }
            else
            {
                *(uint32_t *)h->mv_cache [list][scan8[0] + 0 - 1 * 8] =
                    *(uint32_t *)h->mv_cache [list][scan8[0] + 1 - 1 * 8] =
                        *(uint32_t *)h->mv_cache [list][scan8[0] + 2 - 1 * 8] =
                            *(uint32_t *)h->mv_cache [list][scan8[0] + 3 - 1 * 8] = 0;
                *(uint32_t *)&h->ref_cache[list][scan8[0] + 0 - 1 * 8] = ((top_type ? LIST_NOT_USED : PART_NOT_AVAILABLE) & 0xFF) * 0x01010101;
            }

            for (i = 0; i < 2; i++)
            {
                int cache_idx = scan8[0] - 1 + i * 2 * 8;

                if (USES_LIST(left_type[i], list))
                {
                    const int b_xy = h->mb2b_xy[left_xy[i]] + 3;
                    const int b8_xy = h->mb2b8_xy[left_xy[i]] + 1;
                    *(uint32_t *)h->mv_cache[list][cache_idx  ] = *(uint32_t *)s->current_picture.motion_val[list][b_xy + h->b_stride * left_block[0 + i * 2]];
                    *(uint32_t *)h->mv_cache[list][cache_idx + 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy + h->b_stride * left_block[1 + i * 2]];
                    h->ref_cache[list][cache_idx  ] = s->current_picture.ref_index[list][b8_xy + h->b8_stride * (left_block[0 + i * 2] >> 1)];
                    h->ref_cache[list][cache_idx + 8] = s->current_picture.ref_index[list][b8_xy + h->b8_stride * (left_block[1 + i * 2] >> 1)];
                }
                else
                {
                    *(uint32_t *)h->mv_cache [list][cache_idx  ] =
                        *(uint32_t *)h->mv_cache [list][cache_idx + 8] = 0;
                    h->ref_cache[list][cache_idx  ] =
                        h->ref_cache[list][cache_idx + 8] = left_type[i] ? LIST_NOT_USED : PART_NOT_AVAILABLE;
                }
            }

            if (for_deblock || ((IS_DIRECT(mb_type) && !h->direct_spatial_mv_pred) && !FRAME_MBAFF))
            { continue; }

            if (USES_LIST(topleft_type, list))
            {
                const int b_xy = h->mb2b_xy[topleft_xy] + 3 + h->b_stride + (topleft_partition & 2 * h->b_stride);
                const int b8_xy = h->mb2b8_xy[topleft_xy] + 1 + (topleft_partition & h->b8_stride);
                *(uint32_t *)h->mv_cache[list][scan8[0] - 1 - 1 * 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy];
                h->ref_cache[list][scan8[0] - 1 - 1 * 8] = s->current_picture.ref_index[list][b8_xy];
            }
            else
            {
                *(uint32_t *)h->mv_cache[list][scan8[0] - 1 - 1 * 8] = 0;
                h->ref_cache[list][scan8[0] - 1 - 1 * 8] = topleft_type ? LIST_NOT_USED : PART_NOT_AVAILABLE;
            }

            if (USES_LIST(topright_type, list))
            {
                const int b_xy = h->mb2b_xy[topright_xy] + 3 * h->b_stride;
                const int b8_xy = h->mb2b8_xy[topright_xy] + h->b8_stride;
                *(uint32_t *)h->mv_cache[list][scan8[0] + 4 - 1 * 8] = *(uint32_t *)s->current_picture.motion_val[list][b_xy];
                h->ref_cache[list][scan8[0] + 4 - 1 * 8] = s->current_picture.ref_index[list][b8_xy];
            }
            else
            {
                *(uint32_t *)h->mv_cache [list][scan8[0] + 4 - 1 * 8] = 0;
                h->ref_cache[list][scan8[0] + 4 - 1 * 8] = topright_type ? LIST_NOT_USED : PART_NOT_AVAILABLE;
            }

            if ((IS_SKIP(mb_type) || IS_DIRECT(mb_type)) && !FRAME_MBAFF)
            { continue; }

            h->ref_cache[list][scan8[5 ] + 1] =
                h->ref_cache[list][scan8[7 ] + 1] =
                    h->ref_cache[list][scan8[13] + 1] = //FIXME remove past 3 (init somewhere else)
                        h->ref_cache[list][scan8[4 ]] =
                            h->ref_cache[list][scan8[12]] = PART_NOT_AVAILABLE;
            *(uint32_t *)h->mv_cache [list][scan8[5 ] + 1] =
                *(uint32_t *)h->mv_cache [list][scan8[7 ] + 1] =
                    *(uint32_t *)h->mv_cache [list][scan8[13] + 1] = //FIXME remove past 3 (init somewhere else)
                        *(uint32_t *)h->mv_cache [list][scan8[4 ]] =
                            *(uint32_t *)h->mv_cache [list][scan8[12]] = 0;

            if ( h->pps.cabac )
            {
                /* XXX beurk, Load mvd */
                if (USES_LIST(top_type, list))
                {
                    const int b_xy = h->mb2b_xy[top_xy] + 3 * h->b_stride;
                    *(uint32_t *)h->mvd_cache[list][scan8[0] + 0 - 1 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + 0];
                    *(uint32_t *)h->mvd_cache[list][scan8[0] + 1 - 1 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + 1];
                    *(uint32_t *)h->mvd_cache[list][scan8[0] + 2 - 1 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + 2];
                    *(uint32_t *)h->mvd_cache[list][scan8[0] + 3 - 1 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + 3];
                }
                else
                {
                    *(uint32_t *)h->mvd_cache [list][scan8[0] + 0 - 1 * 8] =
                        *(uint32_t *)h->mvd_cache [list][scan8[0] + 1 - 1 * 8] =
                            *(uint32_t *)h->mvd_cache [list][scan8[0] + 2 - 1 * 8] =
                                *(uint32_t *)h->mvd_cache [list][scan8[0] + 3 - 1 * 8] = 0;
                }

                if (USES_LIST(left_type[0], list))
                {
                    const int b_xy = h->mb2b_xy[left_xy[0]] + 3;
                    *(uint32_t *)h->mvd_cache[list][scan8[0] - 1 + 0 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + h->b_stride * left_block[0]];
                    *(uint32_t *)h->mvd_cache[list][scan8[0] - 1 + 1 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + h->b_stride * left_block[1]];
                }
                else
                {
                    *(uint32_t *)h->mvd_cache [list][scan8[0] - 1 + 0 * 8] =
                        *(uint32_t *)h->mvd_cache [list][scan8[0] - 1 + 1 * 8] = 0;
                }

                if (USES_LIST(left_type[1], list))
                {
                    const int b_xy = h->mb2b_xy[left_xy[1]] + 3;
                    *(uint32_t *)h->mvd_cache[list][scan8[0] - 1 + 2 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + h->b_stride * left_block[2]];
                    *(uint32_t *)h->mvd_cache[list][scan8[0] - 1 + 3 * 8] = *(uint32_t *)h->mvd_table[list][b_xy + h->b_stride * left_block[3]];
                }
                else
                {
                    *(uint32_t *)h->mvd_cache [list][scan8[0] - 1 + 2 * 8] =
                        *(uint32_t *)h->mvd_cache [list][scan8[0] - 1 + 3 * 8] = 0;
                }

                *(uint32_t *)h->mvd_cache [list][scan8[5 ] + 1] =
                    *(uint32_t *)h->mvd_cache [list][scan8[7 ] + 1] =
                        *(uint32_t *)h->mvd_cache [list][scan8[13] + 1] = //FIXME remove past 3 (init somewhere else)
                            *(uint32_t *)h->mvd_cache [list][scan8[4 ]] =
                                *(uint32_t *)h->mvd_cache [list][scan8[12]] = 0;

                if (h->slice_type_nos == FF_B_TYPE)
                {
                    fill_rectangle(&h->direct_cache[scan8[0]], 4, 4, 8, 0, 1);

                    if (IS_DIRECT(top_type))
                    {
                        *(uint32_t *)&h->direct_cache[scan8[0] - 1 * 8] = 0x01010101;
                    }
                    else if (IS_8X8(top_type))
                    {
                        int b8_xy = h->mb2b8_xy[top_xy] + h->b8_stride;
                        h->direct_cache[scan8[0] + 0 - 1 * 8] = h->direct_table[b8_xy];
                        h->direct_cache[scan8[0] + 2 - 1 * 8] = h->direct_table[b8_xy + 1];
                    }
                    else
                    {
                        *(uint32_t *)&h->direct_cache[scan8[0] - 1 * 8] = 0;
                    }

                    if (IS_DIRECT(left_type[0]))
                    { h->direct_cache[scan8[0] - 1 + 0 * 8] = 1; }
                    else if (IS_8X8(left_type[0]))
                    { h->direct_cache[scan8[0] - 1 + 0 * 8] = h->direct_table[h->mb2b8_xy[left_xy[0]] + 1 + h->b8_stride * (left_block[0] >> 1)]; }
                    else
                    { h->direct_cache[scan8[0] - 1 + 0 * 8] = 0; }

                    if (IS_DIRECT(left_type[1]))
                    { h->direct_cache[scan8[0] - 1 + 2 * 8] = 1; }
                    else if (IS_8X8(left_type[1]))
                    { h->direct_cache[scan8[0] - 1 + 2 * 8] = h->direct_table[h->mb2b8_xy[left_xy[1]] + 1 + h->b8_stride * (left_block[2] >> 1)]; }
                    else
                    { h->direct_cache[scan8[0] - 1 + 2 * 8] = 0; }
                }
            }

            if (FRAME_MBAFF)
            {
#define MAP_MVS\
    MAP_F2F(scan8[0] - 1 - 1*8, topleft_type)\
    MAP_F2F(scan8[0] + 0 - 1*8, top_type)\
    MAP_F2F(scan8[0] + 1 - 1*8, top_type)\
    MAP_F2F(scan8[0] + 2 - 1*8, top_type)\
    MAP_F2F(scan8[0] + 3 - 1*8, top_type)\
    MAP_F2F(scan8[0] + 4 - 1*8, topright_type)\
    MAP_F2F(scan8[0] - 1 + 0*8, left_type[0])\
    MAP_F2F(scan8[0] - 1 + 1*8, left_type[0])\
    MAP_F2F(scan8[0] - 1 + 2*8, left_type[1])\
    MAP_F2F(scan8[0] - 1 + 3*8, left_type[1])

                if (MB_FIELD)
                {
#define MAP_F2F(idx, mb_type)\
    if(!IS_INTERLACED(mb_type) && h->ref_cache[list][idx] >= 0){\
        h->ref_cache[list][idx] <<= 1;\
        h->mv_cache[list][idx][1] /= 2;\
        h->mvd_cache[list][idx][1] /= 2;\
    }
                    MAP_MVS
#undef MAP_F2F
                }
                else
                {
#define MAP_F2F(idx, mb_type)\
    if(IS_INTERLACED(mb_type) && h->ref_cache[list][idx] >= 0){\
        h->ref_cache[list][idx] >>= 1;\
        h->mv_cache[list][idx][1] <<= 1;\
        h->mvd_cache[list][idx][1] <<= 1;\
    }
                    MAP_MVS
#undef MAP_F2F
                }
            }
        }
    }

#endif

    h->neighbor_transform_size = !!IS_8x8DCT(top_type) + !!IS_8x8DCT(left_type[0]);
}

static inline void write_back_intra_pred_mode(H264Context *h)
{
    const int mb_xy = h->mb_xy;

    h->intra4x4_pred_mode[mb_xy][0] = h->intra4x4_pred_mode_cache[7 + 8 * 1];
    h->intra4x4_pred_mode[mb_xy][1] = h->intra4x4_pred_mode_cache[7 + 8 * 2];
    h->intra4x4_pred_mode[mb_xy][2] = h->intra4x4_pred_mode_cache[7 + 8 * 3];
    h->intra4x4_pred_mode[mb_xy][3] = h->intra4x4_pred_mode_cache[7 + 8 * 4];
    h->intra4x4_pred_mode[mb_xy][4] = h->intra4x4_pred_mode_cache[4 + 8 * 4];
    h->intra4x4_pred_mode[mb_xy][5] = h->intra4x4_pred_mode_cache[5 + 8 * 4];
    h->intra4x4_pred_mode[mb_xy][6] = h->intra4x4_pred_mode_cache[6 + 8 * 4];
}

/**
 * checks if the top & left blocks are available if needed & changes the dc mode so it only uses the available blocks.
 */
static inline int check_intra4x4_pred_mode(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    static const int8_t top [12] = { -1, 0, LEFT_DC_PRED, -1, -1, -1, -1, -1, 0};
    static const int8_t left[12] = { 0, -1, TOP_DC_PRED, 0, -1, -1, -1, 0, -1, DC_128_PRED};
    int i;

    if (!(h->top_samples_available & 0x8000))
    {
        for (i = 0; i < 4; i++)
        {
            int status = top[ h->intra4x4_pred_mode_cache[scan8[0] + i] ];

            if (status < 0)
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "top block unavailable for requested intra4x4 mode %d at %d %d\n", status, s->mb_x, s->mb_y);
                av_log(h->s.avctx, AV_LOG_WARNING, "top block unavailable for requested intra4x4 mode %d at %d %d\n", status, s->mb_x, s->mb_y);
                return -1;
            }
            else if (status)
            {
                h->intra4x4_pred_mode_cache[scan8[0] + i] = status;
            }
        }
    }

    if ((h->left_samples_available & 0x8888) != 0x8888)
    {
        static const int mask[4] = {0x8000, 0x2000, 0x80, 0x20};

        for (i = 0; i < 4; i++)
        {
            if (!(h->left_samples_available & mask[i]))
            {
                int status = left[ h->intra4x4_pred_mode_cache[scan8[0] + 8 * i] ];

                if (status < 0)
                {
                    //av_log(h->s.avctx, AV_LOG_ERROR, "left block unavailable for requested intra4x4 mode %d at %d %d\n", status, s->mb_x, s->mb_y);
                    av_log(h->s.avctx, AV_LOG_WARNING, "left block unavailable for requested intra4x4 mode %d at %d %d\n", status, s->mb_x, s->mb_y);
                    return -1;
                }
                else if (status)
                {
                    h->intra4x4_pred_mode_cache[scan8[0] + 8 * i] = status;
                }
            }
        }
    }

    return 0;
} //FIXME cleanup like next

/**
 * checks if the top & left blocks are available if needed & changes the dc mode so it only uses the available blocks.
 */
static inline int check_intra_pred_mode(H264Context *h, int mode)
{
    MpegEncContext *const s = &h->s;
    static const int8_t top [7] = {LEFT_DC_PRED8x8, 1, -1, -1};
    static const int8_t left[7] = { TOP_DC_PRED8x8, -1, 2, -1, DC_128_PRED8x8};

    /* 2010/08/16 14:00:00 liuxw+00139685 */
    /* ������ʱ��������intra chromaԤ��ģʽ */
    int tmp_mode = mode;

    if (mode > 6U)
    {
        //        av_log(h->s.avctx, AV_LOG_ERROR, "out of range intra chroma pred mode at %d %d\n", s->mb_x, s->mb_y);
        av_log(h->s.avctx, AV_LOG_WARNING, "out of range intra chroma pred mode at %d %d\n", s->mb_x, s->mb_y);
        return -1;
    }

    if (!(h->top_samples_available & 0x8000))
    {
        mode = top[ mode ];

        if (mode < 0)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "top block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "top block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
    }

    if ((h->left_samples_available & 0x8080) != 0x8080)
    {
        mode = left[ mode ];

        /* 2010/08/16 14:00:00 liuxw+00139685 */
        /* ��intra chromaԤ��ģʽ����DCԤ��ʱ���Ż�������һ�� */
        //		if(h->left_samples_available&0x8080){ //mad cow disease mode, aka MBAFF + constrained_intra_pred
        if (h->left_samples_available & 0x8080 && !tmp_mode) //mad cow disease mode, aka MBAFF + constrained_intra_pred
        {
            mode = ALZHEIMER_DC_L0T_PRED8x8 + (!(h->left_samples_available & 0x8000)) + 2 * (mode == DC_128_PRED8x8);
        }

        if (mode < 0)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "left block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "left block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
    }

    return mode;
}


/* 2010/08/13 11:30:00 liuxw+00139685 */
/* �����ӵĺ��������ڼ��luma intra���Ԥ��ģʽ�ĺϷ��� */
static inline int check_intra_pred_mode_luma(H264Context *h, int mode)
{
    MpegEncContext *const s = &h->s;
    static const int8_t top [7] = {LEFT_DC_PRED8x8, 1, -1, -1};
    static const int8_t left[7] = { TOP_DC_PRED8x8, -1, 2, -1, DC_128_PRED8x8};

    if (mode > 6U)
    {
        //        av_log(h->s.avctx, AV_LOG_ERROR, "out of range intra chroma pred mode at %d %d\n", s->mb_x, s->mb_y);
        av_log(h->s.avctx, AV_LOG_WARNING, "out of range intra chroma pred mode at %d %d\n", s->mb_x, s->mb_y);
        return -1;
    }

    if (!(h->top_samples_available & 0x8000))
    {
        mode = top[ mode ];

        if (mode < 0)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "top block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "top block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
    }

    if ((h->left_samples_available & 0x8080) != 0x8080)
    {
        mode = left[ mode ];

        //if(h->left_samples_available&0x8080){ //mad cow disease mode, aka MBAFF + constrained_intra_pred
        //    mode= ALZHEIMER_DC_L0T_PRED8x8 + (!(h->left_samples_available&0x8000)) + 2*(mode == DC_128_PRED8x8);
        //}
        if (mode < 0)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "left block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "left block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
    }

    return mode;
}
/**
 * gets the predicted intra4x4 prediction mode.
 */
static inline int pred_intra_mode(H264Context *h, int n)
{
    const int index8 = scan8[n];
    const int left = h->intra4x4_pred_mode_cache[index8 - 1];
    const int top = h->intra4x4_pred_mode_cache[index8 - 8];
    const int min = FFMIN(left, top);

    tprintf(h->s.avctx, "mode:%d %d min:%d\n", left , top, min);

    if (min < 0) { return DC_PRED; }
    else      { return min; }
}

static inline void write_back_non_zero_count(H264Context *h)
{
    const int mb_xy = h->mb_xy;

    h->non_zero_count[mb_xy][0] = h->non_zero_count_cache[7 + 8 * 1];
    h->non_zero_count[mb_xy][1] = h->non_zero_count_cache[7 + 8 * 2];
    h->non_zero_count[mb_xy][2] = h->non_zero_count_cache[7 + 8 * 3];
    h->non_zero_count[mb_xy][3] = h->non_zero_count_cache[7 + 8 * 4];
    h->non_zero_count[mb_xy][4] = h->non_zero_count_cache[4 + 8 * 4];
    h->non_zero_count[mb_xy][5] = h->non_zero_count_cache[5 + 8 * 4];
    h->non_zero_count[mb_xy][6] = h->non_zero_count_cache[6 + 8 * 4];

    h->non_zero_count[mb_xy][9] = h->non_zero_count_cache[1 + 8 * 2];
    h->non_zero_count[mb_xy][8] = h->non_zero_count_cache[2 + 8 * 2];
    h->non_zero_count[mb_xy][7] = h->non_zero_count_cache[2 + 8 * 1];

    h->non_zero_count[mb_xy][12] = h->non_zero_count_cache[1 + 8 * 5];
    h->non_zero_count[mb_xy][11] = h->non_zero_count_cache[2 + 8 * 5];
    h->non_zero_count[mb_xy][10] = h->non_zero_count_cache[2 + 8 * 4];

    /*hanqingrui add*/
    if (IS_8x8DCT(h->s.current_picture.mb_type[mb_xy]))
    {
        int i;

        for (i = 0; i < 4; i++)
        {
            int index = scan8[i * 4];
            int sum = h->non_zero_count_cache[index] + h->non_zero_count_cache[index + 1] +
                      h->non_zero_count_cache[index + 8] + h->non_zero_count_cache[index + 9];

            h->non_zero_count_8x8[mb_xy][i] = sum;
        }
    }
    else
    {
        memset(h->non_zero_count_8x8[mb_xy], 0, 16);
    }

    /*hanqingrui add end*/
}

/**
 * gets the predicted number of non-zero coefficients.
 * @param n block index
 */
static inline int pred_non_zero_count(H264Context *h, int n)
{
    const int index8 = scan8[n];
    const int left = h->non_zero_count_cache[index8 - 1];
    const int top = h->non_zero_count_cache[index8 - 8];
    int i = left + top;

    if (i < 64) { i = (i + 1) >> 1; }

    tprintf(h->s.avctx, "pred_nnz L%X T%X n%d s%d P%X\n", left, top, n, scan8[n], i & 31);

    return i & 31;
}

static inline int fetch_diagonal_mv(H264Context *h, const int16_t **C, int i, int list, int part_width)
{
    const int topright_ref = h->ref_cache[list][ i - 8 + part_width ];
    MpegEncContext *s = &h->s;

    /* there is no consistent mapping of mvs to neighboring locations that will
     * make mbaff happy, so we can't move all this logic to fill_caches */
    if (FRAME_MBAFF)
    {
        const uint32_t *mb_types = s->current_picture_ptr->mb_type;
        const int16_t *mv;
        *(uint32_t *)h->mv_cache[list][scan8[0] - 2] = 0;
        *C = h->mv_cache[list][scan8[0] - 2];

        if (!MB_FIELD
            && (s->mb_y & 1) && i < scan8[0] + 8 && topright_ref != PART_NOT_AVAILABLE)
        {
            int topright_xy = s->mb_x + (s->mb_y - 1) * s->mb_stride + (i == scan8[0] + 3);

            if (IS_INTERLACED(mb_types[topright_xy]))
            {
#define SET_DIAG_MV(MV_OP, REF_OP, X4, Y4)\
    const int x4 = X4, y4 = Y4;\
    const int mb_type = mb_types[(x4>>2)+(y4>>2)*s->mb_stride];\
    if(!USES_LIST(mb_type,list))\
        return LIST_NOT_USED;\
    mv = s->current_picture_ptr->motion_val[list][x4 + y4*h->b_stride];\
    h->mv_cache[list][scan8[0]-2][0] = mv[0];\
    h->mv_cache[list][scan8[0]-2][1] = mv[1] MV_OP;\
    return s->current_picture_ptr->ref_index[list][(x4>>1) + (y4>>1)*h->b8_stride] REF_OP;

                SET_DIAG_MV(*2, >> 1, (s->mb_x * 4 + (i & 7) - 4 + part_width), s->mb_y * 4 - 1);
            }
        }

        if (topright_ref == PART_NOT_AVAILABLE
            && ((s->mb_y & 1) || i >= scan8[0] + 8) && (i & 7) == 4
            && h->ref_cache[list][scan8[0] - 1] != PART_NOT_AVAILABLE)
        {
            if (!MB_FIELD
                && IS_INTERLACED(mb_types[h->left_mb_xy[0]]))
            {
                SET_DIAG_MV(*2, >> 1, s->mb_x * 4 - 1, (s->mb_y | 1) * 4 + (s->mb_y & 1) * 2 + (i >> 4) - 1);
            }

            if (MB_FIELD
                && !IS_INTERLACED(mb_types[h->left_mb_xy[0]])
                && i >= scan8[0] + 8)
            {
                // left shift will turn LIST_NOT_USED into PART_NOT_AVAILABLE, but that's OK.
                SET_DIAG_MV( / 2, << 1, s->mb_x * 4 - 1, (s->mb_y&~1) * 4 - 1 + ((i - scan8[0]) >> 3) * 2);
            }
        }

#undef SET_DIAG_MV
    }

    if (topright_ref != PART_NOT_AVAILABLE)
    {
        *C = h->mv_cache[list][ i - 8 + part_width ];
        return topright_ref;
    }
    else
    {
        tprintf(s->avctx, "topright MV not available\n");

        *C = h->mv_cache[list][ i - 8 - 1 ];
        return h->ref_cache[list][ i - 8 - 1 ];
    }
}

/**
 * gets the predicted MV.
 * @param n the block index
 * @param part_width the width of the partition (4, 8,16) -> (1, 2, 4)
 * @param mx the x component of the predicted motion vector
 * @param my the y component of the predicted motion vector
 */
static inline void pred_motion(H264Context *const h, int n, int part_width, int list, int ref, int *const mx, int *const my)
{
    const int index8 = scan8[n];
    const int top_ref =      h->ref_cache[list][ index8 - 8 ];
    const int left_ref =     h->ref_cache[list][ index8 - 1 ];
    const int16_t *const A = h->mv_cache[list][ index8 - 1 ];
    const int16_t *const B = h->mv_cache[list][ index8 - 8 ];
    const int16_t *C;
    int diagonal_ref, match_count;

    assert(part_width == 1 || part_width == 2 || part_width == 4);

    /* mv_cache
      B . . A T T T T
      U . . L . . , .
      U . . L . . . .
      U . . L . . , .
      . . . L . . . .
    */

    diagonal_ref = fetch_diagonal_mv(h, &C, index8, list, part_width);
    match_count = (diagonal_ref == ref) + (top_ref == ref) + (left_ref == ref);
    tprintf(h->s.avctx, "pred_motion match_count=%d\n", match_count);

    if (match_count > 1) //most common
    {
        *mx = mid_pred(A[0], B[0], C[0]);
        *my = mid_pred(A[1], B[1], C[1]);
    }
    else if (match_count == 1)
    {
        if (left_ref == ref)
        {
            *mx = A[0];
            *my = A[1];
        }
        else if (top_ref == ref)
        {
            *mx = B[0];
            *my = B[1];
        }
        else
        {
            *mx = C[0];
            *my = C[1];
        }
    }
    else
    {
        if (top_ref == PART_NOT_AVAILABLE && diagonal_ref == PART_NOT_AVAILABLE && left_ref != PART_NOT_AVAILABLE)
        {
            *mx = A[0];
            *my = A[1];
        }
        else
        {
            *mx = mid_pred(A[0], B[0], C[0]);
            *my = mid_pred(A[1], B[1], C[1]);
        }
    }

    tprintf(h->s.avctx, "pred_motion (%2d %2d %2d) (%2d %2d %2d) (%2d %2d %2d) -> (%2d %2d %2d) at %2d %2d %d list %d\n", top_ref, B[0], B[1],                    diagonal_ref, C[0], C[1], left_ref, A[0], A[1], ref, *mx, *my, h->s.mb_x, h->s.mb_y, n, list);
}

/**
 * gets the directionally predicted 16x8 MV.
 * @param n the block index
 * @param mx the x component of the predicted motion vector
 * @param my the y component of the predicted motion vector
 */
static inline void pred_16x8_motion(H264Context *const h, int n, int list, int ref, int *const mx, int *const my)
{
    if (n == 0)
    {
        const int top_ref =      h->ref_cache[list][ scan8[0] - 8 ];
        const int16_t *const B = h->mv_cache[list][ scan8[0] - 8 ];

        tprintf(h->s.avctx, "pred_16x8: (%2d %2d %2d) at %2d %2d %d list %d\n", top_ref, B[0], B[1], h->s.mb_x, h->s.mb_y, n, list);

        if (top_ref == ref)
        {
            *mx = B[0];
            *my = B[1];
            return;
        }
    }
    else
    {
        const int left_ref =     h->ref_cache[list][ scan8[8] - 1 ];
        const int16_t *const A = h->mv_cache[list][ scan8[8] - 1 ];

        tprintf(h->s.avctx, "pred_16x8: (%2d %2d %2d) at %2d %2d %d list %d\n", left_ref, A[0], A[1], h->s.mb_x, h->s.mb_y, n, list);

        if (left_ref == ref)
        {
            *mx = A[0];
            *my = A[1];
            return;
        }
    }

    //RARE
    pred_motion(h, n, 4, list, ref, mx, my);
}

/**
 * gets the directionally predicted 8x16 MV.
 * @param n the block index
 * @param mx the x component of the predicted motion vector
 * @param my the y component of the predicted motion vector
 */
static inline void pred_8x16_motion(H264Context *const h, int n, int list, int ref, int *const mx, int *const my)
{
    if (n == 0)
    {
        const int left_ref =      h->ref_cache[list][ scan8[0] - 1 ];
        const int16_t *const A =  h->mv_cache[list][ scan8[0] - 1 ];

        tprintf(h->s.avctx, "pred_8x16: (%2d %2d %2d) at %2d %2d %d list %d\n", left_ref, A[0], A[1], h->s.mb_x, h->s.mb_y, n, list);

        if (left_ref == ref)
        {
            *mx = A[0];
            *my = A[1];
            return;
        }
    }
    else
    {
        const int16_t *C;
        int diagonal_ref;

        diagonal_ref = fetch_diagonal_mv(h, &C, scan8[4], list, 2);

        tprintf(h->s.avctx, "pred_8x16: (%2d %2d %2d) at %2d %2d %d list %d\n", diagonal_ref, C[0], C[1], h->s.mb_x, h->s.mb_y, n, list);

        if (diagonal_ref == ref)
        {
            *mx = C[0];
            *my = C[1];
            return;
        }
    }

    //RARE
    pred_motion(h, n, 2, list, ref, mx, my);
}

static inline void pred_pskip_motion(H264Context *const h, int *const mx, int *const my)
{
    const int top_ref = h->ref_cache[0][ scan8[0] - 8 ];
    const int left_ref = h->ref_cache[0][ scan8[0] - 1 ];

    tprintf(h->s.avctx, "pred_pskip: (%d) (%d) at %2d %2d\n", top_ref, left_ref, h->s.mb_x, h->s.mb_y);

    if (top_ref == PART_NOT_AVAILABLE || left_ref == PART_NOT_AVAILABLE
        || !( top_ref | *(uint32_t *)h->mv_cache[0][ scan8[0] - 8 ])
        || !(left_ref | *(uint32_t *)h->mv_cache[0][ scan8[0] - 1 ]))
    {

        *mx = *my = 0;
        return;
    }

    pred_motion(h, 0, 4, 0, 0, mx, my);

    return;
}

static int get_scale_factor(H264Context *const h, int poc, int poc1, int i)
{
    int poc0 = h->ref_list[0][i].poc;
    int td = av_clip(poc1 - poc0, -128, 127);

    if (td == 0 || h->ref_list[0][i].long_ref)
    {
        return 256;
    }
    else
    {
        int tb = av_clip(poc - poc0, -128, 127);
        int tx = (16384 + (FFABS(td) >> 1)) / td;
        return av_clip((tb * tx + 32) >> 6, -1024, 1023);
    }
}

static inline void direct_dist_scale_factor(H264Context *const h)
{
    MpegEncContext *const s = &h->s;
    /* 2010/08/11 14:30:00 liuxw+00139685 */
    /* ����ǰͼ��Ϊ֡�����Ҷ�����POC�ȵ׳���POCҪ�������£���ǰPOC�ǵ��ڵ׳���POC(����С��) */
    //const int poc = FIELD_PICTURE ? h->s.current_picture_ptr->field_poc[ s->picture_structure == PICT_BOTTOM_FIELD ]
    const int poc = FIELD_PICTURE ? h->s.current_picture_ptr->field_poc[ s->picture_structure == PICT_BOTTOM_FIELD ] : h->s.current_picture_ptr->poc;
    const int poc1 = h->ref_list[1][0].poc;
    int i, field;

    /* 2010/03/16 19:29:00 liuxw+00139685 */
    /* ������mbaff���жϣ���û�У��򲻹�mbaff����ʹ�ܣ�����������²�����
       �����ӻᵼ��dist_scale_factor_field������������ǽṹ�������������� */
    if (FRAME_MBAFF)
    {
        for (field = 0; field < 2; field++)
        {
            const int poc  = h->s.current_picture_ptr->field_poc[field];
            const int poc1 = h->ref_list[1][0].field_poc[field];

            for (i = 0; i < (int)(2 * h->ref_count[0]); i++)
            { h->dist_scale_factor_field[field][i ^ field] = get_scale_factor(h, poc, poc1, i + 16); }
        }
    }

    for (i = 0; i < (int)h->ref_count[0]; i++)
    {
        h->dist_scale_factor[i] = get_scale_factor(h, poc, poc1, i);
    }
}

/* 2010/08/24 15:00:00 liuxw+00139685 */
/* ����������һ�����������ݵ�ǰ��slice num */
//static void fill_colmap(H264Context *h, int map[2][16+32], int list, int field, int colfield, int mbafi)
static void fill_colmap(H264Context *h, int map[2][16 + 32], int list, int field, int colfield, int mbafi, int slice_num)
{
    MpegEncContext *const s = &h->s;
    Picture *const ref1 = &h->ref_list[1][0];
    int j, old_ref, rfield;
    /* ��mbaff=0��start=0����mbaff=1��start=16 liuxw+00139685 */
    int start = mbafi ? 16 : 0;
    /* ��mbaff=0��end=h->ref_count[list]����mbaff=1��end �� 16+2*h->ref_count[list] liuxw+00139685 */
    /* 2010/08/24 15:00:00 liuxw+00139685 */
    /* endֻ�Ǳ�ʾlist0�Ľ�������list�޹� */
    //  int end  = mbafi ? 16+2*h->ref_count[list] : h->ref_count[list];
    int end  = mbafi ? 16 + 2 * h->ref_count[0] : h->ref_count[0];

    /* ����ǰΪ֡����mbaff=0ʱ��interl=0������ǰΪ������mbaff=1ʱ��interl=1 liuxw+00139685 */
    int interl = mbafi || s->picture_structure != PICT_FRAME;

    /* bogus; fills in for missing frames */
    memset(map[list], 0, sizeof(map[list]));

    for (rfield = 0; rfield < 2; rfield++)
    {
        /* 2010/08/24 15:00:00 liuxw+00139685 */
        /* �޸�Ϊ��ǰҪ�����slice����Ӧ�Ĳο����� */
        //      for(old_ref=0; old_ref<ref1->ref_count[colfield][list]; old_ref++)
        for (old_ref = 0; old_ref < ref1->ref_count[colfield][slice_num][list]; old_ref++)
        {
            /* 2010/08/24 15:00:00 liuxw+00139685 */
            /* �޸�Ϊ��ǰҪ�����slice����Ӧ�Ĳο����� */
            //          int poc = ref1->ref_poc[colfield][list][old_ref];
            int poc = ref1->ref_poc[colfield][slice_num][list][old_ref];

            /* ��ǰΪ֡����mbaff=0ʱ,��λref1(��Ϊ��ǰͼ��)�Ĳο������е�ǰ�ο�ͼ���POC�ĵͶ�λ(����ο����Ը�Ϊ֡�ο�) liuxw+00139685 */
            if (!interl)
            { poc |= 3; }
            /* 1����ǰ��֡��mbaff=1����ref1�Ĳο������е�ǰ�ο�ͼ��Ϊ֡
               2����ǰ�ǳ�����ref1�Ĳο������е�ǰ�ο�ͼ��Ϊ֡
               ������������֮һ����poc�ĵ���λ(�ο�����)����(����׵�����) liuxw+00139685 */
            else if ( interl && (poc & 3) == 3) //FIXME store all MBAFF references so this isnt needed
            { poc = (poc&~3) + rfield + 1; }

            for (j = start; j < end; j++)
            {
                /* �ӵ�ǰͼ��Ĳο�������ѡ����ref1��poc��ȵĲο�ͼ�񣬲��������ڵ�ǰͼ��ο������е������� liuxw+00139685 */
                /* 2010/04/010 14:30:00 liuxw+00139685 */
                /* ��ǰֻ��ʹ�òο��б�0����Զ���ref1ʱ�Ĳο��б�0�Ͳο��б�1 */
                //              if(4*h->ref_list[list][j].frame_num + (h->ref_list[list][j].reference&3) == poc){
                if (4 * h->ref_list[0][j].frame_num + (h->ref_list[0][j].reference & 3) == poc)
                {
                    /* ��mbaff=1ʱ���׳���ӳ���ϵҪ����һ���任��������׳�������[ԭ����Ϊ�洢˳��Ͷ���һ����Ϊ��ʹ��ȡ˳��һ��] liuxw+00139685 */
                    int cur_ref = mbafi ? (j - 16) ^ field : j;

                    /* mbaff=1�泡��ӳ���ϵ liuxw+00139685 */
                    /* ???��ǰmbaff=0,��ǰͼ���ref1���ǳ�������£�
                    ��1��map�����������׳�������[ref_count>16]��
                      2�� map[list][old_ref]��ֵ���ױ�����[ref_count>16] liuxw+00139685 */
                    /* 2010/03/16 15:32:00 liuxw+00139685 */
                    /* ����mbafi���жϣ�ֻ�е�mbaff=1ʱ���Ż��map[list][]���и�ֵ��
                       ���򵱳�����ʱ��ref_count > 16ʱ��map[list][]��������Ḳ�ǽṹ��������﷨Ԫ��
                       2010/04/08 15:32 hanqr+00115692
                       �����ж���������Ϊ����ǰ������֡���ԣ���ref1��Ӧ�ĵط��ǳ�����ʱ�����ʹ��map_col_to_list0��
                       ����ǰ�����ǳ�����ʱ�����ʹ��map_col_to_list0_field */
                    /* 2010/04/20 14:30:00 liuxw+00139685 [AZ1D01992] */
                    /* ��ref1����mbaff�Ĺ���ʱ��ҲҪ�Գ�(��16��ʼ)���д洢,��Ϊ�ں���ʹ��col_mal_to_list0ʱ������Ϊref1.mbaff=1,������������16��ƫ�� */
                    //					if((FRAME_MBAFF)||(1 == mbafi))
                    /* 2010/08/17 17:00:00 liuxw+00139685 */
                    /* ��FRAME_MBAFF������ȥ�� */
                    //if((FRAME_MBAFF)||(1 == mbafi) || ref1->mbaff)
                    /* 2010/08/26 10:00:00 liuxw+00139685 */
                    /* ����һ���ж�������ֻ�е�mbafiΪ1(��ǰ֡ΪMBAFF)����ref1Ϊ֡������Ż����˷�֧���������mbafi==1,����ref1Ϊ������ô���µļ������Ḳ��map[list][old_ref]���� */
                    if ((1 == mbafi && ref1->interlaced_frame == 0) || ref1->mbaff)
                    {
                        /* 2010/06/02 17:00:00 liuxw+00139685 */
                        /* ��old_ref����16��ȡģ��Ϊ�˷�ֹ������������ref1->ref_count[colfield][list]����16�����ֽ����˴˷�֧����ܿ��ܻḲ�ǻ�h�ṹ������ĳ�Ա */
                        map[list][2 * (old_ref % 16) + (rfield ^ field) + 16] = cur_ref;

                        if (old_ref >= 16)
                        {
                            av_log(NULL, AV_LOG_WARNING, "MBAFF:enable,but ref[%d] over 16!\n", old_ref);
                            s->avctx->iTotalError++;
                            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
                        }
                    }

                    /* mbaff=0ʱ��֡�򳡵�ӳ���ϵ��mbaff=1ʱ��֡��ӳ���ϵ liuxw+00139685 */
                    if (rfield == field)
                    { map[list][old_ref] = cur_ref; }

                    break;
                }
            }
        }
    }
}

/* Bֱ֡��Ԥ�⣨ʱ�䡢�ռ䣩����Ҫ�õ������ݵĳ�ʼ�� liuxw+00139685 */
static inline void direct_ref_list_init(H264Context *const h)
{
    MpegEncContext *const s = &h->s;
    Picture *const ref1 = &h->ref_list[1][0];
    Picture *const cur = s->current_picture_ptr;
    int list, j, field;
    /* ��ǰͼ����֡���Ƕ���ʱ��sidx=0����ǰͼ���ǵ׳�ʱ��sidx=1 liuxw+00139685 */
    int sidx = (s->picture_structure & 1) ^ 1;
    /* ref1(ǰ��ο������еĵ�һ���ο�ͼ��)��ref1����֡���Ƕ�����ref1sidx=0����ǰͼ���ǵ׳�ʱ��ref1sidx=1 liuxw+00139685 */
    int ref1sidx = (ref1->reference & 1) ^ 1;
    int ref1sidx2 = ref1sidx;
    int flag = 0;

    /* ���浱ǰͼ���ǰ����ο����м���POC����ǰͼ��Ľṹ�嵱�� liuxw+00139685 */
    for (list = 0; list < 2; list++)
    {
        /* 2010/08/24 14:00:00 liuxw+00139685 */
        /* ����ÿ��slice��ref poc */
        //      cur->ref_count[sidx][list] = h->ref_count[list];
        cur->ref_count[sidx][h->current_slice][list] = h->ref_count[list];

        for (j = 0; j < (int)h->ref_count[list]; j++)
            /* ��ǰ�����POC�ļ��㷽ʽ��(frame_num<<2 | reference&3)
               ��ǰ�ο�ͼ����֡��(frame_num<<2) + 3
               ��ǰ�ο�ͼ���Ƕ�����(frame_num<<2) + 1
               ��ǰ�ο�ͼ���ǵ׳���(frame_num<<2) + 2 liuxw+00139685 */
            //          cur->ref_poc[sidx][list][j] = 4*h->ref_list[list][j].frame_num + (h->ref_list[list][j].reference&3);
        { cur->ref_poc[sidx][h->current_slice][list][j] = 4 * h->ref_list[list][j].frame_num + (h->ref_list[list][j].reference & 3); }
    }

    /* ��ǰͼ����֡����cur->ref_count[0]��cur->ref_count[1]��ͬ����sidx liuxw+00139685 */
    if (s->picture_structure == PICT_FRAME)
    {
        /* 2010/08/24 14:00:00 liuxw+00139685 */
        /* ����ÿ��slice��ref poc */
        //      memcpy(cur->ref_count[1], cur->ref_count[0], sizeof(cur->ref_count[0]));
        //      memcpy(cur->ref_poc  [1], cur->ref_poc  [0], sizeof(cur->ref_poc  [0]));
        memcpy(cur->ref_count[1][h->current_slice], cur->ref_count[0][h->current_slice], sizeof(cur->ref_count[0][h->current_slice]));
        memcpy(cur->ref_poc  [1][h->current_slice], cur->ref_poc  [0][h->current_slice], sizeof(cur->ref_poc  [0][h->current_slice]));
    }

    cur->mbaff = FRAME_MBAFF;

    /* ����ǿռ�ֱ��Ԥ�⣬��ֱ�ӷ��� liuxw+00139685 */
    /* 2010/08/26 10:00:00 liuxw+00139685 */
    /* ����cur->pict_typeֻ�Ǳ�ʾ��ǰ֡��һ��slice�����ͣ���һ��֡�е�һ��sliceΪP�ڶ���ΪB sliceʱ����ô�Ͳ���ֱ������ȥ(����B���ʱ��ֱ��Ԥ��) */
    //	if(cur->pict_type != FF_B_TYPE || h->direct_spatial_mv_pred)
    if (s->pict_type != FF_B_TYPE || h->direct_spatial_mv_pred)
    { return; }

    /* 2010/08/26 10:00:00 liuxw+00139685 */
    /* �����ǰ��֡��ref1��һ�ο����ԣ���ô����ref1->slice_numѡ����һ������sliceslice��Ŀ��col_pic����һ���� */
    if (PICT_FRAME == s->picture_structure && ref1->interlaced_frame)
    {
        ref1sidx2 = FFABS(ref1->field_poc[0] - cur->poc) >= FFABS(ref1->field_poc[1] - cur->poc);
        ref1sidx = ref1sidx2;
    }

    /* 2010/08/26 10:00:00 liuxw+00139685 */
    /* �����ǰ�ǳ���ref1��֡����ô��ֻȡindexΪ0��ref1->slice_num��Ŀ */
    if (PICT_FRAME != s->picture_structure && !ref1->interlaced_frame)
    {
        ref1sidx2 = 0;
    }

    /* 2010/08/26 10:00:00 liuxw+00139685 */
    /* �����ǰ֡��mbaff����ref1Ҳ��mbaff����ô�ڼ���field_map��ʱ�򣬽�ֻȡindexΪ0��ref1->slice_num��Ŀ */
    if (cur->mbaff && ref1->mbaff)
    {
        flag = 1;
    }

    /* ʱ��ֱ��Ԥ��: ref1�����ǰ��Ԥ�������cur�ο����е�ӳ���ϵ liuxw+00139685 */
    for (list = 0; list < 2; list++)
    {
        /* mbaff=0ʱ��ref1�Ĳο����к�cur�ο����е�ӳ���ϵ(֡���������׳���һ��ֻ�������е�һ��) liuxw+00139685 */
        /* 2010/04/20 9:30:00 liuxw+00139685 [AZ1D01992] */
        /* ����һ��ѭ��������ǰͼ����֡��ref1�ǳ������Σ����ܻ��õ�map_col_to_list0[1] */
        //      fill_colmap(h, h->map_col_to_list0, list, sidx, ref1sidx, 0);
        int j;

        //		for(i=0;i<2;i++)
        //		{
        /* 2010/08/24 14:00:00 liuxw+00139685 */
        /* ����ref1��ÿһ��slice�Ĳο����ж�Ҫ����map */
        /*			fill_colmap(h, h->map_col_to_list0[i], list, sidx, ref1sidx, 0);
        			ref1sidx = !ref1sidx; */
        for (j = 0; j < ref1->slice_num[ref1sidx2]; j++)
        {
            fill_colmap(h, h->map_col_to_list0[j], list, sidx, ref1sidx, 0, j);
        }

        //			ref1sidx = !ref1sidx;
        //		}
        /* mbaff=1ʱ��ref1�Ĳο����к�cur�ο����е�ӳ���ϵ��֡���������׳���һ��ʹ�����֣� liuxw+00139685 */
        /* 2010/03/16 19:30:00 liuxw+00139685 */
        /* ���Ӷ�mbaff���жϣ�����ǰmbaffû��ʹ�ܣ�����˲�����
           ����ǰΪ���������ref_count����16ʱ���ͻḲ�ǽṹ���������� */
        if (FRAME_MBAFF)
        {
            /* 2010/08/24 14:00:00 liuxw+00139685 */
            /* ����ref1��ÿһ��slice�Ĳο����ж�Ҫ����map */
            /*			for(field=0; field<2; field++)
            				fill_colmap(h, h->map_col_to_list0_field[field], list, field, field, 1); */
            for (field = 0; field < 2; field++)
            {
                /* 2010/08/26 10:00:00 liuxw+00139685 */
                /* �����ǰ֡��mbaff����ref1Ҳ��mbaff����ô�ڼ���field_map��ʱ�򣬽�ֻȡindexΪ0��ref1->slice_num��Ŀ */
                //for(j=0;j<ref1->slice_num[field];j++)
                for (j = 0; j < ref1->slice_num[flag ? 0 : field]; j++)
                {
                    fill_colmap(h, h->map_col_to_list0_field[field][j], list, field, field, 1, j);
                }
            }
        }
    }
}

static inline void pred_direct_motion(H264Context *const h, int *mb_type)
{
    MpegEncContext *const s = &h->s;
    int b8_stride = h->b8_stride;
    int b4_stride = h->b_stride;
    int mb_xy = h->mb_xy;
    int mb_type_col[2];
    const int16_t (*l1mv0)[2], (*l1mv1)[2];
    const int8_t *l1ref0, *l1ref1;
    const int is_b8x8 = IS_8X8(*mb_type);
    unsigned int sub_mb_type;
    int i8, i4;
    /* 2010/04/20 14:00:00 liuxw+00139685��[AZ1D01992] */
    /* ����һ������������ǰͼ��Ϊ֡��ref1Ϊ�������ʹ�� */
    /* 2010/08/26 10:00:00 liuxw+00139685 */
    /* ����ǰͼ��Ϊ֡��ref1Ϊ��ʱ������flag��ָ��������ͬһʱ��ֻ�����һ�����������ȫ��index=0���� */
    //	int flag = 0;
    /* 2010/06/10 14:30:00 liuxw+00139685 */
    /* ����һ����������ֹ����ʱ������������� */
    int pre_flag = 0;

#ifdef LXW_DEBUG

    /* add by lxw for test */
    if (frame_num == TARGET_FRAME && s->mb_x == MB_X && s->mb_y == MB_Y)
    {
        av_log(NULL, AV_LOG_ERROR, "come in poc:%d mb_x:%d mb_y:%d \n", s->current_picture_ptr->field_poc[0], s->mb_x, s->mb_y);
    }

#endif

#define MB_TYPE_16x16_OR_INTRA (MB_TYPE_16x16|MB_TYPE_INTRA4x4|MB_TYPE_INTRA16x16|MB_TYPE_INTRA_PCM)


    /* 2010/04/15 18:30:00 liuxw+00139685 [AZ1D01992] */
    /* ������һ���ж�����������ǰͼ��Ͳο�ͼ��ȫ�ǳ��������෴�������������if��� */
    //     if(IS_INTERLACED(h->ref_list[1][0].mb_type[mb_xy])){ // AFL/AFR/FR/FL -> AFL/FL
    if (IS_INTERLACED( h->ref_list[1][0].mb_type[mb_xy]) || (h->ref_list[1][0].mbaff != 1 && h->ref_list[1][0].interlaced_frame))
    {
        if (!IS_INTERLACED(*mb_type))                    //     AFR/FR    -> AFL/FL
        {
            int cur_poc = s->current_picture_ptr->poc;
            int *col_poc = h->ref_list[1]->field_poc;
            int col_parity = FFABS(col_poc[0] - cur_poc) >= FFABS(col_poc[1] - cur_poc);
            mb_xy = s->mb_x + ((s->mb_y&~1) + col_parity) * s->mb_stride;
            b8_stride = 0;
            /* 2010/04/20 14:00:00 liuxw+00139685��[AZ1D01992] */
            /* ���浱ǰ����Ӧ��ref1�ĳ�(������׳�) */
            //flag = col_parity;
        }
        else if (!(s->picture_structure & h->ref_list[1][0].reference) && !h->ref_list[1][0].mbaff) // FL -> FL & differ parity
        {
            int fieldoff = 2 * (h->ref_list[1][0].reference) - 3;
            mb_xy += s->mb_stride * fieldoff;
        }

        goto single_col;
    }
    else                                                 // AFL/AFR/FR/FL -> AFR/FR
    {
        if (IS_INTERLACED(*mb_type))                     // AFL       /FL -> AFR/FR
        {
            mb_xy = s->mb_x + (s->mb_y&~1) * s->mb_stride;
            mb_type_col[0] = h->ref_list[1][0].mb_type[mb_xy];
            mb_type_col[1] = h->ref_list[1][0].mb_type[mb_xy + s->mb_stride];
            b8_stride *= 3;
            b4_stride *= 6;
            /* 2010/06/10 14:30:00 liuxw+00139685 */
            /* ��λ�����ӵı�������ֹ����ʱ������������� */
            pre_flag = 1;

            //FIXME IS_8X8(mb_type_col[0]) && !h->sps.direct_8x8_inference_flag
            if (    (mb_type_col[0] & MB_TYPE_16x16_OR_INTRA)
                    && (mb_type_col[1] & MB_TYPE_16x16_OR_INTRA)
                    && !is_b8x8)
            {
                sub_mb_type = MB_TYPE_16x16 | MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2; /* B_SUB_8x8 */
                *mb_type   |= MB_TYPE_16x8 | MB_TYPE_L0L1 | MB_TYPE_DIRECT2; /* B_16x8 */
            }
            else
            {
                sub_mb_type = MB_TYPE_16x16 | MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2; /* B_SUB_8x8 */
                *mb_type   |= MB_TYPE_8x8 | MB_TYPE_L0L1;
            }
        }
        else                                             //     AFR/FR    -> AFR/FR
        {
        single_col:
            mb_type_col[0] =
                mb_type_col[1] = h->ref_list[1][0].mb_type[mb_xy];

            if (IS_8X8(mb_type_col[0]) && !h->sps.direct_8x8_inference_flag)
            {
                /* FIXME save sub mb types from previous frames (or derive from MVs)
                * so we know exactly what block size to use */
                sub_mb_type = MB_TYPE_8x8 | MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2; /* B_SUB_4x4 */
                *mb_type   |= MB_TYPE_8x8 | MB_TYPE_L0L1;
            }
            else if (!is_b8x8 && (mb_type_col[0] & MB_TYPE_16x16_OR_INTRA))
            {
                sub_mb_type = MB_TYPE_16x16 | MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2; /* B_SUB_8x8 */
                *mb_type   |= MB_TYPE_16x16 | MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2; /* B_16x16 */
            }
            else
            {
                sub_mb_type = MB_TYPE_16x16 | MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2; /* B_SUB_8x8 */
                *mb_type   |= MB_TYPE_8x8 | MB_TYPE_L0L1;
            }
        }
    }

    /* 2010/06/01 9:00:00 liuxw+00139685 */
    /* ��Bֱ֡��Ԥ��ʱ����ref1��colocated��������ʱ�����кϷ����жϣ����Ƿ�(����ͼ��Χ)�������������h->mb_xy */
    if (0 > mb_xy || s->mb_stride * s->mb_height < mb_xy)
    {
        av_log(s->avctx, AV_LOG_WARNING, "Colocated MB is out of ref pic when process Direct MB!\n");
        s->avctx->iTotalError++;
        IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MV);
        mb_xy = h->mb_xy;
    }

    /* (�õ�mvx(0\1)��refx(0\1)) */
    l1mv0  = &h->ref_list[1][0].motion_val[0][h->mb2b_xy [mb_xy]];
    l1mv1  = &h->ref_list[1][0].motion_val[1][h->mb2b_xy [mb_xy]];
    l1ref0 = &h->ref_list[1][0].ref_index [0][h->mb2b8_xy[mb_xy]];
    l1ref1 = &h->ref_list[1][0].ref_index [1][h->mb2b8_xy[mb_xy]];

    if (!b8_stride)
    {
        if (s->mb_y & 1)
        {
            l1ref0 += h->b8_stride;
            l1ref1 += h->b8_stride;
            l1mv0  +=  2 * b4_stride;
            l1mv1  +=  2 * b4_stride;
        }
    }

    /* �ռ�ֱ��Ԥ�� liuxw+00139685 */
    if (h->direct_spatial_mv_pred)
    {
        int ref[2];
        int mv[2][2];
        int list;

        /* FIXME interlacing + spatial direct uses wrong colocated block positions */

        /* ref = min(neighbors) */
        for (list = 0; list < 2; list++)
        {
            int refa = h->ref_cache[list][scan8[0] - 1];
            int refb = h->ref_cache[list][scan8[0] - 8];
            int refc = h->ref_cache[list][scan8[0] - 8 + 4];

            if (refc == PART_NOT_AVAILABLE)
            { refc = h->ref_cache[list][scan8[0] - 8 - 1]; }

            ref[list] = FFMIN3((unsigned)refa, (unsigned)refb, (unsigned)refc);

            if (ref[list] < 0)
            { ref[list] = -1; }
        }

        if (ref[0] < 0 && ref[1] < 0)
        {
            ref[0] = ref[1] = 0;
            mv[0][0] = mv[0][1] =
                           mv[1][0] = mv[1][1] = 0;
        }
        else
        {
            for (list = 0; list < 2; list++)
            {
                if (ref[list] >= 0)
                { pred_motion(h, 0, 4, list, ref[list], &mv[list][0], &mv[list][1]); }
                else
                { mv[list][0] = mv[list][1] = 0; }
            }
        }

        if (ref[1] < 0)
        {
            if (!is_b8x8)
            { *mb_type &= ~MB_TYPE_L1; }

            sub_mb_type &= ~MB_TYPE_L1;
        }
        else if (ref[0] < 0)
        {
            if (!is_b8x8)
            { *mb_type &= ~MB_TYPE_L0; }

            sub_mb_type &= ~MB_TYPE_L0;
        }

        if (IS_INTERLACED(*mb_type) != IS_INTERLACED(mb_type_col[0]))
        {
            for (i8 = 0; i8 < 4; i8++)
            {
                int x8 = i8 & 1;
                int y8 = i8 >> 1;
                int xy8 = x8 + y8 * b8_stride;
                int xy4 = 3 * x8 + y8 * b4_stride;
                int a = 0, b = 0;

                if (is_b8x8 && !IS_DIRECT(h->sub_mb_type[i8]))
                { continue; }

                h->sub_mb_type[i8] = sub_mb_type;

                fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, (uint8_t)ref[0], 1);
                fill_rectangle(&h->ref_cache[1][scan8[i8 * 4]], 2, 2, 8, (uint8_t)ref[1], 1);

                if (!IS_INTRA(mb_type_col[y8])
                    && (   (l1ref0[xy8] == 0 && FFABS(l1mv0[xy4][0]) <= 1 && FFABS(l1mv0[xy4][1]) <= 1)
                           || (l1ref0[xy8]  < 0 && l1ref1[xy8] == 0 && FFABS(l1mv1[xy4][0]) <= 1 && FFABS(l1mv1[xy4][1]) <= 1)))
                {
                    if (ref[0] > 0)
                    { a = pack16to32(mv[0][0], mv[0][1]); }

                    if (ref[1] > 0)
                    { b = pack16to32(mv[1][0], mv[1][1]); }
                }
                else
                {
                    a = pack16to32(mv[0][0], mv[0][1]);
                    b = pack16to32(mv[1][0], mv[1][1]);
                }

                fill_rectangle(&h->mv_cache[0][scan8[i8 * 4]], 2, 2, 8, a, 4);
                fill_rectangle(&h->mv_cache[1][scan8[i8 * 4]], 2, 2, 8, b, 4);
            }
        }
        else if (IS_16X16(*mb_type))
        {
            int a = 0, b = 0;

            fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, (uint8_t)ref[0], 1);
            fill_rectangle(&h->ref_cache[1][scan8[0]], 4, 4, 8, (uint8_t)ref[1], 1);

            /* 2010/08/29 15:00:00 liuxw+00139685 */
            /* colZeroFlag����1�������ܹ�������������ֻ�����������һ����h->ref_list[1][0]�Ƕ��ڲο� */
            /*	if(!IS_INTRA(mb_type_col[0])
                   && (   (l1ref0[0] == 0 && FFABS(l1mv0[0][0]) <= 1 && FFABS(l1mv0[0][1]) <= 1)
                       || (l1ref0[0]  < 0 && l1ref1[0] == 0 && FFABS(l1mv1[0][0]) <= 1 && FFABS(l1mv1[0][1]) <= 1
                           && (h->x264_build>33 || !h->x264_build))))*/
            if (!IS_INTRA(mb_type_col[0]) && !h->ref_list[1][0].long_ref && ((l1ref0[0] == 0 && FFABS(l1mv0[0][0]) <= 1 && FFABS(l1mv0[0][1]) <= 1)
                    || (l1ref0[0]  < 0 && l1ref1[0] == 0 && FFABS(l1mv1[0][0]) <= 1 && FFABS(l1mv1[0][1]) <= 1 && (h->x264_build > 33 || !h->x264_build))))
            {
                if (ref[0] > 0)
                { a = pack16to32(mv[0][0], mv[0][1]); }

                if (ref[1] > 0)
                { b = pack16to32(mv[1][0], mv[1][1]); }
            }
            else
            {
                a = pack16to32(mv[0][0], mv[0][1]);
                b = pack16to32(mv[1][0], mv[1][1]);
            }

            fill_rectangle(&h->mv_cache[0][scan8[0]], 4, 4, 8, a, 4);
            fill_rectangle(&h->mv_cache[1][scan8[0]], 4, 4, 8, b, 4);
        }
        else
        {
            /* 2010/06/12 10:30:00 liuxw+00139685 */
            /* ���˱�־����λ�����ֽ���˷�֧����˵�����������룬��b8_stride��b4_stride�������� */
            if (pre_flag)
            {
                b8_stride = s->b8_stride;
                b4_stride = s->b4_stride;
            }

            for (i8 = 0; i8 < 4; i8++)
            {
                const int x8 = i8 & 1;
                const int y8 = i8 >> 1;

                if (is_b8x8 && !IS_DIRECT(h->sub_mb_type[i8]))
                { continue; }

                h->sub_mb_type[i8] = sub_mb_type;

                fill_rectangle(&h->mv_cache[0][scan8[i8 * 4]], 2, 2, 8, pack16to32(mv[0][0], mv[0][1]), 4);
                fill_rectangle(&h->mv_cache[1][scan8[i8 * 4]], 2, 2, 8, pack16to32(mv[1][0], mv[1][1]), 4);
                fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, (uint8_t)ref[0], 1);
                fill_rectangle(&h->ref_cache[1][scan8[i8 * 4]], 2, 2, 8, (uint8_t)ref[1], 1);

                /* col_zero_flag */
                /* 2010/08/29 15:00:00 liuxw+00139685 */
                /* colZeroFlag����1�������ܹ�������������ֻ�����������һ����h->ref_list[1][0]�Ƕ��ڲο� */
                /* if(!IS_INTRA(mb_type_col[0]) && (   l1ref0[x8 + y8*b8_stride] == 0
                                               || (l1ref0[x8 + y8*b8_stride] < 0 && l1ref1[x8 + y8*b8_stride] == 0
                                                   && (h->x264_build>33 || !h->x264_build)))) */
                if (!IS_INTRA(mb_type_col[0]) && !h->ref_list[1][0].long_ref && (   l1ref0[x8 + y8 * b8_stride] == 0
                        || (l1ref0[x8 + y8 * b8_stride] < 0 && l1ref1[x8 + y8 * b8_stride] == 0 && (h->x264_build > 33 || !h->x264_build))))
                {
                    const int16_t (*l1mv)[2] = l1ref0[x8 + y8 * b8_stride] == 0 ? l1mv0 : l1mv1;

                    if (IS_SUB_8X8(sub_mb_type))
                    {
                        const int16_t *mv_col = l1mv[x8 * 3 + y8 * 3 * b4_stride];

                        if (FFABS(mv_col[0]) <= 1 && FFABS(mv_col[1]) <= 1)
                        {
                            if (ref[0] == 0)
                            { fill_rectangle(&h->mv_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 4); }

                            if (ref[1] == 0)
                            { fill_rectangle(&h->mv_cache[1][scan8[i8 * 4]], 2, 2, 8, 0, 4); }
                        }
                    }
                    else
                        for (i4 = 0; i4 < 4; i4++)
                        {
                            const int16_t *mv_col = l1mv[x8 * 2 + (i4 & 1) + (y8 * 2 + (i4 >> 1)) * b4_stride];

                            if (FFABS(mv_col[0]) <= 1 && FFABS(mv_col[1]) <= 1)
                            {
                                if (ref[0] == 0)
                                { *(uint32_t *)h->mv_cache[0][scan8[i8 * 4 + i4]] = 0; }

                                if (ref[1] == 0)
                                { *(uint32_t *)h->mv_cache[1][scan8[i8 * 4 + i4]] = 0; }
                            }
                        }
                }
            }
        }
    }
    else
    {
        /* direct temporal mv pred */
        /* 2010/04/20 14:00:00 liuxw+00139685��[AZ1D01992] */
        /* ����ǰͼ��Ϊ֡��ref1Ϊ��������£�����ref1�Ƕ������ǵ׳���ѡ��map_col_to_list0 */
        //		const int *map_col_to_list0[2] = {h->map_col_to_list0[0], h->map_col_to_list0[1]};
        /* 2010/08/24 15:00:00 liuxw+00139685 */
        /* ѡȡcol_MB���ڵ�slice�Ĳο�����map */
        //		const int *map_col_to_list0[2] = {h->map_col_to_list0[flag][0], h->map_col_to_list0[flag][1]};
        const int *map_col_to_list0[2] = {h->map_col_to_list0[(h->ref_list[1][0].slice_table[mb_xy] & (MAX_SLICES - 1)) - 1][0], h->map_col_to_list0[(h->ref_list[1][0].slice_table[mb_xy] & (MAX_SLICES - 1)) - 1][1]};
        const int *dist_scale_factor = h->dist_scale_factor;
        int ref_offset = 0;

        if (FRAME_MBAFF && IS_INTERLACED(*mb_type))
        {
            /* 2010/08/24 15:00:00 liuxw+00139685 */
            /* ѡȡcol_MB���ڵ�slice�Ĳο�����map */
            //			map_col_to_list0[0] = h->map_col_to_list0_field[s->mb_y&1][0];
            //			map_col_to_list0[1] = h->map_col_to_list0_field[s->mb_y&1][1];
            map_col_to_list0[0] = h->map_col_to_list0_field[s->mb_y & 1][(h->ref_list[1][0].slice_table[mb_xy] & (MAX_SLICES - 1)) - 1][0];
            map_col_to_list0[1] = h->map_col_to_list0_field[s->mb_y & 1][(h->ref_list[1][0].slice_table[mb_xy] & (MAX_SLICES - 1)) - 1][1];
            dist_scale_factor   = h->dist_scale_factor_field[s->mb_y & 1];
        }

        if (h->ref_list[1][0].mbaff && IS_INTERLACED(mb_type_col[0]))
        {
            ref_offset += 16;
        }

        if (IS_INTERLACED(*mb_type) != IS_INTERLACED(mb_type_col[0]))
        {
            /* FIXME assumes direct_8x8_inference == 1 ��maybe have problems??? liuxw+00139685��*/
            int y_shift  = 2 * !IS_INTERLACED(*mb_type);

            for (i8 = 0; i8 < 4; i8++)
            {
                const int x8 = i8 & 1;
                const int y8 = i8 >> 1;
                int ref0, scale;
                const int16_t (*l1mv)[2] = l1mv0;

                if (is_b8x8 && !IS_DIRECT(h->sub_mb_type[i8]))
                { continue; }

                h->sub_mb_type[i8] = sub_mb_type;

                fill_rectangle(&h->ref_cache[1][scan8[i8 * 4]], 2, 2, 8, 0, 1);

                if (IS_INTRA(mb_type_col[y8]))
                {
                    fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 1);
                    fill_rectangle(&h-> mv_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 4);
                    fill_rectangle(&h-> mv_cache[1][scan8[i8 * 4]], 2, 2, 8, 0, 4);
                    continue;
                }

                /* 2010/08/25 10:00:00 liuxw+00139685 */
                /* ����ǰ�ǳ���ref1��֡����ôcol_mb��������(0/1,2/3) */
                if (pre_flag && i8 > 1)
                {
                    /* 2010/08/26 10:00:00 liuxw+00139685 */
                    /* ����ǰ��MBAFF�еĳ���飬ref1��֡(MBAFF)����ôcol_mb��������(0/1,2/3) */
                    if (FRAME_MBAFF && IS_INTERLACED(*mb_type))
                    {
                        map_col_to_list0[0] = h->map_col_to_list0_field[s->mb_y & 1][(h->ref_list[1][0].slice_table[mb_xy + s->mb_stride] & (MAX_SLICES - 1)) - 1][0];
                        map_col_to_list0[1] = h->map_col_to_list0_field[s->mb_y & 1][(h->ref_list[1][0].slice_table[mb_xy + s->mb_stride] & (MAX_SLICES - 1)) - 1][1];
                    }
                    /* 2010/08/25 10:00:00 liuxw+00139685 */
                    /* ����ǰ�ǳ���ref1��֡����ôcol_mb��������(0/1,2/3) */
                    else
                    {
                        map_col_to_list0[0] = h->map_col_to_list0[(h->ref_list[1][0].slice_table[mb_xy + s->mb_stride] & (MAX_SLICES - 1)) - 1][0];
                        map_col_to_list0[1] = h->map_col_to_list0[(h->ref_list[1][0].slice_table[mb_xy + s->mb_stride] & (MAX_SLICES - 1)) - 1][1];
                    }
                }

                ref0 = l1ref0[x8 + y8 * b8_stride];

                if (ref0 >= 0)
                { ref0 = map_col_to_list0[0][ref0 + ref_offset]; }
                /* 2010/04/15 18:30:00 liuxw+00139685 */
                /* �����˶�ref1ǰ��ο��Ƿ���ڵ��жϺͶ�ref1ǰ���򶼲�����ʱ��ref��mv�Ĵ��� */
                /*			else
                			ref0 = map_col_to_list0[1][l1ref1[x8 + y8*b8_stride] + ref_offset]; */
                else if (l1ref1[x8 + y8 * b8_stride] >= 0)
                {
                    ref0 = map_col_to_list0[1][l1ref1[x8 + y8 * b8_stride] + ref_offset];
                    l1mv = l1mv1;
                }
                else
                {
                    fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 1);
                    fill_rectangle(&h->mv_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 4);
                    fill_rectangle(&h->mv_cache[1][scan8[i8 * 4]], 2, 2, 8, 0, 4);
                    continue;
                }

                scale = dist_scale_factor[ref0];
                fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, ref0, 1);

                {
                    const int16_t *mv_col = l1mv[x8 * 3 + y8 * b4_stride];
                    int my_col = (mv_col[1] << y_shift) / 2;
                    int mx = (scale * mv_col[0] + 128) >> 8;
                    int my = (scale * my_col + 128) >> 8;
                    fill_rectangle(&h->mv_cache[0][scan8[i8 * 4]], 2, 2, 8, pack16to32(mx, my), 4);
                    fill_rectangle(&h->mv_cache[1][scan8[i8 * 4]], 2, 2, 8, pack16to32(mx - mv_col[0], my - my_col), 4);
                    /* add by lxw for test */
                    //					av_log(NULL,AV_LOG_ERROR,"ref0(map_col_to_list0):%d scale:(dist_scale_factor):%d mx_col:%d my_col:%d mx0:%d my0%d\n",ref0,scale,mv_col[0],my_col,mx,my);
                }
            }

            return;
        }

        /* one-to-one mv scaling */

        if (IS_16X16(*mb_type))
        {
            int ref, mv0, mv1;

            fill_rectangle(&h->ref_cache[1][scan8[0]], 4, 4, 8, 0, 1);

            if (IS_INTRA(mb_type_col[0]))
            {
                ref = mv0 = mv1 = 0;
            }
            else
            {
                /* 2010/04/15 18:30:00 liuxw+00139685 */
                /* �����˶�colref��colmv��ѡ��[����ɲμ�Э��] */
                //const int ref0 = l1ref0[0] >= 0 ? map_col_to_list0[0][l1ref0[0] + ref_offset]
                //: map_col_to_list0[1][l1ref1[0] + ref_offset];
                const int ref0 = l1ref0[0] >= 0 ? map_col_to_list0[0][l1ref0[0] + ref_offset]
                                 : (l1ref1[0] >= 0 ? map_col_to_list0[1][l1ref1[0] + ref_offset] : 0);
                const int scale = dist_scale_factor[ref0];
                //const int16_t *mv_col = l1ref0[0] >= 0 ? l1mv0[0] : l1mv1[0];
                int16_t tmp_mv_col[2] = {0, 0};
                const int16_t *mv_col = l1ref0[0] >= 0 ? l1mv0[0] : (l1ref1[0] >= 0 ? l1mv1[0] : tmp_mv_col);
                int mv_l0[2];
                mv_l0[0] = (scale * mv_col[0] + 128) >> 8;
                mv_l0[1] = (scale * mv_col[1] + 128) >> 8;
                ref = ref0;
                mv0 = pack16to32(mv_l0[0], mv_l0[1]);
                mv1 = pack16to32(mv_l0[0] - mv_col[0], mv_l0[1] - mv_col[1]);
            }

            fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, ref, 1);
            fill_rectangle(&h-> mv_cache[0][scan8[0]], 4, 4, 8, mv0, 4);
            fill_rectangle(&h-> mv_cache[1][scan8[0]], 4, 4, 8, mv1, 4);
        }
        else
        {
            /* 2010/06/10 14:30:00 liuxw+00139685 */
            /* ���˱�־����λ�����ֽ���˷�֧����˵�����������룬��b8_stride��b4_stride�������� */
            if (pre_flag)
            {
                b8_stride = s->b8_stride;
                b4_stride = s->b4_stride;
            }

            for (i8 = 0; i8 < 4; i8++)
            {
                const int x8 = i8 & 1;
                const int y8 = i8 >> 1;
                int ref0, scale;
                const int16_t (*l1mv)[2] = l1mv0;
                /* 2010/04/15 18:30:00 liuxw+00139685 */
                /* �����˶�ref1�����ǰ���ѡ���־[����ɲμ�Э��] */
                int flag = 1;

                if (is_b8x8 && !IS_DIRECT(h->sub_mb_type[i8]))
                { continue; }

                h->sub_mb_type[i8] = sub_mb_type;
                fill_rectangle(&h->ref_cache[1][scan8[i8 * 4]], 2, 2, 8, 0, 1);

                if (IS_INTRA(mb_type_col[0]))
                {
                    fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 1);
                    fill_rectangle(&h-> mv_cache[0][scan8[i8 * 4]], 2, 2, 8, 0, 4);
                    fill_rectangle(&h-> mv_cache[1][scan8[i8 * 4]], 2, 2, 8, 0, 4);
                    continue;
                }

                ref0 = l1ref0[x8 + y8 * b8_stride] + ref_offset;

                /* 2010/04/15 18:30:00 hanqr+00115692 [AZ1D01992] */
                /* �޸��˶�ref1����ο����ж����� */
                //               if(ref0 >= 0)
                if (ref0 >= ref_offset)
                { ref0 = map_col_to_list0[0][ref0]; }
                /* 2010/04/15 18:30:00 liuxw+00139685 */
                /* �����˶�ref1ǰ��ο��Ƿ���ڵ��жϺͶ�ref1ǰ���򶼲�����ʱ��ref��mv�Ĵ��� */
                //               else{
                else if (l1ref1[x8 + y8 * b8_stride] >= 0)
                {
                    ref0 = map_col_to_list0[1][l1ref1[x8 + y8 * b8_stride] + ref_offset];
                    l1mv = l1mv1;
                }
                else
                {
                    ref0 = 0;
                    flag = 0;
                }

                scale = dist_scale_factor[ref0];

                fill_rectangle(&h->ref_cache[0][scan8[i8 * 4]], 2, 2, 8, ref0, 1);

                if (IS_SUB_8X8(sub_mb_type))
                {
                    /* 2010/04/15 18:30:00 liuxw+00139685 */
                    /* ������ref1ǰ��ͺ���ο���������ʱmv_col�Ĵ��� */
                    //					const int16_t *mv_col = l1mv[x8*3 + y8*3*b4_stride];
                    int16_t tmp_mv_col[2] = {0, 0};
                    const int16_t *mv_col = flag ? l1mv[x8 * 3 + y8 * 3 * b4_stride] : tmp_mv_col;
                    int mx = (scale * mv_col[0] + 128) >> 8;
                    int my = (scale * mv_col[1] + 128) >> 8;
                    fill_rectangle(&h->mv_cache[0][scan8[i8 * 4]], 2, 2, 8, pack16to32(mx, my), 4);
                    fill_rectangle(&h->mv_cache[1][scan8[i8 * 4]], 2, 2, 8, pack16to32(mx - mv_col[0], my - mv_col[1]), 4);
                    /* add by lxw for test */
                    //					av_log(NULL,AV_LOG_ERROR,"ref0(map_col_to_list0):%d scale:(dist_scale_factor):%d mx_col:%d my_col:%d mx0:%d my0%d\n",ref0,scale,mv_col[0],mv_col[1],mx,my);
                }
                else
                    for (i4 = 0; i4 < 4; i4++)
                    {
                        /* 2010/04/15 18:30:00 liuxw+00139685 */
                        /* ������ref1ǰ��ͺ���ο���������ʱmv_col�Ĵ��� */
                        //					const int16_t *mv_col = l1mv[x8*2 + (i4&1) + (y8*2 + (i4>>1))*b4_stride];
                        int16_t tmp_mv_col[2] = {0, 0};
                        const int16_t *mv_col = flag ? l1mv[x8 * 2 + (i4 & 1) + (y8 * 2 + (i4 >> 1)) * b4_stride] : tmp_mv_col;
                        int16_t *mv_l0 = h->mv_cache[0][scan8[i8 * 4 + i4]];
                        mv_l0[0] = (scale * mv_col[0] + 128) >> 8;
                        mv_l0[1] = (scale * mv_col[1] + 128) >> 8;
                        /* 2010/04/15 18:30:00 liuxw+00139685 */
                        /* �����˶Ե�ǰ��鲿�ֺ���ο�mv�Ĵ洢 */
                        *(uint32_t *)h->mv_cache[0][scan8[i8 * 4 + i4]] = pack16to32(mv_l0[0], mv_l0[1]);
                        *(uint32_t *)h->mv_cache[1][scan8[i8 * 4 + i4]] = pack16to32(mv_l0[0] - mv_col[0], mv_l0[1] - mv_col[1]);
                        /* add by lxw for test */
                        //					av_log(NULL,AV_LOG_ERROR,"[4x4] ref0(map_col_to_list0):%d scale:(dist_scale_factor):%d mx_col:%d my_col:%d mx0:%d my0%d\n",ref0,scale,mv_col[0],mv_col[1], mv_l0[0],mv_l0[1]);
                    }
            }
        }
    }
}

static inline void write_back_motion(H264Context *h, int mb_type)
{
    MpegEncContext *const s = &h->s;
    const int b_xy = 4 * s->mb_x + 4 * s->mb_y * h->b_stride;
    const int b8_xy = 2 * s->mb_x + 2 * s->mb_y * h->b8_stride;
    int list;

    if (!USES_LIST(mb_type, 0))
    { fill_rectangle(&s->current_picture.ref_index[0][b8_xy], 2, 2, h->b8_stride, (uint8_t)LIST_NOT_USED, 1); }

    for (list = 0; list < (int)h->list_count; list++)
    {
        int y;

        if (!USES_LIST(mb_type, list))
        { continue; }

        for (y = 0; y < 4; y++)
        {
            *(uint64_t *)s->current_picture.motion_val[list][b_xy + 0 + y * h->b_stride] = *(uint64_t *)h->mv_cache[list][scan8[0] + 0 + 8 * y];
            *(uint64_t *)s->current_picture.motion_val[list][b_xy + 2 + y * h->b_stride] = *(uint64_t *)h->mv_cache[list][scan8[0] + 2 + 8 * y];
        }

        if ( h->pps.cabac )
        {
            if (IS_SKIP(mb_type))
            { fill_rectangle(h->mvd_table[list][b_xy], 4, 4, h->b_stride, 0, 4); }
            else
                for (y = 0; y < 4; y++)
                {
                    *(uint64_t *)h->mvd_table[list][b_xy + 0 + y * h->b_stride] = *(uint64_t *)h->mvd_cache[list][scan8[0] + 0 + 8 * y];
                    *(uint64_t *)h->mvd_table[list][b_xy + 2 + y * h->b_stride] = *(uint64_t *)h->mvd_cache[list][scan8[0] + 2 + 8 * y];
                }
        }

        {
            int8_t *ref_index = &s->current_picture.ref_index[list][b8_xy];
            ref_index[0 + 0 * h->b8_stride] = h->ref_cache[list][scan8[0]];
            ref_index[1 + 0 * h->b8_stride] = h->ref_cache[list][scan8[4]];
            ref_index[0 + 1 * h->b8_stride] = h->ref_cache[list][scan8[8]];
            ref_index[1 + 1 * h->b8_stride] = h->ref_cache[list][scan8[12]];
        }
    }

    if (h->slice_type_nos == FF_B_TYPE && h->pps.cabac)
    {
        if (IS_8X8(mb_type))
        {
            uint8_t *direct_table = &h->direct_table[b8_xy];
            direct_table[1 + 0 * h->b8_stride] = IS_DIRECT(h->sub_mb_type[1]) ? 1 : 0;
            direct_table[0 + 1 * h->b8_stride] = IS_DIRECT(h->sub_mb_type[2]) ? 1 : 0;
            direct_table[1 + 1 * h->b8_stride] = IS_DIRECT(h->sub_mb_type[3]) ? 1 : 0;
        }
    }
}

const uint8_t *ff_h264_decode_nal(H264Context *h, const uint8_t *src, int *dst_length, int *consumed, int length)
{
    int i, si, di;
    uint8_t *dst;
    int bufidx;

    //    src[0]&0x80;                //forbidden bit
    h->nal_ref_idc = src[0] >> 5;
    h->nal_unit_type = src[0] & 0x1F;

    src++;
    length--;

#if HAVE_FAST_UNALIGNED
# if HAVE_FAST_64BIT
#   define RS 7

    for (i = 0; i + 1 < length; i += 9)
    {
        if (!((~ * (const uint64_t *)(src + i) & (*(const uint64_t *)(src + i) - 0x0100010001000101ULL)) & 0x8000800080008080ULL))
# else
#   define RS 3
    for (i = 0; i + 1 < length; i += 5)
    {
        if (!((~ * (const uint32_t *)(src + i) & (*(const uint32_t *)(src + i) - 0x01000101U)) & 0x80008080U))
# endif
            continue;

        if (i > 0 && !src[i]) { i--; }

        while (src[i]) { i++; }

#else
#   define RS 0

    for (i = 0; i + 1 < length; i += 2)
    {
        if (src[i]) { continue; }

        if (i > 0 && src[i - 1] == 0) { i--; }

#endif

        if (i + 2 < length && src[i + 1] == 0 && src[i + 2] <= 3)
        {
            if (src[i + 2] != 3)
            {
                /* startcode, so we must be past the end */
                length = i;
            }

            break;
        }

        i -= RS;
    }

    if (i >= length - 1) //no escaped 0
    {
        *dst_length = length;
        *consumed = length + 1; //+1 for the header
        return src;
    }

    bufidx = h->nal_unit_type == NAL_DPC ? 1 : 0; // use second escape buffer for inter data

    /* �޸Ĵ��루sdk) */
    // h->rbsp_buffer[bufidx]= av_fast_realloc(h->rbsp_buffer[bufidx], &h->rbsp_buffer_size[bufidx], length+FF_INPUT_BUFFER_PADDING_SIZE);
    if (length + FF_INPUT_BUFFER_PADDING_SIZE > (int)h->rbsp_buffer_size[bufidx])
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "dec_ctx[%p] rbsp_buffer_size%d[%d] is less than current bitstream size[%d]\n,", h->s.avctx, bufidx, h->rbsp_buffer_size[bufidx], length + FF_INPUT_BUFFER_PADDING_SIZE);
        //		return NULL;
        length = h->rbsp_buffer_size[bufidx];
        h->s.avctx->iTotalError++;
    }

    dst = h->rbsp_buffer[bufidx];

    if (dst == NULL)
    {
        return NULL;
    }

    memcpy(dst, src, i);
    si = di = i;

    while (si + 2 < length)
    {
        //remove escapes (very rare 1:2^22)
        if (src[si + 2] > 3)
        {
            dst[di++] = src[si++];
            dst[di++] = src[si++];
        }
        else if (src[si] == 0 && src[si + 1] == 0)
        {
            if (src[si + 2] == 3) //escape
            {
                dst[di++] = 0;
                dst[di++] = 0;
                si += 3;
                continue;
            }
            else  //next start code
            { goto nsc; }
        }

        dst[di++] = src[si++];
    }

    while (si < length)
    { dst[di++] = src[si++]; }

nsc:

    memset(dst + di, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    *dst_length = di;
    *consumed = si + 1; //+1 for the header
    //FIXME store exact number of bits in the getbitcontext (it is needed for decoding)
    return dst;
}

int ff_h264_decode_rbsp_trailing(H264Context *h, const uint8_t *src)
{
    int v = *src;
    int r;

    tprintf(h->s.avctx, "rbsp trailing %X\n", v);

    for (r = 1; r < 9; r++)
    {
        if (v & 1) { return r; }

        v >>= 1;
    }

    return 0;
}

/**
 * IDCT transforms the 16 dc values and dequantizes them.
 * @param qp quantization parameter
 */
static void h264_luma_dc_dequant_idct_c(DCTELEM *block, int qp, int qmul)
{
#define stride 16
    int i;
    int temp[16]; //FIXME check if this is a good idea
    static const int x_offset[4] = {0, 1 * stride, 4 * stride,  5 * stride};
    static const int y_offset[4] = {0, 2 * stride, 8 * stride, 10 * stride};

    //memset(block, 64, 2*256);
    //return;
    for (i = 0; i < 4; i++)
    {
        const int offset = y_offset[i];
        const int z0 = block[offset + stride * 0] + block[offset + stride * 4];
        const int z1 = block[offset + stride * 0] - block[offset + stride * 4];
        const int z2 = block[offset + stride * 1] - block[offset + stride * 5];
        const int z3 = block[offset + stride * 1] + block[offset + stride * 5];

        temp[4 * i + 0] = z0 + z3;
        temp[4 * i + 1] = z1 + z2;
        temp[4 * i + 2] = z1 - z2;
        temp[4 * i + 3] = z0 - z3;
    }

    for (i = 0; i < 4; i++)
    {
        const int offset = x_offset[i];
        const int z0 = temp[4 * 0 + i] + temp[4 * 2 + i];
        const int z1 = temp[4 * 0 + i] - temp[4 * 2 + i];
        const int z2 = temp[4 * 1 + i] - temp[4 * 3 + i];
        const int z3 = temp[4 * 1 + i] + temp[4 * 3 + i];

        block[stride * 0 + offset] = ((((z0 + z3) * qmul + 128 ) >> 8)); //FIXME think about merging this into decode_residual
        block[stride * 2 + offset] = ((((z1 + z2) * qmul + 128 ) >> 8));
        block[stride * 8 + offset] = ((((z1 - z2) * qmul + 128 ) >> 8));
        block[stride * 10 + offset] = ((((z0 - z3) * qmul + 128 ) >> 8));
    }
}

#if 0
/**
 * DCT transforms the 16 dc values.
 * @param qp quantization parameter ??? FIXME
 */
static void h264_luma_dc_dct_c(DCTELEM *block/*, int qp*/)
{
    //    const int qmul= dequant_coeff[qp][0];
    int i;
    int temp[16]; //FIXME check if this is a good idea
    static const int x_offset[4] = {0, 1 * stride, 4 * stride,  5 * stride};
    static const int y_offset[4] = {0, 2 * stride, 8 * stride, 10 * stride};

    for (i = 0; i < 4; i++)
    {
        const int offset = y_offset[i];
        const int z0 = block[offset + stride * 0] + block[offset + stride * 4];
        const int z1 = block[offset + stride * 0] - block[offset + stride * 4];
        const int z2 = block[offset + stride * 1] - block[offset + stride * 5];
        const int z3 = block[offset + stride * 1] + block[offset + stride * 5];

        temp[4 * i + 0] = z0 + z3;
        temp[4 * i + 1] = z1 + z2;
        temp[4 * i + 2] = z1 - z2;
        temp[4 * i + 3] = z0 - z3;
    }

    for (i = 0; i < 4; i++)
    {
        const int offset = x_offset[i];
        const int z0 = temp[4 * 0 + i] + temp[4 * 2 + i];
        const int z1 = temp[4 * 0 + i] - temp[4 * 2 + i];
        const int z2 = temp[4 * 1 + i] - temp[4 * 3 + i];
        const int z3 = temp[4 * 1 + i] + temp[4 * 3 + i];

        block[stride * 0 + offset] = (z0 + z3) >> 1;
        block[stride * 2 + offset] = (z1 + z2) >> 1;
        block[stride * 8 + offset] = (z1 - z2) >> 1;
        block[stride * 10 + offset] = (z0 - z3) >> 1;
    }
}
#endif

#undef xStride
#undef stride

static void chroma_dc_dequant_idct_c(DCTELEM *block, int qp, int qmul)
{
    const int stride = 16 * 2;
    const int xStride = 16;
    int a, b, c, d, e;

    a = block[stride * 0 + xStride * 0];
    b = block[stride * 0 + xStride * 1];
    c = block[stride * 1 + xStride * 0];
    d = block[stride * 1 + xStride * 1];

    e = a - b;
    a = a + b;
    b = c - d;
    c = c + d;

    block[stride * 0 + xStride * 0] = ((a + c) * qmul) >> 7;
    block[stride * 0 + xStride * 1] = ((e + b) * qmul) >> 7;
    block[stride * 1 + xStride * 0] = ((a - c) * qmul) >> 7;
    block[stride * 1 + xStride * 1] = ((e - b) * qmul) >> 7;
}

#if 0
static void chroma_dc_dct_c(DCTELEM *block)
{
    const int stride = 16 * 2;
    const int xStride = 16;
    int a, b, c, d, e;

    a = block[stride * 0 + xStride * 0];
    b = block[stride * 0 + xStride * 1];
    c = block[stride * 1 + xStride * 0];
    d = block[stride * 1 + xStride * 1];

    e = a - b;
    a = a + b;
    b = c - d;
    c = c + d;

    block[stride * 0 + xStride * 0] = (a + c);
    block[stride * 0 + xStride * 1] = (e + b);
    block[stride * 1 + xStride * 0] = (a - c);
    block[stride * 1 + xStride * 1] = (e - b);
}
#endif

/**
 * gets the chroma qp.
 */
static inline int get_chroma_qp(H264Context *h, int t, int qscale)
{
    return h->pps.chroma_qp_table[t][qscale];
}

/* src_x_offset: ���ڵ�ǰ�Ӻ���ڲο�ͼ���е�x�����ȫ�ֵ�ַ[�����chroma����0~pic_width/2-1]
   src_y_offset: ���ڵ�ǰ�Ӻ���ڲο�ͼ���е�y�����ȫ�ֵ�ַ[�����chroma����0~pic_height/2-1] liuxw+00139685 */

static inline int mc_dir_part(H264Context *h, Picture *pic, int n, int square, int chroma_height, int delta, int list,
                              uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                              int src_x_offset, int src_y_offset,
                              qpel_mc_func *qpix_op, h264_chroma_mc_func chroma_op)
{
    MpegEncContext *const s = &h->s;
    const int mx = h->mv_cache[list][ scan8[n] ][0] + src_x_offset * 8;  /* �õ�Ԥ���Y����x�������ڲο�ͼ���е�ȫ�ֵ�ַ��1/4���ص�λ����src_x_offset*2*4��
																	     *2��ʾ��chroma��ƫ��ת��ΪY��ƫ�ƣ�*4��ʾ���뵽1/4���ص�λ liuxw+00139685 */
    int my =       h->mv_cache[list][ scan8[n] ][1] + src_y_offset * 8;  /* �õ�Ԥ���Y����y�������ڲο�ͼ���е�ȫ�ֵ�ַ��src_y_offset*2*4��
																	  *2��ʾ��chroma��ƫ��ת��ΪY��ƫ�ƣ�*4��ʾ���뵽1/4���ص�λ liuxw+00139685 */
    const int luma_xy = (mx & 3) + ((my & 3) << 2);	/* ��ֵ����ָ����������� liuxw+00139685 */
    uint8_t *src_y = pic->data[0] + (mx >> 2) + (my >> 2) * h->mb_linesize; /* ��ǰ���y������Դ��ַ liuxw+00139685 */
    uint8_t *src_cb, * src_cr;
    int extra_width = h->emu_edge_width;   /* padding���(16) liuxw+00139685 */
    int extra_height = h->emu_edge_height; /* padding�߶�(16) liuxw+00139685 */
    int emu = 0;
    const int full_mx = mx >> 2; /* ��ǰ��Y����x�����ڲο�ͼ���е�ȫ�ֵ�ַ(�����ص�λ) liuxw+00139685 */
    const int full_my = my >> 2; /* ��ǰ��Y����y�����ڲο�ͼ���е�ȫ�ֵ�ַ(�����ص�λ) liuxw+00139685 */
    const int pic_width  = 16 * s->mb_width;
    const int pic_height = (16 * s->mb_height) >> MB_FIELD; /* ���ǵ�ǰ����ǳ���飬��ͼ��߶�ת��Ϊ��ͼ��ĸ߶ȣ�������2�� liuxw+00139685 */
    int iRet = 0;

    if (mx & 7) { extra_width -= 3; }

    if (my & 7) { extra_height -= 3; }

    /* 2010/04/06 14:30:00 liuxw+00139685 */
    /* ���Ӷ�mv�Ϸ��Եļ��(avoid segment) */
    if (NULL == pic->data[0] ||  NULL == pic->data[1] || NULL == pic->data[2])
    {
        av_log(s->avctx, AV_LOG_WARNING, "source data is NULL!\n");
        s->avctx->iTotalError++;
        return -1;
    }

    /* ???
    	liuxw+00139685 */
    if (   full_mx < 0 - extra_width
           || full_my < 0 - extra_height
           || full_mx + 16/*FIXME*/ > pic_width + extra_width
           || full_my + 16/*FIXME*/ > pic_height + extra_height)
    {
        ff_emulated_edge_mc(s->edge_emu_buffer, src_y - 2 - 2 * h->mb_linesize, h->mb_linesize, 16 + 5, 16 + 5/*FIXME*/, full_mx - 2, full_my - 2, pic_width, pic_height);
        src_y = s->edge_emu_buffer + 2 + 2 * h->mb_linesize;
        emu = 1;
    }

#ifdef LXW_DEBUG

    if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
    {
        int i , j;
        uint8_t *src_y_tmp = src_y;
        av_log(s->avctx, AV_LOG_ERROR, "predict value[integer]: POC[%d] mb_x[%d] mb_y[%d]!\n", s->current_picture_ptr->field_poc[0], s->mb_x, s->mb_y);

        for (i = 0; i < 16; i++)
        {
            for (j = 0; j < 16; j++)
            { av_log(s->avctx, AV_LOG_ERROR, "%3d ", src_y_tmp[j]); }

            av_log(s->avctx, AV_LOG_ERROR, "\n");
            src_y_tmp += h->mb_linesize;
        }
    }

#endif
    /* ����luma_xy���ö�Ӧ�Ĳ�ֵ�����Ե�ǰ�������Ӧ�Ĳ�ֵ liuxw+00139685 */
    qpix_op[luma_xy](dest_y, src_y, h->mb_linesize); //FIXME try variable height perhaps?

    /* �����ǰ�ӿ鲻��һ��������(16x16��8x8��4x4),��Ὣ�����ηֽ������������(16x8�ֽ������8x8�����ֱ�������������ν��в�ֵ liuxw+00139685 */
    if (!square)
    {
        qpix_op[luma_xy](dest_y + delta, src_y + delta, h->mb_linesize);
    }

#ifdef LXW_DEBUG

    if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
    {
        av_log(s->avctx, AV_LOG_ERROR, "CONFIG_GRAY:%d s->flags:%d CODEC_FLAG_GRAY:%d s->flags&CODEC_FLAG_GRAY:%d!\n",
               CONFIG_GRAY, s->flags, CODEC_FLAG_GRAY, s->flags & CODEC_FLAG_GRAY);
    }

#endif

    if (CONFIG_GRAY && s->flags & CODEC_FLAG_GRAY) { return iRet; }

    if (MB_FIELD)
    {
        // chroma offset when predicting from a field of opposite parity
        /* ��������ο��׳�����my + =-2;����ǵ׳��ο�����,��my += 2(ԭ���ݲ������liuxw+00139685 */
        my += 2 * ((s->mb_y & 1) - (pic->reference - 1));
        /* ???
        liuxw+00139685 */
        emu |= (my >> 3) < 0 || (my >> 3) + 8 >= (pic_height >> 1);
    }

    /* chroma��1/8���ؾ��� liuxw+00139685 */
    src_cb = pic->data[1] + (mx >> 3) + (my >> 3) * h->mb_uvlinesize;	/* ��ǰ���u������Դ��ַ liuxw+00139685 */
    src_cr = pic->data[2] + (mx >> 3) + (my >> 3) * h->mb_uvlinesize;	/* ��ǰ���v������Դ��ַ liuxw+00139685 */

    if (emu)
    {
        ff_emulated_edge_mc(s->edge_emu_buffer, src_cb, h->mb_uvlinesize, 9, 9/*FIXME*/, (mx >> 3), (my >> 3), pic_width >> 1, pic_height >> 1);
        src_cb = s->edge_emu_buffer;
    }

    chroma_op(dest_cb, src_cb, h->mb_uvlinesize, chroma_height, mx & 7, my & 7);

    if (emu)
    {
        ff_emulated_edge_mc(s->edge_emu_buffer, src_cr, h->mb_uvlinesize, 9, 9/*FIXME*/, (mx >> 3), (my >> 3), pic_width >> 1, pic_height >> 1);
        src_cr = s->edge_emu_buffer;
    }

    chroma_op(dest_cr, src_cr, h->mb_uvlinesize, chroma_height, mx & 7, my & 7);

#ifdef LXW_DEBUG

    if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
    {
        int i , j;
        uint8_t *src_y_tmp = dest_y;
        av_log(s->avctx, AV_LOG_ERROR, "predict value[decimal]: POC[%d] mb_x[%d] mb_y[%d]!\n", s->current_picture_ptr->field_poc[0], s->mb_x, s->mb_y);

        for (i = 0; i < 16; i++)
        {
            for (j = 0; j < 16; j++)
            { av_log(s->avctx, AV_LOG_ERROR, "%3d ", src_y_tmp[j]); }

            av_log(s->avctx, AV_LOG_ERROR, "\n");
            src_y_tmp += h->mb_linesize;
        }
    }

#endif

    return iRet;
}

/* n: �����õ���ǰ�Ӻ��Ĳο�������		square: 16x16 8x8 4x4 -> 1 others -> 0
   chroma_heigth: ��ǰ�Ӻ���ɫ�ȵĸ߶ȣ�   delta: ��square=1ʱ��delta=0;square=0ʱ,delta���ڵ�ǰ�Ӻ���ڵڶ��������ο����һ�������ο��ƫ��
   ��16x8,�����16x8���Էֳ�����8x8�������Σ��ڶ������ڵ�һ����ƫ�ƣ�8����
	x_offset����ǰ����ڲ��Ӻ���x�����ƫ�ƣ�����chroma���Ե�ƫ�ƣ�(16x8,�����16x8����chroma���Զ��ڵ�ǰ����ƫ��x_offset=0,y_offset=0)
	y_offset����ǰ����ڲ��Ӻ���y�����ƫ�ƣ�����chroma���Ե�ƫ�ƣ�(16x8,�����16x8����chroma���Զ��ڵ�ǰ����ƫ��x_offset=0,y_offset=4)
	liuxw+00139685 */
static inline int mc_part_std(H264Context *h, int n, int square, int chroma_height, int delta,
                              uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                              int x_offset, int y_offset,
                              qpel_mc_func *qpix_put, h264_chroma_mc_func chroma_put,
                              qpel_mc_func *qpix_avg, h264_chroma_mc_func chroma_avg,
                              int list0, int list1)
{
    MpegEncContext *const s = &h->s;
    qpel_mc_func *qpix_op =  qpix_put;
    h264_chroma_mc_func chroma_op = chroma_put;
    int iRet = 0;

    dest_y  += 2 * x_offset + 2 * y_offset * h->mb_linesize; /* �õ���ǰ�Ӻ��Y�ĵ�ַ liuxw+00139685 */
    dest_cb +=   x_offset +   y_offset * h->mb_uvlinesize; /* �õ���ǰ�Ӻ��U�ĵ�ַ liuxw+00139685 */
    dest_cr +=   x_offset +   y_offset * h->mb_uvlinesize; /* �õ���ǰ�Ӻ��V�ĵ�ַ liuxw+00139685 */
    x_offset += 8 * s->mb_x;								 /* x_offset���ڵ�ǰ�Ӻ���ڲο�ͼ���е�x�����ȫ�ֵ�ַ[�����chroma����0~pic_width/2-1] liuxw+00139685 */
    /* �����ǰ�������MBAFF������ĳ���飬��ʵ��ͼ���е�mb_y����2��ת��������Ӧ��mb_y liuxw+00139685 */
    y_offset += 8 * (s->mb_y >> MB_FIELD);				 /* y_offset���ڵ�ǰ�Ӻ���ڲο�ͼ���е�y�����ȫ�ֵ�ַ[�����chroma����0~pic_height/2-1] liuxw+00139685 */

    if (list0)
    {
        /* 2010/06/02 19:00:00 liuxw+00139685 */
        /* ���Ӷ�h->ref_cache[0][ scan8[n] ]�Ϸ�ֵ���ж� */
        //      Picture *ref = &h->ref_list[0][ h->ref_cache[0][ scan8[n] ] ];
        Picture *ref = NULL;

        if (h->ref_cache[0][ scan8[n] ] < 0 || ((MB_MBAFF && MB_FIELD) ? (h->ref_cache[0][ scan8[n] ] >= (int)(16 + h->ref_count[0] * 2)) : (h->ref_cache[0][ scan8[n] ] >= (int)h->ref_count[0])))
        {
            av_log(NULL, AV_LOG_WARNING, "list0 ref index[%d] of MB[mb_x:%d mb_y:%d] is exceed range!\n", h->ref_cache[0][ scan8[n] ], h->s.mb_x, h->s.mb_y);
            s->avctx->iTotalError++;
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
            return -1;
        }

        ref  = &h->ref_list[0][ h->ref_cache[0][ scan8[n] ] ];
        iRet = mc_dir_part(h, ref, n, square, chroma_height, delta, 0,
                           dest_y, dest_cb, dest_cr, x_offset, y_offset,
                           qpix_op, chroma_op);

        if (iRet < 0)
        {
            return -1;
        }

        qpix_op =  qpix_avg;
        chroma_op = chroma_avg;
    }

    if (list1)
    {
        /* 2010/06/02 19:00:00 liuxw+00139685 */
        /* ���Ӷ�h->ref_cache[0][ scan8[n] ]�Ϸ�ֵ���ж� */
        //      Picture *ref= &h->ref_list[1][ h->ref_cache[1][ scan8[n] ] ];
        Picture *ref = NULL;

        if (h->ref_cache[1][ scan8[n] ] < 0 || ((MB_MBAFF && MB_FIELD) ? (h->ref_cache[1][ scan8[n] ] >= (int)(16 + h->ref_count[1] * 2)) : (h->ref_cache[1][ scan8[n] ] >= (int)h->ref_count[1])))
        {
            av_log(NULL, AV_LOG_WARNING, "list1 ref index[%d] of MB[mb_x:%d mb_y:%d] is exceed range!\n", h->ref_cache[1][ scan8[n] ], h->s.mb_x, h->s.mb_y);
            s->avctx->iTotalError++;
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
            return -1;
        }

        ref  = &h->ref_list[1][ h->ref_cache[1][ scan8[n] ] ];
        iRet = mc_dir_part(h, ref, n, square, chroma_height, delta, 1,
                           dest_y, dest_cb, dest_cr, x_offset, y_offset,
                           qpix_op, chroma_op);

        if (iRet < 0)
        {
            return -1;
        }
    }

    return iRet;
}

static inline int mc_part_weighted(H264Context *h, int n, int square, int chroma_height, int delta,
                                   uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                                   int x_offset, int y_offset,
                                   qpel_mc_func *qpix_put, h264_chroma_mc_func chroma_put,
                                   h264_weight_func luma_weight_op, h264_weight_func chroma_weight_op,
                                   h264_biweight_func luma_weight_avg, h264_biweight_func chroma_weight_avg,
                                   int list0, int list1)
{
    MpegEncContext *const s = &h->s;
    int iRet = 0;

    dest_y  += 2 * x_offset + 2 * y_offset * h->  mb_linesize;
    dest_cb +=   x_offset +   y_offset * h->mb_uvlinesize;
    dest_cr +=   x_offset +   y_offset * h->mb_uvlinesize;
    x_offset += 8 * s->mb_x;
    y_offset += 8 * (s->mb_y >> MB_FIELD);

    /* ����˫��Ԥ�⣨B��ʾ��Ȩ����ʽ��Ȩ��liuxw+00139685 */
    if (list0 && list1)
    {
        /* don't optimize for luma-only case, since B-frames usually
         * use implicit weights => chroma too. */
        uint8_t *tmp_cb = s->obmc_scratchpad;
        uint8_t *tmp_cr = s->obmc_scratchpad + 8;
        uint8_t *tmp_y  = s->obmc_scratchpad + 8 * h->mb_uvlinesize;
        int refn0 = h->ref_cache[0][ scan8[n] ];
        int refn1 = h->ref_cache[1][ scan8[n] ];

        /* 2010/06/03 14:30:00 liuxw+00139685 */
        /* ��refn0/refn1���кϷ����ж� */
        if (refn0 < 0 || ((MB_MBAFF && MB_FIELD) ? (refn0 >= (int)(16 + h->ref_count[0] * 2)) : (refn0 >= (int)h->ref_count[0])))
        {
            av_log(NULL, AV_LOG_WARNING, "list0 ref index[%d] of MB[mb_x:%d mb_y:%d] is exceed range!\n", refn0, h->s.mb_x, h->s.mb_y);
            s->avctx->iTotalError++;
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
            return -1;
        }

        if (refn1 < 0 || ((MB_MBAFF && MB_FIELD) ? (refn1 >= (int)(16 + h->ref_count[1] * 2)) : (refn1 >= (int)h->ref_count[1])))
        {
            av_log(NULL, AV_LOG_WARNING, "list0 ref index[%d] of MB[mb_x:%d mb_y:%d] is exceed range!\n", refn1, h->s.mb_x, h->s.mb_y);
            s->avctx->iTotalError++;
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
            return -1;
        }

        /* ����(list0)Ԥ�⣺�õ�δ��Ȩǰ��Ԥ��ֵ��1/4���أ�liuxw+00139685 */
        iRet = mc_dir_part(h, &h->ref_list[0][refn0], n, square, chroma_height, delta, 0,
                           dest_y, dest_cb, dest_cr, x_offset, y_offset, qpix_put, chroma_put);

        if (iRet < 0)
        { return -1; }

        /* ǰ��(list1)Ԥ�⣺�õ�δ��Ȩǰ��Ԥ��ֵ��1/4���أ�liuxw+00139685 */
        iRet = mc_dir_part(h, &h->ref_list[1][refn1], n, square, chroma_height, delta, 1,
                           tmp_y, tmp_cb, tmp_cr, x_offset, y_offset, qpix_put, chroma_put);

        if (iRet < 0)
        { return -1; }

        /* ��ʽԤ�� liuxw+00139685 */
        if (h->use_weight == 2)
        {
            int weight0 = h->implicit_weight[TOP_OR_BOT_FLD_MB][refn0][refn1];
            int weight1 = 64 - weight0;

#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
            { av_log(h->s.avctx, AV_LOG_WARNING, "implicit weight: ref0:%d ref1:%d weight0:%d weight1:%d\n", refn0, refn1, weight0, weight1); }

#endif

            /* �õ���Ȩ���Ԥ��ֵ liuxw+00139685 */
            luma_weight_avg(  dest_y,  tmp_y,  h->  mb_linesize, 5, weight0, weight1, 0);

            if (CHROMA)
            {
                chroma_weight_avg(dest_cb, tmp_cb, h->mb_uvlinesize, 5, weight0, weight1, 0);
                chroma_weight_avg(dest_cr, tmp_cr, h->mb_uvlinesize, 5, weight0, weight1, 0);
            }
        }
        /* ��ʾԤ�� liuxw+00139685 */
        else
        {

#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
                av_log(h->s.avctx, AV_LOG_WARNING, "explicit weight: luma_log2_weight_denom:%d  refn0:%d luma_weight:%d  luma_offset:%d  refn1:%d luma_weight:%d luma_offset:%d\n",
                       h->luma_log2_weight_denom, refn0, h->luma_weight[0][refn0], h->luma_offset[0][refn0], refn1, h->luma_weight[1][refn1], h->luma_offset[1][refn1]);

#endif

            /* �õ���Ȩ���Ԥ��ֵ liuxw+00139685 */
            luma_weight_avg(dest_y, tmp_y, h->mb_linesize, h->luma_log2_weight_denom,
                            h->luma_weight[0][refn0], h->luma_weight[1][refn1],
                            h->luma_offset[0][refn0] + h->luma_offset[1][refn1]);

            /* 2010/03/08 19:32:00 liuxw+00139685 */
            /* ���Ӷ�ɫ�ȼ�Ȩ������ */
            if (CHROMA)
            {
                /* 2010/07/05 16:30:00 liuxw+00139685 */
                /* ���Ӷ���ʾ��Ȩ(w0+w1)�Ϸ��Ե��ж� */
                if ((h->chroma_weight[0][refn0][0] + h->chroma_weight[1][refn1][0]) > ((h->chroma_log2_weight_denom == 7) ? 128 : 129) || (h->chroma_weight[0][refn0][0] + h->chroma_weight[1][refn1][0]) < -128)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "B explicit weight Cb component: Toatl[%d] of w0[%d]+w1[%d] is exceed valid range[%d-%d]\n",
                           h->chroma_weight[0][refn0][0] + h->chroma_weight[1][refn1][0], h->chroma_weight[0][refn0][0], h->chroma_weight[1][refn1][0], -128, (h->chroma_log2_weight_denom == 7) ? 127 : 128);
                    s->avctx->iTotalError++;
                    IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_WEIGHTED);
                }

                if ((h->chroma_weight[0][refn0][1] + h->chroma_weight[1][refn1][1]) > ((h->chroma_log2_weight_denom == 7) ? 128 : 129) || (h->chroma_weight[0][refn0][1] + h->chroma_weight[1][refn1][1]) < -128)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "B explicit weight Cr component: Toatl[%d] of w0[%d]+w1[%d] is exceed valid range[%d-%d]\n",
                           h->chroma_weight[0][refn0][1] + h->chroma_weight[1][refn1][1], h->chroma_weight[0][refn0][1], h->chroma_weight[1][refn1][1], -128, (h->chroma_log2_weight_denom == 7) ? 127 : 128);
                    s->avctx->iTotalError++;
                    IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_WEIGHTED);
                }

                chroma_weight_avg(dest_cb, tmp_cb, h->mb_uvlinesize, h->chroma_log2_weight_denom,
                                  h->chroma_weight[0][refn0][0], h->chroma_weight[1][refn1][0],
                                  h->chroma_offset[0][refn0][0] + h->chroma_offset[1][refn1][0]);
                chroma_weight_avg(dest_cr, tmp_cr, h->mb_uvlinesize, h->chroma_log2_weight_denom,
                                  h->chroma_weight[0][refn0][1], h->chroma_weight[1][refn1][1],
                                  h->chroma_offset[0][refn0][1] + h->chroma_offset[1][refn1][1]);
            }
        }
    }
    /* ����Ԥ�⣨P��ʾ��Ȩ��B��ʾ����(list0)��Ȩ��B��ʾǰ��(list1)��Ȩ liuxw+00139685 */
    else
    {
        int list = list1 ? 1 : 0;	/* �õ�Ԥ�ⷽ�� liuxw+00139685 */
        int refn = h->ref_cache[list][ scan8[n] ];
        /* 2010/06/03 14:30:00 liuxw+00139685 */
        /* ��refn���кϷ����ж� */
        //      Picture *ref= &h->ref_list[list][refn];
        Picture *ref = NULL;

        if (refn < 0 || ((MB_MBAFF && MB_FIELD) ? (refn >= (int)(16 + h->ref_count[list] * 2)) : (refn >= (int)h->ref_count[list])))
        {
            av_log(NULL, AV_LOG_WARNING, "list%d ref index[%d] of MB[mb_x:%d mb_y:%d] is exceed range!\n", list, refn, h->s.mb_x, h->s.mb_y);
            s->avctx->iTotalError++;
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
        }

        ref = &h->ref_list[list][refn];
        /* �õ�δ��Ȩǰ��Ԥ��ֵ��1/4���أ�liuxw+00139685 */
        iRet = mc_dir_part(h, ref, n, square, chroma_height, delta, list,
                           dest_y, dest_cb, dest_cr, x_offset, y_offset, qpix_put, chroma_put);

        if (iRet < 0)
        { return -1; }

        /* lumaԤ������ֵ��Ȩ liuxw+00139685 */
        luma_weight_op(dest_y, h->mb_linesize, h->luma_log2_weight_denom,
                       h->luma_weight[list][refn], h->luma_offset[list][refn]);

        /* ɫ��Ҳ�����˼�Ȩ liuxw+00139685 */
        if (CHROMA)
        {
            /* cb Ԥ������ֵ��Ȩ liuxw+00139685 */
            chroma_weight_op(dest_cb, h->mb_uvlinesize, h->chroma_log2_weight_denom,
                             h->chroma_weight[list][refn][0], h->chroma_offset[list][refn][0]);
            /* cb Ԥ������ֵ��Ȩ liuxw+00139685 */
            chroma_weight_op(dest_cr, h->mb_uvlinesize, h->chroma_log2_weight_denom,
                             h->chroma_weight[list][refn][1], h->chroma_offset[list][refn][1]);
        }
    }

    return iRet;
}

static inline int mc_part(H264Context *h, int n, int square, int chroma_height, int delta,
                          uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                          int x_offset, int y_offset,
                          qpel_mc_func *qpix_put, h264_chroma_mc_func chroma_put,
                          qpel_mc_func *qpix_avg, h264_chroma_mc_func chroma_avg,
                          h264_weight_func *weight_op, h264_biweight_func *weight_avg,
                          int list0, int list1)
{
    int iRet = 0;

    if ((h->use_weight == 2 && list0 && list1
         && (h->implicit_weight[TOP_OR_BOT_FLD_MB][ h->ref_cache[0][scan8[n]] ][ h->ref_cache[1][scan8[n]] ] != 32))
        || h->use_weight == 1)
        iRet = mc_part_weighted(h, n, square, chroma_height, delta, dest_y, dest_cb, dest_cr,
                                x_offset, y_offset, qpix_put, chroma_put,
                                weight_op[0], weight_op[3], weight_avg[0], weight_avg[3], list0, list1);
    else
        iRet =  mc_part_std(h, n, square, chroma_height, delta, dest_y, dest_cb, dest_cr,
                            x_offset, y_offset, qpix_put, chroma_put, qpix_avg, chroma_avg, list0, list1);

    if (iRet < 0)
    { return -1; }

    return iRet;
}

static inline void prefetch_motion(H264Context *h, int list)
{
    /* fetch pixels for estimated mv 4 macroblocks ahead
     * optimized for 64byte cache lines */
    MpegEncContext *const s = &h->s;
    const int refn = h->ref_cache[list][scan8[0]];

    if (refn >= 0)
    {
        const int mx = (h->mv_cache[list][scan8[0]][0] >> 2) + 16 * s->mb_x + 8;
        const int my = (h->mv_cache[list][scan8[0]][1] >> 2) + 16 * s->mb_y;
        uint8_t **src = h->ref_list[list][refn].data;
        int off = mx + (my + (s->mb_x & 3) * 4) * h->mb_linesize + 64;
        s->dsp.prefetch(src[0] + off, s->linesize, 4);
        off = (mx >> 1) + ((my >> 1) + (s->mb_x & 7)) * s->uvlinesize + 64;
        s->dsp.prefetch(src[1] + off, src[2] - src[1], 2);
    }
}

/* mc: �õ�֡��Ԥ��ֵ liuxw+00139685 */
static int hl_motion(H264Context *h, uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                     qpel_mc_func (*qpix_put)[16], h264_chroma_mc_func (*chroma_put),
                     qpel_mc_func (*qpix_avg)[16], h264_chroma_mc_func (*chroma_avg),
                     h264_weight_func *weight_op, h264_biweight_func *weight_avg)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = h->mb_xy;
    const int mb_type = s->current_picture.mb_type[mb_xy];
    int iRet = 0;

    assert(IS_INTER(mb_type));

    prefetch_motion(h, 0);

    if (IS_16X16(mb_type))
    {
        iRet = mc_part(h, 0, 1, 8, 0, dest_y, dest_cb, dest_cr, 0, 0,
                       qpix_put[0], chroma_put[0], qpix_avg[0], chroma_avg[0],
                       &weight_op[0], &weight_avg[0],
                       IS_DIR(mb_type, 0, 0), IS_DIR(mb_type, 0, 1));

        if (iRet < 0) { return -1; }
    }
    else if (IS_16X8(mb_type))
    {
        iRet = mc_part(h, 0, 0, 4, 8, dest_y, dest_cb, dest_cr, 0, 0,
                       qpix_put[1], chroma_put[0], qpix_avg[1], chroma_avg[0],
                       &weight_op[1], &weight_avg[1],
                       IS_DIR(mb_type, 0, 0), IS_DIR(mb_type, 0, 1));

        if (iRet < 0) { return -1; }

        iRet = mc_part(h, 8, 0, 4, 8, dest_y, dest_cb, dest_cr, 0, 4,
                       qpix_put[1], chroma_put[0], qpix_avg[1], chroma_avg[0],
                       &weight_op[1], &weight_avg[1],
                       IS_DIR(mb_type, 1, 0), IS_DIR(mb_type, 1, 1));

        if (iRet < 0) { return -1; }
    }
    else if (IS_8X16(mb_type))
    {
        iRet = mc_part(h, 0, 0, 8, 8 * h->mb_linesize, dest_y, dest_cb, dest_cr, 0, 0,
                       qpix_put[1], chroma_put[1], qpix_avg[1], chroma_avg[1],
                       &weight_op[2], &weight_avg[2],
                       IS_DIR(mb_type, 0, 0), IS_DIR(mb_type, 0, 1));

        if (iRet < 0) { return -1; }

        iRet = mc_part(h, 4, 0, 8, 8 * h->mb_linesize, dest_y, dest_cb, dest_cr, 4, 0,
                       qpix_put[1], chroma_put[1], qpix_avg[1], chroma_avg[1],
                       &weight_op[2], &weight_avg[2],
                       IS_DIR(mb_type, 1, 0), IS_DIR(mb_type, 1, 1));

        if (iRet < 0) { return -1; }
    }
    else
    {
        int i;

        assert(IS_8X8(mb_type));

        for (i = 0; i < 4; i++)
        {
            const int sub_mb_type = h->sub_mb_type[i];
            const int n = 4 * i;
            int x_offset = (i & 1) << 2;
            int y_offset = (i & 2) << 1;

            if (IS_SUB_8X8(sub_mb_type))
            {
                iRet = mc_part(h, n, 1, 4, 0, dest_y, dest_cb, dest_cr, x_offset, y_offset,
                               qpix_put[1], chroma_put[1], qpix_avg[1], chroma_avg[1],
                               &weight_op[3], &weight_avg[3],
                               IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

                if (iRet < 0) { return -1; }
            }
            else if (IS_SUB_8X4(sub_mb_type))
            {
                iRet = mc_part(h, n  , 0, 2, 4, dest_y, dest_cb, dest_cr, x_offset, y_offset,
                               qpix_put[2], chroma_put[1], qpix_avg[2], chroma_avg[1],
                               &weight_op[4], &weight_avg[4],
                               IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

                if (iRet < 0) { return -1; }

                iRet = mc_part(h, n + 2, 0, 2, 4, dest_y, dest_cb, dest_cr, x_offset, y_offset + 2,
                               qpix_put[2], chroma_put[1], qpix_avg[2], chroma_avg[1],
                               &weight_op[4], &weight_avg[4],
                               IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

                if (iRet < 0) { return -1; }
            }
            else if (IS_SUB_4X8(sub_mb_type))
            {
                iRet = mc_part(h, n  , 0, 4, 4 * h->mb_linesize, dest_y, dest_cb, dest_cr, x_offset, y_offset,
                               qpix_put[2], chroma_put[2], qpix_avg[2], chroma_avg[2],
                               &weight_op[5], &weight_avg[5],
                               IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

                if (iRet < 0) { return -1; }

                iRet = mc_part(h, n + 1, 0, 4, 4 * h->mb_linesize, dest_y, dest_cb, dest_cr, x_offset + 2, y_offset,
                               qpix_put[2], chroma_put[2], qpix_avg[2], chroma_avg[2],
                               &weight_op[5], &weight_avg[5],
                               IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

                if (iRet < 0) { return -1; }
            }
            else
            {
                int j;
                assert(IS_SUB_4X4(sub_mb_type));

                for (j = 0; j < 4; j++)
                {
                    int sub_x_offset = x_offset + 2 * (j & 1);
                    int sub_y_offset = y_offset +   (j & 2);
                    iRet = mc_part(h, n + j, 1, 2, 0, dest_y, dest_cb, dest_cr, sub_x_offset, sub_y_offset,
                                   qpix_put[2], chroma_put[2], qpix_avg[2], chroma_avg[2],
                                   &weight_op[6], &weight_avg[6],
                                   IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

                    if (iRet < 0) { return -1; }
                }
            }
        }
    }

    prefetch_motion(h, 1);
    return iRet;
}

static av_cold void init_cavlc_level_tab(void)
{
    int suffix_length, mask;
    unsigned int i;

    for (suffix_length = 0; suffix_length < 7; suffix_length++)
    {
        for (i = 0; i < (1 << LEVEL_TAB_BITS); i++)
        {
            int prefix = LEVEL_TAB_BITS - av_log2(2 * i);
            int level_code = (prefix << suffix_length) + (i >> (LEVEL_TAB_BITS - prefix - 1 - suffix_length)) - (1 << suffix_length);

            mask = -(level_code & 1);
            level_code = (((2 + level_code) >> 1) ^ mask) - mask;

            if (prefix + 1 + suffix_length <= LEVEL_TAB_BITS)
            {
                cavlc_level_tab[suffix_length][i][0] = level_code;
                cavlc_level_tab[suffix_length][i][1] = prefix + 1 + suffix_length;
            }
            else if (prefix + 1 <= LEVEL_TAB_BITS)
            {
                cavlc_level_tab[suffix_length][i][0] = prefix + 100;
                cavlc_level_tab[suffix_length][i][1] = prefix + 1;
            }
            else
            {
                cavlc_level_tab[suffix_length][i][0] = LEVEL_TAB_BITS + 100;
                cavlc_level_tab[suffix_length][i][1] = LEVEL_TAB_BITS;
            }
        }
    }
}

/* 2010/30/04 10:32:00 liuxw+00139685 */
/* ��decode_init_vlc�������ӷ����� */
//static av_cold void decode_init_vlc(void){
static av_cold int decode_init_vlc(void)
{
    static int done = 0;
    /* 2010/30/04 10:36:00 liuxw+00139685 */
    /* ���ӷ����� */
    int iRet = 0;

    if (!done)
    {
        int i;
        int offset;
        done = 1;

        chroma_dc_coeff_token_vlc.table = chroma_dc_coeff_token_vlc_table;
        chroma_dc_coeff_token_vlc.table_allocated = chroma_dc_coeff_token_vlc_table_size;
        /* 2010/30/04 10:39:00 liuxw+00139685 */
        /* ���ӷ�����ͷ�������ж� */
        iRet = init_vlc(&chroma_dc_coeff_token_vlc, CHROMA_DC_COEFF_TOKEN_VLC_BITS, 4 * 5,
                        &chroma_dc_coeff_token_len [0], 1, 1,
                        &chroma_dc_coeff_token_bits[0], 1, 1,
                        INIT_VLC_USE_NEW_STATIC);

        if (0 > iRet)
        {
            return -1;
        }

        offset = 0;

        for (i = 0; i < 4; i++)
        {
            coeff_token_vlc[i].table = coeff_token_vlc_tables + offset;
            coeff_token_vlc[i].table_allocated = coeff_token_vlc_tables_size[i];
            /* 2010/30/04 10:39:00 liuxw+00139685 */
            /* ���ӷ�����ͷ�������ж� */
            iRet = init_vlc(&coeff_token_vlc[i], COEFF_TOKEN_VLC_BITS, 4 * 17,
                            &coeff_token_len [i][0], 1, 1,
                            &coeff_token_bits[i][0], 1, 1,
                            INIT_VLC_USE_NEW_STATIC);

            if (0 > iRet)
            {
                return -1;
            }

            offset += coeff_token_vlc_tables_size[i];
        }

        /*
         * This is a one time safety check to make sure that
         * the packed static coeff_token_vlc table sizes
         * were initialized correctly.
         */
        assert(offset == FF_ARRAY_ELEMS(coeff_token_vlc_tables));

        for (i = 0; i < 3; i++)
        {
            chroma_dc_total_zeros_vlc[i].table = chroma_dc_total_zeros_vlc_tables[i];
            chroma_dc_total_zeros_vlc[i].table_allocated = chroma_dc_total_zeros_vlc_tables_size;
            /* 2010/30/04 10:39:00 liuxw+00139685 */
            /* ���ӷ�����ͷ�������ж� */
            iRet = init_vlc(&chroma_dc_total_zeros_vlc[i],
                            CHROMA_DC_TOTAL_ZEROS_VLC_BITS, 4,
                            &chroma_dc_total_zeros_len [i][0], 1, 1,
                            &chroma_dc_total_zeros_bits[i][0], 1, 1,
                            INIT_VLC_USE_NEW_STATIC);

            if (0 > iRet)
            {
                return -1;
            }
        }

        for (i = 0; i < 15; i++)
        {
            total_zeros_vlc[i].table = total_zeros_vlc_tables[i];
            total_zeros_vlc[i].table_allocated = total_zeros_vlc_tables_size;
            /* 2010/30/04 10:39:00 liuxw+00139685 */
            /* ���ӷ�����ͷ�������ж� */
            iRet = init_vlc(&total_zeros_vlc[i],
                            TOTAL_ZEROS_VLC_BITS, 16,
                            &total_zeros_len [i][0], 1, 1,
                            &total_zeros_bits[i][0], 1, 1,
                            INIT_VLC_USE_NEW_STATIC);

            if (0 > iRet)
            {
                return -1;
            }
        }

        for (i = 0; i < 6; i++)
        {
            run_vlc[i].table = run_vlc_tables[i];
            run_vlc[i].table_allocated = run_vlc_tables_size;
            /* 2010/30/04 10:39:00 liuxw+00139685 */
            /* ���ӷ�����ͷ�������ж� */
            iRet = init_vlc(&run_vlc[i],
                            RUN_VLC_BITS, 7,
                            &run_len [i][0], 1, 1,
                            &run_bits[i][0], 1, 1,
                            INIT_VLC_USE_NEW_STATIC);

            if (0 > iRet)
            {
                return -1;
            }
        }

        run7_vlc.table = run7_vlc_table,
                 run7_vlc.table_allocated = run7_vlc_table_size;
        /* 2010/30/04 10:39:00 liuxw+00139685 */
        /* ���ӷ�����ͷ�������ж� */
        iRet = init_vlc(&run7_vlc, RUN7_VLC_BITS, 16,
                        &run_len [6][0], 1, 1,
                        &run_bits[6][0], 1, 1,
                        INIT_VLC_USE_NEW_STATIC);

        if (0 > iRet)
        {
            return -1;
        }

        init_cavlc_level_tab();
    }

    /* 2010/30/04 10:37:00 liuxw+00139685 */
    /* ���ӷ����� */
    return iRet;
}

static void free_tables(H264Context *h)
{
    int i;
    H264Context *hx;
    av_freep(&h->intra4x4_pred_mode);
    av_freep(&h->chroma_pred_mode_table);
    av_freep(&h->cbp_table);
    av_freep(&h->mvd_table[0]);
    av_freep(&h->mvd_table[1]);
    av_freep(&h->direct_table);
    av_freep(&h->non_zero_count);

    /*hanqingrui add*/
    av_freep(&h->non_zero_count_8x8);
    /*hanqingrui add end*/

    av_freep(&h->slice_table_base);
    h->slice_table = NULL;

    av_freep(&h->mb2b_xy);
    av_freep(&h->mb2b8_xy);

    for (i = 0; i < h->s.avctx->thread_count; i++)
    {
        hx = h->thread_context[i];

        if (!hx) { continue; }

        av_freep(&hx->top_borders[1]);
        av_freep(&hx->top_borders[0]);
        av_freep(&hx->s.obmc_scratchpad);
    }
}

static void init_dequant8_coeff_table(H264Context *h)
{
    int i, q, x;
    const int transpose = (h->s.dsp.h264_idct8_add != ff_h264_idct8_add_c); //FIXME ugly
    h->dequant8_coeff[0] = h->dequant8_buffer[0];
    h->dequant8_coeff[1] = h->dequant8_buffer[1];

    for (i = 0; i < 2; i++ )
    {
        if (i && !memcmp(h->pps.scaling_matrix8[0], h->pps.scaling_matrix8[1], 64 * sizeof(uint8_t)))
        {
            h->dequant8_coeff[1] = h->dequant8_buffer[0];
            break;
        }

        for (q = 0; q < 52; q++)
        {
            int shift = div6[q];
            int idx = rem6[q];

            for (x = 0; x < 64; x++)
                h->dequant8_coeff[i][q][transpose ? (x >> 3) | ((x & 7) << 3) : x] =
                    ((uint32_t)dequant8_coeff_init[idx][ dequant8_coeff_init_scan[((x >> 1) & 12) | (x & 3)] ] *
                     h->pps.scaling_matrix8[i][x]) << shift;
        }
    }
}

static void init_dequant4_coeff_table(H264Context *h)
{
    int i, j, q, x;
    const int transpose = (h->s.dsp.h264_idct_add != ff_h264_idct_add_c); //FIXME ugly

    for (i = 0; i < 6; i++ )
    {
        h->dequant4_coeff[i] = h->dequant4_buffer[i];

        for (j = 0; j < i; j++)
        {
            if (!memcmp(h->pps.scaling_matrix4[j], h->pps.scaling_matrix4[i], 16 * sizeof(uint8_t)))
            {
                h->dequant4_coeff[i] = h->dequant4_buffer[j];
                break;
            }
        }

        if (j < i)
        { continue; }

        for (q = 0; q < 52; q++)
        {
            int shift = div6[q] + 2;
            int idx = rem6[q];

            for (x = 0; x < 16; x++)
                h->dequant4_coeff[i][q][transpose ? (x >> 2) | ((x << 2) & 0xF) : x] =
                    ((uint32_t)dequant4_coeff_init[idx][(x & 1) + ((x >> 2) & 1)] *
                     h->pps.scaling_matrix4[i][x]) << shift;
        }
    }
}

static void init_dequant_tables(H264Context *h)
{
    int i, x;
    init_dequant4_coeff_table(h);

    if (h->pps.transform_8x8_mode)
    { init_dequant8_coeff_table(h); }

    if (h->sps.transform_bypass)
    {
        for (i = 0; i < 6; i++)
            for (x = 0; x < 16; x++)
            { h->dequant4_coeff[i][0][x] = 1 << 6; }

        if (h->pps.transform_8x8_mode)
            for (i = 0; i < 2; i++)
                for (x = 0; x < 64; x++)
                { h->dequant8_coeff[i][0][x] = 1 << 6; }
    }
}


/**
 * allocates tables.
 * needs width/height
 */
static int alloc_tables(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int big_mb_num = s->mb_stride * (s->mb_height + 1);
    int x, y;

    CHECKED_ALLOCZ(s->avctx, h->intra4x4_pred_mode, big_mb_num * 8  * sizeof(uint8_t))

    CHECKED_ALLOCZ(s->avctx, h->non_zero_count    , big_mb_num * 16 * sizeof(uint8_t))

    /*hanqingrui add*/
    CHECKED_ALLOCZ(s->avctx, h->non_zero_count_8x8, big_mb_num * 16 * sizeof(uint8_t))
    /*hanqingrui add end*/

    CHECKED_ALLOCZ(s->avctx, h->slice_table_base  , (big_mb_num + s->mb_stride) * sizeof(*h->slice_table_base))
    CHECKED_ALLOCZ(s->avctx, h->cbp_table, big_mb_num * sizeof(uint16_t))

    CHECKED_ALLOCZ(s->avctx, h->chroma_pred_mode_table, big_mb_num * sizeof(uint8_t))
    CHECKED_ALLOCZ(s->avctx, h->mvd_table[0], 32 * big_mb_num * sizeof(uint16_t));
    CHECKED_ALLOCZ(s->avctx, h->mvd_table[1], 32 * big_mb_num * sizeof(uint16_t));
    CHECKED_ALLOCZ(s->avctx, h->direct_table, 32 * big_mb_num * sizeof(uint8_t));

    memset(h->slice_table_base, -1, (big_mb_num + s->mb_stride)  * sizeof(*h->slice_table_base));
    h->slice_table = h->slice_table_base + s->mb_stride * 2 + 1;

    CHECKED_ALLOCZ(s->avctx, h->mb2b_xy  , big_mb_num * sizeof(uint32_t));
    CHECKED_ALLOCZ(s->avctx, h->mb2b8_xy , big_mb_num * sizeof(uint32_t));

    for (y = 0; y < s->mb_height; y++)
    {
        for (x = 0; x < s->mb_width; x++)
        {
            const int mb_xy = x + y * s->mb_stride;
            const int b_xy = 4 * x + 4 * y * h->b_stride;
            const int b8_xy = 2 * x + 2 * y * h->b8_stride;

            h->mb2b_xy [mb_xy] = b_xy;
            h->mb2b8_xy[mb_xy] = b8_xy;
        }
    }

    s->obmc_scratchpad = NULL;

    if (!h->dequant4_coeff[0])
    { init_dequant_tables(h); }

    return 0;
fail:
    free_tables(h);
    h->s.avctx->iMallocFailed = 1;
    av_log(h->s.avctx, AV_LOG_ERROR, "Malloc memory for tables failed\n");
    return -1;
}

/**
 * Mimic alloc_tables(), but for every context thread.
 */
static void clone_tables(H264Context *dst, H264Context *src)
{
    dst->intra4x4_pred_mode       = src->intra4x4_pred_mode;
    dst->non_zero_count           = src->non_zero_count;

    /*hanqingrui add*/
    dst->non_zero_count_8x8       = src->non_zero_count_8x8;
    /*hanqingrui add end*/

    dst->slice_table              = src->slice_table;
    dst->cbp_table                = src->cbp_table;
    dst->mb2b_xy                  = src->mb2b_xy;
    dst->mb2b8_xy                 = src->mb2b8_xy;
    dst->chroma_pred_mode_table   = src->chroma_pred_mode_table;
    dst->mvd_table[0]             = src->mvd_table[0];
    dst->mvd_table[1]             = src->mvd_table[1];
    dst->direct_table             = src->direct_table;

    dst->s.obmc_scratchpad = NULL;
    ff_h264_pred_init(&dst->hpc, src->s.codec_id);
}

/**
 * Init context
 * Allocate buffers which are not shared amongst multiple threads.
 */
static int context_init(H264Context *h)
{
    CHECKED_ALLOCZ(h->s.avctx, h->top_borders[0], h->s.mb_width * (16 + 8 + 8) * sizeof(uint8_t))
    CHECKED_ALLOCZ(h->s.avctx, h->top_borders[1], h->s.mb_width * (16 + 8 + 8) * sizeof(uint8_t))

    return 0;
fail:
    return -1; // free_tables will clean up for us
}

static av_cold void common_init(H264Context *h)
{
    MpegEncContext *const s = &h->s;

    s->width = s->avctx->width;
    s->height = s->avctx->height;
    s->codec_id = s->avctx->codec->id;
    /* init the intra pred founction pointer */
    ff_h264_pred_init(&h->hpc, s->codec_id);

    h->dequant_coeff_pps = -1;
    s->unrestricted_mv = 1;
    s->decode = 1; //FIXME
    /* init many founction pointer and tables */
    dsputil_init(&s->dsp, s->avctx); // needed so that idct permutation is known early

    memset(h->pps.scaling_matrix4, 16, 6 * 16 * sizeof(uint8_t));
    memset(h->pps.scaling_matrix8, 16, 2 * 64 * sizeof(uint8_t));
}

/**
 * Reset SEI values at the beginning of the frame.
 *
 * @param h H.264 context.
 */
static void reset_sei(H264Context *h)
{
    h->sei_recovery_frame_cnt       = -1;
    h->sei_dpb_output_delay         =  0;
    h->sei_cpb_removal_delay        = -1;
    h->sei_buffering_period_present =  0;
}

static av_cold int decode_init(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;
    MpegEncContext *const s = &h->s;

    /* 2010/30/04 10:41:00 liuxw+00139685 */
    /* ���ӷ����� */
    int iRet = 0;

    /* init MpegEncContext with default value */
    MPV_decode_defaults(s);

    s->avctx = avctx;
    /* init intra predict founction pointer and many others fountion pointer and tables */
    common_init(h);

    s->out_format = FMT_H264;
    s->workaround_bugs = avctx->workaround_bugs;

    // set defaults
    //    s->decode_mb= ff_h263_decode_mb;
    s->quarter_sample = 1;

    if (!avctx->has_b_frames)
    { s->low_delay = 1; }

    if (s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
    { avctx->pix_fmt = PIX_FMT_VDPAU_H264; }
    else
    { avctx->pix_fmt = avctx->get_format(avctx, avctx->codec->pix_fmts); }

    avctx->hwaccel = ff_find_hwaccel(avctx->codec->id, avctx->pix_fmt);

    /* 2010/30/04 10:39:00 liuxw+00139685 */
    /* ���ӷ�����ͷ�������ж� */
    iRet = decode_init_vlc();

    if (0 > iRet)
    {
        return -1;
    }

    if (avctx->extradata_size[0] > 0 && avctx->extradata[0] &&
        *(char *)avctx->extradata[0] == 1)
    {
        h->is_avc = 1;
        h->got_avcC = 0;
    }
    else
    {
        h->is_avc = 0;
    }

    h->thread_context[0] = h;
    h->outputed_poc = INT_MIN;
    h->prev_poc_msb = 1 << 16;
    reset_sei(h);

    if (avctx->codec_id == CODEC_ID_H264)
    {
        if (avctx->ticks_per_frame == 1)
        {
            s->avctx->time_base.den *= 2;
        }

        avctx->ticks_per_frame = 2;
    }

    /* 2010/30/04 10:41:00 liuxw+00139685 */
    /* ��rbsp_buffer��parse.buffer����һ�̶���С�ڴ� */
#define MAX_FRAME_SIZE 1920*1088*2

    s->parse_context.buffer = av_malloc_hw(avctx, MAX_FRAME_SIZE);

    if (NULL == s->parse_context.buffer)
    {
        av_log(avctx, AV_LOG_WARNING, "codec_ctx[%p] decode_init() malloc buffer for s->parse_context.buffer failed!\n", avctx);
        goto END;
    }

    s->parse_context.buffer_size = MAX_FRAME_SIZE;

    h->rbsp_buffer[0] = av_malloc_hw(avctx, MAX_FRAME_SIZE);
    h->rbsp_buffer[1] = av_malloc_hw(avctx, MAX_FRAME_SIZE);

    if (NULL == h->rbsp_buffer[0] || NULL == h->rbsp_buffer[1])
    {
        av_log(avctx, AV_LOG_WARNING, "codec_ctx[%p] Malloc buffer for h->rbsp_buffer failed!\n", avctx);
        goto END;
    }

    h->rbsp_buffer_size[0] = h->rbsp_buffer_size[1] = MAX_FRAME_SIZE;

    /* 2010/06/17 15:30:00 liuxw+00139685 */
    /* �ڳ�ʼ������������parse_context״̬��ʼ��(ԭ������mpv_common_init��ʵ�ֵ�) */
    s->parse_context.state = -1;

    return 0;

    /* 2010/30/04 10:43:00 liuxw+00139685 */
    /* ����rbsp_buffer��parse.buffer����һ�̶���С�ڴ�ʧ�ܣ����ͷ��Ѿ�����ɹ����ڴ� */
END:

    if (s->parse_context.buffer)
    {
        av_free_hw(s->parse_context.buffer);
        s->parse_context.buffer = NULL;
        s->parse_context.buffer_size = 0;
    }

    if (NULL == h->rbsp_buffer[0])
    {
        av_free_hw(h->rbsp_buffer[0]);
        h->rbsp_buffer[0] = NULL;
        h->rbsp_buffer_size[0] = 0;
    }

    if (NULL == h->rbsp_buffer[1])
    {
        av_free_hw(h->rbsp_buffer[1]);
        h->rbsp_buffer[1] = NULL;
        h->rbsp_buffer_size[1] = 0;
    }

    return -1;
}

static int frame_start(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int i;

    if (MPV_frame_start(s, s->avctx) < 0)
    { return -1; }

    ff_er_frame_start(s);
    /*
     * MPV_frame_start uses pict_type to derive key_frame.
     * This is incorrect for H.264; IDR markings must be used.
     * Zero here; IDR markings per slice in frame or fields are ORed in later.
     * See decode_nal_units().
     */
    s->current_picture_ptr->key_frame = 0;

    assert(s->linesize && s->uvlinesize);

    for (i = 0; i < 16; i++)
    {
        h->block_offset[i] = 4 * ((scan8[i] - scan8[0]) & 7) + 4 * s->linesize * ((scan8[i] - scan8[0]) >> 3);
        h->block_offset[24 + i] = 4 * ((scan8[i] - scan8[0]) & 7) + 8 * s->linesize * ((scan8[i] - scan8[0]) >> 3);
    }

    for (i = 0; i < 4; i++)
    {
        h->block_offset[16 + i] =
            h->block_offset[20 + i] = 4 * ((scan8[i] - scan8[0]) & 7) + 4 * s->uvlinesize * ((scan8[i] - scan8[0]) >> 3);
        h->block_offset[24 + 16 + i] =
            h->block_offset[24 + 20 + i] = 4 * ((scan8[i] - scan8[0]) & 7) + 8 * s->uvlinesize * ((scan8[i] - scan8[0]) >> 3);
    }

    /* can't be in alloc_tables because linesize isn't known there.
     * FIXME: redo bipred weight to not require extra buffer? */
    for (i = 0; i < s->avctx->thread_count; i++)
        if (!h->thread_context[i]->s.obmc_scratchpad)
        { h->thread_context[i]->s.obmc_scratchpad = av_malloc_hw(s->avctx, 16 * 2 * s->linesize + 8 * 2 * s->uvlinesize); }

    /* some macroblocks will be accessed before they're available */
    if (FRAME_MBAFF || s->avctx->thread_count > 1)
    { memset(h->slice_table, -1, (s->mb_height * s->mb_stride - 1) * sizeof(*h->slice_table)); }

    //    s->decode= (s->flags&CODEC_FLAG_PSNR) || !s->encoding || s->current_picture.reference /*|| h->contains_intra*/ || 1;

    // We mark the current picture as non-reference after allocating it, so
    // that if we break out due to an error it can be released automatically
    // in the next MPV_frame_start().
    // SVQ3 as well as most other codecs have only last/next/current and thus
    // get released even with set reference, besides SVQ3 and others do not
    // mark frames as reference later "naturally".
    if (s->codec_id != CODEC_ID_SVQ3)
    { s->current_picture_ptr->reference = 0; }

    s->current_picture_ptr->field_poc[0] =
        s->current_picture_ptr->field_poc[1] = INT_MAX;
    /* 2010/08/29 16:00:00 liuxw+00139685 */
    /* ��λiErrorCode */
    s->current_picture_ptr->iErrorCode = 0;
    assert(s->current_picture_ptr->long_ref == 0);

    return 0;
}

static inline void backup_mb_border(H264Context *h, uint8_t *src_y, uint8_t *src_cb, uint8_t *src_cr, int linesize, int uvlinesize, int simple)
{
    MpegEncContext *const s = &h->s;
    int i;
    int step    = 1;
    int offset  = 1;
    int uvoffset = 1;
    int top_idx = 1;
    int skiplast = 0;

    src_y  -=   linesize;
    src_cb -= uvlinesize;
    src_cr -= uvlinesize;

    if (!simple && FRAME_MBAFF)
    {
        if (s->mb_y & 1)
        {
            offset  = MB_MBAFF ? 1 : 17;
            uvoffset = MB_MBAFF ? 1 : 9;

            if (!MB_MBAFF)
            {
                *(uint64_t *)(h->top_borders[0][s->mb_x] + 0) = *(uint64_t *)(src_y +  15 * linesize);
                *(uint64_t *)(h->top_borders[0][s->mb_x] + 8) = *(uint64_t *)(src_y + 8 + 15 * linesize);

                if (simple || !CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY))
                {
                    *(uint64_t *)(h->top_borders[0][s->mb_x] + 16) = *(uint64_t *)(src_cb + 7 * uvlinesize);
                    *(uint64_t *)(h->top_borders[0][s->mb_x] + 24) = *(uint64_t *)(src_cr + 7 * uvlinesize);
                }
            }
        }
        else
        {
            if (!MB_MBAFF)
            {
                h->left_border[0] = h->top_borders[0][s->mb_x][15];

                if (simple || !CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY))
                {
                    h->left_border[34   ] = h->top_borders[0][s->mb_x][16 + 7  ];
                    h->left_border[34 + 18] = h->top_borders[0][s->mb_x][16 + 8 + 7];
                }

                skiplast = 1;
            }

            offset  =
                uvoffset =
                    top_idx = MB_MBAFF ? 0 : 1;
        }

        step = MB_MBAFF ? 2 : 1;
    }

    // There are two lines saved, the line above the the top macroblock of a pair,
    // and the line above the bottom macroblock
    h->left_border[offset] = h->top_borders[top_idx][s->mb_x][15];

    for (i = 1; i < 17 - skiplast; i++)
    {
        h->left_border[offset + i * step] = src_y[15 + i *  linesize];
    }

    *(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 0) = *(uint64_t *)(src_y +  16 * linesize);
    *(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 8) = *(uint64_t *)(src_y + 8 + 16 * linesize);

    if (simple || !CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY))
    {
        h->left_border[uvoffset + 34   ] = h->top_borders[top_idx][s->mb_x][16 + 7];
        h->left_border[uvoffset + 34 + 18] = h->top_borders[top_idx][s->mb_x][24 + 7];

        for (i = 1; i < 9 - skiplast; i++)
        {
            h->left_border[uvoffset + 34   + i * step] = src_cb[7 + i * uvlinesize];
            h->left_border[uvoffset + 34 + 18 + i * step] = src_cr[7 + i * uvlinesize];
        }

        *(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 16) = *(uint64_t *)(src_cb + 8 * uvlinesize);
        *(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 24) = *(uint64_t *)(src_cr + 8 * uvlinesize);
    }
}

static inline void xchg_mb_border(H264Context *h, uint8_t *src_y, uint8_t *src_cb, uint8_t *src_cr, int linesize, int uvlinesize, int xchg, int simple)
{
    MpegEncContext *const s = &h->s;
    int temp8, i;
    uint64_t temp64;
    int deblock_left;
    int deblock_top;
    int mb_xy;
    int step    = 1;
    int offset  = 1;
    int uvoffset = 1;
    int top_idx = 1;

    if (!simple && FRAME_MBAFF)
    {
        if (s->mb_y & 1)
        {
            offset  = MB_MBAFF ? 1 : 17;
            uvoffset = MB_MBAFF ? 1 : 9;
        }
        else
        {
            offset  =
                uvoffset =
                    top_idx = MB_MBAFF ? 0 : 1;
        }

        step = MB_MBAFF ? 2 : 1;
    }

    if (h->deblocking_filter == 2)
    {
        mb_xy = h->mb_xy;
        deblock_left = h->slice_table[mb_xy] == h->slice_table[mb_xy - 1];
        deblock_top  = h->slice_table[mb_xy] == h->slice_table[h->top_mb_xy];
    }
    else
    {
        deblock_left = (s->mb_x > 0);
        deblock_top =  (s->mb_y > !!MB_FIELD);
    }

    src_y  -=   linesize + 1;
    src_cb -= uvlinesize + 1;
    src_cr -= uvlinesize + 1;

#define XCHG(a,b,t,xchg)\
    t= a;\
    if(xchg)\
        a= b;\
    b= t;

    if (deblock_left)
    {
        for (i = !deblock_top; i < 16; i++)
        {
            XCHG(h->left_border[offset + i * step], src_y [i *  linesize], temp8, xchg);
        }

        XCHG(h->left_border[offset + i * step], src_y [i *  linesize], temp8, 1);
    }

    if (deblock_top)
    {
        XCHG(*(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 0), *(uint64_t *)(src_y + 1), temp64, xchg);
        XCHG(*(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 8), *(uint64_t *)(src_y + 9), temp64, 1);

        if (s->mb_x + 1 < s->mb_width)
        {
            XCHG(*(uint64_t *)(h->top_borders[top_idx][s->mb_x + 1]), *(uint64_t *)(src_y + 17), temp64, 1);
        }
    }

    if (simple || !CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY))
    {
        if (deblock_left)
        {
            for (i = !deblock_top; i < 8; i++)
            {
                XCHG(h->left_border[uvoffset + 34   + i * step], src_cb[i * uvlinesize], temp8, xchg);
                XCHG(h->left_border[uvoffset + 34 + 18 + i * step], src_cr[i * uvlinesize], temp8, xchg);
            }

            XCHG(h->left_border[uvoffset + 34   + i * step], src_cb[i * uvlinesize], temp8, 1);
            XCHG(h->left_border[uvoffset + 34 + 18 + i * step], src_cr[i * uvlinesize], temp8, 1);
        }

        if (deblock_top)
        {
            XCHG(*(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 16), *(uint64_t *)(src_cb + 1), temp64, 1);
            XCHG(*(uint64_t *)(h->top_borders[top_idx][s->mb_x] + 24), *(uint64_t *)(src_cr + 1), temp64, 1);
        }
    }
}
/* IDCT , ֡��Ԥ�⡢֡��Ԥ�⡢��·�˲�������ؽ� */
static av_always_inline int hl_decode_mb_internal(H264Context *h, int simple)
{
    MpegEncContext *const s = &h->s;
    const int mb_x = s->mb_x;
    const int mb_y = s->mb_y;
    const int mb_xy = h->mb_xy;
    const int mb_type = s->current_picture.mb_type[mb_xy];
    uint8_t  *dest_y, *dest_cb, *dest_cr;
    int linesize, uvlinesize /*dct_offset*/;
    int i;
    int *block_offset = &h->block_offset[0];
    const int transform_bypass = !simple && (s->qscale == 0 && h->sps.transform_bypass);
    /* is_h264 should always be true if SVQ3 is disabled. */
    const int is_h264 = !CONFIG_SVQ3_DECODER || simple || s->codec_id == CODEC_ID_H264;
    void (*idct_add)(uint8_t * dst, DCTELEM * block, int stride);
    void (*idct_dc_add)(uint8_t * dst, DCTELEM * block, int stride);
    int iRet = 0;

    dest_y  = s->current_picture.data[0] + (mb_x + mb_y * s->linesize  ) * 16;
    dest_cb = s->current_picture.data[1] + (mb_x + mb_y * s->uvlinesize) * 8;
    dest_cr = s->current_picture.data[2] + (mb_x + mb_y * s->uvlinesize) * 8;

    s->dsp.prefetch(dest_y + (s->mb_x & 3) * 4 * s->linesize + 64, s->linesize, 4);
    s->dsp.prefetch(dest_cb + (s->mb_x & 7)*s->uvlinesize + 64, dest_cr - dest_cb, 2);

    if (!simple && MB_FIELD)
    {
        linesize   = h->mb_linesize   = s->linesize * 2;
        uvlinesize = h->mb_uvlinesize = s->uvlinesize * 2;
        block_offset = &h->block_offset[24];

        if (mb_y & 1)
        {
            //FIXME move out of this function?
            dest_y -= s->linesize * 15;
            dest_cb -= s->uvlinesize * 7;
            dest_cr -= s->uvlinesize * 7;
        }

        if (FRAME_MBAFF)
        {
            int list;

            for (list = 0; list < (int)h->list_count; list++)
            {
                if (!USES_LIST(mb_type, list))
                { continue; }

                if (IS_16X16(mb_type))
                {
                    int8_t *ref = &h->ref_cache[list][scan8[0]];
                    fill_rectangle(ref, 4, 4, 8, (16 + *ref) ^ (s->mb_y & 1), 1);
                }
                else
                {
                    for (i = 0; i < 16; i += 4)
                    {
                        int ref = h->ref_cache[list][scan8[i]];

                        if (ref >= 0)
                        { fill_rectangle(&h->ref_cache[list][scan8[i]], 2, 2, 8, (16 + ref) ^ (s->mb_y & 1), 1); }
                    }
                }
            }
        }
    }
    else
    {
        linesize   = h->mb_linesize   = s->linesize;
        uvlinesize = h->mb_uvlinesize = s->uvlinesize;
        //        dct_offset = s->linesize * 16;
    }

    /* PCM */
    if (!simple && IS_INTRA_PCM(mb_type))
    {
        for (i = 0; i < 16; i++)
        {
            memcpy(dest_y + i *  linesize, h->mb       + i * 8, 16);
        }

        for (i = 0; i < 8; i++)
        {
            memcpy(dest_cb + i * uvlinesize, h->mb + 128 + i * 4,  8);
            memcpy(dest_cr + i * uvlinesize, h->mb + 160 + i * 4,  8);
        }
    }
    else  	/* IDCT��֡��Ԥ�⡢֡��Ԥ�⡢����ؽ�*/
    {
        if (IS_INTRA(mb_type)) 	/* ֡��Ԥ�⡢IDCT */
        {
            if (h->deblocking_filter)	/* ���µ�ǰ���ı�Ե��Ϣ */
            { xchg_mb_border(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize, 1, simple); }

            /* ֡��chromaԤ�� */
            if (simple || !CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY))
            {
                /* 2010/06/04 14:30:00 liuxw+00139685 */
                /* ��ֹ���������Ӷ�h->chroma_pred_mode�Ϸ��Եļ�� */
                if (h->chroma_pred_mode >= 11)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "chroma intra8x8_pred_mod[%d] is invalid!\n", h->chroma_pred_mode);
                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                h->hpc.pred8x8[ h->chroma_pred_mode ](dest_cb, uvlinesize);
                h->hpc.pred8x8[ h->chroma_pred_mode ](dest_cr, uvlinesize);
            }

            /* mb_type = intra4x4 or intra8x8 */
            if (IS_INTRA4x4(mb_type))
            {
                if (simple || !s->encoding) 	/* �����ͽ��� */
                {
                    if (IS_8x8DCT(mb_type)) 	/* DCTģʽΪ8x8dct�������Ľ��� */
                    {
                        if (transform_bypass) 	/* transform_bypassģʽ��������8x8DCT) */
                        {
                            idct_dc_add =
                                idct_add    = s->dsp.add_pixels8;
                        }
                        else 	/* ����ģʽ(����8x8DCT */
                        {
                            idct_dc_add = s->dsp.h264_idct8_dc_add;
                            idct_add    = s->dsp.h264_idct8_add;
                        }		/* intra8x8 Ԥ�⡢IDCT������ؽ� */

                        for (i = 0; i < 16; i += 4)
                        {
                            uint8_t *const ptr = dest_y + block_offset[i];
                            const int dir = h->intra4x4_pred_mode_cache[ scan8[i] ];

                            if (transform_bypass && h->sps.profile_idc == 244 && dir <= 1)
                            {
                                /* 2010/06/04 14:30:00 liuxw+00139685 */
                                /* ��ֹ���������Ӷ�dir�Ϸ��Եļ�� */
                                if (dir >= 2)
                                {
                                    av_log(h->s.avctx, AV_LOG_WARNING, "pred8x8l_add[%d] is invalid!\n", dir);
                                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                                    h->s.avctx->iTotalError++;
                                    return -1;
                                }

                                h->hpc.pred8x8l_add[dir](ptr, h->mb + i * 16, linesize);
                            }
                            else 	/* intra8x8 Ԥ�� */
                            {
                                const int nnz = h->non_zero_count_cache[ scan8[i] ];

                                /* 2010/06/04 14:30:00 liuxw+00139685 */
                                /* ��ֹ���������Ӷ�dir�Ϸ��Եļ�� */
                                if (dir >= 12)
                                {
                                    av_log(h->s.avctx, AV_LOG_WARNING, "intra8x8l_pred_mod[%d] is invalid!\n", dir);
                                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                                    h->s.avctx->iTotalError++;
                                    return -1;
                                }

                                h->hpc.pred8x8l[ dir ](ptr, (h->topleft_samples_available << i) & 0x8000,
                                                       (h->topright_samples_available << i) & 0x4000, linesize);

                                if (nnz) /* intra8x8 IDCT�ͺ���ؽ� */
                                {
                                    if (nnz == 1 && h->mb[i * 16])		/* ֻ����DC��IDCT�ͺ���ؽ� */
                                    { idct_dc_add(ptr, h->mb + i * 16, linesize); }
                                    else	/* ����DC��AC��IDCT�ͺ���ؽ� */
                                    { idct_add   (ptr, h->mb + i * 16, linesize); }
                                }
                            }
                        }
                    }
                    else 	/* DCTģʽΪ8x8dct�������Ľ��� */
                    {
                        if (transform_bypass) 	/* transform_bypassģʽ��������4x4DCT) */
                        {
                            idct_dc_add =
                                idct_add    = s->dsp.add_pixels4;
                        }
                        else 	/* ����ģʽ(����4x4DCT */
                        {
                            idct_dc_add = s->dsp.h264_idct_dc_add;
                            idct_add    = s->dsp.h264_idct_add;
                        }	/* intra4x4 Ԥ�⡢IDCT������ؽ� */

                        for (i = 0; i < 16; i++)
                        {
                            uint8_t *const ptr = dest_y + block_offset[i];
                            const int dir = h->intra4x4_pred_mode_cache[ scan8[i] ];

                            if (transform_bypass && h->sps.profile_idc == 244 && dir <= 1)
                            {
                                /* 2010/06/04 14:30:00 liuxw+00139685 */
                                /* ��ֹ���������Ӷ�dir�Ϸ��Եļ�� */
                                if (dir >= 2)
                                {
                                    av_log(h->s.avctx, AV_LOG_WARNING, "pred4x4_add[%d] is invalid!\n", dir);
                                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                                    h->s.avctx->iTotalError++;
                                    return -1;
                                }

                                h->hpc.pred4x4_add[dir](ptr, h->mb + i * 16, linesize);
                            }
                            else
                            {
                                uint8_t *topright;
                                int nnz, tr;

                                if (dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED)
                                {
                                    const int topright_avail = (h->topright_samples_available << i) & 0x8000;
                                    assert(mb_y || linesize <= block_offset[i]);

                                    if (!topright_avail)
                                    {
                                        tr = ptr[3 - linesize] * 0x01010101;
                                        topright = (uint8_t *) &tr;
                                    }
                                    else
                                    { topright = ptr + 4 - linesize; }
                                }
                                else
                                { topright = NULL; }

                                /* intra4x4 Ԥ�� */
                                /* 2010/06/04 14:30:00 liuxw+00139685 */
                                /* ���Ӷ�dir�ĺϷ��Խ��м�� */
                                if (dir >= 15)
                                {
                                    av_log(h->s.avctx, AV_LOG_WARNING, "intra4x4_pred_mod[%d] is invalid!\n", dir);
                                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                                    h->s.avctx->iTotalError++;
                                    return -1;
                                }

                                h->hpc.pred4x4[ dir ](ptr, topright, linesize);
                                nnz = h->non_zero_count_cache[ scan8[i] ];

                                if (nnz)
                                {
                                    if (is_h264)
                                    {
                                        if (nnz == 1 && h->mb[i * 16])	/* ֻ����DC��IDCT �ͺ���ؽ� */
                                        { idct_dc_add(ptr, h->mb + i * 16, linesize); }
                                        else	/* ����DC��AC��IDCT�ͺ���ؽ� */
                                        { idct_add   (ptr, h->mb + i * 16, linesize); }
                                    }
                                    else  /* codec_id  = svq3 */
                                    { svq3_add_idct_c(ptr, h->mb + i * 16, linesize, s->qscale, 0); }
                                }
                            }
                        }
                    }
                }
            }
            else 	/* intra16x16Ԥ�⡢DC��������IDCT */
            {
                /* 2010/06/04 14:30:00 liuxw+00139685 */
                /* ��ֹ���������Ӷ�h->intra16x16_pred_mode�ĺϷ��Լ�� */
                if (h->intra16x16_pred_mode >= 7)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "intra16x16_pred_mod[%d] is invalid!\n", h->intra16x16_pred_mode);
                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                h->hpc.pred16x16[ h->intra16x16_pred_mode ](dest_y , linesize);	/* intra16x16Ԥ�� */

                if (is_h264)
                {
                    if (!transform_bypass)	/* intra16x16 DC��������IDCT */
                    { h264_luma_dc_dequant_idct_c(h->mb, s->qscale, h->dequant4_coeff[0][s->qscale][0]); }
                }
                else	/* codec_id  = svq3 */
                { svq3_luma_dc_dequant_idct_c(h->mb, s->qscale); }
            }

            if (h->deblocking_filter)	/* ���µ�ǰ���ı�Ե��Ϣ */
            { xchg_mb_border(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize, 0, simple); }
        }
        else if (is_h264) 	/* ֡��Ԥ�� */
        {
            iRet = hl_motion(h, dest_y, dest_cb, dest_cr,
                             s->me.qpel_put, s->dsp.put_h264_chroma_pixels_tab,
                             s->me.qpel_avg, s->dsp.avg_h264_chroma_pixels_tab,
                             s->dsp.weight_h264_pixels_tab, s->dsp.biweight_h264_pixels_tab);

            if (iRet < 0) { return -1; }

#ifdef LXW_DEBUG

            /* add by lxw for test */
            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && (h->s.mb_y == MB_Y || h->s.mb_y == MB_Y2))
            {
                int i, j;
                uint8_t  *dest_y_temp = dest_y;
                av_log(h->s.avctx, AV_LOG_ERROR, "predict value:[frame_num:%d POC:%d mb_x:%d mb_y:%d] use_weight:%d\n", frame_num, h->s.current_picture_ptr->field_poc[0], h->s.mb_x, h->s.mb_y, h->use_weight);

                for (i = 0; i < 16; i++)
                {
                    for (j = 0; j < 16; j++)
                    { av_log(h->s.avctx, AV_LOG_ERROR, "%3d ", dest_y_temp[j]); }

                    av_log(h->s.avctx, AV_LOG_ERROR, "\n");
                    dest_y_temp += linesize;
                }
            }

#endif
        }

        /* ����intra4x4��intra8x8���IDCT�ͺ���ؽ�(intra4x4��intra8x8�Ѿ�IDCT�ͺ���ؽ��� */
        if (!IS_INTRA4x4(mb_type))
        {
            if (is_h264)
            {
                if (IS_INTRA16x16(mb_type))
                {
                    /* intra16x16 IDCT�ͺ���ؽ� */
                    if (transform_bypass)
                    {
                        /* transform_bypassģʽ������ */
                        if (h->sps.profile_idc == 244 && (h->intra16x16_pred_mode == VERT_PRED8x8 || h->intra16x16_pred_mode == HOR_PRED8x8))
                        {
                            /* 2010/06/04 14:30:00 liuxw+00139685 */
                            /* ��ֹ���������Ӷ�h->intra16x16_pred_mode�Ϸ��Եļ�� */
                            if (h->intra16x16_pred_mode >= 3)
                            {
                                av_log(h->s.avctx, AV_LOG_WARNING, "intra16x16_pred_mode[%d] is invalid!\n", h->intra16x16_pred_mode);
                                IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                                h->s.avctx->iTotalError++;
                                return -1;
                            }

                            h->hpc.pred16x16_add[h->intra16x16_pred_mode](dest_y, block_offset, h->mb, linesize);
                        }
                        else
                        {
                            for (i = 0; i < 16; i++)
                            {
                                if (h->non_zero_count_cache[ scan8[i] ] || h->mb[i * 16])
                                { s->dsp.add_pixels4(dest_y + block_offset[i], h->mb + i * 16, linesize); }
                            }
                        }
                    }
                    else
                    {
                        /* ����DCT������� */
                        s->dsp.h264_idct_add16intra(dest_y, block_offset, h->mb, linesize, h->non_zero_count_cache);	/* intra16x16 IDCT�ͺ���ؽ� */
                    }
                }
                else if (h->cbp & 15)
                {
                    /* ֡��IDCT�ͺ���ؽ� */
                    if (transform_bypass)
                    {
                        /* transform_bypassģʽ������ */
                        const int di = IS_8x8DCT(mb_type) ? 4 : 1;
                        idct_add = IS_8x8DCT(mb_type) ? s->dsp.add_pixels8 : s->dsp.add_pixels4;

                        for (i = 0; i < 16; i += di)
                        {
                            if (h->non_zero_count_cache[ scan8[i] ])
                            {
                                idct_add(dest_y + block_offset[i], h->mb + i * 16, linesize);
                            }
                        }
                    }
                    else
                    {
                        /* ����DCT������� */
                        if (IS_8x8DCT(mb_type))
                        {
                            /* DCTģʽΪ8x8dct�������Ľ��� */
                            s->dsp.h264_idct8_add4(dest_y, block_offset, h->mb, linesize, h->non_zero_count_cache);
                        }
                        else
                        {
                            /* DCTģʽΪ4x4dct�������Ľ��� */
                            s->dsp.h264_idct_add16(dest_y, block_offset, h->mb, linesize, h->non_zero_count_cache);
                        }
                    }
                }
            }
            else
            {
                /* codec_id = svq3 */
                for (i = 0; i < 16; i++)
                {
                    if (h->non_zero_count_cache[ scan8[i] ] || h->mb[i * 16])
                    {
                        //FIXME benchmark weird rule, & below
                        uint8_t *const ptr = dest_y + block_offset[i];
                        svq3_add_idct_c(ptr, h->mb + i * 16, linesize, s->qscale, IS_INTRA(mb_type) ? 1 : 0);
                    }
                }
            }
        }

        /* ɫ�ȿ��IDCT���ؽ� */
        if ((simple || !CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY)) && (h->cbp & 0x30))
        {
            uint8_t *dest[2] = {dest_cb, dest_cr};

            if (transform_bypass)
            {
                if (IS_INTRA(mb_type) && h->sps.profile_idc == 244 && (h->chroma_pred_mode == VERT_PRED8x8 || h->chroma_pred_mode == HOR_PRED8x8))
                {
                    /* 2010/06/04 14:30:00 liuxw+00139685 */
                    /* ��ֹ���������Ӷ�h->chroma_pred_mode�Ϸ��Եļ�� */
                    if (h->chroma_pred_mode >= 3)
                    {
                        av_log(h->s.avctx, AV_LOG_WARNING, "chroma_pred_mode[%d] is invalid!\n", h->chroma_pred_mode);
                        IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                        h->s.avctx->iTotalError++;
                        return -1;
                    }

                    h->hpc.pred8x8_add[h->chroma_pred_mode](dest[0], block_offset + 16, h->mb + 16 * 16, uvlinesize);
                    h->hpc.pred8x8_add[h->chroma_pred_mode](dest[1], block_offset + 20, h->mb + 20 * 16, uvlinesize);
                }
                else
                {
                    idct_add = s->dsp.add_pixels4;

                    for (i = 16; i < 16 + 8; i++)
                    {
                        if (h->non_zero_count_cache[ scan8[i] ] || h->mb[i * 16])
                        { idct_add   (dest[(i & 4) >> 2] + block_offset[i], h->mb + i * 16, uvlinesize); }
                    }
                }
            }
            else
            {
                /* chroma dc��������IDCT */
                chroma_dc_dequant_idct_c(h->mb + 16 * 16, h->chroma_qp[0], h->dequant4_coeff[IS_INTRA(mb_type) ? 1 : 4][h->chroma_qp[0]][0]);
                chroma_dc_dequant_idct_c(h->mb + 16 * 16 + 4 * 16, h->chroma_qp[1], h->dequant4_coeff[IS_INTRA(mb_type) ? 2 : 5][h->chroma_qp[1]][0]);

                if (is_h264)
                {
                    idct_add = s->dsp.h264_idct_add;
                    idct_dc_add = s->dsp.h264_idct_dc_add;

                    for (i = 16; i < 16 + 8; i++)
                    {
                        if (h->non_zero_count_cache[ scan8[i] ])	/* chroma DC/AC IDCT ���ؽ� */
                        { idct_add   (dest[(i & 4) >> 2] + block_offset[i], h->mb + i * 16, uvlinesize); }
                        else if (h->mb[i * 16])	/* chroma DC IDCT���ؽ� */
                        { idct_dc_add(dest[(i & 4) >> 2] + block_offset[i], h->mb + i * 16, uvlinesize); }
                    }
                }
                else
                {
                    /* codec_id = svq3 */
                    for (i = 16; i < 16 + 8; i++)
                    {
                        if (h->non_zero_count_cache[ scan8[i] ] || h->mb[i * 16])
                        {
                            uint8_t *const ptr = dest[(i & 4) >> 2] + block_offset[i];
                            svq3_add_idct_c(ptr, h->mb + i * 16, uvlinesize, chroma_qp[s->qscale + 12] - 12, 2);
                        }
                    }
                }
            }
        }
    }

    if (h->cbp || IS_INTRA(mb_type))
    { s->dsp.clear_blocks(h->mb); }	/* ���block������ */

#ifdef LXW_DEBUG

    if (h->deblocking_filter == 0)
    {
        /* add by lxw for test */
        if (frame_num == TARGET_FRAME  && mb_x == MB_X && (mb_y == MB_Y || mb_y == MB_Y2))
        {
            av_log(NULL, AV_LOG_ERROR, "frame_num:%d poc:%d mb_x:%d mb_y:%d\n", frame_num, h->s.current_picture_ptr->field_poc[0], mb_x, mb_y);
            {
                int y_linesize = linesize;
                int i, j;
                unsigned char *ptr = dest_y;

                av_log(NULL, AV_LOG_ERROR, "Recon Value:\n ");

                for (i = 0; i < 16; i++)
                {
                    for (j = 0; j < 16; j++)
                    { av_log(NULL, AV_LOG_ERROR, "%d ", ptr[j]); }

                    av_log(NULL, AV_LOG_ERROR, "\n ");
                    ptr += y_linesize;
                }
            }
        }
    }

#endif

    /* ��·�˲� */
    if (h->deblocking_filter)
    {
        backup_mb_border(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize, simple);
        fill_caches(h, mb_type, 1); //FIXME don't fill stuff which isn't used by filter_mb
        h->chroma_qp[0] = get_chroma_qp(h, 0, s->current_picture.qscale_table[mb_xy]);
        h->chroma_qp[1] = get_chroma_qp(h, 1, s->current_picture.qscale_table[mb_xy]);

#ifdef LXW_DEBUG

        /* add by lxw for test */
        if (frame_num == TARGET_FRAME  && mb_x == MB_X && (mb_y == MB_Y || mb_y == MB_Y2))
        {
            av_log(NULL, AV_LOG_ERROR, "frame_num:%d poc:%d mb_x:%d mb_y:%d\n", frame_num, h->s.current_picture_ptr->field_poc[0], mb_x, mb_y);
            {
                int y_linesize = linesize;
                int i, j;
                unsigned char *ptr = dest_y;

                av_log(NULL, AV_LOG_ERROR, "Recon Value(Befoer LP):\n ");

                for (i = 0; i < 16; i++)
                {
                    for (j = 0; j < 16; j++)
                    { av_log(NULL, AV_LOG_ERROR, "%d ", ptr[j]); }

                    av_log(NULL, AV_LOG_ERROR, "\n ");
                    ptr += y_linesize;
                }
            }
        }

#endif

        if (!simple && FRAME_MBAFF)
        {
            /* ���ӵĻ�·�˲� */
            filter_mb(h, mb_x, mb_y, dest_y, dest_cb, dest_cr, linesize, uvlinesize);
        }
        else
        {
            /* �򵥵Ļ�·�˲� */
            filter_mb_fast(h, mb_x, mb_y, dest_y, dest_cb, dest_cr, linesize, uvlinesize);
        }

#ifdef LXW_DEBUG

        /* add by lxw for test */
        if (frame_num == TARGET_FRAME  && mb_x == MB_X && (mb_y == MB_Y || mb_y == MB_Y2))
        {
            int y_linesize = linesize;
            int i, j;
            unsigned char *ptr = dest_y;

            av_log(NULL, AV_LOG_ERROR, "Loop-Filterd Value:\n ");

            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < 16; j++)
                { av_log(NULL, AV_LOG_ERROR, "%d ", ptr[j]); }

                av_log(NULL, AV_LOG_ERROR, "\n ");
                ptr += y_linesize;
            }
        }

#endif
    }

    return iRet;
}

/**
 * Process a macroblock; this case avoids checks for expensive uncommon cases.
 */
static int hl_decode_mb_simple(H264Context *h)
{
    return hl_decode_mb_internal(h, 1);
}

/**
 * Process a macroblock; this handles edge cases, such as interlacing.
 */
static int av_noinline hl_decode_mb_complex(H264Context *h)
{
    return hl_decode_mb_internal(h, 0);
}
/* IDCT , ֡��Ԥ�⡢֡��Ԥ�⡢��·�˲�������ؽ� */
static int hl_decode_mb(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = h->mb_xy;
    const int mb_type = s->current_picture.mb_type[mb_xy];
    int is_complex = CONFIG_SMALL || h->is_complex || IS_INTRA_PCM(mb_type) || s->qscale == 0;
    int iRet = 0;

    if (is_complex)
    { iRet = hl_decode_mb_complex(h); }
    else { iRet = hl_decode_mb_simple(h); }

    if (iRet < 0) { return -1; }

    return iRet;
}

static void pic_as_field(Picture *pic, const int parity)
{
    int i;

    for (i = 0; i < 4; ++i)
    {
        if (parity == PICT_BOTTOM_FIELD)
        { pic->data[i] += pic->linesize[i]; }

        pic->reference = parity;
        pic->linesize[i] *= 2;
    }

    pic->poc = pic->field_poc[parity == PICT_BOTTOM_FIELD];
}

static int split_field_copy(Picture *dest, Picture *src,
                            int parity, int id_add)
{
    int match = !!(src->reference & parity);

    if (match)
    {
        *dest = *src;

        if (parity != PICT_FRAME)
        {
            pic_as_field(dest, parity);
            dest->pic_id *= 2;
            dest->pic_id += id_add;
        }
    }

    return match;
}

static int build_def_list(Picture *def, Picture **in, int len, int is_long, int sel)
{
    int i[2] = {0};
    int index = 0;

    while (i[0] < len || i[1] < len)
    {
        while (i[0] < len && !(in[ i[0] ] && (in[ i[0] ]->reference & sel)))
        { i[0]++; }

        while (i[1] < len && !(in[ i[1] ] && (in[ i[1] ]->reference & (sel ^ 3))))
        { i[1]++; }

        if (i[0] < len)
        {
            if (sel == 3 && in[ i[0] ]->reference != 3)
            {
                in[ i[0] ]->pic_id = is_long ? i[0] : in[ i[0] ]->frame_num;
                i[0]++;
                continue;
            }

            in[ i[0] ]->pic_id = is_long ? i[0] : in[ i[0] ]->frame_num;
            split_field_copy(&def[index++], in[ i[0]++ ], sel  , 1);
        }

        if (i[1] < len)
        {
            if (sel == 3 && in[ i[1] ]->reference != 3)
            {
                in[ i[1] ]->pic_id = is_long ? i[1] : in[ i[1] ]->frame_num;
                i[1]++;
                continue;
            }

            in[ i[1] ]->pic_id = is_long ? i[1] : in[ i[1] ]->frame_num;
            split_field_copy(&def[index++], in[ i[1]++ ], sel ^ 3, 0);
        }
    }

    return index;
}

static int add_sorted(Picture **sorted, Picture **src, int len, int limit, int dir)
{
    int i, best_poc;
    int out_i = 0;

    for (;;)
    {
        best_poc = dir ? INT_MIN : INT_MAX;

        for (i = 0; i < len; i++)
        {
            const int poc = src[i]->poc;

            if (((poc > limit) ^ dir) && ((poc < best_poc) ^ dir))
            {
                best_poc = poc;
                sorted[out_i] = src[i];
            }
        }

        if (best_poc == (dir ? INT_MIN : INT_MAX))
        { break; }

        limit = sorted[out_i++]->poc - dir;
    }

    return out_i;
}

/**
 * fills the default_ref_list.
 */
/* �ο�ͼ����еĳ�ʼ�������ڲο�֡������Ĭ�ϲο�ͼ���б������ liuxw+00139685 */
static int fill_default_ref_list(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int i, len;

    if (h->slice_type_nos == FF_B_TYPE)
    {
        Picture *sorted[32];  /* ���32���ο�ͼ�� liuxw+00139685 */
        int cur_poc, list;
        int lens[2];
        int tmp_poc, index = -1;

        /* 2010/08/18 13:30:00 liuxw+00139685 */
        /* ����ǳ�������£����ҵ�ǰ������һ���Ѿ���������Ϊ���ڲο������ڶ��ڲο�֡�����У����ҵ�ǰ����POCС���ѽ��볡��POC
           ,��ʱ����֡���ڵĶ��ڲο�֡�����е�POC�����޸ģ��޸�Ϊ��ǰ������һ����POC�����ұ�����ڲο������ź��޸�ǰ��POC��ֵ*/
        //		 if(FIELD_PICTURE)  /* ����ǰ�ǳ�����ȡ��ǰ����POC liuxw+00139685 */
        //			cur_poc= s->current_picture_ptr->field_poc[ s->picture_structure == PICT_BOTTOM_FIELD ];
        if (FIELD_PICTURE) /* ����ǰ�ǳ�����ȡ��ǰ����POC liuxw+00139685 */
        {
            int i;
            cur_poc = s->current_picture_ptr->field_poc[ s->picture_structure == PICT_BOTTOM_FIELD ];

            for (i = 0; i < h->short_ref_count; i++)
            {
                if (s->current_picture_ptr->coded_picture_number  == h->short_ref[i]->coded_picture_number && cur_poc == s->current_picture_ptr->poc)
                {
                    tmp_poc = h->short_ref[i]->poc;
                    h->short_ref[i]->poc = s->current_picture_ptr->field_poc[ s->picture_structure != PICT_BOTTOM_FIELD ];
                    index = i;
                }
            }
        }
        else	/* ����ǰ��֡����ȡ��ǰ֡��POC liuxw+00139685 */
        { cur_poc = s->current_picture_ptr->poc; }

        for (list = 0; list < 2; list++)
        {
            /* �ɶ��ڲο�֡���У�����DIR��������ǰpicture��ν��вο�֡��������
               dir=1: ��poc��cur_pocС�Ľ��н�������dir=0: ��poc��cru_poc��Ľ�����������
               list0: ������dir=1��Ȼ����dir=0��list1: ������dir=0��Ȼ����dir=1. list0liuxw+00139685 */
            len = add_sorted(sorted    , h->short_ref, h->short_ref_count, cur_poc, 1 ^ list);
            len += add_sorted(sorted + len, h->short_ref, h->short_ref_count, cur_poc, 0 ^ list);
            assert(len <= 32);
            /* ���ݵ�ǰpic�����Ժ������ο�֡���е������У����������������Ĭ�ϲο�ͼ�����
                �����ǰpic��֡����Ĭ�ϲο�ͼ����е��������ο�֡���У������ǰpic�Ƕ���������������ο�
            	֡���е�˳�����Ŷ������ٽ����ŵ׳���˳��������У������ǰpic�ǵ׳�������������ο�֡���е�
            	˳�����ŵ׳����ٽ����Ŷ�����˳��������С�							list0liuxw+00139685 */
            len = build_def_list(h->default_ref_list[list]    , sorted     , len, 0, s->picture_structure);
            /* ����ٽ����ڲο�֡���а���LongTermIdx����͵�ǰpic�����ԣ�������ν����ڲο�ͼ�����Ĭ�ϲο�ͼ�������
              �����ڶ��ڲο����еĺ��棩�����ǰpic��֡����ֱ�ӽ����ڲο����м��뵽Ĭ�ϲο�ͼ������У�����Ƕ�����
               ����ݳ��ڲο�֡���е�˳�����Ŷ��������ŵ׳����������У������ǰ���ǵ׳�������ݳ��ڲο�֡���е�˳��
               ���ŵ׳������Ŷ�������������.													    liuxw+00139685 */
            len += build_def_list(h->default_ref_list[list] + len, h->long_ref, 16 , 1, s->picture_structure);
            assert(len <= 32);

            /* �����ǰ�ο�ͼ����������ο�ͼ��ĸ�������Ĭ�ϲο�ͼ���б��еĸ�������δ�����Ĳο�ͼ����� liuxw+00139685 */
            if (len < (int)h->ref_count[list])
            { memset(&h->default_ref_list[list][len], 0, sizeof(Picture) * (h->ref_count[list] - len)); }

            lens[list] = len;
        }

        /* ���list0��list1�����ο�������ȫ��ͬ�����Ҳο�ͼ���������1������Ҫ��list1[0]��list1[1]����λ�ý��н���
           �ô�����������ο�ͼ����ͬ�����Ȼֻ��ȡ���е�һ���ο�ͼ���б���list1[0]��list1[1]λ�û����󣬵���ǰͼ��
           �ο�list0[0]��list1[0](��ǰ��list1[1])���򽫻�ʡ��list1��refidx��ռ�ı�������ֻ�Ǿٸ����ӣ�liuxw+00139685 */
        if (lens[0] == lens[1] && lens[1] > 1)
        {
            for (i = 0; h->default_ref_list[0][i].data[0] == h->default_ref_list[1][i].data[0] && i < lens[0]; i++);

            if (i == lens[0])
            { FFSWAP(Picture, h->default_ref_list[1][0], h->default_ref_list[1][1]); }
        }

        /* 2010/08/18 13:30:00 liuxw+00139685 */
        /* �������ڲο����н��й��޸ģ���������������޸Ļ��� */
        if (-1 != index)
        {
            h->short_ref[index]->poc = tmp_poc;
        }
    }
    else
    {
        /* P slice��Ĭ�ϲο�ͼ����е����򣨶��ڣ������ݵ�ǰͼ������ԺͶ��ڲ�֡���е����򣬾���Ĭ�ϲο�ͼ������ж��ڲο�ͼ�������
           ����˳��ͬB slice��list0�Ķ��ڲο�ͼ������																	liuxw+00139685 */
        len = build_def_list(h->default_ref_list[0]    , h->short_ref, h->short_ref_count, 0, s->picture_structure);
        /* P slice��Ĭ�ϲο�ͼ����е����򣨳��ڣ������ݵ�ǰͼ������Ժͳ��ڲ�֡���е����򣬾���Ĭ�ϲο�ͼ������г��ڲο�ͼ�������
        ����˳��ͬB slice��list0�ĳ��ڲο�����																	liuxw+00139685 */
        len += build_def_list(h->default_ref_list[0] + len, h-> long_ref, 16                , 1, s->picture_structure);
        assert(len <= 32);

        /* �����ǰ�ο�ͼ����������ο�ͼ��ĸ�������Ĭ�ϲο�ͼ���б��еĸ�������δ�����Ĳο�ͼ����� liuxw+00139685 */
        if (len < (int)h->ref_count[0])
        { memset(&h->default_ref_list[0][len], 0, sizeof(Picture) * (h->ref_count[0] - len)); }
    }

#ifdef TRACE    /* ����������Ҫ��trace��Ϣ��ӡ�ĸ�����ϸ����˼��ȷ liuxw+00139685 */

    for (i = 0; i < h->ref_count[0]; i++)
    {
        tprintf(h->s.avctx, "List0: %s fn:%d 0x%p\n", (h->default_ref_list[0][i].long_ref ? "LT" : "ST"), h->default_ref_list[0][i].pic_id, h->default_ref_list[0][i].data[0]);
    }

    if (h->slice_type_nos == FF_B_TYPE)
    {
        for (i = 0; i < h->ref_count[1]; i++)
        {
            tprintf(h->s.avctx, "List1: %s fn:%d 0x%p\n", (h->default_ref_list[1][i].long_ref ? "LT" : "ST"), h->default_ref_list[1][i].pic_id, h->default_ref_list[1][i].data[0]);
        }
    }

#endif
    return 0;
}

static void print_short_term(H264Context *h);
static void print_long_term(H264Context *h);

/**
 * Extract structure information about the picture described by pic_num in
 * the current decoding context (frame or field). Note that pic_num is
 * picture number without wrapping (so, 0<=pic_num<max_pic_num).
 * @param pic_num picture number for which to extract structure information
 * @param structure one of PICT_XXX describing structure of picture
 *                      with pic_num
 * @return frame number (short term) or long term index of picture
 *         described by pic_num
 */
static int pic_num_extract(H264Context *h, int pic_num, int *structure)
{
    MpegEncContext *const s = &h->s;

    *structure = s->picture_structure;

    if (FIELD_PICTURE)
    {
        if (!(pic_num & 1))
            /* opposite field */
        { *structure ^= PICT_FRAME; }

        pic_num >>= 1;
    }

    return pic_num;
}

static int decode_ref_pic_list_reordering(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int list, index, pic_structure;

    print_short_term(h);
    print_long_term(h);

    /* ���ź����Ĭ�ϲο�ͼ����п���������ʵ���õ��Ĳο�ͼ����� ֻ�´��ָ���Ĳο�ͼ��ĸ�����
       ���Ĭ�ϲο�ͼ����еĸ������㣬��᲻��Ĳο�ͼ��ȫ����0�� ���Ĭ�ϲο�ͼ����еĸ�������
       ����ֻ�´��ǰ��ָ���ĸ�����												liuxw+00139685 */
    for (list = 0; list < (int)h->list_count; list++)
    {
        memcpy(h->ref_list[list], h->default_ref_list[list], sizeof(Picture)*h->ref_count[list]);

        /* ref_pic_list_reordering_flag_l0��0������������()��ֱ��ʹ������������list0���������1������
           Ҫ���������򣬼�����ȡ������﷨Ԫ��										liuxw+00139685 */
        if (get_bits1(&s->gb))
        {
            int pred = h->curr_pic_num;      /* �õ���ǰͼ���pic_num�����ں��������ڲο�ͼ���pic_num liuxw+00139685 */

            for (index = 0; ; index++)
            {
                unsigned int reordering_of_pic_nums_idc = get_ue_golomb_31(&s->gb);
                unsigned int pic_id;
                int i;
                Picture *ref = NULL;

                /* �����������־ liuxw+00139685 */
                if (reordering_of_pic_nums_idc == 3)
                { break; }

                if (index >= (int)h->ref_count[list])
                {
                    //                   v_log(h->s.avctx, AV_LOG_ERROR, "reference count overflow\n");
                    av_log(h->s.avctx, AV_LOG_WARNING, "reference count overflow\n");
                    return -1;
                }

                if (reordering_of_pic_nums_idc < 3)
                {
                    /* ���ڲο�ͼ�������� liuxw+00139685 */
                    if (reordering_of_pic_nums_idc < 2)
                    {
                        const unsigned int abs_diff_pic_num = get_ue_golomb(&s->gb) + 1;
                        int frame_num;

                        if (abs_diff_pic_num > (unsigned int)h->max_pic_num)
                        {
                            //                          av_log(h->s.avctx, AV_LOG_ERROR, "abs_diff_pic_num overflow\n");
                            av_log(h->s.avctx, AV_LOG_WARNING, "abs_diff_pic_num overflow\n");
                            return -1;
                        }

                        /* �õ���ǰҪ����������� ���ڲο�ͼ���pic_num liuxw+00139685 */
                        if (reordering_of_pic_nums_idc == 0) { pred -= abs_diff_pic_num; }
                        else                                { pred += abs_diff_pic_num; }

                        pred &= h->max_pic_num - 1;

                        /* Ѱ�ҵ�ǰҪ��������ͼ���frame_num liuxw+00139685 */
                        frame_num = pic_num_extract(h, pred, &pic_structure);

                        /* �ڶ��ڲο�֡������Ѱ��frame_num���ڵ�ǰ���ڲο�ͼ���frame_num������ liuxw+00139685 */
                        for (i = h->short_ref_count - 1; i >= 0; i--)
                        {
                            ref = h->short_ref[i];
                            assert(ref->reference);
                            assert(!ref->long_ref);

                            if (
                                ref->frame_num == frame_num &&
                                (ref->reference & pic_structure)
                            )
                            { break; }
                        }

                        /* ���ҵ��ˣ����µģ�Ԥ����㣩pic_num������ liuxw+00139685 */
                        if (i >= 0)
                        { ref->pic_id = pred; }
                    }
                    else
                    {
                        int long_idx;
                        /* �õ���Ҫ������ĳ��ڲο�ͼ���pic_idx liuxw+00139685 */
                        pic_id = get_ue_golomb(&s->gb); //long_term_pic_idx

                        /* �õ����ڲο�֡���е����� liuxw+00139685 */
                        long_idx = pic_num_extract(h, pic_id, &pic_structure);

                        if (long_idx > 31)
                        {
                            //                          av_log(h->s.avctx, AV_LOG_ERROR, "long_term_pic_idx[%d] overflow\n",long_idx);
                            av_log(h->s.avctx, AV_LOG_WARNING, "long_term_pic_idx[%d] overflow\n", long_idx);
                            return -1;
                        }

                        /* �õ���ǰҪ����������ĳ��ڲο�ͼ�����ڵ�֡ liuxw+00139685 */
                        ref = h->long_ref[long_idx];
                        assert(!(ref && !ref->reference));

                        /* �����ڣ����޸ĵ�ǰ���ڲο�ͼ���pic_id liuxw+00139685 */
                        if (ref && (ref->reference & pic_structure))
                        {
                            ref->pic_id = pic_id;
                            assert(ref->long_ref);
                            i = 0;
                        }
                        else
                        {
                            i = -1;
                        }
                    }

                    /* ��Ҫ����������Ķ��ڻ��ڲο�֡δ�ڶ��ڲο�֡���л��ڲο�֡�������ҵ����򱨴� liuxw+00139685 */
                    if (i < 0)
                    {
                        //                      av_log(h->s.avctx, AV_LOG_ERROR, "reference picture missing during reorder\n");
                        /* 2010/03/22 17:00:00 liuxw+00139685 */
                        /* ���Ӵ����� */
                        av_log(h->s.avctx, AV_LOG_WARNING, "reference picture missing during reorder\n");
                        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
                        s->avctx->iTotalError++;
                        memset(&h->ref_list[list][index], 0, sizeof(Picture)); //FIXME
                    }
                    else
                    {
                        /* Ѱ�Ҷ��ڻ��ڲο�ͼ���ڲο�ͼ������е�λ�� liuxw+00139685 */
                        for (i = index; i + 1 < (int)h->ref_count[list]; i++)
                        {
                            if (ref->long_ref == h->ref_list[list][i].long_ref && ref->pic_id == h->ref_list[list][i].pic_id)
                            { break; }
                        }

                        /* ���ڵ�ǰ�ο�ͼ��ǰ��Ķ��ڻ��ڲο�ͼ������������һ��λ�� liuxw+00139685 */
                        for (; i > index; i--)
                        {
                            h->ref_list[list][i] = h->ref_list[list][i - 1];
                        }

                        /* ����ǰ�ο�ͼ�������������λ�ã��ǽṹ�帳ֵ������ָ�븳ֵ��liuxw+00139685 */
                        h->ref_list[list][index] = *ref;

                        /* ��Ϊref�ǴӶ��ڲο�֡�������ҵ��ģ�������֡�ο�����ʽ������ǰ�ǳ�������Ҫ���䣨�Ѵ���ڲο�ͼ������У�
                           �ο�������(top or bottom)��poc��linesize��data��ʼ��ַ������Ӧ���޸�  liuxw+00139685 */
                        if (FIELD_PICTURE)
                        {
                            pic_as_field(&h->ref_list[list][index], pic_structure);
                        }
                    }
                }
                else
                {
                    /* ������command index���� liuxw+00139685 */
                    //                  av_log(h->s.avctx, AV_LOG_ERROR, "illegal reordering_of_pic_nums_idc\n");
                    av_log(h->s.avctx, AV_LOG_WARNING, "illegal reordering_of_pic_nums_idc\n");
                    return -1;
                }
            }
        }
    }

    /* �Բο�ͼ����н�����֤ liuxw+00139685 */
    for (list = 0; list < (int)h->list_count; list++)
    {
        for (index = 0; index < (int)h->ref_count[list]; index++)
        {
            /* 2010/03/13 10:40:00 liuxw+00139685 */
            /* ����û�б�Ҫ������������Ϊ��Ĭ��(��ʼ)�ο������еĸ���С��ref_countʱ������Ļᱻ�ÿգ���Э��8.2.4.1
               ??? ������������ֺ󣬲�֪���ں�����ѡ��ο�ͼ������ʱ��û�м�� */
            if (!h->ref_list[list][index].data[0])
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "Missing reference picture\n");
                av_log(h->s.avctx, AV_LOG_WARNING, "Missing reference picture\n");
                h->s.avctx->iTotalError++;
                IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
                h->ref_list[list][index] = s->current_picture; //FIXME this is not a sensible solution
            }
        }
    }

    return 0;
}

static void fill_mbaff_ref_list(H264Context *h)
{
    int list, i, j;
    int field;

    /* 2010/03/13 17:35:00 liuxw+00139685 */
    /* ��ѭ�������޸�Ϊh->list_countЧ�ʻ���� */
    //  for(list=0; list<2; list++){ //FIXME try list_count
    for (list = 0; list < (int)h->list_count; list++)
    {
        for (i = 0; i < (int)h->ref_count[list]; i++)
        {
            /* ref_list[0��15]��֡�ο����У�ref_list[16��47]�����ο�����(������ǰ�� liuxw+00139685 */
            /* ???����ǰ���Ƕ�������ref_list[16��47]����ֱ��ʹ�ã����ǵ׳�����֪���������ʹ��(�׳�ʱ�ο����еĵ׳���ǰ) */
            Picture *frame = &h->ref_list[list][i];
            Picture *field = &h->ref_list[list][16 + 2 * i];
            field[0] = *frame;

            /* ���ο������е�linesizeҪ������������֡��ȡ���У�liuxw+00139685 */
            for (j = 0; j < 3; j++)
            { field[0].linesize[j] <<= 1; }

            field[0].reference = PICT_TOP_FIELD;
            field[0].poc = field[0].field_poc[0];
            field[1] = field[0];

            /* �׳�YUV�����붥��YUV������ʼ��ַ��ƫ�� liuxw+00139685 */
            for (j = 0; j < 3; j++)
            { field[1].data[j] += frame->linesize[j]; }

            field[1].reference = PICT_BOTTOM_FIELD;
            field[1].poc = field[1].field_poc[1];

            /* ��ʾ��Ȩ��������׳�����֡�ļ�Ȩ��ͬ
               0-15���֡�ļ�Ȩ��16��47 ��ų��ļ�Ȩ liuxw+00139685 */
            /* 2010/08/11 19:30:00 liuxw+00139685 */
            /* ��P֡��ʾ��Ȩ��ʱ��MBAFF�ҵ�ǰ����ǳ����ʱ�����ڴ���16��Ȩֵû�н��и�ֵ��Ĭ��Ϊ0�ݣ����Լ�Ȩ��ȫΪ0 */
            //			if(1 == h->pps.weighted_bipred_idc)
            if (1 == h->pps.weighted_pred || 1 == h->pps.weighted_bipred_idc)
            {
                h->luma_weight[list][16 + 2 * i] = h->luma_weight[list][16 + 2 * i + 1] = h->luma_weight[list][i];
                h->luma_offset[list][16 + 2 * i] = h->luma_offset[list][16 + 2 * i + 1] = h->luma_offset[list][i];

                for (j = 0; j < 2; j++)
                {
                    h->chroma_weight[list][16 + 2 * i][j] = h->chroma_weight[list][16 + 2 * i + 1][j] = h->chroma_weight[list][i][j];
                    h->chroma_offset[list][16 + 2 * i][j] = h->chroma_offset[list][16 + 2 * i + 1][j] = h->chroma_offset[list][i][j];
                }
            }
        }
    }

    /* ��ʽ��Ȩimplicit_weight[48][48]��������׳�����֡�ļ�Ȩ��ͬ
       implicit_weight[0-15][0-15]: ֡����ʽ��Ȩ
       implicit_weight[16-47][16-47]��������ʽ��Ȩ liuxw+00139685 */
    /* ??? ֮ǰ���ʱ���Ǵ��implicit_weight[ref0][ref1]��������ȷ��implicit_weight[ref1][ref0] */

    /* 2010/05/4 10:00:00 liuxw+00139685 [AZ1D02057] */
    /* �޸�B֡��ʽ��Ȩ������ */
    /*  for(j=0; j<(int)h->ref_count[1]; j++){
            for(i=0; i<(int)h->ref_count[0]; i++)
    			h->implicit_weight[j][16+2*i] = h->implicit_weight[j][16+2*i+1] = h->implicit_weight[j][i];
    		memcpy(h->implicit_weight[16+2*j],   h->implicit_weight[j], sizeof(*h->implicit_weight));
    		memcpy(h->implicit_weight[16+2*j+1], h->implicit_weight[j], sizeof(*h->implicit_weight));
    	}
    */
    /* 2010/08/23 14:00:00 liuxw+00139685 */
    /* �����ǰΪp slice����ôû�б�Ҫ�ٽ��м��� */
    if (FF_B_TYPE == h->slice_type_nos && 2 == h->pps.weighted_bipred_idc)
    {
        for (j = 0; j < (int)h->ref_count[0]; j++)
        { memcpy(h->implicit_weight[1][j], h->implicit_weight[0][j], sizeof(*h->implicit_weight[0][0])); }

        for (field = 0; field < 2; field++)
        {
            int ref0, ref1;
            int cur_poc = h->s.current_picture_ptr->field_poc[field];

            for (ref0 = 0; ref0 < (int)h->ref_count[0] * 2; ref0++)
            {
                int poc0 = h->ref_list[0][(ref0 ^ field) + 16].poc;

                if (1 == h->ref_list[0][(ref0 ^ field) + 16].long_ref)
                {
                    for (ref1 = 0; ref1 < (int)h->ref_count[1] * 2; ref1++)
                    {
                        h->implicit_weight[field][(ref0 ^ field) + 16][(ref1 ^ field) + 16] = 32;
                    }

                    continue;
                }

                for (ref1 = 0; ref1 < (int)h->ref_count[1] * 2; ref1++)
                {
                    int poc1 = h->ref_list[1][(ref1 ^ field) + 16].poc;
                    int td = av_clip(poc1 - poc0, -128, 127);

                    if (1 == h->ref_list[1][(ref1 ^ field) + 16].long_ref)
                    {
                        h->implicit_weight[field][(ref0 ^ field) + 16][(ref1 ^ field) + 16] = 32;
                        continue;
                    }

                    if (td)
                    {
                        int tb = av_clip(cur_poc - poc0, -128, 127);
                        int tx = (16384 + (FFABS(td) >> 1)) / td;
                        int dist_scale_factor = av_clip((tb * tx + 32) >> 6, -1024, 1023) >> 2;

                        /* tb=DiffPoc(Curpic,pic0); td = DiffPoc(pic1,pic0); dist_scale_factor=(const)*tb/td;
                        dist_scale_factor < -64: tb>0&&td<0&&|tb|>|td| || tb<0&&td>0&&|tb|>|td|
                        dist_scale_factor > 128: tb>0&&td>0&&|tb|>|td| || tb<0&&td<0&&|tb|>|td| liuxw+00139685 */
                        if (dist_scale_factor < -64 || dist_scale_factor > 128 || dist_scale_factor == 32)
                        { h->implicit_weight[field][(ref0 ^ field) + 16][(ref1 ^ field) + 16] = 32; }
                        else
                        {
                            h->implicit_weight[field][(ref0 ^ field) + 16][(ref1 ^ field) + 16] = 64 - dist_scale_factor;
                            h->use_weight = 2;
                        }
                    }
                    else
                    { h->implicit_weight[field][(ref0 ^ field) + 16][(ref1 ^ field) + 16] = 32; }
                }
            }
        }
    }
}


/* ��ʾ��Ȩ��P֡��ʾ��Ȩ��B֡��ʾ��Ȩ��: �õ�luma_weight_flag��luma_log2_weight_denom��luma_weight��luma_offset
chroma_weight_flag��chroma_log2_weight_denom��chroma_weight��chroma_offset  liuxw+00139685 */
static int pred_weight_table(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int list, i;
    int luma_def, chroma_def;

    h->use_weight = 0;
    h->use_weight_chroma = 0;
    h->luma_log2_weight_denom = get_ue_golomb(&s->gb);

    /* 2010/04/06 15:20:00 liuxw+00139685 */
    /* ���Ӷ�luma_log2_weight_denom�Ϸ��Եļ�� */
    if (h->luma_log2_weight_denom > 7)
    {
        av_log(s->avctx, AV_LOG_WARNING, "luma_log2_weight_denom[%d] is invalid!\n", h->luma_log2_weight_denom);
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_WEIGHTED);
        s->avctx->iTotalError++;
        h->luma_log2_weight_denom = 0;
        //		return -1;
    }

    /* 2010/03/08 9:20:00 liuxw+00139685 */
    /* ���Ӷ�chroma�Ƿ���ڵ��ж� */
    //	 h->chroma_log2_weight_denom= get_ue_golomb(&s->gb);
    if (CHROMA)
    {
        h->chroma_log2_weight_denom = get_ue_golomb(&s->gb);

        /* 2010/04/06 15:20:00 liuxw+00139685 */
        /* ���Ӷ�chroma_log2_weight_denom�Ϸ��Եļ�� */
        if (h->chroma_log2_weight_denom > 7)
        {
            av_log(s->avctx, AV_LOG_WARNING, "chroma_log2_weight_denom[%d] is invalid!\n", h->chroma_log2_weight_denom);
            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_WEIGHTED);
            s->avctx->iTotalError++;
            h->luma_log2_weight_denom = 0;
            //			return -1;
        }
    }

    luma_def = 1 << h->luma_log2_weight_denom;
    chroma_def = 1 << h->chroma_log2_weight_denom;

    for (list = 0; list < 2; list++)
    {
        h->luma_weight_flag[list]   = 0;
        h->chroma_weight_flag[list] = 0;

        for (i = 0; i < (int)h->ref_count[list]; i++)
        {
            int luma_weight_flag, chroma_weight_flag;

            luma_weight_flag = get_bits1(&s->gb);

            /* �����Ȩϵ�����ڣ�������������ж���Ȩϵ����ƫ�� liuxw+00139685 */
            if (luma_weight_flag)
            {
                h->luma_weight[list][i] = get_se_golomb(&s->gb);
                h->luma_offset[list][i] = get_se_golomb(&s->gb);

                /* 2010/04/06 15:20:00 liuxw+00139685 */
                /* ���Ӷ�luma_weight/luma_offset�Ϸ��Եļ�� */
                if (h->luma_weight[list][i] > 127 || h->luma_weight[list][i] < -128 || h->luma_offset[list][i] > 127 || h->luma_offset[list][i] < -128)
                {
                    av_log(s->avctx, AV_LOG_WARNING, "luma_weight[%d]/luma_offset[%d] is invalid!\n", h->luma_weight[list][i], h->luma_offset[list][i]);
                    IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_WEIGHTED);
                    s->avctx->iTotalError++;
                    h->luma_weight[list][i] = h->luma_offset[list][i] = 0;
                    //					return -1;
                }

                /* �����i���ο�֡�ļ�Ȩϵ��������Ĭ��ֵ����ƫ�����0����ô��ǰ�ο�֡ʹ���˼�Ȩ��
                   ����ǰ�ο�֡ʹ��Ĭ�ϼ�Ȩ��liuxw+00139685 */
                if (   h->luma_weight[list][i] != luma_def
                       || h->luma_offset[list][i] != 0)
                {
                    h->use_weight = 1;
                    h->luma_weight_flag[list] = 1;
                }
            }
            /* �����Ȩϵ�������ڣ��򽫼�Ȩϵ����ƫ��ֱ�Ӹ�ֵ liuxw+00139685 */
            else
            {
                h->luma_weight[list][i] = luma_def;
                h->luma_offset[list][i] = 0;
            }

            /* ���ɫ��(UV)���ڣ����ж�chroma�Ƿ��Ȩ */
            if (CHROMA)
            {
                chroma_weight_flag = get_bits1(&s->gb);

                /* �����Ȩϵ�����ڣ�������������ж���Ȩϵ����ƫ�� liuxw+00139685 */
                if (chroma_weight_flag)
                {
                    int j;

                    for (j = 0; j < 2; j++)
                    {
                        h->chroma_weight[list][i][j] = get_se_golomb(&s->gb);
                        h->chroma_offset[list][i][j] = get_se_golomb(&s->gb);

                        /* 2010/04/06 15:20:00 liuxw+00139685 */
                        /* ���Ӷ�chroma_weight/chroma_offset�Ϸ��Եļ�� */
                        if (h->chroma_weight[list][i][j] > 127 || h->chroma_weight[list][i][j] < -128 || h->chroma_offset[list][i][j] > 127 || h->chroma_offset[list][i][j] < -128)
                        {
                            av_log(s->avctx, AV_LOG_WARNING, "chroma_weight[%d]/chroma_offset[%d] is invalid!\n", h->chroma_weight[list][i][j], h->chroma_offset[list][i][j]);
                            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_WEIGHTED);
                            s->avctx->iTotalError++;
                            h->chroma_weight[list][i][j] = h->chroma_offset[list][i][j] = 0;
                            //							return -1;
                        }

                        /* �����i���ο�֡�ļ�Ȩϵ��������Ĭ��ֵ����ƫ�����0����ô��ǰ�ο�֡ʹ���˼�Ȩ��
                        ����ǰ�ο�֡ʹ��Ĭ�ϼ�Ȩ��liuxw+00139685 */
                        if (   h->chroma_weight[list][i][j] != chroma_def
                               || h->chroma_offset[list][i][j] != 0)
                        {
                            h->use_weight_chroma = 1;
                            h->chroma_weight_flag[list] = 1;
                        }
                    }
                }
                /* �����Ȩϵ�������ڣ��򽫼�Ȩϵ����ƫ��ֱ�Ӹ�ֵ liuxw+00139685 */
                else
                {
                    int j;

                    for (j = 0; j < 2; j++)
                    {
                        h->chroma_weight[list][i][j] = chroma_def;
                        h->chroma_offset[list][i][j] = 0;
                    }
                }
            }
        }

        /* �����P֡��ʾ��Ȩ����ֱ���˳�������B֡��ʾ��Ȩ�����������list1�ļ�Ȩϵ�� liuxw+00139685 */
        if (h->slice_type_nos != FF_B_TYPE) { break; }
    }

    /* ���ü�Ȩ��־��0��Ĭ�ϼ�Ȩ 1����ʾ��Ȩ 2����ʽ��Ȩ liuxw+00139685 */
    h->use_weight = h->use_weight || h->use_weight_chroma;
    return 0;
}

/* B֡��ʽ��Ȩ��luma_weight��chroma_weight liuxw+00139685 */
static void implicit_weight_table(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int ref0, ref1, i;
    /* 2010/30/10 14:48:00 liuxw+00139685 */
    /* �޸ĵ�ǰpicture��poc���޸�ǰ������ǰpicture�ǵ׳�ʱ����poc�����ڵ�ǰframe��poc���ʵ��º��������ʽȨֵ�Ĵ��� */
    //int cur_poc = s->picture_structure ==s->current_picture_ptr->poc;
    int cur_poc = s->picture_structure == PICT_FRAME ? s->current_picture_ptr->poc : s->current_picture_ptr->field_poc[s->picture_structure - 1];

    /* ��ռ�Ȩ�ı�־ liuxw+00139685 */
    for (i = 0; i < 2; i++)
    {
        h->luma_weight_flag[i]   = 0;
        h->chroma_weight_flag[i] = 0;
    }

    /* ǰ�������ο�֡���ж�ֻ��һ���ο�֡�����������ο�֡��ʱ�������ǵ�ǰ֡Ϊ���ĶԳƵģ������Ȩ��Ĭ�ϼ�Ȩ��ƽ���� liuxw+00139685 */
    if (   h->ref_count[0] == 1 && h->ref_count[1] == 1
           && h->ref_list[0][0].poc + h->ref_list[1][0].poc == 2 * cur_poc)
    {
        h->use_weight = 0;
        h->use_weight_chroma = 0;
        /* 2010/05/4 10:00:00 liuxw+00139685 [AZ1D02057] */
        /* �޸�B֡��ʽ��Ȩ������ */
        h->implicit_weight[0][0][0] = 32;
        return;
    }

    /* ��λ��ʽ��Ȩ��־ liuxw+00139685 */
    h->use_weight = 2;
    h->use_weight_chroma = 2;
    h->luma_log2_weight_denom = 5;
    h->chroma_log2_weight_denom = 5;

    for (ref0 = 0; ref0 < (int)h->ref_count[0]; ref0++)
    {
        int poc0 = h->ref_list[0][ref0].poc;

        /* 2010/30/08 9:20:00 liuxw+00139685 */
        /* �������룺�����ǰ�ο�֡(list0)Ϊ���ڲο�֡�����֡����Ӧlist1�����еĲο�֡������Ĭ�ϼ�Ȩ */
        if (1 == h->ref_list[0][ref0].long_ref)
        {
            for (ref1 = 0; ref1 < (int)h->ref_count[1]; ref1++)
            {
                h->implicit_weight[0][ref0][ref1] = 32;
            }

            continue;
        }

        for (ref1 = 0; ref1 < (int)h->ref_count[1]; ref1++)
        {
            int poc1 = h->ref_list[1][ref1].poc;
            int td = av_clip(poc1 - poc0, -128, 127);

            /* �������룺�����ǰ�ο�֡(list0)Ϊ���ڲο�֡�������Ĭ�ϼ�Ȩ */
            if (1 == h->ref_list[1][ref1].long_ref)
            {
                h->implicit_weight[0][ref0][ref1] = 32;
                continue;
            }

            if (td)
            {
                int tb = av_clip(cur_poc - poc0, -128, 127);
                int tx = (16384 + (FFABS(td) >> 1)) / td;
                int dist_scale_factor = av_clip((tb * tx + 32) >> 6, -1024, 1023) >> 2;

                /* tb=DiffPoc(Curpic,pic0); td = DiffPoc(pic1,pic0); dist_scale_factor=(const)*tb/td;
                	dist_scale_factor < -64: tb>0&&td<0&&|tb|>|td| || tb<0&&td>0&&|tb|>|td|
                	dist_scale_factor > 128: tb>0&&td>0&&|tb|>|td| || tb<0&&td<0&&|tb|>|td| liuxw+00139685 */
                if (dist_scale_factor < -64 || dist_scale_factor > 128)
                { h->implicit_weight[0][ref0][ref1] = 32; }
                else
                { h->implicit_weight[0][ref0][ref1] = 64 - dist_scale_factor; }
            }
            /* ���ǰ��ͺ���ο�����ʹ��һ֡�������Ĭ�ϼ�Ȩ(ƽ��) liuxw+00139685 */
            else
            { h->implicit_weight[0][ref0][ref1] = 32; }
        }
    }
}

/**
 * Mark a picture as no longer needed for reference. The refmask
 * argument allows unreferencing of individual fields or the whole frame.
 * If the picture becomes entirely unreferenced, but is being held for
 * display purposes, it is marked as such.
 * @param refmask mask of fields to unreference; the mask is bitwise
 *                anded with the reference marking of pic
 * @return non-zero if pic becomes entirely unreferenced (except possibly
 *         for display purposes) zero if one of the fields remains in
 *         reference
 */
static inline int unreference_pic(H264Context *h, Picture *pic, int refmask)
{
    int i;

    if (pic->reference &= refmask)
    {
        return 0;
    }
    else
    {
        for (i = 0; h->delayed_pic[i]; i++)
            if (pic == h->delayed_pic[i])
            {
                pic->reference = DELAYED_PIC_REF;
                break;
            }

        return 1;
    }
}

/**
 * instantaneous decoder refresh.
 */
static void idr(H264Context *h)
{
    int i;

    for (i = 0; i < 16; i++)
    {
        remove_long(h, i, 0);
    }

    assert(h->long_ref_count == 0);

    for (i = 0; i < h->short_ref_count; i++)
    {
        unreference_pic(h, h->short_ref[i], 0);
        h->short_ref[i] = NULL;
    }

    h->short_ref_count = 0;
    h->prev_frame_num = 0;
    h->prev_frame_num_offset = 0;
    h->prev_poc_msb =
        h->prev_poc_lsb = 0;
}

/* forget old pics after a seek */
static void flush_dpb(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;
    int i;

    /* ���delay buffer */
    for (i = 0; i < MAX_DELAYED_PIC_COUNT; i++)
    {
        if (h->delayed_pic[i])
        { h->delayed_pic[i]->reference = 0; }

        h->delayed_pic[i] = NULL;
    }

    h->outputed_poc = INT_MIN;
    /* ��ն��ڲο����кͳ��ڲο����� */
    idr(h);

    /* ��յ�ǰͼ�� */
    if (h->s.current_picture_ptr)
    { h->s.current_picture_ptr->reference = 0; }

    h->s.first_field = 0;
    /* ��λSEI */
    reset_sei(h);
    /* ���YUV buffer���к�״̬��Ϣ */
    ff_mpeg_flush(avctx);

}

/**
 * Find a Picture in the short term reference list by frame number.
 * @param frame_num frame number to search for
 * @param idx the index into h->short_ref where returned picture is found
 *            undefined if no picture found.
 * @return pointer to the found picture, or NULL if no pic with the provided
 *                 frame number is found
 */
static Picture *find_short(H264Context *h, int frame_num, int *idx)
{
    MpegEncContext *const s = &h->s;
    int i;

    for (i = 0; i < h->short_ref_count; i++)
    {
        Picture *pic = h->short_ref[i];

        if (s->avctx->debug & FF_DEBUG_MMCO)
        { av_log(h->s.avctx, AV_LOG_DEBUG, "%d %d %p\n", i, pic->frame_num, pic); }

        if (pic->frame_num == frame_num)
        {
            *idx = i;
            return pic;
        }
    }

    return NULL;
}

/**
 * Remove a picture from the short term reference list by its index in
 * that list.  This does no checking on the provided index; it is assumed
 * to be valid. Other list entries are shifted down.
 * @param i index into h->short_ref of picture to remove.
 */
static void remove_short_at_index(H264Context *h, int i)
{
    assert(i >= 0 && i < h->short_ref_count);
    h->short_ref[i] = NULL;

    if (--h->short_ref_count)
    { memmove(&h->short_ref[i], &h->short_ref[i + 1], (h->short_ref_count - i)*sizeof(Picture *)); }
}

/**
 *
 * @return the removed picture or NULL if an error occurs
 */
static Picture *remove_short(H264Context *h, int frame_num, int ref_mask)
{
    MpegEncContext *const s = &h->s;
    Picture *pic;
    int i;

    if (s->avctx->debug & FF_DEBUG_MMCO)
    { av_log(h->s.avctx, AV_LOG_DEBUG, "remove short %d count %d\n", frame_num, h->short_ref_count); }

    pic = find_short(h, frame_num, &i);

    if (pic)
    {
        if (unreference_pic(h, pic, ref_mask))
        { remove_short_at_index(h, i); }
    }

    return pic;
}

/**
 * Remove a picture from the long term reference list by its index in
 * that list.
 * @return the removed picture or NULL if an error occurs
 */
static Picture *remove_long(H264Context *h, int i, int ref_mask)
{
    Picture *pic;

    pic = h->long_ref[i];

    if (pic)
    {
        if (unreference_pic(h, pic, ref_mask))
        {
            assert(h->long_ref[i]->long_ref == 1);
            h->long_ref[i]->long_ref = 0;
            h->long_ref[i] = NULL;
            h->long_ref_count--;
        }
    }

    return pic;
}

/**
 * print short term list
 */
static void print_short_term(H264Context *h)
{
    uint32_t i;

    if (h->s.avctx->debug & FF_DEBUG_MMCO)
    {
        av_log(h->s.avctx, AV_LOG_DEBUG, "short term list:\n");

        for (i = 0; i < (unsigned int)h->short_ref_count; i++)
        {
            Picture *pic = h->short_ref[i];
            av_log(h->s.avctx, AV_LOG_DEBUG, "%d fn:%d poc:%d %p\n", i, pic->frame_num, pic->poc, pic->data[0]);
        }
    }
}

/**
 * print long term list
 */
static void print_long_term(H264Context *h)
{
    uint32_t i;

    if (h->s.avctx->debug & FF_DEBUG_MMCO)
    {
        av_log(h->s.avctx, AV_LOG_DEBUG, "long term list:\n");

        for (i = 0; i < 16; i++)
        {
            Picture *pic = h->long_ref[i];

            if (pic)
            {
                av_log(h->s.avctx, AV_LOG_DEBUG, "%d fn:%d poc:%d %p\n", i, pic->frame_num, pic->poc, pic->data[0]);
            }
        }
    }
}

/**
 * Executes the reference picture marking (memory management control operations).
 */
static int execute_ref_pic_marking(H264Context *h, MMCO *mmco, int mmco_count)
{
    MpegEncContext *const s = &h->s;
    int i, j;
    int current_ref_assigned = 0;
    Picture *av_uninit(pic);

    if ((s->avctx->debug & FF_DEBUG_MMCO) && mmco_count == 0)
    { av_log(h->s.avctx, AV_LOG_DEBUG, "no mmco here\n"); }

    for (i = 0; i < mmco_count; i++)
    {
        int structure, av_uninit(frame_num);

        if (s->avctx->debug & FF_DEBUG_MMCO)
        { av_log(h->s.avctx, AV_LOG_DEBUG, "mmco:%d %d %d\n", h->mmco[i].opcode, h->mmco[i].short_pic_num, h->mmco[i].long_arg); }

        /* ��������������أ����ȵõ����ڲο�ͼ�� liuxw+00139685 */
        if (   mmco[i].opcode == MMCO_SHORT2UNUSED
               || mmco[i].opcode == MMCO_SHORT2LONG)
        {
            /* �ڶ��ڲο�֡�������ҵ�pic_num����short_pic_num����һͼ���frame_num liuxw+00139685 */
            frame_num = pic_num_extract(h, mmco[i].short_pic_num, &structure);
            /* �ڶ��ڲο�֡�������ҵ�frame_num����һ֡ liuxw+00139685 */
            pic = find_short(h, frame_num, &j);

            if (!pic)
            {
                /* �����ǰָ���Ķ��ڲο�ͼ���ڶ��ڲο������У�����������������֮һ��
                   1����ǰ������MMCO_SHORT2UNUSED��2����ǰ������MMCO_SHORT2LONG����ָ���ĳ��ڲο�֡
                   �����ڣ�3��ǰ������MMCO_SHORT2LONG����ָ���ĳ��ڲο�֡���ڵ���frame_num�����ڽ�Ҫ��
                   ��Ϊ���ڲο�֡��frame_num���ͻᱨ������ǰ������MMCO_SHORT2LONG������ָ���ĳ���
                   �ο�֡���ڣ��ڴ棩���Ҿ��ǽ�Ҫ����Ϊ���ڲο�����һ֡���򲻻ᱨ�� liuxw+00139685 */
                if (mmco[i].opcode != MMCO_SHORT2LONG || !h->long_ref[mmco[i].long_arg]
                    || h->long_ref[mmco[i].long_arg]->frame_num != frame_num)
                    //              av_log(h->s.avctx, AV_LOG_ERROR, "mmco: unref short failure\n");
                { av_log(h->s.avctx, AV_LOG_WARNING, "mmco: unref short failure\n"); }

                IMEDIA_SET_ERR_SLICE(h->s.avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
                h->s.avctx->iTotalError++;
                continue;
            }
        }

        switch (mmco[i].opcode)
        {
                /* �����ǰ��֡���򽫶��ڲο�֡������pic_num��С����һ֡ȥ���������ǰ�ǳ�����shor_pic_num����һ��
                   ��Ϊ�����ο��������һ����һ���ο�֡�е�һ�����ǻ����ο����е�һ������ô���ο�֡�򻥲��ο���Ҳ��Ϊ
                   �����ο���������һ���Ĳο����Բ��䡣liuxw+00139685 */
            case MMCO_SHORT2UNUSED:
                if (s->avctx->debug & FF_DEBUG_MMCO)
                { av_log(h->s.avctx, AV_LOG_DEBUG, "mmco: unref short %d count %d\n", h->mmco[i].short_pic_num, h->short_ref_count); }

                /* ��frame_num�ж�Ӧ�Ĳο�����(֡���������׳�)���ڲο�֡������ȥ�� liuxw+00139685 */
                remove_short(h, frame_num, structure ^ PICT_FRAME);
                break;

            case MMCO_SHORT2LONG:

                /* ���Ҫ��ָ��Ϊ���ڲο�ͼ��Ķ��ڲο�ͼ�񲻴����ڳ��ڲο�ͼ�������
                   ����ô�ͽ���ǰ�ĳ��ڲο�ͼ��ӳ��ڲο�֡������ȥ�� liuxw+00139685 */
                if (h->long_ref[mmco[i].long_arg] != pic)
                { remove_long(h, mmco[i].long_arg, 0); }

                /* ��Ҫ��ָ��Ϊ���ڲο�ͼ��Ķ��ڲο�ͼ��Ӷ��ڲο�֡��ȥ�� liuxw+00139685 */
                remove_short_at_index(h, j);
                /* ����ǰ�ο�ͼ����볤�ڲο�ͼ�����У������³��ڲο����е���Ϣ liuxw+00139685 */
                h->long_ref[ mmco[i].long_arg ] = pic;

                if (h->long_ref[ mmco[i].long_arg ])
                {
                    h->long_ref[ mmco[i].long_arg ]->long_ref = 1;
                    h->long_ref_count++;
                }

                break;

            case MMCO_LONG2UNUSED:
                /*  ����long_pic_num�Ͳο����Եõ�long_frame_index,�Ӷ��õ�ref_pic liuxw+00139685 */
                j = pic_num_extract(h, mmco[i].long_arg, &structure);
                pic = h->long_ref[j];

                if (pic)
                {
                    /* ����long_frame_index�Ͳο����ԣ�֡���������׳�������Ӧ��֡�򳡣�����ף���Ϊ�����ο� liuxw+00139685 */
                    remove_long(h, j, structure ^ PICT_FRAME);
                }
                else if (s->avctx->debug & FF_DEBUG_MMCO)
                { av_log(h->s.avctx, AV_LOG_DEBUG, "mmco: unref long failure\n"); }

                break;

            case MMCO_LONG:

                // Comment below left from previous code as it is an interresting note.
                /* First field in pair is in short term list or
                 * at a different long term index.
                 * This is not allowed; see 7.4.3.3, notes 2 and 3.
                 * Report the problem and keep the pair where it is,
                 * and mark this field valid.
                 */
                /* �����ǰͼ�񲻵���ָ�����ڲο�ͼ����long_term_pic_num��ͼ�����Ƚ�
                   long_term_pic_num��ͼ��ӳ��ڲο�ͼ����ȥ�� liuxw+00139685 */
                if (h->long_ref[mmco[i].long_arg] != s->current_picture_ptr)
                {
                    remove_long(h, mmco[i].long_arg, 0);
                    /* ����ǰͼ����뵽���ڲο������У������³��ڲο����е���Ϣ liuxw+00139685 */
                    h->long_ref[ mmco[i].long_arg ] = s->current_picture_ptr;
                    h->long_ref[ mmco[i].long_arg ]->long_ref = 1;
                    h->long_ref_count++;
                }

                s->current_picture_ptr->reference |= s->picture_structure;
                /* ��λ��ǰͼ���Ѿ����й�marking���� liuxw+00139685 */
                current_ref_assigned = 1;
                break;

            case MMCO_SET_MAX_LONG:
                assert(mmco[i].long_arg <= 16);

                // just remove the long term which index is greater than new max
                /* �������µ�max_long_term_frame_idx�ĳ��ڲο�������ȥ�� liuxw+00139685 */
                for (j = mmco[i].long_arg; j < 16; j++)
                {
                    remove_long(h, j, 0);
                }

                break;

            case MMCO_RESET:

                /* ��ȥ���еĶ��ڲο�֡ liuxw+00139685 */
                while (h->short_ref_count)
                {
                    remove_short(h, h->short_ref[0]->frame_num, 0);
                }

                /* ��ȥ���еĳ��ڲο�֡ liuxw+00139685 */
                for (j = 0; j < 16; j++)
                {
                    remove_long(h, j, 0);
                }

                /* ��״̬��Ϣ(����ͬIDR֡ʱһ����) liuxw+00139685 */
                s->current_picture_ptr->poc =
                    s->current_picture_ptr->field_poc[0] =
                        s->current_picture_ptr->field_poc[1] =
                            h->poc_lsb =
                                h->poc_msb =
                                    h->frame_num =
                                        s->current_picture_ptr->frame_num = 0;
                break;

            default:
                assert(0);
        }
    }

    /* �����ǰͼ��û�н��з��䣨�ǳ��ڡ����ڻ��ǲ����ο���liuxw+00139685 */
    if (!current_ref_assigned)
    {
        /* Second field of complementary field pair; the first field of
         * which is already referenced. If short referenced, it
         * should be first entry in short_ref. If not, it must exist
         * in long_ref; trying to put it on the short list here is an
         * error in the encoded bit stream (ref: 7.4.3.3, NOTE 2 and 3).
         */
        /*  ������ڲο�֡�ĸ�������0���ҵ�ǰͼ��Ϊ�ڶ�������һ����Ϊ�ο�����
            ������¶��ڲο�֡����(�Ѿ�������)������ǰ�����ο�������Ϊ֡�ο� liuxw+00139685 */
        if (h->short_ref_count && h->short_ref[0] == s->current_picture_ptr)
        {
            /* Just mark the second field valid */
            s->current_picture_ptr->reference = PICT_FRAME;
        }
        else if (s->current_picture_ptr->long_ref)
        {
            /* ����if (!current_ref_assigned)�����֧���ܶԳ��ڲο�ͼ�����κθı�
               ��ֻ����for(i=0; i<mmco_count; i++)��ѭ���жԳ��ڲο����иı䣻
               �����������һ�������ڲο����ڶ��������ڲο� liuxw+00139685 */
            //          av_log(h->s.avctx, AV_LOG_ERROR, "illegal short term reference "
            av_log(h->s.avctx, AV_LOG_WARNING, "illegal short term reference "
                   "assignment for second field "
                   "in complementary field pair "
                   "(first field is long term)\n");
            IMEDIA_SET_ERR_SLICE(h->s.avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
            h->s.avctx->iTotalError++;
        }
        else
        {
            /* �Ӷ��ڲο�֡�������ҵ�ǰͼ���frame_num�����ҵ��ˣ��򱨴���Ϊ��δ�Ž�ȥ��liuxw+00139685 */
            pic = remove_short(h, s->current_picture_ptr->frame_num, 0);

            if (pic)
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "illegal short term buffer state detected\n");
                av_log(h->s.avctx, AV_LOG_WARNING, "illegal short term buffer state detected\n");
                IMEDIA_SET_ERR_SLICE(h->s.avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
                h->s.avctx->iTotalError++;
            }

            /* �����ڲο�ͼ���������1�������еĲο�֡������һ֡ liuxw+00139685 */
            if (h->short_ref_count)
            { memmove(&h->short_ref[1], &h->short_ref[0], h->short_ref_count * sizeof(Picture *)); }

            /* �ѵ�ǰͼ����뵽���ڲο�֡���е�short_ref[0] �����¶��ڲο����е���Ϣ liuxw+00139685 */
            h->short_ref[0] = s->current_picture_ptr;
            h->short_ref_count++;
            s->current_picture_ptr->reference |= s->picture_structure;
        }
    }

    /* ���ִ��marking����ڲο�֡�ͳ��ڲο�֡�ĸ�������sps�е�frame_num(max_ref_frame_num)��
       ��˵���������⣨��Ϊ��marking��ʱ���Ѿ����й����) liuxw+00139685 */
    if (h->long_ref_count + h->short_ref_count > h->sps.ref_frame_count)
    {
        /* We have too many reference frames, probably due to corrupted
         * stream. Need to discard one frame. Prevents overrun of the
         * short_ref and long_ref buffers.
         */
        //      av_log(h->s.avctx, AV_LOG_ERROR,
        av_log(h->s.avctx, AV_LOG_WARNING,
               "number of reference frames exceeds max (probably "
               "corrupt input), discarding one\n");
        IMEDIA_SET_ERR_SLICE(h->s.avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
        h->s.avctx->iTotalError++;

        /* ����������������ȫ�ǳ��ڲο�֡ʱ���������ڲο�������С����һ��
           (???Ӧ�ö�������������һ���űȽϺ�) liuxw+00139685 */
        if (h->long_ref_count && !h->short_ref_count)
        {
            for (i = 0; i < 16; ++i)
                if (h->long_ref[i])
                { break; }

            assert(i < 16);
            remove_long(h, i, 0);
        }
        else
        {
            /* ��������������������ڶ��ڲο�֡���������ڲο�����������һ�� liuxw+00139685 */
            pic = h->short_ref[h->short_ref_count - 1];
            remove_short(h, pic->frame_num, 0);
        }
    }

    print_short_term(h);
    print_long_term(h);
    return 0;
}

/* �ο�ͼ����еı�� lixxw+00139685 */
static int decode_ref_pic_marking(H264Context *h, GetBitContext *gb)
{
    MpegEncContext *const s = &h->s;
    int i;

    h->mmco_index = 0;

    if (h->nal_unit_type == NAL_IDR_SLICE)
    {
        //FIXME fields
        /* ??? ���﷨Ԫ�ز�δʹ�ã���֪���᲻�������� liuxw+00139685 */
        s->broken_link = get_bits1(gb) - 1;

        /* ����ǰIDRͼ����Ϊ���ڲο�ͼ�� */
        if (get_bits1(gb))
        {
            h->mmco[0].opcode = MMCO_LONG;
            h->mmco[0].long_arg = 0;
            h->mmco_index = 1;
        }
    }
    else
    {
        /* ��������Ӧ�Ĳο�ͼ���Ƿ�ʽ lixxw+00139685 */
        if (get_bits1(gb)) // adaptive_ref_pic_marking_mode_flag
        {
            for (i = 0; i < MAX_MMCO_COUNT; i++)
            {
                /* �õ��ο�ͼ���ǵĲ����� lixxw+00139685 */
                MMCOOpcode opcode = get_ue_golomb_31(gb);
                /* ��¼�ο�ͼ���ǵĲ����� lixxw+00139685 */
                h->mmco[i].opcode = opcode;

                /* �������н�����Ӧ��������﷨Ԫ�أ������б��� lixxw+00139685 */

                /* �õ����ڲο�ͼ���pic_num�������б��� lixxw+00139685 */
                if (opcode == MMCO_SHORT2UNUSED || opcode == MMCO_SHORT2LONG)
                {
                    h->mmco[i].short_pic_num = (h->curr_pic_num - get_ue_golomb(gb) - 1) & (h->max_pic_num - 1);
                    /*                    if(h->mmco[i].short_pic_num >= h->short_ref_count || h->short_ref[ h->mmco[i].short_pic_num ] == NULL){
                                            av_log(s->avctx, AV_LOG_ERROR, "illegal short ref in memory management control operation %d\n", mmco);
                                            return -1;
                                        }*/
                }

                /* �õ����ڲο�ͼ���pic_num���ڲο�ͼ������ֵ�������б��� lixxw+00139685 */
                if (opcode == MMCO_SHORT2LONG || opcode == MMCO_LONG2UNUSED || opcode == MMCO_LONG || opcode == MMCO_SET_MAX_LONG)
                {
                    unsigned int long_arg = get_ue_golomb_31(gb);

                    /* 2010/08/11 10:00:00 liuxw+00139685 */
                    /* �����������4��Ҳ������������ڲο�֡������ʱ����ֵҪ��1 */
                    if (opcode == MMCO_SET_MAX_LONG)
                    {
                        long_arg--;
                    }

                    /* 1��long_pic_num����32��fieldʱ����2����long_pic_num����16(frame)��3��max_long_frame_index����16��
                       4��long_pic_num����16����opcode==MMCO_SHORT2LONG(���ο��ǳ����򽫰�����ǰ���Ĳο�֡�򻥲��ο�����Ϊ���ڲο������ڲο�ͼ���ǰ�֡���д�ŵ�)��
                       5��long_pic_num����16����opcode==MMCO_LONG(����ǰ�ǳ����򽫰�����ǰ���Ĳο�֡�򻥲��ο�����Ϊ���ڲο�֡�����ڲο�ͼ���ǰ�֡���д�ŵ�);
                       ������������֮һ���ᱨ�����ֻ����opcode == MMCO_LONG2UNUSED���ҵ�ǰ�ǳ�������£�0<long_pic_num<32
                       ����������long_pic_num�Ͳο����Եõ�long_frame_index���Ӷ�����long_frame_index�Ͳο����Խ���unused������ liuxw+00139685 */
                    if (long_arg >= 32 || (long_arg >= 16 && !(opcode == MMCO_LONG2UNUSED && FIELD_PICTURE)))
                    {
                        //                      av_log(h->s.avctx, AV_LOG_ERROR, "illegal long ref in memory management control operation %d\n", opcode);
                        av_log(h->s.avctx, AV_LOG_WARNING, "illegal long ref in memory management control operation %d\n", opcode);
                        return -1;
                    }

                    h->mmco[i].long_arg = long_arg;
                }

                /* ��������ĺϷ��� lixxw+00139685 */
                if (opcode > (unsigned)MMCO_LONG)
                {
                    //                  av_log(h->s.avctx, AV_LOG_ERROR, "illegal memory management control operation %d\n", opcode);
                    av_log(h->s.avctx, AV_LOG_WARNING, "illegal memory management control operation %d\n", opcode);
                    return -1;
                }

                /* ������������0�����˳��ο�ͼ��ı�ǣ������������ȡ�������﷨Ԫ�� lixxw+00139685 */
                if (opcode == MMCO_END)
                { break; }
            }

            /* ��¼ִ�вο�ͼ���ǵĴ��� lixxw+00139685 */
            h->mmco_index = i;
        }
        else
        {
            /* ���û������Ĳο�ͼ���Ƿ�ʽ liuxw+00139685 */
            assert(h->long_ref_count + h->short_ref_count <= h->sps.ref_frame_count);

            /* �����ڲο�֡����ҳ��ڲο�֡�Ͷ��ڲο�֡����������ref_frame_numʱ������������������֮һ��
               1����ǰͼ����֡ 2����ǰͼ���Ƕ���
               ��ô�����ڲο�֡�����е�pic_num��С����һ֡�Ӷ��ڲο�֡�����б��Ϊunused liuxw+00139685 */
            if (h->short_ref_count && h->long_ref_count + h->short_ref_count == h->sps.ref_frame_count &&
                !(FIELD_PICTURE && !s->first_field && s->current_picture_ptr->reference))
            {
                h->mmco[0].opcode = MMCO_SHORT2UNUSED;
                h->mmco[0].short_pic_num = h->short_ref[ h->short_ref_count - 1 ]->frame_num;
                h->mmco_index = 1;

                /* ����ǳ����򽫶��ڲο�֡������pic_num��С����һ֡������(�����Ƿ����ο�)�����Ϊunused liuxw+00139685 */
                if (FIELD_PICTURE)
                {
                    h->mmco[0].short_pic_num *= 2;
                    h->mmco[1].opcode = MMCO_SHORT2UNUSED;
                    h->mmco[1].short_pic_num = h->mmco[0].short_pic_num + 1;
                    h->mmco_index = 2;
                }
            }
        }
    }

    return 0;
}

static int init_poc(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int max_frame_num = 1 << h->sps.log2_max_frame_num;
    int field_poc[2];
    Picture *cur = s->current_picture_ptr;

    h->frame_num_offset = h->prev_frame_num_offset;

    if (h->frame_num < h->prev_frame_num)
    { h->frame_num_offset += max_frame_num; }

    if (h->sps.poc_type == 0)
    {
        const int max_poc_lsb = 1 << h->sps.log2_max_poc_lsb;

        if     (h->poc_lsb < h->prev_poc_lsb && (int)(h->prev_poc_lsb - h->poc_lsb) >= max_poc_lsb / 2)
        { h->poc_msb = h->prev_poc_msb + max_poc_lsb; }
        else if (h->poc_lsb > h->prev_poc_lsb && (int)(h->prev_poc_lsb - h->poc_lsb) < -max_poc_lsb / 2)
        { h->poc_msb = h->prev_poc_msb - max_poc_lsb; }
        else
        { h->poc_msb = h->prev_poc_msb; }

        field_poc[0] =
            field_poc[1] = h->poc_msb + h->poc_lsb;

        if (s->picture_structure == PICT_FRAME)
        { field_poc[1] += h->delta_poc_bottom; }
    }
    else if (h->sps.poc_type == 1)
    {
        int abs_frame_num, expected_delta_per_poc_cycle, expectedpoc;
        int i;

        /* 2010/04/30 17:00:00 liuxw+00139685 [az1d02058] */
        /* ����һ���жϣ�����ǰpictureΪIDRʱ����delta_poc��ʼ��Ϊ0 */
        if (NAL_IDR_SLICE == h->nal_unit_type)
        {
            h->delta_poc[0] = 0;
        }

        if (h->sps.poc_cycle_length != 0)
        { abs_frame_num = h->frame_num_offset + h->frame_num; }
        else
        { abs_frame_num = 0; }

        if (h->nal_ref_idc == 0 && abs_frame_num > 0)
        { abs_frame_num--; }

        expected_delta_per_poc_cycle = 0;

        for (i = 0; i < h->sps.poc_cycle_length; i++)
        { expected_delta_per_poc_cycle += h->sps.offset_for_ref_frame[ i ]; } //FIXME integrate during sps parse

        if (abs_frame_num > 0)
        {
            int poc_cycle_cnt          = (abs_frame_num - 1) / h->sps.poc_cycle_length;
            int frame_num_in_poc_cycle = (abs_frame_num - 1) % h->sps.poc_cycle_length;

            expectedpoc = poc_cycle_cnt * expected_delta_per_poc_cycle;

            for (i = 0; i <= frame_num_in_poc_cycle; i++)
            { expectedpoc = expectedpoc + h->sps.offset_for_ref_frame[ i ]; }
        }
        else
        { expectedpoc = 0; }

        if (h->nal_ref_idc == 0)
        { expectedpoc = expectedpoc + h->sps.offset_for_non_ref_pic; }

        field_poc[0] = expectedpoc + h->delta_poc[0];
        field_poc[1] = field_poc[0] + h->sps.offset_for_top_to_bottom_field;

        if (s->picture_structure == PICT_FRAME)
        { field_poc[1] += h->delta_poc[1]; }
    }
    else
    {
        int poc = 2 * (h->frame_num_offset + h->frame_num);

        if (!h->nal_ref_idc)
        { poc--; }

        field_poc[0] = poc;
        field_poc[1] = poc;
    }

    if (s->picture_structure != PICT_BOTTOM_FIELD)
    { s->current_picture_ptr->field_poc[0] = field_poc[0]; }

    if (s->picture_structure != PICT_TOP_FIELD)
    { s->current_picture_ptr->field_poc[1] = field_poc[1]; }

    cur->poc = FFMIN(cur->field_poc[0], cur->field_poc[1]);

    /* 2010/05/7 9:00:00 liuxw+00139685 */
    /* ����ǰ��������һ����IDR����cur->poc��Ϊ0 */
    if (cur->field_poc[0] == 0 || cur->field_poc[1] == 0)
    {
        cur->poc = 0;
    }

    return 0;
}


/**
 * initialize scan tables
 */
static void init_scan_tables(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int i;

    if (s->dsp.h264_idct_add == ff_h264_idct_add_c) //FIXME little ugly
    {
        memcpy(h->zigzag_scan, zigzag_scan, 16 * sizeof(uint8_t));
        memcpy(h-> field_scan,  field_scan, 16 * sizeof(uint8_t));
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
#define T(x) (x>>2) | ((x<<2) & 0xF)
            h->zigzag_scan[i] = T(zigzag_scan[i]);
            h-> field_scan[i] = T( field_scan[i]);
#undef T
        }
    }

    if (s->dsp.h264_idct8_add == ff_h264_idct8_add_c)
    {
        memcpy(h->zigzag_scan8x8,       ff_zigzag_direct,     64 * sizeof(uint8_t));
        memcpy(h->zigzag_scan8x8_cavlc, zigzag_scan8x8_cavlc, 64 * sizeof(uint8_t));
        memcpy(h->field_scan8x8,        field_scan8x8,        64 * sizeof(uint8_t));
        memcpy(h->field_scan8x8_cavlc,  field_scan8x8_cavlc,  64 * sizeof(uint8_t));
    }
    else
    {
        for (i = 0; i < 64; i++)
        {
#define T(x) (x>>3) | ((x&7)<<3)
            h->zigzag_scan8x8[i]       = T(ff_zigzag_direct[i]);
            h->zigzag_scan8x8_cavlc[i] = T(zigzag_scan8x8_cavlc[i]);
            h->field_scan8x8[i]        = T(field_scan8x8[i]);
            h->field_scan8x8_cavlc[i]  = T(field_scan8x8_cavlc[i]);
#undef T
        }
    }

    if (h->sps.transform_bypass) //FIXME same ugly
    {
        h->zigzag_scan_q0          = zigzag_scan;
        h->zigzag_scan8x8_q0       = ff_zigzag_direct;
        h->zigzag_scan8x8_cavlc_q0 = zigzag_scan8x8_cavlc;
        h->field_scan_q0           = field_scan;
        h->field_scan8x8_q0        = field_scan8x8;
        h->field_scan8x8_cavlc_q0  = field_scan8x8_cavlc;
    }
    else
    {
        h->zigzag_scan_q0          = h->zigzag_scan;
        h->zigzag_scan8x8_q0       = h->zigzag_scan8x8;
        h->zigzag_scan8x8_cavlc_q0 = h->zigzag_scan8x8_cavlc;
        h->field_scan_q0           = h->field_scan;
        h->field_scan8x8_q0        = h->field_scan8x8;
        h->field_scan8x8_cavlc_q0  = h->field_scan8x8_cavlc;
    }
}

/**
 * Replicates H264 "master" context to thread contexts.
 */
static void clone_slice(H264Context *dst, H264Context *src)
{
    memcpy(dst->block_offset,     src->block_offset, sizeof(dst->block_offset));
    dst->s.current_picture_ptr  = src->s.current_picture_ptr;
    dst->s.current_picture      = src->s.current_picture;
    dst->s.linesize             = src->s.linesize;
    dst->s.uvlinesize           = src->s.uvlinesize;
    dst->s.first_field          = src->s.first_field;

    dst->prev_poc_msb           = src->prev_poc_msb;
    dst->prev_poc_lsb           = src->prev_poc_lsb;
    dst->prev_frame_num_offset  = src->prev_frame_num_offset;
    dst->prev_frame_num         = src->prev_frame_num;
    dst->short_ref_count        = src->short_ref_count;

    memcpy(dst->short_ref,        src->short_ref,        sizeof(dst->short_ref));
    memcpy(dst->long_ref,         src->long_ref,         sizeof(dst->long_ref));
    memcpy(dst->default_ref_list, src->default_ref_list, sizeof(dst->default_ref_list));
    memcpy(dst->ref_list,         src->ref_list,         sizeof(dst->ref_list));

    memcpy(dst->dequant4_coeff,   src->dequant4_coeff,   sizeof(src->dequant4_coeff));
    memcpy(dst->dequant8_coeff,   src->dequant8_coeff,   sizeof(src->dequant8_coeff));
}

/**
 * decodes a slice header.
 * This will also call MPV_common_init() and frame_start() as needed.
 *
 * @param h h264context
 * @param h0 h264 master context (differs from 'h' when doing sliced based parallel decoding)
 *
 * @return 0 if okay, <0 if an error occurred, 1 if decoding must not be multithreaded
 */
static int decode_slice_header(H264Context *h, H264Context *h0)
{
    MpegEncContext *const s = &h->s;
    MpegEncContext *const s0 = &h0->s;
    unsigned int first_mb_in_slice;
    unsigned int pps_id;
    int num_ref_idx_active_override_flag;
    unsigned int slice_type, tmp, i, j;
    int default_ref_list_done = 0;
    int last_pic_structure;

    s->dropable = h->nal_ref_idc == 0;

    /* �����ǰ֡Ϊ�ǲο�֡���Ҵ�CODEC_FLAG2_FAST���ܣ�����Ե��ü򵥵�1/4���ز�ֵ */
    if ((s->avctx->flags2 & CODEC_FLAG2_FAST) && !h->nal_ref_idc)
    {
        s->me.qpel_put = s->dsp.put_2tap_qpel_pixels_tab;
        s->me.qpel_avg = s->dsp.avg_2tap_qpel_pixels_tab;
    }
    else 	/* ����Э��Ĳ�ֵ */
    {
        s->me.qpel_put = s->dsp.put_h264_qpel_pixels_tab;
        s->me.qpel_avg = s->dsp.avg_h264_qpel_pixels_tab;
    }

    first_mb_in_slice = get_ue_golomb(&s->gb);

    /* ���CODEC_FLAG2_CHUNKS���ܴ򿪣�����һЩslice���ĳ�ʼ��(������û�д򿪣���slice����ʼ����decode_nal_unit()���Ѿ�����) */
    if ((s->flags2 & CODEC_FLAG2_CHUNKS) && first_mb_in_slice == 0)
    {
        h0->current_slice = 0;

        if (!s0->first_field)
        { s->current_picture_ptr = NULL; }
    }

    slice_type = get_ue_golomb_31(&s->gb);

    if (slice_type > 9)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "slice type too large (%d) at %d %d\n", h->slice_type, s->mb_x, s->mb_y);
        av_log(h->s.avctx, AV_LOG_WARNING, "slice type too large (%d) at %d %d\n", h->slice_type, s->mb_x, s->mb_y);
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���ӷ����� */
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_TYPE);
        s->avctx->iTotalError++;
        return -1;
    }

    if (slice_type > 4)
    {
        slice_type -= 5;
        h->slice_type_fixed = 1;
    }
    else
    { h->slice_type_fixed = 0; }

    slice_type = golomb_to_pict_type[ slice_type ];

    /* 1�������IDR֡���򽫲ο����г�ʼ����־��Ϊ1��������ʼ����IDRʱ�ο������ѱ���գ�
       2���������picture�ĵ�һ��slice���ҵ�ǰslice_tyep������һ��slice��slice_typeʱ��Ҳ�����ʼ���ο����� */
    if (slice_type == FF_I_TYPE || (h0->current_slice != 0 && slice_type == h0->last_slice_type) )
    {
        default_ref_list_done = 1;
    }

    h->slice_type = slice_type;
    h->slice_type_nos = slice_type & 3;

    s->pict_type = h->slice_type; // to make a few old functions happy, it's wrong though
    /* ����ǰ֡ΪB֡��������ǰû�вο�֡ʱ;ȥ��ԭ�򣺽��B֡����һ��ο������� */
    /*    if (s->pict_type == FF_B_TYPE && s0->last_picture_ptr == NULL) {
            av_log(h->s.avctx, AV_LOG_ERROR,
                   "B picture before any references, skipping\n");
            return -1;
        } */

    pps_id = get_ue_golomb(&s->gb);

    if (pps_id >= MAX_PPS_COUNT)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "pps_id out of range\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "pps_id out of range\n");
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_ID);
        s->avctx->iTotalError++;
        pps_id = 0;
        //      return -1;
    }

    if (!h0->pps_buffers[pps_id])
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "non-existing PPS referenced\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "non-existing PPS referenced\n");
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_ID);
        s->avctx->iTotalError++;
        return -1;
    }

    h->pps = *h0->pps_buffers[pps_id];

    if (!h0->sps_buffers[h->pps.sps_id])
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "non-existing SPS referenced\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "non-existing SPS referenced\n");
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_ID);
        s->avctx->iTotalError++;
        return -1;
    }

    h->sps = *h0->sps_buffers[h->pps.sps_id];

    /* ���pps�ı�������³�ʼ���������� liuxw+00139685 */
    if (h == h0 && h->dequant_coeff_pps != pps_id)
    {
        h->dequant_coeff_pps = pps_id;
        init_dequant_tables(h);
    }

    s->mb_width = h->sps.mb_width;
    s->mb_height = h->sps.mb_height * (2 - h->sps.frame_mbs_only_flag);

    s->width = 16 * s->mb_width;
    s->height = 16 * s->mb_height;

    /* 2010/05/26 10:33:00 liuxw+00139685 */
    /* ȷ�����еĳ��Ȳ���ı䣬��ֹ���� */
    /* 2010/06/03 10:00:00 liuxw+00139685 */
    /* ������s->context_initializedȥ������Ϊ��sps���Ѿ�������и�ֵ */
    //	if(s->context_initialized)
    //	{
    if (s->width != s->avctx->width || s->height != s->avctx->height)
    {
        av_log(NULL, AV_LOG_WARNING, "s->width[%d] != s->avctx->width[%d] || s->height[%d] != s->avctx->height[%d]\n", s->width, s->avctx->width, s->height, s->avctx->height);
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_OTHER);
        s->avctx->iTotalError++;

        s->width  = s->avctx->width;
        s->height = s->avctx->height;
        s->mb_width = s->width / MB_WIDTH;
        s->mb_height = s->height / MB_HEIGHT;
    }

    //	}

    h->b_stride =  s->mb_width * 4;
    h->b8_stride = s->mb_width * 2;

    /* ���¼���ͼ��ԭʼ��С�ķ������� */
    /*
    s->width = 16*s->mb_width - 2*FFMIN(h->sps.crop_right, 7);
    if(h->sps.frame_mbs_only_flag)
        s->height= 16*s->mb_height - 2*FFMIN(h->sps.crop_bottom, 7);
    else
        s->height= 16*s->mb_height - 4*FFMIN(h->sps.crop_bottom, 3);   */

    /* 2010/30/06 14:33:00 liuxw+00139685 */
    /* ����������Ϣ */
    s->avctx->iChromaFormat = h->sps.chroma_format_idc;
    /* ��������״̬��Ϣ */
    s->avctx->iActualProfile = h->sps.profile_idc;
    s->avctx->iActualLevel   = h->sps.level_idc;
    s->avctx->iActualRefNum  = h->sps.ref_frame_count;
    /* �������������룩*/

    /* �ڹ����ṹ���Ѿ���ʼ��������£������ǰ�߳��������slice����ǰ�����slice�Ŀ�Ȼ�߶Ȳ�һ�µ�����£��ͷŵ�ǰ�߳���ռ����Դ */
    if (s->context_initialized
        && (   s->width != s->avctx->width || s->height != s->avctx->height))
    {
        if (h != h0)
        { return -1; }   // width / height changed during parallelized decoding

        free_tables(h);
        flush_dpb(s->avctx);
        MPV_common_end(s);
    }

    /* ��������ṹ�����û�г�ʼ��������г�ʼ�� */
    if (!s->context_initialized)
    {
        /* 2010/06/10 18:30:00 liuxw+00139685 */
        /* ���ӷ�����(���ڴ����ʧ�ܵ�����·���) */
        int iRet = 0;

        /* �����ṹ�����û�г�ʼ��ʱ��hӦ�õ���h0 */
        if (h != h0)
        { return -1; }  // we cant (re-)initialize context during parallel decoding

        /* ��ʼ�������ṹ������� */
        if (MPV_common_init(s) < 0)
        { return -1; }

        s->first_field = 0;

        init_scan_tables(h);
        /* 2010/06/10 18:30:00 liuxw+00139685 */
        /* ���ӷ�����(���ڴ����ʧ�ܵ�����·���) */
        //	   alloc_tables(h);
        iRet = alloc_tables(h);

        if (iRet < 0)
        {
            return -1;
        }

        /* Ϊÿһ���̷߳���һ��h264����ͨ������,�����г�ʼ��(������һ������) */
        for (i = 1; i < (unsigned int)s->avctx->thread_count; i++)
        {
            H264Context *c;
            c = h->thread_context[i] = av_malloc_hw(s->avctx, sizeof(H264Context));
            memcpy(c, h->s.thread_context[i], sizeof(MpegEncContext));
            memset(&c->s + 1, 0, sizeof(H264Context) - sizeof(MpegEncContext));
            c->sps = h->sps;
            c->pps = h->pps;
            init_scan_tables(c);
            clone_tables(c, h);
        }

        /* ��ʼ��ÿ���̵߳Ľ���ͨ�������Ĳ��ֳ�Ա����ռ䣨������*/
        for (i = 0; i < (unsigned int)s->avctx->thread_count; i++)
            if (context_init(h->thread_context[i]) < 0)
            { return -1; }

        s->avctx->width = s->width;
        s->avctx->height = s->height;

        /* 2010/08/28 14:30:00 liuxw+00139685 */
        /* ��sps.sarΪ(0,0),�����踳ֵ */
        if (h->sps.sar.den != 0 && h->sps.sar.num != 0)
        { s->avctx->sample_aspect_ratio = h->sps.sar; }

        //if(!s->avctx->sample_aspect_ratio.den)
        //    s->avctx->sample_aspect_ratio.den = 1;

        if (h->sps.timing_info_present_flag)
        {
            // s->avctx->time_base= (AVRational){h->sps.num_units_in_tick, h->sps.time_scale};  /* changed by lxw */
            s->avctx->time_base.num = h->sps.num_units_in_tick;
            s->avctx->time_base.den = h->sps.time_scale;

            if (h->x264_build > 0 && h->x264_build < 44)
            { s->avctx->time_base.den *= 2; }

            //         av_reduce(&s->avctx->time_base.num, &s->avctx->time_base.den, s->avctx->time_base.num, s->avctx->time_base.den, 1<<30);
        }
    }

    h->frame_num = get_bits(&s->gb, h->sps.log2_max_frame_num);

    h->mb_mbaff = 0;
    h->mb_aff_frame = 0;
    last_pic_structure = s0->picture_structure;

    if (h->sps.frame_mbs_only_flag)
    {
        s->picture_structure = PICT_FRAME;
    }
    else
    {
        if (get_bits1(&s->gb))
        {
            //field_pic_flag
            s->picture_structure = PICT_TOP_FIELD + get_bits1(&s->gb); //bottom_field_flag
        }
        else
        {
            s->picture_structure = PICT_FRAME;
            h->mb_aff_frame = h->sps.mb_aff;
        }
    }

    h->mb_field_decoding_flag = s->picture_structure != PICT_FRAME;

    /*�����һ֡�ĵ�һ��slice����Ҫȥ���frame_num�Ƿ����� */
    if (h0->current_slice == 0)
    {
        /* ��frame_num��������???[��̸���] */
        while (h->frame_num !=  h->prev_frame_num &&
               h->frame_num != (h->prev_frame_num + 1) % (1 << h->sps.log2_max_frame_num))
        {
            av_log(NULL, AV_LOG_DEBUG, "Frame num gap %d %d\n", h->frame_num, h->prev_frame_num);

            if (frame_start(h) < 0)
            { return -1; }

            h->prev_frame_num++;
            h->prev_frame_num %= 1 << h->sps.log2_max_frame_num;
            s->current_picture_ptr->frame_num = h->prev_frame_num;
            execute_ref_pic_marking(h, NULL, 0);
        }

        /* See if we have a decoded first field looking for a pair... */
        if (s0->first_field)
        {
            assert(s0->current_picture_ptr);
            assert(s0->current_picture_ptr->data[0]);
            assert(s0->current_picture_ptr->reference != DELAYED_PIC_REF);

            /* figure out if we have a complementary field pair */
            if (!FIELD_PICTURE || s->picture_structure == last_pic_structure)
            {
                /*
                 * Previous field is unmatched. Don't display it, but let it
                 * remain for reference if marked as such.
                 */
                /* �����ǰ����ǰһ�����ǻ������ԣ���ô����ʾǰһ�������ǰһ���ǲο���������Ȼ������ο����� */
                s0->current_picture_ptr = NULL;
                s0->first_field = FIELD_PICTURE;

                if (s0->first_field)
                {
                    s->avctx->uiFldCnt++;
                }

            }
            else     /* ��ǰ����ǰһ���ǻ������� */
            {
                if (h->nal_ref_idc &&
                    s0->current_picture_ptr->reference &&
                    s0->current_picture_ptr->frame_num != h->frame_num)
                {
                    /*
                     * This and previous field were reference, but had
                     * different frame_nums. Consider this field first in
                     * pair. Throw away previous field except for reference
                     * purposes.
                     */
                    /* �����ǰ����ǰһ����һ�Ի����ο����ԣ���������ͬһ֡�������ǰһ������Ϊ��Ϊһ֡�ĵ�һ��������������������ο����ԣ�*/
                    s0->first_field = 1;
                    s0->current_picture_ptr = NULL;
                    s->avctx->uiFldCnt++;

                }
                else if ((h->nal_ref_idc || s0->current_picture_ptr->reference) && s0->current_picture_ptr->frame_num != h->frame_num)
                {
                    s0->first_field = 1;
                    s0->current_picture_ptr = NULL;
                    s->avctx->uiFldCnt++;
                }
                else
                {
                    /* ��һ֡�еĵڶ������׳�)����0��һ����־λ��������־λ��*/
                    /* Second field in complementary pair */
                    s0->first_field = 0;
                }
            }

        }
        else     /* �����ǰ�Ƕ�������first_field��1�������ǰ��֡����first_field��0 */
        {
            /* Frame or first field in a potentially complementary pair */
            assert(!s0->current_picture_ptr);
            s0->first_field = FIELD_PICTURE;
        }

        /* �����ǰ��֡���Ƕ����������frame_start();���ǵ׳�������Ҫ�ٵ��� */
        /* frame_start():  */
        if ((!FIELD_PICTURE || s0->first_field) && frame_start(h) < 0)
        {
            s0->first_field = 0;
            return -1;
        }

        /* �������� */
        if (!FIELD_PICTURE || s0->first_field) /* ����֡���Ƕ���ʱ�������ѽ���I P B֡������ */
        {
            switch (slice_type)
            {
                case FF_I_TYPE:
                    s->avctx->uiDecIFrames++;  /* �����ѽ���I֡���� */
                    break;

                case FF_P_TYPE:
                    s->avctx->uiDecPFrames++;	 /* �����ѽ���P֡���� */
                    break;

                case FF_B_TYPE:
                    s->avctx->uiDecBFrames++;  /* �����ѽ���B֡���� */
                    break;

                default:
                    break;
            }
        }

        /* ��������������) */
    }

    /* ����slice���н���Ļ�������˵�һ���߳����ⶼ��Ҫ������һ���̵߳�Ҫ�õ��ı� */
    if (h != h0)
    { clone_slice(h, h0); }

    s->current_picture_ptr->frame_num = h->frame_num; //FIXME frame_num cleanup

    assert(s->mb_num == s->mb_width * s->mb_height);

    /* ���first_mb_in_slice�ĺϷ��� liuxw+00139685 */
    if ((int)(first_mb_in_slice << FIELD_OR_MBAFF_PICTURE) >= s->mb_num ||
        (int)first_mb_in_slice >= s->mb_num)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "first_mb_in_slice overflow\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "first_mb_in_slice overflow\n");
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_ADDR);
        s->avctx->iTotalError++;
        first_mb_in_slice = 0;
        //      return -1;
    }

    s->resync_mb_x = s->mb_x = first_mb_in_slice % s->mb_width;
    s->resync_mb_y = s->mb_y = (first_mb_in_slice / s->mb_width) << FIELD_OR_MBAFF_PICTURE;

    if (s->picture_structure == PICT_BOTTOM_FIELD)
    { s->resync_mb_y = s->mb_y = s->mb_y + 1; }

    assert(s->mb_y < s->mb_height);

    if (s->picture_structure == PICT_FRAME)
    {
        h->curr_pic_num =   h->frame_num;
        h->max_pic_num = 1 << h->sps.log2_max_frame_num;
    }
    else   /* CurrPicNum�����Э��7.4.3 liuxw+00139685 */
    {
        h->curr_pic_num = 2 * h->frame_num + 1;
        h->max_pic_num = 1 << (h->sps.log2_max_frame_num + 1);
    }

    if (h->nal_unit_type == NAL_IDR_SLICE)
    {
        get_ue_golomb(&s->gb); /* idr_pic_id */
    }

    if (h->sps.poc_type == 0)
    {
        h->poc_lsb = get_bits(&s->gb, h->sps.log2_max_poc_lsb);

        /* 2010/05/5 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        if ((int)h->poc_lsb >= (1 << h->sps.log2_max_poc_lsb))
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "poc_lsb[%d] out of range[0,%d]\n", h->poc_lsb, h->sps.log2_max_poc_lsb);
            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
            s->avctx->iTotalError++;
            h->poc_lsb = (1 << h->sps.log2_max_poc_lsb) - 1;
        }


        if (h->pps.pic_order_present == 1 && s->picture_structure == PICT_FRAME)
        {
            h->delta_poc_bottom = get_se_golomb(&s->gb);
        }
    }

    if (h->sps.poc_type == 1 && !h->sps.delta_pic_order_always_zero_flag)
    {
        h->delta_poc[0] = get_se_golomb(&s->gb);

        if (h->pps.pic_order_present == 1 && s->picture_structure == PICT_FRAME)
        { h->delta_poc[1] = get_se_golomb(&s->gb); }
    }

    /* ����picture order count */
    init_poc(h);

#ifdef LXW_DEBUG

    /* add by lxw for test */
    if (frame_num == TARGET_FRAME)
    {
        av_log(NULL, AV_LOG_ERROR, "Begin deug!\n");
    }

#endif

    if (h->pps.redundant_pic_cnt_present)
    {
        h->redundant_pic_count = get_ue_golomb(&s->gb);
    }

    //set defaults, might be overridden a few lines later
    h->ref_count[0] = h->pps.ref_count[0];
    h->ref_count[1] = h->pps.ref_count[1];

    if (h->slice_type_nos != FF_I_TYPE)
    {
        if (h->slice_type_nos == FF_B_TYPE)
        {
            h->direct_spatial_mv_pred = get_bits1(&s->gb);
        }

        num_ref_idx_active_override_flag = get_bits1(&s->gb);

        if (num_ref_idx_active_override_flag)
        {
            h->ref_count[0] = get_ue_golomb(&s->gb) + 1;

            if (h->slice_type_nos == FF_B_TYPE)
            { h->ref_count[1] = get_ue_golomb(&s->gb) + 1; }
            /* 2010/03/11 10:40:00 liuxw+00139685 */
            /* ����else��֧��䣺��Ϊp sliceʱ��ֻ��һ��list0�ο��б� */
            else
            {
                h->ref_count[1] = 0;
            }

            /* 2010/03/11 10:40:00 liuxw+00139685 */
            /* ��interԤ��ʱ���ο�ͼ�����list0�ĸ���һ��Ҫ����0С��32��list1ֻҪС��32���� */
            //          if(h->ref_count[0]-1 > 32-1 || h->ref_count[1]-1 > 32-1){
            if (h->ref_count[0] - 1 > 32 - 1 || h->ref_count[1] > 32)
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "reference overflow\n");
                av_log(h->s.avctx, AV_LOG_WARNING, "reference overflow\n");
                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ���Ӵ����� */
                IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
                h->ref_count[0] = h->ref_count[1] = 1;
                s->avctx->iTotalError++;
                //               return -1;
            }
        }

        if (h->slice_type_nos == FF_B_TYPE) /* B slice liuxw+00139685 */
        { h->list_count = 2; }
        else	/* P slice */
        {
            h->list_count = 1;
            /* 2010/08/23 14:00:00 liuxw+00139685 */
            /* �����ǰΪp slice����ôlist1����Ŀ��Ϊ0 */
            h->ref_count[1] = 0;
        }
    }
    else/* I slice */
    {
        h->list_count = 0;
        /* 2010/04/08 21:30:00 liuxw+00139685 */
        /* �������ǰ֡ΪI֡ʱ��ref_countû�б���0�����º������� */
        h->ref_count[0] = 0;
        h->ref_count[1] = 0;
    }

    /* 2010/05/5 15:00:00 liuxw+00139685 */
    /* ���ӶԲο�֡�����ж� */
    //if((int)h->ref_count[0] > (h->sps.ref_frame_count<<FIELD_PICTURE))
    //{
    //	av_log(s->avctx,AV_LOG_WARNING,"Ref count  is wrong!\n");
    //	s->avctx->iTotalError++;
    //	h->ref_count[0] = FFMIN(h->sps.ref_frame_count,(h->short_ref_count+h->long_ref_count));
    //}
    //if((int)h->ref_count[1] > (h->sps.ref_frame_count<<FIELD_PICTURE))
    //{
    //	h->ref_count[1] = FFMIN(h->sps.ref_frame_count,(h->short_ref_count+h->long_ref_count));
    //}

    if ((h->slice_type_nos == FF_P_TYPE && !h->ref_count[0]) || (h->slice_type_nos == FF_B_TYPE && (!h->ref_count[0] || !h->ref_count[1])))
    {
        av_log(s->avctx, AV_LOG_WARNING, "P Frame,no ref list0 or B Frame no ref list0/list1/listx!\n");
        s->avctx->iTotalError++;
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
        return -1;
    }

    /* 2010/06/03 19:30:00 liuxw+00139685 */
    /* ����mbaff����֡���룬��ôlist0/1�вο�ͼ��ĸ�������С��16 */
    if (!FIELD_PICTURE)
    {
        h->ref_count[0] = FFMIN(h->ref_count[0], 16);
        h->ref_count[1] = FFMIN(h->ref_count[1], 16);

        if (h->ref_count[0] > 16 || h->ref_count[1] > 16)
        {
            av_log(NULL, AV_LOG_WARNING, "h->ref_count[0] > 16 || h->ref_count[1] > 16");
            s->avctx->iTotalError++;
            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
        }

    }

    /* �����P��B֡������вο����еĳ�ʼ��������(IDR���picture�ĵ�һ��slice�򲻽��вο����еĳ�ʼ�� liuxw+00139685 */
    if (!default_ref_list_done)
    {
        fill_default_ref_list(h);
    }

    /* �ο����е�������I֡���������� liuxw+00139685 */
    if (h->slice_type_nos != FF_I_TYPE && decode_ref_pic_list_reordering(h) < 0)
    {
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
        s->avctx->iTotalError++;
        return -1;
    }

    /* ������ǰpicture����һ֡�� liuxw+00139685 */
    if (h->slice_type_nos != FF_I_TYPE)
    {
        s->last_picture_ptr = &h->ref_list[0][0];
        ff_copy_picture(&s->last_picture, s->last_picture_ptr);
    }

    /* ������ǰpicture����һ֡�� liuxw+00139685 */
    if (h->slice_type_nos == FF_B_TYPE)
    {
        s->next_picture_ptr = &h->ref_list[1][0];
        ff_copy_picture(&s->next_picture, s->next_picture_ptr);
    }

    /* ��ȨԤ��(B֡��ʾ��Ȩ��P֡��ʾ��Ȩ liuxw+00139685 */
    if (   (h->pps.weighted_pred          && h->slice_type_nos == FF_P_TYPE )
           ||  (h->pps.weighted_bipred_idc == 1 && h->slice_type_nos == FF_B_TYPE ) )
    { pred_weight_table(h); }
    /* B֡��ʽ��Ȩ liuxw+00139685 */
    else if (h->pps.weighted_bipred_idc == 2 && h->slice_type_nos == FF_B_TYPE)
    { implicit_weight_table(h); }
    else
    {
        /* ���Ȩ��־ liuxw+00139685 */
        h->use_weight = 0;

        for (i = 0; i < 2; i++)
        {
            h->luma_weight_flag[i]   = 0;
            h->chroma_weight_flag[i] = 0;
        }
    }

    /* �������еı�ǣ��ǲο�ͼ���� liuxw+00139685 */
    if (h->nal_ref_idc)
    {
        if (decode_ref_pic_marking(h0, &s->gb) < 0)
        {
            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_MARKER);
            s->avctx->iTotalError++;
            return -1;
        }
    }

    /* �����mbaff���ܣ����ʼ��mbaff�Ĳο�֡���� liuxw+00139685 */
    if (FRAME_MBAFF)
    { fill_mbaff_ref_list(h); }

    /* B֡ʱ��ֱ��Ԥ��ģʽʱ���������е�DistScaleFactor liuxw+00139685 */
    if (h->slice_type_nos == FF_B_TYPE && !h->direct_spatial_mv_pred)
    { direct_dist_scale_factor(h); }

    /* Bֱ֡��Ԥ��ο����г�ʼ�� liuxw+00139685 */
    direct_ref_list_init(h);

    if ( h->slice_type_nos != FF_I_TYPE && h->pps.cabac )
    {
        tmp = get_ue_golomb_31(&s->gb);

        if (tmp > 2)
        {
            //          av_log(s->avctx, AV_LOG_ERROR, "cabac_init_idc[%d] overflow!\n",tmp);
            av_log(s->avctx, AV_LOG_WARNING, "cabac_init_idc[%d] overflow!\n", tmp);
            /* 2010/03/22 17:00:00 liuxw+00139685 */
            /* ���Ӵ����� */
            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_VLC);
            s->avctx->iTotalError++;
            tmp = 0;
            //          return -1;
        }

        h->cabac_init_idc = tmp;
    }

    h->last_qscale_diff = 0;
    tmp = h->pps.init_qp + get_se_golomb(&s->gb);

    if (tmp > 51)
    {
        //      av_log(s->avctx, AV_LOG_ERROR, "QP %u out of range\n", tmp);
        av_log(s->avctx, AV_LOG_WARNING, "QP %u out of range\n", tmp);
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_QP);
        s->avctx->iTotalError++;
        tmp = 26;
        //      return -1;
    }

    /* luma QP chroma QP liuxw+00139685 */
    s->qscale = tmp;
    h->chroma_qp[0] = get_chroma_qp(h, 0, s->qscale);
    h->chroma_qp[1] = get_chroma_qp(h, 1, s->qscale);

    //FIXME qscale / qp ... stuff
    if (h->slice_type == FF_SP_TYPE)
    {
        get_bits1(&s->gb); /* sp_for_switch_flag */
    }

    if (h->slice_type == FF_SP_TYPE || h->slice_type == FF_SI_TYPE)
    {
        get_se_golomb(&s->gb); /* slice_qs_delta */
    }

    h->deblocking_filter = 1;
    h->slice_alpha_c0_offset = 0;
    h->slice_beta_offset = 0;

    if ( h->pps.deblocking_filter_parameters_present )
    {
        tmp = get_ue_golomb_31(&s->gb);

        if (tmp > 2)
        {
            //          av_log(s->avctx, AV_LOG_ERROR, "deblocking_filter_idc[%d] out of range!\n", tmp);
            av_log(s->avctx, AV_LOG_WARNING, "deblocking_filter_idc[%d] out of range!\n", tmp);
            /* 2010/03/22 17:00:00 liuxw+00139685 */
            /* ���Ӵ����� */
            IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_DEBLOCKING);
            s->avctx->iTotalError++;
            tmp = 1;
            //          return -1;
        }

        h->deblocking_filter = tmp;

        if (h->deblocking_filter < 2)
        { h->deblocking_filter ^= 1; } // 1<->0

        if ( h->deblocking_filter )
        {
            h->slice_alpha_c0_offset = get_se_golomb(&s->gb) << 1;
            h->slice_beta_offset = get_se_golomb(&s->gb) << 1;

            /* 2010/03/22 17:00:00 liuxw+00139685 */
            /* ���Ӷ�slice_alpha_c0_offset��slice_beta_offset�ĺϷ��Լ�� */
            if (h->slice_alpha_c0_offset > 12 || h->slice_alpha_c0_offset < -12 || h->slice_beta_offset > 12 || h->slice_beta_offset < -12)
            {
                av_log(s->avctx, AV_LOG_WARNING, "slice_alpha_c0_offset[%d]/slice_beta_offset[%d] out of range!\n", h->slice_alpha_c0_offset, h->slice_beta_offset);
                IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_DEBLOCKING);
                s->avctx->iTotalError++;
                h->slice_alpha_c0_offset = h->slice_beta_offset = 0;
                //				return -1;
            }
        }
    }

    if (   s->avctx->skip_loop_filter >= AVDISCARD_ALL
           || (s->avctx->skip_loop_filter >= AVDISCARD_NONKEY && h->slice_type_nos != FF_I_TYPE)
           || (s->avctx->skip_loop_filter >= AVDISCARD_BIDIR  && h->slice_type_nos == FF_B_TYPE)
           || (s->avctx->skip_loop_filter >= AVDISCARD_NONREF && h->nal_ref_idc == 0))
    { h->deblocking_filter = 0; }

    if (h->deblocking_filter == 1 && h0->max_contexts > 1)
    {
        if (s->avctx->flags2 & CODEC_FLAG2_FAST)
        {
            /* Cheat slightly for speed:
               Do not bother to deblock across slices. */
            h->deblocking_filter = 2;
        }
        else
        {
            h0->max_contexts = 1;

            if (!h0->single_decode_warning)
            {
                av_log(s->avctx, AV_LOG_INFO, "Cannot parallelize deblocking type 1, decoding such frames in sequential order\n");
                h0->single_decode_warning = 1;
            }

            if (h != h0)
            { return 1; } // deblocking switched inside frame
        }
    }

#if 0 //FMO

    if ( h->pps.num_slice_groups > 1  && h->pps.mb_slice_group_map_type >= 3 && h->pps.mb_slice_group_map_type <= 5)
    { slice_group_change_cycle = get_bits(&s->gb, ? ); }

#endif

    h0->last_slice_type = slice_type;
    h->slice_num = ++h0->current_slice;

    if (h->slice_num >= MAX_SLICES)
    {
        //      av_log(s->avctx, AV_LOG_ERROR, "Too many slices, increase MAX_SLICES and recompile\n");
        av_log(s->avctx, AV_LOG_WARNING, "Too many slices, increase MAX_SLICES and recompile\n");
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SLICE(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_NUM);
        s->avctx->iTotalError++;
    }

    for (j = 0; j < 2; j++)
    {
        int idx = 0;
        int *ref2frm = h->ref2frm[h->slice_num & (MAX_SLICES - 1)][j];
        ref2frm[0] =
            ref2frm[1] = -1;

        for (i = 0; i < 16; i++)
            /* 2010/08/10 19:00:00 liuxw+00139685 */
            /* ����ʹ��frame_num���ܵ������⣬���Խ����滻Ϊcoded_picture_number */
            //          ref2frm[i+2]= 4*h->ref_list[j][i].frame_num +(h->ref_list[j][i].reference&3);
        { ref2frm[i + 2] = 4 * h->ref_list[j][i].coded_picture_number + (h->ref_list[j][i].reference & 3); }

        /* 2010/04/09 08:30:00 hanqr+00115692 */
        /* ���Ӷ�frame_mbaff���жϣ�����mbaff��ֻ���16~31���и�ֵ */
        if (!FRAME_MBAFF)
        {
            for (i = 16; i < 32; i++)
                //				ref2frm[i+2]= 4*h->ref_list[j][i].frame_num+(h->ref_list[j][i].reference&3);
            { ref2frm[i + 2] = 4 * h->ref_list[j][i].coded_picture_number + (h->ref_list[j][i].reference & 3); }
        }
        else
        {
            ref2frm[18 + 0] =
                ref2frm[18 + 1] = -1;

            for (i = 16; i < 48; i++)
                //				ref2frm[i+4]= 4*h->ref_list[j][i].frame_num+(h->ref_list[j][i].reference&3);
            { ref2frm[i + 4] = 4 * h->ref_list[j][i].coded_picture_number + (h->ref_list[j][i].reference & 3); }
        }

    }

    h->emu_edge_width = (s->flags & CODEC_FLAG_EMU_EDGE) ? 0 : 16;
    h->emu_edge_height = (FRAME_MBAFF || FIELD_PICTURE) ? 0 : h->emu_edge_width;

    s->avctx->refs = h->sps.ref_frame_count;

    if (s->avctx->debug & FF_DEBUG_PICT_INFO)
    {
        av_log(h->s.avctx, AV_LOG_DEBUG, "slice:%d %s mb:%d %c%s%s pps:%u frame:%d poc:%d/%d ref:%d/%d qp:%d loop:%d:%d:%d weight:%d%s %s\n",
               h->slice_num,
               (s->picture_structure == PICT_FRAME ? "F" : s->picture_structure == PICT_TOP_FIELD ? "T" : "B"),
               first_mb_in_slice,
               av_get_pict_type_char(h->slice_type), h->slice_type_fixed ? " fix" : "", h->nal_unit_type == NAL_IDR_SLICE ? " IDR" : "",
               pps_id, h->frame_num,
               s->current_picture_ptr->field_poc[0], s->current_picture_ptr->field_poc[1],
               h->ref_count[0], h->ref_count[1],
               s->qscale,
               h->deblocking_filter, h->slice_alpha_c0_offset / 2, h->slice_beta_offset / 2,
               h->use_weight,
               h->use_weight == 1 && h->use_weight_chroma ? "c" : "",
               h->slice_type == FF_B_TYPE ? (h->direct_spatial_mv_pred ? "SPAT" : "TEMP") : ""
                  );
    }

    return 0;
}

/**
 *
 */
static inline int get_level_prefix(GetBitContext *gb)
{
    unsigned int buf;
    int log;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf = GET_CACHE(re, gb);

    log = 32 - av_log2(buf);
#ifdef TRACE
    print_bin(buf >> (32 - log), log);
    av_log(NULL, AV_LOG_DEBUG, "%5d %2d %3d lpr @%5d in %s get_level_prefix\n", buf >> (32 - log), log, log - 1, get_bits_count(gb), __FILE__);
#endif

    LAST_SKIP_BITS(re, gb, log);
    CLOSE_READER(re, gb);

    return log - 1;
}

static inline int get_dct8x8_allowed(H264Context *h)
{
    if (h->sps.direct_8x8_inference_flag)
    { return !(*(uint64_t *)h->sub_mb_type & ((MB_TYPE_16x8 | MB_TYPE_8x16 | MB_TYPE_8x8                ) * 0x0001000100010001ULL)); }
    else
    { return !(*(uint64_t *)h->sub_mb_type & ((MB_TYPE_16x8 | MB_TYPE_8x16 | MB_TYPE_8x8 | MB_TYPE_DIRECT2) * 0x0001000100010001ULL)); }
}

/**
 * decodes a residual block.
 * @param n block index
 * @param scantable scantable
 * @param max_coeff number of coefficients in the block
 * @return <0 if an error occurred
 */
static int decode_residual(H264Context *h, GetBitContext *gb, DCTELEM *block, int n, const uint8_t *scantable, const uint32_t *qmul, int max_coeff)
{
    MpegEncContext *const s = &h->s;
    static const int coeff_token_table_index[17] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    int level[16];
    int zeros_left, coeff_num, coeff_token, total_coeff, i, j, trailing_ones, run_before;

    //FIXME put trailing_onex into the context

    if (n == CHROMA_DC_BLOCK_INDEX)
    {
        coeff_token = get_vlc2(gb, chroma_dc_coeff_token_vlc.table, CHROMA_DC_COEFF_TOKEN_VLC_BITS, 1);
        total_coeff = coeff_token >> 2;
    }
    else
    {
        if (n == LUMA_DC_BLOCK_INDEX)
        {
            total_coeff = pred_non_zero_count(h, 0);
            coeff_token = get_vlc2(gb, coeff_token_vlc[ coeff_token_table_index[total_coeff] ].table, COEFF_TOKEN_VLC_BITS, 2);
            total_coeff = coeff_token >> 2;
        }
        else
        {
            total_coeff = pred_non_zero_count(h, n);
            coeff_token = get_vlc2(gb, coeff_token_vlc[ coeff_token_table_index[total_coeff] ].table, COEFF_TOKEN_VLC_BITS, 2);
            total_coeff = coeff_token >> 2;
            h->non_zero_count_cache[ scan8[n] ] = total_coeff;
        }
    }

    //FIXME set last_non_zero?

    if (total_coeff == 0)
    { return 0; }

    if (total_coeff > max_coeff)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "corrupted macroblock %d %d (total_coeff=%d)\n", s->mb_x, s->mb_y, total_coeff);
        av_log(h->s.avctx, AV_LOG_WARNING, "corrupted macroblock %d %d (total_coeff=%d)\n", s->mb_x, s->mb_y, total_coeff);
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ�������ۼƴ������� */
        IMEDIA_SET_ERR_RESIDUAL(s->avctx->iErrorCode, IMEDIA_ERR_RESIDUAL_TOTAL_COEFF);
        s->avctx->iTotalError++;
        return -1;
    }

    trailing_ones = coeff_token & 3;
    tprintf(h->s.avctx, "trailing:%d, total:%d\n", trailing_ones, total_coeff);
    assert(total_coeff <= 16);

    i = show_bits(gb, 3);
    skip_bits(gb, trailing_ones);
    level[0] = 1 - ((i & 4) >> 1);
    level[1] = 1 - ((i & 2)   );
    level[2] = 1 - ((i & 1) << 1);

    if (trailing_ones < total_coeff)
    {
        int mask, prefix;
        int suffix_length = total_coeff > 10 && trailing_ones < 3;
        int bitsi = show_bits(gb, LEVEL_TAB_BITS);
        int level_code = cavlc_level_tab[suffix_length][bitsi][0];

        skip_bits(gb, cavlc_level_tab[suffix_length][bitsi][1]);

        if (level_code >= 100)
        {
            prefix = level_code - 100;

            if (prefix == LEVEL_TAB_BITS)
            { prefix += get_level_prefix(gb); }

            //first coefficient has suffix_length equal to 0 or 1
            if (prefix < 14) //FIXME try to build a large unified VLC table for all this
            {
                if (suffix_length)
                { level_code = (prefix << suffix_length) + get_bits(gb, suffix_length); } //part
                else
                { level_code = (prefix << suffix_length); } //part
            }
            else if (prefix == 14)
            {
                if (suffix_length)
                { level_code = (prefix << suffix_length) + get_bits(gb, suffix_length); } //part
                else
                { level_code = prefix + get_bits(gb, 4); } //part
            }
            else
            {
                level_code = (15 << suffix_length) + get_bits(gb, prefix - 3); //part

                if (suffix_length == 0) { level_code += 15; } //FIXME doesn't make (much)sense

                if (prefix >= 16)
                { level_code += (1 << (prefix - 3)) - 4096; }
            }

            if (trailing_ones < 3) { level_code += 2; }

            suffix_length = 2;
            mask = -(level_code & 1);
            level[trailing_ones] = (((2 + level_code) >> 1) ^ mask) - mask;
        }
        else
        {
            if (trailing_ones < 3) { level_code += (level_code >> 31) | 1; }

            suffix_length = 1;

            if (level_code + 3U > 6U)
            { suffix_length++; }

            level[trailing_ones] = level_code;
        }

        //remaining coefficients have suffix_length > 0
        for (i = trailing_ones + 1; i < total_coeff; i++)
        {
            static const unsigned int suffix_limit[7] = {0, 3, 6, 12, 24, 48, INT_MAX };
            int bitsi = show_bits(gb, LEVEL_TAB_BITS);
            level_code = cavlc_level_tab[suffix_length][bitsi][0];

            skip_bits(gb, cavlc_level_tab[suffix_length][bitsi][1]);

            if (level_code >= 100)
            {
                prefix = level_code - 100;

                if (prefix == LEVEL_TAB_BITS)
                {
                    prefix += get_level_prefix(gb);
                }

                if (prefix < 15)
                {
                    level_code = (prefix << suffix_length) + get_bits(gb, suffix_length);
                }
                else
                {
                    level_code = (15 << suffix_length) + get_bits(gb, prefix - 3);

                    if (prefix >= 16)
                    { level_code += (1 << (prefix - 3)) - 4096; }
                }

                mask = -(level_code & 1);
                level_code = (((2 + level_code) >> 1) ^ mask) - mask;
            }

            level[i] = level_code;

            if (suffix_limit[suffix_length] + level_code > 2U * suffix_limit[suffix_length])
            { suffix_length++; }
        }
    }

    if (total_coeff == max_coeff)
    { zeros_left = 0; }
    else
    {
        if (n == CHROMA_DC_BLOCK_INDEX)
        {
            /* 2010/04/14 11:30:00 liuxw+00139685 [AZ1D01990] */
            /* Ϊ�˷�ֹ���������ӶԱ������ļ�� */
            if (0 > total_coeff - 1 || 3 < total_coeff - 1)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "index[%d] of chroma_dc_total_zeros_vlc is out of range!\n", total_coeff - 1);
                IMEDIA_SET_ERR_SLICE(h->s.avctx->iErrorCode, IMEDIA_ERR_SLICE_ID);
                h->s.avctx->iTotalError++;
                return -1;
            }

            zeros_left = get_vlc2(gb, chroma_dc_total_zeros_vlc[ total_coeff - 1 ].table, CHROMA_DC_TOTAL_ZEROS_VLC_BITS, 1);
        }
        else
        {
            /* 2010/04/14 11:30:00 liuxw+00139685 [AZ1D01990] */
            /* Ϊ�˷�ֹ���������ӶԱ������ļ�� */
            if (0 > total_coeff - 1 || 15 < total_coeff - 1)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "index[%d] of total_zeros_vlc is out of range!\n", total_coeff - 1);
                IMEDIA_SET_ERR_SLICE(h->s.avctx->iErrorCode, IMEDIA_ERR_SLICE_ID);
                h->s.avctx->iTotalError++;
                return -1;
            }

            zeros_left = get_vlc2(gb, total_zeros_vlc[ total_coeff - 1 ].table, TOTAL_ZEROS_VLC_BITS, 1);
        }
    }

    coeff_num = zeros_left + total_coeff - 1;
    j = scantable[coeff_num];

    if (n > 24)
    {
        block[j] = level[0];

        for (i = 1; i < total_coeff; i++)
        {
            if (zeros_left <= 0)
            { run_before = 0; }
            else if (zeros_left < 7)
            {
                run_before = get_vlc2(gb, run_vlc[zeros_left - 1].table, RUN_VLC_BITS, 1);
            }
            else
            {
                run_before = get_vlc2(gb, run7_vlc.table, RUN7_VLC_BITS, 2);
            }

            zeros_left -= run_before;
            coeff_num -= 1 + run_before;
            j = scantable[ coeff_num ];

            block[j] = level[i];
        }
    }
    else
    {
        block[j] = (level[0] * qmul[j] + 32) >> 6;

        for (i = 1; i < total_coeff; i++)
        {
            if (zeros_left <= 0)
            { run_before = 0; }
            else if (zeros_left < 7)
            {
                run_before = get_vlc2(gb, run_vlc[zeros_left - 1].table, RUN_VLC_BITS, 1);
            }
            else
            {
                run_before = get_vlc2(gb, run7_vlc.table, RUN7_VLC_BITS, 2);
            }

            zeros_left -= run_before;
            coeff_num -= 1 + run_before;
            j = scantable[ coeff_num ];

            block[j] = (level[i] * qmul[j] + 32) >> 6;
        }
    }

    if (zeros_left < 0)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "negative number of zero coeffs at %d %d\n", s->mb_x, s->mb_y);
        av_log(h->s.avctx, AV_LOG_WARNING, "negative number of zero coeffs at %d %d\n", s->mb_x, s->mb_y);
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ�������ۼƴ������� */
        IMEDIA_SET_ERR_RESIDUAL(s->avctx->iErrorCode, IMEDIA_ERR_RESIDUAL_ZERO_LEFT);
        s->avctx->iTotalError++;
        return -1;
    }

    return 0;
}

static void predict_field_decoding_flag(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = h->mb_xy;
    int mb_type = (h->slice_table[mb_xy - 1] == h->slice_num)
                  ? s->current_picture.mb_type[mb_xy - 1]
                  : (h->slice_table[mb_xy - s->mb_stride] == h->slice_num)
                  ? s->current_picture.mb_type[mb_xy - s->mb_stride]
                  : 0;
    h->mb_mbaff = h->mb_field_decoding_flag = IS_INTERLACED(mb_type) ? 1 : 0;
}

/**
 * decodes a P_SKIP or B_SKIP macroblock
 */
static void decode_mb_skip(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = h->mb_xy;
    int mb_type = 0;

    memset(h->non_zero_count[mb_xy], 0, 16);

    /*hanqingrui add*/
    memset(h->non_zero_count_8x8[mb_xy], 0, 16);
    /*hanqingrui add end*/

    memset(h->non_zero_count_cache + 8, 0, 8 * 5); //FIXME ugly, remove pfui

    if (MB_FIELD)
    { mb_type |= MB_TYPE_INTERLACED; }

    if ( h->slice_type_nos == FF_B_TYPE )
    {
        // just for fill_caches. pred_direct_motion will set the real mb_type
        mb_type |= MB_TYPE_P0L0 | MB_TYPE_P0L1 | MB_TYPE_DIRECT2 | MB_TYPE_SKIP;

        fill_caches(h, mb_type, 0); //FIXME check what is needed and what not ...
        pred_direct_motion(h, &mb_type);
        mb_type |= MB_TYPE_SKIP;
    }
    else
    {
        int mx, my;
        mb_type |= MB_TYPE_16x16 | MB_TYPE_P0L0 | MB_TYPE_P1L0 | MB_TYPE_SKIP;

        fill_caches(h, mb_type, 0); //FIXME check what is needed and what not ...
        pred_pskip_motion(h, &mx, &my);
        fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, 0, 1);
        fill_rectangle(  h->mv_cache[0][scan8[0]], 4, 4, 8, pack16to32(mx, my), 4);
    }

    write_back_motion(h, mb_type);
    s->current_picture.mb_type[mb_xy] = mb_type;
    s->current_picture.qscale_table[mb_xy] = s->qscale;
    h->slice_table[ mb_xy ] = h->slice_num;
    h->prev_mb_skipped = 1;
}

/**
 * decodes a macroblock
 * @returns 0 if OK, AC_ERROR / DC_ERROR / MV_ERROR if an error is noticed
 */
/* CAVLC: ģʽԤ�⡢����в������ */
static int decode_mb_cavlc(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int mb_xy;
    int partition_count;
    unsigned int mb_type, cbp;
    int dct8x8_allowed = h->pps.transform_8x8_mode;

    mb_xy = h->mb_xy = s->mb_x + s->mb_y * s->mb_stride;

    tprintf(s->avctx, "pic:%d mb:%d/%d\n", h->frame_num, s->mb_x, s->mb_y);
    cbp = 0; /* avoid warning. FIXME: find a solution without slowing
                down the code */

    if (h->slice_type_nos != FF_I_TYPE)
    {
        if (s->mb_skip_run == -1)
        {
            int left_mb;
            s->mb_skip_run = get_ue_golomb(&s->gb);
            /* 2010/04/06 14:30:00 liuxw+00139685 */
            /* ���Ӷ�mb_skip_run���ж� */
            left_mb =  FRAME_MBAFF ? ((s->mb_y & 1) ? (2 * s->mb_x + (s->mb_y - 1) * s->mb_width + 1) : (2 * s->mb_x + s->mb_y * s->mb_width)) : (s->mb_x + s->mb_y / (1 + FIELD_PICTURE) * s->mb_width);
            left_mb = (s->mb_num >> FIELD_PICTURE) - left_mb;

            if (s->mb_skip_run > left_mb)
            {
                av_log(s->avctx, AV_LOG_WARNING, "mb_skip_run[%d] is out of range!\n", s->mb_skip_run);
                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_SKIP_MB_RUN);
                s->avctx->iTotalError++;
                return -1;
            }
        }

        /* ����ǰ�����skip���(p_skip��b_skip��������к����루�õ�MV)������(�޲в */
        if (s->mb_skip_run--)
        {
            if (FRAME_MBAFF && (s->mb_y & 1) == 0)
            {
                if (s->mb_skip_run == 0)
                { h->mb_mbaff = h->mb_field_decoding_flag = get_bits1(&s->gb); }
                else
                { predict_field_decoding_flag(h); }
            }

            decode_mb_skip(h);
            /* 2010/04/13 08:30:00 liuxw+00139685 */
            /* ����ǰ����cbp��Ϊ0 */
            h->cbp_table[mb_xy] = 0;
            h->cbp = 0;
            return 0;
        }
    }

    if (FRAME_MBAFF)
    {
        /* ����ȱ��һ���ж�����??? liuxw+00139685 */
        if ( (s->mb_y & 1) == 0 )
        { h->mb_mbaff = h->mb_field_decoding_flag = get_bits1(&s->gb); }

#ifdef LXW_DEBUG

        if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
        { av_log(h->s.avctx, AV_LOG_WARNING, "frame_num:%d mb_x:%d mb_y:%d mb_field_decoding_flag:%d\n", frame_num, s->mb_x, s->mb_y, h->mb_field_decoding_flag); }

#endif
    }

    h->prev_mb_skipped = 0;

    mb_type = get_ue_golomb(&s->gb);

    if (h->slice_type_nos == FF_B_TYPE)
    {
        if (mb_type < 23)
        {
            partition_count = b_mb_type_info[mb_type].partition_count;
            mb_type =         b_mb_type_info[mb_type].type;
        }
        else
        {
            mb_type -= 23;
            goto decode_intra_mb;
        }
    }
    else if (h->slice_type_nos == FF_P_TYPE)
    {
        if (mb_type < 5)
        {
            partition_count = p_mb_type_info[mb_type].partition_count;
            mb_type =         p_mb_type_info[mb_type].type;
        }
        else
        {
            mb_type -= 5;
            goto decode_intra_mb;
        }
    }
    else
    {
        assert(h->slice_type_nos == FF_I_TYPE);

        if (h->slice_type == FF_SI_TYPE && mb_type)
        { mb_type--; }

    decode_intra_mb:

        if (mb_type > 25)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "mb_type %d in %c slice too large at %d %d\n", mb_type, av_get_pict_type_char(h->slice_type), s->mb_x, s->mb_y);
            /* 2010/03/22 17:00:00 liuxw+00139685 */
            /* ���Ӵ����� */
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_TYPE);
            s->avctx->iTotalError++;
            return -1;
        }

        partition_count = 0;
        cbp = i_mb_type_info[mb_type].cbp;
        h->intra16x16_pred_mode = i_mb_type_info[mb_type].pred_mode;
        mb_type = i_mb_type_info[mb_type].type;
    }

    if (MB_FIELD)
    { mb_type |= MB_TYPE_INTERLACED; }

    h->slice_table[ mb_xy ] = h->slice_num;

    if (IS_INTRA_PCM(mb_type)) /* PCM */
    {
        unsigned int x;

        // We assume these blocks are very rare so we do not optimize it.
        align_get_bits(&s->gb);

        // The pixels are stored in the same order as levels in h->mb array.
        for (x = 0; x < (unsigned int)(CHROMA ? 384 : 256); x++)
        {
            ((uint8_t *)h->mb)[x] = get_bits(&s->gb, 8);
        }

        // In deblocking, the quantizer is 0
        s->current_picture.qscale_table[mb_xy] = 0;
        // All coeffs are present
        memset(h->non_zero_count[mb_xy], 16, 16);

        /*hanqingrui add*/
        memset(h->non_zero_count_8x8[mb_xy], 16, 16);
        /*hanqingrui add end*/

        s->current_picture.mb_type[mb_xy] = mb_type;
        return 0;
    }

    if (MB_MBAFF)
    {
        h->ref_count[0] <<= 1;
        h->ref_count[1] <<= 1;
    }

    fill_caches(h, mb_type, 0);

    /* ֡��Ԥ�� */
    if (IS_INTRA(mb_type))
    {
        int pred_mode;

        //            init_top_left_availability(h);
        /* LUMA: intra4x4Ԥ��ģʽ��intra8x8Ԥ��ģʽ */
        if (IS_INTRA4x4(mb_type))
        {
            int i;
            int di = 1;

            if (dct8x8_allowed && get_bits1(&s->gb))
            {
                mb_type |= MB_TYPE_8x8DCT;
                di = 4;
            }

            //                fill_intra4x4_pred_table(h);
            /* intra4x4: di = 1; intra8x8: di = 4 */
            for (i = 0; i < 16; i += di)
            {
                int mode = pred_intra_mode(h, i); /* ����intra4x4��intra8x8��Ԥ��ģʽ */

                if (!get_bits1(&s->gb)) /* Ԥ��Ĳ�׼����������ж�ȡԤ��ģʽ */
                {
                    const int rem_mode = get_bits(&s->gb, 3);
                    mode = rem_mode + (rem_mode >= mode);
                }

                if (di == 4)
                { fill_rectangle( &h->intra4x4_pred_mode_cache[ scan8[i] ], 2, 2, 8, mode, 1 ); }
                else
                { h->intra4x4_pred_mode_cache[ scan8[i] ] = mode; }
            }

            write_back_intra_pred_mode(h);			/* ����intra4x4��intra8x8��Ԥ��ģʽ */

            /* ���intra4x4��intra8x8��Ԥ��ģʽ�ĺϷ��� */
            if ( check_intra4x4_pred_mode(h) < 0)
            {
                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ���Ӵ����� */
                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                s->avctx->iTotalError++;
                return -1;
            }
        }
        else 	/* LUMA: intra16x16 Ԥ��ģʽ */
        {
            /* 2010/08/13 11:30:00 liuxw+00139685 */
            /* �滻�Ѿ��еĺ������������ڼ��luma intra���Ԥ��ģʽ�ĺϷ��� */
            //			h->intra16x16_pred_mode= check_intra_pred_mode(h, h->intra16x16_pred_mode);/* ����intra16x16Ԥ��ģʽ�����ģʽ�ĺϷ��� */
            h->intra16x16_pred_mode = check_intra_pred_mode_luma(h, h->intra16x16_pred_mode); /* ����intra16x16Ԥ��ģʽ�����ģʽ�ĺϷ��� */

            if (h->intra16x16_pred_mode < 0)
            {
                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ���Ӵ����� */
                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                s->avctx->iTotalError++;
                return -1;
            }
        }

        if (CHROMA) 	/* chroma����(�Ȳ��ǵ�ɫY) */
        {
            pred_mode = check_intra_pred_mode(h, get_ue_golomb_31(&s->gb)); /* ����chroma intra8x8Ԥ��ģʽ�����ģʽ�ĺϷ��� */

            if (pred_mode < 0)
            {
                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ���Ӵ����� */
                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                s->avctx->iTotalError++;
                return -1;
            }

            h->chroma_pred_mode = pred_mode;
        }
    }
    else if (partition_count == 4) 	/* ֡��Ԥ�⣺P8x8��B8x8 */
    {
        int i, j, sub_partition_count[4], list, ref[2][4];

        if (h->slice_type_nos == FF_B_TYPE) 	/* B8x8 */
        {
            for (i = 0; i < 4; i++)
            {
                h->sub_mb_type[i] = get_ue_golomb_31(&s->gb);

                if (h->sub_mb_type[i] >= 13)
                {
                    //                  av_log(h->s.avctx, AV_LOG_ERROR, "B sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    av_log(h->s.avctx, AV_LOG_WARNING, "B sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    /* 2010/03/22 17:00:00 liuxw+00139685 */
                    /* ���Ӵ����� */
                    IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_TYPE);
                    s->avctx->iTotalError++;
                    return -1;
                }

                sub_partition_count[i] = b_sub_mb_type_info[ h->sub_mb_type[i] ].partition_count;
#ifdef LXW_DEBUG

                if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
                { av_log(h->s.avctx, AV_LOG_WARNING, "Partion%d sub_partition_count[%d] sub_mb_type{%d] \n", i, sub_partition_count[i], h->sub_mb_type[i]); }

#endif
                h->sub_mb_type[i] =      b_sub_mb_type_info[ h->sub_mb_type[i] ].type;
            }/* ��4���Ӻ��8x8�д���direct8x8Ԥ��ģʽ�������ֱ��Ԥ��ģʽ */

            if (   IS_DIRECT(h->sub_mb_type[0]) || IS_DIRECT(h->sub_mb_type[1])
                   || IS_DIRECT(h->sub_mb_type[2]) || IS_DIRECT(h->sub_mb_type[3]))
            {
                pred_direct_motion(h, &mb_type);
                h->ref_cache[0][scan8[4]] =
                    h->ref_cache[1][scan8[4]] =
                        h->ref_cache[0][scan8[12]] =
                            h->ref_cache[1][scan8[12]] = PART_NOT_AVAILABLE;
            }
        }
        else
        {
            assert(h->slice_type_nos == FF_P_TYPE); //FIXME SP correct ?

            for (i = 0; i < 4; i++)
            {
                h->sub_mb_type[i] = get_ue_golomb_31(&s->gb);

                if (h->sub_mb_type[i] >= 4)
                {
                    //                  av_log(h->s.avctx, AV_LOG_ERROR, "P sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    av_log(h->s.avctx, AV_LOG_WARNING, "P sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    /* 2010/03/22 17:00:00 liuxw+00139685 */
                    /* ���Ӵ����� */
                    IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_TYPE);
                    s->avctx->iTotalError++;
                    return -1;
                }

                sub_partition_count[i] = p_sub_mb_type_info[ h->sub_mb_type[i] ].partition_count;
#ifdef LXW_DEBUG

                if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
                { av_log(h->s.avctx, AV_LOG_WARNING, "Partion%d sub_partition_count[%d] sub_mb_type{%d] \n", i, sub_partition_count[i], h->sub_mb_type[i]); }

#endif
                h->sub_mb_type[i] =      p_sub_mb_type_info[ h->sub_mb_type[i] ].type;
            }
        }

        for (list = 0; list < (int)h->list_count; list++)
        {
            int ref_count = IS_REF0(mb_type) ? 1 : h->ref_count[list];

            for (i = 0; i < 4; i++)
            {
                if (IS_DIRECT(h->sub_mb_type[i])) { continue; }

                if (IS_DIR(h->sub_mb_type[i], 0, list))
                {
                    unsigned int tmp;

                    if (ref_count == 1)
                    {
                        tmp = 0;
                    }
                    else if (ref_count == 2)
                    {
                        tmp = get_bits1(&s->gb) ^ 1;
                    }
                    else
                    {
                        tmp = get_ue_golomb_31(&s->gb);

                        if ((int)tmp >= ref_count)
                        {
                            //                          av_log(h->s.avctx, AV_LOG_ERROR, "ref[%u] overflow\n", tmp);
                            av_log(h->s.avctx, AV_LOG_WARNING, "ref[%u] overflow\n", tmp);
                            /* 2010/03/22 17:00:00 liuxw+00139685 */
                            /* ���Ӵ����� */
                            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                            s->avctx->iTotalError++;
                            return -1;
                        }
                    }

                    ref[list][i] = tmp;
                }
                else
                {
                    //FIXME
                    ref[list][i] = -1;
                }

#ifdef LXW_DEBUG

                if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
                { av_log(h->s.avctx, AV_LOG_WARNING, "list%d Partion%d ref_index:%d \n", list, i, ref[list][i]); }

#endif
            }
        }

        if (dct8x8_allowed)
        { dct8x8_allowed = get_dct8x8_allowed(h); }

        /* ���16x16��4���Ӻ��8x8��ģʽԤ�� */
        for (list = 0; list < (int)h->list_count; list++)
        {
            for (i = 0; i < 4; i++)
            {
                if (IS_DIRECT(h->sub_mb_type[i]))
                {
                    h->ref_cache[list][ scan8[4 * i] ] = h->ref_cache[list][ scan8[4 * i] + 1 ];
                    continue;
                }

                h->ref_cache[list][ scan8[4 * i]   ] = h->ref_cache[list][ scan8[4 * i] + 1 ] =
                        h->ref_cache[list][ scan8[4 * i] + 8 ] = h->ref_cache[list][ scan8[4 * i] + 9 ] = ref[list][i];

                /* �Ӻ���sub_partition��8x8 8x4 4x8 4x4֡��ģʽԤ��) ������direct8x8 */
                if (IS_DIR(h->sub_mb_type[i], 0, list))
                {
                    const int sub_mb_type = h->sub_mb_type[i];
                    const int block_width = (sub_mb_type & (MB_TYPE_16x16 | MB_TYPE_16x8)) ? 2 : 1;

                    for (j = 0; j < sub_partition_count[i]; j++)
                    {
                        int mx, my;
                        const int index = 4 * i + block_width * j;
                        int16_t (* mv_cache)[2] = &h->mv_cache[list][ scan8[index] ];
                        pred_motion(h, index, block_width, list, h->ref_cache[list][ scan8[index] ], &mx, &my);
                        mx += get_se_golomb(&s->gb);
                        my += get_se_golomb(&s->gb);
#ifdef LXW_DEBUG

                        if (frame_num == TARGET_FRAME && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
                        { av_log(h->s.avctx, AV_LOG_WARNING, "list%d Partion%d SubPartion%d mv_x:%d mv_y:%d\n", list, i, j, mx, my); }

#endif
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        /* p8x8 or b8x8 */
                        if (IS_SUB_8X8(sub_mb_type))
                        {
                            mv_cache[ 1 ][0] =
                                mv_cache[ 8 ][0] = mv_cache[ 9 ][0] = mx;
                            mv_cache[ 1 ][1] =
                                mv_cache[ 8 ][1] = mv_cache[ 9 ][1] = my;
                        }
                        else if (IS_SUB_8X4(sub_mb_type)) 	/* p8x4 or b8x4 */
                        {
                            mv_cache[ 1 ][0] = mx;
                            mv_cache[ 1 ][1] = my;
                        }
                        else if (IS_SUB_4X8(sub_mb_type)) 	/* p4x8 or b4x8 */
                        {
                            mv_cache[ 8 ][0] = mx;
                            mv_cache[ 8 ][1] = my;
                        }

                        mv_cache[ 0 ][0] = mx;
                        mv_cache[ 0 ][1] = my;
                    }
                }
                else
                {
                    uint32_t *p = (uint32_t *)&h->mv_cache[list][ scan8[4 * i] ][0];
                    p[0] = p[1] =
                               p[8] = p[9] = 0;
                }
            }
        }
    }
    else if (IS_DIRECT(mb_type)) 	/* direct16x16֡��Ԥ��ģʽ */
    {
        pred_direct_motion(h, &mb_type);
        dct8x8_allowed &= h->sps.direct_8x8_inference_flag;
    }
    else 	/* ֡������Ԥ��ģʽ��p16x16 p8x16 p16x8 b16x16 b8x16 b16x8 */
    {
        int list, mx, my, i;

        //FIXME we should set ref_idx_l? to 0 if we use that later ...
        if (IS_16X16(mb_type)) 	/* p16x16 or b16x16 */
        {
            for (list = 0; list < (int)h->list_count; list++)
            {
                unsigned int val;

                if (IS_DIR(mb_type, 0, list))
                {
                    if (h->ref_count[list] == 1)
                    {
                        val = 0;
                    }
                    else if (h->ref_count[list] == 2)
                    {
                        val = get_bits1(&s->gb) ^ 1;
                    }
                    else
                    {
                        val = get_ue_golomb_31(&s->gb);

                        if (val >= h->ref_count[list])
                        {
                            //                             av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                            av_log(h->s.avctx, AV_LOG_WARNING, "ref %u overflow\n", val);
                            /* 2010/03/22 17:00:00 liuxw+00139685 */
                            /* ���Ӵ����� */
                            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                            s->avctx->iTotalError++;
                            return -1;
                        }
                    }
                }
                else
                { val = LIST_NOT_USED & 0xFF; }

                fill_rectangle(&h->ref_cache[list][ scan8[0] ], 4, 4, 8, val, 1);
            }

            for (list = 0; list < (int)h->list_count; list++)
            {
                unsigned int val;

                if (IS_DIR(mb_type, 0, list)) /* ����֡��p16x16��b16x16��Ԥ��ģʽ */
                {
                    pred_motion(h, 0, 4, list, h->ref_cache[list][ scan8[0] ], &mx, &my);
                    mx += get_se_golomb(&s->gb);
                    my += get_se_golomb(&s->gb);
                    tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                    val = pack16to32(mx, my);
                }
                else
                { val = 0; }

                fill_rectangle(h->mv_cache[list][ scan8[0] ], 4, 4, 8, val, 4);
            }
        }
        else if (IS_16X8(mb_type)) 	/* p16x8 or b16x8 */
        {
            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++) 	/* 2��16x8���Ӻ�� */
                {
                    unsigned int val;

                    if (IS_DIR(mb_type, i, list))
                    {
                        if (h->ref_count[list] == 1)
                        {
                            val = 0;
                        }
                        else if (h->ref_count[list] == 2)
                        {
                            val = get_bits1(&s->gb) ^ 1;
                        }
                        else
                        {
                            val = get_ue_golomb_31(&s->gb);

                            if (val >= h->ref_count[list])
                            {
                                /* 2010/03/22 17:00:00 liuxw+00139685 */
                                /* ���Ӵ����� */
                                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                                s->avctx->iTotalError++;
                                //                                  av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                av_log(h->s.avctx, AV_LOG_WARNING, "ref %u overflow\n", val);
                                return -1;
                            }
                        }
                    }
                    else
                    { val = LIST_NOT_USED & 0xFF; }

                    fill_rectangle(&h->ref_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, val, 1);
                }
            }/* ����֡��p16x8��b16x8��Ԥ��ģʽ */

            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++) /* 2��16x8���Ӻ�� */
                {
                    unsigned int val;

                    if (IS_DIR(mb_type, i, list))
                    {
                        pred_16x8_motion(h, 8 * i, list, h->ref_cache[list][scan8[0] + 16 * i], &mx, &my);
                        mx += get_se_golomb(&s->gb);
                        my += get_se_golomb(&s->gb);
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        val = pack16to32(mx, my);
                    }
                    else
                    { val = 0; }

                    fill_rectangle(h->mv_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, val, 4);
                }
            }
        }
        else 	/* p8x16 or b8x16 */
        {
            assert(IS_8X16(mb_type));

            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++)
                {
                    unsigned int val;

                    if (IS_DIR(mb_type, i, list)) //FIXME optimize
                    {
                        if (h->ref_count[list] == 1)
                        {
                            val = 0;
                        }
                        else if (h->ref_count[list] == 2)
                        {
                            val = get_bits1(&s->gb) ^ 1;
                        }
                        else
                        {
                            val = get_ue_golomb_31(&s->gb);

                            if (val >= h->ref_count[list])
                            {
                                //                                  av_log(h->s.avctx, AV_LOG_ERROR, "ref %u overflow\n", val);
                                av_log(h->s.avctx, AV_LOG_WARNING, "ref %u overflow\n", val);
                                /* 2010/03/22 17:00:00 liuxw+00139685 */
                                /* ���Ӵ����� */
                                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                                s->avctx->iTotalError++;
                                return -1;
                            }
                        }
                    }
                    else
                    { val = LIST_NOT_USED & 0xFF; }

                    fill_rectangle(&h->ref_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, val, 1);
                }
            }/* ����֡��p8x16��b8x16��Ԥ��ģʽ */

            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++) 	/* 2��8x16���Ӻ�� */
                {
                    unsigned int val;

                    if (IS_DIR(mb_type, i, list))
                    {
                        pred_8x16_motion(h, i * 4, list, h->ref_cache[list][ scan8[0] + 2 * i ], &mx, &my);
                        mx += get_se_golomb(&s->gb);
                        my += get_se_golomb(&s->gb);
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);
                        val = pack16to32(mx, my);
                    }
                    else
                    { val = 0; }

                    fill_rectangle(h->mv_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, val, 4);
                }
            }
        }
    }

    /* ���浱ǰ����֡��Ԥ��ģʽ */
    if (IS_INTER(mb_type))
    { write_back_motion(h, mb_type); }

    /* ������intra16x16�������CBP(��Ϊintra16x16��CBP������mb_type��) */
    if (!IS_INTRA16x16(mb_type))
    {
        cbp = get_ue_golomb(&s->gb);

        if (cbp > 47)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "cbp too large (%u) at %d %d\n", cbp, s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "cbp too large (%u) at %d %d\n", cbp, s->mb_x, s->mb_y);
            /* 2010/03/22 17:00:00 liuxw+00139685 */
            /* ���Ӵ����� */
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_CBP);
            s->avctx->iTotalError++;
            return -1;
        }

        /* chroma����(�Ȳ��ǵ�ɫY) */
        if (CHROMA)
        {
            if (IS_INTRA4x4(mb_type)) { cbp = golomb_to_intra4x4_cbp[cbp]; } /* ֡��cbpת�����ڲ�ʵ���Ż��� */
            else                     { cbp = golomb_to_inter_cbp   [cbp]; } /* ֡��cbpת�����ڲ�ʵ���Ż��� */
        }
        else   /* chroma������(���ǵ�ɫY) */
        {
            /* 2010/05/18 19:00:00 liuxw+00139685 */
            /* ��cbp��ֵ����16������������ */
            cbp %= 16;

            if (IS_INTRA4x4(mb_type)) { cbp = golomb_to_intra4x4_cbp_gray[cbp]; } /* ֡��cbpת�����ڲ�ʵ���Ż��� */
            else                     { cbp = golomb_to_inter_cbp_gray[cbp]; }	/* ֡��cbpת�����ڲ�ʵ���Ż��� */
        }
    }

    h->cbp = cbp;

    if (dct8x8_allowed && (cbp & 15) && !IS_INTRA(mb_type))
    {
        if (get_bits1(&s->gb))
        {
            mb_type |= MB_TYPE_8x8DCT;
            h->cbp_table[mb_xy] = cbp;
        }
    }

    s->current_picture.mb_type[mb_xy] = mb_type;

    /* ��CBP��Ϊ0�������ڲв�)��intra16x16 */
    if (cbp || IS_INTRA16x16(mb_type))
    {
        int i8x8, i4x4, chroma_idx;
        int dquant;
        GetBitContext *gb = IS_INTRA(mb_type) ? h->intra_gb_ptr : h->inter_gb_ptr;
        const uint8_t *scan, *scan8x8, *dc_scan;

        //        fill_non_zero_count_cache(h);
        /* ������(inverse scan)(���������) */
        if (IS_INTERLACED(mb_type))
        {
            scan8x8 = s->qscale ? h->field_scan8x8_cavlc : h->field_scan8x8_cavlc_q0;
            scan = s->qscale ? h->field_scan : h->field_scan_q0;
            dc_scan = luma_dc_field_scan;
        }
        else
        {
            scan8x8 = s->qscale ? h->zigzag_scan8x8_cavlc : h->zigzag_scan8x8_cavlc_q0;
            scan = s->qscale ? h->zigzag_scan : h->zigzag_scan_q0;
            dc_scan = luma_dc_zigzag_scan;
        }

        dquant = get_se_golomb(&s->gb);	/* MB delta QP */

        if ( dquant > 25 || dquant < -26 )
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "dquant out of range (%d) at %d %d\n", dquant, s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "dquant out of range (%d) at %d %d\n", dquant, s->mb_x, s->mb_y);
            /* 2010/03/22 17:00:00 liuxw+00139685 */
            /* ���Ӵ����� */
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_QP);
            s->avctx->iTotalError++;
            return -1;
        }

        s->qscale += dquant;	/* MB LUMA QP */

        if (((unsigned)s->qscale) > 51)
        {
            if (s->qscale < 0) { s->qscale += 52; }
            else            { s->qscale -= 52; }
        }

        h->chroma_qp[0] = get_chroma_qp(h, 0, s->qscale);	/* CB QP */
        h->chroma_qp[1] = get_chroma_qp(h, 1, s->qscale);	/* CR QP */

        /* ����intra16x16�Ĳв� */
        if (IS_INTRA16x16(mb_type))
        {
            /* ���� intra16x16 DC �Ĳв� */
            if ( decode_residual(h, h->intra_gb_ptr, h->mb, LUMA_DC_BLOCK_INDEX, dc_scan, h->dequant4_coeff[0][s->qscale], 16) < 0)
            {
                return -1; //FIXME continue if partitioned and other return -1 too
            }

            assert((cbp & 15) == 0 || (cbp & 15) == 15);

            /* �����вΪ0��8x8�Ӻ�� */
            if (cbp & 15)
            {
                for (i8x8 = 0; i8x8 < 4; i8x8++)
                {
                    /* ����ÿ��4x4�Ŀ�Ĳв���з����� */
                    for (i4x4 = 0; i4x4 < 4; i4x4++)
                    {
                        const int index = i4x4 + 4 * i8x8;

                        if ( decode_residual(h, h->intra_gb_ptr, h->mb + 16 * index, index, scan + 1, h->dequant4_coeff[0][s->qscale], 15) < 0 )
                        {
                            return -1;
                        }
                    }
                }
            }
            else
            {
                fill_rectangle(&h->non_zero_count_cache[scan8[0]], 4, 4, 8, 0, 1);
            }
        }
        else 	/* ����mb_type != intra16x16 && cbp��Ϊ0�ĺ��Ĳв� */
        {
            for (i8x8 = 0; i8x8 < 4; i8x8++) 	/* �����вΪ0��8x8�Ŀ� */
            {
                if (cbp & (1 << i8x8))
                {
                    /* DCTģʽΪ8x8DCT�������Ľ��� */
                    if (IS_8x8DCT(mb_type))
                    {
                        DCTELEM *buf = &h->mb[64 * i8x8];
                        uint8_t *nnz;

                        /* ����8x8��Ĳв���з����� */
                        for (i4x4 = 0; i4x4 < 4; i4x4++)
                        {
                            if ( decode_residual(h, gb, buf, i4x4 + 4 * i8x8, scan8x8 + 16 * i4x4,
                                                 h->dequant8_coeff[IS_INTRA( mb_type ) ? 0 : 1][s->qscale], 16) < 0 )
                            { return -1; }
                        }

                        nnz = &h->non_zero_count_cache[ scan8[4 * i8x8] ];
                        nnz[0] += nnz[1] + nnz[8] + nnz[9];
                    }
                    else   /* DCTģʽΪ4x4DCT�������Ľ��� */
                    {
                        for (i4x4 = 0; i4x4 < 4; i4x4++)
                        {
                            const int index = i4x4 + 4 * i8x8;

                            /* ����4x4��Ĳв���з����� */
                            if ( decode_residual(h, gb, h->mb + 16 * index, index, scan, h->dequant4_coeff[IS_INTRA( mb_type ) ? 0 : 3][s->qscale], 16) < 0 )
                            {
                                return -1;
                            }
                        }
                    }
                }
                else 	/* �в�Ϊ0��8x8�� */
                {
                    uint8_t *const nnz = &h->non_zero_count_cache[ scan8[4 * i8x8] ];
                    nnz[0] = nnz[1] = nnz[8] = nnz[9] = 0;
                }
            }
        }

        /* ����chroma DC�в� */
        if (cbp & 0x30) 	/* ����cb cr DC�в� */
        {
            for (chroma_idx = 0; chroma_idx < 2; chroma_idx++)

                /* ����cb/cr 2x2���DC�в� */
                if ( decode_residual(h, gb, h->mb + 256 + 16 * 4 * chroma_idx, CHROMA_DC_BLOCK_INDEX, chroma_dc_scan, NULL, 4) < 0)
                {
                    return -1;
                }
        }

        /* ����chroma AC�в������ */
        if (cbp & 0x20) 	/* ����cb cr AC 8x8��Ĳв� */
        {
            for (chroma_idx = 0; chroma_idx < 2; chroma_idx++)
            {
                const uint32_t *qmul = h->dequant4_coeff[chroma_idx + 1 + (IS_INTRA( mb_type ) ? 0 : 3)][h->chroma_qp[chroma_idx]];

                for (i4x4 = 0; i4x4 < 4; i4x4++) 	/* ����cb/crÿ��4x4���AC�в� */
                {
                    const int index = 16 + 4 * chroma_idx + i4x4;

                    if ( decode_residual(h, gb, h->mb + 16 * index, index, scan + 1, qmul, 15) < 0)
                    {
                        return -1;
                    }
                }
            }
        }
        else 	/* chroma  AC�в�Ϊ0 */
        {
            uint8_t *const nnz = &h->non_zero_count_cache[0];
            nnz[ scan8[16] + 0 ] = nnz[ scan8[16] + 1 ] = nnz[ scan8[16] + 8 ] = nnz[ scan8[16] + 9 ] =
                                       nnz[ scan8[20] + 0 ] = nnz[ scan8[20] + 1 ] = nnz[ scan8[20] + 8 ] = nnz[ scan8[20] + 9 ] = 0;
        }
    }
    else 	/* CPBΪ0 && mb_type != intra16x16 */
    {
        uint8_t *const nnz = &h->non_zero_count_cache[0];
        fill_rectangle(&nnz[scan8[0]], 4, 4, 8, 0, 1);
        nnz[ scan8[16] + 0 ] = nnz[ scan8[16] + 1 ] = nnz[ scan8[16] + 8 ] = nnz[ scan8[16] + 9 ] =
                                   nnz[ scan8[20] + 0 ] = nnz[ scan8[20] + 1 ] = nnz[ scan8[20] + 8 ] = nnz[ scan8[20] + 9 ] = 0;
    }

    s->current_picture.qscale_table[mb_xy] = s->qscale;
    write_back_non_zero_count(h);	/* ���浱ǰ������ϵ�� */

    if (MB_MBAFF)
    {
        h->ref_count[0] >>= 1;
        h->ref_count[1] >>= 1;
    }

    return 0;
}

static int decode_cabac_field_decoding_flag(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int mb_x = s->mb_x;
    const int mb_y = s->mb_y & ~1;
    const int mba_xy = mb_x - 1 +  mb_y   * s->mb_stride;
    const int mbb_xy = mb_x     + (mb_y - 2) * s->mb_stride;

    unsigned int ctx = 0;

    if ( h->slice_table[mba_xy] == h->slice_num && IS_INTERLACED( s->current_picture.mb_type[mba_xy] ) )
    {
        ctx += 1;
    }

    if ( h->slice_table[mbb_xy] == h->slice_num && IS_INTERLACED( s->current_picture.mb_type[mbb_xy] ) )
    {
        ctx += 1;
    }

    return get_cabac_noinline( &h->cabac, &h->cabac_state[70 + ctx] );
}

static int decode_cabac_intra_mb_type(H264Context *h, int ctx_base, int intra_slice)
{
    uint8_t *state = &h->cabac_state[ctx_base];
    int mb_type;

    if (intra_slice)
    {
        MpegEncContext *const s = &h->s;
        const int mba_xy = h->left_mb_xy[0];
        const int mbb_xy = h->top_mb_xy;
        int ctx = 0;

        if ( h->slice_table[mba_xy] == h->slice_num && !IS_INTRA4x4( s->current_picture.mb_type[mba_xy] ) )
        { ctx++; }

        if ( h->slice_table[mbb_xy] == h->slice_num && !IS_INTRA4x4( s->current_picture.mb_type[mbb_xy] ) )
        { ctx++; }

        if ( get_cabac_noinline( &h->cabac, &state[ctx] ) == 0 )
        { return 0; }   /* I4x4 */

        state += 2;
    }
    else
    {
        if ( get_cabac_noinline( &h->cabac, &state[0] ) == 0 )
        { return 0; }   /* I4x4 */
    }

    if ( get_cabac_terminate( &h->cabac ) )
    { return 25; }  /* PCM */

    mb_type = 1; /* I16x16 */
    mb_type += 12 * get_cabac_noinline( &h->cabac, &state[1] ); /* cbp_luma != 0 */

    if ( get_cabac_noinline( &h->cabac, &state[2] ) ) /* cbp_chroma */
    { mb_type += 4 + 4 * get_cabac_noinline( &h->cabac, &state[2 + intra_slice] ); }

    mb_type += 2 * get_cabac_noinline( &h->cabac, &state[3 + intra_slice] );
    mb_type += 1 * get_cabac_noinline( &h->cabac, &state[3 + 2 * intra_slice] );
    return mb_type;
}

static int decode_cabac_mb_type_b( H264Context *h )
{
    MpegEncContext *const s = &h->s;

    const int mba_xy = h->left_mb_xy[0];
    const int mbb_xy = h->top_mb_xy;
    int ctx = 0;
    int bits;
    assert(h->slice_type_nos == FF_B_TYPE);

    if ( h->slice_table[mba_xy] == h->slice_num && !IS_DIRECT( s->current_picture.mb_type[mba_xy] ) )
    { ctx++; }

    if ( h->slice_table[mbb_xy] == h->slice_num && !IS_DIRECT( s->current_picture.mb_type[mbb_xy] ) )
    { ctx++; }

    if ( !get_cabac_noinline( &h->cabac, &h->cabac_state[27 + ctx] ) )
    { return 0; } /* B_Direct_16x16 */

    if ( !get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 3] ) )
    {
        return 1 + get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 5] ); /* B_L[01]_16x16 */
    }

    bits = get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 4] ) << 3;
    bits |= get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 5] ) << 2;
    bits |= get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 5] ) << 1;
    bits |= get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 5] );

    if ( bits < 8 )
    { return bits + 3; } /* B_Bi_16x16 through B_L1_L0_16x8 */
    else if ( bits == 13 )
    {
        return decode_cabac_intra_mb_type(h, 32, 0) + 23;
    }
    else if ( bits == 14 )
    { return 11; } /* B_L1_L0_8x16 */
    else if ( bits == 15 )
    { return 22; } /* B_8x8 */

    bits = ( bits << 1 ) | get_cabac_noinline( &h->cabac, &h->cabac_state[27 + 5] );
    return bits - 4; /* B_L0_Bi_* through B_Bi_Bi_* */
}

static int decode_cabac_mb_skip( H264Context *h, int mb_x, int mb_y )
{
    MpegEncContext *const s = &h->s;
    int mba_xy, mbb_xy;
    int ctx = 0;

    if (FRAME_MBAFF) //FIXME merge with the stuff in fill_caches?
    {
        int mb_xy = mb_x + (mb_y&~1) * s->mb_stride;
        mba_xy = mb_xy - 1;

        if ( (mb_y & 1)
             && h->slice_table[mba_xy] == h->slice_num
             && MB_FIELD == !!IS_INTERLACED( s->current_picture.mb_type[mba_xy] ) )
        { mba_xy += s->mb_stride; }

        if ( MB_FIELD )
        {
            mbb_xy = mb_xy - s->mb_stride;

            if ( !(mb_y & 1)
                 && h->slice_table[mbb_xy] == h->slice_num
                 && IS_INTERLACED( s->current_picture.mb_type[mbb_xy] ) )
            { mbb_xy -= s->mb_stride; }
        }
        else
        { mbb_xy = mb_x + (mb_y - 1) * s->mb_stride; }
    }
    else
    {
        int mb_xy = h->mb_xy;
        mba_xy = mb_xy - 1;
        mbb_xy = mb_xy - (s->mb_stride << FIELD_PICTURE);
    }

    if ( h->slice_table[mba_xy] == h->slice_num && !IS_SKIP( s->current_picture.mb_type[mba_xy] ))
    { ctx++; }

    if ( h->slice_table[mbb_xy] == h->slice_num && !IS_SKIP( s->current_picture.mb_type[mbb_xy] ))
    { ctx++; }

    if ( h->slice_type_nos == FF_B_TYPE )
    { ctx += 13; }

    return get_cabac_noinline( &h->cabac, &h->cabac_state[11 + ctx] );
}

static int decode_cabac_mb_intra4x4_pred_mode( H264Context *h, int pred_mode )
{
    int mode = 0;

    if ( get_cabac( &h->cabac, &h->cabac_state[68] ) )
    { return pred_mode; }

    mode += 1 * get_cabac( &h->cabac, &h->cabac_state[69] );
    mode += 2 * get_cabac( &h->cabac, &h->cabac_state[69] );
    mode += 4 * get_cabac( &h->cabac, &h->cabac_state[69] );

    if ( mode >= pred_mode )
    { return mode + 1; }
    else
    { return mode; }
}

static int decode_cabac_mb_chroma_pre_mode( H264Context *h)
{
    const int mba_xy = h->left_mb_xy[0];
    const int mbb_xy = h->top_mb_xy;

    int ctx = 0;

    /* No need to test for IS_INTRA4x4 and IS_INTRA16x16, as we set chroma_pred_mode_table to 0 */
    if ( h->slice_table[mba_xy] == h->slice_num && h->chroma_pred_mode_table[mba_xy] != 0 )
    { ctx++; }

    if ( h->slice_table[mbb_xy] == h->slice_num && h->chroma_pred_mode_table[mbb_xy] != 0 )
    { ctx++; }

    if ( get_cabac_noinline( &h->cabac, &h->cabac_state[64 + ctx] ) == 0 )
    { return 0; }

    if ( get_cabac_noinline( &h->cabac, &h->cabac_state[64 + 3] ) == 0 )
    { return 1; }

    if ( get_cabac_noinline( &h->cabac, &h->cabac_state[64 + 3] ) == 0 )
    { return 2; }
    else
    { return 3; }
}

static int decode_cabac_mb_cbp_luma( H264Context *h)
{
    int cbp_b, cbp_a, ctx, cbp = 0;

    cbp_a = h->slice_table[h->left_mb_xy[0]] == h->slice_num ? h->left_cbp : -1;
    cbp_b = h->slice_table[h->top_mb_xy]     == h->slice_num ? h->top_cbp  : -1;

    ctx = !(cbp_a & 0x02) + 2 * !(cbp_b & 0x04);
    cbp |= get_cabac_noinline(&h->cabac, &h->cabac_state[73 + ctx]);
    ctx = !(cbp   & 0x01) + 2 * !(cbp_b & 0x08);
    cbp |= get_cabac_noinline(&h->cabac, &h->cabac_state[73 + ctx]) << 1;
    ctx = !(cbp_a & 0x08) + 2 * !(cbp   & 0x01);
    cbp |= get_cabac_noinline(&h->cabac, &h->cabac_state[73 + ctx]) << 2;
    ctx = !(cbp   & 0x04) + 2 * !(cbp   & 0x02);
    cbp |= get_cabac_noinline(&h->cabac, &h->cabac_state[73 + ctx]) << 3;
    return cbp;
}
static int decode_cabac_mb_cbp_chroma( H264Context *h)
{
    int ctx;
    int cbp_a, cbp_b;

    cbp_a = (h->left_cbp >> 4) & 0x03;
    cbp_b = (h-> top_cbp >> 4) & 0x03;

    ctx = 0;

    if ( cbp_a > 0 ) { ctx++; }

    if ( cbp_b > 0 ) { ctx += 2; }

    if ( get_cabac_noinline( &h->cabac, &h->cabac_state[77 + ctx] ) == 0 )
    { return 0; }

    ctx = 4;

    if ( cbp_a == 2 ) { ctx++; }

    if ( cbp_b == 2 ) { ctx += 2; }

    return 1 + get_cabac_noinline( &h->cabac, &h->cabac_state[77 + ctx] );
}
static int decode_cabac_mb_dqp( H264Context *h)
{
    int   ctx = h->last_qscale_diff != 0;
    int   val = 0;

    while ( get_cabac_noinline( &h->cabac, &h->cabac_state[60 + ctx] ) )
    {
        ctx = 2 + (ctx >> 1);
        val++;

        if (val > 102) //prevent infinite loop
        { return INT_MIN; }
    }

    if ( val & 0x01 )
    { return   (val + 1) >> 1 ; }
    else
    { return -((val + 1) >> 1); }
}
static int decode_cabac_p_mb_sub_type( H264Context *h )
{
    if ( get_cabac( &h->cabac, &h->cabac_state[21] ) )
    { return 0; }   /* 8x8 */

    if ( !get_cabac( &h->cabac, &h->cabac_state[22] ) )
    { return 1; }   /* 8x4 */

    if ( get_cabac( &h->cabac, &h->cabac_state[23] ) )
    { return 2; }   /* 4x8 */

    return 3;       /* 4x4 */
}
static int decode_cabac_b_mb_sub_type( H264Context *h )
{
    int type;

    if ( !get_cabac( &h->cabac, &h->cabac_state[36] ) )
    { return 0; }   /* B_Direct_8x8 */

    if ( !get_cabac( &h->cabac, &h->cabac_state[37] ) )
    { return 1 + get_cabac( &h->cabac, &h->cabac_state[39] ); } /* B_L0_8x8, B_L1_8x8 */

    type = 3;

    if ( get_cabac( &h->cabac, &h->cabac_state[38] ) )
    {
        if ( get_cabac( &h->cabac, &h->cabac_state[39] ) )
        { return 11 + get_cabac( &h->cabac, &h->cabac_state[39] ); } /* B_L1_4x4, B_Bi_4x4 */

        type += 4;
    }

    type += 2 * get_cabac( &h->cabac, &h->cabac_state[39] );
    type +=   get_cabac( &h->cabac, &h->cabac_state[39] );
    return type;
}

static inline int decode_cabac_mb_transform_size( H264Context *h )
{
    return get_cabac_noinline( &h->cabac, &h->cabac_state[399 + h->neighbor_transform_size] );
}

static int decode_cabac_mb_ref( H264Context *h, int list, int n )
{
    int refa = h->ref_cache[list][scan8[n] - 1];
    int refb = h->ref_cache[list][scan8[n] - 8];
    int ref  = 0;
    int ctx  = 0;

    if ( h->slice_type_nos == FF_B_TYPE)
    {
        if ( refa > 0 && !h->direct_cache[scan8[n] - 1] )
        { ctx++; }

        if ( refb > 0 && !h->direct_cache[scan8[n] - 8] )
        { ctx += 2; }
    }
    else
    {
        if ( refa > 0 )
        { ctx++; }

        if ( refb > 0 )
        { ctx += 2; }
    }

    while ( get_cabac( &h->cabac, &h->cabac_state[54 + ctx] ) )
    {
        ref++;
        ctx = (ctx >> 2) + 4;

        if (ref >= 32 /*h->ref_list[list]*/)
        {
            return -1;
        }
    }

    return ref;
}

static int decode_cabac_mb_mvd( H264Context *h, int list, int n, int l )
{
    int amvd = abs( h->mvd_cache[list][scan8[n] - 1][l] ) +
               abs( h->mvd_cache[list][scan8[n] - 8][l] );
    int ctxbase = (l == 0) ? 40 : 47;
    int mvd;
    int ctx = (amvd > 2) + (amvd > 32);

    if (!get_cabac(&h->cabac, &h->cabac_state[ctxbase + ctx]))
    { return 0; }

    mvd = 1;
    ctx = 3;

    while ( mvd < 9 && get_cabac( &h->cabac, &h->cabac_state[ctxbase + ctx] ) )
    {
        mvd++;

        if ( ctx < 6 )
        { ctx++; }
    }

    if ( mvd >= 9 )
    {
        int k = 3;

        while ( get_cabac_bypass( &h->cabac ) )
        {
            mvd += 1 << k;
            k++;

            if (k > 24)
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "overflow in decode_cabac_mb_mvd\n");
                av_log(h->s.avctx, AV_LOG_WARNING, "overflow in decode_cabac_mb_mvd\n");
                IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_MV);
                h->s.avctx->iTotalError++;
                return INT_MIN;
            }
        }

        while ( k-- )
        {
            if ( get_cabac_bypass( &h->cabac ) )
            { mvd += 1 << k; }
        }
    }

    return get_cabac_bypass_sign( &h->cabac, -mvd );
}

static av_always_inline int get_cabac_cbf_ctx( H264Context *h, int cat, int idx, int is_dc )
{
    int nza, nzb;
    int ctx = 0;

    if ( is_dc )
    {
        if ( cat == 0 )
        {
            nza = h->left_cbp & 0x100;
            nzb = h-> top_cbp & 0x100;
        }
        else
        {
            nza = (h->left_cbp >> (6 + idx)) & 0x01;
            nzb = (h-> top_cbp >> (6 + idx)) & 0x01;
        }
    }
    else
    {
        assert(cat == 1 || cat == 2 || cat == 4);
        nza = h->non_zero_count_cache[scan8[idx] - 1];
        nzb = h->non_zero_count_cache[scan8[idx] - 8];
    }

    if ( nza > 0 )
    { ctx++; }

    if ( nzb > 0 )
    { ctx += 2; }

    return ctx + 4 * cat;
}

DECLARE_ASM_CONST(1, uint8_t, last_coeff_flag_offset_8x8[63]) =
{
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8
};

static av_always_inline void decode_cabac_residual_internal( H264Context *h, DCTELEM *block, int cat, int n, const uint8_t *scantable, const uint32_t *qmul, int max_coeff, int is_dc )
{
    static const int significant_coeff_flag_offset[2][6] =
    {
        { 105 + 0, 105 + 15, 105 + 29, 105 + 44, 105 + 47, 402 },
        { 277 + 0, 277 + 15, 277 + 29, 277 + 44, 277 + 47, 436 }
    };
    static const int last_coeff_flag_offset[2][6] =
    {
        { 166 + 0, 166 + 15, 166 + 29, 166 + 44, 166 + 47, 417 },
        { 338 + 0, 338 + 15, 338 + 29, 338 + 44, 338 + 47, 451 }
    };
    static const int coeff_abs_level_m1_offset[6] =
    {
        227 + 0, 227 + 10, 227 + 20, 227 + 30, 227 + 39, 426
    };
    static const uint8_t significant_coeff_flag_offset_8x8[2][63] =
    {
        {
            0, 1, 2, 3, 4, 5, 5, 4, 4, 3, 3, 4, 4, 4, 5, 5,
            4, 4, 4, 4, 3, 3, 6, 7, 7, 7, 8, 9, 10, 9, 8, 7,
            7, 6, 11, 12, 13, 11, 6, 7, 8, 9, 14, 10, 9, 8, 6, 11,
            12, 13, 11, 6, 9, 14, 10, 9, 11, 12, 13, 11, 14, 10, 12
        },
        {
            0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 7, 7, 8, 4, 5,
            6, 9, 10, 10, 8, 11, 12, 11, 9, 9, 10, 10, 8, 11, 12, 11,
            9, 9, 10, 10, 8, 11, 12, 11, 9, 9, 10, 10, 8, 13, 13, 9,
            9, 10, 10, 8, 13, 13, 9, 9, 10, 10, 14, 14, 14, 14, 14
        }
    };
    /* node ctx: 0..3: abslevel1 (with abslevelgt1 == 0).
     * 4..7: abslevelgt1 + 3 (and abslevel1 doesn't matter).
     * map node ctx => cabac ctx for level=1 */
    static const uint8_t coeff_abs_level1_ctx[8] = { 1, 2, 3, 4, 0, 0, 0, 0 };
    /* map node ctx => cabac ctx for level>1 */
    static const uint8_t coeff_abs_levelgt1_ctx[8] = { 5, 5, 5, 5, 6, 7, 8, 9 };
    static const uint8_t coeff_abs_level_transition[2][8] =
    {
        /* update node ctx after decoding a level=1 */
        { 1, 2, 3, 3, 4, 5, 6, 7 },
        /* update node ctx after decoding a level>1 */
        { 4, 4, 4, 4, 5, 6, 7, 7 }
    };

    int index[64];

    int av_unused last;
    int coeff_count = 0;
    int node_ctx = 0;

    uint8_t *significant_coeff_ctx_base;
    uint8_t *last_coeff_ctx_base;
    uint8_t *abs_level_m1_ctx_base;

#if !ARCH_X86
#define CABAC_ON_STACK
#endif
#ifdef CABAC_ON_STACK
#define CC &cc
    CABACContext cc;
    cc.range     = h->cabac.range;
    cc.low       = h->cabac.low;
    cc.bytestream = h->cabac.bytestream;
#else
#define CC &h->cabac
#endif


    /* cat: 0-> DC 16x16  n = 0
     *      1-> AC 16x16  n = luma4x4idx
     *      2-> Luma4x4   n = luma4x4idx
     *      3-> DC Chroma n = iCbCr
     *      4-> AC Chroma n = 16 + 4 * iCbCr + chroma4x4idx
     *      5-> Luma8x8   n = 4 * luma8x8idx
     */

    /* read coded block flag */
    if ( is_dc || cat != 5 )
    {
        if ( get_cabac( CC, &h->cabac_state[85 + get_cabac_cbf_ctx( h, cat, n, is_dc ) ] ) == 0 )
        {
            if ( !is_dc )
            { h->non_zero_count_cache[scan8[n]] = 0; }

#ifdef CABAC_ON_STACK
            h->cabac.range     = cc.range     ;
            h->cabac.low       = cc.low       ;
            h->cabac.bytestream = cc.bytestream;
#endif
            return;
        }
    }

    significant_coeff_ctx_base = h->cabac_state
                                 + significant_coeff_flag_offset[MB_FIELD][cat];
    last_coeff_ctx_base = h->cabac_state
                          + last_coeff_flag_offset[MB_FIELD][cat];
    abs_level_m1_ctx_base = h->cabac_state
                            + coeff_abs_level_m1_offset[cat];

    if ( !is_dc && cat == 5 )
    {
#define DECODE_SIGNIFICANCE( coefs, sig_off, last_off ) \
    for(last= 0; last < coefs; last++) { \
        uint8_t *sig_ctx = significant_coeff_ctx_base + sig_off; \
        if( get_cabac( CC, sig_ctx )) { \
            uint8_t *last_ctx = last_coeff_ctx_base + last_off; \
            index[coeff_count++] = last; \
            if( get_cabac( CC, last_ctx ) ) { \
                last= max_coeff; \
                break; \
            } \
        } \
    }\
    if( last == max_coeff -1 ) {\
        index[coeff_count++] = last;\
    }
        const uint8_t *sig_off = significant_coeff_flag_offset_8x8[MB_FIELD];
#if ARCH_X86 && HAVE_7REGS && HAVE_EBX_AVAILABLE && !defined(BROKEN_RELOCATIONS)
        coeff_count = decode_significance_8x8_x86(CC, significant_coeff_ctx_base, index, sig_off);
    }
    else
    {
        coeff_count = decode_significance_x86(CC, max_coeff, significant_coeff_ctx_base, index);
#else
        DECODE_SIGNIFICANCE( 63, sig_off[last], last_coeff_flag_offset_8x8[last] );
    }
    else
    {
        DECODE_SIGNIFICANCE( max_coeff - 1, last, last );
#endif
    }

    assert(coeff_count > 0);

    if ( is_dc )
    {
        if ( cat == 0 )
        { h->cbp_table[h->mb_xy] |= 0x100; }
        else
        { h->cbp_table[h->mb_xy] |= 0x40 << n; }
    }
    else
    {
        if ( cat == 5 )
        { fill_rectangle(&h->non_zero_count_cache[scan8[n]], 2, 2, 8, coeff_count, 1); }
        else
        {
            assert( cat == 1 || cat == 2 || cat == 4 );
            h->non_zero_count_cache[scan8[n]] = coeff_count;
        }
    }

    do
    {
        uint8_t *ctx = coeff_abs_level1_ctx[node_ctx] + abs_level_m1_ctx_base;

        int j = scantable[index[--coeff_count]];

        if ( get_cabac( CC, ctx ) == 0 )
        {
            node_ctx = coeff_abs_level_transition[0][node_ctx];

            if ( is_dc )
            {
                block[j] = get_cabac_bypass_sign( CC, -1);
            }
            else
            {
                /* add by lxw for test */
                //				av_log(NULL,AV_LOG_ERROR,"index:%d QP:%d ",j,qmul[j]);
                block[j] = (get_cabac_bypass_sign( CC, -((int)qmul[j])) + 32) >> 6;
                //				av_log(NULL,AV_LOG_ERROR,"after QP block[%d]:%d\n",j,block[j]);
            }
        }
        else
        {
            int coeff_abs = 2;
            ctx = coeff_abs_levelgt1_ctx[node_ctx] + abs_level_m1_ctx_base;
            node_ctx = coeff_abs_level_transition[1][node_ctx];

            while ( coeff_abs < 15 && get_cabac( CC, ctx ) )
            {
                coeff_abs++;
            }

            if ( coeff_abs >= 15 )
            {
                int j = 0;

                while ( get_cabac_bypass( CC ) )
                {
                    j++;
                }

                coeff_abs = 1;

                while ( j-- )
                {
                    coeff_abs += coeff_abs + get_cabac_bypass( CC );
                }

                coeff_abs += 14;
            }

            if ( is_dc )
            {
                block[j] = get_cabac_bypass_sign( CC, -coeff_abs );
            }
            else
            {
                /* add by lxw for test */
                //				av_log(NULL,AV_LOG_ERROR,"index:%d QP:%d ",j,qmul[j]);
                block[j] = (get_cabac_bypass_sign( CC, -coeff_abs ) * qmul[j] + 32) >> 6;
                //				av_log(NULL,AV_LOG_ERROR,"after QP block[%d]:%d\n",j,block[j]);
            }
        }
    }
    while ( coeff_count );

#ifdef CABAC_ON_STACK
    h->cabac.range     = cc.range     ;
    h->cabac.low       = cc.low       ;
    h->cabac.bytestream = cc.bytestream;
#endif

}

#if !CONFIG_SMALL
static void decode_cabac_residual_dc( H264Context *h, DCTELEM *block, int cat, int n, const uint8_t *scantable, const uint32_t *qmul, int max_coeff )
{
    decode_cabac_residual_internal(h, block, cat, n, scantable, qmul, max_coeff, 1);
}

static void decode_cabac_residual_nondc( H264Context *h, DCTELEM *block, int cat, int n, const uint8_t *scantable, const uint32_t *qmul, int max_coeff )
{
    decode_cabac_residual_internal(h, block, cat, n, scantable, qmul, max_coeff, 0);
}
#endif

static void decode_cabac_residual( H264Context *h, DCTELEM *block, int cat, int n, const uint8_t *scantable, const uint32_t *qmul, int max_coeff )
{
#if CONFIG_SMALL
    decode_cabac_residual_internal(h, block, cat, n, scantable, qmul, max_coeff, cat == 0 || cat == 3);
#else

    if ( cat == 0 || cat == 3 ) { decode_cabac_residual_dc(h, block, cat, n, scantable, qmul, max_coeff); }
    else { decode_cabac_residual_nondc(h, block, cat, n, scantable, qmul, max_coeff); }

#endif
}

static inline void compute_mb_neighbors(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy  = h->mb_xy;
    h->top_mb_xy     = mb_xy - s->mb_stride;
    h->left_mb_xy[0] = mb_xy - 1;

    if (FRAME_MBAFF)
    {
        const int pair_xy          = s->mb_x     + (s->mb_y & ~1) * s->mb_stride;
        const int top_pair_xy      = pair_xy     - s->mb_stride;
        const int top_mb_field_flag  = IS_INTERLACED(s->current_picture.mb_type[top_pair_xy]);
        const int left_mb_field_flag = IS_INTERLACED(s->current_picture.mb_type[pair_xy - 1]);
        const int curr_mb_field_flag = MB_FIELD;
        const int bottom = (s->mb_y & 1);

        if (curr_mb_field_flag && (bottom || top_mb_field_flag))
        {
            h->top_mb_xy -= s->mb_stride;
        }

        if (!left_mb_field_flag == curr_mb_field_flag)
        {
            h->left_mb_xy[0] = pair_xy - 1;
        }
    }
    else if (FIELD_PICTURE)
    {
        h->top_mb_xy -= s->mb_stride;
    }

    return;
}

/**
 * decodes a macroblock
 * @returns 0 if OK, AC_ERROR / DC_ERROR / MV_ERROR if an error is noticed
 */
/* CABAC: ģʽԤ�⡢����в������ */
static int decode_mb_cabac(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int mb_xy;
    int mb_type, partition_count, cbp = 0;
    int dct8x8_allowed = h->pps.transform_8x8_mode;

    mb_xy = h->mb_xy = s->mb_x + s->mb_y * s->mb_stride;

    tprintf(s->avctx, "pic:%d mb:%d/%d\n", h->frame_num, s->mb_x, s->mb_y);

    if ( h->slice_type_nos != FF_I_TYPE )
    {
        int skip;

        /* a skipped mb needs the aff flag from the following mb */
        if ( FRAME_MBAFF && s->mb_x == 0 && (s->mb_y & 1) == 0 )
        { predict_field_decoding_flag(h); }

        if ( FRAME_MBAFF && (s->mb_y & 1) == 1 && h->prev_mb_skipped )
        { skip = h->next_mb_skipped; }
        else
        { skip = decode_cabac_mb_skip( h, s->mb_x, s->mb_y ); }

        /* read skip flags */
        if ( skip )  	/* ����ǰ�����skip���(p_skip��b_skip��������к����루�õ�MV)������(�޲в */
        {
            if ( FRAME_MBAFF && (s->mb_y & 1) == 0 )
            {
                s->current_picture.mb_type[mb_xy] = MB_TYPE_SKIP;
                h->next_mb_skipped = decode_cabac_mb_skip( h, s->mb_x, s->mb_y + 1 );

                if (!h->next_mb_skipped)
                { h->mb_mbaff = h->mb_field_decoding_flag = decode_cabac_field_decoding_flag(h); }
            }

            decode_mb_skip(h);

            h->cbp_table[mb_xy] = 0;
            /* 2010/04/09 08:30:00 hanqr+00115692 */
            /* ����ǰ����cbp��Ϊ0 */
            h->cbp = 0;
            h->chroma_pred_mode_table[mb_xy] = 0;
            h->last_qscale_diff = 0;

            return 0;

        }
    }

    if (FRAME_MBAFF)
    {
        if ( (s->mb_y & 1) == 0 )
            h->mb_mbaff =
                h->mb_field_decoding_flag = decode_cabac_field_decoding_flag(h);
    }

    h->prev_mb_skipped = 0;
    /* �õ����ں�����Ϣ */
    compute_mb_neighbors(h);

    if ( h->slice_type_nos == FF_B_TYPE )
    {
        mb_type = decode_cabac_mb_type_b( h );

        if ( mb_type < 23 )
        {
            partition_count = b_mb_type_info[mb_type].partition_count;
            mb_type =         b_mb_type_info[mb_type].type;
        }
        else
        {
            mb_type -= 23;
            goto decode_intra_mb;
        }
    }
    else if ( h->slice_type_nos == FF_P_TYPE )
    {
        if ( get_cabac_noinline( &h->cabac, &h->cabac_state[14] ) == 0 )
        {
            /* P-type */
            if ( get_cabac_noinline( &h->cabac, &h->cabac_state[15] ) == 0 )
            {
                /* P_L0_D16x16, P_8x8 */
                mb_type = 3 * get_cabac_noinline( &h->cabac, &h->cabac_state[16] );
            }
            else
            {
                /* P_L0_D8x16, P_L0_D16x8 */
                mb_type = 2 - get_cabac_noinline( &h->cabac, &h->cabac_state[17] );
            }

            partition_count = p_mb_type_info[mb_type].partition_count;
            mb_type =         p_mb_type_info[mb_type].type;
        }
        else
        {
            mb_type = decode_cabac_intra_mb_type(h, 17, 0);
            goto decode_intra_mb;
        }
    }
    else
    {
        mb_type = decode_cabac_intra_mb_type(h, 3, 1);

        if (h->slice_type == FF_SI_TYPE && mb_type)
        { mb_type--; }

        assert(h->slice_type_nos == FF_I_TYPE);
    decode_intra_mb:

        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���ӺϷ����ж� */
        if (mb_type > 25)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "mb_type %d in %c slice too large at %d %d\n", mb_type, av_get_pict_type_char(h->slice_type), s->mb_x, s->mb_y);
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_TYPE);
            s->avctx->iTotalError++;
            return -1;
        }

        partition_count = 0;
        cbp = i_mb_type_info[mb_type].cbp;
        h->intra16x16_pred_mode = i_mb_type_info[mb_type].pred_mode;
        mb_type = i_mb_type_info[mb_type].type;
    }

    if (MB_FIELD)
    { mb_type |= MB_TYPE_INTERLACED; }

    h->slice_table[ mb_xy ] = h->slice_num;

    /* PCM */
    if (IS_INTRA_PCM(mb_type))
    {
        const uint8_t *ptr;

        // We assume these blocks are very rare so we do not optimize it.
        // FIXME The two following lines get the bitstream position in the cabac
        // decode, I think it should be done by a function in cabac.h (or cabac.c).
        ptr = h->cabac.bytestream;

        if (h->cabac.low & 0x1) { ptr--; }

        if (CABAC_BITS == 16)
        {
            if (h->cabac.low & 0x1FF) { ptr--; }
        }

        // The pixels are stored in the same order as levels in h->mb array.
        memcpy(h->mb, ptr, 256);
        ptr += 256;

        if (CHROMA)
        {
            memcpy(h->mb + 128, ptr, 128);
            ptr += 128;
        }

        ff_init_cabac_decoder(&h->cabac, ptr, h->cabac.bytestream_end - ptr);

        // All blocks are present
        h->cbp_table[mb_xy] = 0x1ef;
        h->chroma_pred_mode_table[mb_xy] = 0;
        // In deblocking, the quantizer is 0
        s->current_picture.qscale_table[mb_xy] = 0;
        // All coeffs are present
        memset(h->non_zero_count[mb_xy], 16, 16);

        /*hanqingrui add*/
        memset(h->non_zero_count_8x8[mb_xy], 16, 16);
        /*hanqingrui add end*/

        s->current_picture.mb_type[mb_xy] = mb_type;
        h->last_qscale_diff = 0;
        return 0;
    }

    if (MB_MBAFF)
    {
        h->ref_count[0] <<= 1;
        h->ref_count[1] <<= 1;
    }

    fill_caches(h, mb_type, 0);

    /* ֡��Ԥ�� */
    if ( IS_INTRA( mb_type ) )
    {
        int i, pred_mode;

        /* LUMA: intra4x4Ԥ��ģʽ��intra8x8Ԥ��ģʽ */
        if ( IS_INTRA4x4( mb_type ) )
        {
            /* intra8x8Ԥ��ģʽ */
            if ( dct8x8_allowed && decode_cabac_mb_transform_size( h ) )
            {
                mb_type |= MB_TYPE_8x8DCT;

                for ( i = 0; i < 16; i += 4 )
                {
                    int pred = pred_intra_mode( h, i );
                    int mode = decode_cabac_mb_intra4x4_pred_mode( h, pred );
                    fill_rectangle( &h->intra4x4_pred_mode_cache[ scan8[i] ], 2, 2, 8, mode, 1 );
                }
            }
            else  	/* intra4x4Ԥ��ģʽ */
            {
                for ( i = 0; i < 16; i++ )
                {
                    int pred = pred_intra_mode( h, i );
                    h->intra4x4_pred_mode_cache[ scan8[i] ] = decode_cabac_mb_intra4x4_pred_mode( h, pred );

                    //av_log( s->avctx, AV_LOG_ERROR, "i4x4 pred=%d mode=%d\n", pred, h->intra4x4_pred_mode_cache[ scan8[i] ] );
                }
            }

            write_back_intra_pred_mode(h);	/* ����intra4x4��intra8x8��Ԥ��ģʽ */

            /* ���intra4x4��intra8x8��Ԥ��ģʽ�ĺϷ��� */
            if ( check_intra4x4_pred_mode(h) < 0 )
            {
                /* 2010/03/22 17:00:00 liuxw+00139685 */

                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                s->avctx->iTotalError++;
                return -1;
            }
        }
        else  	/* LUMA: intra16x16 Ԥ��ģʽ */
        {
            /* 2010/08/13 11:30:00 liuxw+00139685 */
            /* �滻�Ѿ��еĺ������������ڼ��luma intra���Ԥ��ģʽ�ĺϷ��� */
            //			h->intra16x16_pred_mode= check_intra_pred_mode( h, h->intra16x16_pred_mode );  /* ����intra16x16Ԥ��ģʽ�����ģʽ�ĺϷ��� */
            h->intra16x16_pred_mode = check_intra_pred_mode_luma( h, h->intra16x16_pred_mode ); /* ����intra16x16Ԥ��ģʽ�����ģʽ�ĺϷ��� */

            if ( h->intra16x16_pred_mode < 0 )
            {
                /* 2010/03/22 17:00:00 liuxw+00139685 */

                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                s->avctx->iTotalError++;
                return -1;
            }
        }

        if (CHROMA) 	/* chroma����(�Ȳ��ǵ�ɫY) */
        {
            h->chroma_pred_mode_table[mb_xy] =
                pred_mode                        = decode_cabac_mb_chroma_pre_mode( h );	/* ����chroma intra8x8Ԥ��ģʽ */
            /* ���chroma intra8x8Ԥ��ģʽ�ĺϷ��� */
            pred_mode = check_intra_pred_mode( h, pred_mode );

            if ( pred_mode < 0 )
            {
                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ���Ӵ�������ۻ��ܵĴ����� */
                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_MODE);
                s->avctx->iTotalError++;
                return -1;
            }

            h->chroma_pred_mode = pred_mode;
        }
    }
    else if ( partition_count == 4 )  	/* ֡��Ԥ�⣺P8x8��B8x8 */
    {
        int i, j, sub_partition_count[4], list, ref[2][4];

        if ( h->slice_type_nos == FF_B_TYPE )  	/* B8x8 */
        {
            for ( i = 0; i < 4; i++ )
            {
                h->sub_mb_type[i] = decode_cabac_b_mb_sub_type( h );

                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ���Ӵ�������ۻ��ܵĴ����� */
                if (h->sub_mb_type[i] >= 13)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "B sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_TYPE);
                    s->avctx->iTotalError++;
                    return -1;
                }

                sub_partition_count[i] = b_sub_mb_type_info[ h->sub_mb_type[i] ].partition_count;
                h->sub_mb_type[i] =      b_sub_mb_type_info[ h->sub_mb_type[i] ].type;
            }	/* ��4���Ӻ��8x8�д���direct8x8Ԥ��ģʽ�������ֱ��Ԥ��ģʽ */

            if ( IS_DIRECT(h->sub_mb_type[0] | h->sub_mb_type[1] |
                           h->sub_mb_type[2] | h->sub_mb_type[3]) )
            {
                pred_direct_motion(h, &mb_type);
                h->ref_cache[0][scan8[4]] =
                    h->ref_cache[1][scan8[4]] =
                        h->ref_cache[0][scan8[12]] =
                            h->ref_cache[1][scan8[12]] = PART_NOT_AVAILABLE;

                if ( h->ref_count[0] > 1 || h->ref_count[1] > 1 )
                {
                    for ( i = 0; i < 4; i++ )
                        if ( IS_DIRECT(h->sub_mb_type[i]) )
                        { fill_rectangle( &h->direct_cache[scan8[4 * i]], 2, 2, 8, 1, 1 ); }
                }
            }
        }
        else
        {
            for ( i = 0; i < 4; i++ )
            {
                h->sub_mb_type[i] = decode_cabac_p_mb_sub_type( h );

                /* 2010/03/22 17:00:00 liuxw+00139685 */
                /* ����sub_mb_type�ĺϷ����ж� */
                if (h->sub_mb_type[i] >= 4)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "P sub_mb_type %u out of range at %d %d\n", h->sub_mb_type[i], s->mb_x, s->mb_y);
                    IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_TYPE);
                    s->avctx->iTotalError++;
                    return -1;
                }

                sub_partition_count[i] = p_sub_mb_type_info[ h->sub_mb_type[i] ].partition_count;
                h->sub_mb_type[i] =      p_sub_mb_type_info[ h->sub_mb_type[i] ].type;
            }
        }

        for ( list = 0; list < (int)h->list_count; list++ )
        {
            for ( i = 0; i < 4; i++ )
            {
                if (IS_DIRECT(h->sub_mb_type[i])) { continue; }

                if (IS_DIR(h->sub_mb_type[i], 0, list))
                {
                    if ( h->ref_count[list] > 1 )
                    {
                        ref[list][i] = decode_cabac_mb_ref( h, list, 4 * i );

                        if (ref[list][i] >= (int)h->ref_count[list])
                        {
                            /* 2010/03/22 17:00:00 liuxw+00139685 */
                            /* ���Ӵ����� */
                            //                              av_log(s->avctx, AV_LOG_ERROR, "Reference %d >= %d\n", ref[list][i], h->ref_count[list]);
                            av_log(s->avctx, AV_LOG_WARNING, "Reference %d >= %d\n", ref[list][i], h->ref_count[list]);
                            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                            s->avctx->iTotalError++;
                            return -1;
                        }
                    }
                    else
                    { ref[list][i] = 0; }
                }
                else
                {
                    ref[list][i] = -1;
                }

                h->ref_cache[list][ scan8[4 * i] + 1 ] =
                    h->ref_cache[list][ scan8[4 * i] + 8 ] = h->ref_cache[list][ scan8[4 * i] + 9 ] = ref[list][i];
            }
        }

        if (dct8x8_allowed)
        { dct8x8_allowed = get_dct8x8_allowed(h); }

        /* �Ӻ��8x8��ģʽԤ�� */
        for (list = 0; list < (int)h->list_count; list++)
        {
            /* 4��8x8���Ӻ�� */
            for (i = 0; i < 4; i++)
            {
                h->ref_cache[list][ scan8[4 * i]   ] = h->ref_cache[list][ scan8[4 * i] + 1 ];

                if (IS_DIRECT(h->sub_mb_type[i]))
                {
                    fill_rectangle(h->mvd_cache[list][scan8[4 * i]], 2, 2, 8, 0, 4);
                    continue;
                }

                /* �Ӻ���sub_partition��8x8 8x4 4x8 4x4֡��ģʽԤ��) ������direct8x8 */
                if (IS_DIR(h->sub_mb_type[i], 0, list) && !IS_DIRECT(h->sub_mb_type[i]))
                {
                    const int sub_mb_type = h->sub_mb_type[i];
                    const int block_width = (sub_mb_type & (MB_TYPE_16x16 | MB_TYPE_16x8)) ? 2 : 1;

                    for (j = 0; j < sub_partition_count[i]; j++)
                    {
                        int mpx, mpy;
                        int mx, my;
                        const int index = 4 * i + block_width * j;
                        int16_t (* mv_cache)[2] = &h->mv_cache[list][ scan8[index] ];
                        int16_t (* mvd_cache)[2] = &h->mvd_cache[list][ scan8[index] ];
                        pred_motion(h, index, block_width, list, h->ref_cache[list][ scan8[index] ], &mpx, &mpy);

                        mx = mpx + decode_cabac_mb_mvd( h, list, index, 0 );
                        my = mpy + decode_cabac_mb_mvd( h, list, index, 1 );
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        /* p8x8 or b8x8 */
                        if (IS_SUB_8X8(sub_mb_type))
                        {
                            mv_cache[ 1 ][0] =
                                mv_cache[ 8 ][0] = mv_cache[ 9 ][0] = mx;
                            mv_cache[ 1 ][1] =
                                mv_cache[ 8 ][1] = mv_cache[ 9 ][1] = my;

                            mvd_cache[ 1 ][0] =
                                mvd_cache[ 8 ][0] = mvd_cache[ 9 ][0] = mx - mpx;
                            mvd_cache[ 1 ][1] =
                                mvd_cache[ 8 ][1] = mvd_cache[ 9 ][1] = my - mpy;
                        }
                        else if (IS_SUB_8X4(sub_mb_type)) 	/* p8x4 or b8x4 */
                        {
                            mv_cache[ 1 ][0] = mx;
                            mv_cache[ 1 ][1] = my;

                            mvd_cache[ 1 ][0] = mx - mpx;
                            mvd_cache[ 1 ][1] = my - mpy;
                        }
                        else if (IS_SUB_4X8(sub_mb_type)) 	/* p4x8 or b4x8 */
                        {
                            mv_cache[ 8 ][0] = mx;
                            mv_cache[ 8 ][1] = my;

                            mvd_cache[ 8 ][0] = mx - mpx;
                            mvd_cache[ 8 ][1] = my - mpy;
                        }

                        mv_cache[ 0 ][0] = mx;
                        mv_cache[ 0 ][1] = my;

                        mvd_cache[ 0 ][0] = mx - mpx;
                        mvd_cache[ 0 ][1] = my - mpy;
                    }
                }
                else 	/* �в�Ϊ0���Ӻ��8x8 */
                {
                    uint32_t *p = (uint32_t *)&h->mv_cache[list][ scan8[4 * i] ][0];
                    uint32_t *pd = (uint32_t *)&h->mvd_cache[list][ scan8[4 * i] ][0];
                    p[0] = p[1] = p[8] = p[9] = 0;
                    pd[0] = pd[1] = pd[8] = pd[9] = 0;
                }
            }
        }
    }
    else if ( IS_DIRECT(mb_type) )  	/* direct16x16֡��Ԥ��ģʽ */
    {
        pred_direct_motion(h, &mb_type);
        fill_rectangle(h->mvd_cache[0][scan8[0]], 4, 4, 8, 0, 4);
        fill_rectangle(h->mvd_cache[1][scan8[0]], 4, 4, 8, 0, 4);
        dct8x8_allowed &= h->sps.direct_8x8_inference_flag;
    }
    else  	/* ֡������Ԥ��ģʽ��p16x16 p8x16 p16x8 b16x16 b8x16 b16x8 */
    {
        int list, mx, my, i, mpx, mpy;

        if (IS_16X16(mb_type)) 	/* p16x16 or b16x16 */
        {
            for (list = 0; list < (int)h->list_count; list++)
            {
                if (IS_DIR(mb_type, 0, list))
                {
                    int ref;

                    if (h->ref_count[list] > 1)
                    {
                        ref = decode_cabac_mb_ref(h, list, 0);

                        if (ref >= (int)h->ref_count[list])
                        {
                            //                          av_log(s->avctx, AV_LOG_ERROR, "Reference %d >= %d\n", ref, h->ref_count[list]);
                            av_log(s->avctx, AV_LOG_WARNING, "Reference %d >= %d\n", ref, h->ref_count[list]);
                            /* 2010/03/22 17:00:00 liuxw+00139685 */
                            /* ���Ӵ����� */
                            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                            s->avctx->iTotalError++;
                            return -1;
                        }
                    }
                    else
                    { ref = 0; }

                    fill_rectangle(&h->ref_cache[list][ scan8[0] ], 4, 4, 8, ref, 1);
                }
                else
                { fill_rectangle(&h->ref_cache[list][ scan8[0] ], 4, 4, 8, (uint8_t)LIST_NOT_USED, 1); } //FIXME factorize and the other fill_rect below too
            }

            /* ����֡��p16x16��b16x16��Ԥ��ģʽ */
            for (list = 0; list < (int)h->list_count; list++)
            {
                if (IS_DIR(mb_type, 0, list))
                {
                    pred_motion(h, 0, 4, list, h->ref_cache[list][ scan8[0] ], &mpx, &mpy);

                    mx = mpx + decode_cabac_mb_mvd( h, list, 0, 0 );
                    my = mpy + decode_cabac_mb_mvd( h, list, 0, 1 );
                    tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                    fill_rectangle(h->mvd_cache[list][ scan8[0] ], 4, 4, 8, pack16to32(mx - mpx, my - mpy), 4);
                    fill_rectangle(h->mv_cache[list][ scan8[0] ], 4, 4, 8, pack16to32(mx, my), 4);
                }
                else
                { fill_rectangle(h->mv_cache[list][ scan8[0] ], 4, 4, 8, 0, 4); }
            }
        }	/* p16x8 or b16x8 */
        else if (IS_16X8(mb_type))
        {
            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++) 	/* 2��16x8���Ӻ�� */
                {
                    if (IS_DIR(mb_type, i, list))
                    {
                        int ref;

                        if (h->ref_count[list] > 1)
                        {
                            ref = decode_cabac_mb_ref( h, list, 8 * i );

                            if (ref >= (int)h->ref_count[list])
                            {
                                //                                  av_log(s->avctx, AV_LOG_ERROR, "Reference %d >= %d\n", ref, h->ref_count[list]);
                                av_log(s->avctx, AV_LOG_WARNING, "Reference %d >= %d\n", ref, h->ref_count[list]);
                                /* 2010/03/22 17:00:00 liuxw+00139685 */
                                /* ���Ӵ����� */
                                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                                s->avctx->iTotalError++;
                                return -1;
                            }
                        }
                        else
                        { ref = 0; }

                        fill_rectangle(&h->ref_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, ref, 1);
                    }
                    else
                    { fill_rectangle(&h->ref_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, (LIST_NOT_USED & 0xFF), 1); }
                }
            }	/* ����֡��p16x8��b16x8��Ԥ��ģʽ */

            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++)
                {
                    if (IS_DIR(mb_type, i, list))
                    {
                        pred_16x8_motion(h, 8 * i, list, h->ref_cache[list][scan8[0] + 16 * i], &mpx, &mpy);
                        mx = mpx + decode_cabac_mb_mvd( h, list, 8 * i, 0 );
                        my = mpy + decode_cabac_mb_mvd( h, list, 8 * i, 1 );
                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);

                        fill_rectangle(h->mvd_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, pack16to32(mx - mpx, my - mpy), 4);
                        fill_rectangle(h->mv_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, pack16to32(mx, my), 4);
                    }
                    else
                    {
                        fill_rectangle(h->mvd_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, 0, 4);
                        fill_rectangle(h-> mv_cache[list][ scan8[0] + 16 * i ], 4, 2, 8, 0, 4);
                    }
                }
            }
        }
        else 	/* p8x16 or b8x16 */
        {
            assert(IS_8X16(mb_type));

            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++)
                {
                    if (IS_DIR(mb_type, i, list)) //FIXME optimize
                    {
                        int ref;

                        if (h->ref_count[list] > 1)
                        {
                            ref = decode_cabac_mb_ref( h, list, 4 * i );

                            if (ref >= (int)h->ref_count[list])
                            {
                                //                                  av_log(s->avctx, AV_LOG_ERROR, "Reference %d >= %d\n", ref, h->ref_count[list]);
                                av_log(s->avctx, AV_LOG_WARNING, "Reference %d >= %d\n", ref, h->ref_count[list]);
                                /* 2010/03/22 17:00:00 liuxw+00139685 */
                                /* ���Ӵ����� */
                                IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_REF);
                                s->avctx->iTotalError++;
                                return -1;
                            }
                        }
                        else
                        { ref = 0; }

                        fill_rectangle(&h->ref_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, ref, 1);
                    }
                    else
                    { fill_rectangle(&h->ref_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, (LIST_NOT_USED & 0xFF), 1); }
                }
            }	/* ����֡��p8x16��b8x16��Ԥ��ģʽ */

            for (list = 0; list < (int)h->list_count; list++)
            {
                for (i = 0; i < 2; i++) 	/* 2��8x16���Ӻ�� */
                {
                    if (IS_DIR(mb_type, i, list))
                    {
                        pred_8x16_motion(h, i * 4, list, h->ref_cache[list][ scan8[0] + 2 * i ], &mpx, &mpy);
                        mx = mpx + decode_cabac_mb_mvd( h, list, 4 * i, 0 );
                        my = mpy + decode_cabac_mb_mvd( h, list, 4 * i, 1 );

                        tprintf(s->avctx, "final mv:%d %d\n", mx, my);
                        fill_rectangle(h->mvd_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, pack16to32(mx - mpx, my - mpy), 4);
                        fill_rectangle(h->mv_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, pack16to32(mx, my), 4);
                    }
                    else
                    {
                        fill_rectangle(h->mvd_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, 0, 4);
                        fill_rectangle(h-> mv_cache[list][ scan8[0] + 2 * i ], 2, 4, 8, 0, 4);
                    }
                }
            }
        }
    }

    /* ���浱ǰ����֡��Ԥ��ģʽ */
    if ( IS_INTER( mb_type ) )
    {
        h->chroma_pred_mode_table[mb_xy] = 0;
        write_back_motion( h, mb_type );
    }

    /* ������intra16x16�������CBP(��Ϊintra16x16��CBP������mb_type��) */
    if ( !IS_INTRA16x16( mb_type ) )
    {
        cbp  = decode_cabac_mb_cbp_luma( h );

        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӷ�cbp���ж� */
        if (cbp > 47)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "cbp too large (%u) at %d %d\n", cbp, s->mb_x, s->mb_y);
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_CBP);
            s->avctx->iTotalError++;
            return -1;
        }

        if (CHROMA)	/* chroma����(�Ȳ��ǵ�ɫY) */
        { cbp |= decode_cabac_mb_cbp_chroma( h ) << 4; }
    }

    h->cbp_table[mb_xy] = h->cbp = cbp;

    if ( dct8x8_allowed && (cbp & 15) && !IS_INTRA( mb_type ) )
    {
        if ( decode_cabac_mb_transform_size( h ) )
        { mb_type |= MB_TYPE_8x8DCT; }
    }

    s->current_picture.mb_type[mb_xy] = mb_type;

    /* ��CBP��Ϊ0�������ڲв�)��intra16x16 */
    if ( cbp || IS_INTRA16x16( mb_type ) )
    {
        const uint8_t *scan, *scan8x8, *dc_scan;
        const uint32_t *qmul;
        int dqp;

        /* ���������(inverse scan) */
        if (IS_INTERLACED(mb_type))
        {
            scan8x8 = s->qscale ? h->field_scan8x8 : h->field_scan8x8_q0;
            scan = s->qscale ? h->field_scan : h->field_scan_q0;
            dc_scan = luma_dc_field_scan;
        }
        else
        {
            scan8x8 = s->qscale ? h->zigzag_scan8x8 : h->zigzag_scan8x8_q0;
            scan = s->qscale ? h->zigzag_scan : h->zigzag_scan_q0;
            dc_scan = luma_dc_zigzag_scan;
        }

        h->last_qscale_diff = dqp = decode_cabac_mb_dqp( h );	/* delta QP */

        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* �޸�dqp�ļ������ */
        if (dqp > 25 || dqp < -26)
        {
            //      if( dqp == INT_MIN ){
            //         av_log(h->s.avctx, AV_LOG_ERROR, "cabac decode of qscale diff failed at %d %d\n", s->mb_x, s->mb_y);
            av_log(h->s.avctx, AV_LOG_WARNING, "cabac decode of qscale diff failed at %d %d\n", s->mb_x, s->mb_y);
            IMEDIA_SET_ERR_MB(s->avctx->iErrorCode, IMEDIA_ERR_MB_QP);
            s->avctx->iTotalError++;
            return -1;
        }

        s->qscale += dqp;	/* MB LUMA QP */

        if (((unsigned)s->qscale) > 51)
        {
            if (s->qscale < 0) { s->qscale += 52; }
            else            { s->qscale -= 52; }
        }

        h->chroma_qp[0] = get_chroma_qp(h, 0, s->qscale);	/* CB QP */
        h->chroma_qp[1] = get_chroma_qp(h, 1, s->qscale);	/* CR QP */

        /* ����intra16x16�Ĳв� */
        if ( IS_INTRA16x16( mb_type ) )
        {
            int i;
            //av_log( s->avctx, AV_LOG_ERROR, "INTRA16x16 DC\n" );
            decode_cabac_residual( h, h->mb, 0, 0, dc_scan, NULL, 16);	/* ���� intra16x16 DC �Ĳв� */

            if ( cbp & 15 )
            {
                /* �����вΪ0��8x8�Ӻ�� */
                qmul = h->dequant4_coeff[0][s->qscale];

                for ( i = 0; i < 16; i++ )
                {
                    //av_log( s->avctx, AV_LOG_ERROR, "INTRA16x16 AC:%d\n", i );
                    decode_cabac_residual(h, h->mb + 16 * i, 1, i, scan + 1, qmul, 15);	/* ����ÿ��4x4�Ŀ�Ĳв���з����� */
                }
            }
            else
            {
                fill_rectangle(&h->non_zero_count_cache[scan8[0]], 4, 4, 8, 0, 1);
            }
        }
        else  	/* ����mb_type != intra16x16 && cbp��Ϊ0�ĺ��Ĳв� */
        {
            int i8x8, i4x4;

            for ( i8x8 = 0; i8x8 < 4; i8x8++ )  	/* �����вΪ0��8x8�Ŀ� */
            {
                if ( cbp & (1 << i8x8) )
                {
                    /* DCTģʽΪ8x8dct�����Ľ��� */
                    if ( IS_8x8DCT(mb_type) )
                    {
                        /* ����8x8��Ĳв���з����� */
                        decode_cabac_residual(h, h->mb + 64 * i8x8, 5, 4 * i8x8,
                                              scan8x8, h->dequant8_coeff[IS_INTRA( mb_type ) ? 0 : 1][s->qscale], 64);
                    }
                    else  	/* DCTģʽΪ4x4dct�����Ľ��� */
                    {
                        qmul = h->dequant4_coeff[IS_INTRA( mb_type ) ? 0 : 3][s->qscale];

                        for ( i4x4 = 0; i4x4 < 4; i4x4++ )
                        {
                            const int index = 4 * i8x8 + i4x4;
                            //av_log( s->avctx, AV_LOG_ERROR, "Luma4x4: %d\n", index );
                            //START_TIMER				/* ����4x4��Ĳв���з����� */
                            decode_cabac_residual(h, h->mb + 16 * index, 2, index, scan, qmul, 16);
                            //STOP_TIMER("decode_residual")
                        }
                    }
                }
                else  	/* �в�Ϊ0��8x8�� */
                {
                    uint8_t *const nnz = &h->non_zero_count_cache[ scan8[4 * i8x8] ];
                    nnz[0] = nnz[1] = nnz[8] = nnz[9] = 0;
                }
            }
        }

        /* ����chroma DC�в� */
        if ( cbp & 0x30 )
        {
            int c;

            for ( c = 0; c < 2; c++ )  	/* ����cb cr DC�в� */
            {
                //av_log( s->avctx, AV_LOG_ERROR, "INTRA C%d-DC\n",c );
                decode_cabac_residual(h, h->mb + 256 + 16 * 4 * c, 3, c, chroma_dc_scan, NULL, 4);	/* ����cb/cr 2x2���DC�в� */
            }
        }

        /* ����chroma AC�в������ */
        if ( cbp & 0x20 )
        {
            int c, i;

            for ( c = 0; c < 2; c++ )  	/* ����cb cr AC 8x8��Ĳв� */
            {
                qmul = h->dequant4_coeff[c + 1 + (IS_INTRA( mb_type ) ? 0 : 3)][h->chroma_qp[c]];

                for ( i = 0; i < 4; i++ )
                {
                    const int index = 16 + 4 * c + i;
                    //av_log( s->avctx, AV_LOG_ERROR, "INTRA C%d-AC %d\n",c, index - 16 );
                    decode_cabac_residual(h, h->mb + 16 * index, 4, index, scan + 1, qmul, 15); /* ����ÿ��4x4��Ĳв���з����� */
                }
            }
        }
        else  	/* chroma  AC�в�Ϊ0 */
        {
            uint8_t *const nnz = &h->non_zero_count_cache[0];
            nnz[ scan8[16] + 0 ] = nnz[ scan8[16] + 1 ] = nnz[ scan8[16] + 8 ] = nnz[ scan8[16] + 9 ] =
                                       nnz[ scan8[20] + 0 ] = nnz[ scan8[20] + 1 ] = nnz[ scan8[20] + 8 ] = nnz[ scan8[20] + 9 ] = 0;
        }
    }
    else
    {
        /* CPBΪ0 && mb_type != intra16x16 */
        uint8_t *const nnz = &h->non_zero_count_cache[0];
        fill_rectangle(&nnz[scan8[0]], 4, 4, 8, 0, 1);
        nnz[ scan8[16] + 0 ] = nnz[ scan8[16] + 1 ] = nnz[ scan8[16] + 8 ] = nnz[ scan8[16] + 9 ] =
                                   nnz[ scan8[20] + 0 ] = nnz[ scan8[20] + 1 ] = nnz[ scan8[20] + 8 ] = nnz[ scan8[20] + 9 ] = 0;
        h->last_qscale_diff = 0;
    }

    s->current_picture.qscale_table[mb_xy] = s->qscale;
    write_back_non_zero_count(h);	/* ���浱ǰ������ϵ�� */

    if (MB_MBAFF)
    {
        h->ref_count[0] >>= 1;
        h->ref_count[1] >>= 1;
    }

    return 0;
}


static void filter_mb_edgev( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp )
{
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table + 52)[index_a];
    const int beta  = (beta_table + 52)[qp + h->slice_beta_offset];

    /* 2010/07/08 9:30:00 liuxw+00139685 [AZ1D02186] */
    /* ���Ӷ�alpha��beta���жϣ����alpha����betaС�ڵ����������£����ý����˲� */
    if (alpha > 0 && beta > 0)
    {
        if ( bS[0] < 4 )
        {
            int8_t tc[4];
            tc[0] = (tc0_table + 52)[index_a][bS[0]];
            tc[1] = (tc0_table + 52)[index_a][bS[1]];
            tc[2] = (tc0_table + 52)[index_a][bS[2]];
            tc[3] = (tc0_table + 52)[index_a][bS[3]];
#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
                av_log(h->s.avctx, AV_LOG_WARNING, "V: qp:%d alpha_c0_offset:%d slice_beta:%d index_a:%d index_b:%d alpha:%d beta:%d bs[0]:%d bs[1]:%d bs[2]:%d bs[3]:%d alpha:%d beta:%d tc[0]:%d tc[1]:%d tc[2]:%d tc[3]:%d\n",
                       qp, h->slice_alpha_c0_offset, h->slice_beta_offset, index_a, qp + h->slice_beta_offset, alpha, beta, bS[0], bS[1], bS[2], bS[3], alpha, beta, tc[0], tc[1], tc[2], tc[3]);

#endif
            h->s.dsp.h264_h_loop_filter_luma(pix, stride, alpha, beta, tc);
        }
        else
        {
#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
                av_log(h->s.avctx, AV_LOG_WARNING, "V: qp:%d alpha_c0_offset:%d slice_beta:%d index_a:%d index_b:%d alpha:%d beta:%d bs[0]:%d bs[1]:%d bs[2]:%d bs[3]:%d alpha:%d beta:%d \n",
                       qp, h->slice_alpha_c0_offset, h->slice_beta_offset, index_a, qp + h->slice_beta_offset, alpha, beta, bS[0], bS[1], bS[2], bS[3], alpha, beta);

#endif
            h->s.dsp.h264_h_loop_filter_luma_intra(pix, stride, alpha, beta);
        }
    }
}
static void filter_mb_edgecv( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp )
{
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table + 52)[index_a];
    const int beta  = (beta_table + 52)[qp + h->slice_beta_offset];

    /* 2010/07/08 9:30:00 liuxw+00139685 [AZ1D02186] */
    /* ���Ӷ�alpha��beta���жϣ����alpha����betaС�ڵ����������£����ý����˲� */
    if (alpha > 0 && beta > 0)
    {
        if ( bS[0] < 4 )
        {
            int8_t tc[4];
            tc[0] = (tc0_table + 52)[index_a][bS[0]] + 1;
            tc[1] = (tc0_table + 52)[index_a][bS[1]] + 1;
            tc[2] = (tc0_table + 52)[index_a][bS[2]] + 1;
            tc[3] = (tc0_table + 52)[index_a][bS[3]] + 1;
            h->s.dsp.h264_h_loop_filter_chroma(pix, stride, alpha, beta, tc);
        }
        else
        {
            h->s.dsp.h264_h_loop_filter_chroma_intra(pix, stride, alpha, beta);
        }
    }
}

static void filter_mb_mbaff_edgev( H264Context *h, uint8_t *pix, int stride, int16_t bS[8], int qp[2] )
{
    int i;

    for ( i = 0; i < 16; i++, pix += stride)
    {
        int index_a;
        int alpha;
        int beta;

        int qp_index;
        int bS_index = (i >> 1);

        if (!MB_FIELD)
        {
            bS_index &= ~1;
            bS_index |= (i & 1);
        }

        if ( bS[bS_index] == 0 )
        {
            continue;
        }

        qp_index = MB_FIELD ? (i >> 3) : (i & 1);
        index_a = qp[qp_index] + h->slice_alpha_c0_offset;
        alpha = (alpha_table + 52)[index_a];
        beta  = (beta_table + 52)[qp[qp_index] + h->slice_beta_offset];

        if ( bS[bS_index] < 4 )
        {
            const int tc0 = (tc0_table + 52)[index_a][bS[bS_index]];
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int p2 = pix[-3];
            const int q0 = pix[0];
            const int q1 = pix[1];
            const int q2 = pix[2];

            if ( FFABS( p0 - q0 ) < alpha && FFABS( p1 - p0 ) < beta && FFABS( q1 - q0 ) < beta )
            {
                int tc = tc0;
                int i_delta;

                if ( FFABS( p2 - p0 ) < beta )
                {
                    pix[-2] = p1 + av_clip( ( p2 + ( ( p0 + q0 + 1 ) >> 1 ) - ( p1 << 1 ) ) >> 1, -tc0, tc0 );
                    tc++;
                }

                if ( FFABS( q2 - q0 ) < beta )
                {
                    pix[1] = q1 + av_clip( ( q2 + ( ( p0 + q0 + 1 ) >> 1 ) - ( q1 << 1 ) ) >> 1, -tc0, tc0 );
                    tc++;
                }

                i_delta = av_clip( (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3, -tc, tc );
                pix[-1] = av_clip_uint8( p0 + i_delta );    /* p0' */
                pix[0]  = av_clip_uint8( q0 - i_delta );    /* q0' */
                tprintf(h->s.avctx, "filter_mb_mbaff_edgev i:%d, qp:%d, indexA:%d, alpha:%d, beta:%d, tc:%d\n# bS:%d -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x]\n", i, qp[qp_index], index_a, alpha, beta, tc, bS[bS_index], pix[-3], p1, p0, q0, q1, pix[2], p1, pix[-1], pix[0], q1);
            }
        }
        else
        {
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int p2 = pix[-3];

            const int q0 = pix[0];
            const int q1 = pix[1];
            const int q2 = pix[2];

            if ( FFABS( p0 - q0 ) < alpha && FFABS( p1 - p0 ) < beta && FFABS( q1 - q0 ) < beta )
            {

                if (FFABS( p0 - q0 ) < (( alpha >> 2 ) + 2 ))
                {
                    if ( FFABS( p2 - p0 ) < beta)
                    {
                        const int p3 = pix[-4];
                        /* p0', p1', p2' */
                        pix[-1] = ( p2 + 2 * p1 + 2 * p0 + 2 * q0 + q1 + 4 ) >> 3;
                        pix[-2] = ( p2 + p1 + p0 + q0 + 2 ) >> 2;
                        pix[-3] = ( 2 * p3 + 3 * p2 + p1 + p0 + q0 + 4 ) >> 3;
                    }
                    else
                    {
                        /* p0' */
                        pix[-1] = ( 2 * p1 + p0 + q1 + 2 ) >> 2;
                    }

                    if ( FFABS( q2 - q0 ) < beta)
                    {
                        const int q3 = pix[3];
                        /* q0', q1', q2' */
                        pix[0] = ( p1 + 2 * p0 + 2 * q0 + 2 * q1 + q2 + 4 ) >> 3;
                        pix[1] = ( p0 + q0 + q1 + q2 + 2 ) >> 2;
                        pix[2] = ( 2 * q3 + 3 * q2 + q1 + q0 + p0 + 4 ) >> 3;
                    }
                    else
                    {
                        /* q0' */
                        pix[0] = ( 2 * q1 + q0 + p1 + 2 ) >> 2;
                    }
                }
                else
                {
                    /* p0', q0' */
                    pix[-1] = ( 2 * p1 + p0 + q1 + 2 ) >> 2;
                    pix[ 0] = ( 2 * q1 + q0 + p1 + 2 ) >> 2;
                }

                tprintf(h->s.avctx, "filter_mb_mbaff_edgev i:%d, qp:%d, indexA:%d, alpha:%d, beta:%d\n# bS:4 -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x, %02x, %02x]\n", i, qp[qp_index], index_a, alpha, beta, p2, p1, p0, q0, q1, q2, pix[-3], pix[-2], pix[-1], pix[0], pix[1], pix[2]);
            }
        }
    }
}
static void filter_mb_mbaff_edgecv( H264Context *h, uint8_t *pix, int stride, int16_t bS[8], int qp[2] )
{
    int i;

    for ( i = 0; i < 8; i++, pix += stride)
    {
        int index_a;
        int alpha;
        int beta;

        int qp_index;
        int bS_index = i;

        if ( bS[bS_index] == 0 )
        {
            continue;
        }

        qp_index = MB_FIELD ? (i >> 2) : (i & 1);
        index_a = qp[qp_index] + h->slice_alpha_c0_offset;
        alpha = (alpha_table + 52)[index_a];
        beta  = (beta_table + 52)[qp[qp_index] + h->slice_beta_offset];

        if ( bS[bS_index] < 4 )
        {
            const int tc = (tc0_table + 52)[index_a][bS[bS_index]] + 1;
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int q0 = pix[0];
            const int q1 = pix[1];

            if ( FFABS( p0 - q0 ) < alpha && FFABS( p1 - p0 ) < beta && FFABS( q1 - q0 ) < beta )
            {
                const int i_delta = av_clip( (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3, -tc, tc );

                pix[-1] = av_clip_uint8( p0 + i_delta );    /* p0' */
                pix[0]  = av_clip_uint8( q0 - i_delta );    /* q0' */
                tprintf(h->s.avctx, "filter_mb_mbaff_edgecv i:%d, qp:%d, indexA:%d, alpha:%d, beta:%d, tc:%d\n# bS:%d -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x]\n", i, qp[qp_index], index_a, alpha, beta, tc, bS[bS_index], pix[-3], p1, p0, q0, q1, pix[2], p1, pix[-1], pix[0], q1);
            }
        }
        else
        {
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int q0 = pix[0];
            const int q1 = pix[1];

            if ( FFABS( p0 - q0 ) < alpha && FFABS( p1 - p0 ) < beta && FFABS( q1 - q0 ) < beta )
            {
                pix[-1] = ( 2 * p1 + p0 + q1 + 2 ) >> 2; /* p0' */
                pix[0]  = ( 2 * q1 + q0 + p1 + 2 ) >> 2; /* q0' */
                tprintf(h->s.avctx, "filter_mb_mbaff_edgecv i:%d\n# bS:4 -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x, %02x, %02x]\n", i, pix[-3], p1, p0, q0, q1, pix[2], pix[-3], pix[-2], pix[-1], pix[0], pix[1], pix[2]);
            }
        }
    }
}

static void filter_mb_edgeh( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp )
{
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table + 52)[index_a];
    const int beta  = (beta_table + 52)[qp + h->slice_beta_offset];

    /* 2010/07/08 9:30:00 liuxw+00139685 [AZ1D02186] */
    /* ���Ӷ�alpha��beta���жϣ����alpha����betaС�ڵ����������£����ý����˲� */
    if (alpha > 0 && beta > 0)
    {
        if ( bS[0] < 4 )
        {
            int8_t tc[4];
            tc[0] = (tc0_table + 52)[index_a][bS[0]];
            tc[1] = (tc0_table + 52)[index_a][bS[1]];
            tc[2] = (tc0_table + 52)[index_a][bS[2]];
            tc[3] = (tc0_table + 52)[index_a][bS[3]];
#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
                av_log(h->s.avctx, AV_LOG_WARNING, "H: qp:%d alpha_c0_offset:%d slice_beta:%d index_a:%d index_b:%d alpha:%d beta:%d bs[0]:%d bs[1]:%d bs[2]:%d bs[3]:%d alpha:%d beta:%d tc[0]:%d tc[1]:%d tc[2]:%d tc[3]:%d\n",
                       qp, h->slice_alpha_c0_offset, h->slice_beta_offset, index_a, qp + h->slice_beta_offset, alpha, beta, bS[0], bS[1], bS[2], bS[3], alpha, beta, tc[0], tc[1], tc[2], tc[3]);

#endif
            h->s.dsp.h264_v_loop_filter_luma(pix, stride, alpha, beta, tc);
        }
        else
        {
#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
                av_log(h->s.avctx, AV_LOG_WARNING, "H: qp:%d alpha_c0_offset:%d slice_beta:%d index_a:%d index_b:%d alpha:%d beta:%d bs[0]:%d bs[1]:%d bs[2]:%d bs[3]:%d alpha:%d beta:%d\n",
                       qp, h->slice_alpha_c0_offset, h->slice_beta_offset, index_a, qp + h->slice_beta_offset, alpha, beta, bS[0], bS[1], bS[2], bS[3], alpha, beta);

#endif
            h->s.dsp.h264_v_loop_filter_luma_intra(pix, stride, alpha, beta);
        }
    }
}

static void filter_mb_edgech( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp )
{
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table + 52)[index_a];
    const int beta  = (beta_table + 52)[qp + h->slice_beta_offset];

    /* 2010/07/08 9:30:00 liuxw+00139685 [AZ1D02186] */
    /* ���Ӷ�alpha��beta���жϣ����alpha����betaС�ڵ����������£����ý����˲� */
    if (alpha > 0 && beta > 0)
    {
        if ( bS[0] < 4 )
        {
            int8_t tc[4];
            tc[0] = (tc0_table + 52)[index_a][bS[0]] + 1;
            tc[1] = (tc0_table + 52)[index_a][bS[1]] + 1;
            tc[2] = (tc0_table + 52)[index_a][bS[2]] + 1;
            tc[3] = (tc0_table + 52)[index_a][bS[3]] + 1;
            h->s.dsp.h264_v_loop_filter_chroma(pix, stride, alpha, beta, tc);
        }
        else
        {
            h->s.dsp.h264_v_loop_filter_chroma_intra(pix, stride, alpha, beta);
        }
    }
}

static void filter_mb_fast( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize)
{
    MpegEncContext *const s = &h->s;
    int mb_y_firstrow = s->picture_structure == PICT_BOTTOM_FIELD;
    int mb_xy, mb_type;
    int qp, qp0, qp1, qpc, qpc0, qpc1, qp_thresh;

    mb_xy = h->mb_xy;

    if (mb_x == 0 || mb_y == mb_y_firstrow || !s->dsp.h264_loop_filter_strength || h->pps.chroma_qp_diff ||
        !(s->flags2 & CODEC_FLAG2_FAST) || //FIXME filter_mb_fast is broken, thus hasto be, but should not under CODEC_FLAG2_FAST
        (h->deblocking_filter == 2 && (h->slice_table[mb_xy] != h->slice_table[h->top_mb_xy] ||
                                       h->slice_table[mb_xy] != h->slice_table[mb_xy - 1])))
    {
        filter_mb(h, mb_x, mb_y, img_y, img_cb, img_cr, linesize, uvlinesize);
        return;
    }

    assert(!FRAME_MBAFF);

    mb_type = s->current_picture.mb_type[mb_xy];
    qp = s->current_picture.qscale_table[mb_xy];
    qp0 = s->current_picture.qscale_table[mb_xy - 1];
    qp1 = s->current_picture.qscale_table[h->top_mb_xy];
    qpc = get_chroma_qp( h, 0, qp );
    qpc0 = get_chroma_qp( h, 0, qp0 );
    qpc1 = get_chroma_qp( h, 0, qp1 );
    qp0 = (qp + qp0 + 1) >> 1;
    qp1 = (qp + qp1 + 1) >> 1;
    qpc0 = (qpc + qpc0 + 1) >> 1;
    qpc1 = (qpc + qpc1 + 1) >> 1;
    qp_thresh = 15 - h->slice_alpha_c0_offset;

    if (qp <= qp_thresh && qp0 <= qp_thresh && qp1 <= qp_thresh &&
        qpc <= qp_thresh && qpc0 <= qp_thresh && qpc1 <= qp_thresh)
    { return; }

    if ( IS_INTRA(mb_type) )
    {
        int16_t bS4[4] = {4, 4, 4, 4};
        int16_t bS3[4] = {3, 3, 3, 3};
        int16_t *bSH = FIELD_PICTURE ? bS3 : bS4;

        if ( IS_8x8DCT(mb_type) )
        {
            filter_mb_edgev( h, &img_y[4 * 0], linesize, bS4, qp0 );
            filter_mb_edgev( h, &img_y[4 * 2], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4 * 0 * linesize], linesize, bSH, qp1 );
            filter_mb_edgeh( h, &img_y[4 * 2 * linesize], linesize, bS3, qp );
        }
        else
        {
            filter_mb_edgev( h, &img_y[4 * 0], linesize, bS4, qp0 );
            filter_mb_edgev( h, &img_y[4 * 1], linesize, bS3, qp );
            filter_mb_edgev( h, &img_y[4 * 2], linesize, bS3, qp );
            filter_mb_edgev( h, &img_y[4 * 3], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4 * 0 * linesize], linesize, bSH, qp1 );
            filter_mb_edgeh( h, &img_y[4 * 1 * linesize], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4 * 2 * linesize], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4 * 3 * linesize], linesize, bS3, qp );
        }

        filter_mb_edgecv( h, &img_cb[2 * 0], uvlinesize, bS4, qpc0 );
        filter_mb_edgecv( h, &img_cb[2 * 2], uvlinesize, bS3, qpc );
        filter_mb_edgecv( h, &img_cr[2 * 0], uvlinesize, bS4, qpc0 );
        filter_mb_edgecv( h, &img_cr[2 * 2], uvlinesize, bS3, qpc );
        filter_mb_edgech( h, &img_cb[2 * 0 * uvlinesize], uvlinesize, bSH, qpc1 );
        filter_mb_edgech( h, &img_cb[2 * 2 * uvlinesize], uvlinesize, bS3, qpc );
        filter_mb_edgech( h, &img_cr[2 * 0 * uvlinesize], uvlinesize, bSH, qpc1 );
        filter_mb_edgech( h, &img_cr[2 * 2 * uvlinesize], uvlinesize, bS3, qpc );
        return;
    }
    else
    {
        DECLARE_ALIGNED_8(int16_t, bS[2][4][4]);
        uint64_t (*bSv)[4] = (uint64_t( *)[4])bS;
        int edges;

        if ( IS_8x8DCT(mb_type) && (h->cbp & 7) == 7 )
        {
            edges = 4;
            bSv[0][0] = bSv[0][2] = bSv[1][0] = bSv[1][2] = 0x0002000200020002ULL;
        }
        else
        {
            int mask_edge1 = (mb_type & (MB_TYPE_16x16 | MB_TYPE_8x16)) ? 3 :
                             (mb_type & MB_TYPE_16x8) ? 1 : 0;
            int mask_edge0 = (mb_type & (MB_TYPE_16x16 | MB_TYPE_8x16))
                             && (s->current_picture.mb_type[mb_xy - 1] & (MB_TYPE_16x16 | MB_TYPE_8x16))
                             ? 3 : 0;
            int step = IS_8x8DCT(mb_type) ? 2 : 1;
            edges = (mb_type & MB_TYPE_16x16) && !(h->cbp & 15) ? 1 : 4;
            s->dsp.h264_loop_filter_strength( bS, h->non_zero_count_cache, h->ref_cache, h->mv_cache,
                                              (h->slice_type_nos == FF_B_TYPE), edges, step, mask_edge0, mask_edge1, FIELD_PICTURE);
        }

        if ( IS_INTRA(s->current_picture.mb_type[mb_xy - 1]) )
        { bSv[0][0] = 0x0004000400040004ULL; }

        if ( IS_INTRA(s->current_picture.mb_type[h->top_mb_xy]) )
        { bSv[1][0] = FIELD_PICTURE ? 0x0003000300030003ULL : 0x0004000400040004ULL; }

#define FILTER(hv,dir,edge)\
    if(bSv[dir][edge]) {\
        filter_mb_edge##hv( h, &img_y[4*edge*(dir?linesize:1)], linesize, bS[dir][edge], edge ? qp : qp##dir );\
        if(!(edge&1)) {\
            filter_mb_edgec##hv( h, &img_cb[2*edge*(dir?uvlinesize:1)], uvlinesize, bS[dir][edge], edge ? qpc : qpc##dir );\
            filter_mb_edgec##hv( h, &img_cr[2*edge*(dir?uvlinesize:1)], uvlinesize, bS[dir][edge], edge ? qpc : qpc##dir );\
        }\
    }

        if ( edges == 1 )
        {
            FILTER(v, 0, 0);
            FILTER(h, 1, 0);
        }
        else if ( IS_8x8DCT(mb_type) )
        {
            FILTER(v, 0, 0);
            FILTER(v, 0, 2);
            FILTER(h, 1, 0);
            FILTER(h, 1, 2);
        }
        else
        {
            FILTER(v, 0, 0);
            FILTER(v, 0, 1);
            FILTER(v, 0, 2);
            FILTER(v, 0, 3);
            FILTER(h, 1, 0);
            FILTER(h, 1, 1);
            FILTER(h, 1, 2);
            FILTER(h, 1, 3);
        }

#undef FILTER
    }
}


static av_always_inline void filter_mb_dir(H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize, int mb_xy, int mb_type, int mvy_limit, int first_vertical_edge_done, int dir)
{
    MpegEncContext *const s = &h->s;
    int edge;
    const int mbm_xy = dir == 0 ? mb_xy - 1 : h->top_mb_xy;
    const int mbm_type = s->current_picture.mb_type[mbm_xy];
    /* 2010/04/23 14:30:00 liuxw+00139685 */
    /* ��������warning  */
    //  int (*ref2frm) [64] = h->ref2frm[ h->slice_num & (MAX_SLICES-1) ][0] + (MB_MBAFF ? 20 : 2);
    //  int (*ref2frmm)[64] = h->ref2frm[ h->slice_table[mbm_xy] & (MAX_SLICES-1) ][0] + (MB_MBAFF ? 20 : 2);
    int (*ref2frm) [64] = (int( *)[64])(h->ref2frm[h->slice_num & (MAX_SLICES - 1)][0] + (MB_MBAFF ? 20 : 2));
    int (*ref2frmm)[64] = (int( *)[64])(h->ref2frm[h->slice_table[mbm_xy] & (MAX_SLICES - 1) ][0] + (MB_MBAFF ? 20 : 2));

    int start = h->slice_table[mbm_xy] == 0xFFFF ? 1 : 0;

    const int edges = (mb_type & (MB_TYPE_16x16 | MB_TYPE_SKIP))
                      == (MB_TYPE_16x16 | MB_TYPE_SKIP) ? 1 : 4;
    // how often to recheck mv-based bS when iterating between edges
    const int mask_edge = (mb_type & (MB_TYPE_16x16 | (MB_TYPE_16x8 << dir))) ? 3 :
                          (mb_type & (MB_TYPE_8x16 >> dir)) ? 1 : 0;
    // how often to recheck mv-based bS when iterating along each edge
    const int mask_par0 = mb_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir));

    if (first_vertical_edge_done)
    {
        start = 1;
    }

    if (h->deblocking_filter == 2 && h->slice_table[mbm_xy] != h->slice_table[mb_xy])
    { start = 1; }

    if (FRAME_MBAFF && (dir == 1) && ((mb_y & 1) == 0) && start == 0 && !IS_INTERLACED(mb_type) && IS_INTERLACED(mbm_type))
    {
        // This is a special case in the norm where the filtering must
        // be done twice (one each of the field) even if we are in a
        // frame macroblock.
        //
        static const int nnz_idx[4] = {4, 5, 6, 3};
        unsigned int tmp_linesize   = 2 *   linesize;
        unsigned int tmp_uvlinesize = 2 * uvlinesize;
        int mbn_xy = mb_xy - 2 * s->mb_stride;
        int qp;
        int i, j;
        int16_t bS[4];

        for (j = 0; j < 2; j++, mbn_xy += s->mb_stride)
        {
            if ( IS_INTRA(mb_type) || IS_INTRA(s->current_picture.mb_type[mbn_xy]) )
            {
                bS[0] = bS[1] = bS[2] = bS[3] = 3;
            }
            else
            {
                const uint8_t *mbn_nnz = h->non_zero_count[mbn_xy];

                for ( i = 0; i < 4; i++ )
                {

                    if (IS_8x8DCT(h->s.current_picture.mb_type[mbn_xy]))
                    {
                        if ( h->non_zero_count_cache[scan8[0] + i] != 0 || h->non_zero_count_8x8[mbn_xy][(i >> 1) + 2] != 0 )
                        { bS[i] = 2; }
                        else
                        { bS[i] = 1; }

                    }
                    else
                    {
                        if ( h->non_zero_count_cache[scan8[0] + i] != 0 ||
                             mbn_nnz[nnz_idx[i]] != 0 )
                        { bS[i] = 2; }
                        else
                        { bS[i] = 1; }
                    }
                }
            }

            // Do not use s->qscale as luma quantizer because it has not the same
            // value in IPCM macroblocks.
            qp = ( s->current_picture.qscale_table[mb_xy] + s->current_picture.qscale_table[mbn_xy] + 1 ) >> 1;
            tprintf(s->avctx, "filter mb:%d/%d dir:%d edge:%d, QPy:%d ls:%d uvls:%d", mb_x, mb_y, dir, edge, qp, tmp_linesize, tmp_uvlinesize);
            {
                int i;

                for (i = 0; i < 4; i++) { tprintf(s->avctx, " bS[%d]:%d", i, bS[i]); }

                tprintf(s->avctx, "\n");
            }
            filter_mb_edgeh( h, &img_y[j * linesize], tmp_linesize, bS, qp );
            filter_mb_edgech( h, &img_cb[j * uvlinesize], tmp_uvlinesize, bS,
                              ( h->chroma_qp[0] + get_chroma_qp( h, 0, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
            filter_mb_edgech( h, &img_cr[j * uvlinesize], tmp_uvlinesize, bS,
                              ( h->chroma_qp[1] + get_chroma_qp( h, 1, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
        }

        start = 1;
    }

    /* Calculate bS */
    for ( edge = start; edge < edges; edge++ )
    {
        /* mbn_xy: neighbor macroblock */
        const int mbn_xy = edge > 0 ? mb_xy : mbm_xy;
        const int mbn_type = s->current_picture.mb_type[mbn_xy];
        int (*ref2frmn)[64] = edge > 0 ? ref2frm : ref2frmm;
        int16_t bS[4];
        int qp;

        if ( (edge & 1) && IS_8x8DCT(mb_type) )
        { continue; }

        if ( IS_INTRA(mb_type) || IS_INTRA(mbn_type) )
        {
            int value;

            if (edge == 0)
            {
                if ( (!IS_INTERLACED(mb_type) && !IS_INTERLACED(mbm_type)) || ((FRAME_MBAFF || (s->picture_structure != PICT_FRAME)) && (dir == 0)))
                {
                    value = 4;
                }
                else
                {
                    value = 3;
                }
            }
            else
            {
                value = 3;
            }

            bS[0] = bS[1] = bS[2] = bS[3] = value;
        }
        else
        {
            //          int i, l;
            int i ;
            int mv_done;

            if ( edge & mask_edge )
            {
                bS[0] = bS[1] = bS[2] = bS[3] = 0;
                mv_done = 1;
            }
            else if ( FRAME_MBAFF && IS_INTERLACED(mb_type ^ mbn_type))
            {
                bS[0] = bS[1] = bS[2] = bS[3] = 1;
                mv_done = 1;
            }
            else if ( mask_par0 && (edge || (mbn_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir)))) )
            {
                int b_idx = 8 + 4 + edge * (dir ? 8 : 1);
                int bn_idx = b_idx - (dir ? 8 : 1);
                int v = 0;
                /* 2010/08/22 16:30:00 liuxw+00139685 */
                /* ����ԭ�ȵĴ���û����ȫ����Э����ʵ�֣������޸�Ϊ��ȫ����Э����ʵ�� */
#if 0

                for ( l = 0; !v && l < 1 + (h->slice_type_nos == FF_B_TYPE); l++ )
                {
                    v |= ref2frm[l][h->ref_cache[l][b_idx]] != ref2frmn[l][h->ref_cache[l][bn_idx]] ||
                         FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[l][bn_idx][0] ) >= 4 ||
                         FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[l][bn_idx][1] ) >= mvy_limit;
                }

                if (h->slice_type_nos == FF_B_TYPE && v)
                {
                    v = 0;

                    for ( l = 0; !v && l < 2; l++ )
                    {
                        int ln = 1 - l;
                        v |= ref2frm[l][h->ref_cache[l][b_idx]] != ref2frmn[ln][h->ref_cache[ln][bn_idx]] ||
                             FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[ln][bn_idx][0] ) >= 4 ||
                             FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[ln][bn_idx][1] ) >= mvy_limit;
                    }
                }

#else

                int ref_q0 = h->ref_cache[0][b_idx] < 0 ? INT32_MIN : ref2frm[0][h->ref_cache[0][b_idx]];
                int ref_q1 = h->ref_cache[1][b_idx] < 0 ? INT32_MIN : ref2frm[1][h->ref_cache[1][b_idx]];
                int ref_p0 = h->ref_cache[0][bn_idx] < 0 ? INT32_MIN : ref2frmn[0][h->ref_cache[0][bn_idx]];
                int ref_p1 = h->ref_cache[1][bn_idx] < 0 ? INT32_MIN : ref2frmn[1][h->ref_cache[1][bn_idx]];

                if ( ((ref_p0 == ref_q0) && (ref_p1 == ref_q1)) || ((ref_p0 == ref_q1) && (ref_p1 == ref_q0)))
                {
                    // L0 and L1 reference pictures of p0 are different; q0 as well
                    if (ref_p0 != ref_p1)
                    {
                        // compare MV for the same reference picture
                        if (ref_p0 == ref_q0)
                        {
                            if (ref_p0 == INT32_MIN)
                            {
                                v =  (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit);
                            }
                            else if (ref_p1 == INT32_MIN)
                            {
                                v =  (FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit);
                            }
                            else
                            {
                                v =  (FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit) |
                                     (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit);
                            }
                        }
                        else
                        {
                            v =  (FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit) |
                                 (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit);
                        }
                    }
                    else
                    {
                        // L0 and L1 reference pictures of p0 are the same; q0 as well
                        v = ((FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit) |
                             (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit))
                            &&
                            ((FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit) |
                             (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit));
                    }
                }
                else
                {
                    v = 1;
                }

#endif
                bS[0] = bS[1] = bS[2] = bS[3] = v;
                mv_done = 1;
            }
            else
            { mv_done = 0; }

            for ( i = 0; i < 4; i++ )
            {
                int x = dir == 0 ? edge : i;
                int y = dir == 0 ? i    : edge;
                int b_idx = 8 + 4 + x + 8 * y;
                int bn_idx = b_idx - (dir ? 8 : 1);

                if ( h->non_zero_count_cache[b_idx] | h->non_zero_count_cache[bn_idx] )
                {
                    bS[i] = 2;
                }
                else if (!mv_done)
                {
                    /* 2010/08/22 16:30:00 liuxw+00139685 */
                    /* ����ԭ�ȵĴ���û����ȫ����Э����ʵ�֣������޸�Ϊ��ȫ����Э����ʵ�� */
#if 0
                    bS[i] = 0;

                    for ( l = 0; l < 1 + (h->slice_type_nos == FF_B_TYPE); l++ )
                    {
                        if ( ref2frm[l][h->ref_cache[l][b_idx]] != ref2frmn[l][h->ref_cache[l][bn_idx]] ||
                             FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[l][bn_idx][0] ) >= 4 ||
                             FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[l][bn_idx][1] ) >= mvy_limit )
                        {
                            bS[i] = 1;
                            break;
                        }
                    }

                    if (h->slice_type_nos == FF_B_TYPE && bS[i])
                    {
                        bS[i] = 0;

                        for ( l = 0; l < 2; l++ )
                        {
                            int ln = 1 - l;

                            if ( ref2frm[l][h->ref_cache[l][b_idx]] != ref2frmn[ln][h->ref_cache[ln][bn_idx]] ||
                                 FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[ln][bn_idx][0] ) >= 4 ||
                                 FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[ln][bn_idx][1] ) >= mvy_limit )
                            {
                                bS[i] = 1;
                                break;
                            }
                        }
                    }

#else

                    int v = 0;
                    int ref_q0 = h->ref_cache[0][b_idx] < 0 ? INT32_MIN : ref2frm[0][h->ref_cache[0][b_idx]];
                    int ref_q1 = h->ref_cache[1][b_idx] < 0 ? INT32_MIN : ref2frm[1][h->ref_cache[1][b_idx]];
                    int ref_p0 = h->ref_cache[0][bn_idx] < 0 ? INT32_MIN : ref2frmn[0][h->ref_cache[0][bn_idx]];
                    int ref_p1 = h->ref_cache[1][bn_idx] < 0 ? INT32_MIN : ref2frmn[1][h->ref_cache[1][bn_idx]];

                    if ( ((ref_p0 == ref_q0) && (ref_p1 == ref_q1)) || ((ref_p0 == ref_q1) && (ref_p1 == ref_q0)))
                    {
                        // L0 and L1 reference pictures of p0 are different; q0 as well
                        if (ref_p0 != ref_p1)
                        {
                            // compare MV for the same reference picture
                            if (ref_p0 == ref_q0)
                            {
                                if (ref_p0 == INT32_MIN)
                                {
                                    v =  (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit);
                                }
                                else if (ref_p1 == INT32_MIN)
                                {
                                    v =  (FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4 ) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit);
                                }
                                else
                                {
                                    v =  (FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit) |
                                         (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit);
                                }
                            }
                            else
                            {
                                v =  (FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit) |
                                     (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit);
                            }
                        }
                        else
                        {
                            // L0 and L1 reference pictures of p0 are the same; q0 as well
                            v = ((FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit) |
                                 (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit))
                                &&
                                ((FFABS( h->mv_cache[0][b_idx][0] - h->mv_cache[1][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[0][b_idx][1] - h->mv_cache[1][bn_idx][1] ) >= mvy_limit) |
                                 (FFABS( h->mv_cache[1][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4) | (FFABS( h->mv_cache[1][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= mvy_limit));
                        }
                    }
                    else
                    {
                        v = 1;
                    }

                    bS[i] = v;
#endif
                }
            }

            if (bS[0] + bS[1] + bS[2] + bS[3] == 0)
            { continue; }
        }

        /* Filter edge */
        // Do not use s->qscale as luma quantizer because it has not the same
        // value in IPCM macroblocks.
        qp = ( s->current_picture.qscale_table[mb_xy] + s->current_picture.qscale_table[mbn_xy] + 1 ) >> 1;
        //tprintf(s->avctx, "filter mb:%d/%d dir:%d edge:%d, QPy:%d, QPc:%d, QPcn:%d\n", mb_x, mb_y, dir, edge, qp, h->chroma_qp, s->current_picture.qscale_table[mbn_xy]);
        tprintf(s->avctx, "filter mb:%d/%d dir:%d edge:%d, QPy:%d ls:%d uvls:%d", mb_x, mb_y, dir, edge, qp, linesize, uvlinesize);
        {
            int i;

            for (i = 0; i < 4; i++) { tprintf(s->avctx, " bS[%d]:%d", i, bS[i]); }

            tprintf(s->avctx, "\n");
        }

        if ( dir == 0 )
        {
            filter_mb_edgev( h, &img_y[4 * edge], linesize, bS, qp );

            if ( (edge & 1) == 0 )
            {
                filter_mb_edgecv( h, &img_cb[2 * edge], uvlinesize, bS, ( h->chroma_qp[0] + get_chroma_qp( h, 0, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                filter_mb_edgecv( h, &img_cr[2 * edge], uvlinesize, bS, ( h->chroma_qp[1] + get_chroma_qp( h, 1, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
            }
        }
        else
        {
            filter_mb_edgeh( h, &img_y[4 * edge * linesize], linesize, bS, qp );

            if ( (edge & 1) == 0 )
            {
                filter_mb_edgech( h, &img_cb[2 * edge * uvlinesize], uvlinesize, bS, ( h->chroma_qp[0] + get_chroma_qp( h, 0, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                filter_mb_edgech( h, &img_cr[2 * edge * uvlinesize], uvlinesize, bS, ( h->chroma_qp[1] + get_chroma_qp( h, 1, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
            }
        }
    }
}

static void filter_mb( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = mb_x + mb_y * s->mb_stride;
    const int mb_type = s->current_picture.mb_type[mb_xy];
    const int mvy_limit = IS_INTERLACED(mb_type) ? 2 : 4;
    int first_vertical_edge_done = 0;
    //    av_unused int dir;

    //for sufficiently low qp, filtering wouldn't do anything
    //this is a conservative estimate: could also check beta_offset and more accurate chroma_qp
    if (!FRAME_MBAFF)
    {
        int qp_thresh = 15 - h->slice_alpha_c0_offset - FFMAX3(0, h->pps.chroma_qp_index_offset[0], h->pps.chroma_qp_index_offset[1]);
        int qp = s->current_picture.qscale_table[mb_xy];

        if (qp <= qp_thresh && (mb_x == 0 || ((qp + s->current_picture.qscale_table[mb_xy - 1] + 1) >> 1) <= qp_thresh)
            && (mb_y == 0 || ((qp + s->current_picture.qscale_table[h->top_mb_xy] + 1) >> 1) <= qp_thresh))
        {
            return;
        }
    }

    // CAVLC 8x8dct requires NNZ values for residual decoding that differ from what the loop filter needs
    if (!h->pps.cabac && h->pps.transform_8x8_mode)
    {
        int top_type, left_type[2];
        top_type     = s->current_picture.mb_type[h->top_mb_xy]    ;
        left_type[0] = s->current_picture.mb_type[h->left_mb_xy[0]];
        left_type[1] = s->current_picture.mb_type[h->left_mb_xy[1]];

        if (IS_8x8DCT(top_type))
        {
            h->non_zero_count_cache[4 + 8 * 0] =
                h->non_zero_count_cache[5 + 8 * 0] = h->cbp_table[h->top_mb_xy] & 4;
            h->non_zero_count_cache[6 + 8 * 0] =
                h->non_zero_count_cache[7 + 8 * 0] = h->cbp_table[h->top_mb_xy] & 8;
        }

        if (IS_8x8DCT(left_type[0]))
        {
            h->non_zero_count_cache[3 + 8 * 1] =
                h->non_zero_count_cache[3 + 8 * 2] = h->cbp_table[h->left_mb_xy[0]] & 2; //FIXME check MBAFF
        }

        if (IS_8x8DCT(left_type[1]))
        {
            h->non_zero_count_cache[3 + 8 * 3] =
                h->non_zero_count_cache[3 + 8 * 4] = h->cbp_table[h->left_mb_xy[1]] & 8; //FIXME check MBAFF
        }

        if (IS_8x8DCT(mb_type))
        {
            h->non_zero_count_cache[scan8[0   ]] = h->non_zero_count_cache[scan8[1   ]] =
                    h->non_zero_count_cache[scan8[2   ]] = h->non_zero_count_cache[scan8[3   ]] = h->cbp & 1;

            h->non_zero_count_cache[scan8[0 + 4]] = h->non_zero_count_cache[scan8[1 + 4]] =
                    h->non_zero_count_cache[scan8[2 + 4]] = h->non_zero_count_cache[scan8[3 + 4]] = h->cbp & 2;

            h->non_zero_count_cache[scan8[0 + 8]] = h->non_zero_count_cache[scan8[1 + 8]] =
                    h->non_zero_count_cache[scan8[2 + 8]] = h->non_zero_count_cache[scan8[3 + 8]] = h->cbp & 4;

            h->non_zero_count_cache[scan8[0 + 12]] = h->non_zero_count_cache[scan8[1 + 12]] =
                        h->non_zero_count_cache[scan8[2 + 12]] = h->non_zero_count_cache[scan8[3 + 12]] = h->cbp & 8;
        }
    }

    if (FRAME_MBAFF
        // left mb is in picture
        && h->slice_table[mb_xy - 1] != 0xFFFF
        // and current and left pair do not have the same interlaced type
        && (IS_INTERLACED(mb_type) != IS_INTERLACED(s->current_picture.mb_type[mb_xy - 1]))
        // and left mb is in the same slice if deblocking_filter == 2
        && (h->deblocking_filter != 2 || h->slice_table[mb_xy - 1] == h->slice_table[mb_xy]))
    {
        /* First vertical edge is different in MBAFF frames
         * There are 8 different bS to compute and 2 different Qp
         */
        const int pair_xy = mb_x + (mb_y&~1) * s->mb_stride;
        const int left_mb_xy[2] = { pair_xy - 1, pair_xy - 1 + s->mb_stride };
        int16_t bS[8];
        int qp[2];
        int bqp[2];
        int rqp[2];
        int mb_qp, mbn0_qp, mbn1_qp;
        int i;
        first_vertical_edge_done = 1;

        if ( IS_INTRA(mb_type) )
        { bS[0] = bS[1] = bS[2] = bS[3] = bS[4] = bS[5] = bS[6] = bS[7] = 4; }
        else
        {
            for ( i = 0; i < 8; i++ )
            {
                int mbn_xy = MB_FIELD ? left_mb_xy[i >> 2] : left_mb_xy[i & 1];

                if ( IS_INTRA( s->current_picture.mb_type[mbn_xy] ) )
                { bS[i] = 4; }
                else if ( h->non_zero_count_cache[12 + 8 * (i >> 1)] != 0 ||
                          ((!h->pps.cabac && IS_8x8DCT(s->current_picture.mb_type[mbn_xy])) ?
                           (h->cbp_table[mbn_xy] & ((MB_FIELD ? (i & 2) : (mb_y & 1)) ? 8 : 2)) :
                               h->non_zero_count[mbn_xy][MB_FIELD ? i & 3 : (i >> 2) + (mb_y & 1) * 2]))
                { bS[i] = 2; }
                else
                { bS[i] = 1; }
            }
        }

        mb_qp = s->current_picture.qscale_table[mb_xy];
        mbn0_qp = s->current_picture.qscale_table[left_mb_xy[0]];
        mbn1_qp = s->current_picture.qscale_table[left_mb_xy[1]];
        qp[0] = ( mb_qp + mbn0_qp + 1 ) >> 1;
        bqp[0] = ( get_chroma_qp( h, 0, mb_qp ) +
                   get_chroma_qp( h, 0, mbn0_qp ) + 1 ) >> 1;
        rqp[0] = ( get_chroma_qp( h, 1, mb_qp ) +
                   get_chroma_qp( h, 1, mbn0_qp ) + 1 ) >> 1;
        qp[1] = ( mb_qp + mbn1_qp + 1 ) >> 1;
        bqp[1] = ( get_chroma_qp( h, 0, mb_qp ) +
                   get_chroma_qp( h, 0, mbn1_qp ) + 1 ) >> 1;
        rqp[1] = ( get_chroma_qp( h, 1, mb_qp ) +
                   get_chroma_qp( h, 1, mbn1_qp ) + 1 ) >> 1;

        /* Filter edge */
        tprintf(s->avctx, "filter mb:%d/%d MBAFF, QPy:%d/%d, QPb:%d/%d QPr:%d/%d ls:%d uvls:%d", mb_x, mb_y, qp[0], qp[1], bqp[0], bqp[1], rqp[0], rqp[1], linesize, uvlinesize);
        {
            int i;

            for (i = 0; i < 8; i++)
            { tprintf(s->avctx, " bS[%d]:%d", i, bS[i]); }

            tprintf(s->avctx, "\n");
        }
        filter_mb_mbaff_edgev ( h, &img_y [0], linesize,   bS, qp );
        filter_mb_mbaff_edgecv( h, &img_cb[0], uvlinesize, bS, bqp );
        filter_mb_mbaff_edgecv( h, &img_cr[0], uvlinesize, bS, rqp );
    }

#if CONFIG_SMALL

    for ( dir = 0; dir < 2; dir++ )
    { filter_mb_dir(h, mb_x, mb_y, img_y, img_cb, img_cr, linesize, uvlinesize, mb_xy, mb_type, mvy_limit, dir ? 0 : first_vertical_edge_done, dir); }

#else
    filter_mb_dir(h, mb_x, mb_y, img_y, img_cb, img_cr, linesize, uvlinesize, mb_xy, mb_type, mvy_limit, first_vertical_edge_done, 0);
    filter_mb_dir(h, mb_x, mb_y, img_y, img_cb, img_cr, linesize, uvlinesize, mb_xy, mb_type, mvy_limit, 0, 1);
#endif
}

static int decode_slice(struct AVCodecContext *avctx, void *arg)
{
    H264Context *h = *(void **)arg;
    MpegEncContext *const s = &h->s;
    const int part_mask = s->partitioned_frame ? (AC_END | AC_ERROR) : 0x7F;

    s->mb_skip_run = -1;

    h->is_complex = FRAME_MBAFF || s->picture_structure != PICT_FRAME || s->codec_id != CODEC_ID_H264 ||
                    (CONFIG_GRAY && (s->flags & CODEC_FLAG_GRAY));

    /* CABAC�в���� */
    if ( h->pps.cabac )
    {
        int i;

        /* realign */
        align_get_bits( &s->gb );

        /* ��ʼ��cabac��״̬�ͽ������ */
        ff_init_cabac_states( &h->cabac);
        ff_init_cabac_decoder( &h->cabac,
                               s->gb.buffer + get_bits_count(&s->gb) / 8,
                               ( s->gb.size_in_bits - get_bits_count(&s->gb) + 7) / 8);

        /* calculate pre-state */
        for ( i = 0; i < 460; i++ )
        {
            int pre;

            if ( h->slice_type_nos == FF_I_TYPE )
            { pre = av_clip( ((cabac_context_init_I[i][0] * s->qscale) >> 4 ) + cabac_context_init_I[i][1], 1, 126 ); }
            else
            { pre = av_clip( ((cabac_context_init_PB[h->cabac_init_idc][i][0] * s->qscale) >> 4 ) + cabac_context_init_PB[h->cabac_init_idc][i][1], 1, 126 ); }

            if ( pre <= 63 )
            { h->cabac_state[i] = 2 * ( 63 - pre ) + 0; }
            else
            { h->cabac_state[i] = 2 * ( pre - 64 ) + 1; }
        }

        for (;;)
        {
            //START_TIMER
            int ret = 0;
            int eos;
            int iRet = 0;
            //STOP_TIMER("decode_mb_cabac")
#ifdef LXW_DEBUG

            /* add by lxw for test */
            if (frame_num == TARGET_FRAME   && s->mb_x == MB_X && s->mb_y == MB_Y)
            {
                av_log(NULL, AV_LOG_ERROR, "s->mb_x:%d s->mb_y:%d begin debug!\n", s->mb_x, s->mb_y);
            }

#endif

            /* 2010/04/08 14:30:00 liuxw+00139685 */
            /* ���ӶԺ��λ�úϷ��Եķ�Χ������������򶪰������������(avoid segment) */
            if (h->s.mb_x < 0 || h->s.mb_x >= s->mb_stride || h->s.mb_y < 0 || h->s.mb_y >= s->mb_height)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "mb_x or mb_y is invalid!\n");
                IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_OTHER);
                h->s.avctx->iTotalError++;
                return -1;
            }

            ret = decode_mb_cabac(h); /* ģʽԤ�⡢����в������ */

            /* add by lxw for test */
#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "frame num:%d mb_x: %d mb_y:%d ref0:%d mv_x:%d mv_y:%d ref1:%d mv_x:%d mv_y:%d!\n",
                       frame_num, h->s.mb_x, h->s.mb_y, h->ref_cache[0][0], h->mv_cache[0][0][0], h->mv_cache[0][0][1] , h->ref_cache[1][0], h->mv_cache[0][1][0], h->mv_cache[0][1][1]);
            }

#endif

            if (ret >= 0) { iRet = hl_decode_mb(h); } /* IDCT , ֡��Ԥ�⡢֡��Ԥ�⡢�ؽ�����·�˲� */

            if (iRet < 0) { return -1; }

            /* ��MBAFF���ܴ򿪣���һ��Ҫ��һ������ */
            if ( ret >= 0 && FRAME_MBAFF )  //FIXME optimal? or let mb_decode decode 16x32 ?
            {
                s->mb_y++;

                /* 2010/06/01 18:30:00 liuxw+00139685 */
                /* ���ӶԺ��λ�úϷ��Եķ�Χ������������򶪰������������(avoid segment) */
                if (h->s.mb_x < 0 || h->s.mb_x >= s->mb_stride || h->s.mb_y < 0 || h->s.mb_y >= s->mb_height)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "mb_x[%d] or mb_y[%d] is invalid!\n", h->s.mb_x, h->s.mb_y);
                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_OTHER);
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                ret = decode_mb_cabac(h);

                if (ret >= 0) { iRet = hl_decode_mb(h); }

                if (iRet < 0) { return -1; }

                s->mb_y--;
            }

            eos = get_cabac_terminate( &h->cabac );	/* ����slice end ��־ */

            if ( ret < 0 || h->cabac.bytestream > h->cabac.bytestream_end + 2)
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding MB %d %d, bytestream (%td)\n", s->mb_x, s->mb_y, h->cabac.bytestream_end - h->cabac.bytestream);
                av_log(h->s.avctx, AV_LOG_WARNING, "error while decoding MB %d %d, bytestream (%td)\n", s->mb_x, s->mb_y, h->cabac.bytestream_end - h->cabac.bytestream);
                IMEDIA_SET_ERR_RESIDUAL(h->s.avctx->iErrorCode, IMEDIA_ERR_RESIDUAL_OTHER);
                h->s.avctx->iTotalError++;
                ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_ERROR | DC_ERROR | MV_ERROR)&part_mask);
                return -1;
            }

            if ( ++s->mb_x >= s->mb_width )
            {
                s->mb_x = 0;
                ff_draw_horiz_band(s, 16 * s->mb_y, 16);
                ++s->mb_y;

                if (FIELD_OR_MBAFF_PICTURE)
                {
                    ++s->mb_y;
                }
            }

            if ( eos || s->mb_y >= s->mb_height )
            {
                tprintf(s->avctx, "slice end %d %d\n", get_bits_count(&s->gb), s->gb.size_in_bits);
                ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x - 1, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);
                return 0;
            }
        }

    }
    else     /* CAVLC�в���� */
    {
        for (;;)
        {
            int ret = 0;
            int iRet = 0;

#ifdef LXW_DEBUG

            /* add by lxw for test */
            if (frame_num == TARGET_FRAME  && s->mb_x == MB_X && (s->mb_y == MB_Y || s->mb_y == MB_Y2))
            {
                av_log(NULL, AV_LOG_ERROR, "s->mb_x:%d s->mb_y:%d begin debug!\n", s->mb_x, s->mb_y);
            }

#endif

            /* 2010/04/08 14:30:00 liuxw+00139685 */
            /* ���ӶԺ��λ�úϷ��Եķ�Χ������������򶪰������������(avoid segment) */
            if (h->s.mb_x < 0 || h->s.mb_x >= s->mb_stride || h->s.mb_y < 0 || h->s.mb_y >= s->mb_height)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "mb_x or mb_y is invalid!\n");
                IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_OTHER);
                h->s.avctx->iTotalError++;
                return -1;
            }

            ret = decode_mb_cavlc(h);	/* ģʽԤ�⡢����в������ */

#ifdef LXW_DEBUG

            if (frame_num == TARGET_FRAME && h->s.mb_x == MB_X && h->s.mb_y == MB_Y)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "frame num:%d mb_x: %d mb_y:%d ref0:%d mv_x:%d mv_y:%d ref1:%d mv_x:%d mv_y:%d!\n",
                       frame_num, h->s.mb_x, h->s.mb_y, h->ref_cache[0][0], h->mv_cache[0][0][0], h->mv_cache[0][0][1] , h->ref_cache[1][0], h->mv_cache[0][1][0], h->mv_cache[0][1][1]);
            }

#endif

            if (ret >= 0) { iRet = hl_decode_mb(h); }		/* IDCT , ֡��Ԥ�⡢֡��Ԥ�⡢�ؽ�����·�˲� */

            if (iRet < 0) { return -1; }

            /* ��MBAFF���ܴ򿪣���һ��Ҫ��һ������ */
            if (ret >= 0 && FRAME_MBAFF) //FIXME optimal? or let mb_decode decode 16x32 ?
            {
                s->mb_y++;

                /* 2010/06/01 18:30:00 liuxw+00139685 */
                /* ���ӶԺ��λ�úϷ��Եķ�Χ������������򶪰������������(avoid segment) */
                if (h->s.mb_x < 0 || h->s.mb_x >= s->mb_stride || h->s.mb_y < 0 || h->s.mb_y >= s->mb_height)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "mb_x[%d] or mb_y[%d] is invalid!\n", h->s.mb_x, h->s.mb_y);
                    IMEDIA_SET_ERR_MB(h->s.avctx->iErrorCode, IMEDIA_ERR_MB_OTHER);
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                ret = decode_mb_cavlc(h);

                if (ret >= 0) { iRet = hl_decode_mb(h); }

                if (iRet < 0) { return -1; }

                s->mb_y--;
            }

            if (ret < 0)
            {
                //              av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding MB %d %d\n", s->mb_x, s->mb_y);
                av_log(h->s.avctx, AV_LOG_WARNING, "error while decoding MB %d %d\n", s->mb_x, s->mb_y);
                IMEDIA_SET_ERR_RESIDUAL(h->s.avctx->iErrorCode, IMEDIA_ERR_RESIDUAL_OTHER);
                h->s.avctx->iTotalError++;
                ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_ERROR | DC_ERROR | MV_ERROR)&part_mask);

                return -1;
            }

            if (++s->mb_x >= s->mb_width)
            {
                s->mb_x = 0;
                ff_draw_horiz_band(s, 16 * s->mb_y, 16);
                ++s->mb_y;

                if (FIELD_OR_MBAFF_PICTURE)
                {
                    ++s->mb_y;
                }

                if (s->mb_y >= s->mb_height)
                {
                    tprintf(s->avctx, "slice end %d %d\n", get_bits_count(&s->gb), s->gb.size_in_bits);

                    if (get_bits_count(&s->gb) == s->gb.size_in_bits )
                    {
                        ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x - 1, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);

                        return 0;
                    }
                    else
                    {
                        ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);

                        return -1;
                    }
                }
            }

            if (get_bits_count(&s->gb) >= s->gb.size_in_bits && s->mb_skip_run <= 0)
            {
                tprintf(s->avctx, "slice end %d %d\n", get_bits_count(&s->gb), s->gb.size_in_bits);

                if (get_bits_count(&s->gb) == s->gb.size_in_bits )
                {
                    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x - 1, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);

                    return 0;
                }
                else
                {
                    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_ERROR | DC_ERROR | MV_ERROR)&part_mask);

                    return -1;
                }
            }
        }
    }

#if 0

    for (; s->mb_y < s->mb_height; s->mb_y++)
    {
        for (; s->mb_x < s->mb_width; s->mb_x++)
        {
            int ret = decode_mb(h);

            hl_decode_mb(h);

            if (ret < 0)
            {
                av_log(s->avctx, AV_LOG_ERROR, "error while decoding MB %d %d\n", s->mb_x, s->mb_y);
                ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_ERROR | DC_ERROR | MV_ERROR)&part_mask);

                return -1;
            }

            if (++s->mb_x >= s->mb_width)
            {
                s->mb_x = 0;

                if (++s->mb_y >= s->mb_height)
                {
                    if (get_bits_count(s->gb) == s->gb.size_in_bits)
                    {
                        ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x - 1, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);

                        return 0;
                    }
                    else
                    {
                        ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);

                        return -1;
                    }
                }
            }

            if (get_bits_count(s-> ? gb) >= s->gb?.size_in_bits)
            {
                if (get_bits_count(s->gb) == s->gb.size_in_bits)
                {
                    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x - 1, s->mb_y, (AC_END | DC_END | MV_END)&part_mask);

                    return 0;
                }
                else
                {
                    ff_er_add_slice(s, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y, (AC_ERROR | DC_ERROR | MV_ERROR)&part_mask);

                    return -1;
                }
            }
        }

        s->mb_x = 0;
        ff_draw_horiz_band(s, 16 * s->mb_y, 16);
    }

#endif
    return -1; //not reached
}

static int decode_picture_timing(H264Context *h)
{
    MpegEncContext *const s = &h->s;

    if (h->sps.nal_hrd_parameters_present_flag || h->sps.vcl_hrd_parameters_present_flag)
    {
        h->sei_cpb_removal_delay = get_bits(&s->gb, h->sps.cpb_removal_delay_length);
        h->sei_dpb_output_delay = get_bits(&s->gb, h->sps.dpb_output_delay_length);
    }

    if (h->sps.pic_struct_present_flag)
    {
        unsigned int i, num_clock_ts;
        h->sei_pic_struct = get_bits(&s->gb, 4);

        if (h->sei_pic_struct > SEI_PIC_STRUCT_FRAME_TRIPLING)
        { return -1; }

        num_clock_ts = sei_num_clock_ts_table[h->sei_pic_struct];

        for (i = 0 ; i < num_clock_ts ; i++)
        {
            if (get_bits(&s->gb, 1))                  /* clock_timestamp_flag */
            {
                unsigned int full_timestamp_flag;
                skip_bits(&s->gb, 2);                 /* ct_type */
                skip_bits(&s->gb, 1);                 /* nuit_field_based_flag */
                skip_bits(&s->gb, 5);                 /* counting_type */
                full_timestamp_flag = get_bits(&s->gb, 1);
                skip_bits(&s->gb, 1);                 /* discontinuity_flag */
                skip_bits(&s->gb, 1);                 /* cnt_dropped_flag */
                skip_bits(&s->gb, 8);                 /* n_frames */

                if (full_timestamp_flag)
                {
                    skip_bits(&s->gb, 6);             /* seconds_value 0..59 */
                    skip_bits(&s->gb, 6);             /* minutes_value 0..59 */
                    skip_bits(&s->gb, 5);             /* hours_value 0..23 */
                }
                else
                {
                    if (get_bits(&s->gb, 1))          /* seconds_flag */
                    {
                        skip_bits(&s->gb, 6);         /* seconds_value range 0..59 */

                        if (get_bits(&s->gb, 1))      /* minutes_flag */
                        {
                            skip_bits(&s->gb, 6);     /* minutes_value 0..59 */

                            if (get_bits(&s->gb, 1))  /* hours_flag */
                            { skip_bits(&s->gb, 5); } /* hours_value 0..23 */
                        }
                    }
                }

                if (h->sps.time_offset_length > 0)
                { skip_bits(&s->gb, h->sps.time_offset_length); } /* time_offset */
            }
        }
    }

    return 0;
}

static int decode_unregistered_user_data(H264Context *h, int size)
{
    MpegEncContext *const s = &h->s;
    uint8_t user_data[16 + 256];
    int e, build, i;

    if (size < 16)
    { return -1; }

    for (i = 0; i < sizeof(user_data) - 1 && i < size; i++)
    {
        user_data[i] = get_bits(&s->gb, 8);
    }

    user_data[i] = 0;
    e = sscanf(user_data + 16, "x264 - core %d"/*%s - H.264/MPEG-4 AVC codec - Copyleft 2005 - http://www.videolan.org/x264.html*/, &build);

    if (e == 1 && build >= 0)
    { h->x264_build = build; }

    if (s->avctx->debug & FF_DEBUG_BUGS)
    { av_log(s->avctx, AV_LOG_DEBUG, "user data:\"%s\"\n", user_data + 16); }

    for (; i < size; i++)
    { skip_bits(&s->gb, 8); }

    return 0;
}

static int decode_recovery_point(H264Context *h)
{
    MpegEncContext *const s = &h->s;

    h->sei_recovery_frame_cnt = get_ue_golomb(&s->gb);
    skip_bits(&s->gb, 4);       /* 1b exact_match_flag, 1b broken_link_flag, 2b changing_slice_group_idc */

    return 0;
}

static int decode_buffering_period(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    unsigned int sps_id;
    int sched_sel_idx;
    SPS *sps;

    sps_id = get_ue_golomb_31(&s->gb);

    if (sps_id > 31 || !h->sps_buffers[sps_id])
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "non-existing SPS %d referenced in buffering period\n", sps_id);
        av_log(h->s.avctx, AV_LOG_WARNING, "non-existing SPS %d referenced in buffering period\n", sps_id);
        return -1;
    }

    sps = h->sps_buffers[sps_id];

    // NOTE: This is really so duplicated in the standard... See H.264, D.1.1
    if (sps->nal_hrd_parameters_present_flag)
    {
        for (sched_sel_idx = 0; sched_sel_idx < sps->cpb_cnt; sched_sel_idx++)
        {
            h->initial_cpb_removal_delay[sched_sel_idx] = get_bits(&s->gb, sps->initial_cpb_removal_delay_length);
            skip_bits(&s->gb, sps->initial_cpb_removal_delay_length); // initial_cpb_removal_delay_offset
        }
    }

    if (sps->vcl_hrd_parameters_present_flag)
    {
        for (sched_sel_idx = 0; sched_sel_idx < sps->cpb_cnt; sched_sel_idx++)
        {
            h->initial_cpb_removal_delay[sched_sel_idx] = get_bits(&s->gb, sps->initial_cpb_removal_delay_length);
            skip_bits(&s->gb, sps->initial_cpb_removal_delay_length); // initial_cpb_removal_delay_offset
        }
    }

    h->sei_buffering_period_present = 1;
    return 0;
}

int ff_h264_decode_sei(H264Context *h)
{
    MpegEncContext *const s = &h->s;

    while (get_bits_count(&s->gb) + 16 < s->gb.size_in_bits)
    {
        int size, type;

        type = 0;

        do
        {
            type += show_bits(&s->gb, 8);
        }
        while (get_bits(&s->gb, 8) == 255);

        size = 0;

        do
        {
            size += show_bits(&s->gb, 8);
        }
        while (get_bits(&s->gb, 8) == 255);

        switch (type)
        {
            case SEI_TYPE_PIC_TIMING: // Picture timing SEI
                if (decode_picture_timing(h) < 0)
                {
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                break;

            case SEI_TYPE_USER_DATA_UNREGISTERED:
                if (decode_unregistered_user_data(h, size) < 0)
                {
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                break;

            case SEI_TYPE_RECOVERY_POINT:
                if (decode_recovery_point(h) < 0)
                {
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                break;

            case SEI_BUFFERING_PERIOD:
                if (decode_buffering_period(h) < 0)
                {
                    h->s.avctx->iTotalError++;
                    return -1;
                }

                break;

            default:
                skip_bits(&s->gb, 8 * size);
        }

        //FIXME check bits here
        align_get_bits(&s->gb);
    }

    return 0;
}

static inline int decode_hrd_parameters(H264Context *h, SPS *sps)
{
    MpegEncContext *const s = &h->s;
    int cpb_count, i;
    cpb_count = get_ue_golomb_31(&s->gb) + 1;

    if (cpb_count > 32U)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "cpb_count %d invalid\n", cpb_count);
        if (NULL != h->s.avctx)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "cpb_count %d invalid\n", cpb_count);
        }

        return -1;
    }

    get_bits(&s->gb, 4); /* bit_rate_scale */
    get_bits(&s->gb, 4); /* cpb_size_scale */

    for (i = 0; i < cpb_count; i++)
    {
        get_ue_golomb(&s->gb); /* bit_rate_value_minus1 */
        get_ue_golomb(&s->gb); /* cpb_size_value_minus1 */
        get_bits1(&s->gb);     /* cbr_flag */
    }

    sps->initial_cpb_removal_delay_length = get_bits(&s->gb, 5) + 1;
    sps->cpb_removal_delay_length = get_bits(&s->gb, 5) + 1;
    sps->dpb_output_delay_length = get_bits(&s->gb, 5) + 1;
    sps->time_offset_length = get_bits(&s->gb, 5);
    sps->cpb_cnt = cpb_count;
    return 0;
}

/*static inline */ int decode_vui_parameters(H264Context *h, SPS *sps)
{
    MpegEncContext *const s = &h->s;
    int aspect_ratio_info_present_flag;
    unsigned int aspect_ratio_idc;

    aspect_ratio_info_present_flag = get_bits1(&s->gb);

    if ( aspect_ratio_info_present_flag )
    {
        aspect_ratio_idc = get_bits(&s->gb, 8);

        if ( aspect_ratio_idc == EXTENDED_SAR )
        {
            sps->sar.num = get_bits(&s->gb, 16);
            sps->sar.den = get_bits(&s->gb, 16);
        }
        else if (aspect_ratio_idc < FF_ARRAY_ELEMS(pixel_aspect) && aspect_ratio_idc > 0)
        {
            sps->sar =  pixel_aspect[aspect_ratio_idc];
        }
        else
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "illegal aspect ratio\n");
            av_log(h->s.avctx, AV_LOG_WARNING, "illegal aspect ratio_idc[%d]\n", aspect_ratio_idc);

            if (NULL != h->s.avctx)
            {
                IMEDIA_SET_ERR_SEQ(h->s.avctx->iErrorCode, IMEDIA_ERR_SEQ_OTHER);
                h->s.avctx->iTotalError++;
            }

            sps->sar.num = sps->sar.den = 1;
            return -1;
        }
    }
    else
    {
        sps->sar.num = sps->sar.den = 1;
    }

    //  s->avctx->aspect_ratio= sar_width*s->width / (float)(s->height*sar_height);
    /* 2010/07/23 15:30:00 liuxw+00139685 */
    /* ��avctx��sample_aspect_ratio���и�ֵ */
    if ( NULL != s->avctx)
    {
        s->avctx->sample_aspect_ratio.num = sps->sar.num;
        s->avctx->sample_aspect_ratio.den = sps->sar.den;
    }

    if (get_bits1(&s->gb))
    {
        /* overscan_info_present_flag */
        get_bits1(&s->gb);      /* overscan_appropriate_flag */
    }

    if (get_bits1(&s->gb)) /* video_signal_type_present_flag */
    {
        get_bits(&s->gb, 3);    /* video_format */
        get_bits1(&s->gb);      /* video_full_range_flag */

        if (get_bits1(&s->gb))
        {
            /* colour_description_present_flag */
            get_bits(&s->gb, 8); /* colour_primaries */
            get_bits(&s->gb, 8); /* transfer_characteristics */
            get_bits(&s->gb, 8); /* matrix_coefficients */
        }
    }

    if (get_bits1(&s->gb))
    {
        /* chroma_location_info_present_flag */
        get_ue_golomb(&s->gb);  /* chroma_sample_location_type_top_field */
        get_ue_golomb(&s->gb);  /* chroma_sample_location_type_bottom_field */
    }

    sps->timing_info_present_flag = get_bits1(&s->gb);

    if (sps->timing_info_present_flag)
    {
        sps->num_units_in_tick = get_bits_long(&s->gb, 32);
        sps->time_scale = get_bits_long(&s->gb, 32);
        sps->fixed_frame_rate_flag = get_bits1(&s->gb);
    }

    sps->nal_hrd_parameters_present_flag = get_bits1(&s->gb);

    if (sps->nal_hrd_parameters_present_flag)
        if (decode_hrd_parameters(h, sps) < 0)
        { return -1; }

    sps->vcl_hrd_parameters_present_flag = get_bits1(&s->gb);

    if (sps->vcl_hrd_parameters_present_flag)
        if (decode_hrd_parameters(h, sps) < 0)
        { return -1; }

    if (sps->nal_hrd_parameters_present_flag || sps->vcl_hrd_parameters_present_flag)
    { get_bits1(&s->gb); }     /* low_delay_hrd_flag */

    sps->pic_struct_present_flag = get_bits1(&s->gb);

    sps->bitstream_restriction_flag = get_bits1(&s->gb);

    if (sps->bitstream_restriction_flag)
    {
        get_bits1(&s->gb);     /* motion_vectors_over_pic_boundaries_flag */
        get_ue_golomb(&s->gb); /* max_bytes_per_pic_denom */
        get_ue_golomb(&s->gb); /* max_bits_per_mb_denom */
        get_ue_golomb(&s->gb); /* log2_max_mv_length_horizontal */
        get_ue_golomb(&s->gb); /* log2_max_mv_length_vertical */
        sps->num_reorder_frames = get_ue_golomb(&s->gb);
        get_ue_golomb(&s->gb); /*max_dec_frame_buffering*/

        if (sps->num_reorder_frames > 16U /*max_dec_frame_buffering || max_dec_frame_buffering > 16*/)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "illegal num_reorder_frames %d\n", sps->num_reorder_frames);
            if (NULL != h->s.avctx)
            {
                av_log(h->s.avctx, AV_LOG_WARNING, "illegal num_reorder_frames %d\n", sps->num_reorder_frames);
                IMEDIA_SET_ERR_SEQ(h->s.avctx->iErrorCode, IMEDIA_ERR_SEQ_OTHER);
                h->s.avctx->iTotalError++;
            }

            /* 2010/05/31 10:00:00 liuxw+00139685 */
            /* ��num_reorder_frames������16���򱨴�������bitstream_restriction_flag��num_reorder_frames��Ϊ0  */
            sps->num_reorder_frames = 0;
            sps->bitstream_restriction_flag = 0;
            return -1;
        }
    }

    return 0;
}

static void decode_scaling_list(H264Context *h, uint8_t *factors, int size,
                                const uint8_t *jvt_list, const uint8_t *fallback_list)
{
    MpegEncContext *const s = &h->s;
    int i, last = 8, next = 8;
    const uint8_t *scan = size == 16 ? zigzag_scan : ff_zigzag_direct;

    if (!get_bits1(&s->gb)) /* matrix not written, we use the predicted one */
    { memcpy(factors, fallback_list, size * sizeof(uint8_t)); }
    else
        for (i = 0; i < size; i++)
        {
            if (next)
            { next = (last + get_se_golomb(&s->gb)) & 0xff; }

            if (!i && !next) /* matrix not written, we use the preset one */
            {
                memcpy(factors, jvt_list, size * sizeof(uint8_t));
                break;
            }

            last = factors[scan[i]] = next ? next : last;
        }
}

/*static*/ void decode_scaling_matrices(H264Context *h, SPS *sps, PPS *pps, int is_sps,
                                        uint8_t (*scaling_matrix4)[16], uint8_t (*scaling_matrix8)[64])
{
    MpegEncContext *const s = &h->s;
    int fallback_sps = !is_sps && sps->scaling_matrix_present;
    const uint8_t *fallback[4] =
    {
        fallback_sps ? sps->scaling_matrix4[0] : default_scaling4[0],
        fallback_sps ? sps->scaling_matrix4[3] : default_scaling4[1],
        fallback_sps ? sps->scaling_matrix8[0] : default_scaling8[0],
        fallback_sps ? sps->scaling_matrix8[1] : default_scaling8[1]
    };

    if (get_bits1(&s->gb))
    {
        sps->scaling_matrix_present |= is_sps;
        decode_scaling_list(h, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]); // Intra, Y
        decode_scaling_list(h, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
        decode_scaling_list(h, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
        decode_scaling_list(h, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]); // Inter, Y
        decode_scaling_list(h, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
        decode_scaling_list(h, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb

        if (is_sps || pps->transform_8x8_mode)
        {
            decode_scaling_list(h, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]); // Intra, Y
            decode_scaling_list(h, scaling_matrix8[1], 64, default_scaling8[1], fallback[3]); // Inter, Y
        }
    }
}

/* 2010/05/5 15:00:00 liuxw+00139685 */
/* ������������鵱ǰ������SPS����ǰsps_id��ͬ��SPS���������ؼ��﷨Ԫ�ز�ͬ���򷵻�-1�����򷵻�0 */
int sps_check(SPS *cur_sps, SPS *old_sps)
{

    if (cur_sps->profile_idc != old_sps->profile_idc)
    {
        return -1;
    }

    if (cur_sps->mb_width != old_sps->mb_width)
    {
        return -1;
    }

    if (cur_sps->mb_height != old_sps->mb_height)
    {
        return -1;
    }

    return 0;
}

/* 2010/05/5 15:00:00 liuxw+00139685 */
/* ����������Ѱ��һ���Ѿ�������SPS�������������� */
int find_sps(H264Context *h, int *index)
{
    int i = 0;

    for (i = 0; i < MAX_SPS_COUNT; i++)
    {
        if (h->sps_buffers[i])
        {
            *index = i;
            return 0;
        }
    }

    return -1;
}


int ff_h264_decode_seq_parameter_set(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    int profile_idc, level_idc;
    unsigned int sps_id;
    int i;
    SPS *sps;
    int warn_cnt = 0;
    int width, height;

    /* 2010/30/04 9:07:00 liuxw+00139685 */
    /* ����һ��ָ�����飬�� �ڴ�ӡͼ����Ϣ */
    const char *apszColorFmt[4] = {"Gray", "420", "422", "444"};

    profile_idc = get_bits(&s->gb, 8);
    get_bits1(&s->gb);   //constraint_set0_flag
    get_bits1(&s->gb);   //constraint_set1_flag
    get_bits1(&s->gb);   //constraint_set2_flag
    get_bits1(&s->gb);   //constraint_set3_flag
    get_bits(&s->gb, 4); // reserved

    /* 2010/04/06 10:07:00 liuxw+00139685 */
    /* ���Ӷ�profile_idc�ĺϷ����ж� */
    if (!(66 == profile_idc || 77 == profile_idc || 100 == profile_idc))
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "Profile[%d] not supported currently\n", profile_idc);
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_PROFILE_LEVEL);
        s->avctx->iTotalError++;
        return -1;
    }

    level_idc = get_bits(&s->gb, 8);

    switch (level_idc)
    {
        case 10:
        case 11:
        case 12:
        case 13:
        case 20:
        case 21:
        case 22:
        case 30:
        case 31:
        case 32:
        case 40:
        case 41:
        case 42:
        case 50:
        case 51:
            break;

        default:
            av_log(h->s.avctx, AV_LOG_WARNING, "Level[%d] is invalid!\n", level_idc);
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_PROFILE_LEVEL);
            s->avctx->iTotalError++;
            warn_cnt++;
            //			return -1;
    }

    sps_id = get_ue_golomb_31(&s->gb);

    if (sps_id >= MAX_SPS_COUNT)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "sps_id (%d) out of range\n", sps_id);
        av_log(h->s.avctx, AV_LOG_WARNING, "sps_id (%d) out of range\n", sps_id);
        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_ID);
        s->avctx->iTotalError++;
        sps_id = 0;
        warn_cnt++;
        //      return -1;
    }

    //  sps= av_mallocz(sizeof(SPS));
    sps = av_malloc_hw(s->avctx, sizeof(SPS));

    if (sps == NULL)
    {
        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ���Ӵ�����־��Ϣ��ӡ */
        av_log(h->s.avctx, AV_LOG_ERROR, "Malloc memory for sps is failed!\n");
        h->s.avctx->iMallocFailed = 1;
        return -1;
    }

    memset(sps, 0, sizeof(SPS));

    sps->profile_idc = profile_idc;
    sps->level_idc = level_idc;

    memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
    memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
    sps->scaling_matrix_present = 0;

    if (sps->profile_idc >= 100) //high profile
    {
        sps->chroma_format_idc = get_ue_golomb_31(&s->gb);

        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ����chroma_format_idc�Ϸ����ж� */
        if (sps->chroma_format_idc != 1)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "chroma_format_idc[%d] is out of range or not supported currently!\n", sps->chroma_format_idc);
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_COLOR_SPACE);
            s->avctx->iTotalError++;
            goto fail;
        }

        if (sps->chroma_format_idc == 3)
        { sps->residual_color_transform_flag = get_bits1(&s->gb); }

        sps->bit_depth_luma   = get_ue_golomb(&s->gb) + 8;
        sps->bit_depth_chroma = get_ue_golomb(&s->gb) + 8;

        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ����bit_depth_luma/bit_depth_chroma�Ϸ����ж� */
        if (sps->bit_depth_luma != 8 || sps->bit_depth_chroma != 8)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "bit_depth_luma[%d]/bit_depth_chroma[%d] is out of range or not supported currently!\n", sps->bit_depth_luma, sps->bit_depth_chroma);
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_BIT_DEPTH);
            s->avctx->iTotalError++;
            sps->bit_depth_luma = sps->bit_depth_chroma = 0;
            warn_cnt++;
            //			goto fail;
        }

        sps->transform_bypass = get_bits1(&s->gb);
        decode_scaling_matrices(h, sps, NULL, 1, sps->scaling_matrix4, sps->scaling_matrix8);
    }
    else
    {
        sps->chroma_format_idc = 1;
    }

    sps->log2_max_frame_num = get_ue_golomb(&s->gb) + 4;

    /* 2010/03/22 17:07:00 liuxw+00139685 */
    /* ����log2_max_frame_num�Ϸ����ж� */
    if (sps->log2_max_frame_num > 16)
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "log2_max_frame_num[%d] is out of range!!\n", sps->log2_max_frame_num);
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_FRAME_NUM);
        s->avctx->iTotalError++;
        sps->log2_max_frame_num = 4;
        warn_cnt++;
        //		goto fail;
    }

    sps->poc_type = get_ue_golomb_31(&s->gb);

    if (sps->poc_type == 0)
    {
        //FIXME #define
        sps->log2_max_poc_lsb = get_ue_golomb(&s->gb) + 4;

        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ����log2_max_poc_lsb�Ϸ����ж� */
        if (sps->log2_max_poc_lsb > 16)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "log2_max_poc_lsb[%d] is out of range!!\n", sps->log2_max_poc_lsb);
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_ID);
            s->avctx->iTotalError++;
            sps->log2_max_poc_lsb = 4;
            warn_cnt++;
            //			goto fail;
        }
    }
    else if (sps->poc_type == 1)
    {
        //FIXME #define
        sps->delta_pic_order_always_zero_flag = get_bits1(&s->gb);
        sps->offset_for_non_ref_pic = get_se_golomb(&s->gb);
        sps->offset_for_top_to_bottom_field = get_se_golomb(&s->gb);
        sps->poc_cycle_length                = get_ue_golomb(&s->gb);

        if ((unsigned)sps->poc_cycle_length >= FF_ARRAY_ELEMS(sps->offset_for_ref_frame))
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "poc_cycle_length[%d] overflow[%d]\n", sps->poc_cycle_length,FF_ARRAY_ELEMS(sps->offset_for_ref_frame));
            av_log(h->s.avctx, AV_LOG_WARNING, "poc_cycle_length[%d] overflow[%d]\n", sps->poc_cycle_length, FF_ARRAY_ELEMS(sps->offset_for_ref_frame));
            /* 2010/03/22 17:07:00 liuxw+00139685 */
            /* ���Ӵ����� */
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_ID);
            s->avctx->iTotalError++;
            sps->poc_cycle_length = FF_ARRAY_ELEMS(sps->offset_for_ref_frame) - 1;
            warn_cnt++;
            //          goto fail;
        }

        for (i = 0; i < sps->poc_cycle_length; i++)
        { sps->offset_for_ref_frame[i] = get_se_golomb(&s->gb); }
    }
    else if (sps->poc_type != 2)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "Poc type[%d] is invalid!\n", sps->poc_type);
        av_log(h->s.avctx, AV_LOG_WARNING, "Poc type[%d] is invalid!\n", sps->poc_type);
        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ���Ӵ��󷵻��� */
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_ID);
        s->avctx->iTotalError++;
        sps->poc_type = 0;
        sps->log2_max_poc_lsb = 4;
        warn_cnt++;
        //      goto fail;
    }

    sps->ref_frame_count = get_ue_golomb_31(&s->gb);

    /* 2010/04/06 10:46:00 liuxw+00139685 */
    /* �޸��ж����� */
    //  if(sps->ref_frame_count > MAX_PICTURE_COUNT-2 || sps->ref_frame_count >= 32U){
    if (sps->ref_frame_count > 16)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "too many reference frames\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "too many reference frames\n");
        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_FRAME_NUM);
        s->avctx->iTotalError++;
        sps->ref_frame_count = 1;
        warn_cnt++;
        //      goto fail;
    }

    sps->gaps_in_frame_num_allowed_flag = get_bits1(&s->gb);
    sps->mb_width = get_ue_golomb(&s->gb) + 1;
    sps->mb_height = get_ue_golomb(&s->gb) + 1;

    sps->frame_mbs_only_flag = get_bits1(&s->gb);

    if (!sps->frame_mbs_only_flag)
    { sps->mb_aff = get_bits1(&s->gb); }
    else
    { sps->mb_aff = 0; }

    /* 2010/03/22 17:07:00 liuxw+00139685 */
    /* ���Ӷ�mb_width��mb_height�Ϸ����ж� */
    /* 2010/06/03 10:00:00 liuxw+00139685 */
    /* ��frame_mb_only_flagҲ���ǵ�mb_height��ȥ */
    if (sps->mb_width > MAX_PIC_WIDTH / MB_WIDTH || (sps->mb_height << (1 - sps->frame_mbs_only_flag)) > MAX_PIC_HEIGHT / MB_HEIGHT || (sps->mb_height << (1 - sps->frame_mbs_only_flag)) < MIN_PIC_HEIGHT / MB_HEIGHT
        || sps->mb_width < MIN_PIC_WIDTH / MB_WIDTH || (unsigned)sps->mb_width >= INT_MAX / MB_WIDTH || (unsigned)sps->mb_height >= INT_MAX / MB_HEIGHT || avcodec_check_dimensions(NULL, 16 * sps->mb_width, 16 * sps->mb_height))
    {
        int index;
        //     av_log(h->s.avctx, AV_LOG_ERROR, "mb_width[%d]/mb_height[%d] overflow!\n",sps->mb_width,sps->mb_height);
        av_log(h->s.avctx, AV_LOG_WARNING, "mb_width[%d]/mb_height[%d] overflow[%d,%d]!\n", sps->mb_width, sps->mb_height, MAX_PIC_WIDTH / MB_WIDTH, MAX_PIC_HEIGHT / MB_HEIGHT);
        /* 2010/03/22 17:07:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_SIZE);
        s->avctx->iTotalError++;
        warn_cnt++;

        if (find_sps(h, &index) < 0)
        {
            goto fail;
        }

        sps->mb_width  = h->sps_buffers[index]->mb_width;
        sps->mb_height = h->sps_buffers[index]->mb_height;
        sps->frame_mbs_only_flag = h->sps_buffers[index]->frame_mbs_only_flag;
        //          goto fail;
    }

    sps->direct_8x8_inference_flag = get_bits1(&s->gb);

    /* 2010/04/06 11:07:00 liuxw+00139685 */
    /* ���Ӵ����� */
    if (!sps->frame_mbs_only_flag && !sps->direct_8x8_inference_flag)
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "direct_8x8_inference_flag must be 1 when frame_mbs_only_flag equal to 0!\n");
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_MARKER);
        s->avctx->iTotalError++;
        sps->direct_8x8_inference_flag = 1;
        warn_cnt++;
        //		goto fail;
    }

#ifndef ALLOW_INTERLACE

    if (sps->mb_aff)
    { av_log(h->s.avctx, AV_LOG_ERROR, "MBAFF support not included; enable it at compile-time.\n"); }

#endif
    sps->crop = get_bits1(&s->gb);

    if (sps->crop)
    {
        sps->crop_left  = get_ue_golomb(&s->gb);
        sps->crop_right = get_ue_golomb(&s->gb);
        sps->crop_top   = get_ue_golomb(&s->gb);
        sps->crop_bottom = get_ue_golomb(&s->gb);

        /* 2010/04/06 11:07:00 liuxw+00139685 */
        /* ���Ӷ�crop���ж� */
        if ((int)(sps->crop_left + sps->crop_right) >= sps->mb_width * MB_WIDTH || (int)(sps->crop_top + sps->crop_bottom) >= sps->mb_height * MB_HEIGHT * (2 - sps->frame_mbs_only_flag))
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "crop_left[%d]/crop_right[%d]/crop_top[%d]/crop_bottom[%d] is invalid!\n", sps->crop_left, sps->crop_right, sps->crop_top, sps->crop_bottom);
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_SIZE);
            s->avctx->iTotalError++;
            sps->crop_left  = sps->crop_right = sps->crop_top = sps->crop_bottom = 0;
            warn_cnt++;
            //			goto fail;
        }

        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ȥ���˴�ӡ��֧��crop���ܵ���� */
        /*        if(sps->crop_left || sps->crop_top){
                   av_log(h->s.avctx, AV_LOG_ERROR, "insane cropping not completely supported, this could look slightly wrong ...\n");
               }
              if(sps->crop_right >= 8 || sps->crop_bottom >= (8>> !sps->frame_mbs_only_flag)){
                   av_log(h->s.avctx, AV_LOG_ERROR, "brainfart cropping not supported, this could look slightly wrong ...\n");
               }
          */
    }
    else
    {
        sps->crop_left   =
            sps->crop_right  =
                sps->crop_top    =
                    sps->crop_bottom = 0;
    }

    if (sps->crop)
    {
        int SubWidthC[] = {1, 2, 2, 1};
        int SubHeightC[] = {1, 2, 1, 1};
        int crop_left, crop_right, crop_top, crop_bottom;

        crop_left   = SubWidthC[sps->chroma_format_idc] * sps->crop_left;
        crop_right  = SubWidthC[sps->chroma_format_idc] * sps->crop_right;
        crop_top    = SubHeightC[sps->chroma_format_idc] * ( 2 - sps->frame_mbs_only_flag ) * sps->crop_top;
        crop_bottom = SubHeightC[sps->chroma_format_idc] * ( 2 - sps->frame_mbs_only_flag ) * sps->crop_bottom;

        width  = sps->mb_width * MB_WIDTH - crop_left - crop_right;
        height = sps->mb_height * MB_HEIGHT * (2 - sps->frame_mbs_only_flag) - crop_top - crop_bottom;
    }
    else
    {
        width  = sps->mb_width * MB_WIDTH;
        height = sps->mb_height * MB_HEIGHT * (2 - sps->frame_mbs_only_flag);
    }

    if (s->avctx->usActualWidth || s->avctx->usActualHeight)
    {
        if (s->avctx->usActualWidth != width || s->avctx->usActualHeight != height)
        {
            int index;

            av_log(NULL, AV_LOG_WARNING, "usActualWidth[%d] != width[%d] || usActualHeight[%d] != height[%d]!\n", s->avctx->usActualWidth, width, s->avctx->usActualHeight, height);
            IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_SIZE);
            s->avctx->iTotalError++;

            if (find_sps(h, &index) < 0)
            {
                goto fail;
            }

            sps->mb_width  = h->sps_buffers[index]->mb_width;
            sps->mb_height = h->sps_buffers[index]->mb_height;
            /* 2010/06/03 9:00:00 liuxw+00139685 */
            /* �������⣬Ҳ�����crop�������� */
            sps->crop        = h->sps_buffers[index]->crop;
            sps->crop_top    = h->sps_buffers[index]->crop_top;
            sps->crop_bottom = h->sps_buffers[index]->crop_bottom;
            sps->crop_left   = h->sps_buffers[index]->crop_left;
            sps->crop_right  = h->sps_buffers[index]->crop_right;
        }
    }
    else
    {
        s->avctx->usActualWidth  = width;
        s->avctx->usActualHeight = height;
        /* 2010/06/03 9:30:00 liuxw+00139685 */
        /* ���Ϸ���width��height[δcrop��]����avctx->widht��avctx->height */
        s->avctx->width  = sps->mb_width * MB_WIDTH;
        s->avctx->height = (sps->mb_height * (2 - sps->frame_mbs_only_flag)) * MB_HEIGHT;
    }

    /* ��⵱ǰ�����Ŀ�Ⱥͳ����Ƿ񳬹���̬�����е�����Ⱥ͸߶� */
    if (s->avctx->usActualWidth > s->avctx->usSourceWidth || s->avctx->usActualHeight > s->avctx->usSourceHeight)
    {
        //		av_log(s->avctx, AV_LOG_ERROR, "codec_ctx[%p] h264_dec[%p] decode_slice_header() the ActualWidth[%u] of the bitstream is more than MaxWidth[%u] or the ActualHeight[%u] of the bitstream is more than MaxHeight[%u]!\n",
        av_log(s->avctx, AV_LOG_WARNING, "codec_ctx[%p] h264_dec[%p] decode_slice_header() the ActualWidth[%u] of the bitstream is more than MaxWidth[%u] or the ActualHeight[%u] of the bitstream is more than MaxHeight[%u]!\n",
               s->avctx, h, (unsigned int)s->avctx->usActualWidth, (unsigned int)s->avctx->usSourceWidth, (unsigned int)s->avctx->usActualHeight, (unsigned int)s->avctx->usSourceHeight);
        IMEDIA_SET_ERR_SEQ(s->avctx->iErrorCode, IMEDIA_ERR_SEQ_SIZE);
        s->avctx->iTotalError++;
        goto fail;
    }


    sps->vui_parameters_present_flag = get_bits1(&s->gb);

    if ( sps->vui_parameters_present_flag )
    { decode_vui_parameters(h, sps); }

    if (s->avctx->debug & FF_DEBUG_PICT_INFO)
    {
        av_log(h->s.avctx, AV_LOG_DEBUG, "sps:%u profile:%d/%d poc:%d ref:%d %dx%d %s %s crop:%d/%d/%d/%d %s %s\n",
               sps_id, sps->profile_idc, sps->level_idc,
               sps->poc_type,
               sps->ref_frame_count,
               sps->mb_width, sps->mb_height,
               sps->frame_mbs_only_flag ? "FRM" : (sps->mb_aff ? "MB-AFF" : "PIC-AFF"),
               sps->direct_8x8_inference_flag ? "8B8" : "",
               sps->crop_left, sps->crop_right,
               sps->crop_top, sps->crop_bottom,
               sps->vui_parameters_present_flag ? "VUI" : ""
               /* 2010/30/04 9:10:00 liuxw+00139685 */
               /* ��c99д����Ϊc89��д����MS2008��֧��c99 */
               /* ,((const char*[]){"Gray","420","422","444"})[sps->chroma_format_idc] */
               , apszColorFmt[sps->chroma_format_idc]
              );
    }

    /* 2010/04/06 10:07:00 liuxw+00139685 */
    /* ���Ӷ����ı������ĺϷ����ж� */
    if (h->s.gb.index > h->s.gb.size_in_bits)
    {
        av_log(s->avctx, AV_LOG_WARNING, "Bit conmused[%d] by SPS is over its size[%d]!\n", h->s.gb.index, h->s.gb.size_in_bits);

        if (h->sps_buffers[sps_id])
        {
            av_free_hw(sps);
            h->sps = *h->sps_buffers[sps_id];
            return 0;
        }
    }

    if (warn_cnt && h->sps_buffers[sps_id] && sps_check(sps, h->sps_buffers[sps_id]) < 0)
    {
        av_free_hw(sps);
        h->sps = *h->sps_buffers[sps_id];
        return 0;
    }

    if (h->sps_buffers[sps_id])
    {
        av_free_hw(h->sps_buffers[sps_id]);
    }

    h->sps_buffers[sps_id] = sps;
    h->sps = *sps;
    return 0;
fail:
    av_free_hw(sps);
    return -1;
}

static void
build_qp_table(PPS *pps, int t, int index)
{
    int i;

    for (i = 0; i < 52; i++)
    { pps->chroma_qp_table[t][i] = chroma_qp[av_clip(i + index, 0, 51)]; }
}

/* 2010/05/5 15:00:00 liuxw+00139685 */
/* ������������⵱ǰ������PPS���Ѿ�������pps_id��ͬ��PPS���йؼ��﷨Ԫ�صıȽ� */
int pps_check(PPS *cur_pps, PPS *old_pps)
{
    if (cur_pps->cabac != old_pps->cabac)
    {
        return -1;
    }

    return 0;
}

int ff_h264_decode_picture_parameter_set(H264Context *h, int bit_length)
{
    MpegEncContext *const s = &h->s;
    unsigned int pps_id = get_ue_golomb(&s->gb);
    PPS *pps;
    int warn_cnt = 0;

    if (pps_id >= MAX_PPS_COUNT)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "pps_id (%d) out of range\n", pps_id);
        av_log(h->s.avctx, AV_LOG_WARNING, "pps_id (%d) out of range\n", pps_id);
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_ID);
        s->avctx->iTotalError++;
        pps_id = 0;
        warn_cnt++;
        //      return -1;
    }

    //  pps= av_mallocz(sizeof(PPS));
    pps = av_malloc_hw(s->avctx, sizeof(PPS));

    if (pps == NULL)
    {
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* �����ڴ����ʧ�ܵ���־��Ϣ*/
        av_log(h->s.avctx, AV_LOG_ERROR, "Malloc memory for pps is failed!\n");
        h->s.avctx->iMallocFailed = 1;
        return -1;
    }

    memset(pps, 0, sizeof(PPS));

    pps->sps_id = get_ue_golomb_31(&s->gb);

    if ((unsigned)pps->sps_id >= MAX_SPS_COUNT || h->sps_buffers[pps->sps_id] == NULL)
    {
        int index;
        //      av_log(h->s.avctx, AV_LOG_ERROR, "sps_id out of range\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "sps_id out of range\n");
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_ID);
        s->avctx->iTotalError++;

        if (find_sps(h, &index) < 0)
        {
            goto fail;
        }

        pps->sps_id = index;
        warn_cnt++;
        //      goto fail;
    }

    pps->cabac = get_bits1(&s->gb);
    pps->pic_order_present = get_bits1(&s->gb);
    pps->slice_group_count = get_ue_golomb(&s->gb) + 1;

    /* 2010/03/22 17:00:00 liuxw+00139685 */
    /* ���Ӷ�slice_group_count�Ϸ��Եļ�� */
    if (1 != pps->slice_group_count)
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "slice_group_count[%d] is invalid or not supported currently!\n", pps->slice_group_count);
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_SLICE_GROUP_COUNT);
        s->avctx->iTotalError++;
        pps->slice_group_count = 1;
        warn_cnt++;
        //		goto fail;
    }

    if (pps->slice_group_count > 1 )
    {
        pps->mb_slice_group_map_type = get_ue_golomb(&s->gb);
        //      av_log(h->s.avctx, AV_LOG_ERROR, "FMO not supported\n");
        av_log(h->s.avctx, AV_LOG_WARNING, "FMO not supported\n");

        switch (pps->mb_slice_group_map_type)
        {
            case 0:
#if 0
                |   for ( i = 0; i <= num_slice_groups_minus1; i++ ) |   |        |
                    |    run_length[ i ]                                | 1  | ue(v)   |
#endif
                    break;

            case 2:
#if 0
                |   for ( i = 0; i < num_slice_groups_minus1; i++ )  |   |        |
                    | {                                                  |   |        |
                            |    top_left_mb[ i ]                               | 1  | ue(v)   |
                            |    bottom_right_mb[ i ]                           | 1  | ue(v)   |
                            |
                      }                                               |   |        |
#endif
                    break;

            case 3:
            case 4:
            case 5:
#if 0
                |   slice_group_change_direction_flag               | 1  | u(1)    |
                |   slice_group_change_rate_minus1                  | 1  | ue(v)   |
#endif
                break;

            case 6:
#if 0
                |   slice_group_id_cnt_minus1                       | 1  | ue(v)   |
                |   for ( i = 0; i <= slice_group_id_cnt_minus1; i++ |   |        |
                          | )                                                  |   |        |
                    |    slice_group_id[ i ]                            | 1  | u(v)    |
#endif
                    break;
        }
    }

    pps->ref_count[0] = get_ue_golomb(&s->gb) + 1;
    pps->ref_count[1] = get_ue_golomb(&s->gb) + 1;

    if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1)
    {
        //      av_log(h->s.avctx, AV_LOG_ERROR, "reference list0 count[%d] or reference list1 count[%d] overflow!\n",pps->ref_count[0],pps->ref_count[1]);
        av_log(h->s.avctx, AV_LOG_WARNING, "reference list0 count[%d] or reference list1 count[%d] overflow!\n", pps->ref_count[0], pps->ref_count[1]);
        /* 2010/03/22 17:00:00 liuxw+00139685 */
        /* ���Ӵ����� */
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_SLICE_REF);
        s->avctx->iTotalError++;
        pps->ref_count[0] = pps->ref_count[1] = 1;
        warn_cnt++;
        //      goto fail;
    }

    pps->weighted_pred = get_bits1(&s->gb);
    pps->weighted_bipred_idc = get_bits(&s->gb, 2);

    /* 2010/04/06 14:00:00 liuxw+00139685 */
    /* ���Ӷ�weighted_bipred_idc�Ϸ��Եļ���� */
    if ((h->sps_buffers[pps->sps_id]->profile_idc == 66 && (pps->weighted_pred != 0 || pps->weighted_bipred_idc != 0)) || pps->weighted_bipred_idc == 3)
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "weighted_pred[%d] or weighted_bipred_idc[%d] in pps is invalid!\n", pps->weighted_pred, pps->weighted_bipred_idc);
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_MARKER);
        s->avctx->iTotalError++;
        pps->weighted_pred = pps->weighted_bipred_idc = 0;
        warn_cnt++;
        //		goto fail;
    }

    pps->init_qp = get_se_golomb(&s->gb) + 26;
    pps->init_qs = get_se_golomb(&s->gb) + 26;

    /* 2010/04/06 14:00:00 liuxw+00139685 */
    /* ���Ӷ�init_qp/init_qs�Ϸ��Եļ���� */
    if (pps->init_qp >= 52 || pps->init_qp < 0 || pps->init_qs >= 52 || pps->init_qs < 0)
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "init_qp[%d]/init_qs[%d] in pps is invalid!\n", pps->init_qp, pps->init_qs);
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_QP);
        s->avctx->iTotalError++;
        pps->init_qp = 26;
        warn_cnt++;
        //		goto fail;
    }

    pps->chroma_qp_index_offset[0] = get_se_golomb(&s->gb);

    /* 2010/04/06 14:00:00 liuxw+00139685 */
    /* ���Ӷ�chroma_qp_index_offset�Ϸ��Եļ���� */
    if (pps->chroma_qp_index_offset[0] > 12 || pps->chroma_qp_index_offset[0] < -12)
    {
        av_log(h->s.avctx, AV_LOG_WARNING, "chroma_qp_index_offset[%d] in pps is invalid!\n", pps->chroma_qp_index_offset[0]);
        IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_QP);
        s->avctx->iTotalError++;
        pps->chroma_qp_index_offset[0] = 0;
        warn_cnt++;
        //		goto fail;
    }

    pps->deblocking_filter_parameters_present = get_bits1(&s->gb);
    pps->constrained_intra_pred = get_bits1(&s->gb);
    pps->redundant_pic_cnt_present = get_bits1(&s->gb);
    pps->transform_8x8_mode = 0;
    h->dequant_coeff_pps = -1; //contents of sps/pps can change even if id doesn't, so reinit
    memcpy(pps->scaling_matrix4, h->sps_buffers[pps->sps_id]->scaling_matrix4, sizeof(pps->scaling_matrix4));
    memcpy(pps->scaling_matrix8, h->sps_buffers[pps->sps_id]->scaling_matrix8, sizeof(pps->scaling_matrix8));

    if (get_bits_count(&s->gb) < bit_length)
    {
        pps->transform_8x8_mode = get_bits1(&s->gb);
        decode_scaling_matrices(h, h->sps_buffers[pps->sps_id], pps, 0, pps->scaling_matrix4, pps->scaling_matrix8);
        pps->chroma_qp_index_offset[1] = get_se_golomb(&s->gb); //second_chroma_qp_index_offset

        /* 2010/04/06 14:00:00 liuxw+00139685 */
        /* ���Ӷ�chroma_qp_index_offset�Ϸ��Եļ���� */
        if (pps->chroma_qp_index_offset[1] > 12 || pps->chroma_qp_index_offset[1] < -12)
        {
            av_log(h->s.avctx, AV_LOG_WARNING, "chroma_qp_index_offset[%d] in pps is invalid!\n", pps->chroma_qp_index_offset[1]);
            IMEDIA_SET_ERR_PIC(s->avctx->iErrorCode, IMEDIA_ERR_PIC_QP);
            s->avctx->iTotalError++;
            pps->chroma_qp_index_offset[1] = 0;
            warn_cnt++;
            //			goto fail;
        }
    }
    else
    {
        pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
    }

    build_qp_table(pps, 0, pps->chroma_qp_index_offset[0]);
    build_qp_table(pps, 1, pps->chroma_qp_index_offset[1]);

    if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
    { h->pps.chroma_qp_diff = 1; }

    if (s->avctx->debug & FF_DEBUG_PICT_INFO)
    {
        av_log(h->s.avctx, AV_LOG_DEBUG, "pps:%u sps:%u %s slice_groups:%d ref:%d/%d %s qp:%d/%d/%d/%d %s %s %s %s\n",
               pps_id, pps->sps_id,
               pps->cabac ? "CABAC" : "CAVLC",
               pps->slice_group_count,
               pps->ref_count[0], pps->ref_count[1],
               pps->weighted_pred ? "weighted" : "",
               pps->init_qp, pps->init_qs, pps->chroma_qp_index_offset[0], pps->chroma_qp_index_offset[1],
               pps->deblocking_filter_parameters_present ? "LPAR" : "",
               pps->constrained_intra_pred ? "CONSTR" : "",
               pps->redundant_pic_cnt_present ? "REDU" : "",
               pps->transform_8x8_mode ? "8x8DCT" : ""
              );
    }

    /* 2010/04/06 10:07:00 liuxw+00139685 */
    /* ���Ӷ����ı������ĺϷ����ж� */
    if (h->s.gb.index > h->s.gb.size_in_bits)
    {
        av_log(s->avctx, AV_LOG_WARNING, "Bit conmused[%d] by PPS is over its size[%d]!\n", h->s.gb.index, h->s.gb.size_in_bits);

        if (h->pps_buffers[pps_id])
        {
            av_free_hw(pps);
            return 0;
        }
    }

    if (warn_cnt && h->pps_buffers[pps_id] && (pps_check(pps, h->pps_buffers[pps_id]) < 0 || warn_cnt > 3))
    {
        av_free_hw(pps);
        return 0;
    }

    if (h->pps_buffers[pps_id])
    {
        av_free_hw(h->pps_buffers[pps_id]);
    }

    h->pps_buffers[pps_id] = pps;
    return 0;
fail:
    av_free_hw(pps);
    return -1;
}

/**
 * Call decode_slice() for each context.
 *
 * @param h h264 master context
 * @param context_count number of contexts to execute
 */
static void execute_decode_slices(H264Context *h, int context_count)
{
    MpegEncContext *const s = &h->s;
    AVCodecContext *const avctx = s->avctx;
    H264Context *hx;
    int i;

    if (s->avctx->hwaccel)
    { return; }

    if (s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
    { return; }

    if (context_count == 1)
    {
        decode_slice(avctx, &h);
    }
    else
    {
        for (i = 1; i < context_count; i++)
        {
            hx = h->thread_context[i];
            hx->s.error_recognition = avctx->error_recognition;
            hx->s.error_count = 0;
        }

        avctx->execute(avctx, (void *)decode_slice,
                       (void **)h->thread_context, NULL, context_count, sizeof(void *));

        /* pull back stuff from slices to master context */
        hx = h->thread_context[context_count - 1];
        s->mb_x = hx->s.mb_x;
        s->mb_y = hx->s.mb_y;
        s->dropable = hx->s.dropable;
        s->picture_structure = hx->s.picture_structure;

        for (i = 1; i < context_count; i++)
        { h->s.error_count += h->thread_context[i]->s.error_count; }
    }
}


static int decode_nal_units(H264Context *h, const uint8_t *buf, int buf_size)
{
    MpegEncContext *const s = &h->s;
    AVCodecContext *const avctx = s->avctx;
    int buf_index = 0;
    H264Context *hx; ///< thread context
    int context_count = 0;

    h->max_contexts = avctx->thread_count;
#if 0
    int i;

    for (i = 0; i < 50; i++)
    {
        av_log(NULL, AV_LOG_ERROR, "%02X ", buf[i]);
    }

#endif

    /* ÿ����һ��decode_nal_units�����������������һ�γ�ʼֵ */
    if (!(s->flags2 & CODEC_FLAG2_CHUNKS))
    {
        h->current_slice = 0;

        /* �����ǰ��һ֡�򶥳�����s->current_picture_ptr�ÿ� liuxw+00139685 */
        if (!s->first_field)
        { s->current_picture_ptr = NULL; }

        reset_sei(h);  /* ��λSEI�еı�־λ */
    }

    for (;;)
    {
        int consumed;
        int dst_length;
        int bit_length;
        const uint8_t *ptr;
        int i, nalsize = 0;
        int err;

        /* AVC1��iso base media��ʽ�ļ��е�һ��box������ */
        if (h->is_avc)
        {
            if (buf_index >= buf_size)
            { break; }

            nalsize = 0;

            for (i = 0; i < h->nal_length_size; i++)
            { nalsize = (nalsize << 8) | buf[buf_index++]; }

            if (nalsize <= 1 || (nalsize + buf_index > buf_size))
            {
                if (nalsize == 1)
                {
                    buf_index++;
                    continue;
                }
                else
                {
                    //                  av_log(h->s.avctx, AV_LOG_ERROR, "AVC: nal size %d\n", nalsize);
                    av_log(h->s.avctx, AV_LOG_WARNING, "AVC: nal size %d\n", nalsize);
                    break;
                }
            }
        }
        else
        {
            /* h264 elementary stream */
            /* Ѱ����ʼ��0x000001 */
            for (; buf_index + 3 < buf_size; buf_index++)
            {
                // This should always succeed in the first iteration.
                if (buf[buf_index] == 0 && buf[buf_index + 1] == 0 && buf[buf_index + 2] == 1)
                { break; } /* �ҵ���ʼ������˳� */
            }

            /* һֱ�ҵ�stream buffer�ĵ����������ֽڣ���û���ҵ���ʼ�룬���˳�(��Ҳ��for����������) */
            if (buf_index + 3 >= buf_size)
            { break; }

            /* ���ҵ��ˣ���������ʼ�� */
            buf_index += 3;
        }

        hx = h->thread_context[context_count];
        /* ��EBSPת��ΪRBSP��ȥ�������еķ�����0x03��*/
        ptr = ff_h264_decode_nal(hx, buf + buf_index, &dst_length, &consumed, h->is_avc ? nalsize : buf_size - buf_index);

        if (ptr == NULL || dst_length < 0)
        {
            return -1;
        }

        /* ȥ���������Ϊ0���ֽ� */
        while (ptr[dst_length - 1] == 0 && dst_length > 0)
        { dst_length--; }

        /* ȥ��bit�������λtrainling���أ��õ���ǰnal������bit����Ŀ liuxw+00139685 */
        bit_length = !dst_length ? 0 : (8 * dst_length - ff_h264_decode_rbsp_trailing(h, ptr + dst_length - 1));

        if (s->avctx->debug & FF_DEBUG_STARTCODE)
        {
            av_log(h->s.avctx, AV_LOG_DEBUG, "NAL %d at %d/%d length %d\n", hx->nal_unit_type, buf_index, buf_size, dst_length);
        }

        /* avc1 */
        if (h->is_avc && (nalsize != consumed))
        {
            int i, debug_level = AV_LOG_DEBUG;

            for (i = consumed; i < nalsize; i++)
                if (buf[buf_index + i])
                    //                 debug_level = AV_LOG_ERROR;
                { debug_level = AV_LOG_WARNING; }

            av_log(h->s.avctx, debug_level, "AVC: Consumed only %d bytes instead of %d\n", consumed, nalsize);
            consumed = nalsize;
        }

        /* ��ǰ���ĵ��ֽ��� */
        buf_index += consumed;

        /* �����skipģ���ҵ�ǰ֡�����ο�����ɶ�����ǰ֡ */
        if (  (s->hurry_up == 1 && h->nal_ref_idc  == 0) //FIXME do not discard SEI id
              || (avctx->skip_frame >= AVDISCARD_NONREF && h->nal_ref_idc  == 0))
        { continue; }

    again:
        err = 0;

        switch (hx->nal_unit_type)
        {
                /* IDR slice���� */
            case NAL_IDR_SLICE:
                if (h->nal_unit_type != NAL_IDR_SLICE)
                {
                    //              av_log(h->s.avctx, AV_LOG_ERROR, "Invalid mix of idr and non-idr slices");
                    av_log(h->s.avctx, AV_LOG_WARNING, "Invalid mix of idr and non-idr slices");
                    //              return -1;
                    h->nal_unit_type = NAL_IDR_SLICE;
                }

                /* 2010/04/06 14:30:00 liuxw+00139685 */
                /* ���Ӷ�hx->nal_ref_idc�ļ�� */
                if (hx->nal_ref_idc == 0)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "nal_ref_idc[%d] is invalid when nal_unit_type equal to NAL_IDR_SLICE!\n", hx->nal_ref_idc);
                    IMEDIA_SET_ERR_SEQ(h->s.avctx->iErrorCode, IMEDIA_ERR_SEQ_OTHER);
                    h->s.avctx->iTotalError++;
                    //				break;
                    hx->nal_ref_idc = 2;
                }

                /* ��ն��ڲο��ͳ��ڲο������Լ���0��frame_num,poc */
                idr(h); //FIXME ensure we don't loose some frames if there is reordering

                /* ��IDR slice���� */
            case NAL_SLICE:
                /* ��ʼ����ǰbit stream����Ϣ�����ȣ���ʼ��ַ�� */
                init_get_bits(&hx->s.gb, ptr, bit_length);
                hx->intra_gb_ptr =
                    hx->inter_gb_ptr = &hx->s.gb;
                hx->s.data_partitioning = 0;

                /* ����sliceͷ��Ϣ���ο����й���init,reordering,marking) */
                if ((err = decode_slice_header(hx, h)))
                { break; }

                /* Ӳ�����ٹ��ܣ��ݲ�֧�� */
                if (s->avctx->hwaccel && h->current_slice == 1)
                {
                    if (s->avctx->hwaccel->start_frame(s->avctx, NULL, 0) < 0)
                    { return -1; }
                }

                /* �ؼ�֡(IDR)��־���� */
                /* 2010/08/9 19:00:00 liuxw+00139685 */
                /* ֻ��IDR��ʱ�������Ϊ�ؼ�֡ */
                //          s->current_picture_ptr->key_frame |= (hx->nal_unit_type == NAL_IDR_SLICE) || (h->sei_recovery_frame_cnt >= 0);
                s->current_picture_ptr->key_frame |= (hx->nal_unit_type == NAL_IDR_SLICE);

                /* Ӳ������, �ݲ�֧�� */
                if (hx->redundant_pic_count == 0 && hx->s.hurry_up < 5
                    && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
                    && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos != FF_B_TYPE)
                    && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos == FF_I_TYPE)
                    && avctx->skip_frame < AVDISCARD_ALL)
                {
                    if (avctx->hwaccel)
                    {
                        if (avctx->hwaccel->decode_slice(avctx, &buf[buf_index - consumed], consumed) < 0)
                        { return -1; }
                    }
                    else if (CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
                    {

                        static const uint8_t start_code[] = {0x00, 0x00, 0x01};
                        //ff_vdpau_add_data_chunk(s, start_code, sizeof(start_code));
                        //ff_vdpau_add_data_chunk(s, &buf[buf_index - consumed], consumed );
                    }
                    else
                    { context_count++; }  /* �ý�в��־ */
                }

                break;

            case NAL_DPA:
                init_get_bits(&hx->s.gb, ptr, bit_length);
                hx->intra_gb_ptr =
                    hx->inter_gb_ptr = NULL;
                hx->s.data_partitioning = 1;

                err = decode_slice_header(hx, h);
                break;

            case NAL_DPB:
                init_get_bits(&hx->intra_gb, ptr, bit_length);
                hx->intra_gb_ptr = &hx->intra_gb;
                break;

            case NAL_DPC:
                init_get_bits(&hx->inter_gb, ptr, bit_length);
                hx->inter_gb_ptr = &hx->inter_gb;

                if (hx->redundant_pic_count == 0 && hx->intra_gb_ptr && hx->s.data_partitioning
                    && s->context_initialized
                    && s->hurry_up < 5
                    && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
                    && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos != FF_B_TYPE)
                    && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos == FF_I_TYPE)
                    && avctx->skip_frame < AVDISCARD_ALL)
                { context_count++; }

                break;

                /* SEI ���� */
            case NAL_SEI:
                init_get_bits(&s->gb, ptr, bit_length);
                ff_h264_decode_sei(h);
                break;

                /* sps ���� */
            case NAL_SPS:

                /* 2010/04/06 14:30:00 liuxw+00139685 */
                /* ���Ӷ�hx->nal_ref_idc�ļ�� */
                if (hx->nal_ref_idc == 0)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "nal_ref_idc[%d] is invalid when nal_unit_type equal to NAL_SPS!\n", hx->nal_ref_idc);
                    //				break;
                    IMEDIA_SET_ERR_SEQ(h->s.avctx->iErrorCode, IMEDIA_ERR_SEQ_OTHER);
                    h->s.avctx->iTotalError++;
                }

                init_get_bits(&s->gb, ptr, bit_length);
                ff_h264_decode_seq_parameter_set(h);

                if (s->flags & CODEC_FLAG_LOW_DELAY)
                { s->low_delay = 1; }

                if (avctx->has_b_frames < 2)
                { avctx->has_b_frames = !s->low_delay; }

                break;

                /* pps���� */
            case NAL_PPS:

                /* 2010/04/06 14:30:00 liuxw+00139685 */
                /* ���Ӷ�hx->nal_ref_idc�ļ�� */
                if (hx->nal_ref_idc == 0)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "nal_ref_idc[%d] is invalid when nal_unit_type equal to NAL_PPS!\n", hx->nal_ref_idc);
                    //				break;
                    IMEDIA_SET_ERR_SEQ(h->s.avctx->iErrorCode, IMEDIA_ERR_SEQ_OTHER);
                    h->s.avctx->iTotalError++;
                }

                init_get_bits(&s->gb, ptr, bit_length);
                ff_h264_decode_picture_parameter_set(h, bit_length);
                break;

            case NAL_AUD:
            case NAL_END_SEQUENCE:
            case NAL_END_STREAM:
            case NAL_FILLER_DATA:
            case NAL_SPS_EXT:
            case NAL_AUXILIARY_SLICE:
                break;

            default:
                av_log(avctx, AV_LOG_DEBUG, "Unknown NAL code: %d (%d bits)\n", h->nal_unit_type, bit_length);
        }

        /* �в���롢��������IDCT���ؽ�����·�˲� */
        if (context_count == h->max_contexts)
        {
            execute_decode_slices(h, context_count);
            context_count = 0;
        }

        if (err < 0)
        {
            //          av_log(h->s.avctx, AV_LOG_ERROR, "decode_slice_header error\n");
            av_log(h->s.avctx, AV_LOG_WARNING, "decode_slice_header error\n");

            if (h->s.avctx->iMallocFailed)
            {
                return -1;
            }
        }
        else if (err == 1)
        {
            /* Slice could not be decoded in parallel mode, copy down
             * NAL unit stuff to context 0 and restart. Note that
             * rbsp_buffer is not transferred, but since we no longer
             * run in parallel mode this should not be an issue. */
            h->nal_unit_type = hx->nal_unit_type;
            h->nal_ref_idc   = hx->nal_ref_idc;
            hx = h;
            goto again;
        }
    }

    /* һ�㲻����˷�֧ */
    if (context_count)
    { execute_decode_slices(h, context_count); }

    /* ���ؽ��뵱ǰ֡���ĵ��ֽ��� */
    return buf_index;
}

/**
 * returns the number of bytes consumed for building the current frame
 */
static int get_consumed_bytes(MpegEncContext *s, int pos, int buf_size)
{
    if (pos == 0) { pos = 1; } //avoid infinite loops (i doubt that is needed but ...)

    if (pos + 10 > buf_size) { pos = buf_size; } // oops ;)

    return pos;
}

/* h264 decode: һ��ֻ��һ֡������(�ɰ�������nal_type������) */
static int decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size,
                        const uint8_t *buf, int buf_size)
{
    H264Context *h = avctx->priv_data;
    MpegEncContext *s = &h->s;
    AVFrame *pict = data;
    int buf_index;

    s->flags = avctx->flags;
    s->flags2 = avctx->flags2;

    /* 2010/06/29 19:30:00 liuxw+00139685 */
    /* ��display_pic������δ�����pic���������display_pic�е�ͼ�� */
    if (h->display_pic_num)
    {
        Picture *out;
        int i;

        out = h->display_pic[0];
        h->display_pic_num--;

        if (out->reference && out->reference != DELAYED_PIC_REF)
        {
            h->display_pic_ref_num--;
        }

        if (out->reference == DELAYED_PIC_REF)
        {
            out->reference = 0;
        }

#ifdef LXW_DEBUG
        {
            char frame_type[3][10] = {"I frame", "P frame", "B frame"};
            av_log(h->s.avctx, AV_LOG_WARNING, "output_num: %d poc: %d frame_type: %s frame_num:%d\n", output_num, out->poc, frame_type[out->pict_type - 1], out->coded_picture_number);
            output_num++;
        }
#endif

        for (i = 0; i < h->display_pic_num; i++)
        {
            h->display_pic[i] = h->display_pic[i + 1];
        }

        *data_size = sizeof(AVFrame);
        *pict = *(AVFrame *)out;
        /* ���ȥ��crop���YUV���� */
        {
            int SubHeightC[] = {1, 2, 1, 1};
            int SubWidthC[]  = {1, 2, 2, 1};

            pict->data[0] = pict->data[0] + pict->linesize[0] * h->sps.crop_top * SubHeightC[ h->sps.chroma_format_idc] * ( 2 -  h->sps.frame_mbs_only_flag ) + h->sps.crop_left * SubWidthC[h->sps.chroma_format_idc];
            pict->data[1] = pict->data[1] + pict->linesize[1] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
            pict->data[2] = pict->data[2] + pict->linesize[2] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
        }

        assert(pict->data[0] || !*data_size);
        ff_print_debug_info(s, pict);

        return sizeof(AVFrame);

    }


    /* end of stream, output what is still in the buffers */
    if (buf_size == 0)
    {
        Picture *out;
        int i, out_idx;

        //FIXME factorize this with the output code below
        out = h->delayed_pic[0];

        if (out  == NULL) /* ���delay buffer��û�л����YUV�����ý�����־������ */
        {
            pict->ucLastFrame = 1;
            return 0;
        }

        out_idx = 0;

        /* ��delay buffer��ѡȡPOC��С��һ֡YUV��Ϊ��ǰ���֡ */
        /* 2010/05/31 10:00:00 liuxw+00139685 */
        /* ���Ӷ�i�ķ�Χ�����ƣ���ֹ���� */
        /* 2010/06/29 19:30:00 liuxw+00139685 */
        /* �޸��ж����� */
        //      for(i=1; i<s->avctx->has_b_frames && h->delayed_pic[i] && (h->delayed_pic[i]->poc && !h->delayed_pic[i]->key_frame); i++)
        for (i = 1; i < h->delay_pic_num && (h->delayed_pic[i]->poc && !h->delayed_pic[i]->key_frame); i++)
        {
            if (h->delayed_pic[i]->poc < out->poc)
            {
                out = h->delayed_pic[i];
                out_idx = i;
            }
        }

        out->reference = 0;

        /* 2010/06/29 19:30:00 liuxw+00139685 */
        /* delay_pic����Ŀ���� */
        h->delay_pic_num--;

#ifdef LXW_DEBUG
        {
            char frame_type[3][10] = {"I frame", "P frame", "B frame"};
            av_log(h->s.avctx, AV_LOG_WARNING, "output_num: %d poc: %d frame_type: %s\n", output_num, out->poc, frame_type[out->pict_type - 1]);
            output_num++;
        }
#endif

        /* �����СPOC֮֡�󣬽���СPOC֡�����֡��ǰ�ƶ� */
        for (i = out_idx; h->delayed_pic[i]; i++)
        { h->delayed_pic[i] = h->delayed_pic[i + 1]; }

        /* ����ǰ���֡���ڣ�������ָ��ʹ�С��ֵ */
        //h->outputed_poc = out->poc;
        *data_size = sizeof(AVFrame);
        *pict = *(AVFrame *)out;
        //s->avctx->has_b_frames--;
        /* ȥ��ͼ��crop���� */
        {
            int SubHeightC[] = {1, 2, 1, 1};
            int SubWidthC[] = {1, 2, 2, 1};

            pict->data[0] = pict->data[0] + pict->linesize[0] * h->sps.crop_top * SubHeightC[ h->sps.chroma_format_idc] * ( 2 -  h->sps.frame_mbs_only_flag ) + h->sps.crop_left * SubWidthC[h->sps.chroma_format_idc];
            pict->data[1] = pict->data[1] + pict->linesize[1] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
            pict->data[2] = pict->data[2] + pict->linesize[2] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
        }

        /* ��delay buffer�л�����YUV�����ý�����־Ϊ0,������AVFrame�ṹ��Ĵ�С */
        /* 2010/05/31 10:00:00 liuxw+00139685 */
        /* ���ӷ�����������delay֡�ĸ���С�ڵ���0ʱ���˳� */
        /* 2010/06/29 19:30:00 liuxw+00139685 */
        /* �޸��ж����� */
        //if(h->delayed_pic[0] || 0 < s->avctx->has_b_frames)
        if (h->delay_pic_num )
        {
            pict->ucLastFrame = 0;   //add by lxw
            return sizeof(AVFrame);
        }
        else	/* ��delay_buffer��û��YUV�����ý�����־Ϊ1��������0 */
        {
            pict->ucLastFrame = 1;   //add by lxw
            return 0;
        }
    }

    /* AVC1��iso base media��ʽ�ļ��е�һ��box������ */
    if (h->is_avc && !h->got_avcC)
    {
        int i, cnt, nalsize;
        unsigned char *p = avctx->extradata[0];

        if (avctx->extradata_size[0] < 7)
        {
            //          av_log(avctx, AV_LOG_ERROR, "avcC too short\n");
            av_log(avctx, AV_LOG_WARNING, "avcC too short\n");
            return -1;
        }

        if (*p != 1)
        {
            //          av_log(avctx, AV_LOG_ERROR, "Unknown avcC version %d\n", *p);
            av_log(avctx, AV_LOG_WARNING, "Unknown avcC version %d\n", *p);
            return -1;
        }

        /* sps and pps in the avcC always have length coded with 2 bytes,
           so put a fake nal_length_size = 2 while parsing them */
        h->nal_length_size = 2;
        // Decode sps from avcC
        cnt = *(p + 5) & 0x1f; // Number of sps
        p += 6;

        for (i = 0; i < cnt; i++)
        {
            nalsize = AV_RB16(p) + 2;

            if (decode_nal_units(h, p, nalsize) < 0)
            {
                //              av_log(avctx, AV_LOG_ERROR, "Decoding sps %d from avcC failed\n", i);
                av_log(avctx, AV_LOG_WARNING, "Decoding sps %d from avcC failed\n", i);
                return -1;
            }

            p += nalsize;
        }

        // Decode pps from avcC
        cnt = *(p++); // Number of pps

        for (i = 0; i < cnt; i++)
        {
            nalsize = AV_RB16(p) + 2;

            if (decode_nal_units(h, p, nalsize)  != nalsize)
            {
                //              av_log(avctx, AV_LOG_ERROR, "Decoding pps %d from avcC failed\n", i);
                av_log(avctx, AV_LOG_WARNING, "Decoding pps %d from avcC failed\n", i);
                return -1;
            }

            p += nalsize;
        }

        // Now store right nal length size, that will be use to parse all other nals
        h->nal_length_size = ((*(((char *)(avctx->extradata)) + 4)) & 0x03) + 1;
        // Do not reparse avcC
        h->got_avcC = 1;
    }

    /* avc: ��һ�ε���ʱ�����s->avctx->extradata�е�sps��pps���н��� */
    if (!h->got_avcC && !h->is_avc && s->avctx->extradata_num)
    {
        int i;

        for (i = 0; i < s->avctx->extradata_num; i++)
        {
            /* ����sps��pps */
            if (decode_nal_units(h, s->avctx->extradata[i], s->avctx->extradata_size[i]) < 0)
            { return -1; }
        }

        h->got_avcC = 1; /* ���ú�����λ��־λ */
    }

    /* ����nal, �������õ����ֽ��� */
    buf_index = decode_nal_units(h, buf, buf_size);

    if (buf_index <= 0)
    { return -1; }

    /* �����֧��CODEC_FLAG2_CHUNKS���ܶ��ҵ�ǰͼ��ָ��Ϊ�գ��򷵻� */
    if (!(s->flags2 & CODEC_FLAG2_CHUNKS) && !s->current_picture_ptr)
    {
        if (avctx->skip_frame >= AVDISCARD_NONREF || s->hurry_up) { return 0; } /* ���������֡���ܣ����������أ����򱨴����쳣���� */

        //      av_log(avctx, AV_LOG_ERROR, "no frame!\n");
        av_log(avctx, AV_LOG_WARNING, "No frame output currently!\n");

        //      return -1;
        if (FIELD_PICTURE)
        {
            s->avctx->uiFldCnt++;
        }
        else
        {
            s->avctx->uiDiscardFrames++;
        }

        return 1;
    }

    /* �����֧��CODEC_FLAG2_CHUNKS���ܻ�ǰ����ĸ߶ȴ��ڵ������ͼ��߶�ʱ�����ͼ�� */
    if (!(s->flags2 & CODEC_FLAG2_CHUNKS) || (s->mb_y >= s->mb_height && s->mb_height))
    {
        Picture *out = s->current_picture_ptr;
        Picture *cur = s->current_picture_ptr;
        /* 2010/06/29 19:30:00 liuxw+00139685 */
        /* ע�͵����ڴ����޸ĵ���һЩ���õ���ʱ������������ʱ����tmp_count */
        //      int i, pics, cross_idr, out_of_order, out_idx;
        int i, out_idx;
        int tmp_count = 0;

        s->mb_y = 0;

        s->current_picture_ptr->qscale_type = FF_QSCALE_TYPE_H264;
        s->current_picture_ptr->pict_type = s->pict_type;

        /* 2010/05/8 18:00:00 liuxw+00139685 */
        /* �����ǰ�ǳ����룬����ΪIDR���׳�ΪP,����֡����ΪIDR */
        if (s->current_picture_ptr->key_frame)
        {
            s->current_picture_ptr->pict_type = FF_I_TYPE;
        }

        /* Ӳ�����빦�ܣ��ݲ�֧�� */
        if (CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
            ;//ff_vdpau_h264_set_reference_frames(s);

        /* ��ǰͼ��Ϊ�ο�ͼ����ִ��marking��Ĳο����� */
        if (!s->dropable)
        {
            /* 2010/06/29 20:30:00 liuxw+00139685 */
            /* �����ܲο�֡�ĸ��� */
            if (FIELD_PICTURE && s->first_field || !FIELD_PICTURE)
            {
                h->total_ref_count = h->short_ref_count + h->long_ref_count;
            }

            execute_ref_pic_marking(h, h->mmco, h->mmco_index);
            h->prev_poc_msb = h->poc_msb;
            h->prev_poc_lsb = h->poc_lsb;

            /* 2010/08/24 14:00:00 liuxw+00139685 */
            /* ��slice_table��slice_num��ֵ */
            cur->slice_num[s->picture_structure == PICT_BOTTOM_FIELD] = h->slice_num;
            memcpy(cur->slice_table, h->slice_table, cur->slice_table_size);
        }

        h->prev_frame_num_offset = h->frame_num_offset;
        h->prev_frame_num = h->frame_num;

        /* Ӳ�����빦�ܣ��ݲ�֧�� */
        if (avctx->hwaccel)
        {
            if (avctx->hwaccel->end_frame(avctx) < 0)
                //              av_log(avctx, AV_LOG_ERROR, "hardware accelerator failed to decode picture\n");
            { av_log(avctx, AV_LOG_WARNING, "hardware accelerator failed to decode picture\n"); }
        }

        /* Ӳ�����빦�ܣ��ݲ�֧�� */
        if (CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)
            ; //ff_vdpau_h264_picture_complete(s);

        /*
         * FIXME: Error handling code does not seem to support interlaced
         * when slices span multiple rows
         * The ff_er_add_slice calls don't work right for bottom
         * fields; they cause massive erroneous error concealing
         * Error marking covers both fields (top and bottom).
         * This causes a mismatched s->error_count
         * and a bad error table. Further, the error count goes to
         * INT_MAX when called for bottom field, because mb_y is
         * past end by one (callers fault) and resync_mb_y != 0
         * causes problems for the first MB line, too.
         */
#ifdef LXW_DEBUG

        if (frame_num == TARGET_FRAME)
        {
            int i , j ;
            unsigned char *ptr = cur->data[0];
            int linesize = cur->linesize[0];
            ptr += MB_Y * linesize * 16 + MB_X * 16;

            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < 16; j++)
                {
                    av_log(h->s.avctx, AV_LOG_WARNING, "%3d ", ptr[j]);
                }

                av_log(h->s.avctx, AV_LOG_WARNING, "\n");
                ptr += linesize;
            }
        }

#endif

        if (!FIELD_PICTURE)
        { ff_er_frame_end(s); }  /* �������� */

        /* ��ͼ���Ƚ���padding������һЩ������ֵ */
        MPV_frame_end(s);

        /* 2010/04/15 18:30:00 liuxw+00139685 [AZ1D01992] */
        /* ÿ����һ��picture[frame/field],�����﷨Ԫ��interlaced_frame */
        cur->interlaced_frame = FIELD_PICTURE;

        /* 2010/05/10 15:00:00 liuxw+00139685 */
        /* store error code for every frame */
        cur->iErrorCode	|= s->avctx->iErrorCode;

        /* һ֡���Ƿ�������������ϣ���Ϊһ�λ����һ֡�� */
        /* 2010/08/09 17:30:00 liuxw+00139685 */
        /* ����һ���ж������������ǰΪFRAME����ôֱ�������� */
        //		if (cur->field_poc[0]==INT_MAX || cur->field_poc[1]==INT_MAX)
        if (FIELD_PICTURE && (cur->field_poc[0] == INT_MAX || cur->field_poc[1] == INT_MAX))
        {
            /* Wait for second field. */
            *data_size = 0; /* ����һ��������� */
        }
        else
        {
#ifdef LXW_DEBUG
            {
                char frame_type[3][10] = {"I frame", "P frame", "B frame"};

                if (FIELD_PICTURE)
                { av_log(h->s.avctx, AV_LOG_WARNING, "Frame Num: %d Cur Poc: %d Field[0]: %d Field[1]: %d Frame Type: %s reference: %d Interlaced frame!\n", frame_num, cur->poc, cur->field_poc[0], cur->field_poc[1], frame_type[cur->pict_type - 1], cur->reference); }
                else
                { av_log(h->s.avctx, AV_LOG_WARNING, "Frame Num: %d Cur Poc: %d Frame Type: %s reference: %d Progressive frame!\n", frame_num, cur->poc, frame_type[cur->pict_type - 1], cur->reference); }
            }

            if (frame_num == TARGET_FRAME)
            {
                int i , j ;
                unsigned char *ptr = cur->data[0];
                int linesize = cur->linesize[0];
                ptr += MB_Y * linesize * 16 + MB_X * 16;

                for (i = 0; i < 16; i++)
                {
                    for (j = 0; j < 16; j++)
                    {
                        av_log(h->s.avctx, AV_LOG_WARNING, "%3d ", ptr[j]);
                    }

                    av_log(h->s.avctx, AV_LOG_WARNING, "\n");
                    ptr += linesize;
                }
            }


            frame_num++;
#endif

            cur->repeat_pict = 0;

            /* 2010/06/07 11:30:00 liuxw+00139685 */
            /* ��ֹ����������ǰ֡(��������)�Ѿ�������ϣ���������0 first_field�������ں���֡����ʱ������first_field=1������û�н�s->current_picturc_ptr=NULL */
            s->first_field = 0;

            /* Signal interlacing information externally. */
            /* Prioritize picture timing SEI information over used decoding process if it exists. */
            if (h->sps.pic_struct_present_flag)
            {
                switch (h->sei_pic_struct)
                {
                    case SEI_PIC_STRUCT_FRAME:
                        cur->interlaced_frame = 0;
                        break;

                    case SEI_PIC_STRUCT_TOP_FIELD:
                    case SEI_PIC_STRUCT_BOTTOM_FIELD:
                    case SEI_PIC_STRUCT_TOP_BOTTOM:
                    case SEI_PIC_STRUCT_BOTTOM_TOP:
                        cur->interlaced_frame = 1;
                        break;

                    case SEI_PIC_STRUCT_TOP_BOTTOM_TOP:
                    case SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM:
                        // Signal the possibility of telecined film externally (pic_struct 5,6)
                        // From these hints, let the applications decide if they apply deinterlacing.
                        cur->repeat_pict = 1;
                        /* 2010/04/46 13:30:00 liuxw+00139685 [AZ1D01991] */
                        /* ����ǰΪ֡��MBAFFʱ����Ϊprogressive;��Ϊ������Ϊinterlace */
                        //                  cur->interlaced_frame = FIELD_OR_MBAFF_PICTURE;
                        cur->interlaced_frame = FIELD_PICTURE;
                        break;

                    case SEI_PIC_STRUCT_FRAME_DOUBLING:
                        // Force progressive here, as doubling interlaced frame is a bad idea.
                        cur->interlaced_frame = 0;
                        cur->repeat_pict = 2;
                        break;

                    case SEI_PIC_STRUCT_FRAME_TRIPLING:
                        cur->interlaced_frame = 0;
                        cur->repeat_pict = 4;
                        break;
                }
            }
            else
            {
                /* Derive interlacing flag from used decoding process. */
                /* 2010/04/46 13:30:00 liuxw+00139685 [AZ1D01991] */
                /* ����ǰΪ֡��MBAFFʱ����Ϊprogressive;��Ϊ������Ϊinterlace */
                //                cur->interlaced_frame = FIELD_OR_MBAFF_PICTURE;
                cur->interlaced_frame = FIELD_PICTURE;
            }

            /* 2010/08/11 15:30:00 liuxw+00139685 */
            /* ����sps��VUI��Ϣ���ܻ��interlaced���иı䣬�����VUI�ݶԽ���û���κΰ���������ǿ�ư�interlaced�Ļ��� */
            cur->interlaced_frame = FIELD_PICTURE;

            if (cur->field_poc[0] != cur->field_poc[1])
            {
                /* Derive top_field_first from field pocs. */
                cur->top_field_first = cur->field_poc[0] < cur->field_poc[1];
            }
            else
            {
                if (cur->interlaced_frame || h->sps.pic_struct_present_flag)
                {
                    /* Use picture timing SEI information. Even if it is a information of a past frame, better than nothing. */
                    if (h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM || h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM_TOP)
                    { cur->top_field_first = 1; }
                    else
                    { cur->top_field_first = 0; }
                }
                else
                {
                    /* Most likely progressive */
                    cur->top_field_first = 0;
                }
            }

            /* 2010/06/29 18:30:00 liuxw+00139685 */
            /* ע�͵���ǰ������ĵ��� */
#if 0

            //FIXME do something with unavailable reference frames
            /* Sort B-frames into display order */
            /* VUI���﷨Ԫ�أ�bitstream_restriction_flag->�����������ƣ�num_reorder_frames->ͼ�����ǰ��洢ͼ��������Ŀ */
            if (h->sps.bitstream_restriction_flag && s->avctx->has_b_frames < h->sps.num_reorder_frames)
            {
                s->avctx->has_b_frames = h->sps.num_reorder_frames;
                s->low_delay = 0;
            }

            /* strict_std_compliance ����Э����ϸ�̶� */
            if (   s->avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT && !h->sps.bitstream_restriction_flag)
            {
                s->avctx->has_b_frames = MAX_DELAYED_PIC_COUNT; /* ����MAX_DELAYED_PIC_COUNT��ſ�ʼ��� */
                s->low_delay = 0;
            }

            pics = 0;

            /* delay buffer�л���YUV����Ŀ */
            while (h->delayed_pic[pics]) { pics++; }

            assert(pics <= MAX_DELAYED_PIC_COUNT);
            /* ����ǰ֡�ŵ�delay buffer�е�һ��Ϊ�յ�λ��ȥ */
            h->delayed_pic[pics++] = cur;

            if (cur->reference == 0)	/* ����ǰ֡�����ο�����reference�����޸�ΪDELAYED_PIC_REF */
            { cur->reference = DELAYED_PIC_REF; }

            out = h->delayed_pic[0];
            out_idx = 0;

            /* ��delay buffer��Ѱ��POC��С��һ֡��Ϊ��ǰ��� */
            /* �˳�������1������i��delay bufferΪ�� 2����i��delay buffer��poc��0����poc�ﵽ������IDR֡��
               3����i��delay bufferΪIDR֡���ڶ��㶼�Ѿ������˵����㣩4��POC�ﵽ���ֵ�����¿�ʼ�����Ҳ�Ϊ0��������*/
            /* 2010/05/31 10:00:00 liuxw+00139685 */
            /* ���Ӷ�i�ķ�Χ�����ƣ���ֹ���� */
            for (i = 1; i < (s->avctx->has_b_frames + 1) && h->delayed_pic[i] && (h->delayed_pic[i]->poc && !h->delayed_pic[i]->key_frame) && ((out->poc - h->delayed_pic[i]->poc) < INT32_MAX); i++)
                if (h->delayed_pic[i]->poc < out->poc)
                {
                    out = h->delayed_pic[i];
                    out_idx = i;
                }

            /* cross_idr���壺������ǰdelay buffer���Ƿ����IDR֡����poc�ﵽ�����Ϊ0��֡����1���������� 0�����������ڣ�
               cross_idrΪ���������1��delay buffer�е�һ֡��POC=0����poc�ﵽ������IDR֡����2������for�˳�����Ϊ�ڶ���������
               3��delay buffer�е�һ֡ΪIDR֡(��һ��Ӧ���Ѿ����������㣩 */
            cross_idr = !h->delayed_pic[0]->poc || !!h->delayed_pic[i] || h->delayed_pic[0]->key_frame;
            /* out_of_order���壺��ȥ����1���Ƿ����delay buffer��POC��С��֡��POC���ϴ����֡��POCҪС��1�����ڣ����ֵĸ��ʽ�С��0�������ڣ�
               out_of_orderΪ�������(�޸ĺ�һֱΪ��)[1��2ͬʱ�����ݣ�1����ǰdelay buffer�в���IDR֡����POC�ﵽ�����Ϊ0��֡
               2��delay buffer��POC��С��֡��POC���ϴ����֡��POCҪС */
            out_of_order = !cross_idr && out->poc < h->outputed_poc;

            /* ����������Ѿ��ṩ�����delay֡��MAX_DELAY�����Ȼ���MAX_DELAY,����MAX_DELAY��1֡ʱ�������1֡ */
            if (h->sps.bitstream_restriction_flag && s->avctx->has_b_frames >= h->sps.num_reorder_frames)
            { }
            /* ��������û���ṩ���delay֡������������������ʱ��delay֡������1����ǰ֡�������YUV���������㣬�����delay buffer��poc��С����һ֡��
               ִ��������1��delay buffer��POC��С��֡��POC���ϴ����֡��POCҪС 2��[a &&((b&&c)||d)��������ֻ�����һ��] a.����ʱ��־Ϊ1;
               b.��ǰdelay buffer���ǲ�����IDR֡����poc�ﵽ�����Ϊ0��֡;c.��ǰdelay buffer��POC��С��֡���ϴ����֡�������һ֡;d.��ǰ֡ΪB֡
               3��[a&&b&&c] a.��ǰ֡ΪB֡�ҿ������ο���b.��ǰdelay buffer���ǲ�����IDR֡����poc�ﵽ�����Ϊ0��֡;c.��ǰdelay buffer��POC��С��֡���ϴ����֡�������һ֡ */
            /* 2010/04/23 16:00:00 liuxw+00139685 */
            /* ����һ���������������ǰ֡��B֡�����ο������ҵ�ǰ֡��POCС��0����Ҫ�����POCС��0 */
            else if ((out_of_order && pics - 1 == s->avctx->has_b_frames && s->avctx->has_b_frames < MAX_DELAYED_PIC_COUNT)
                     || (s->low_delay && ((!cross_idr && out->poc > h->outputed_poc + 2)
                                          || cur->pict_type == FF_B_TYPE))  || (cur->pict_type == FF_B_TYPE && h->nal_ref_idc &&
                                                  ((!cross_idr && out->poc > h->outputed_poc + 2) || (out->poc < 0 && h->outputed_poc < 0 && out->poc > h->outputed_poc + 2) ||
                                                   (out->poc < 0 && h->outputed_poc > 0 && s->current_picture_ptr->poc < 0 ))))
            {
                /* ϵͳ������ʱ������ϵͳ��ʱ��־ */
                s->low_delay = 0;
                /* ��ʱ���֡����Ŀ��1 */
                s->avctx->has_b_frames++;
            }

            /* 2010/05/6 15:00:00 liuxw+00139685 */
            /* ����ǰdelay buffer���ˣ���ǿ�����buffer��POC��С��һ֡ */
            if (s->avctx->has_b_frames > MAX_DELAYED_PIC_COUNT)
            {
                out_of_order = 0;
                out->reference = 0;
                s->avctx->has_b_frames--;
            }

            /* out_of_orderһֱΪ�٣�����֡������򽫵�ǰ���֡�����֡��ǰ�� */
            if (out_of_order || pics > s->avctx->has_b_frames)
            {
                /* ����ǰ���ͼ�����ο�������Ȼ���ο���������ΪDELAYED_PIC_REF������Ϊ0���Ժ���buffer��ȥ�����Ѿ�����Ҳ����ο���*/
                out->reference &= ~DELAYED_PIC_REF;

                for (i = out_idx; h->delayed_pic[i]; i++)
                { h->delayed_pic[i] = h->delayed_pic[i + 1]; }

                /* 2010/06/11 10:30:00 liuxw+00139685 */
                /* �����Ϊ���򣬽������������һ֡����ʱӦ�ý�delay��֡��Ҳ��Ӧ�ļ��� */
                if (out_of_order)
                {
                    s->avctx->has_b_frames--;
                }
            }

            /* ���һ֡YUV */
            if (!out_of_order && pics > s->avctx->has_b_frames)
            {
                *data_size = sizeof(AVFrame);

                /* 2010/04/46 13:30:00 liuxw+00139685 [AZ1D01991] */
                /* ����ǰ�Ĵ����ȥ������ǰ����ÿһ֡��interlaced_frame���и�ֵ */
                //				out->interlaced_frame = s->picture_structure; /* ���֡���� liuxw+00139685 */

                h->outputed_poc = out->poc;
                *pict = *(AVFrame *)out;
                /* ���ȥ��crop���YUV���� */
                {
                    int SubHeightC[] = {1, 2, 1, 1};
                    int SubWidthC[]  = {1, 2, 2, 1};

                    pict->data[0] = pict->data[0] + pict->linesize[0] * h->sps.crop_top * SubHeightC[ h->sps.chroma_format_idc] * ( 2 -  h->sps.frame_mbs_only_flag ) + h->sps.crop_left * SubWidthC[h->sps.chroma_format_idc];
                    pict->data[1] = pict->data[1] + pict->linesize[1] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
                    pict->data[2] = pict->data[2] + pict->linesize[2] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
                }
                /* 2010/05/26 15:00:00 liuxw+00139685 [AZ1D02104] */
                /* ����ӡdebug��Ϣ�ĺ�����ǰ������������������ */
                assert(pict->data[0] || !*data_size);
                ff_print_debug_info(s, pict);

            }
            else
            {
                /* 2010/30/06 11:10:00 liuxw+00139685 */
                /* ����ӡ����־��Ϣ�����޸��� */
                //av_log(avctx, AV_LOG_DEBUG, "no picture\n");
                av_log(avctx, AV_LOG_DEBUG, "Current picture[POC:%d] delayed!\n", cur->poc);
            }
        }
    }

#else

            if (!cur->poc && cur->key_frame)
            {
                h->display_pic_num = 0;

                while (h->delay_pic_num)
                {
                    out = h->delayed_pic[0];
                    out_idx = 0;

                    for (i = 1; i < h->delay_pic_num && (h->delayed_pic[i]->poc && !h->delayed_pic[i]->key_frame); i++)
                    {
                        if (h->delayed_pic[i]->poc < out->poc)
                        {
                            out = h->delayed_pic[i];
                            out_idx = i;
                        }
                    }


                    for (i = out_idx; i < h->delay_pic_num + 1; i++)
                    {
                        h->delayed_pic[i] = h->delayed_pic[i + 1];
                    }

                    h->delay_pic_num--;
                    out->reference = 0;
                    h->display_pic[h->display_pic_num++] = out;

                }

                h->delay_pic_ref_num = 0;
                h->delay_pic_num = 0;
                //for(i=0; i < h->delay_pic_num; i++)
                //{
                //	if(h->delayed_pic[i]->reference & 3)
                //	{
                //		h->delay_pic_ref_num++;
                //	}
                //}
            }

            while (cur->poc && (h->delay_pic_num - h->delay_pic_ref_num - h->display_pic_ref_num + tmp_count + h->total_ref_count) >= MAX_DELAYED_PIC_COUNT)
            {

                out = h->delayed_pic[0];
                out_idx = 0;

                /* 2010/08/09 19:30:00 liuxw+00139685 */
                /* �޸��ж�������ֻ��POC!=0�����˳� */
                for (i = 1; i < h->delay_pic_num && /*!(!*/h->delayed_pic[i]->poc /*&& h->delayed_pic[i]->key_frame)*/ && !h->delayed_pic[i]->is_output; i++)
                {
                    if (h->delayed_pic[i]->poc < out->poc)
                    {
                        out = h->delayed_pic[i];
                        out_idx = i;
                    }
                }

                if (out->reference & 3)
                {
                    h->delay_pic_ref_num--;
                    h->display_pic_ref_num++;
                }
                else
                {
                    out->reference = 0;
                    tmp_count--;
                }

                h->delay_pic_num--;
                tmp_count++;
                out->is_output = 1;
                h->display_pic[h->display_pic_num++] = out;

                for (i = out_idx; i < h->delay_pic_num + 1; i++)
                {
                    h->delayed_pic[i] = h->delayed_pic[i + 1];
                }

            }

            h->delay_pic_ref_num = 0;

            for (i = 0; i < h->delay_pic_num; i++)
            {
                if (h->delayed_pic[i]->reference & 3)
                {
                    h->delay_pic_ref_num++;
                }
            }

            h->total_ref_count = h->short_ref_count + h->long_ref_count;

            while (h->delay_pic_num >= MAX_DELAYED_PIC_COUNT)
            {
                h->delay_pic_num--;
                h->display_pic[h->display_pic_num++] = h->delayed_pic[0];

                if (h->delayed_pic[0]->reference > 0 && h->delayed_pic[0]->reference < 4)
                {
                    h->delay_pic_ref_num--;
                    h->display_pic_ref_num++;
                }

                for (i = 0; i < h->delay_pic_num + 1; i++)
                {
                    h->delayed_pic[i] = h->delayed_pic[i + 1];
                }
            }

            h->delayed_pic[h->delay_pic_num] = cur;
            h->delay_pic_num++;

            if (cur->reference)
            {
                h->delay_pic_ref_num++;
            }
            else
            {
                cur->reference = DELAYED_PIC_REF;
            }

            if (h->display_pic_num)
            {
                out = h->display_pic[0];
                h->display_pic_num--;

                if (out->reference && out->reference != DELAYED_PIC_REF)
                {
                    h->display_pic_ref_num--;
                }

                if (out->reference == DELAYED_PIC_REF)
                {
                    out->reference = 0;
                }

#ifdef LXW_DEBUG
                {
                    char frame_type[3][10] = {"I frame", "P frame", "B frame"};
                    av_log(h->s.avctx, AV_LOG_WARNING, "output_num: %d poc: %d frame_type: %s frame_num:%d\n", output_num, out->poc, frame_type[out->pict_type - 1], cur->coded_picture_number);
                    output_num++;
                }
#endif

                for (i = 0; i < h->display_pic_num; i++)
                {
                    h->display_pic[i] = h->display_pic[i + 1];
                }

                *data_size = sizeof(AVFrame);
                *pict = *(AVFrame *)out;
                /* ���ȥ��crop���YUV���� */
                {
                    int SubHeightC[] = {1, 2, 1, 1};
                    int SubWidthC[]  = {1, 2, 2, 1};

                    pict->data[0] = pict->data[0] + pict->linesize[0] * h->sps.crop_top * SubHeightC[ h->sps.chroma_format_idc] * ( 2 -  h->sps.frame_mbs_only_flag ) + h->sps.crop_left * SubWidthC[h->sps.chroma_format_idc];
                    pict->data[1] = pict->data[1] + pict->linesize[1] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
                    pict->data[2] = pict->data[2] + pict->linesize[2] * h->sps.crop_top * (2 - h->sps.frame_mbs_only_flag) + h->sps.crop_left;
                }
                /* 2010/05/26 15:00:00 liuxw+00139685 [AZ1D02104] */
                /* ����ӡdebug��Ϣ�ĺ�����ǰ������������������ */
                assert(pict->data[0] || !*data_size);
                ff_print_debug_info(s, pict);
            }

        }
    }

#endif

    //  assert(pict->data[0] || !*data_size);
    /* ��ӡ��ǰ��֡�������Ϣ(Ĭ���ǹرգ� */
    //    ff_print_debug_info(s, pict);
#if 0 //?

    /* Return the Picture timestamp as the frame number */
    /* we subtract 1 because it is added on utils.c     */
    avctx->frame_number = s->picture_number - 1;
#endif

    /* ���ؽ��뵱ǰ֡�����ѵ��ֽ��� */
    return get_consumed_bytes(s, buf_index, buf_size);
}
#if 0
static inline void fill_mb_avail(H264Context *h)
{
    MpegEncContext *const s = &h->s;
    const int mb_xy = s->mb_x + s->mb_y * s->mb_stride;

    if (s->mb_y)
    {
        h->mb_avail[0] = s->mb_x                 && h->slice_table[mb_xy - s->mb_stride - 1] == h->slice_num;
        h->mb_avail[1] =                            h->slice_table[mb_xy - s->mb_stride    ] == h->slice_num;
        h->mb_avail[2] = s->mb_x + 1 < s->mb_width && h->slice_table[mb_xy - s->mb_stride + 1] == h->slice_num;
    }
    else
    {
        h->mb_avail[0] =
            h->mb_avail[1] =
                h->mb_avail[2] = 0;
    }

    h->mb_avail[3] = s->mb_x && h->slice_table[mb_xy - 1] == h->slice_num;
    h->mb_avail[4] = 1; //FIXME move out
    h->mb_avail[5] = 0; //FIXME move out
}
#endif

#ifdef TEST
#undef printf
#undef random
#define COUNT 8000
#define SIZE (COUNT*40)
int main(void)
{
    int i;
    uint8_t temp[SIZE];
    PutBitContext pb;
    GetBitContext gb;
    //    int int_temp[10000];
    DSPContext dsp;
    AVCodecContext avctx;

    dsputil_init(&dsp, &avctx);

    init_put_bits(&pb, temp, SIZE);

    for (i = 0; i < COUNT; i++)
    {
        START_TIMER
        set_ue_golomb(&pb, i);
        STOP_TIMER("set_ue_golomb");
    }

    flush_put_bits(&pb);

    init_get_bits(&gb, temp, 8 * SIZE);

    for (i = 0; i < COUNT; i++)
    {
        int j, s;

        s = show_bits(&gb, 24);

        START_TIMER
        j = get_ue_golomb(&gb);

        if (j != i)
        {
        }

        STOP_TIMER("get_ue_golomb");
    }


    init_put_bits(&pb, temp, SIZE);

    for (i = 0; i < COUNT; i++)
    {
        START_TIMER
        set_se_golomb(&pb, i - COUNT / 2);
        STOP_TIMER("set_se_golomb");
    }

    flush_put_bits(&pb);

    init_get_bits(&gb, temp, 8 * SIZE);

    for (i = 0; i < COUNT; i++)
    {
        int j, s;

        s = show_bits(&gb, 24);

        START_TIMER
        j = get_se_golomb(&gb);

        if (j != i - COUNT / 2)
        {
        }

        STOP_TIMER("get_se_golomb");
    }

#if 0
    DCTELEM block[16];
    uint8_t src[16], ref[16];
    uint64_t error = 0, max_error = 0;

    for (i = 0; i < COUNT; i++)
    {
        int j;

        for (j = 0; j < 16; j++)
        {
            ref[j] = random() % 255;
            src[j] = random() % 255;
        }

        h264_diff_dct_c(block, src, ref, 4);

        //normalize
        for (j = 0; j < 16; j++)
        {
            block[j] = block[j] * 4;

            if (j & 1) { block[j] = (block[j] * 4 + 2) / 5; }

            if (j & 4) { block[j] = (block[j] * 4 + 2) / 5; }
        }

        s->dsp.h264_idct_add(ref, block, 4);

        for (j = 0; j < 16; j++)
        {
            int diff = FFABS(src[j] - ref[j]);

            error += diff * diff;
            max_error = FFMAX(max_error, diff);
        }
    }

    for (qp = 0; qp < 52; qp++)
    {
        for (i = 0; i < 16; i++)
        { src1_block[i] = src2_block[i] = random() % 255; }

    }

    uint8_t bitstream[COUNT];
    uint8_t nal[COUNT * 2];
    H264Context h;
    memset(&h, 0, sizeof(H264Context));

    for (i = 0; i < COUNT; i++)
    {
        int zeros = i;
        int nal_length;
        int consumed;
        int out_length;
        uint8_t *out;
        int j;

        for (j = 0; j < COUNT; j++)
        {
            bitstream[j] = (random() % 255) + 1;
        }

        for (j = 0; j < zeros; j++)
        {
            int pos = random() % COUNT;

            while (bitstream[pos] == 0)
            {
                pos++;
                pos %= COUNT;
            }

            bitstream[pos] = 0;
        }

        START_TIMER

        nal_length = encode_nal(&h, nal, bitstream, COUNT, COUNT * 2);

        if (nal_length < 0)
        {
            return -1;
        }

        out = ff_h264_decode_nal(&h, nal, &out_length, &consumed, nal_length);

        STOP_TIMER("NAL")

        if (out_length != COUNT)
        {
            return -1;
        }

        if (consumed != nal_length)
        {
            return -1;
        }

        if (memcmp(bitstream, out, COUNT))
        {
            return -1;
        }
    }

#endif

    return 0;
}
#endif /* TEST */


static av_cold int decode_end(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;
    MpegEncContext *s = &h->s;
    int i;

    av_freep(&h->rbsp_buffer[0]);
    av_freep(&h->rbsp_buffer[1]);
    free_tables(h); //FIXME cleanup init stuff perhaps

    for (i = 0; i < MAX_SPS_COUNT; i++)
    { av_freep(h->sps_buffers + i); }

    for (i = 0; i < MAX_PPS_COUNT; i++)
    { av_freep(h->pps_buffers + i); }

    MPV_common_end(s);

    //    memset(h, 0, sizeof(H264Context));

    return 0;
}


/* ������������λh264������ */
static av_cold int h264_reset(H264Context *h)
{
    int iRet = 0;

    MpegEncContext *const	s		= &h->s;
    AVCodecContext 		*avctx	= s->avctx;
    //	int i;

    /* ���H264Context�ṹ����� */
    memset(h, 0, sizeof(H264Context));

    /* ��ʼ��H264Context�ṹ����� */
    iRet = decode_init(avctx);

    if (0 != iRet)
    {
        av_log(avctx, AV_LOG_ERROR, "ctx_dec[%p] h264_ctx[%p] decode_init() failed! return code: %d !\n", avctx, h, iRet);
    }

#if 0
    /* �������������h264context���ѷ�����ڴ�ָ�� */
    uint8_t *rbsp_buffer[2]				= {h->rbsp_buffer[0], h->rbsp_buffer[1]} ;
    unsigned int rbsp_buffer_size[2]    = { h->rbsp_buffer_size[0], h->rbsp_buffer_size[1]};
    int8_t (*intra4x4_pred_mode)[8]		= h->intra4x4_pred_mode;
    uint8_t (*top_borders[2])[16 + 2 * 8]	= {h->top_borders[0], h->top_borders[1]};
    uint8_t (*non_zero_count)[16]		= h->non_zero_count;
    uint32_t *mb2b_xy					= h->mb2b_xy;
    uint32_t *mb2b8_xy					= h->mb2b8_xy;
    SPS *sps_buffers[MAX_SPS_COUNT];
    PPS *pps_buffers[MAX_PPS_COUNT];
    uint32_t (*dequant4_coeff[6])[16]	= {h->dequant4_coeff[0], h->dequant4_coeff[1], h->dequant4_coeff[2], h->dequant4_coeff[3], \
                                           h->dequant4_coeff[4], h->dequant4_coeff[5], h->dequant4_coeff[6]
                                        };
    uint32_t (*dequant8_coeff[2])[64]	= {h->dequant8_coeff[0], h->dequant8_coeff[2]};
    uint16_t *slice_table_base			= h->slice_table_base;
    uint16_t *slice_table				= h->slice_table;
    GetBitContext *intra_gb_ptr			= h->intra_gb_ptr;
    GetBitContext *inter_gb_ptr			= h->inter_gb_ptr;
    uint16_t     *cbp_table				= h->cbp_table;
    uint8_t     *chroma_pred_mode_table = h->chroma_pred_mode_table;
    int16_t     (*mvd_table[2])[2]		= {h->mvd_table[0], h->mvd_table[1]};
    uint8_t     *direct_table			= h->direct_table;
    const uint8_t *zigzag_scan_q0		= h->zigzag_scan_q0;
    const uint8_t *zigzag_scan8x8_q0	= h->zigzag_scan8x8_q0;
    const uint8_t *zigzag_scan8x8_cavlc_q0 = h->zigzag_scan8x8_cavlc_q0;
    struct H264Context *thread_context[MAX_THREADS];

    for (i = 0; i < MAX_SPS_COUNT; i++)
    {
        sps_buffers[i] = h->sps_buffers[i];
    }

    for (i = 0; i < MAX_PPS_COUNT; i++)
    {
        pps_buffers[i] = h->pps_buffers[i];
    }

    for (i = 0; i < MAX_THREADS; i++)
    {
        thread_context[i] = h->thread_context[i];
    }

    /* ���H264Context */
    memset(&(h->nal_ref_idc), 0, sizeof(H264Context) - sizeof(MpegEncContext));

    /* h264��������ʼ�� */
    iRet = decode_init(s->avctx);

    if (0 != iRet)
    {
        av_log(s->avctx, AV_LOG_ERROR, "ctx_dec[%p] h264_ctx[%p] decode_init() failed! return code: %d !\n", avctx, h, iRet);
    }

    /* �ָ�֮ǰ�����ָ�� */
    h->rbsp_buffer[0]			= rbsp_buffer[0];
    h->rbsp_buffer[1]			= rbsp_buffer[1];
    h->rbsp_buffer_size[0]		= rbsp_buffer_size[0];
    h->rbsp_buffer_size[1]		= rbsp_buffer_size[1];
    h->intra4x4_pred_mode		= intra4x4_pred_mode;
    h->top_borders[0]			= top_borders[0];
    h->top_borders[1]			= top_borders[1];
    h->non_zero_count			= non_zero_count;
    h->mb2b_xy					= mb2b_xy;
    h->mb2b8_xy					= mb2b8_xy;
    h->slice_table_base			= slice_table_base;
    h->slice_table				= slice_table;
    h->intra_gb_ptr				= intra_gb_ptr;
    h->inter_gb_ptr				= inter_gb_ptr;
    h->cbp_table				= cbp_table;
    h->chroma_pred_mode_table   = chroma_pred_mode_table;
    h->mvd_table[0]				= mvd_table[0];
    h->mvd_table[1]				= mvd_table[1];
    h->direct_table				= direct_table;
    h->zigzag_scan_q0			= zigzag_scan_q0;
    h->zigzag_scan8x8_q0		= zigzag_scan8x8_q0;
    h->zigzag_scan8x8_cavlc_q0	= zigzag_scan8x8_cavlc_q0;
    h->dequant8_coeff[0]		= dequant8_coeff[0];
    h->dequant8_coeff[1]		= dequant8_coeff[1];

    for (i = 0; i < 6; i++)
    {
        h->dequant4_coeff[i] = dequant4_coeff[i];
    }

    for (i = 0; i < MAX_SPS_COUNT; i++)
    {
        h->sps_buffers[i] = sps_buffers[i];
    }

    for (i = 0; i < MAX_PPS_COUNT; i++)
    {
        h->pps_buffers[i] = pps_buffers[i];
    }

    for (i = 0; i < MAX_THREADS; i++)
    {
        h->thread_context[i] = thread_context[i];
    }

#endif

    return iRet;
}

/* ��������(sdk)����λ������ */
static av_cold int decode_reset(AVCodecContext *avctx)
{
    int iRet = 0;

    H264Context *h = avctx->priv_data;
    MpegEncContext *s = &h->s;
    int i;

    /* ���buffer */
    flush_dpb(avctx);

    /* �ͷ��ڴ�(YUV buffer����) */
    av_freep(&h->rbsp_buffer[0]);
    av_freep(&h->rbsp_buffer[1]);
    free_tables(h); //FIXME cleanup init stuff perhaps

    for (i = 0; i < MAX_SPS_COUNT; i++)
    { av_freep(h->sps_buffers + i); }

    for (i = 0; i < MAX_PPS_COUNT; i++)
    { av_freep(h->pps_buffers + i); }

    //	MPV_common_end_hw(s);
    MPV_common_end(s);

    /* ��λ����ʼ��AVCodecContext�ṹ����� */
    iRet = avcodec_reset(avctx);

    if (0 != iRet)
    {
        av_log(avctx, AV_LOG_WARNING, "ctx_dec[%p] avcodec_reset() failed! return code: %d !\n", avctx, iRet);
        return iRet;
    }

    /* ��λ����ʼ��H264Context�ṹ����� */
    iRet = h264_reset(h);

    if (0 != iRet)
    {
        av_log(avctx, AV_LOG_WARNING, "ctx_dec[%p] h264_ctx[%p] h264_reset() failed! return code: %d !\n", avctx, h, iRet);
        return iRet;
    }

    /* ��λ����ʼ��MpegEncContext�ṹ����� */
    /*	iRet = MPV_common_reset(s);
    	if(0 != iRet)
    	{
    		av_log(avctx, AV_LOG_WARNING, "ctx_dec[%p] MPV_common_reset() failed! return code: %d !\n",avctx, iRet);
    		return iRet;
    	}
    */
    return iRet;
}

#if __STDC_VERSION__ >= 199901L
AVCodec h264_decoder =
{
    "h264",
    CODEC_TYPE_VIDEO,
    CODEC_ID_H264,
    sizeof(H264Context),
    decode_init,
    NULL,
    decode_end,
    decode_frame,
    /*CODEC_CAP_DRAW_HORIZ_BAND |*/ CODEC_CAP_DR1 | CODEC_CAP_DELAY,
    .flush = flush_dpb,
    .long_name = NULL_IF_CONFIG_SMALL("H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10"),
    .pix_fmts = ff_hwaccel_pixfmt_list_420,
    .Frame_Parse = H264_Frame_Parse,
    .reset = decode_reset,
};
#else
AVCodec h264_decoder =
{
    "h264",
    CODEC_TYPE_VIDEO,
    CODEC_ID_H264,
    sizeof(H264Context),
    decode_init,
    NULL,
    decode_end,
    decode_frame,
    /*CODEC_CAP_DRAW_HORIZ_BAND |*/ CODEC_CAP_DR1 | CODEC_CAP_DELAY,
    NULL,
    flush_dpb,
    NULL,
    ff_hwaccel_pixfmt_list_420,
    NULL_IF_CONFIG_SMALL("H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10"),
    NULL,
    NULL,
    NULL,
    H264_Frame_Parse,
    decode_reset
};
#endif

#if CONFIG_H264_VDPAU_DECODER
AVCodec h264_vdpau_decoder =
{
    "h264_vdpau",
    CODEC_TYPE_VIDEO,
    CODEC_ID_H264,
    sizeof(H264Context),
    decode_init,
    NULL,
    decode_end,
    decode_frame,
    CODEC_CAP_DR1 | CODEC_CAP_DELAY | CODEC_CAP_HWACCEL_VDPAU,
    .flush = flush_dpb,
    .long_name = NULL_IF_CONFIG_SMALL("H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10 (VDPAU acceleration)"),
};
#endif

#if CONFIG_SVQ3_DECODER
#include "svq3.c"
#endif
