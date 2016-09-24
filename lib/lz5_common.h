/*
    LZ5 - Fast LZ compression algorithm 
    Copyright (C) 2011-2015, Yann Collet.
    Copyright (C) 2015-2016, Przemyslaw Skibinski <inikep@gmail.com>

    BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following disclaimer
    in the documentation and/or other materials provided with the
    distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    You can contact the author at :
       - LZ5 source repository : https://github.com/inikep/lz5
*/

#ifndef LZ5_COMMON_H_2983827168210
#define LZ5_COMMON_H_2983827168210

#if defined (__cplusplus)
extern "C" {
#endif

// TO DO:
// sl codewords
// remove if (offset < 8) printf("ERROR 
// improve LZ5_COMPRESSBOUND

/*-************************************
*  Memory routines
**************************************/
#include <stdlib.h>   /* malloc, calloc, free */
#include <string.h>   /* memset, memcpy */
#include "mem.h"
#include "lz5_compress.h"      /* LZ5_GCC_VERSION */


#define LZ5_LOG_COMPRESS(...) //printf(__VA_ARGS__)
#define LZ5_LOG_DECOMPRESS(...) //printf(__VA_ARGS__)

#define LZ5_LOG_COMPRESS_LZ4(...) //printf(__VA_ARGS__)
#define COMPLOG_CODEWORDS_LZ4(...) //printf(__VA_ARGS__)
#define LZ5_LOG_DECOMPRESS_LZ4(...) //printf(__VA_ARGS__)
#define DECOMPLOG_CODEWORDS_LZ4(...) //printf(__VA_ARGS__)

#define LZ5_LOG_COMPRESS_LZ5v2(...) //printf(__VA_ARGS__)
#define COMPLOG_CODEWORDS_LZ5v2(...) //printf(__VA_ARGS__)
#define DECOMPLOG_CODEWORDS_LZ5v2(...) //printf(__VA_ARGS__)
#define LZ5_LOG_DECOMPRESS_LZ5v2(...) //printf(__VA_ARGS__)




/*-************************************
*  Common Constants
**************************************/
#define MINMATCH 4
//#define USE_LZ4_ONLY
//#define USE_8BIT_CODEWORDS

#define WILDCOPYLENGTH 16
#define LASTLITERALS WILDCOPYLENGTH
#define MFLIMIT (WILDCOPYLENGTH+MINMATCH)

#define LZ5_MAX_PRICE           (1<<28)
#define LZ5_INIT_LAST_OFFSET    0
#define LZ5_MAX_16BIT_OFFSET    (1<<16)
#define MM_LONGOFF              16
#define LZ5_BLOCK_SIZE          (1<<16)

/* LZ4 codewords */
#define ML_BITS_LZ4  4
#define ML_MASK_LZ4  ((1U<<ML_BITS_LZ4)-1)
#define RUN_BITS_LZ4 (8-ML_BITS_LZ4)
#define RUN_MASK_LZ4 ((1U<<RUN_BITS_LZ4)-1)

/* LZ5v2 codewords */
#define ML_BITS_LZ5v2   4
#define ML_MASK_LZ5v2   ((1U<<ML_BITS_LZ5v2)-1)
#define RUN_BITS_LZ5v2  3
#define RUN_MASK_LZ5v2  ((1U<<RUN_BITS_LZ5v2)-1)
#define ML_RUN_BITS     (ML_BITS_LZ5v2 + RUN_BITS_LZ5v2)
#define LZ5_24BIT_OFFSET_LOAD   price += LZ5_highbit32(offset)
#define LZ5_LENGTH_SIZE_LZ5v2(len) ((len >= (1<<16)+RUN_MASK_LZ5v2) ? 5 : ((len >= 254+RUN_MASK_LZ5v2) ? 3 : ((len >= RUN_MASK_LZ5v2) ? 1 : 0)))

#ifdef USE_8BIT_CODEWORDS
    #define LZ5_MAX_8BIT_OFFSET (1<<8)
    #define LAST_LONG_OFF 15
    #define FIRST_SHORT_OFF 16
    #define LAST_SHORT_OFF 31
    #define SHORT_OFF_COUNT (LAST_SHORT_OFF-FIRST_SHORT_OFF)
#else
    #define LAST_LONG_OFF 31
#endif

typedef enum { noLimit = 0, limitedOutput = 1 } limitedOutput_directive;
typedef enum { LZ5_parser_fast, LZ5_parser_noChain, LZ5_parser_hashChain, LZ5_parser_priceFast, LZ5_parser_lowestPrice, LZ5_parser_optimalPrice, LZ5_parser_optimalPriceBT } LZ5_parser_type;   /* from faster to stronger */ 
typedef enum { LZ5_coderwords_LZ4, LZ5_coderwords_LZ5v2 } LZ5_decompress_type;
typedef struct
{
    U32 windowLog;     /* largest match distance : impact decompression buffer size */
    U32 contentLog;    /* full search segment : larger == more compression, slower, more memory (useless for fast) */
    U32 hashLog;       /* dispatch table : larger == more memory, faster*/
    U32 hashLog3;      /* dispatch table : larger == more memory, faster*/
    U32 searchNum;     /* nb of searches : larger == more compression, slower*/
    U32 searchLength;  /* size of matches : larger == faster decompression */
    U32 minMatchLongOff;  /* min match size with offsets >= 1<<16 */ 
    U32 sufficientLength;  /* used only by optimal parser: size of matches which is acceptable: larger == more compression, slower */
    U32 fullSearch;    /* used only by optimal parser: perform full search of matches: 1 == more compression, slower */
    LZ5_parser_type parserType;
    LZ5_decompress_type decompressType;
} LZ5_parameters; 


struct LZ5_stream_s
{
    const BYTE* end;        /* next block here to continue on current prefix */
    const BYTE* base;       /* All index relative to this position */
    const BYTE* dictBase;   /* alternate base for extDict */
    U32   dictLimit;        /* below that point, need extDict */
    U32   lowLimit;         /* below that point, no more dict */
    U32   nextToUpdate;     /* index from which to continue dictionary update */
    U32   allocatedMemory;
    int   compressionLevel;
    LZ5_parameters params;
    U32   hashTableSize;
    U32   chainTableSize;
    U32*  chainTable;
    U32*  hashTable;
    int   last_off;
    BYTE* tokenPtr;
    int   tokenPart;
};


/* *************************************
*  HC Pre-defined compression levels
***************************************/
#define LZ5_WINDOWLOG_LZ4   16
#define LZ5_CHAINLOG_LZ4    LZ5_WINDOWLOG_LZ4
#define LZ5_HASHLOG_LZ4     18

#define LZ5_WINDOWLOG_LZ5v2 22
#define LZ5_CHAINLOG_LZ5v2  LZ5_WINDOWLOG_LZ5v2
#define LZ5_HASHLOG_LZ5v2   18

#define LZ5_DEFAULT_CLEVEL  8


static const LZ5_parameters LZ5_defaultParameters[LZ5_MAX_CLEVEL+1] =
{
    /*            windLog,            contentLog,           HashLog,  H3,  Snum, SL,   MMLongOff, SuffL, FS, Parser function,           Decompressor type  */
    {                   0,                     0,                 0,   0,     0,  0,           0,     0,  0, LZ5_parser_fast,           LZ5_coderwords_LZ4   }, // level 0 - never used
    {   LZ5_WINDOWLOG_LZ4,                     0,                12,   0,     0,  0,           0,     0,  0, LZ5_parser_fast,           LZ5_coderwords_LZ4   }, // level 1
    {   LZ5_WINDOWLOG_LZ4,                     0,   LZ5_HASHLOG_LZ4,   0,     0,  0,           0,     0,  0, LZ5_parser_fast,           LZ5_coderwords_LZ4   }, // level 2
    {   LZ5_WINDOWLOG_LZ4,                     0,   LZ5_HASHLOG_LZ4,   0,     0,  0,           0,     0,  0, LZ5_parser_noChain,        LZ5_coderwords_LZ4   }, // level 3
    {   LZ5_WINDOWLOG_LZ4,      LZ5_CHAINLOG_LZ4,   LZ5_HASHLOG_LZ4,   0,     2,  5,           0,     0,  0, LZ5_parser_hashChain,      LZ5_coderwords_LZ4   }, // level 4
    {   LZ5_WINDOWLOG_LZ4,      LZ5_CHAINLOG_LZ4,   LZ5_HASHLOG_LZ4,   0,     6,  5,           0,     0,  0, LZ5_parser_hashChain,      LZ5_coderwords_LZ4   }, // level 5
    {   LZ5_WINDOWLOG_LZ4,      LZ5_CHAINLOG_LZ4,   LZ5_HASHLOG_LZ4,   0,     8,  5,           0,     0,  0, LZ5_parser_hashChain,      LZ5_coderwords_LZ4   }, // level 6
    {   LZ5_WINDOWLOG_LZ4,      LZ5_CHAINLOG_LZ4,   LZ5_HASHLOG_LZ4,   0,    16,  4,           0,     0,  0, LZ5_parser_hashChain,      LZ5_coderwords_LZ4   }, // level 7
    {   LZ5_WINDOWLOG_LZ4,      LZ5_CHAINLOG_LZ4,   LZ5_HASHLOG_LZ4,   0,   256,  4,           0,     0,  0, LZ5_parser_hashChain,      LZ5_coderwords_LZ4   }, // level 8
    {   LZ5_WINDOWLOG_LZ4,   LZ5_WINDOWLOG_LZ4+1,                23,  16,   256,  4,           0, 1<<10,  1, LZ5_parser_optimalPriceBT, LZ5_coderwords_LZ4   }, // level 9
    { LZ5_WINDOWLOG_LZ5v2,    LZ5_CHAINLOG_LZ5v2,                14,  13,     1,  5,  MM_LONGOFF,     0,  0, LZ5_parser_priceFast,      LZ5_coderwords_LZ5v2 }, // level 10
    { LZ5_WINDOWLOG_LZ5v2,    LZ5_CHAINLOG_LZ5v2, LZ5_HASHLOG_LZ5v2,  13,     1,  5,  MM_LONGOFF,     0,  0, LZ5_parser_priceFast,      LZ5_coderwords_LZ5v2 }, // level 11
    { LZ5_WINDOWLOG_LZ5v2,    LZ5_CHAINLOG_LZ5v2, LZ5_HASHLOG_LZ5v2,  13,     1,  5,  MM_LONGOFF,     0,  0, LZ5_parser_lowestPrice,    LZ5_coderwords_LZ5v2 }, // level 12
    { LZ5_WINDOWLOG_LZ5v2,    LZ5_CHAINLOG_LZ5v2,                23,  16,     2,  5,  MM_LONGOFF,     0,  0, LZ5_parser_lowestPrice,    LZ5_coderwords_LZ5v2 }, // level 13
    { LZ5_WINDOWLOG_LZ5v2,    LZ5_CHAINLOG_LZ5v2,                23,  16,     8,  4,  MM_LONGOFF,     0,  0, LZ5_parser_lowestPrice,    LZ5_coderwords_LZ5v2 }, // level 14
    { LZ5_WINDOWLOG_LZ5v2,    LZ5_CHAINLOG_LZ5v2,                23,  16,     8,  4,  MM_LONGOFF,    64,  0, LZ5_parser_optimalPrice,   LZ5_coderwords_LZ5v2 }, // level 15
    { LZ5_WINDOWLOG_LZ5v2,  LZ5_CHAINLOG_LZ5v2+1,                23,  16,     8,  4,  MM_LONGOFF,    64,  1, LZ5_parser_optimalPriceBT, LZ5_coderwords_LZ5v2 }, // level 16
    { LZ5_WINDOWLOG_LZ5v2,  LZ5_CHAINLOG_LZ5v2+1,                23,  16,   128,  4,  MM_LONGOFF,    64,  1, LZ5_parser_optimalPriceBT, LZ5_coderwords_LZ5v2 }, // level 17
    {                  24,                    25,                28,  24, 1<<10,  4,  MM_LONGOFF, 1<<10,  1, LZ5_parser_optimalPriceBT, LZ5_coderwords_LZ5v2 }, // level 18
//  {                  10,                    10,                10,   0,     0,  4,           0,     0,  0, LZ5_fast          }, // min values
//  {                  24,                    24,                28,  24, 1<<24,  7,           0, 1<<24,  2, LZ5_optimal_price }, // max values
};



/*-************************************
*  Compiler Options
**************************************/
#ifdef _MSC_VER    /* Visual Studio */
#  define FORCE_INLINE static __forceinline
#  include <intrin.h>
#  pragma warning(disable : 4127)        /* disable: C4127: conditional expression is constant */
#  pragma warning(disable : 4293)        /* disable: C4293: too large shift (32-bits) */
#else
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)   /* C99 */
#    if defined(__GNUC__) || defined(__clang__)
#      define FORCE_INLINE static inline __attribute__((always_inline))
#    else
#      define FORCE_INLINE static inline
#    endif
#  else
#    define FORCE_INLINE static
#  endif   /* __STDC_VERSION__ */
#endif  /* _MSC_VER */

