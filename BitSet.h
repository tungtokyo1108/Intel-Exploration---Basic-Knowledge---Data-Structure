/*
 * BitSet.h
 *
 *  Created on: Jul 27, 2018
 *      Author: tungdang
 */

#ifndef BITSET_H_
#define BITSET_H_

#pragma once
#include "Object.h"
#include "utility.h"
#include "MemCopy.h"
#include "Threading.h"

namespace iSTD {

#define BitSetTemplateList class CAllocatorType
#define CBitSetType        CBitSet<CAllocatorType>

template <int T> struct TPtrSize
{};

// Specialization for 32bit poiters:

template <> struct TPtrSize<4> {
	typedef unsigned int type;
	static inline type Bit(const DWORD index)
	{
		return BIT(index);
	}

	static inline DWORD Bsf(const type mask)
	{
		return iSTD::bsf(mask);
	}

	static inline DWORD Bsr(const type mask)
	{
		return iSTD::bsr(mask);
	}

	static inline DWORD Count(const type mask)
	{
		return iSTD::BitCount(mask);
	}
};

// Specialization for 64bit pointers:

template <> struct TPtrSize<8> {
	typedef unsigned long long type;
	static inline type Bit(const DWORD index)
	{
		return QWBIT(index);
	}

	static inline DWORD Bsf(const type mask)
	{
		return iSTD::bsf64(mask);
	}

	static inline DWORD Bsr(const type mask)
	{
		return iSTD::bsr64(mask);
	}

	static inline DWORD Count(const type mask)
	{
		return iSTD::BitCount64(mask);
	}
};

template <BitSetTemplateList>
class CBitSet : public CObject<CAllocatorType> {
private:
	static const DWORD BITS_PER_BYTE = 8;
	typedef DWORD BITSET_ARRAY_TYPE;
	static const DWORD cBitsPerArrayElement = sizeof(BITSET_ARRAY_TYPE) * BITS_PER_BYTE;
	static const DWORD cBitsInPtr = sizeof(BITSET_ARRAY_TYPE*) * 8;
	typedef TPtrSize<sizeof(void*)>CPtrSize;
	typedef CPtrSize::type bitptr_t;

public:
	CBitSet(void);
	CBitSet(DWORD size);
	CBitSet(const CBitSetType& other);

	virtual ~CBitSet(void);
	void Resize(DWORD size);
	void Clear(void);
	void SetAll(void);
	void Invert(void);

	bool IsEmpty() const;
	bool IsEmpty(DWORD start, DWORD length) const;
	bool IsSet(DWORD index) const;
	bool Intersets(const CBitSetType& other) const;

	DWORD GetNextMember(DWORD start) const;
	void Set(DWORD index);
	void Set(const CBitSetType& other);
	void UnSet(DWORD index);
	void UnSet(const CBitSetType& other);

	DWORD GetSize(void) const;
	DWORD BitCount(void) const;
	DWORD BitCount(DWORD limit) const;
	long Min(void) const;
	long Max(void) const;

	template<class T>
	T ConvertTo() const;
	bool operator==(const CBitSetType& other) const;
	bool operator!=(const CBitSetType& other) const;

	CBitSetType& operator= (const CBitSetType& other);
	CBitSetType& operator|= (const CBitSetType& other);
	CBitSetType& operator&= (const CBitSetType& other);

protected:
	union
	{
		BITSET_ARRAY_TYPE* m_BitSetArray;
		bitptr_t m_PtrBits;
	};

	DWORD m_Size;
	void Create(DWORD size);
	void Copy(const CBitSetType& other);
	void Delete(void);

	bool StoredInPtr(void) const;
	bitptr_t GetActivePtrMask(void) const;

	BITSET_ARRAY_TYPE* GetArrayPointer(void);
	const BITSET_ARRAY_TYPE* GetArrayPointer(void) const;

