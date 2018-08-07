/*
 * SparseSet.h
 *
 *  Created on: Aug 3, 2018
 *      Author: tungdang
 */

#ifndef SPARSESET_H_
#define SPARSESET_H_

#pragma once
#include "Object.h"
#include "MemCopy.h"

namespace iSTD {

#define SparseSetTemplateList class CAllocatorType
#define CSparseSetType CSparseSet<CAllocatorType>

template<SparseSetTemplateList>
class CSparseSet : public CObject<CAllocatorType> {

public:
	CSparseSet(void);
	CSparseSet(DWORD size);
	CSparseSet(const CSparseSetType& other);

	virtual ~CSparseSet(void);

	void Resize(DWORD size);
	void Clear (void);
	void SetAll(void);
	void Invert(void);

	bool IsEmpty() const;
	bool IsSet(DWORD index) const;
	bool Intersects(const CSparseSetType& other) const;

	void Set(DWORD index);
	void Set(const CSparseSetType& other);
	void UnSet(DWORD index);
	void UnSet(const CSparseSetType& other);

	bool UnsafeIsSet(DWORD index) const;
	void UnsafeSet(DWORD index);

	DWORD GetSize(void) const;

	bool operator==(const CSparseSetType& other) const;
	bool operator!=(const CSparseSetType& other) const;

	CSparseSetType& operator= (const CSparseSetType& other);
	CSparseSetType& operator|= (const CSparseSetType& other);
	CSparseSetType& operator&= (const CSparseSetType& other);

	DWORD GetNumMembers(void) const;
	DWORD GetMember(DWORD memberNum) const;

protected:
	DWORD* m_Array;
	DWORD* m_Members;
	DWORD* m_Size;
	DWORD* m_NumMembers;