#define LZ5_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#if (LZ5_GCC_VERSION >= 302) || (__INTEL_COMPILER >= 800) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#define likely(expr)     expect((expr) != 0, 1)
#define unlikely(expr)   expect((expr) != 0, 0)

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define ALLOCATOR(n,s) calloc(n,s)
#define FREEMEM        free
#define MEM_INIT       memset
#define LZ5_DICT_SIZE (1 << 24)
#ifndef MAX
    #define MAX(a,b) ((a)>(b))?(a):(b)
#endif
#ifndef MIN
	#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#if MINMATCH == 3
    #define MEM_readMINMATCH(ptr) (U32)(MEM_read32(ptr)<<8) 
#else
    #define MEM_readMINMATCH(ptr) (U32)(MEM_read32(ptr)) 
#endif 




/*-************************************
*  Reading and writing into memory
**************************************/
#define STEPSIZE sizeof(size_t)


MEM_STATIC void LZ5_copy8(void* dst, const void* src)
{
    memcpy(dst,src,8);
}

/* customized variant of memcpy, which can overwrite up to 7 bytes beyond dstEnd */
MEM_STATIC void LZ5_wildCopy(void* dstPtr, const void* srcPtr, void* dstEnd)
{
    BYTE* d = (BYTE*)dstPtr;
    const BYTE* s = (const BYTE*)srcPtr;
    BYTE* const e = (BYTE*)dstEnd;

#if 0
    const size_t l2 = 8 - (((size_t)d) & (sizeof(void*)-1));
    LZ5_copy8(d,s); if (d>e-9) return;
    d+=l2; s+=l2;
#endif /* join to align */

    do { LZ5_copy8(d,s); d+=8; s+=8; } while (d<e);
}

