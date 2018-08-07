/*
 * MemCopy.h
 *
 *  Created on: Jul 22, 2018
 *      Author: tungdang
 */

#ifndef MEMCOPY_H_
#define MEMCOPY_H_

#pragma once
#include "type.h"
#include "Debug.h"
#include "utility.h"
#include <string.h>
#include "CpuUtil.h"
#include <basetsd.h>
#include <math.h>
#include <intrin.h>
#include <xmmintrin.h>
#include <x86intrin.h>
#include <emmintrin.h>

typedef __m128 DQWORD;
typedef DWORD PREFETCH[8];
typedef DWORD CACHELINE[8];
typedef WORD DHWORD[32];

namespace iSTD
{

enum {
	DWORD_SHIFT         = 2,
	    BYTE_TAIL           = 3,
	    INSTR_128_SHIFT     = 4,
	    CACHE_LINE_SHIFT    = 6,
	    DUAL_CACHE_SHIFT    = 7,
	    TAIL_SIZE           = 15,
	    INSTR_WIDTH_128     = 16,
	    INSTR_WIDTH_256     = 32,
	    CACHE_LINE_SIZE     = 64,
	    TIERED_TAIL         = 127,
	    DUAL_CACHE_SIZE     = 128,
	    MIN_ERMSB_ALIGNED   = 4096,
	MIN_STREAM_SIZE = 524288,
};

#ifndef _WIN64
#define USE_INLINE_ASM 0
#define USE_INLINE_ASM 1
#endif

inline void Prefetch(const void*);
inline void PrefetchBuffer(const void*, const size_t);
inline void CachelineFlush(const void*);

template <size_t size>
inline void MemCopy(void*, const void*);
inline void MemCopy(void*, const void*, const size_t);
inline void MemCopyWC(void*, const void*, const size_t);
inline void MemCopySwapBypes(void*, const void*, const size_t, const unsigned int);
inline void ScalarSwapBypes(__m128i*, const __m128i**, const size_t, const unsigned int);

inline void SafeMemSet(void*, const int, const size_t);
inline int SafeMemCompare(const void*, const void*, const size_t);
inline void SafeMemMove(void*, const void*, const size_t);

#ifndef _WIN64
inline void __fastcall FastBlockCopyFromUSWC_SSE4_1_movntdqa_movdqa(void* dst, const void* src);
inline void __fastcall FastBlockCopyFromUSWC_SSE4_1_movntdqa_movdqu(void* dst, const void* src);
#endif

inline void FastMemCopyFromWC(void* dst, const void* src, const size_t bytes, CPU_INSTRUCTION_LEVEL cpuInstructionLevel);
inline void FastCpuBlt(BYTE*, const DWORD, BYTE*, const DWORD, const DWORD, DWORD);
inline void FindWordBufferMinMax(WORD*, const DWORD, WORD&, WORD&);
inline void FindDWordBufferMinMax(DWORD*, const DWORD, DWORD&, DWORD&);
inline void FindWordBufferMinMaxRestart(WORD*, const DWORD, const WORD, WORD&, WORD&, CPU_INSTRUCTION_LEVEL cpuInstructionLevel);
inline void FindDWordBufferMinMaxRestart(DWORD*, const DWORD, const DWORD, DWORD&, DWORD&, CPU_INSTRUCTION_LEVEL cpuInstructionLevel);

inline void FindWordBufferMinMaxCopy(WORD*, WORD*, const DWORD, WORD&, WORD&);
inline void FindDWordBufferMinMaxCopy(DWORD*, DWORD*, const DWORD, DWORD&, DWORD&);
inline void FindWordBufferMinMaxRestartCopy(WORD*, WORD*, const DWORD, const WORD, WORD&, WORD&, CPU_INSTRUCTION_LEVEL cpuInstructionLevel);
inline void FindDWordBufferMinMaxRestartCopy(DWORD*, DWORD*, const DWORD, const DWORD, DWORD&, DWORD&, CPU_INSTRUCTION_LEVEL cpuInstructionLevel);

inline void Prefetch(const void* ptr) {
	_mm_prefetch((const char*)ptr, _MM_HINT_NTA);
}

inline void PrefetchBuffer(const void* pBuffer, const size_t bytes) {
	const size_t cachelines = bytes/sizeof(PREFETCH);
	for (size_t i=0; i <= cachelines; i++)
	{
		_mm_prefetch((const char*)pBuffer + i * sizeof(PREFETCH),_MM_HINT_NTA);
	}
}

inline void CachelineFlush(const void* ptr) {
	_mm_clflush((char*)ptr);
}

template <size_t size>
inline void MemCopy(void* dst, const void* src) {
	MemCopy(dst,src,size);
}

template <>
inline void MemCopy<1>(void* dst, const void* src) {
	const BYTE* pSrc = reinterpret_cast<const BYTE*>(src);
	BYTE* pDst = reinterpret_cast<BYTE*>(dst);
	*pDst = *pSrc;
}

template <>
inline void MemCopy<2>(void* dst, const void* src) {
	const WORD* pSrc = reinterpret_cast<const WORD*>(src);
	WORD* pDst = reinterpret_cast<WORD*>(dst);
	*pDst = *pSrc;
}

template <>
inline void MemCopy<4>(void* dst, const void* src) {
	const UINT32* pSrc = reinterpret_cast<const UINT32*>(src);
	UINT32* pDst = reinterpret_cast<UINT32*>(dst);
	*pDst = *pSrc;
}

template <>
inline void MemCopy<8>(void* dst, const void* src) {
	const UINT64* pSrc = reinterpret_cast<const UINT64*>(src);
	UINT64* pDst = reinterpret_cast<UINT64*>(dst);
	*pDst = *pSrc;
}

template <>
inline void MemCopy<16>(void* dst, const void* src) {
	const __m128i* pMMSrc = reinterpret_cast<const __m128i*>(src);
	__m128i* pMMDst = reinterpret_cast<__m128i*>(dst);
	__m128i xmm0 = _mm_loadu_si128(pMMSrc);
	_mm_storeu_si128(pMMDst,xmm0);
}

template <>
inline void MemCopy<28>(void* dst, const void* src) {
	const __m128i* pMMSrc = reinterpret_cast<const __m128i*>(src);
	__m128i* pMMDst = reinterpret_cast<__m128i*>(dst);
	__m128i xmm0 = _mm_loadu_si128(pMMSrc);
	_mm_storeu_si128(pMMDst,xmm0);

	pMMSrc += 1;
	pMMDst += 1;

	const UINT64* pSrc64 = reinterpret_cast<const UINT64*>(pMMSrc);
	UINT64* pDst64 = reinterpret_cast<UINT64*>(pMMDst);
	*pDst64 = *pSrc64;

	pDst64 += 1;
	pSrc64 += 1;

	const UINT32* pSrc32 = reinterpret_cast<const UINT32*>(pSrc64);
	UINT32* pDst32 = reinterpret_cast<UINT32*>(pDst64);
	*pDst32 = *pSrc32;
}

inline void MemCopy(void* dst, const void* src, const size_t bytes) {
	UINT8* pDst8 = reinterpret_cast<UINT8*>(dst);
	const UINT8* pSrc8 = reinterpret_cast<const UINT8*>(src);
	size_t bytesRemaining = bytes;

	if (bytesRemaining == 0)
		return;

	if (bytesRemaining <= 4)
	{
		if (bytesRemaining == 1)
		{
			*pDst8 = *pSrc8;
			return;
		}

		if (bytesRemaining == 2)
		{
			*reinterpret_cast<UINT16*>(pDst8) = *reinterpret_cast<const UINT16*>(pSrc8);
			return;
		}

		if (bytesRemaining == 3)
		{
			*reinterpret_cast<UINT16*>(pDst8) = *reinterpret_cast<const UINT16*>(pSrc8);
			*(pDst8 + 2) = *(pSrc8 + 2);
			return;
		}

		*reinterpret_cast<UINT32*>(pDst8) = *reinterpret_cast<const UINT32*>(pSrc8);
		return;
	}

	if (bytesRemaining > 8 && reinterpret_cast<UINT_PTR>(pDst8) & 0x3)
	{
		if (reinterpret_cast<UINT_PTR>(pDst8) & 0x1)
		{
			*pDst8 = *pSrc8;
			pDst8 +=1;
			pSrc8 += 1;
		}
		if (reinterpret_cast<UINT_PTR>(pDst8) & 0x2)
		{
			bytesRemaining -= 2;
			pDst8 += 2;
			pSrc8 += 2;
		}
	}

	if (bytesRemaining <= CACHE_LINE_SIZE)
	{
		const size_t ptrAdvance = bytesRemaining & 0x3;
		pDst8 += ptrAdvance;
		pSrc8 += ptrAdvance;

		switch(bytesRemaining / 4)
		{
		case 16:
			*reinterpret_cast<UINT32*>(pDst8 - 64) = *reinterpret_cast<const UINT32*>(pSrc8 - 64);
			break;
		case 15:
			*reinterpret_cast<UINT32*>(pDst8 - 60) = *reinterpret_cast<const UINT32*>(pSrc8 - 60);
			break;
		case 14:
			*reinterpret_cast<UINT32*>(pDst8 - 56) = *reinterpret_cast<const UINT32*>(pSrc8 - 56);
			break;
		case 13:
			*reinterpret_cast<UINT32*>(pDst8 - 52) = *reinterpret_cast<const UINT32*>(pSrc8 - 52);
			break;
		case 12:
			*reinterpret_cast<UINT32*>(pDst8 - 48) = *reinterpret_cast<const UINT32*>(pSrc8 - 48);
			break;
		case 11:
		    *reinterpret_cast<UINT32*>( pDst8 - 44 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 44 );
		    break;
		case 10:
		    *reinterpret_cast<UINT32*>( pDst8 - 40 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 40 );
		    break;
		case 9:
		    *reinterpret_cast<UINT32*>( pDst8 - 36 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 36 );
		    break;
		case 8:
		    *reinterpret_cast<UINT32*>( pDst8 - 32 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 32 );
		    break;
		case 7:
		    *reinterpret_cast<UINT32*>( pDst8 - 28 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 28 );
		    break;
		case 6:
		    *reinterpret_cast<UINT32*>( pDst8 - 24 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 24 );
		    break;
		case 5:
		    *reinterpret_cast<UINT32*>( pDst8 - 20 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 20 );
		    break;
		case 4:
		    *reinterpret_cast<UINT32*>( pDst8 - 16 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 16 );
		    break;
		case 3:
		    *reinterpret_cast<UINT32*>( pDst8 - 12 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 12 );
		    break;
		case 2:
		    *reinterpret_cast<UINT32*>( pDst8 - 8 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 8 );
		    break;
		case 1:
		    *reinterpret_cast<UINT32*>( pDst8 - 4 ) = *reinterpret_cast<const UINT32*>( pSrc8 - 4 );
		    break;
		}

		if (bytesRemaining & 0x1)
			{
				*pDst8 = *pSrc8;
				bytesRemaining -= 1;
				pDst8 += 1;
				pSrc8 += 1;
			}

		if (bytesRemaining & 0x2)
			{
				*reinterpret_cast<UINT16*>(pDst8) = *reinterpret_cast<const UINT16*>(pSrc8);
				bytesRemaining -=2;
				pDst8 += 2;
				pSrc8 += 2;
			}
	}

	else
	{
		__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
		const size_t alignDst16 = reinterpret_cast<UINT_PTR>(pDst8) & TAIL_SIZE;
		if (alignDst16 != 0)
		{
			const size_t alignSize = 0x10 - alignDst16;
			pDst8 += alignSize;
			pSrc8 += alignSize;
			switch(alignSize / 4)
			{
			case 3:
				*reinterpret_cast<UINT32*>(pDst8-12) = *reinterpret_cast<const UINT32*>(pSrc8-12);
				break;
			case 2:
				*reinterpret_cast<UINT32*>(pDst8-8) = *reinterpret_cast<const UINT32*>(pSrc8-8);
				break;
			case 1:
				*reinterpret_cast<UINT32*>(pDst8-4) = *reinterpret_cast<const UINT32*>(pSrc8-4);
				break;
			}
			bytesRemaining -= alignSize;
		}

		if (bytesRemaining > MIN_STREAM_SIZE)
		{
			while (bytesRemaining >= 128)
			{
				pDst8 += 128;
				pSrc8 += 128;
				bytesRemaining -= 128;

				xmm0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 128));
				xmm1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 112));
				xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 96));
				xmm3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 80));
				xmm4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 64));
				xmm5 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 48));
				xmm6 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 32));
				xmm7 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 16));

				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 128),xmm0);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 112),xmm1);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 96),xmm2);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 80),xmm3);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 64),xmm4);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 48),xmm5);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 32),xmm6);
				_mm_stream_si128(reinterpret_cast<__m128i*>(pDst8 - 16),xmm7);
			}

			const size_t ptrAdvance = bytesRemaining & ~0xF;
			pDst8 += ptrAdvance;
			pSrc8 += ptrAdvance;

			switch(bytesRemaining / 16)
			{
			case 7:
				xmm0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 112));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 112),xmm0);
				break;
			case 6:
				xmm1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 96));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 96),xmm1);
				break;
			case 5:
				xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 80));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 80),xmm2);
				break;
			case 4:
				xmm3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 64));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 64),xmm3);
				break;
			case 3:
				xmm4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 48));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 48),xmm4);
				break;
			case 2:
				xmm5 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 32));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 32),xmm5);
				break;
			case 1:
				xmm6 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSrc8 - 16));
				_mm_store_si128(reinterpret_cast<__m128i*>(pDst8 - 16),xmm6);
				break;
			}

			bytesRemaining -= ptrAdvance;
		}
	}

	if (bytesRemaining)
	{
		const size_t ptrAdvance = bytesRemaining & ~0x3;
		pDst8 += ptrAdvance;
		pSrc8 += ptrAdvance;

		switch(bytesRemaining / 4)
		{
		case 3:
			*reinterpret_cast<UINT32*>(pDst8 - 12) = *reinterpret_cast<const UINT32*>(pSrc8 - 12);
			break;
		case 2:
			*reinterpret_cast<UINT32*>(pDst8 - 8) = *reinterpret_cast<const UINT32*>(pSrc8 - 8);
			break;
		case 1:
			*reinterpret_cast<UINT32*>(pDst8 - 4) = *reinterpret_cast<const UINT32*>(pSrc8 - 4);
			break;
		}

		if (bytesRemaining & 0x1)
		{
			*pDst8 = *pSrc8;
			bytesRemaining -= 1;
			pDst8 += 1;
			pSrc8 += 1;
		}

		if (bytesRemaining & 0x2)
		{
			*reinterpret_cast<UINT16*>(pDst8) = *reinterpret_cast<const UINT16*>(pSrc8);
			bytesRemaining -=2;
			pDst8 += 2;
			pSrc8 += 2;
		}
	}

}

