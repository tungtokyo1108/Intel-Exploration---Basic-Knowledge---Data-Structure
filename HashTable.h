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

template<HashTableTemplateList>
DWORD CHashTableType::GetHashSize( const DWORD size ) const
{
    // the size of the hash table array is a large prime number near but
    // smaller than the maximum size of the cache but not near a power of two
    if( size >= 6151 )
    {
        return 6151;
    }
    else if( size >= 3079 )
    {
        return 3079;
    }
    else if( size >= 1543 )
    {
        return 1543;
    }
    else if( size >= 769 )
    {
        return 769;
    }
    else if( size >= 389 )
    {
        return 389;
    }
    else if( size >= 193 )
    {
        return 193;
    }
    else if( size >= 97 )
    {
        return 97;
    }
    else
    {
        return 53;
    }
}

template<HashTableTemplateList>
DWORD CHashTableType::GetHashIndex(const CHashKey &hasKey) const {
	return (hasKey.GetHashCode() % m_HashArray.GetSize());
}

template<HashTableTemplateList>
CHashTableType::CHashKey::CHashKey(void) : CObject<CAllocatorType>() {
	SafeMemSet(&m_KeyValue,0,sizeof(KeyType));
	m_HashCode = 0;
}

template<HashTableTemplateList>
CHashTableType::CHashKey::CHashKey(const KeyType &keyValue) : CObject<CAllocatorType>() {
#ifndef _WIN32 && _MSC_VER
	ASSERT(IsAligned(&m_KeyValue,sizeof(DQWORD)));
	ASSERT(IsAligned(&m_HashCode,sizeof(DQWORD)));

	m_KeyValue = keyValue;
	ASSERT(m_KeyValue == keyValue);

	if (sizeof(KeyValue) < sizeof(DQWORD))
	{
		const DWORD* pKeyValue = (const DWORD*)&m_KeyValue;
		m_HashCode = *pKeyValue++;

		const DWORD* count = (DWORD)(sizeof(KeyType)/sizeof(DWORD));
		for (DWORD i=1; i < count; ++i)
		{
			const DWORD data = *pKeyValue++;
			m_HashCode = (m_HashCode << 1) ^ data;
		}
	}
	else
	{
		const DWORD szAlignedKeyValue = (DWORD)((((sizeof(KeyType) - 1) / sizeof(DQWORD)) + 1) * sizeof(DQWORD));
		const DWORD szPad = szAlignedKeyValue - (DWORD)sizeof(KeyType);

		if(szPad)
		{
			SafeMemSet((BYTE*)&m_KeyValue + sizeof(KeyValue), 0, szPad);
		}

		const __m128i* pKeyValue = (const __m128i*)&m_KeyValue;
		__m128i* hash = _mm_load_si128(pKeyValue++);

		const DWORD count = szAlignedKeyValue / sizeof(DQWORD);
		for (DWORD i=1; i<count; i++)
		{
			const __m128i data = _mm_load_si128(pKeyValue++);
			hash = _mm_xor_si128(_mm_slli_si128(hash,1),data);
		}

		hash = _mm_xor_si128(_mm_unpackhi_epi32(hash,hash), _mm_unpacklo_epi32(hash,hash));
		hash = _mm_xor_si128(_mm_shuffle_epi32(hash,0x2), hash);
		m_HashCode = _mm_cvtsi128_si32(hash);
	}
#endif

	ASSERT(IsAligned(&m_KeyValue, sizeof(DWORD)));
	ASSERT(IsAligned(&m_HashCode, sizeof(DWORD)));

	m_KeyValue = keyValue;
	ASSERT(m_KeyValue == keyValue);

	const DWORD* pKeyValue = (const DWORD*)&m_KeyValue;
	m_HashCode = *pKeyValue++;

	const DWORD count = (DWORD)(sizeof(KeyValue)/sizeof(DWORD));
	for (DWORD i=1; i<count; i++)
	{
		const DWORD data = *pKeyValue++;
		m_HashCode = (m_HashCode << 1) ^ data;
	}
}

