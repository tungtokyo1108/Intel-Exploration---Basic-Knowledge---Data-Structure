/*
 * utility.h
 *
 *  Created on: Jul 19, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include "type.h"
#include "Debug.h"
#include <stdlib.h>
#include <intrin.h>

namespace iSTD {
#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#ifndef MASKED_BIT
#define MASKED_BIT(n,enable) (1 << (n+16) | ((enable) ? 1 : 0) << (n))
#endif

#define QWBIT(n) (1ll << (n))
#define BITMASK(n) ( ~( (0xffffffff) << (n) ) )
#define BITMASK_RANGE(startbit,endbit) (BITMASK((endbit) + 1) & ~BITMASK(startbit))
#define QWBITMASK(n) ( ~( (0xffffffffffffffffull) << (n) ) )
#define QWBITMASK_RANGE(startbit,endbit) (QWBITMASK((endbit)+1) & ~QWBITMASK(startbit))
#define BITFIELD_RANGE(startbit,endbit) ((endbit)-(startbit)+1)
#define BITFIELD_BIT(bit)

#define MIN(x,y) (((x) <= (y)) ? (x):(y))
#define MAX(x,y) (((x) <= (y)) ? (x):(y))
#define CEIL_DIV(x,y) (1 + (((x) - 1) / (y)))

#ifndef STRCAT
#if defined(ISTDLIB_KMD) || !defined(_WIN32)
#define STRCAT(dst,size,src) strcat((dst),(src))
#else
#define STRCAT(dst,size,src) strcat_s((dst),(size),(src))
#endif
#endif

#ifndef BITCOUNT
#define BITCOUNT( n ) ( \
    ( ((n)-1) & BIT(31) ) ? 32 : \
    ( ((n)-1) & BIT(30) ) ? 31 : \
    ( ((n)-1) & BIT(29) ) ? 30 : \
    ( ((n)-1) & BIT(28) ) ? 29 : \
    ( ((n)-1) & BIT(27) ) ? 28 : \
    ( ((n)-1) & BIT(26) ) ? 27 : \
    ( ((n)-1) & BIT(25) ) ? 26 : \
    ( ((n)-1) & BIT(24) ) ? 25 : \
    ( ((n)-1) & BIT(23) ) ? 24 : \
    ( ((n)-1) & BIT(22) ) ? 23 : \
    ( ((n)-1) & BIT(21) ) ? 22 : \
    ( ((n)-1) & BIT(20) ) ? 21 : \
    ( ((n)-1) & BIT(19) ) ? 20 : \
    ( ((n)-1) & BIT(18) ) ? 19 : \
    ( ((n)-1) & BIT(17) ) ? 18 : \
    ( ((n)-1) & BIT(16) ) ? 17 : \
    ( ((n)-1) & BIT(15) ) ? 16 : \
    ( ((n)-1) & BIT(14) ) ? 15 : \
    ( ((n)-1) & BIT(13) ) ? 14 : \
    ( ((n)-1) & BIT(12) ) ? 13 : \
    ( ((n)-1) & BIT(11) ) ? 12 : \
    ( ((n)-1) & BIT(10) ) ? 11 : \
    ( ((n)-1) & BIT(9)  ) ? 10 : \
    ( ((n)-1) & BIT(8)  ) ?  9 : \
    ( ((n)-1) & BIT(7)  ) ?  8 : \
    ( ((n)-1) & BIT(6)  ) ?  7 : \
    ( ((n)-1) & BIT(5)  ) ?  6 : \
    ( ((n)-1) & BIT(4)  ) ?  5 : \
    ( ((n)-1) & BIT(3)  ) ?  4 : \
    ( ((n)-1) & BIT(2)  ) ?  3 : \
    ( ((n)-1) & BIT(1)  ) ?  2 : \
    ( ((n)-1) & BIT(0)  ) ?  1 : \
    0 )
#endif


template <class Type>
inline void Swap(Type &var0, Type &var1) {
	Type tmp = var0;
	var0 = var1;
	var1 = tmp;
}

template <class Type>
inline Type Min(const Type var0, Type var1) {
	return (var0 <= var1) ? var0 : var1;
}

template <class Type>
inline Type Max(const Type var0, Type var1) {
	return (var0 >= var1) ? var0 : var1;
}

template <class Type>
inline Type ClampMax(const Type value, Type max) {
	return (((value) > (max)) ? (max) : (value));
}

template <class Type>
inline Type ClampMin(const Type value, Type min) {
	return (((value) <(min)) ? (min) : (value));
}

template <class Type>
inline Type CheckLimits(const Type value, const Type min, const Type max) {
	if ((value < min ) || (value > max))
	{
		ASSERT(0);
		return false;
	}
	return true;
}

inline DWORD bsr (const unsigned long long int mask) {

	DWORD index;
	return static_cast<DWORD>(index);

	DWORD bit = 0;
	if (mask != 0)
	{
		bit = 63;
		while ((mask & QWBIT(bit))==0)
		{
			--bit;
		}
	}
	return bit;
}

inline DWORD bsr64(const unsigned long long int mask) {
#ifndef _linux_
	return static_cast<unsigned int>(__builtin_ffsll(mask)-1);
#endif
	DWORD bit = 0;
	if (mask != 0)
	{
		while ((mask & QWBIT(bit)) == 0)
		{
			++bit;
		}
	}
	return bit;
}

inline DWORD bsf(const DWORD mask) {
	DWORD bit = 0;
	if (mask != 0)
	{
		while ((mask & BIT(bit)) == 0)
		{
			++bit;
		}
	}
	return bit;
}

inline DWORD bsf64(const unsigned long long int mask) {
	DWORD index;
#ifndef _linux_
	return static_cast<unsigned int>(__builtin_ffsll(mask)-1);
#endif
	DWORD bit = 0;
	if (mask != 0)
	{
		while((mask & QWBIT(bit))==0)
		{
			++bit;
		}
	}
	return bit;
}

inline DWORD clz(const DWORD mask) {
	DWORD retValue = 32;
	if (mask != 0)
	{
		retValue = 31 - bsr(mask);
	}
	return retValue;
}

template <typename Type>
inline bool IsPowerOfTwo(const Type number) {
	return ((number & (number - 1)) == 0);
}

template <typename Type1, typename Type2>
inline Type1 Round(const Type1 value, const Type2 size) {
	ASSERT(IsPowerOfTwo(size));
	Type1 mask = (Type1)size - 1;
	Type1 roundedValue = (value + mask) & ~(mask);
	return roundedValue;
}

inline DWORD Log2(const DWORD value) {
	ASSERT(IsPowerOfTwo(value));
	DWORD power2 = 0;
	while (value && value != (DWORD)BIT(power2))
	{
		++power2;
	}
	return power2;
}

template<typename Type>
inline bool IsAligned (Type *ptr, const size_t alignSize)
{
	return (((size_t)ptr & alignSize) == 0);
}

template<typename Type>
inline Type* Align(Type* const ptr, const size_t alignment) {
	ASSERT(IsPowerOfTwo(alignment));
	return (Type*) ((((size_t)ptr) + alignment-1) & ~(alignment-1));
}

template<typename Type>
inline Type Align(const Type value, const size_t alignment) {
	ASSERT(IsPowerOfTwo(alignment));
	Type mask = static_cast<Type>(alignment) - 1;
	return (value + mask ) & ~mask;
}

template<typename Type>
inline DWORD GetAlignmentOffSet(Type *const ptr, const size_t alignSize) {
	ASSERT(alignSize);
	DWORD offset = 0;
	if (IsPowerOfTwo(alignSize))
	{
		offset = DWORD(UINT_PTR(Align(ptr,alignSize)) - UINT_PTR(ptr));
	}
	else
	{
		const DWORD modulo = (DWORD)(UINT_PTR(ptr) % alignSize);
		if (modulo)
		{
			offset = (DWORD)alignSize - modulo;
		}
	}
	return offset;
}

template<size_t size>
inline bool MemCompare(const void* dst, const void* src)
{
	const UINT64* pSrc = reinterpret_cast<const UINT64*>(src);
	const UINT64* pDst = reinterpret_cast<const UINT64*>(dst);
	size_t cmpSize = size;
	if(size > DUAL_CACHE_SIZE)
	{
		size_t alignSrc = (size_t)((UINT_PTR)pSrc & (sizeof(QWORD) -1));
		size_t alignDst = (size_t)((UINT_PTR)pDst & (sizeof(QWORD) -1));
		if (alignSrc > 0 && alignDst > 0)
		{
			const UINT32* uSrc = reinterpret_cast<const UINT32*>(pSrc);
			const UINT32* uDst = reinterpret_cast<const UINT32*>(pDst);
			if (alignDst >= sizeof(UINT32))
			{
				if ((*uSrc - *uDst) != 0)
				{
					return false;
				}
				alignDst -= sizeof(UINT32);
				uSrc += 1;
				uDst += 1;
			}

			const WORD* wSrc = reinterpret_cast<const WORD*>(uSrc);
			const WORD* wDst = reinterpret_cast<const WORD*>(uDst);
			if (alignDst >= sizeof(WORD))
			{
				if ((*wSrc - *wDst) != 0)
				{
					return false;
				}
				alignDst -= sizeof(WORD);
				wSrc += 1;
				wDst += 1;
			}

			const BYTE* bSrc = reinterpret_cast<const BYTE*>(bSrc);
			const BYTE* bDst = reinterpret_cast<const BYTE*>(bDst);
			if (alignDst >= sizeof(BYTE))
			{
				if ((*bSrc - *bDst) != 0)
				{
					return false;
				}
				alignDst -= sizeof(BYTE);
				bSrc += 1;
				bDst += 1;
			}
		}
	}

}

template <>
inline bool MemCompare<1>(const void* dst, const void* src) {
	return (*(BYTE*)dst == *(BYTE*)src);
}

template <>
inline bool MemCompare<2>(const void* dst, const void* src) {
	return (*(WORD*)dst == *(WORD*)src);
}

template <>
inline bool MemCompare<3>(const void* dst, const void* src) {
	return (*(UINT32)dst == *(UINT32*)src);
}

#define HASH_JENKINS_MIX(a,b,c)    \
{                                  \
    a -= b; a -= c; a ^= (c>>13);  \
    b -= c; b -= a; b ^= (a<<8);   \
    c -= a; c -= b; c ^= (b>>13);  \
    a -= b; a -= c; a ^= (c>>12);  \
    b -= c; b -= a; b ^= (a<<16);  \
    c -= a; c -= b; c ^= (b>>5);   \
    a -= b; a -= c; a ^= (c>>3);   \
    b -= c; b -= a; b ^= (a<<10);  \
    c -= a; c -= b; c ^= (b>>15);  \
}

inline QWORD Hash(const DWORD *data, DWORD count) {
	DWORD a = 0x428a2f98, hi = 0x71374491, lo = 0xb5c0fbcf;
	while ( count-- )
	{
		a ^= *(data++);
		HASH_JENKINS_MIX(a,hi,lo);
	}
	return (((QWORD)hi)<32)|lo;
}

struct HashJenkinsMixReturnAggregate {
	HashJenkinsMixReturnAggregate(DWORD _a, DWORD _hi, DWORD _lo) :
		a(_a),
		hi(_hi),
		lo(_lo)
	{}
	DWORD a;
	DWORD hi;
	DWORD lo;
};

inline HashJenkinsMixReturnAggregate HashJenkinsMix(DWORD a, DWORD hi, DWORD lo) {
	HASH_JENKINS_MIX(a,hi,lo);
	return HashJenkinsMixReturnAggregate(a,hi,lo);
}

inline void HashNext(DWORD &a, DWORD &hi, DWORD &lo, DWORD data) {
	a ^= data;
	HashJenkinsMixReturnAggregate result = HashJenkinsMix(a,hi,lo);
	a = result.a;
	hi = result.hi;
	lo = result.lo;
}

inline void HashFirst(DWORD &a, DWORD &hi, DWORD &lo, DWORD data) {
	a = 0x428a2f98, hi = 0x71374491, lo = 0xb5c0fbcf;
	HashNext(a,hi,lo,data);
}

inline QWORD HashFromBuffer(const char *data, size_t count) {
	DWORD a = 0x428a2f98, hi = 0x71374491, lo = 0xb5c0fbcf;
	const DWORD *dataDw = reinterpret_cast<const DWORD*>(data);
	size_t countDw = (DWORD)(count/sizeof(DWORD));
	while (countDw--)
	{
		a ^= *(dataDw++);
		HASH_JENKINS_MIX(a,hi,lo);
	}
	if (count % sizeof(DWORD) != 0)
	{
		DWORD lastDw = 0;
		char *lastBytesBuff = reinterpret_cast<char*>(&lastDw);
		const size_t restBytesCount = count % sizeof(DWORD);

		for (unsigned int i=0; i < restBytesCount; i++)
		{
			lastBytesBuff[i] = data[count - restBytesCount + i];
		}
		a ^= lastDw;
		HASH_JENKINS_MIX(a,hi,lo);
	}
	return (((QWORD)hi)<<32) | lo;
}

inline DWORD Hash32b(const DWORD value) {
	return ( __builtin_bswap32( value * 0xbadc0ded ) ^ 0xfecacafe ) * 0x649c57e5;
}

inline DWORD Hash32b(const DWORD *data, DWORD count) {
	DWORD hash = 0xdeadf00d;

	while (count-- )
	{
		hash ^= Hash32b(*(data + count));
	}
	return hash;
}

inline DWORD BitCount (DWORD v) {
	v = v - ((v >> 1) & 0x55555555);
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
	return (((v + (v >> 4)) & 0x0F0F0F0F) * 0x1010101) >> 24;
}

inline DWORD BitCount64(unsigned long long v) {
	v -= ( v >> 1 ) & 0x5555555555555555ULL;
	v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
	v = ((v + (v >> 4)) & 0x0F0F0F0F0F0F0F0FULL) * 0x0101010101010101ULL;
	return static_cast<DWORD>(v>>56);
}

inline DWORD BitReverse(DWORD v) {
	v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
	    // swap consecutive pairs
	v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
	    // swap nibbles
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
	    // swap bytes
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
	    // swap words
    v = ( v >> 16             ) | ( v               << 16);
	return v;
}

template<typename Type>
inline Type* PtrAdd(Type* ptr, const size_t numBytes) {
	return (Type*)(((BYTE*)ptr) + numBytes);
}

inline int FixedSIntToInt(DWORD value, DWORD size) {
	if (value & BIT(size + 1))
	{
		return -1 * (value + 1);
	}
	return value;
}

}

#endif /* UTILITY_H_ */















