inline void MemCopyWC(void* dst, const void* src, const size_t bytes) {
	const __m128i s_SSE2CmpMask = _mm_setr_epi8(0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00);
    const __m128i* pMMSrc = reinterpret_cast<const __m128i*>(src);
    __m128i* pMMDst = reinterpret_cast<__m128i*>(dst);
    size_t count = bytes;
    size_t cnt = 0;
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

    // If size > 16 align destination and move non-temporally

    if (count >= INSTR_WIDTH_128)
    {
    	// align destination to 16 if necessary

    	UINT32 align = (UINT32)((UINT_PTR)pMMDst & TAIL_SIZE);
    	if (align != 0)
    	{
    		// Move alignment through a masked non-temporal move

    		const char* pSrc = reinterpret_cast<const char*>(pMMSrc);
    		char* pDst = reinterpret_cast<char*>(pMMDst);
    		align = INSTR_WIDTH_128 - align;
    		char shiftCnt = (char)(INSTR_WIDTH_128 - align - 1);
    		__m128i shiftMask = _mm_set1_epi8(shiftCnt);
    		__m128i mask = _mm_cmpgt_epi8(s_SSE2CmpMask,shiftMask);
    		__m128i val = _mm_loadu_si128(pMMSrc);
    		_mm_maskmoveu_si128(val,mask,pDst);

    		pSrc += align;
    		pDst += align;

    		pMMSrc = reinterpret_cast<const __m128i*>(pSrc);
    		pMMDst = reinterpret_cast<__m128i*>(pDst);
    	}

    	// take off the alignment from size
    	count -= align;
    	if ((UINT_PTR)pMMSrc & TAIL_SIZE)
    	{
    		// copy un-alignment by tiers
    		cnt = count >> DUAL_CACHE_SHIFT;
    		for (UINT32 i = 0; i < cnt; i += 1)
    		{
    			xmm0 = _mm_loadu_si128(pMMSrc);
    			xmm1 = _mm_loadu_si128(pMMSrc + 1);
    			xmm2 = _mm_loadu_si128(pMMSrc + 2);
    			xmm3 = _mm_loadu_si128(pMMSrc + 3);
    			xmm4 = _mm_loadu_si128(pMMSrc + 4);
    			xmm5 = _mm_loadu_si128(pMMSrc + 5);
    			xmm6 = _mm_loadu_si128(pMMSrc + 6);
    			xmm7 = _mm_loadu_si128(pMMSrc + 7);
    			pMMSrc += 8;

    			_mm_stream_si128(pMMDst, xmm0);
    			_mm_stream_si128(pMMDst + 1, xmm1);
    			_mm_stream_si128(pMMDst + 2, xmm2);
    			_mm_stream_si128(pMMDst + 3, xmm3);
    			_mm_stream_si128(pMMDst + 4, xmm4);
    			_mm_stream_si128(pMMDst + 5, xmm5);
    			_mm_stream_si128(pMMDst + 6, xmm6);
    			_mm_stream_si128(pMMDst + 7, xmm7);
    			pMMDst += 8;
    		}

    		count &= TIERED_TAIL;
    		if (count != 0)
    		{
    			cnt = count >> INSTR_128_SHIFT;
    			for (UINT32 i=0; i<cnt; i += 1)
    			{
    				xmm0 = _mm_loadu_si128(pMMSrc);
    				pMMSrc += 1;
    				_mm_stream_si128(pMMDst, xmm0);
    				pMMDst += 1;
    			}
    		}
    	}

    	else
    	{
    		// copy aligned by tiers
    		            cnt = count >> DUAL_CACHE_SHIFT;
    		            for (UINT32 i = 0; i < cnt; i += 1)
    		            {
    		                xmm0 = _mm_load_si128(pMMSrc);
    		                xmm1 = _mm_load_si128(pMMSrc + 1);
    		                xmm2 = _mm_load_si128(pMMSrc + 2);
    		                xmm3 = _mm_load_si128(pMMSrc + 3);
    		                xmm4 = _mm_load_si128(pMMSrc + 4);
    		                xmm5 = _mm_load_si128(pMMSrc + 5);
    		                xmm6 = _mm_load_si128(pMMSrc + 6);
    		                xmm7 = _mm_load_si128(pMMSrc + 7);
    		                pMMSrc += 8;

    		                _mm_stream_si128(pMMDst, xmm0);
    		                _mm_stream_si128(pMMDst + 1, xmm1);
    		                _mm_stream_si128(pMMDst + 2, xmm2);
    		                _mm_stream_si128(pMMDst + 3, xmm3);
    		                _mm_stream_si128(pMMDst + 4, xmm4);
    		                _mm_stream_si128(pMMDst + 5, xmm5);
    		                _mm_stream_si128(pMMDst + 6, xmm6);
    		                _mm_stream_si128(pMMDst + 7, xmm7);
    		                pMMDst += 8;
    		            }

    		            count &= TIERED_TAIL;
    		            if (count != 0)
    		            {
    		                cnt = count >> INSTR_128_SHIFT;
    		                for (UINT32 i = 0; i < cnt; i += 1)
    		                {
    		                    xmm0 = _mm_load_si128(pMMSrc);
    		                    pMMSrc += 1;
    		                    _mm_stream_si128(pMMDst, xmm0);
    		                    pMMDst += 1;
    		                }
    		}
    	}
    }

    count &= TAIL_SIZE;
    if (count != 0)
    {
    	cnt = count >> DWORD_SHIFT;
    	DWORD* pDst = reinterpret_cast<DWORD*>(pMMDst);
    	const DWORD* pSrc = reinterpret_cast<const DWORD*>(pMMSrc);
    	for (UINT32 i = 0; i<cnt; i++)
    	{
    		*pDst = *pSrc;
    		pDst += 1;
    		pSrc += 1;
    	}

    	cnt = count & BYTE_TAIL;
    	BYTE* bDst = reinterpret_cast<BYTE*>(pDst);
    	const BYTE* bSrc = reinterpret_cast<const BYTE*>(pSrc);
    	for (UINT32 i=0; i<cnt; i++)
    	{
    		*bDst = *bSrc;
    		bDst += 1;
    		bSrc += 1;
    	}
    }
}

