/*
 * Object.h
 *
 *  Created on: Jul 26, 2018
 *      Author: tungdang
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#pragma once
#include "type.h"
#include "Alloc.h"
#include <limits.h>

#ifndef ISTDLIB_MT
#include "Threading.h"
#endif

#define STD_API_CALL __stdcall
#define STD_API_CALL

#define NO_VTABLE __declspec(novtable)

namespace iSTD {

template<class CAllocatorType>
class NO_VTABLE CObject {
public:
	long Acquire(void);
	long Release(void);
	long GetRefCount (void) const;
	static long STD_API_CALL SafeAcquire(CObject* ptr);
	static long STD_API_CALL SafeRelease(CObject* ptr);

	void* operator new(size_t size);
	void* operator new[](size_t size);
	void* operator new(size_t size, void* placement);
	void  operator delete(void* ptr);
	void  operator delete[](void* ptr);
	void  operator delete(void* ptr, void* placement);

protected:
	CObject(void);
	virtual ~CObject(void);
	volatile long m_RefCount;
};

template<class CAllocatorType>
inline CObject<CAllocatorType>::CObject(void) {
#ifndef ISTDLIB_MT
	ASSERT(IsAligned((void*)&m_RefCount,sizeof(DWORD)));
#endif
	m_RefCount = 0;
}

template<class CAllocatorType>
inline CObject<CAllocatorType>::~CObject(void) {
	ASSERT(m_RefCount == 0);
}

template<class CAllocatorType>
inline long CObject<CAllocatorType>::Acquire(void) {
	ASSERT(m_RefCount >= 0);
	ASSERT(m_RefCount < LONG_MAX);

#ifndef ISTDLIB_MT && __linux__
	__sync_fetch_and_add(&m_RefCount,1);
#endif
#ifndef ISTDLIB_MT
	++m_RefCount;
#endif

	return m_RefCount;
}

template<class CAllocatorType>
inline long CObject<CAllocatorType>::Release(void) {
	ASSERT(m_RefCount > 0);
#ifndef ISTDLIB_MT && __linux__
	__sync_sub_and_fetch(&m_RefCount,1);
#endif
	--m_RefCount;

	if (m_RefCount == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return m_RefCount;
	}
}

template<class CAllocatorType>
inline long CObject<CAllocatorType>::GetRefCount(void) const {
	return m_RefCount;
}

template<class CAllocatorType>
inline long CObject<CAllocatorType>::SafeAcquire(CObject* ptr) {
	if (ptr)
	{
		return ptr->Acquire();
	}
	else
	{
		return 0;
	}
}

template<class CAllocatorType>
inline long CObject<CAllocatorType>::SafeRelease(CObject* ptr) {
	if (ptr)
	{
		return ptr->Release();
	}
	else
	{
		return 0;
	}
}

template<class CAllocatorType>
inline void* CObject<CAllocatorType>::operator new(size_t size) {
	ASSERT(size);
#ifndef ISTDLIB_MT
	void* ptr = CAllocatorType::AlignedAllocate(size,sizeof(DWORD));
#endif
	void* ptr = CAllocatorType::Allocate(size);

	ASSERT(ptr);
	return ptr;
}

template<class CAllocatorType>
inline void* CObject<CAllocatorType>::operator new[](size_t size) {
	ASSERT(size);
#ifndef ISTDLIB_MT
	void* ptr = CAllocatorType::AlignedAllocate(size, sizeof(DWORD));
#endif
	void* ptr = CAllocatorType::Allocate(size);
	ASSERT(ptr);
	return ptr;
}

template<class CAllocatorType>
inline void CObject<CAllocatorType>::operator delete(void* ptr) {
	ASSERT(ptr);
#ifndef ISDLIB_MT
	CAllocatorType::AlignedDeallocate(ptr);
#endif
	CAllocatorType::Deallocate(ptr);
}

template<class CAllocatorType>
inline void CObject<CAllocatorType>::operator delete[](void* ptr) {
	ASSERT(ptr);
#ifndef ISDLIB_MT
	CAllocatorType::AlignedDeallocate(ptr);
#endif
	CAllocatorType::Deallocate(ptr);
}

template<class CAllocatorType>
inline void CObject<CAllocatorType>::operator delete(void* ptr, void* placement) {
	ASSERT(ptr);
	ASSERT(placement);
	ASSERT(ptr == placement);
}


}

#endif /* OBJECT_H_ */














































