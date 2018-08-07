/*
 * HashTable.h
 *
 *  Created on: Aug 7, 2018
 *      Author: tungdang
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#pragma once
#include "Object.h"
#include "Array.h"
#include "LinkedList.h"

namespace iSTD {

template<typename T>
struct IsHashTableTypeSupported {enum{value = false};};

template<>
struct IsHashTableTypeSupported<bool> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<char> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<unsigned char> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<int> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<unsigned int> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<long> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<unsigned long> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<float> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<INT64> {enum{value = true};};

template<>
struct IsHashTableTypeSupported<UINT64> {enum{value = true};};

template<typename T>
struct IsHashTableTypeSupported<T*> {enum{value = true};};

#define HashTableTemplateList class KeyType, class ElementType, class CAllocatorType
#define CHashTableType        CHashTable<KeyType, ElementType, CAllocatorType>

template<HashTableTemplateList>
class CHashTable : public CObject<CAllocatorType> {
public:
	class CHashKey : public CObject<CAllocatorType>
	{
	public:
		CHashKey(void);
		CHashKey(const KeyType &keyValue);
		CHashKey(const KeyType &keyValue, const DWORD hashNode);
		CHashKey(const CHashKey &hashKey);
		virtual ~CHashKey(void);

		DWORD GetHashCode(void) const;
		const KeyType& GetKeyValue(void) const;
		bool operator == (const CHashKey &hashKey) const;

	protected:
#ifndef _MSC_VER
#pragma warning(push)
#pragma Warning(disable:4324)
#endif
		// ALIGN(16) KeyType m_KeyValue;
		// ALIGN(16) DWORD m_HashCode;

		KeyType m_KeyValue;
		DWORD m_HashCode;
	};

	CHashTable(const DWORD size);
	virtual ~CHashTable(void);

	bool Add(const KeyType &key, const ElementType& element);
	bool Get(const KeyType &key, ElementType &element) const;
	bool Remove(const KeyType &key, ElementType &element);
	bool Remove(const ElementType &element);

	bool Add(const CHashKey &hashKey, const ElementType &element);
	bool Get(const CHashKey &hashKey, ElementType &element) const;
	bool Remove(const CHashKey &hashKey, ElementType &element);

	bool IsEmpty(void) const;
	DWORD GetCount(void) const;
	void DebugPrint(void) const;

	C_ASSERT(IsHashTableTypeSupported<ElementType>::value == true);

	protected:

	class CHashNode : public CObject<CAllocatorType> {
	public:
		CHashNode (const CHashKey &hashKey, const ElementType &element);
		virtual ~CHashNode(void);

		const CHashKey& GetHashKey(void) const;
		ElementType GetElement(void) const;

	protected:
		CHashKey m_HashKey;
		ElementType m_Element;
	};

	typedef CLinkedList<CHashNode*, CAllocatorType> CHashTableKeyList;
	typedef CArray<CHashTableKeyList*, CAllocatorType> CHashTableArray;

	CHashTableKeyList* CreateHashKeyList(void);
	void DeleteHashKeyList(CHashTableKeyList* pHashKeyList);
	CHashNode* CreateHashNode(const CHashKey &hashKey, const ElementType &element);
	void DeleteHashNode(CHashNode* pHashNode);

	DWORD GetHashSize(const DWORD size) const;
	DWORD GetHashIndex(const CHashKey &hashKey) const;

	CHashTableArray m_HashArray;
	DWORD m_Count;

#ifndef _DEBUG
	DWORD m_AddCount;
	DWORD m_RemoveCount;
	DWORD m_CollisionCount;
#endif

	public:

	class CIterator
	{
	public:
		CIterator(void);
		CIterator(const CIterator &iterator);

		CIterator& operator--(void);
		CIterator& operator++(void);

		bool operator==(const CIterator &iterator) const;
		bool operator!=(const CIterator &iterator) const;

		CIterator& operator=(const CIterator &iterator);
		ElementType operator*(void);

		const KeyType& GetKeyValue(void);
		friend class CHashTable;
	protected:
		CHashTableArray* m_Array;
		DWORD m_ArrayIndex;
		typename CHashTableKeyList::CIterator m_Iterator;
	};

	CIterator Begin(void) const;
	CIterator End(void) const;

	bool Add(const CHashKey &hashKey, const ElementType &element, CIterator &iterator);
	bool Get(const CHashKey &hashKey, CIterator &iterator) const;
	bool Remove(CIterator iterator);
};

template<HashTableTemplateList>
CHashTableType::CHashTable(const DWORD size) : CObject<CAllocatorType>(), m_HashArray(GetHashSize(size)) {
	m_Count = 0;
#ifndef _DEBUG
	m_AddCount =0 ;
	m_RemoveCount = 0;
	m_CollisionCount = 0;
#endif
}

template<HashTableTemplateList>
CHashTableType::~CHashTable(void) {
	const DWORD size = m_HashArray.GetSize();
	for (DWORD i=0; i<size; ++i)
	{
		CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(i);
		if (pHashKeyList)
		{
			DeleteHashKeyList(pHashKeyList);
		}
	}
}

template<HashTableTemplateList>
bool CHashTableType::Add(const KeyType &key, const ElementType &element) {
	CHashKey hashKey(key);
	return Add(hashKey,element);
}

template<HashTableTemplateList>
bool CHashTableType::Get(const KeyType &key, ElementType &element) const {
	CHashKey hashKey(key);
	return Get(hashKey,element);
}

template<HashTableTemplateList>
bool CHashTableType::Remove(const KeyType &key, ElementType &element) {
	CHashKey hashKey(key);
	return Remove(hashKey,element);
}

template<HashTableTemplateList>
bool CHashTableType::Remove(const ElementType &element) {
	const DWORD size = m_HashArray.GetSize();
	for (DWORD i=0; i<size; ++i)
	{
		CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(i);
		if (pHashKeyList)
		{
			typename CHashTableKeyList::CIterator iterator = pHashKeyList->Begin();
			while (iterator != pHashKeyList->End())
			{
				CHashNode* pHashNode = (*iterator);
				if (pHashNode->GetElement() == element)
				{
					if (pHashKeyList->Remove(iterator))
					{
						DeleteHashNode(pHashNode);
						if(pHashKeyList->IsEmpty())
						{
							m_HashArray.SetElement(i,NULL);
							DeleteHashKeyList(pHashKeyList);
						}
						ASSERT(m_Count > 0);
						--m_Count;
#ifndef _DEBUG
						++m_RemoveCount;
#endif
						return true;
					}
					else
					{
						ASSERT(0);
						return false;
					}
				}
				++iterator;
			}
		}
	}
	ASSERT(0);
	return false;
}

template<HashTableTemplateList>
bool CHashTableType::Add(const CHashKey &hashKey, const ElementType& element) {
	if (m_HashArray.GetSize() == 0)
	{
		return false;
	}

	const DWORD index = GetHashIndex(hashKey);
	ASSERT(index < m_HashArray.GetSize());

	CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(index);

	if(!pHashKeyList)
	{
		pHashKeyList = CreateHashKeyList();
		if (pHashKeyList)
		{
			if (!m_HashArray.SetElement(index,pHashKeyList))
			{
				ASSERT(0);
				SafeDelete(pHashKeyList);
				return false;
			}
		}
		else
		{
			ASSERT(0);
			return false;
		}
	}

	CHashNode* pHashNode = CreateHashNode(hashKey,element);
	if (pHashNode)
	{
		if (pHashKeyList->Add(pHashNode))
		{
			++m_Count;
#ifndef _DEBUG
			++m_AddCount;
			if (pHashKeyList->GetCount() > 1)
			{
				++m_CollisionCount;
			}
#endif
			return true;
		}
		else
		{
			DeleteHashNode(pHashNode);
			ASSERT(0);
			return false;
		}
	}
	else
	{
		ASSERT(0);
		return false;
	}
}

template<HashTableTemplateList>
bool CHashTableType::Remove(const CHashKey &hashKey, ElementType &element) {
	if(m_HashArray.GetSize() != 0)
	{
		const DWORD index = GetHashIndex(hashKey);
		ASSERT(index < m_HashArray.GetSize());
		CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(index);
		if (pHashKeyList)
		{
			typename CHashTableKeyList::CIterator iterator = pHashKeyList->Begin();
			while(iterator != pHashKeyList->End())
			{
				CHashNode* pHashNode = (*iterator);
				if (pHashNode->GetHashKey() == hashKey)
				{
					element = pHashNode->GetElement();
					if(pHashKeyList->Remove(iterator))
					{
						DeleteHashNode(pHashNode);
						if(pHashKeyList->IsEmpty())
						{
							m_HashArray.SetElement(index,NULL);
							DeleteHashKeyList(pHashKeyList);
						}
						ASSERT(m_Count > 0);
						--m_Count;
#ifndef _DEBUG
						++m_RemoveCount;
#endif
						return true;
					}
					else
					{
						ASSERT(0);
						return false;
					}
				}
				++iterator;
			}
		}
	}
	ASSERT(0);
	return false;
}

template<HashTableTemplateList>
bool CHashTableType::IsEmpty(void) const {
	return (m_Count == 0);
}

template<HashTableTemplateList>
DWORD CHashTableType::GetCount(void) const {
	return m_Count;
}

template<HashTableTemplateList>
typename CHashTableType::CHashTableKeyList* CHashTableType::CreateHashKeyList(void) {
	CHashTableKeyList* pHashKeyList = new CHashTableKeyList();
	ASSERT(pHashKeyList);
	return pHashKeyList;
}

template<HashTableTemplateList>
void CHashTableType::DeleteHashKeyList(CHashTableKeyList* pHashKeyList) {
	if (pHashKeyList)
	{
		while(!pHashKeyList->IsEmpty())
		{
			typename CHashTableKeyList::CIterator iterator = pHashKeyList->Begin();
			CHashNode* pHashNode = (*iterator);
			DeleteHashNode(pHashNode);
			pHashKeyList->Remove(iterator);
		}
		iSTD::SafeDelete(pHashKeyList);
	}
}

template<HashTableTemplateList>
typename CHashTableType::CHashNode* CHashTableType::CreateHashNode(const CHashKey &hashKey, const ElementType &element) {
	CHashNode* pHashNode = new CHashNode(hashKey,element);
	ASSERT(pHashNode);
	return pHashNode;
}

template<HashTableTemplateList>
void CHashTableType::DeleteHashNode(CHashNode* pHashNode) {
	if (pHashNode)
	{
		iSTD::SafeDelete(pHashNode);
	}
}

}

#endif /* HASHTABLE_H_ */


















