inline void ScalarSwapBytes(__m128i** dst, const __m128i** src, const size_t byteCount, const unsigned int swapbytes) {
	switch (swapbytes)
	{
	case 2:
	{
		WORD* wDst = reinterpret_cast<WORD*>(*dst);
		const WORD* wSrc = reinterpret_cast<const WORD*>(*src);
		for (UINT32 i=0; i < byteCount / 2; i += 1)
		{
			WORD tmp = *wSrc;
		    *wDst = (tmp >> 8) | (tmp << 8);
			wDst += 1;
			wSrc += 1;
		}
		*src = reinterpret_cast<const __m128i*>(wSrc);
		*dst = reinterpret_cast<__m128i*>(wDst);
	}
	break;
	case 4:
	{
		DWORD* dwDst = reinterpret_cast<DWORD*>(*dst);
		const DWORD* dwSrc = reinterpret_cast<const DWORD*>(*src);
		for (UINT32 i =0; i < byteCount/4; i+=1 )
		{
			DWORD tmp = *dwSrc;
			*dwDst = (tmp >> 24) | (tmp << 24) | ((tmp & 0x0000FF00) << 8) | ((tmp & 0x0000FF00) >> 8);
			dwDst += 1;
			dwSrc += 1;
		}
	}
	break;
	default:
		BYTE* bDst = reinterpret_cast<BYTE*>(*dst);
		const BYTE* bSrc = reinterpret_cast<const BYTE*>(*src);
		*src = reinterpret_cast<const __m128i*>(bSrc + byteCount);
		*dst = reinterpret_cast<__m128i*>(bDst + byteCount);
	}
}