MEM_STATIC void LZ5_wildCopy16(BYTE* dstPtr, const BYTE* srcPtr, BYTE* dstEnd)
{
    do {
        LZ5_copy8(dstPtr, srcPtr);
        LZ5_copy8(dstPtr+8, srcPtr+8);
        dstPtr += 16;
        srcPtr += 16;
    }
    while (dstPtr < dstEnd);
}

/*
 * LZ5_FORCE_SW_BITCOUNT
 * Define this parameter if your target system or compiler does not support hardware bit count
 */
#if defined(_MSC_VER) && defined(_WIN32_WCE)   /* Visual Studio for Windows CE does not support Hardware bit count */
#  define LZ5_FORCE_SW_BITCOUNT
#endif


/* **************************************
*  Function body to include for inlining
****************************************/
MEM_STATIC U32 LZ5_highbit32(U32 val)
{
#   if defined(_MSC_VER)   /* Visual */
    unsigned long r=0;
    _BitScanReverse(&r, val);
    return (unsigned)r;
#   elif defined(__GNUC__) && (__GNUC__ >= 3)   /* GCC Intrinsic */
    return 31 - __builtin_clz(val);
#   else   /* Software version */
    static const int DeBruijnClz[32] = { 0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31 };
    U32 v = val;
    int r;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    r = DeBruijnClz[(U32)(v * 0x07C4ACDDU) >> 27];
    return r;
#   endif
}


