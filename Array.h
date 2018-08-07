/*
 * Array.h
 *
 *  Created on: Jul 31, 2018
 *      Author: tungdang
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#pragma once
#include "Object.h"
#include "Queue.h"
#include "MemCopy.h"

namespace iSTD {

template<typename T>
struct IsArrayTypeSupported {enum {value = false};};

template<typename T>
struct IsArrayTypeSupported<bool> {enum {value = true};};

template<typename T>
struct IsArrayTypeSupported<char> {enum {value = true};};

template<typename T>
struct IsArrayTypeSupported<unsigned char> {enum {value = true};};

template<typename T>
struct IsArrayTypeSupported<int> {enum {value = true};};

template<typename T>
struct IsArrayTypeSupported<unsigned int> {enum {value = true};};

template<>
struct IsArrayTypeSupported<long> {enum {value = true};};

template<>
struct IsArrayTypeSupported<unsigned long> {enum {value = true};};

template<>
struct IsArrayTypeSupported<INT64> {enum {value = true};};

template<>
struct IsArrayTypeSupported<float> {enum {value = true};};

template<>
struct IsArrayTypeSupported<double> {enum {value = true};};

template<typename T>
struct IsArrayTypeSupported<T*> {enum {value = true};};

#define ArrayTemplateList class Type, class CAllocatorType
#define CStaticArrayType CStaticArray<Type,CAllocatorType>
#define CDynamicArrayType CDynamicArray<Type,CAllocatorType>
#define CArrayType CArray<Type,CAllocatorType>

template<ArrayTemplateList>
class CStaticArray : public CObject<CAllocatorType> {
public:

	CStaticArray(const DWORD maxSize);
	virtual ~CStaticArray(void);

	Type GetElement(const DWORD index) const;
	Type GetElementUnsafe(const DWORD index) const;
	bool SetElement(const DWORD index, const Type& element);
	void SetElementUnsafe(const DWORD index, const Type& element);

	DWORD GetSize(void) const;
	void Clear(void);
	void DebugPrint(void) const;

	CStaticArrayType& operator= (const CStaticArrayType &array);
	C_ASSERT(IsArrayTypeSupported<Type>::value == true);

protected:
	Type* m_pArrayBuffer;
	DWORD m_ActualSize;
};

template<ArrayTemplateList>
CStaticArrayType::CStaticArray(const DWORD maxSize) : CObject<CAllocatorType>() {
	m_pArrayBuffer = (Type*)CAllocatorType::AlignedAllocate(maxSize* sizeof(Type), sizeof(Type));
	if (m_pArrayBuffer)
	{
		SafeMemSet(m_pArrayBuffer,0,(maxSize * sizeof(Type)));
		m_ActualSize = maxSize;
	}
	else {
		m_ActualSize = 0;
	}
}

template<ArrayTemplateList>
CStaticArrayType::~CStaticArray(void) {
	if (m_pArrayBuffer)
	{
		CAllocatorType::AlignedDeallocate(m_pArrayBuffer);
		m_pArrayBuffer = NULL;
	}
	m_ActualSize = 0;
}

template<ArrayTemplateList>
Type CStaticArrayType::GetElement(const DWORD index) const {
	Type element;
	if (m_pArrayBuffer && (index < m_ActualSize))
	{
		element = m_pArrayBuffer[index];
	}
	else
	{
		ASSERT(0);
		SafeMemSet(&element,0,sizeof(Type));
	}
	return element;
}

template<ArrayTemplateList>
Type CStaticArrayType::GetElementUnsafe(const DWORD index) const {
	ASSERT(m_pArrayBuffer && (index < m_ActualSize));
	return m_pArrayBuffer[index];
}

template<ArrayTemplateList>
bool CStaticArrayType::SetElement(const DWORD index, const Type& element) {
	bool success = false;
	if (m_pArrayBuffer && (index < m_ActualSize))
	{
		m_pArrayBuffer[index] = element;
		success = true;
	}
	ASSERT(success);
	return success;
}

template<ArrayTemplateList>
void CStaticArrayType::SetElementUnsafe(const DWORD index, const Type& element) {
	ASSERT(m_pArrayBuffer && (index < m_ActualSize));
	m_pArrayBuffer[index] = element;
}

template<ArrayTemplateList>
DWORD CStaticArrayType::GetSize(void) const {
	return m_ActualSize;
}

template<ArrayTemplateList>
void CStaticArrayType::Clear(void) {
	SafeMemSet(m_pArrayBuffer,0,m_ActualSize * sizeof(Type));
}

template<ArrayTemplateList>
void CStaticArrayType::DebugPrint(void) const {
#ifndef _DEBUG
	DPF(GFXDBG_STDLIB, "%s\n", _FUNCTION_);
	DPF(GFXDBG_STDLIB, "\tAddress = %p\n", this);

	if (m_pArrayBuffer)
	{
		for(DWORD i=0; i < GetSize(); i++)
		{
			Type element = GetElement(i);
			DPF(GFXDBG_STDLIB, "\t\tElement[%u] = 0x%08x\n",i,*(DWORD*)&element);
		}
	}
#endif
}

template<ArrayTemplateList>
CStaticArrayType& CStaticArrayType::operator= (const CStaticArrayType &array) {
	if (m_pArrayBuffer && array.m_pArrayBuffer)
	{
		const DWORD copySize = Min(m_ActualSize, array.m_ActualSize);
		MemCopy(m_pArrayBuffer,array.m_pArrayBuffer,copySize * sizeof(Type));
	}
	return *this;
}

template<ArrayTemplateList>
class CDynamicArray : public CObject<CAllocatorType> {
public:
	CDynamicArray(const DWORD initSize);
	CDynamicArray(const DWORD initSize, const DWORD actualSize);
	virtual ~CDynamicArray(void);

	Type GetElement(const DWORD index) const;
	bool SetElement(const DWORD index, const Type& element);
	bool Insert(const DWORD index, const Type& element);

	Type GetElementUnsafe(const DWORD index) const;
	const Type& GetElementReference(const DWORD index) const;

	Type m_elementNotExist;

	DWORD GetSize(void) const;
	bool Resize(const DWORD size);
	bool Shrink(const DWORD size);

	void PreAllocate(const DWORD size);
	void Delete(void);
	void Clear(void);

	void DebugPrint(void) const;
	CDynamicArrayType& operator= (const CDynamicArrayType &array);
	C_ASSERT(IsArrayTypeSupported<Type>::value == true);

protected:
	virtual void CreateArray(const DWORD size);
	void DeleteArray(void);
	DWORD GetMaxSize(void) const;
	bool IsValidIndex(const DWORD index) const;

	Type* m_pArrayBuffer;
	DWORD m_UsedSize;
	DWORD m_ActualSize;
	DECL_DEBUG_MUTEX(m_InstanceNotThreadSafe);
};

template<ArrayTemplateList>
CDynamicArrayType::CDynamicArray(const DWORD initSize) : CObject<CAllocatorType>() {
	m_pArrayBuffer = NULL;
	m_UsedSize = 0;
	m_ActualSize = 0;
	memset((void*)&m_elementNoExist,0,sizeof(Type));
	CreateArray(initSize);
	INIT_DEBUG_MUTEX();
}

template<ArrayTemplateList>
CDynamicArrayType::~CDynamicArray(void) {
	Delete();
	DELETE_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<ArrayTemplateList>
Type CDynamicArrayType::GetElement(const DWORD index) const {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	Type element;
	if (m_pArrayBuffer && IsValidIndex(index))
	{
		element = m_pArrayBuffer[index];
	}
	else
	{
		SafeMemSet(&element,0,sizeof(Type));
	}

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	return element;
}

template<ArrayTemplateList>
Type CDynamicArrayType::GetElementUnsafe(const DWORD index) const {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	ASSERT(m_pArrayBuffer && IsValidIndex(index));
	Type element = m_pArrayBuffer[index];

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	return element;
}

template<ArrayTemplateList>
const Type& CDynamicArrayType::GetElementReference(const DWORD index) const {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	if (m_pArrayBuffer && IsValidIndex(index))
	{
		return m_pArrayBuffer[index];
	}
	else
	{
		ASSERT(m_pArrayBuffer && IsValidIndex(index));
		return m_elementNotExsit;
	}

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
}

template<ArrayTemplateList>
bool CDynamicArrayType::SetElement(const DWORD index, const Type& element) {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	bool success = false;
	if (!IsValidIndex(index))
	{
		CreateArray(index + 1);
	}

	if (m_pArrayBuffer && IsValidIndex(index))
	{
		m_pArrayBuffer[index] = element;
		success = true;
	}

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	ASSERT(success);
	return success;
}

template<ArrayTemplateList>
bool CDynamicArrayType::Insert(const DWORD index, const Type& element) {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	const DWORD oldSize = GetSize();
	if (index < oldSize)
	{
		CreateArray(oldSize + 1);
		SafeMemMove(m_pArrayBuffer + index + 1, m_pArrayBuffer + index, (oldSize - index)*sizeof(Type));
	}
	else
	{
		CreateArray(index + 1);
	}

	bool success = SetElement(index, element);

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	ASSERT(success);
	return success;
}

template<ArrayTemplateList>
DWORD CDynamicArrayType::GetSize(void) const {
	const DWORD size = m_UsedSize;
	return size;
}

template<ArrayTemplateList>
bool CDynamicArrayType::Resize(const DWORD size) {
	bool success = true;
	const DWORD allocSize = size * sizeof(Type);
	Type* pArrayBuffer = (Type*)CAllocatorType::Allocate(allocate);
	if (pArrayBuffer)
	{
		SafeMemSet(pArrayBuffer,0,allocSize);
		if (m_pArrayBuffer)
		{
			const DWORD copySize = Min(size,m_ActualSize);
			MemCopy(pArrayBuffer,m_pArrayBuffer,copySize*sizeof(Type));
			DeleteArray();
		}

		m_pArrayBuffer = pArrayBuffer;
		m_ActualSize = size;
		m_UsedSize = size;
	}
	else
	{
		success = false;
	}
	return success;
}

template<ArrayTemplateList>
bool CDynamicArrayType::Shrink(const DWORD size) {
	bool success = false;
	if (size < m_UsedSize)
	{
		success = true;
		m_UsedSize = size;
	}
	return success;
}

template<ArrayTemplateList>
void CDynamicArrayType::Delete(void) {
	DeleteArray();
	m_UsedSize = 0;
}

template<ArrayTemplateList>
void CDynamicArrayType::Clear(void) {
	SafeMemSet(m_pArrayBuffer,0,m_ActualSize * sizeof(Type));
}

template<ArrayTemplateList>
void CDynamicArrayType::DebugPrint(void) const {
	DPF(GFX_STDLIB, "%s\n", _FUNCTION_);
	DPF(GFX_STDLIB, "\tAddress = %p\n", this);

	if (m_pArrayBuffer)
	{
		for (DWORD i=0; i<GetSize(); i++)
		{
			Type element = GetElement(i);
			DPF(GFX_STDLIB, "\t\tElement[%u] = 0x%08x\n",i,*(DWORD*)&element);
		}
	}
}

template<ArrayTemplateList>
CDynamicArrayType& CDynamicArrayType::operator =(const CDynamicArrayType &array) {
	if (array.m_pArrayBuffer)
	{
		if (m_UsedSize < array.m_UsedSize)
		{
			CreateArray(array.m_UsedSize);
		}
		if (m_pArrayBuffer && (m_UsedSize >= array.m_UsedSize))
		{
			MemCopy(m_pArrayBuffer,array.m_pArrayBuffer,array.m_UsedSize * sizeof(Type));
		}
	}
	return *this;
}

template<ArrayTemplateList>
void CDynamicArrayType::CreateArray(const DWORD size) {
	if (size)
	{
		if (size > GetMaxSize())
		{
			DWORD actualSize = GetMaxSize() * 2;
			if (size > actualSize)
			{
				actualSize = Round(Max<DWORD>(size,32),32);
			}

			ASSERT(actualSize >= size);
			ASSERT(actualSize > m_ActualSize);

			const DWORD allocSize = actualSize * sizeof(Type);
			Type* pArrayBuffer = (Type*)CAllocatorType::Allocate(allocSize);
			if (pArrayBuffer)
			{
				SafeMemSet(pArrayBuffer,0,allocSize);
				if(m_pArrayBuffer)
				{
					MemCopy(pArrayBuffer,m_pArrayBuffer,m_UsedSize*sizeof(Type));
					DeleteArray();
				}

				m_pArrayBuffer = pArrayBuffer;
				m_ActualSize = actualSize;
				m_UsedSize = size;
			}
		}
		else
		{
			m_UsedSize = size;
		}
	}
}

template<ArrayTemplateList>
void CDynamicArrayType::PreAllocate(const DWORD size) {
	if (size && (size > GetMaxSize()))
	{
		DWORD actualSize = GetMaxSize() * 2;
		if (size > actualSize)
		{
			actualSize = Round(Max<DWORD>(size,32),32);
		}
		ASSERT(actualSize >= size);
		ASSERT(actualSize > m_ActualSize);

		const DWORD allocSize = actualSize * sizeof(Type);
		Type* pArrayBuffer = (Type*)CAllocatorType::Allocate(allocSize);
		if (pArrayBuffer)
		{
			SafeMemSet(pArrayBuffer,0,allocSize);
			if(m_pArrayBuffer)
			{
				MemCopy(pArrayBuffer,m_ArrayBuffer,m_UsedSize * sizeof(Type));
				DeleteArray();
			}

			m_pArrayBuffer = pArrayBuffer;
			m_ActualSize = actualSize;
		}
	}
}

template<ArrayTemplateList>
void CDynamicArrayType::DeleteArray(void) {
	if (m_pArrayBuffer)
	{
		CAllocator::Deallocate(m_pArrayBuffer);
		m_pArrayBuffer = NULL;
	}

	m_ActualSize = 0;
}

template<ArrayTemplateList>
DWORD CDynamicArrayType::GetMaxSize(void) const {
	return m_ActualSize;
}

template<ArrayTemplateList>
bool CDynamicArrayType::IsValidIndex(const DWORD index) const {
	return (index < GetSize());
}

template<ArrayTemplateList>
class CArray : public CDynamicArray<Type,CAllocatorType> {
public:
	CArray(const DWORD initSize);
	virtual ~CArray(void);

	bool SetElement(const DWORD index, const Type& element);
	bool Resize(const DWORD size);

	void SetFreeIndex(const DWORD index);
	DWORD GetFreeIndex(void);

	void Delete(void);

protected:

	virtual void CreateArray(const DWORD size);
	typedef CQueue<DWORD,CAllocatorType> CFreeIndexQueue;
	void DeleteFreeIndexQueue(void);

	CFreeIndexQueue* m_pFreeIndexQueue;
	DWORD m_FreeIndex;
};

template<ArrayTemplateList>
CArrayType::CArray(const DWORD initSize) : CDynamicArrayType(initSize) {
	m_FreeIndexQueue = NULL;
	m_FreeIndex = 0;
}

template<ArrayTemplateList>
CArrayType::~CArray(void) {
	DeleteFreeIndexQueue();
}

template<ArrayTemplateList>
bool CArrayType::SetElement(const DWORD index, const Type& element) {
	bool success = CDynamicArrayType::SetElement(index,element);
	if (success)
	{
		m_FreeIndex = Max(index + 1, m_FreeIndex);
#ifndef _DEBUG
		if (m_pFreeIndexQueue != NULl)
		{
			ASSERT(m_pFreeIndexQueue);
		}
#endif
	}
	return success;
}

template<ArrayTemplateList>
bool CArrayType::Resize(const DWORD size) {
	bool success = CDynamicArrayType::Resize(size);
	if (success)
	{
		DeleteFreeIndexQueue();
	}
	return success;
}

template<ArrayTemplateList>
void CArrayType::SetFreeIndex(const DWORD index) {
	if (!m_pFreeIndexQueue)
	{
		m_pFreeIndexQueue = new CFreeIndexQueue();
		ASSERT();
	}

	if (m_pFreeIndexQueue)
	{
		m_pFreeIndexQueue->Push(index);
	}
}

template<ArrayTemplateList>
DWORD CArrayType::GetFreeIndex(void) {
	DWORD index = 0;
	if (m_pFreeIndexQueue && !m_pFreeIndexQueue->IsEmpty())
	{
		index = m_pFreeIndexQueue->Pop();
		ASSERT(index < m_FreeIndex);
	}
	else
	{
		index = mFreeIndex++;
		if (m_pFreeIndexQueue)
		{
			ASSERT((m_pFreeIndexQueue->Find(index) == m_pFreeIndexQueue->End()));
		}
	}

	ASSERT(index <= this->GetSize());
	return index;
}

template<ArrayTemplateList>
void CArrayType::Delete(void) {
	CDynamicArrayType::Delete();
	DeleteFreeIndexQueue();
	m_FreeIndex = 0;
}

template<ArrayTemplateList>
void CArrayType::CreateArray(const DWORD size) {
	CDynamicArrayType::CreateArray(size);
}

template<ArrayTemplateList>
void CArrayType::DeleteFreeIndexQueue(void) {
	if (m_pFreeIndexQueue)
	{
		while(!m_pFreeIndexQueue->IsEmpty())
		{
			m_pFreeIndexQueue->Pop();
		}

		SafeDelete(m_pFreeIndexQueue);
	}
}

}

#endif /* ARRAY_H_ */

















