inline void MemCopySwapBytes(void* dst, const void* src, const size_t bytes, const unsigned int swapbytes) {
	const __m128i* pMMSrc = reinterpret_cast<const __m128i*>(src);
	__m128i* pMMDst = reinterpret_cast<__m128i*>(dst);
	size_t count = bytes;
	size_t cnt = 0;
	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

	const __m128i wordMask = _mm_setr_epi8(0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06,
			                               0x09, 0x08, 0x0b, 0x0a, 0x0d, 0x0c, 0x0f, 0x0e);
	const __m128i dwordMask = _mm_setr_epi8(0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04,
			                                0x0b, 0x0a, 0x09, 0x08, 0x0f, 0x0e, 0x0d, 0x0c);

	CPU_INSTRUCTION_LEVEL cpuInstructionLevel = GetCpuInstructionLevel();
	if (cpuInstructionLevel < CPU_INSTRUCTION_LEVEL_SSE3)
	{
		ScalarSwapBytes(&pMMDst,&pMMSrc,count,swapbytes);
		return;
	}

	// only handle 2 and 4 bytes swapping

	if (swapbytes != 2 && swapbytes != 4)
	{
		MemCopy(pMMDst,pMMSrc,count);
		return;
	}

	// when size is < 16 rely, must use scalar swap

	if (count < INSTR_WIDTH_128)
	{
		ScalarSwapBytes(&pMMDst,&pMMSrc,count,swapbytes);

	}
	else
	{
		const __m128i shuffleMask = (swapbytes == 2) ? wordMask : dwordMask;

		// handle un-signed tiered copy up to 2 catch lines

		if (count < 2 * CACHE_LINE_SIZE)
		{
			cnt = count >> INSTR_128_SHIFT;
			for (UINT32 i=0; i < cnt; i += 1)
			{
				xmm0 = _mm_loadu_si128(pMMSrc);
				pMMSrc += 1;
				xmm0 = _mm_shuffle_epi8(xmm0, shuffleMask);
				_mm_storeu_si128(pMMDst,xmm0);
				pMMDst += 1;
			}
		}

		// handle aligned copy for > 2 cache lines

		else
		{
			// align destination to 16 if necessary
			UINT32 align = (UINT32)((UINT_PTR)pMMDst & TAIL_SIZE);
			if (align != 0)
			{
				align = INSTR_WIDTH_128 - align;
				cnt = align >> DWORD_SHIFT;
				ScalarSwapBytes(&pMMDst,&pMMSrc,cnt * sizeof(DWORD),swapbytes);
				cnt = align & BYTE_TAIL;

				// only words should remain, not bytes
				if (cnt > 0)
				{
					ASSERT(cnt % 2 == 0);
					ASSERT(swapbytes == 2);
					ScalarSwapBytes(&pMMDst,&pMMSrc,cnt,swapbytes);
				}
			}

			count -= align;
			if ((UINT_PTR)pMMSrc & TAIL_SIZE)
			{
				cnt = count >> DUAL_CACHE_SHIFT;
				for (UINT32 i=0; i<cnt; i += 1)
				{
					xmm0 = _mm_loadu_si128(pMMSrc);
					xmm1 = _mm_loadu_si128(pMMSrc + 1);
					xmm2 = _mm_loadu_si128(pMMSrc + 2);
					xmm3 = _mm_loadu_si128(pMMSrc + 3);
					xmm4 = _mm_loadu_si128(pMMSrc + 4);
					xmm5 = _mm_loadu_si128(pMMSrc + 5);
					xmm6 = _mm_loadu_si128(pMMSrc + 6);
					xmm7 = _mm_loadu_si128(pMMSrc + 7);
					pMMSrc += 8;

					xmm0 = _mm_shuffle_epi8(xmm0,shuffleMask);
					xmm1 = _mm_shuffle_epi8(xmm1,shuffleMask);
					xmm2 = _mm_shuffle_epi8(xmm2,shuffleMask);
					xmm3 = _mm_shuffle_epi8(xmm3,shuffleMask);
					xmm4 = _mm_shuffle_epi8(xmm4,shuffleMask);
					xmm5 = _mm_shuffle_epi8(xmm5,shuffleMask);
					xmm6 = _mm_shuffle_epi8(xmm6,shuffleMask);
					xmm7 = _mm_shuffle_epi8(xmm7,shuffleMask);

					_mm_store_si128(pMMDst,xmm0);
					_mm_store_si128(pMMDst + 1, xmm1);
					_mm_store_si128(pMMDst + 2, xmm2);
					_mm_store_si128(pMMDst + 3, xmm3);
					_mm_store_si128(pMMDst + 4, xmm4);
					_mm_store_si128(pMMDst + 5, xmm5);
					_mm_store_si128(pMMDst + 6, xmm6);
					_mm_store_si128(pMMDst + 7, xmm7);
					pMMDst += 8;
				}

				count &= TIERED_TAIL;
				if (count != 0)
				{
					cnt = count >> INSTR_128_SHIFT;
					for (UINT32 i=0; i<cnt ; i += 1)
					{
						xmm0 = _mm_loadu_si128(pMMSrc);
						pMMSrc += 1;
						xmm0 = _mm_shuffle_epi8(xmm0,shuffleMask);
						_mm_store_si128(pMMDst,xmm0);
						pMMDst += 1;
					}
				}
			}
		}

		count &= TAIL_SIZE;
		if (count != 0)
		{
			cnt = count >> DWORD_SHIFT;
			ScalarSwapBytes(&pMMDst, &pMMSrc, cnt * sizeof(DWORD), swapbytes);
			cnt = count & BYTE_TAIL;

			if (cnt > 0)
			{
				ASSERT(cnt % 2 == 0);
				ASSERT(swapbytes == 2);
				ScalarSwapBytes(&pMMDst, &pMMSrc, cnt, swapbytes);
			}
		}
	}
}

