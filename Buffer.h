/*
 * Buffer.h
 *
 *  Created on: Jul 30, 2018
 *      Author: tungdang
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#pragma once
#include "LinearAllocator.h"
#include "MemCopy.h"

namespace iSTD
{

template<class CAllocatorType>
class CBuffer : public CLinearAllocator<CAllocatorType> {
public:
	CBuffer(void);
	virtual ~CBuffer(void);

	bool Allocate(const DWORD allocSize, const DWORD alignSize);
	void Deallocate(void);
	DWORD GetBlockSize(void) const;
	void* GetLinearAddress(void) const;

protected:
	BYTE* m_pAllocAddress;
};

template<class CAllocatorType>
inline CBuffer<CAllocatorType>::CBuffer(void) : CLinearAllocator<CAllocatorType>(NULL,0) {
	m_pAllocAddress = NULL;
}

template<class CAllocatorType>
inline CBuffer<CAllocatorType>::~CBuffer(void) {
	Deallocatoe();
}

template<class CAllocatorType>
inline bool CBuffer<CAllocatorType>::Allocate(const DWORD allocSize, const DWORD alignSize) {
	Deallocate();
	const DWORD alignedAllocSize = allocSize + alignSize;
	if (alignedAllocSize)
	{
		m_pAllocAddress = (BYTE*)CAllocatorType::Allocate(alignedAllocSize);
		if (m_pAllocAddress)
		{
			const DWORD offset = (alignSize) ? GetAlignmentOffset(m_pAllocAddress,alignSize) : 0;
			this->m_pBaseAddress = this->m_pAllocAddress + offset;
			this->m_Size = allocSize;
			SafeMemSet(this->m_pBaseAddress,0,this->m_Size);
		}
		else
		{
			ASSERT(0);
			this->m_Size = 0;
		}
	}
	else
	{
		ASSERT(0);
		this->m_Size = 0;
	}

	this->m_SizeUsed = 0;
	this->m_SizeReserved = 0;

	return (this->m_Size) ? true : false;
}

template<class CAllocatorType>
inline void CBuffer<CAllocatorType>::Deallocate(void) {
	CAllocatorType::Deallocate(m_pAllocAddress);
	m_pAllocAddress = NULL;
	this->m_pBaseAddress = NULL;
	this->m_Size = 0;
	this->m_SizeUsed = 0;
	this->m_SizeReserved = 0;
}

template<class CAllocatorType>
inline DWORD CBuffer<CAllocatorType>::GetBlockSize(void) const {
	return this->m_Size;
}

template<class CAllocatorType>
inline void* CBuffer<CAllocatorType>::GetLinearAddress(void) const {
	return (void*)(this->m_pBaseAddress);
}


}

#endif /* BUFFER_H_ */