template<HashTableTemplateList>
CHashTableType::CHashKey::CHashKey(const KeyType &keyValue, const DWORD hashCode) : CObject<CAllocatorType>() {
	m_KeyValue = keyValue;
	ASSERT(m_KeyValue == keyValue);
	m_HashCode = hashCode;
}

template<HashTableTemplateList>
CHashTableType::CHashKey::CHashKey(const CHashKey &hashKey) {
	m_keyValue = hashKey.m_KeyValue;
	m_HashCode = hashKey.m_HashCode;
}

template<HashTableTemplateList>
CHashTableType::CHashKey::~CHashKey(void) {}

template<HashTableTemplateList>
DWORD CHashTableType::CHashKey::GetHashCode(void) const {
	return m_HashCode;
}

template<HashTableTemplateList>
const KeyType& CHashTableType::CHashKey::GetKeyValue(void) const {
	return m_KeyValue;
}

template<HashTableTemplateList>
inline bool CHashTableType::CHashKey::operator ==(const CHashKey &hashKey) const {
	if (m_HashCode == hashKey.m_HashCode)
	{
		return (m_KeyValue == hashKey.m_KeyValue);
	}
	else
	{
		return false;
	}
}

template<HashTableTemplateList>
CHashTableType::CHashNode::CHashNode (const CHashKey &hashKey, const ElementType &element) : CObject<CAllocatorType>(), m_HashKey(hashKey) {
	m_Element = element;
}

template<HashTableTemplateList>
CHashTableType::CHashNode::~CHashNode(void) {}

template<HashTableTemplateList>
const typename CHashTableType::CHashKey& CHashTableType::CHashNode::GetHashKey(void) const {
	return m_HashKey;
}

template<HashTableTemplateList>
ElementType CHashTableType::CHashNode::GetElement(void) const {
	return m_Element;
}

template<HashTableTemplateList>
CHashTableType::CIterator::CIterator(void) : m_Iterator() {
	m_Array = NULL;
	m_ArrayIndex = 0;
}

template<HashTableTemplateList>
CHashTableType::CIterator::CIterator(const CIterator &iterator) : m_Iterator(iterator.m_Iterator) {
	m_Array = iterator.m_Array;
	m_ArrayIndex = iterator.m_ArrayIndex;
}

template<HashTableTemplateList>
typename CHashTableType::CIterator& CHashTableType::CIterator::operator --(void) {
	--m_Iterator;
	CHashTableKeyList* pHashKeyList = m_Array->GetElement(m_ArrayIndex);
	ASSERT(pHashKeyList);

	while((m_Iterator == pHashKeyList->End()) && (m_ArrayIndex != 0))
	{
		CHashTableKeyList* pPrevHashKeyList = m_Array->GetElement(--m_ArrayIndex);
		if (pPrevHashKeyList)
		{
			pHashKeyList = pPrevHashKeyList;
			m_Iterator = pHashKeyList->End();
			--m_Iterator;
		}
	}
	return *this;
}

template<HashTableTemplateList>
typename CHashTableType::CIterator& CHashTableType::CIterator::operator ++(void) {
	++m_Iterator;
	CHashTableKeyList* pHashKeyList = m_Array->GetElement(m_ArrayIndex);
	ASSERT(pHashKeyList);

	const DWORD maxIndex = m_Array->GetSize() - 1;
	while ((m_Iterator == pHashKeyList->End()) && (m_ArrayIndex != maxIndex))
	{
		CHashTableKeyList* pNextHashKeyList = m_Array->GetElement(++m_ArrayIndex);
		if (pNextHashKeyList)
		{

		}
	}
}

template<HashTableTemplateList>
bool CHashTableType::CIterator::operator ==(const CIterator& iterator) const {
	return m_Iterator == iterator.m_Iterator;
}

template<HashTableTemplateList>
bool CHashTableType::CIterator::operator !=(const CIterator& iterator) const {
	return m_Iterator != iterator.m_Iterator;
}