inline void SafeMemSet(void* dst, const int data, const size_t bytes) {
#ifndef _DEBUG && ISTDLIB_KMD
	::memset(dst,data,bytes);
#endif
}

inline void SafeMemCompare(const void* dst, const void* src, const size_t bytes) {
	::memcmp(dst, src, bytes);
}

inline void SafeMemMove(void* dst, const void* src, const size_t bytes) {
	if (dst != src)
	{
		if (src > dst && bytes)
		{
			size_t t = 0;
			do
			{
				static_cast<unsigned char*>(dst)[t] = static_cast<const unsigned char*>(src)[t];
			}
			while(++t != bytes);
		}
		else
		{
			size_t t = bytes-1;
			do
			{
				static_cast<unsigned char*>(dst)[t] = static_cast<const unsigned char*>(src)[t];
			}
			while(t-- != 0);
		}
	}
}

#define EMIT_R_MR(OPCODE,X,Y) \
	OPCODE                    \
    __asm _emit(0x00 + X*8 + Y)

#define EMIT_R_MR_OFFSET(OPCODE,X,Y,OFFSET) \
	OPCODE                                  \
	__asm _emit(0x80 + X*8 + Y)             \
	__asm _emit(OFFSET&0xFF)                \
	__asm _emit((OFFSET>>8)&0xFF)           \
	__asm _emit((OFFSET>>16)&0xFF)          \
	__asm _emit((OFFSET>>24)&0xFF)          \

#ifndef _MSC_VER
#define REG_EAX  0x00
#define REG_ECX  0x01
#define REG_EDX  0x02
#define REG_EBX  0x03
#define REG_ESP  0x04
#define REG_EBP  0x05
#define REG_ESI  0x06
#define REG_EDI  0x07
#define REG_XMM0 0x00
#define REG_XMM1 0x01
#define REG_XMM2 0x02
#define REG_XMM3 0x03
#define REG_XMM4 0x04
#define REG_XMM5 0x05
#define REG_XMM6 0x06
#define REG_XMM7 0x07
#endif //#if defined( _MSC_VER )

#define MOVNTDQA_OP     \
    __asm _emit 0x66     \
    __asm _emit 0x0F     \
    __asm _emit 0x38     \
    __asm _emit 0x2A

