/*
 * LinearAllocator.h
 *
 *  Created on: Jul 30, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef LINEARALLOCATOR_H_
#define LINEARALLOCATOR_H_

#pragma once
#include "Object.h"
#include "Threading.h"

namespace iSTD
{

template<class CAllocatorType>
class CLinearAllocator : public CObject<CAllocatorType> {
public:
	CLinearAllocator(void* pBaseAddress, const DWORD size);
	virtual ~CLinearAllocator(void);

	DWORD GetAvailalbeSpace(void) const;
	DWORD GetUsedSpace(void) const;

	void* GetSpace(const DWORD size);
	void* GetSpaceAligned(const DWORD size, const DWORD alignSize);
	bool IsEmpty(void) const;
	bool IsFull(void) const;

	void Align(const DWORD alignSize);
	void PutSpace(const DWORD size);
	void PutAllSpace(void);
	void* ReverseSpace(const DWORD size);
	virtual void Resize(const DWORD size);

protected:
	void* m_pBaseAddress;
	DWORD m_Size;
	DWORD m_SizeUsed;
	DWORD m_SizeReserved;

	DECL_DEBUG_MUTEX(m_InstanceNotThreadSafe);
};

template<class CAllocatorType>
inline CLinearAllocator<CAllocatorType>::CLinearAllocator(void* pBaseAddress, const DWORD size) : CObject<CAllocatorType>() {
	m_pBaseAddress = pBaseAddress;
	m_Size = size;
	m_SizeUsed = 0;
	m_SizeReserved = 0;
	INIT_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}


template<class CAllocatorType>
inline CLinearAllocator<CAllocatorType>::~CLinearAllocator(void) {
	DELETE_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<class CAllocatorType>
inline DWORD CLinearAllocator<CAllocatorType>::GetAvailalbeSpace(void) const {
	const DWORD size = m_Size - GetUsedSpace();
	return size;
}

template<class CAllocatorType>
inline DWORD CLinearAllocator<CAllocatorType>::GetUsedSpace(void) const {
	const DWORD size = m_SizeUsed + m_SizeReserved;
	return size;
}

template<class CAllocatorType>
inline void* CLinearAllocator<CAllocatorType>::GetSpace(const DWORD size) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	void* pAddress = NULL;
	if (GetAvailableSpace() >= size)
	{
		pAddress = (BYTE*)m_pBaseAddress + m_SizeUsed;
		m_SizeUsed += size;
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return pAddress;
}

template<class CAllocatorType>
inline void* CLinearAllocator<CAllocatorType>::GetSpaceAligned(const DWORD size, const DWORD alignSize) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	void* pAddress = NULL;
	if (GetAvailableSpace() >= size)
	{
		const DWORD offset = GetAligmentOffset((BYTE*)m_pBaseAddrerss + m_SizeUsed,alignSize);
		if (offset)
		{
			if ((GetAvailableSpace() >= offset) && (GetAvailableSpace() >= offset + size))
			{
				pAddress = (BYTE*)m_pBaseAddress + m_SizeUsed + offset;
				m_SizeUsed += size + offset;
			}
		}
		else
		{
			pAddress = (BYTE*)m_pBaseAddress + m_SizeUsed;
			m_SizeUsed += size;
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return pAddress;
}

template<class CAllocatorType>
inline bool CLinearAllocator<CAllocatorType>::IsEmpty(void) const {
	const bool isEmpty = (m_SizeUsed == 0) && (m_SizeReserved == 0);
	return isEmpty;
}

template<class CAllocatorType>
inline bool CLinearAllocator<CAllocatorType>::IsFull(void) const {
	const bool isFull = (GetAvailableSpace() == 0);
	return isFull;
}

template<class CAllocatorType>
inline void CLinearAllocator<CAllocatorType>::Align(const DWORD alignSize) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	const DWORD offset = GetAligmentOffset((BYTE*)m_pBaseAddress + m_SizeUsed, alignSize);
	if (offset)
	{
		if (m_Size >= m_SizeUsed + offset)
		{
			m_SizeUsed += offset;
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<class CAllocatorType>
inline void CLinearAllocator<CAllocatorType>::PutSpace(const DWORD size) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	m_SizeUsed -= size;

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<class CAllocatorType>
inline void CLinearAllocator<CAllocatorType>::PutAllSpace(void) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	m_SizeUsed = 0;
	m_SizeReserved = 0;

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}

template<class CAllocatorType>
inline void* CLinearAllocator<CAllocatorType>::ReverseSpace(const DWORD size) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

    void* pAddress = NULL;
    if (GetAvailalbleSpace() >= size)
    {
    	pAddress = (BYTE*)m_pBaseAddress + m_Size - (m_SizeReserved + size);
    	m_SizeReserved += size;
    }

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return pAddress;
}

template<class CAllocatorType>
void CLinearAllocator<CAllocatorType>::Resize(const DWORD size) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	m_Size = size;

    RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
}


}

#endif /* LINEARALLOCATOR_H_ */




















