template<HashTableTemplateList>
typename CHashTableType::CIterator& CHashTableType::CIterator::operator =(const CIterator &iterator) {
	m_Array = iterator.m_Array;
	m_ArrayIndex = iterator.m_ArrayIndex;
	m_Iterator = iterator.m_Iterator;
	return *this;
}

template<HashTableTemplateList>
ElementType CHashTableType::CIterator::operator *(void) {
	return (*m_Iterator)->GetElement();
}

template<HashTableTemplateList>
const KeyType& CHashTableType::CIterator::GetKeyValue(void) {
	return (*m_Iterator)->GetHasKey().GetKeyValue();
}

template<HashTableTemplateList>
typename CHashTableType::CIterator CHashTableType::Begin(void) const {
	CIterator iterator;

	const DWORD size = m_HashArray.GetSize();
	for (DWORD i=0; i<size; ++i)
	{
		CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(i);
		if (pHashKeyList)
		{
			iterator.m_Iterator = pHashKeyList->Begin();
			iterator.m_Array = const_cast<CHashTableArray*>(&m_HashArray);
			iterator.m_ArrayIndex = i;
			return iterator;
		}
	}
	return iterator;
}

template<HashTableTemplateList>
typename CHashTableType::CIterator CHashTableType::End(void) const {
	CIterator iterator;
	const DWORD size = m_HashArray.GetSize();
	for (int i=(int)size - 1; i >= 0; --i)
	{
		CHashTableKeyList* pHashKeyList = m_HashArray.GetElement((DWORD)i);
		if (pHashKeyList)
		{
			iterator.m_Iterator = pHashKeyList->End();
			iterator.m_Array = const_cast<CHashTableArray*>(&m_HashArray);
			iterator.m_ArrayIndex = (DWORD)i;
			return iterator;
		}
	}
	return iterator;
}

template<HashTableTemplateList>
bool CHashTableType::Add(const CHashKey &hashKey, const ElementType &element, CIterator &iterator) {
	if (m_HashArray.GetSize() == 0)
	{
		return false;
	}
	const DWORD index = GetHashIndex(hashKey);
	ASSERT(index < m_HashArray.GetSize());
	CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(index);
	if (!pHashKeyList)
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
			iterator.m_Array = &m_HashArray;
			iterator.m_ArrayIndex = index;
			iterator.m_Iterator = pHashKeyList->Begin();
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
bool CHashTableType::Get(const CHashKey &hashKey, CIterator &iterator) const {
	if (m_HashArray.GetSize() != 0)
	{
		const DWORD index = GetHashIndex(hashKey);
		ASSERT(index < m_HashArray.GetSize());
		CHashTableKeyList *pHashKeyList = m_HashKey.GetElement(index);
		if (pHashKeyList)
		{
			typename CHashTableKeyList::CIterator list_iterator = pHashKeyList->Begin();
			while (list_iterator != pHashKeyList->End())
			{
				CHashNode* pHashNode = (*list_iterator);
				if (pHashNode->GetHashKey() == hashKey)
				{
					iterator.m_Array = const_cast<CHashTableArray*>(&m_HashArray);
					iterator.m_ArrayIndex = index;
					iterator.m_Iterator = list_iterator;
					return true;
				}
				++list_iterator;
			}
		}
	}
	return false;
}

template<HashTableTemplateList>
bool CHashTableType::Remove(CIterator iterator) {
	CHashTableKeyList* pHashKeyList = m_HashArray.GetElement(iterator.m_ArrayIndex);
	if (pHashKeyList)
	{
		CHashNode* pHashNode = *(iterator.m_Iterator);
		pHashKeyList->Remove(iterator.m_Iterator);
		DeleteHashNode(pHashNode);
		if (pHashKeyList->IsEmpty())
		{
			m_HashArray.SetElement(iterator.m_ArrayIndex,NULL);
			DeleteHashKeyList(pHashKeyList);
		}
		ASSERT(m_Count > 0);
		--m_Count;
		return true;
	}
	return false;
}

}

#endif /* HASHTABLE_H_ */