/*-************************************
*  Common functions
**************************************/
MEM_STATIC unsigned LZ5_NbCommonBytes (register size_t val)
{
    if (MEM_isLittleEndian()) {
        if (MEM_64bits()) {
#       if defined(_MSC_VER) && defined(_WIN64) && !defined(LZ5_FORCE_SW_BITCOUNT)
            unsigned long r = 0;
            _BitScanForward64( &r, (U64)val );
            return (int)(r>>3);
#       elif (defined(__clang__) || (LZ5_GCC_VERSION >= 304)) && !defined(LZ5_FORCE_SW_BITCOUNT)
            return (__builtin_ctzll((U64)val) >> 3);
#       else
            static const int DeBruijnBytePos[64] = { 0, 0, 0, 0, 0, 1, 1, 2, 0, 3, 1, 3, 1, 4, 2, 7, 0, 2, 3, 6, 1, 5, 3, 5, 1, 3, 4, 4, 2, 5, 6, 7, 7, 0, 1, 2, 3, 3, 4, 6, 2, 6, 5, 5, 3, 4, 5, 6, 7, 1, 2, 4, 6, 4, 4, 5, 7, 2, 6, 5, 7, 6, 7, 7 };
            return DeBruijnBytePos[((U64)((val & -(long long)val) * 0x0218A392CDABBD3FULL)) >> 58];
#       endif
        } else /* 32 bits */ {
#       if defined(_MSC_VER) && !defined(LZ5_FORCE_SW_BITCOUNT)
            unsigned long r;
            _BitScanForward( &r, (U32)val );
            return (int)(r>>3);
#       elif (defined(__clang__) || (LZ5_GCC_VERSION >= 304)) && !defined(LZ5_FORCE_SW_BITCOUNT)
            return (__builtin_ctz((U32)val) >> 3);
#       else
            static const int DeBruijnBytePos[32] = { 0, 0, 3, 0, 3, 1, 3, 0, 3, 2, 2, 1, 3, 2, 0, 1, 3, 3, 1, 2, 2, 2, 2, 0, 3, 1, 2, 0, 1, 0, 1, 1 };
            return DeBruijnBytePos[((U32)((val & -(S32)val) * 0x077CB531U)) >> 27];
#       endif
        }
    } else   /* Big Endian CPU */ {
        if (MEM_64bits()) {
#       if defined(_MSC_VER) && defined(_WIN64) && !defined(LZ5_FORCE_SW_BITCOUNT)
            unsigned long r = 0;
            _BitScanReverse64( &r, val );
            return (unsigned)(r>>3);
#       elif (defined(__clang__) || (LZ5_GCC_VERSION >= 304)) && !defined(LZ5_FORCE_SW_BITCOUNT)
            return (__builtin_clzll((U64)val) >> 3);
#       else
            unsigned r;
            if (!(val>>32)) { r=4; } else { r=0; val>>=32; }
            if (!(val>>16)) { r+=2; val>>=8; } else { val>>=24; }
            r += (!val);
            return r;
#       endif
        } else /* 32 bits */ {
#       if defined(_MSC_VER) && !defined(LZ5_FORCE_SW_BITCOUNT)
            unsigned long r = 0;
            _BitScanReverse( &r, (unsigned long)val );
            return (unsigned)(r>>3);
#       elif (defined(__clang__) || (LZ5_GCC_VERSION >= 304)) && !defined(LZ5_FORCE_SW_BITCOUNT)
            return (__builtin_clz((U32)val) >> 3);
#       else
            unsigned r;
            if (!(val>>16)) { r=2; val>>=8; } else { r=0; val>>=24; }
            r += (!val);
            return r;
#       endif
        }
    }
}