	void Create(DWORD size);
	void Copy(const CSparseSetType& other);
	void Delete(void);

};

template<SparseSetTemplateList>
CSparseSetType::CSparseSet(void) : CObject<CAllocatorType>() {
	m_Array = 0;
	m_Members = 0;
	m_Size = 0;
	m_NumMembers = 0;
}

template<SparseSetTemplateList>
CSparseSetType::CSparseSet(DWORD size) : CObject<CAllocatorType>() {
	m_Array = 0;
	m_Members = 0;
	m_Size = 0;
	m_NumMembers = 0;
	Create(size);
}

template<SparseSetTemplateList>
CSparseSetType::CSparseSet(const CSparseSetType& other) : CObject<CAllocatorType>() {
	m_Array = 0;
	m_Members = 0;
	m_Size = 0;
	m_NumMembers = 0;
	Copy(other);
}

template<SparseSetTemplateList>
CSparseSetType::~CSparseSet(void) {
	Delete();
}

template<SparseSetTemplateList>
void CSparseSetType::Resize(DWORD size) {
	Create(size);
}

template<SparseSetTemplateList>
void CSparseSetType::Clear(void) {
	m_NumMembers = 0;
}

template<SparseSetTemplateList>
void CSparseSetType::SetAll(void) {
	DWORD index = 0;
	for (index = 0; index < GetSize(); index++)
	{
		Set(index);
	}
}

template<SparseSetTemplateList>
void CSparseSetType::Invert(void) {
	DWORD index = 0;
	for (index = 0; index < GetSize(); index++)
	{
		if (IsSet(index))
		{
			UnSet(index);
		}
		else
		{
			Set(index);
		}
	}
}

template<SparseSetTemplateList>
bool CSparseSetType::IsEmpty(void) const {
	if (GetNumMembers() == 0)
	{
		return true;
	}
	return false;
}

template<SparseSetTemplateList>
bool CSparseSetType::IsSet(DWORD index) const {
	bool isSet = false;
	if (index < GetSize())
	{
		DWORD memberNum = m_Array[index];
		if (memberNum < GetNumMembers())
		{
			DWORD member = GetMember(memberNum);
			inSet = (member == index);
		}
	}
	return isSet;
}

template<SparseSetTemplateList>
bool CSparseSetType::UnSafeSet(DOWRD index) const {
	bool isSet = false;
	DWORD memberNum = m_Array[index];
	if (memberNum < GetNumMembers())
	{
		DWORD member = GetMember(memberNum);
		isSet = (member == index);
	}
	return isSet;
}

template<SparseSetTemplateList>
bool CSparseSetType::Intersects(const CSparseSetType& other) const {
	const CSparseSetType* smallSet = &other;
	const CSparseSetType* bigSet = this;

	if (smallSet->GetNumMembers() > bigSet->GetNumMembers())
	{
		const CSparseSetType *tmp = smallSet;
		smallSet = bigSet;
		bigSet = tmp;
	}

	DWORD memberNum = smallSet->GetNumMembers();
	DWORD index = 0;
	while (memberNum--)
	{
		index = smallSet->GetMember(memberNum);
		if (bigSet->IsSet(index))
		{
			return true;
		}
	}
	return false;
}

template<SparseSetTemplateList>
void CSparseSetType::Set(DWORD index) {
	if (index >= GetSize())
	{
		Create(index + 1);
	}
	if (index < GetSize())
	{
		if (IsSet(index) == false)
		{
			m_Array[index] = m_NumMembers;
			m_Members[m_NumMembers] = index;
			m_NumMembers++;
		}
	}
	else
	{
		ASSERT(0);
	}
	ASSERT(IsSet(index));
}

template<SparseSetTemplateList>
void CSparseSetType::UnsafeSet(DWORD index) {
	ASSERT(index < GetSize());
	if (UnSafeIsSet(index) == false)
	{
		m_Array[index] = m_NumMembers;
		m_Members[n_NumMembers] = index;
		m_NumMembers++;
	}
	ASSERT(IsSet(index));
}

template<SparseSetTemplateList>
void CSparseSetType::Set(const CSparseSetType& other) {
	DWORD memberNum = other.GetNumMembers();
	DWORD index = 0;
	while (memberNum--)
	{
		index = other.GetMember(memberNum);
		Set(index);
	}
}

template<SparseSetTemplateList>
void CSparseSetType::UnSet(DWORD index) {
	if (index >= GetSize())
	{
		Create(index + 1);
	}

	if (index < GetSize())
	{
		if (IsSet(index))
		{
			m_NumMembers--;
			DWORD movedIndex = m_Members[m_NumMembers];
			DWORD movedMember = m_Array[index];

			m_Array[movedIndex] = movedMember;
			m_Members[movedMember] = movedIndex;
		}
	}

	else
	{
		ASSERT(0);
	}
	ASSERT(IsSet(index) == false);
}

template<SparseSetTemplateList>
void CSparseSetType::UnSet(const CSparseSetType& other) {
	DWORD memberNum = other.GetNumMembers();
	DWORD index = 0;
	while (memberNum--)
	{
		index = other.GetMember(memberNum);
		UnSet(index);
	}
}

template<SparseSetTemplateList>
DWORD CSparseSetType::GetSize(void) const {
	return m_Size;
}

template<SparseSetTemplateList>
bool CSparseSetType::operator ==(const CSparseSetType& other) const {
	if ((GetSize() == other.GetSize()) && (GetNumMembers() == other.GetNumMembers()))
	{
		DWORD memberNum = other.GetNumMembers();
		DWORD index = 0;
		while (memberNum--)
		{
			index = other.GetNumMembers();
			if (IsSet(index) == false)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

template<SparseSetTemplateList>
bool CSparseSetType::operator !=(const CSparseSetType& other) const {
	if ((GetSize() == other.GetSize()) && (GetNumMembers() == other.GetNumMembers()))
	{
		DWORD memberNum = other.GetNumMembers();
		DWORD index = 0;
		while (memberNum--)
		{
			index = other.GetMember(memberNum);
			if (IsSet(index) == false)
			{
				return true;
			}
		}
		return true;
	}
	return false;
}

template<SparseSetTemplateList>
CSparseSetType& CSparseSetType::operator =(const CSparseSetType &other) {
	Copy(other);
	return *this;
}

template<SparseSetTemplateList>
CSparseSetType& CSparseSetType::operator |= (const CSparseSetType &other) {
	DWORD memberNum = other.GetNumMembers();
	DWORD index = 0;
	while (memberNum--)
	{
		index = other.GetMember(memberNum);
		Set(index);
	}
	return *this;
}

template<SparseSetTemplateList>
CSparseSetType& CSparseSetType::operator &=(const CSparseSetType& other) {
	DWORD memberNum = GetNumMembers();
	DWORD index = 0;

	while(memberNum--)
	{
		index = GetNumber(memberNum);
		if (other.IsSet(index) == false)
		{
			UnSet(index);
		}
	}
	return *this;
}

template<SparseSetTemplateList>
DWORD CSparseSetType::GetNumMembers(void) const {
	return m_NumMembers;
}

template<SparseSetTemplateList>
DWORD CSparseSetType::GetMember(DWORD memberNum) const {
	ASSERT(memberNum < GetNumMembers());
	return m_Members[memberNum];
}

template<SparseSetTemplateList>
void CSparseSetType::Create(DWORD size) {
	const DWORD newArraySize = size;
	const DWORD oldArraySize = GetSize();

	if (newArraySize == oldArraySize) {}
	else if (newArraySize)
	{
		DOWRD* newArray = (DWORD*)CAllocatorType::Allocate(sizeof(DWORD) * newArraySize);
		DWORD* newMembers = (DWORD*)CAllocatorType::Allocate(sizeof(DOWRD) * newArraySize);

		if (newArray && newMembers)
		{
			if (m_Array && (newArraySize > oldArraySize))
			{
				MemCopy(newArray,m_Array,oldArraySize * sizeof(DWORD));
				SafeMemSet(newArray + oldArraySize, 0, (newArraySize - oldArraySize) * sizeof(DWORD));
			}
			else
			{
				SafeMemSet(newArray,0,newArraySize * sizeof(DWORD));
			}

			if (m_Members && (newArraySize > oldArraySize))
			{
				MemCopy(newMembers,m_Members,oldArraySize * sizeof(DWORD));
				SafeMemSet(newMembers + oldArraySize, 0, (newArraySize - oldArraySize) * sizeof(DWORD));
			}
			else
			{
				SafeMemSet(newMembers, 0, newArraySize * sizeof(DWORD));
			}

			Delete();
			m_Array = newArray;
			m_Members = newMembers;
			m_Size = size;
		}
		else
		{
			CAllocatorType::Deallocate(newArray);
			newArray = 0;
			CAllocatorType::Deallocate(newMembers);
			newMembers = 0;
		}
	}
	else
	{
		Delete();
	}
}

template<SparseSetTemplateList>
void CSparseSetType::Copy(const CSparseSetType &other) {
	if (GetSize() == other.GetSize())
	{
		if (this != &other)
		{
			MemCopy(m_Array,other.m_Array,GetSize()*sizeof(DWORD));
			MemCopy(m_Members,other.m_Members,GetSize()*sizeof(DWORD));
			m_NumMembers = other.GetNumMembers();
		}
		else {}
	}
	else
	{
		Create(other.GetSize());
		if (GetSize() == other.GetSize())
		{
			MemCopy(m_Array,other.m_Array,GetSize()*sizeof(DWORD));
			MemCopy(m_Members,other.m_Members,GetSize()*sizeof(DWORD));
			n_NumMembers = other.GetNumMembers();
		}
		else
		{
			ASSERT(0);
		}
	}
}

template<SparseSetTemplateList>
void CSparseSetType::Delete(void) {
	CAllocatorType::Deallocate(m_Array);
	m_Array = 0;
	CAllocatorType::Deallocate(m_Members);
	m_Members = 0;
	m_Size = 0;
	m_NumMembers = 0;
}


}


#endif /* SPARSESET_H_ */






