#define MOVNTDQA_R_MR(DST, SRC)                 \
    EMIT_R_MR(MOVNTDQA_OP, DST, SRC)

#define MOVNTDQA_R_MR_OFFSET(DST, SRC, OFFSET)  \
EMIT_R_MR_OFFSET(MOVNTDQA_OP, DST, SRC, OFFSET)

inline void FastBlockCopyFromUSWC_SSE4_1_movntdqa_movdqu(void* dst, const void* src ) {

}

inline void FastMemCopyFromWC(void* dst, const void* src, const size_t bytes, CPU_INSTRUCTION_LEVEL cpuInstructionLevel) {
	if (cpuInstructionLevel >= CPU_INSTRUCTION_LEVEL_SSE4_1)
	{
		BYTE* p_dst = (BYTE*)dst;
		BYTE* p_src = (BYTE*)src;

		size_t count = bytes;
		if (count >= sizeof(DHWORD))
		{
			// Streaming load must be 16-byte aligned but should be 64-bytes aligned for optimal performance

			const size_t doubleHexWordAlignBytes = GetAlignmentOffSet(p_src,sizeof(DHWORD));
			if (doubleHexWordAlignBytes)
			{
				MemCopy(p_dst, p_src, doubleHexWordAlignBytes);
				p_dst += doubleHexWordAlignBytes;
				p_src += doubleHexWordAlignBytes;
				count -= doubleHexWordAlignBytes;
			}

			ASSERT(IsAligned(p_src,sizeof(DHWORD)) == true);
			const size_t DoubleHexWordsToCopy = count / sizeof(DHWORD);
			if (DoubleHexWordsToCopy)
			{
				// Determine if the destination address is aligned

				const bool isDstDoubleQuadWordAligned = IsAligned(p_dst, sizeof(DQWORD));
				__m128i* pMMSrc = (__m128i*)(p_src);
				__m128i* pMMDst = (__m128i*)(p_dst);
				__m128i* xmm0, xmm1, xmm2, xmm3;
				if (isDstDoubleQuadWordAligned)
				{
#ifndef __GNC__
					_mm_mfence();
#endif
					for (size_t i=0; i<DoubleHexWordsToCopy; i++)
					{
						xmm0 = _mm_stream_load_si128(pMMSrc);
						xmm1 = _mm_stream_load_si128(pMMSrc + 1);
						xmm2 = _mm_stream_load_si128(pMMSrc + 2);
						xmm3 = _mm_stream_load_si128(pMMSrc + 3);
						pMMSrc += 4;
						p_dst += sizeof(DHWORD);
						p_src += sizeof(DHWORD);
						count -= sizeof(DHWORD);
					}
				}
			}
		}

		if (count)
		{
			MemCopy(p_dst, p_src, count);
		}
	}
	else
	{
		MemCopy(dst,src,bytes);
	}
}

inline void FastCpuBlt(BYTE* dst, const DWORD dstPitch, BYTE* src, const DWORD srcPitch, const DWORD stride, DWORD count) {
	do
	{
		MemCopy(dst, src, stride);
		dst += dstPitch;
		src += dstPitch;
	}
	while(--count > 0);
}

inline void FastCpuSet(BYTE* dst, const DWORD dstPitch, const DWORD value, const DWORD stride, DWORD count) {
	do
	{
		SafeMemSet(dst,value, stride);
		dst += dstPitch;
	}
	while(--count > 0);
}

inline void FastCpuBltFromUSWC(BYTE* dst, const DWORD dstPitch, BYTE* src, const DWORD srcPitch, const DWORD stride, DWORD count, CPU_INSTRUCTION_LEVEL level) {
	__declspec(align(16))BYTE backUpRegisters[16*4];
    void* tempPtr = (void*) backUpRegisters;
    do
    {
    	iSTD::FastMemCopyFromWC(dst,src,stride,level);
    	dst += dstPitch;
    	src += srcPitch;
    }
    while(--count > 0);
}

inline void FindWordBufferMinMax(WORD* pBuffer, const DWORD bytes, WORD &min, WORD &max) {
	PrefetchBuffer((BYTE*)pBuffer,bytes);
	WORD wValue = 0;
	WORD wMinValue = 0xffff;
	WORD wMaxValue = 0x0000;

	size_t count = bytes/sizeof(WORD);
	size_t i = 0;
	if (IsAligned(pBuffer,sizeof(WORD)))
	{
		const size_t DoubleQuadWordsPrefetch = sizeof(PREFETCH)/sizeof(DQWORD);
		const size_t WordPerPrefetch = sizeof(PREFETCH)/sizeof(WORD);
		const size_t WordsPerDoubleQuadWord = sizeof(DQWORD)/sizeof(WORD);

		Prefetch((BYTE*)pBuffer + sizeof(PREFETCH));
		Prefetch((BYTE*)pBuffer + 2 * sizeof(PREFETCH));

		// Find min/max per cacheline of values

		if (count >= WordsPerDoubleQuadWord)
		{
			const size_t doubleQuadwordAlignWords = GetAlignmentOffSet(pBuffer,sizeof(DQWORD)/sizeof(WORD));
			if (doubleQuadwordAlignWords)
			{
				for (i=0; i < doubleQuadwordAlignWords; i++)
				{
					wValue = *pBuffer++;
					wMinValue = Min(wMinValue,wValue);
					wMaxValue = Max(wMaxValue,wValue);
				}
				count -= doubleQuadwordAlignWords;
			}

			// Find min/max per cacheline of values
			if (count >= WordsPerDoubleQuadWord)
			{
				__m128i mValue128i;
				// Need convert unsigned values to signed values
				// since min/max is signed up

				__m128i mSignedScale128i = _mm_set1_epi8((WORD)0x8000);

				//Signed min/max initialization
				__m128i mMinValue128i = _mm_set1_epi16(wMinValue-(WORD)0x8000);
				__m128i mMaxValue128i = _mm_set1_epi16(wMaxValue-(WORD)0x8000);

				while(count >= WordPerPrefetch)
				{
					Prefetch((BYTE*)pBuffer + 2 * sizeof(PREFETCH));

					// Process catcheline value per pass
					count -= WordPerPrefetch;

					for (i=0; i < DoubleQuadWordsPrefetch; i++)
					{
						// Get double-quadword values
						mValue128i = *(__m128i*)pBuffer;
						pBuffer += WordsPerDoubleQuadWord;

						// make values signed
						mValue128i = _mm_sub_epi16(mValue128i,mSignedScale128i);

						// Determine parallel min/max
						mMinValue128i = _mm_min_epi16(mMinValue128i,mValue128i);
						mMaxValue128i = _mm_max_epi16(mMaxValue128i,mValue128i);
					}
				}

				while (count >= WordsPerDoubleQuadWord)
				{
					// Process catchline value per pass
					count -= WordsPerDoubleQuadWord;

					// Get double-quadword values
					mValue128i = *(__m128i*)pBuffer;
					pBuffer += WordsPerDoubleQuadWord;

					mValue128i = _mm_sub_epi16(mValue128i, mSignedScale128i);
					mMinValue128i = _mm_min_epi16(mMinValue128i,mValue128i);
					mMaxValue128i = _mm_max_epi16(mMaxValue128i,mValue128i);
				}

				// determine wMinValue
				// make values unsigned

				mMinValue128i = _mm_add_epi16(mMinValue128i,mSignedScale128i);

				// Extract each value in double-quadword to find minimum

				wValue = (WORD)_mm_extract_epi16( mMinValue128i, 0 );
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,1);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,2);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,3);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,4);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,5);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,6);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMinValue128i,7);
				wMinValue = Min(wMinValue,wValue);

				// determine wMaxValue

				mMaxValue128i = _mm_add_epi16(mMaxValue128i,mSignedScale128i);

				// Extract each value in double-quadword to find maximum
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,0);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,1);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,2);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,3);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,4);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,5);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,6);
				wMaxValue = Max(wMaxValue,wValue);
				wValue = (WORD)_mm_extract_epi16(mMaxValue128i,7);
				wMaxValue = Max(wMaxValue,wValue);
			}
		}
	}

	while (count > 0)
	{
		count -= 1;
		wValue = *pBuffer++;
		wMinValue = Min(wMinValue,wValue);
		wMaxValue = Max(wMaxValue,wValue);
	}

	min = wMinValue;
	max = wMaxValue;
}

