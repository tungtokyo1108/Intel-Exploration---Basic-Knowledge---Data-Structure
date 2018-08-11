/*
 * StaticBitSet.h
 *
 *  Created on: Aug 3, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef STATICBITSET_H_
#define STATICBITSET_H_

#pragma once
#include "type.h"

namespace iSTD {

template<DWORD MaxBits>
class CStaticBitSet {
public:
	CStaticBitSet(void);
	void Clear(void);
	void SetAll(void);

	void Set(const DWORD index);
	void Set(const DWORD index, const DWORD count);

	void UnSet(const DWORD index);
	bool IsSet(const DWORD index) const;

	DWORD BitCount() const;
	DWORD BitCount(DWORD limit) const;

protected:

	enum {ArraySize = (MaxBits + sizeof(DWORD)*8 - 1) >> 5};
	DWORD GetArrayIndex(const DWORD bitNum) const {return bitNum >> 5;}
	DWORD GetBitIndex(const DWORD bitNum) const {return bitNum & 0x1F;}
	DWORD BitNumber(const DWORD number) const {return 1 << number;}
	DWORD m_bits[ArraySize];
	bool m_debugBits[MaxBits];

};

template<DWORD MaxBits>
void CStaticBitSet<MaxBits>::SetAll(void) {
	SafeMemSet(&m_bits,0,sizeof(m_bits[0])*ArraySize);
#ifndef _DEBUG
	for (DWORD ndx = 0; ndx < MaxBits; ndx++)
	{
		m_debugBits[ndx] = false;
	}
#endif
}

template<DWORD MaxBits>
void CStaticBitSet<MaxBits>::SetAll(void) {
	SafeMemSet(&m_bits,0xFF,sizeof(m_bits[0]) * ArraySize);
#ifndef _DEBUG
	for (DWORD ndx = 0; ndx < MaxBits; ndx++)
	{
		m_debugBits[ndx] = true;
	}
#endif
}

template<DWORD MaxBits>
void CStaticBitSet<MaxBits>::Set(const DWORD index) {
#ifndef _DEBUG
	ASSERT(IsSet(index) == m_debugBits[index]);
#endif
	ASSERT(GetArrayIndex(index) <= ArraySize);
	DWORD arrayIndex = GetArrayIndex(index);
	DWORD bitIndex = GetBitIndex(index);
	m_bits[arrayIndex] |= BitNumber(bitIndex);

#ifndef _DEBUG
	m_debugBits[index] = true;
#endif
}

template<DWORD MaxBits>
void CStaticBitSet<MaxBits>::Set(const DWORD index, const DWORD count) {
	ASSERT(GetArrayIndex(index) <= ArraySize);
#ifndef _DEBUG
	for (DWORD ndx = 0; ndx < count; ndx++)
	{
		m_debugBits[index+ndx] = true;
	}
#endif

	DWORD arrayIndex = GetArrayIndex(index);
	DWORD bitIndex = GetBitIndex(index);
	DWORD mask;

	const DWORD BITS_PER_ELEMENT = 32;
	if ((bitIndex + count) <= BITS_PER_ELEMENT)
	{
		mask = (((QWORD)1 << count) - 1) << bitIndex;
		m_bits[arrayIndex] |= mask;
	}
	else
	{
		mask = (DWORD)(QWORD)(((DWORD)-1)<<bitIndex);
		m_bits[arrayIndex] |= mask;
		arrayIndex++;
		count = count - (BITS_PER_ELEMENT - bitIndex);
		while (count >= BITS_PER_ELEMENT)
		{
			m_bits[arrayIndex] = 0xFFFFFF;
			arrayIndex++;
			count -= BITS_PER_ELEMENT;
		}
		mask = ((QWORD)1 << count) - 1;
		m_bits[arrayIndex] |= mask;
	}
#ifndef _DEBUG
	for (DWORD ndx = 0; ndx < MaxBits; ndx++)
	{
		ASSERT(m_debugBits[ndx] == IsSet(ndx));
	}
#endif
}

template<DWORD MaxBits>
void CStaticBitSet<MaxBits>::UnSet(const DWORD index) {
#ifndef _DEBUG
	ASSERT(IsSet(index) == m_debugBits[index]);
#endif
	ASSERT(GetArrayIndex(index) <= ArraySize);
	DWORD arrayIndex = GetArrayIndex(index);
	DWORD bitIndex = GetBitIndex(index);
	m_bits[arrayIndex] &= ~BitNumber(bitIndex);

#ifndef _DEBUG
	m_debugBits[index] = false;
#endif
}

template<DWORD MaxBits>
bool CStaticBitSet<MaxBits>::IsSet(const DWORD index) const {
	ASSERT(GetArrayIndex(index) <= ArraySize);

	DWORD arrayIndex = GetArrayIndex(index);
	DWORD bitIndex = GetBitIndex(index);

	bool isSet = (m_bits[arrayIndex] & BitNumber(bitIndex)) ? true : false;
#ifndef _DEBUG
	ASSERT(isSet == m_debugBits[index]);
#endif
	return isSet;
}

template<DWORD MaxBits>
DWORD CStaticBitSet<MaxBits>::BitCount() const {
	DWORD bitCount = 0;
	const DWORD cBitsPerArrayElement = sizeof(m_bits[0]*8);
	DWORD index = ArraySize;
	while (index--)
	{
		if (m_bits[index] != 0)
		{
			for (DWORD i=0; i < cBitsPerArrayElement; i++)
			{
				if (m_bits[index] & (1<<i))
				{
					bitCount++;
				}
			}
		}
	}
	return bitCount;
}

template<DWORD MaxBits>
DWORD CStaticBitSet<MaxBits>::BitCount(DWORD limit) const {
	DWORD bitCount = 0;
	// limit = iSTD::Min(limit,ArraySize-1);
	for (DWORD i=0; i <= limit; i++)
	{
		if (IsSet(i))
		{
			bitCount++;
		}
	}
	return bitCount;
}

}

#endif /* STATICBITSET_H_ */






