MEM_STATIC unsigned LZ5_count(const BYTE* pIn, const BYTE* pMatch, const BYTE* pInLimit)
{
    const BYTE* const pStart = pIn;

    while (likely(pIn<pInLimit-(STEPSIZE-1))) {
        size_t diff = MEM_readST(pMatch) ^ MEM_readST(pIn);
        if (!diff) { pIn+=STEPSIZE; pMatch+=STEPSIZE; continue; }
        pIn += LZ5_NbCommonBytes(diff);
        return (unsigned)(pIn - pStart);
    }

    if (MEM_64bits()) if ((pIn<(pInLimit-3)) && (MEM_read32(pMatch) == MEM_read32(pIn))) { pIn+=4; pMatch+=4; }
    if ((pIn<(pInLimit-1)) && (MEM_read16(pMatch) == MEM_read16(pIn))) { pIn+=2; pMatch+=2; }
    if ((pIn<pInLimit) && (*pMatch == *pIn)) pIn++;
    return (unsigned)(pIn - pStart);
}

/* alias to functions with compressionLevel=1 */
int LZ5_sizeofState_Level1(void);
int LZ5_compress_Level1(const char* source, char* dest, int sourceSize, int maxDestSize);
int LZ5_compress_extState_Level1 (void* state, const char* source, char* dest, int inputSize, int maxDestSize);
LZ5_stream_t* LZ5_resetStream_Level1 (LZ5_stream_t* streamPtr);
LZ5_stream_t* LZ5_createStream_Level1(void);


#if defined (__cplusplus)
}
#endif

#endif /* LZ5_COMMON_H_2983827168210 */