	DEL_DEBUG_MUTEX(m_InstanceNotThreadSafe);
};

template<BitSetTemplateList>
CBitSetType::CBitSet(void) : CObject<CAllocatorType>() {
	m_BitSetArray = NULL;
	m_Size = 0;
	INIT_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<BitSetTemplateList>
CBitSetType::CBitSet(DWORD size) : CObject<CAllocatorType>() {
	m_BitSetArray = NULL;
	m_Size = 0;
	INIT_DEBUG_MUTEX(m_InstanceNotThreadSafe);
	Create(size);
}

template<BitSetTemplateList>
CBitSetType::CBitSet(const CBitSetType& other) : CObject<AllocatorType>() {
	m_BitSetArray = NULL;
	m_Size = 0;
	INIT_DEBUG_MUTEX(m_InstanceNotThreadSafe);
	Copy(other);
}

template<BitSetTemplateList>
CBitSetType::~CBitSet(void) {
	Delete();
	DELETE_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<BitSetTemplateList>
void CBitSetType::Resize(DWORD size) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	Create(size);

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
void CBitSetType::Clear(void) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	if (StoreInPtr())
	{
		m_PtrBits = static_cast<bitptr_t>(0);
	}
	else
	{
		const DWORD cArraySizeInBytes = (m_Size + BITS_PER_BYTE - 1)/BITS_PER_BYTE;
		SafeMemSet(m_BitSetArray,0,cArraySizeInBytes);
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
void CBitSetType::SetAll(void) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	if (StoredInPtr())
	{
		m_PtrBits = GetActivePtrMask();
	}
	else
	{
		ASSERT(m_BitSetArray);
		const DWORD cArraySize = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE cExtraBits = m_Size % cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE cExtraBitMask = cExtraBits ? ((1 << cExtraBits) - 1) : (~0);

		DWORD index;
		for (index = 0; index < cArraySize-1; index++)
		{
			m_BitSetArray[index] = ~((BITSET_ARRAY_TYPE)0);
		}
		if (index < cArraySize)
		{
			m_BitSetArray[index] = ~((BITSET_ARRAY_TYPE)0) & cExtraBitMask;
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
void CBitSetType::Invert(void) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	if (StoredInPtr())
	{
		m_PtrBits = (~m_PtrBits) & GetActivePtrMask();
	}
	else
	{

		ASSERT(m_BitSetArray);
		const DWORD cArraySize = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE cExtraBits = mSize % cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE cExtraBitMask = cExtraBits ? ((1 << cExtraBits) - 1) : (~0);

		DWORD index;
		for (index = 0; index < cArraySize - 1; index++)
		{
			m_BitSetArray[index] = ~m_BitSetArray[index];
		}
		if (index < cArraySize)
		{
			m_BitSetArray[index] = ~m_BitSetArray[index] & cExtracBitMask;
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
bool CBitSetType::IsEmpty(void ) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool isEmpty = true;
	if (StoreInPtr())
	{
		isEmpty = (m_PtrBits == static_cast<bitptr_t>(0));
	}
	else
	{
		DWORD index = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		while (isEmpty && index--)
		{
			isEmpty = (m_BitSetArray[index] == 0);
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return isEmpty;
}

template<BitSetTemplateList>
bool CBitSetType::IsEmpty(DWORD start, DWORD length) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool isEmpty = true;
	ASSERT(start < m_Size);
	ASSERT(length != 0);
	ASSERT(start + length <= m_Size);

	if (StoreInPtr())
	{
		const DWORD end = start + length;
		//Create a bit mask for this range:

		bitptr_t mask = ~(CPtrSize::Bit(start)) - 1;
		if (end < cBitsInPtr)
		{
			mask &= CPtrSize::Bit(end)-1;
		}
		isEmpty = ((mask & m_PtrBits) == static_cast<bitptr_t>(0));
	}
	else
	{
		const DWORD end = start + length;
		const DWORD startArrayIndex = start / cBitsPerArrayElement;
		const DWORD endArrayIndex = (end - 1)/cBitsPerArrayElement;
		const DWORD startBit = start % cBitsPerArrayElement;
		const DWORD endBit = end % cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE startMask = -((1 << startBit) - 1);
		const BITSET_ARRAY_TYPE endMask = endBit ? ((1 << endBit) - 1) : ~0;

		DWORD arrayIndex = startArrayIndex;
		BITSET_ARRAY_TYPE data = m_BitSetArray[arrayIndex];
		data &= startMask;
		if (startArrayIndex == endArrayIndex)
		{
			data &= endMask;
			isEmpty = (data == 0);
		}
		else
		{
			isEmpty = (data == 0);
			if (isEmpty)
			{
				for (arrayIndex = arrayIndex + 1; arrayIndex < endArrayIndex && isEmpty; arrayIndex++)
				{
					data = m_BitSetArray[arrayIndex];
					isEmpty = (data == 0);
				}
			}

			if (isEmpty)
			{
				data = m_BitSetArray[endArrayIndex];
				data &= endMask;
				isEmpty = (data == 0);
			}
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return isEmpty;
}

template<BitSetTemplateList>
bool CBitSetType::IsSet(DWORD index) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool isSet = false;
	if (index < m_Size)
	{
		if (StoreInPtr())
		{
			isSet = ((m_PtrBits & CPtrSize::Bit(index)) != static_cast<bitptr_t>(0));
		}
		else
		{
			DWORD arrayIndex = index / cBitsPerArrayElement;
			DWORD bitIndex = index % cBitsPerArrayElement;
			isSet = ((m_BitSetArray[arrayIndex] & BIT(bitIndex)) != 0);
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return isSet;
}

template<BitSetTemplateList>
bool CBitSetType::Intersets(const CBitSetType& other) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool intersects = false;
	if (StoreInPtr() && other.StoredInPtr())
	{
		intersects = ((m_PtrBits & other.m_PtrBits) != static_cast<bitptr_t>(0));
	}
	else
	{
		// Size of bitptr_t must be multitplicity of size of BITSET_ARRAY_TYPE
		C_ASSERT(sizeof(bitptr_t) % sizeof(BITSET_ARRAY_TYPE) == 0);
		const BITSET_ARRAY_TYPE* ptrThis = GetArrayPointer();
		const BITSET_ARRAY_TYPE* ptrOther = other.GetArrayPointer();

		DWORD minSize = (m_Size < other.m_Size) ? m_Size : other.m_Size;
		DWORD index = (minSize + cBitsPerArrayElement - 1) / cBitsPerArrayElement;

		while (!intersects && index--)
		{
			if ((ptrThis[index] & ptrOther[index]) != 0)
			{
				intersects = true;
			}
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return intersects;
}

template<BitSetTemplateList>
DWORD CBitSetType::GetNextMember(DWORD start) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	DWORD nextMember = m_Size;
	if (start < m_Size)
	{
		if (StoredInPtr())
		{
			bitptr_t ptrBits = m_PtrBits;
			ptrBits &= ~(CPtrSize::Bit(start)-1);

			if (ptrBits != static_cast<bitptr_t>(0))
			{
				nextMember = CPtrSize::Bsf(ptrBits);
			}
		}
		else
		{
			const DWORD startArrayIndex = start/cBitsPerArrayElement;
			const DWORD endArrayIndex = (m_Size - 1)/cBitsPerArrayElement;

			const DWORD startBit = start % cBitsPerArrayElement;
			const DWORD endBit = m_Size % cBitsPerArrayElement;

			const BITSET_ARRAY_SIZE startMask = ~((1 << startBit) - 1);
			const BITSET_ARRAY_SIZE endMask = endBit ? ((1 << endBit) - 1) : ~0;

			DWORD arrayIndex = startArrayIndex;
			BITSET_ARRAY_TYPE data = m_BitSetArray[arrayIndex];
			data &= startMask;

			if (arrayIndex == endArrayIndex)
			{
				data &= endMask;
			}
			else
			{
				if (data == 0)
				{
					for (arrayIndex = arrayIndex + 1; arrayIndex < endArrayIndex; arrayIndex++)
					{
						data = m_BitSetArray[arrayIndex];
						if (data != 0)
						{
							break;
						}
					}

					if (data == 0)
					{
						data = m_BitsetArray[endArrayIndex];
						data &= endMask;
					}
				}
			}

			if (data != 0)
			{
				const DWORD lsb = bsf(data);
				nextMember = (arrayIndex * cBitsPerArrayElement) + lsb;
			}
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return nextMember;
}

template<BitSetTemplateList>
void CBitSetType::Set(DWORD index){
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	if (index >= m_Size)
	{
		Create(index + 1);
	}

	ASSERT(index < m_Size);

	if (StoreInPtr())
	{
		m_PtrBits |= CPtrSize::Bit(index);
	}
	else
	{
		DWORD arrayIndex = index/cBitsPerArrayElment;
		DWORD bitIndex = index % cBitsPerArrayElment;

		m_BitSetArray[arrayIndex] |= BIT(bitIndex);
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
void CBitSetType::Set(const CBitSetType& other) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	DWORD size = other.m_Size;
	if (m_Size < other.m_Size)
	{
		Create(other.m_Size);
		size = m_Size;
	}

	ASSERT(m_Size >= other.m_Size);

	if (StoreInPtr())
	{
		ASSERT(other.StoredInPtr());
		m_PtrBits != other.m_PtrBits;
	}
	else
	{
		const BITSET_ARRAY_TYPE* pOtherArray = other.GetArrayPointer();
		DWORD index = (size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		while (index--)
		{
			m_BitSetArray[index] |= pOtherArray[index];
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
void CBitSetType::UnSet(DWORD index) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	if (index >= m_Size)
	{
		Create(index + 1);
		if (index >= m_Size)
		{
			ASSERT(index < m_Size);
			return;
		}
	}

	if (StoredInPtr())
	{
		m_PtrBits &= (~CPtrSize::Bit(index)) & GetActivePtrMask();
	}
	else
	{
		DWORD arrayIndex = index / cBitsPerArrayElement;
		DWORD bitIndex = index % cBitsPerArrayElement;

		m_BitSetArray[arrayIndex ] &= ~BIT(bitIndex);
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
void CBitSetType::UnSet(const CBitSetType& other) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	DWORD size = other.m_Size;
	if (m_Size < other.m_Size)
	{
		Create(other.m_Size);
		size = m_Size;
	}

	ASSERT(m_Size >= other.m_Size);

	if (StoreInPtr())
	{
		ASSERT(other.StoredInPtr());
		m_PtrBits &= ~other.m_PtrBits;
		m_PtrBits &= GetActivePtrMask();
	}
	else
	{
		const BITSET_ARRAY_TYPE* pOtherArray = other.GetArrayPointer();
		DWORD index = (size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		while (index--)
		{
			m_BitSetArray[index] &= ~pOtherArray[index];
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<BitSetTemplateList>
DWORD CBitSetType::GetSize(void) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	const DWORD size = m_size;

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return size;
}

template<BitSetTemplateList>
long CBitSetType::Min(void) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	long minBit = -1;
	if (StoredInPtr())
	{
		if (m_PtrBits)
		{
			minBit = CPtrSize::Bsf(m_PtrBits);
		}
	}
	else
	{
		const DWORD count = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		for (DWORD i=0; i<count; i++)
		{
			const DWORD lsb = bsf(m_BitSetArray[i]);
			minBit = (i*cBitsPerArrayElement) + lsb;
			break;
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return minBit;
}

template<BitSetTemplateList>
long CBitSetType::Max(void) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	long maxBit = -1;
	if (StoreInPtr())
	{
		if (m_PtrBits)
		{
			maxBit = CPtrSize::Bsr(m_PtrBits);
		}
	}
	else
	{
		const DWORD count = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		for(long i=count -1 ;i >= 0; --i)
		{
			if (m_BitSetArray[i] != 0)
			{
				const DWORD msb = bsr(m_BitSetArray[i]);
				maxBit = (i*cBitsPerArrayElement) + msb;
				break;
			}
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return maxBit;
}

template<BitSetTemplateList> template<class T>
T CBitSetType::ConvertTo() const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	T mask = 0;
	if (StoreInPtr())
	{
		bitptr_t bits = mPtrBits;
		mask = (T)(bits);
	}
	else
	{
		ASSERT(m_BitSetArray);
		mask = ((T*)m_BitSetArray)[0];
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return mask;
}

template<BitSetTemplateList>
bool CBitSetType::operator ==(const CBitSetType& other) const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool isEqual = false;
	if (m_Size == other.m_Size)
	{
		ASSERT(this->StoredInPtr() == other.StoredInPtr());
		if (StoreInPtr())
		{
			isEqual = (m_PtrBits == other.m_PtrBits);
		}
		else
		{
			const DWORD cArraySizeInBytes = (m_Size + BITS_PER_BYTE - 1)/BITS_PER_BYTE;
			isEqual = (0 == SafeMemCompare(m_BitSetArray,other.m_BitSetArray,cArraySizeInBytes));
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return isEqual;
}

template<BitSetTemplateList>
bool CBitSetType::operator !=(const CBitSetType& other)const {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool isNotEqual = true;
	if (m_Size == other.m_Size)
	{
		ASSERT(this->StoredInPtr() == other.StoredInPtr());
		if (StoreInPtr())
		{
			isNotEqual = (m_PtrBits != other.StoredInPtr());
		}
		else
		{
			const DWORD cArraySizeInBytes = (m_Size + BITS_PER_BYTE - 1)/BITS_PER_BYTE;
			isNotEqual = (0 != SafeMemCompare(m_BitsSetArray,other.m_BitSetArray,cArraySizeBytes));
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return isNotEqual;
}

template<BitSetTemplateList>
CBitSetType& CBitSetType::operator =(const CBitSetType &other) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	Copy(other);

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return *this;
}

template<BitSetTemplateList>
CBitSetType& CBitSetType::operator |=(const CBitSetType& other) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	DWORD size = other.m_Size;
	if (m_Size < other.m_Size)
	{
		Create(other.m_Size);
		size = m_Size;
	}

	ASSERT(m_Size >= other.m_Size);
	if (StoredInPtr())
	{
		ASSERT(other.StoredInPtr());
		m_PtrBits |= other.m_PtrBits;
	}
	else
	{
		const BITSET_ARRAY_TYPE* pOtherArray = other.GetArrayPointer();
		DWORD index = (size + cBitsPerArrayElement -1)/cBitsPerArrayElement;
		while (index--)
		{
			m_BitSetArray[index] |= pOtherArray[index];
		}
	}
	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return *this;
}

template<BitSetTemplateList>
CBitSetType& CBitSetType::operator &=(const CBitSetType &other) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	DWORD size = other.m_Size;
	if (m_Size < other.m_Size)
	{
		Create(other.m_Size);
		size = m_Size;
	}
	ASSERT(m_Size >= other.m_Size);
	if (StoreInPtr())
	{
		ASSERT(other.StoredInPtr());
		m_PtrBits &= other.m_PtrBits;
	}
	else
	{
		const BITSET_ARRAY_TYPE* pOtherArray = other.GetArrayPointer();
		DWORD index = (size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		while(index--)
		{
			m_BitSetArray[index] &= pOtherArray[index];
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return *this;
}

template<BitSetTemplateList>
void CBitSetType::Create(DWORD size) {
	if (size == m_Size)
	{

	}
	if (size <= cBitsInPtr)
	{
		bitptr_t newPtrBits = this->ConvertTo<bitptr_t>();
		Delete();
		m_Size = size;
		m_PtrBits = newPtrBits & GetActivePtrMask();
	}
	else
	{
		BITSET_ARRAY_TYPE* ptrthis = GetArrayPointer();
		const DWORD cNewArraySize = (size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		const DWORD cOldArraySize = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE cExtraBits = size % cBitsPerArrayElement;
		const BITSET_ARRAY_TYPE cExtraBitMask = cExtraBits ? ((i << cExtraBits) - 1) : (~0);

		if (cNewArraySize == cOldArraySize)
		{
			m_Size = size;
			if (cNewArraySize)
			{
				ptrThis[cNewArraySize - 1] &= cExtraBitMask;
			}
		}
		else if (cNewArraySize)
		{
			BITSET_ARRAY_TYPE* ptr = (BIT_SET_ARRAY_TYPE*)
					CAllocatorType::Allocate(sizeof(BITSET_ARRAY_TYPE) * cNewArraySize);
			if (ptr)
			{
				if (ptrThis)
				{
					if (cNewArraySize > cOldArraySize)
					{
						MemCopy(ptr,ptrthis,cOldArraySize*sizeof(ptrThis[0]));
						SafeMemSet(ptr + cOldArraySize, 0, (cNewArraySize - cOldArraySize) * sizeof(ptrThis[0]));
					}
					else
					{
						MemCopy(ptr,ptrthis,cNewArraySize*sizeof(ptrThis[0]));
						if (cNewArraySize)
						{
							ptr[cNewArraySize - 1] &= cExtraBitMask;
						}
					}
				}
				else
				{
					SafeMemSet(ptr,0,cNewArray*sizeof(ptrThis[0]));
				}
				Delete();
				m_BitSetArray = ptr;
				m_Size = size;
			}
			else
			{
				ASSERT(0);
			}
		}
		else
		{
			Delete();
		}
	}
}

template<BitSetTemplateList>
void CBitSetType::Copy(const CBitSetType &other) {
	if (this != &other)
	{
		Create(other.m_Size);
	}
	if (m_Size == other.m_Size)
	{
		ASSERT(this->StoredInPtr() == other.StoredInPtr());
		if (StoreInPtr())
		{
			m_PtrBits = other.m_PtrBits;
		}
		else
		{
			const DWORD cArraySizeBytes = (other.m_Size + BITS_PER_BYTE - 1)/BITS_PER_BYTE;
			MemCopy(m_BitSetArray,other.m_BitSetArray,cArraySizeInBytes);
		}
	}
	else
	{
		ASSERT(0);
	}
}

template<BitSetTemplateList>
void CBitSetType::Delete(void) {
	ASSERT(m_BitSetArray || StoredInPtr());
	if (!StoredInPtr() && m_BitSetArray)
	{
		CAllocator::Dealocate(m_BitSetArray);
	}
	m_BitSetArray = NULL;
	m_Size = 0;
}

template<BitSetTemplateList>
DWORD CBitSetType::BitCount(void) const {
	ACQUIRE_DEBUG_MUTEX_READ(m_InstanceNotThreadSafe);
	DWORD bitCount = 0;
	if (StoredInPtr())
	{
		bitCount = CPtrSize::Count(m_PtrBits);
	}
	else
	{
		const DWORD cBitsPerArrayElment = (sizeof(m_BitSetArray[0]) * 8);
		DWORD index = (m_Size + cBitsPerArrayElement - 1)/cBitsPerArrayElement;
		while (index--)
		{
			BITSET_ARRAY_TYPE Elment = m_BitSetArray[index];
			bitCount += iSTD::BitCount(Elment);
		}
	}

	RELEASE_DEBUG_MUTEX_READ(m_InstanceNotThreadSafe);
	return bitCount;
}

template<BitSetTemplateList>
DWORD CBitSetType::BitCount(DWORD limit) const {
	ACQUIRE_DEBUG_MUTEX_READ(m_InstanceNotThreadSafe);

	DWORD bitCount = 0;
	limit = iSTD::Min(limit,s_Size-1);
	for (DWORD i=0; i <= limit; i++)
	{
		if (IsSet(i))
		{
			bitCount++;
		}
	}
	RELEASE_DEBUG_MUTEX_READ(m_InstanceNotThreadSafe);
	return bitCount;
}

template<BitSetTemplateList>
bool CBitSetType::StoredInPtr(void) const {
	return m_Size <= cBitsInPtr;
}

template<BitSetTemplateList>
typename CBitSetType::bitptr_t CBitSetType::GetActivePtrMask(void) const {
	ASSERT(StoreInPtr());
	if (m_Size == cBitsInPtr)
	{
		return static_cast<bitptr_t>(-1);
	}
	return CPtrSize::Bit(m_Size)-1;
}

template<BitSetTemplateList>
typename CBitSetType::BITSET_ARRAY_TYPE* CBitSetType::GetArrayPointer(void) {
	if (StoredInPtr())
	{
		return (reinterpret_cast<BITSET_ARRAY_TYPE*>(&m_PtrBits));
	}
	else
	{
		return m_BitSetArray;
	}
}

template<BitSetTemplateList>
const typename CBitSetType::BITSET_ARRAY_TYPE* CBitSetType::GetArrayPointer(void) const {
	if (StoredInPtr())
	{
		return (reinterpret_cast<const BITSET_ARRAY_TYPE*>(&m_PtrBits));
	}
	else
	{
		return m_BitSetArray;
	}
}

}


#endif /* BITSET_H_ */












































