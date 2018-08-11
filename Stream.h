/*
 * Stream.h
 *
 *  Created on: Aug 2, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef STREAM_H_
#define STREAM_H_

#pragma once
#include "Array.h"
#include "Buffer.h"
#include "utility.h"
#include "MemCopy.h"

namespace iSTD {

template<class CAllocatorType>
class CStream : public CObject<CAllocatorType> {
public:
	CStream(void);
	virtual ~CStream(void);

	void* GetSpace(const DWORD dwSize);
	void* GetLinearAddress(void);
	DWORD GetUsedSpace(void) const;
	void SetUsedSpace(DWORD currentSizeUsed);
	void PutAllSpace(void);

	void* GetSegmentAddress(const DWORD offset, const DWORD size) const;

protected:
	CDynamicArray<CBuffer<CAllocatorType>*,CAllocatorType> m_StreamArray;
	CBuffer<CAllocatorType>* m_pCurrentBuffer;
	DWORD m_StreamBufferCount;
	CBuffer<CAllocatorType>* m_pStreamBuffer;
	DWORD m_dwSizeUsed;
};

template<class CAllocatorType>
inline CStream<CAllocatorType>::CStream(void) : CObject<CAllocatorType>(), m_StreamArray(0) {
	m_pCurrentBuffer = NULL;
	m_StreamBufferCount = 0;
	m_pStreamBuffer = NULL;
	m_dwSizeUsed = 0;
}

template<class CAllocatorType>
inline CStream<CAllocatorType>::~CStream(void) {
	const DWORD dwCount = m_StreamArray.GetSize();
	for (DWORD i=0; i < dwCount; i++)
	{
		CBuffer<CAllocatorType>* pBuffer = m_StreamArray.GetElement(i);
		SafeDelete(pBuffer);
	}
	SafeDelete(m_pStreamBuffer);
}

template<class CAllocatorType>
inline void* CStream<CAllocatorType>::GetSpace(const DWORD dwSize) {
	if ( (!m_pCurrentBuffer) || (dwSize > m_pCurrentBuffer->GetAvailableSpace()))
	{
		m_pCurrentBuffer = new CBuffer<CAllocatorType>();
		ASSERT(m_pCurrentBuffer);
		if (m_pCurrentBuffer && m_pCurrentBuffer->Allocate(Max((DWORD)0x1000, dwSize),sizeof(DWORD)))
		{
			if (m_StreamArray.SetElement(m_StreamBufferCount, m_pCurrentBuffer))
			{
				m_StreamBufferCount++;
			}
			else
			{
				ASSERT(0);
				SafeDelete(m_pCurrentBuffer);
				m_pCurrentBuffer = NULL;
			}
		}
		else
		{
			ASSERT(0);
			SafeDelete(m_pCurrentBuffer);
			m_pCurrentBuffer = NULL;
		}
	}

	if (m_pCurrentBuffer)
	{
		m_dwSizeUsed += dwSize;
		return m_pCurrentBuffer->GetSpace(dwSize);
	}
	else
	{
		return NULL;
	}
}

template<class CAllocatorType>
inline void* CStream<CAllocatorType>::GetLinearAddress(void) {
	if (m_StreamBufferCount)
	{
		CBuffer<CAllocatorType>* pStreamBuffer = new CBuffer<CAllocatorType>();
		ASSERT(pStreamBuffer);
		if (pStreamBuffer && pStreamBuffer->Allocate(m_dwSizeUsed,sizeof(DWORD)))
		{
			if (m_pStreamBuffer)
			{
				MemCopy(pStreamBuffer->GetSpace(m_pStreamBuffer->GetUsedSpace()),
						m_pStreamBuffer->GetLinearAddress(),
						m_pStreamBuffer->GetUsedSpace());
				SafeDelete(m_pStreamBuffer);
			}

			for (DWORD i=0; i < m_StreamBufferCount; i++)
			{
				CBuffer<CAllocatorType>* pBuffer = m_StreamArray.GetElement(i);
				if (pBuffer)
				{
					MemCopy(pStreamBuffer->GetSpace(pBuffer->GetUsedSpace()),
							pBuffer->GetLinearAddress(),
							pBuffer->GetUsedSpace());

					SafeDelete(pBuffer);
					m_StreamArray.SetElement(i,NULL);
				}
			}

			m_pCurrentBuffer = NULL;
			m_StreamBufferCount = 0;
			m_StreamBuffer = pStreamBuffer;
		}
		else
		{
			ASSERT(0);
			SafeDelete(pStreamBuffer);
			pStreamBuffer = NULL;
			return NULL;
		}
	}
	return (m_pStreamBuffer) ? m_pStreamBuffer->GetLinearAddress() : NULL;
}

template<class CAllocatorType>
inline DWORD CStream<CAllocatorType>::GetUsedSpace(void) const {
	return m_dwSizeUsed;
}

template<class CAllocatorType>
inline void CStream<CAllocatorType>::SetUsedSpace(DWORD currentSizeUsed) {
	ASSERT(currentSizeUsed == 0);
	ASSERT(currentSizeUsed < m_dwSizeUsed);
	m_dwSizeUsed = currentSizeUsed;
}

template<class CAllocatorType>
inline void CStream<CAllocatorType>::PutAllSpace(void) {
	const DWORD dwCount = mStreamArray.GetSize();
	for (DWORD i = 0; i < dwCount; i++)
	{
		CBuffer<CAllocatorType>* pBuffer = m_StreamArray.GetElement(i);
		if (pBuffer)
		{
			pBuffer->PutAllSpace();
		}
	}
	m_dwSizeUsed = 0;
	if (m_pStreamBuffer)
	{
		m_pStreamBuffer->PutAllSpace();
	}
}

template<class CAllocatorType>
inline void* CStream<CAllocatorType>::GetSegmentAddress(const DWORD offset, const DWORD size) const {
	DWORD currentOffset = offset;
	if (m_pStreamBuffer)
	{
		if (currentOffset < m_pStreamBuffer->GetUsedSpace())
		{
			if ((currentOffset + size ) <= m_pStreamBuffer->GetUsedSpace())
			{
				return (BYTE*)m_pStreamBuffer->GetLinearAddress() + offset;
			}
			else
			{
				ASSERT(0);
				return NULL;
			}
		}
		else
		{
			currentOffset -= m_pStreamBuffer->GetUsedSpace();
		}
	}

	for (DWORD i = 0; i < m_StreamBufferCount; i++)
	{
		CBuffer<CAllocatorType>* pBuffer = m_StreamArray.GetElement(i);
		if (pBuffer)
		{
			if (currentOffset < pBuffer->GetUsedSpace())
			{
				if ((currentOffset + size) <= pBuffer->GetUsedSpace())
				{
					return (BYTE*)pBuffer->GetLinearAddress() + currentOffset;
				}
				else
				{
					ASSERT(0);
					return NULL;
				}
			}
			else
			{
				currentOffset -= pBuffer->GetUsedSpace();
			}
		}
	}
	return NULL;
}

}


#endif /* STREAM_H_ */









































