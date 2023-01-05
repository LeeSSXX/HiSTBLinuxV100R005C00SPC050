
/**
 * @file libavcodec/huffman.c
 * huffman tree builder and VLC generator
 * Copyright (c) 2006 Konstantin Shishkov
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

#include "avcodec.h"
#include "Bitstream.h"
#include "huffman.h"
/* symbol for Huffman tree node */
#define HNODE -1



/*��ֲ��˼����qsort sdk 2010 0327*/
#ifdef IMEDIA_LINUX_KERNEL
/*************************************************************************/

/*************************************************************************/
/* ����ԭ��*/
static void shortsort(char *lo, char *hi, size_t width,
                      int ( *comp)(const void *, const void *));
static void swap_kn(char *a, char *b, size_t width);
/* this parameter defines the cutoff between using quick sort and
   insertion sort for arrays; arrays with lengths shorter or equal to the
   below value use insertion sort */

/* �����������������ǣ������������ѭ����������СС��CUTOFF������ʱ����ʹ�ò���

�������������������ͱ����˶�С���������ֶ������Ķ��⿪���������ȡֵ8����

���������Ժ��ܹ�ʱ���������㷨�ﵽ����CUTOFF��ֵ��*/

#define CUTOFF 8            /* testing shows that this is good value */



/* Դ������������qsort�Ĵ��룬�����Ҿ����Ƚ�����qsortҪ���õĺ����Ĺ��ܱȽ�

�á�

    shortsort������

    ������������ã������Ѿ����ᵽ�����ǵ��Կ�������ݹ���õ�ʱ���������

��СС��CUTOFF�����飬�͵�������������������򣬶����Ǽ���������������һ��

�ݹ顣��Ϊ��Ȼ�����õ��ǻ������򷽷�����������ʱ���O(n^2)�ɱ��������������

ֻ��8��Ԫ�أ������ٶȱ���Ҫ�ݹ�Ŀ�������Ҫ��öࡣ���⣬��Դ�����ע���У�˵

����һ����������(insertion sort)�������Ҿ������Ӧ����һ��ѡ������Ŷ�

(selection sort)������Ϊʲô��ѡ����������ò�������Ӧ���Ǻ�ѡ�������Ԫ��

�����������й�ϵ��ֻ��ҪN-1�ν���������������ƽ����Ҫ(N^2)/2�Ρ�֮����Ҫѡ��

���������ٵ��㷨������Ϊ�п�����������ĵ���Ԫ�صĴ�С�ܴ�ʹ�ý�����Ϊ����

Ҫ������ƿ����

    ����˵����

       char *lo;    ָ��Ҫ�����������ĵ�һ��Ԫ�ص�ָ��

       char *hi;    ָ��Ҫ���������������һ��Ԫ�ص�ָ��

       size_t width;  �����е���Ԫ�صĴ�С

       int (__cdecl *comp)(const void *,const void *);   �����Ƚ�����Ԫ�ش�

С�ĺ���ָ�룬������������ڵ���qsort��ʱ����Ĳ�������ǰһ��ָ��ָ���Ԫ��

С�ں�һ��ʱ�����ظ����������ʱ������0��������ʱ������������*/

static void shortsort (
    char *lo,
    char *hi,
    size_t width,
    int (*comp)(const void *, const void *)
)
{
    char *p, *max;

    /* Note: in assertions below, i and j are alway inside original bound of
       array to sort. */

    while (hi > lo)
    {
        /* A[i] <= A[j] for i <= j, j > hi */
        max = lo;

        /*�������forѭ�������Ǵ�lo��hi��Ԫ���У�ѡ������һ����maxָ��ָ����

        �������*/
        for (p = lo + width; p <= hi; p += width)
        {
            /* A[i] <= A[max] for lo <= i < p */
            if (comp(p, max) > 0)
            {
                max = p;
            }

            /* A[i] <= A[max] for lo <= i <= p */
        }

        /* A[i] <= A[max] for lo <= i <= hi */

        /*�����������hiָ������򽻻�*/
        swap_kn(max, hi, width);

        /* A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi */

        /*hi��ǰ�ƶ�һ��ָ�롣������һ������hi��������Ѿ��ź���ı�δ���򲿷�

        ���е���Ҫ�������*/

        hi -= width;

        /* A[i] <= A[j] for i <= j, j > hi, loop top condition established */
    }

    /* A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
       so array is sorted */
}