inline void FindWordBufferMinMaxRestart(WORD* pBuffer, const DWORD bytes, const WORD restart, WORD &min, WORD &max,
		                                CPU_INSTRUCTION_LEVEL cpuInstructionLevel) {
	WORD wValue = 0;
	WORD wMinValue = 0xffff;
	WORD wMaxValue = 0x0000;
	size_t count = bytes/sizeof(WORD);
	size_t i = 0;
	if (IsAligned(pBuffer,sizeof(WORD)) && cpuInstructionLevel >= CPU_INSTRUCTION_LEVEL_SSE4_1 )
	{
		const DWORD DoubleQuadWordPerPrefetch = sizeof(PREFETCH)/sizeof(DWORD);
		const DWORD WordsPerPrefetch = sizeof(PREFETCH)/sizeof(WORD);
		const DWORD WordsPerDoubleQuadWord = sizeof(DQWORD)/sizeof(WORD);

		Prefetch((BYTE*)pBuffer + sizeof(PREFETCH));
		Prefetch((BYTE*)pBuffer + 2 * sizeof(PREFETCH));

		if (count >= WordsPerDoubleQuadWord)
		{
			const size_t doubleQuadWordAlignWords = GetAlignmentOffSet(pBuffer,sizeof(DQWORD))/sizeof(WORD);
			if (doubleQuadWordAlignWords)
			{
				for (i=0; i<doubleQuadWordAlignWords; i++)
				{
					wValue = *pBuffer++;
					if (wValue == restart)
					{
						continue;
					}
					wMinValue = Min(wMinValue,wValue);
					wMaxValue = Max(wMaxValue,wValue);
				}
				count -= doubleQuadWordAlignWords;
			}

			if (count >= WordsPerDoubleQuadWord)
			{
				__m128i mInput, mRestarts, mMask;
				__m128i mAll_ones;
				__m128i mMinValue128i, mMaxValue128i;

				while (count >= WordsPerPrefetch)
				{
					Prefetch((BYTE*)pBuffer + 2 * sizeof(PREFETCH));
					count -= WordsPerPrefetch;
					for (i=0; i < DoubleQuadWordPerPrefetch; i++)
					{
						mInput = *(__m128i*)pBuffer;
						pBuffer += WordsPerDoubleQuadWord;
						mMask = _mm_andnot_si128(_mm_cmpeq_epi16(mInput,mRestarts),mAll_ones);
						mMinValue128i = _mm_blendv_epi8(mMinValue128i,_mm_min_epu16(mMinValue128i,mInput),mMask);
						mMaxValue128i = _mm_blendv_epi8(mMaxValue128i,_mm_min_epu16(mMaxValue128i,mInput),mMask);
					}
				}

				while (count >= WordsPerDoubleQuadWord)
				{
					// Process double-quadwords values per pass
					count -= WordsPerDoubleQuadWord;

					// Get double-quadwords values
					mInput = *(__m128i*)pBuffer;
					pBuffer += WordsPerDoubleQuadWord;

					// Make mask of non-restart index field
					mMask -= _mm_andnot_si128(_mm_cmpeq_epi16(mInput,mRestarts),mAll_ones);

					// Copy min and max fields for non-restarts
					mMinValue128i = _mm_blendv_epi8(mMinValue128i,_mm_min_epu16(mMinValue128i,mInput),mMask);
					mMaxValue128i = _mm_blendv_epi8(mMaxValue128i,_mm_min_epu16(mMaxValue128i,mInput),mMask);
				}

				// Determine mMinValue
				// Extract each value in double-quadWord to find minimum

				wValue =(WORD)_mm_extract_epi16(mMinValue128i,0);
				wMinValue = Min(wMinValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMinValue128i,1);
				wMinValue = Min(wMinValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMinValue128i,2);
				wMinValue = Min(wMinValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMinValue128i,3);
				wMinValue = Min(wMinValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMinValue128i,4);
				wMinValue = Min(wMinValue,wValue);
				wValue = (WORD)_mm_extract_epi16( mMinValue128i, 5 );
				wMinValue = Min( wMinValue, wValue );
				wValue = (WORD)_mm_extract_epi16( mMinValue128i, 6 );
			    wMinValue = Min( wMinValue, wValue );
			    wValue = (WORD)_mm_extract_epi16( mMinValue128i, 7 );
				wMinValue = Min( wMinValue, wValue );

				// Determine mMaxValue
				// Extract each value in double=quadWords to find maximum

				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,0);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,1);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,2);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,3);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,4);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,5);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,6);
				wMaxValue = Max(wMaxValue,wValue);
				wValue =(WORD)_mm_extract_epi16(mMaxValue128i,7);
				wMaxValue = Max(wMaxValue,wValue);
			}
		}
	}

	while(count > 0)
	{
		count -= 1;
		wValue = *pBuffer++;
		if (wValue == restart)
		{
			continue;
		}
		wMinValue = Min(wMinValue,wValue);
		wMaxValue = Max(wMaxValue,wValue);
	}

	min = wMinValue;
	max = wMaxValue;
}

inline void FindDWordBufferMinMax(DWORD* pBuffer, const DWORD bytes, DWORD &min, DWORD &max) {
	PrefetchBuffer((BYTE*)pBuffer,bytes);
	DWORD wvalue = 0;
	DWORD wMinValue = 0xffffffff;
	DWORD wMaxValue = 0x00000000;

	DWORD count = bytes/sizeof(DWORD);
	DWORD i = 0;

	if (IsAligned(pBuffer,sizeof(DWORD)))
	{
		const DWORD DoubleQuadWordsPerPrefetch = sizeof(PREFETCH)/sizeof(DQWORD);
		const DWORD DWordPerPrefetch = sizeof(PREFETCH)/sizeof(DWORD);
		const DWORD DWordPerDoubleQuadWord = sizeof(DQWORD)/sizeof(DWORD);

		Prefetch((BYTE*)pBuffer + sizeof(PREFETCH));
		Prefetch((BYTE*)pBuffer + 2 + sizeof(PREFETCH));

		// Find min/max per cacheline of values

		if (count >= DWordPerDoubleQuadWord)
		{
			const DWORD doubleQuadWordAlignWords = GetAlignmentOffSet(pBuffer,sizeof(DWORD))/sizeof(DWORD);

			// if pBuffer is not double-quadword aligned then process
			if (doubleQuadWordAlignWords)
			{
				for (i=0; i < doubleQuadWordAlignWords; i++)
				{
					wvalue = *pBuffer++;
					wMinValue = Min(wMinValue,wvalue);
					wMaxValue = Max(wMaxValue,wvalue);
				}
				count -= doubleQuadWordAlignWords;
			}

			// Find min/max per catchlines of values

			if (count >= DWordPerPrefetch)
			{
				__m128i mValue128i;
				__m128 mValue128;

				// Signed min/max initialization
				// need extra QWORD bits SSE2 FP conversion
				__m128 mMinValue128 = _mm_set1_ps((float)((QWORD)wMinValue));
				__m128 mMaxValue128 = _mm_set1_ps((float)(wMaxValue));

				while (count >= DWordPerPrefetch)
				{
					Prefetch((BYTE*)pBuffer + 2 * sizeof(PREFETCH));

					// Process cacheline values per pass
					count -= DWordPerPrefetch;
					for (i=0; i < DoubleQuadWordsPerPrefetch; i++)
					{
						mValue128i = *(__m128i*)pBuffer;
						pBuffer += DWordPerDoubleQuadWord;

						mValue128 = _mm_cvtepi32_ps(mValue128i);
						mMinValue128 = _mm_min_ps(mMinValue128,mValue128);
						mMaxValue128 = _mm_max_ps(mMaxValue128,mValue128);
					}
				}

				// Process double-quadword values per pass for remainder
				while (count >= DWordPerDoubleQuadWord)
				{
					count -= DWordPerDoubleQuadWord;
					mValue128i = *(__m128i*)pBuffer;
					pBuffer += DWordPerDoubleQuadWord;

					mValue128 = _mm_cvtepi32_ps(mValue128i);
					mMinValue128 = _mm_min_ps(mMinValue128,mValue128);
					mMaxValue128 = _mm_max_ps(mMaxValue128,mValue128);
				}

				// Determine wMinValue
				// Convert back to DWORD
				__m128i mMinValue128i = _mm_cvtps_epi32(mMinValue128);

				wvalue = (DWORD)_mm_cvtsi128_si32(mMinValue128i);
				wMinValue = Min(wMinValue,wvalue);
				wvalue = (DWORD)_mm_cvtsi128_si32(_mm_srli_si128(mMinValue128i,4));
				wMinValue = Min(wMinValue,wvalue);
				wvalue = (DWORD)_mm_cvtsi128_si32(_mm_srli_si128(mMinValue128i,8));
				wMinValue = Min(wMinValue,wvalue);
				wvalue = (DWORD)_mm_cvtsi128_si32(_mm_srli_si128(mMinValue128i,12));
				wMinValue = Min(wMinValue,wvalue);

				__m128i mMaxValue128i = _mm_cvtps_epi32(mMaxValue128);
				wvalue = (DWORD)_mm_cvtsi128_si32(mMaxValue128i);
				wMaxValue = Max(wMaxValue,wvalue);
				wvalue = (DWORD)_mm_cvtsi128_si32(_mm_srli_si128(mMaxValue128i,4));
				wMaxValue = Max(wMaxValue,wvalue);
				wvalue = (DWORD)_mm_cvtsi128_si32(_mm_srli_si128(mMaxValue128i,8));
				wMaxValue = Max(wMaxValue,wvalue);
				wvalue = (DWORD)_mm_cvtsi128_si32(_mm_srli_si128(mMaxValue128i,12));
				wMaxValue = Max(wMaxValue,wvalue);
			}
		}
	}

	while (count > 0)
	{
		count -= 1;
		wvalue = *pBuffer++;
		wMinValue = Min(wMinValue,wvalue);
		wMaxValue = Max(wMaxValue,wvalue);
	}

	min = wMinValue;
	max = wMaxValue;
}



}


#endif /* MEMCOPY_H_ */




















