/*�������swap������

      ��������Ƚϼ򵥣����ǽ���������Ĳ�������������ָ����ʵ�ֵġ�

*/

static void swap_kn (
    char *a,
    char *b,
    size_t width
)
{
    char tmp;

    if ( a != b )

        /* Do the swap one character at a time to avoid potential alignment
           problems. */
        while ( width-- )
        {
            tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
}



/*����������Ҫ�Ĳ��֣�qsort������*/



/*ʹ�õ��Ƿǵݹ鷽ʽ������������һ���Զ����ջʽ�ṹ���������������ջ�Ĵ�С

*/

#define STKSIZ (8*sizeof(void*) - 2)

static void qsort (
    void *base,
    size_t num,
    size_t width,
    int (*comp)(const void *, const void *)
)
{
    /* Note: the number of stack entries required is no more than
       1 + log2(num), so 30 is sufficient for any array */

    /*����ʹ����ĳЩ���ɣ�����ὲ������ʹ��ջ��С�����󲻻����1+log2(num)��

    ���30��ջ��СӦ�����㹻�ˡ�Ϊʲô˵��30�أ���ʵ������STKSIZ�Ķ����п��Լ����sizeof(void*)��4������8*4-2��30*/
    char *lo, *hi;              /* ends of sub-array currently sorting   ����

��������ָ�룬����ָ��������Ͻ���½�*/
    char *mid;                  /* points to middle of subarray  ������м���ָ

��*/
    char *loguy, *higuy;        /* traveling pointers for partition step  ѭ

���е��ζ�ָ��*/
    size_t size;                /* size of the sub-array  ����Ĵ�С*/
    char *lostk[STKSIZ], *histk[STKSIZ];
    int stkptr;                 /* stack for saving sub-array to be processed  ջ��ָ��

*/



    /*���ֻ��һ�������µ�Ԫ�أ����˳�*/

    if (num < 2 || width == 0)
    { return; }                 /* nothing to do */

    stkptr = 0;                 /* initialize stack */

    lo = base;
    hi = (char *)base + width * (num - 1);      /* initialize limits */

    /* this entry point is for pseudo-recursion calling: setting
       lo and hi and jumping to here is like recursion, but stkptr is
       preserved, locals aren't, so we preserve stuff on the stack */

    /*�����ǩ��α�ݹ�Ŀ�ʼ*/
recurse:

    size = (hi - lo) / width + 1;        /* number of el's to sort */

    /* below a certain size, it is faster to use a O(n^2) sorting method */

    /*��sizeС��CUTOFFʱ��ʹ��O(n2)�������㷨����*/
    if (size <= CUTOFF)
    {
        shortsort(lo, hi, width, comp);
    }
    else
    {
        /* First we pick a partitioning element.  The efficiency of the
           algorithm demands that we find one that is approximately the

        median
           of the values, but also that we select one fast.  We choose the
           median of the first, middle, and last elements, to avoid bad
           performance in the face of already sorted data, or data that is

        made
           up of multiple sorted runs appended together.  Testing shows that

        a
           median-of-three algorithm provides better performance than simply
           picking the middle element for the latter case. */

        /*��������Ҫѡ��һ��������㷨�ĸ�Ч��Ҫ�������ҵ�һ�����������м�ֵ

        ���������Ҫ��֤�ܹ��ܿ��ҵ���������ѡ������ĵ�һ��м�������һ�����

        ��ֵ�������������µĵ�Ч�ʡ����Ա�����ѡ�����������м�ֵ���ȵ���ѡ������

        ���м����Ч��Ҫ�ߡ�

         ���ǽ���һ��ΪʲôҪ�����������������⡣�������£����������㷨

        ������ʱ�临�Ӷ���O(n^2)�����������һ���������Ѿ�������ļ����������ѡ����

        ��һ������Ϊ�����Ҳ���������������е��������Ƿ����Ľ���Ƿֳ���һ����

        СΪN-1�������һ����СΪ1�����飬�����Ļ���������Ҫ�ıȽϴ�����N + N-1 + N-2

        + N-3 +...+2+1=(N+1)N/2=O(n^2)�������ѡ��ǰ �� �����������м�ֵ������������

        ����Ҳ�ܹ��õ��ܺõĴ���*/

        mid = lo + (size / 2) * width;      /* find middle element */

        /*��һ�� �м��� �����������Ԫ������*/

        /* Sort the first, middle, last elements into order */
        if (comp(lo, mid) > 0)
        {
            swap_kn(lo, mid, width);
        }

        if (comp(lo, hi) > 0)
        {
            swap_kn(lo, hi, width);
        }

        if (comp(mid, hi) > 0)
        {
            swap_kn(mid, hi, width);
        }

        /* We now wish to partition the array into three pieces, one

        consisting
           of elements <= partition element, one of elements equal to the
           partition element, and one of elements > than it.  This is done
           below; comments indicate conditions established at every step. */

        /*����Ҫ��������������飬һ����С�ڷ�����ģ�һ���ǵ��ڷ�����ģ���

        ��һ���Ǵ��ڷ�����ġ�*/

        /*�����ʼ����loguy �� higuy����ָ�룬����ѭ���������ƶ���ָʾ��Ҫ����������Ԫ�صġ�higuy�ݼ���loguy���������������forѭ�����ǿ�����ֹ��*/

        loguy = lo;
        higuy = hi;

        /* Note that higuy decreases and loguy increases on every iteration,
           so loop must terminate. */
        for (;;)
        {
            /* lo <= loguy < hi, lo < higuy <= hi,
               A[i] <= A[mid] for lo <= i <= loguy,
               A[i] > A[mid] for higuy <= i < hi,
               A[hi] >= A[mid] */

            /* The doubled loop is to avoid calling comp(mid,mid), since some
               existing comparison funcs don't work when passed the same
               value for both pointers. */

            /*��ʼ�ƶ�loguyָ�룬ֱ��A[loguy]>A[mid]*/

            if (mid > loguy)
            {
                do
                {
                    loguy += width;
                }
                while (loguy < mid && comp(loguy, mid) <= 0);
            }

            /*����ƶ���loguy>=mid��ʱ�򣬾ͼ�������ƶ���ʹ��A[loguy]>a

            [mid]����һ��ʵ�������þ���ʹ���ƶ���loguy֮��loguyָ��֮ǰ��Ԫ�ض��ǲ����ڻ���ֵ��Ԫ�ء�*/
            if (mid <= loguy)
            {
                do
                {
                    loguy += width;
                }
                while (loguy <= hi && comp(loguy, mid) <= 0);
            }

            /* lo < loguy <= hi+1, A[i] <= A[mid] for lo <= i < loguy,
               either loguy > hi or A[loguy] > A[mid] */

            /*ִ�е������ʱ��lo<loguy<=hi+1��

              ������lo<=i<loguy����A[i]<=A[mid]��

              ����loguy>hi����������A[loguy]>A[mid]����

             Ҳ����˵��loguyָ��֮ǰ�����A[mid]ҪС���ߵ�����*/



            /*�����ƶ�higuyָ�룬ֱ��A[higuy]<��A[mid]*/

            do
            {
                higuy -= width;
            }
            while (higuy > mid && comp(higuy, mid) > 0);

            /* lo <= higuy < hi, A[i] > A[mid] for higuy < i < hi,
               either higuy == lo or A[higuy] <= A[mid] */



            /*�������ָ�뽻���ˣ����˳�ѭ����*/

            if (higuy < loguy)
            { break; }

            /* if loguy > hi or higuy == lo, then we would have exited, so
               A[loguy] > A[mid], A[higuy] <= A[mid],
               loguy <= hi, higuy > lo */

            /*���loguy>hi ����higuy==lo��������һ��break����Ѿ�������������

            ��������

              ��ˣ���ʱA[loguy]>A[mid],A[higuy]<=A[mid],

             loguy<=hi��higuy>lo��*/

            /*��������ָ��ָ���Ԫ��*/

            swap_kn(loguy, higuy, width);

            /* If the partition element was moved, follow it.  Only need
               to check for mid == higuy, since before the swap,
               A[loguy] > A[mid] implies loguy != mid. */

            /*�������Ԫ�ص�λ���ƶ��ˣ�����Ҫ��������

               ��Ϊ��ǰ���loguy���������ѭ���еĵڶ���ѭ���Ѿ���֤��loguy>mid,

              ��loguyָ�벻��midָ����ȡ�

              ��������ֻ��Ҫ��һ��higuyָ���Ƿ����midָ�룬

             ���ԭ����mid==higuy�����ˣ���ô�����ղŵĽ������м�ֵ���Ѿ�����

            loguyָ���λ�ã�ע�⣺�ղ���ֵ�����ˣ����ǲ�û�н���ָ�롣��higuy��mid��ȣ�����higuy��loguyָ������ݣ�higuy��Ȼ����mid����������mid��loguy�����¸����м�ֵ��*/

            if (mid == higuy)
            { mid = loguy; }

            /* A[loguy] <= A[mid], A[higuy] > A[mid]; so condition at top
               of loop is re-established */

            /*���ѭ��һֱ���е�����ָ�뽻��Ϊֹ*/
        }

        /*     A[i] <= A[mid] for lo <= i < loguy,
               A[i] > A[mid] for higuy < i < hi,
               A[hi] >= A[mid]
               higuy < loguy
           implying:
               higuy == loguy-1
               or higuy == hi - 1, loguy == hi + 1, A[hi] == A[mid] */

        /*��һ��ѭ������֮����Ϊ��û��ִ��loguyָ���higuyָ�����ݵĽ���������loguyָ���ǰ�������Ԫ�ض������ڻ���ֵ����higuyָ��֮�������Ԫ�ض����ڻ���ֵ�����Դ�ʱ�����������

        1)  higuy��loguy��1

        2)  higuy��hi��1��loguy��hi+1

        ���еڶ������������һ��ʼѡ������Ԫ�ص�ʱ��hiָ���Ԫ�غ�midָ���Ԫ��ֵ��ȣ���hiǰ���Ԫ��ȫ���������ڻ���ֵ��ʹ���ƶ�loguyָ���ʱ��һֱ�ƶ�����hi+1��ֹͣ�����ƶ�higuyָ���ʱ��higuyָ���ƶ�һ����ֹͣ�ˣ�ͣ��hi��1����

        */

        /* Find adjacent elements equal to the partition element.  The
           doubled loop is to avoid calling comp(mid,mid), since some
           existing comparison funcs don't work when passed the same value
           for both pointers. */

        higuy += width;

        if (mid < higuy)
        {
            do
            {
                higuy -= width;
            }
            while (higuy > mid && comp(higuy, mid) == 0);
        }

        if (mid >= higuy)
        {
            do
            {
                higuy -= width;
            }
            while (higuy > lo && comp(higuy, mid) == 0);
        }

        /* OK, now we have the following:
              higuy < loguy
              lo <= higuy <= hi
              A[i]  <= A[mid] for lo <= i <= higuy
              A[i]  == A[mid] for higuy < i < loguy
              A[i]  >  A[mid] for loguy <= i < hi
              A[hi] >= A[mid] */

        /*��������Ĵ���higuyָ���֮ǰ�Ķ���С�ڵ���A[mid]��������higuyָ��

        ��loguyָ��֮����ǵ���A[mid]��������loguyָ���֮����Ǵ���A[mid]������ʵ�������ǿ��Կ�����higuyָ��ǰ����Ȼ�����е���A[mid]������������������·����֮��ȷʵ�ܹ���һ���̶��������������Ĵ�С���Ż��˳����Ч�ʡ�*/

        /* We've finished the partition, now we want to sort the subarrays
           [lo, higuy] and [loguy, hi].
           We do the smaller one first to minimize stack usage.
           We only sort arrays of length 2 or more.*/

        /*���������Ѿ�����˷��������Կ�ʼ��������[lo,higuy]��[loguy,hi]������

        ��

          �����ȴ���С���Ǹ����У��������Ա���������ջ��С��N�ɱ��������

        ��

          ���ǿ�������һ�£�����һ���Ѿ���������飬���ÿ�ηֳ�N-1��1�����飬

         ��������ÿ�ζ��ȴ���N-1��һ�룬

         ��ô���ǵĵݹ���Ⱦ��Ǻ�N�ɱ������������ڴ�N��ջ�ռ�Ŀ����Ǻܴ�ġ�

         ����ȴ���1����һ�룬ջ�������ֻ��2�

         ������Ԫ�ظպ��������м�ʱ��ջ�ĳ�����logN��

          ����ջ�Ĳ����������ȰѴ��������Ϣ��ջ��

        */

        if ( higuy - lo >= hi - loguy )
        {
            if (lo < higuy)
            {
                lostk[stkptr] = lo;
                histk[stkptr] = higuy;
                ++stkptr;
            }                           /* save big recursion for later */

            if (loguy < hi)
            {
                lo = loguy;
                goto recurse;           /* do small recursion */
            }
        }
        else
        {
            if (loguy < hi)
            {
                lostk[stkptr] = loguy;
                histk[stkptr] = hi;
                ++stkptr;               /* save big recursion for later */
            }

            if (lo < higuy)
            {
                hi = higuy;
                goto recurse;           /* do small recursion */
            }
        }
    }

    /* We have sorted the array, except for any pending sorts on the stack.
       Check if there are any, and do them. */

    /*��ջ������ֱ��ջΪ�գ��˳�ѭ��*/

    --stkptr;

    if (stkptr >= 0)
    {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;           /* pop subarray from stack */
    }
    else
    { return; }                 /* all subarrays done */
}
#endif

static void get_tree_codes(uint32_t *bits, int16_t *lens, uint8_t *xlat, Node *nodes, int node, uint32_t pfx, int pl, int *pos, int no_zero_count)
{
    int s;

    s = nodes[node].sym;

    if (s != HNODE || (no_zero_count && !nodes[node].count))
    {
        bits[*pos] = pfx;
        lens[*pos] = pl;
        xlat[*pos] = s;
        (*pos)++;
    }
    else
    {
        pfx <<= 1;
        pl++;
        get_tree_codes(bits, lens, xlat, nodes, nodes[node].n0, pfx, pl, pos,
                       no_zero_count);
        pfx |= 1;
        get_tree_codes(bits, lens, xlat, nodes, nodes[node].n0 + 1, pfx, pl, pos,
                       no_zero_count);
    }
}

/*��3�����������̬���䣬��Ϊ�ں˱���ʱջ�ռ���� sdk 20101230*/
static uint32_t bits[256];
static int16_t lens[256];
static uint8_t xlat[256];

static int build_huff_tree(VLC *vlc, Node *nodes, int head, int flags)
{
    int no_zero_count = !(flags & FF_HUFFMAN_FLAG_ZERO_COUNT);
    /*��3�����������̬���䣬��Ϊ�ں˱���ʱջ�ռ���� sdk 20101230*/
    /* uint32_t bits[256];
     int16_t lens[256];
     uint8_t xlat[256];
    */
    int pos = 0;

    get_tree_codes(bits, lens, xlat, nodes, head, 0, 0, &pos, no_zero_count);
    /*sdk 20100928 ���һ������ԭ����0����Ϊ1�������ڴ��ɸ�ָ���Ƿ�Ϊ�վ���*/
    return init_vlc_sparse(vlc, VP6VLCBITS/*9*/, pos, lens, 2, 2, bits, 4, 4, xlat, 1, 1,/*0*/ 1);
}


/**
 * nodes size must be 2*nb_codes
 * first nb_codes nodes.count must be set
 */
int ff_huff_build_tree(AVCodecContext *avctx, VLC *vlc, int nb_codes,
                       Node *nodes, HuffCmp cmp, int flags)
{
    int i, j;
    int cur_node;
    int64_t sum = 0;

    for (i = 0; i < nb_codes; i++)
    {
        nodes[i].sym = i;
        nodes[i].n0 = -2;
        sum += nodes[i].count;
    }

    if (sum >> 31)
    {
        av_log(avctx, AV_LOG_ERROR, "Too high symbol frequencies. Tree construction is not possible\n");
        return -1;
    }

    //qsort(nodes, nb_codes, sizeof(Node), cmp);
    qsort(nodes, nb_codes, sizeof(Node), cmp);
    cur_node = nb_codes;
    nodes[nb_codes * 2 - 1].count = 0;

    for (i = 0; i < nb_codes * 2 - 1; i += 2)
    {
        nodes[cur_node].sym = HNODE;
        nodes[cur_node].count = nodes[i].count + nodes[i + 1].count;
        nodes[cur_node].n0 = i;

        for (j = cur_node; j > 0; j--)
        {
            if (nodes[j].count > nodes[j - 1].count ||
                (nodes[j].count == nodes[j - 1].count &&
                 (!(flags & FF_HUFFMAN_FLAG_HNODE_FIRST) ||
                  nodes[j].n0 == j - 1 || nodes[j].n0 == j - 2 ||
                  (nodes[j].sym != HNODE && nodes[j - 1].sym != HNODE))))
            { break; }

            FFSWAP(Node, nodes[j], nodes[j - 1]);
        }

        cur_node++;
    }

    if (build_huff_tree(vlc, nodes, nb_codes * 2 - 2, flags) < 0)
    {
        av_log(avctx, AV_LOG_ERROR, "Error building tree\n");
        return -1;
    }

    return 0;
}
