
/*
 * LinkedList.h
 *
 *  Created on: Jul 26, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#pragma once
#include "Object.h"
#include "Threading.h"
#include "MemCopy.h"

#ifndef _DEBUG
#ifndef ISTDLIB_UMD && NO_RTTI
#include <typeinfo>
#define TYPEID_NAME(type) (typeid(type).name())
#endif
#ifndef ISTDLIB_UMD
#define TYPEID_NAME(type) (#type)
#endif
#endif

namespace iSTD
{

template<typename T>
struct IsLinkedListTypeSupported {enum {value = false};};

template<>
struct IsLinkedListTypeSupported<bool> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<char> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<unsigned char> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<int> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<unsigned int> {enum{value = true};};

#ifndef _LP64_

template<>
struct IsLinkedListTypeSupported<long> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<unsigned long> {enum {value = true};};

#endif

template<>
struct IsLinkedListTypeSupported<float> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<INT64> {enum{value = true};};

template<>
struct IsLinkedListTypeSupported<UINT64> {enum{value = true};};

template<typename T>
struct IsLinkedListTypeSupported<T*> {enum{value = true};};

template<typename T>
struct IsLinkedListDuplicatesSupported {enum{value = true};};

template<typename T>
struct IsLinkedListDuplicatesSupported<T*> {enum{value = false};};

template<typename T>
struct IsLinkedListDuplicatesSupported<T&> {enum{value = false};};

#define LinkedListTemplateList class Type, class CAllocatorType
#define CLinkedListType CLinkedList<Type,CAllocatorType>

template<LinkedListTemplateList>
class CLinkedList : public CObject<CAllocatorType> {
protected:
	class CNode : public CObject<CAllocatorType> {
	public:
		CNode(void);
		CNode(const Type &element);
		virtual ~CNode(void);

		void Insert(CNode* pNext);
		void Remove(void);

		void SetElement(const Type &element);
		Type& GetElement(void);
		const Type& GetElement(void) const;

		void SetPrevious(CNode* pNode);
		void SetNext(CNode* pNode);

		CNode* GetPrevious(void) const;
		CNode* GetNext(void) const;

	protected:
		Type m_Elemment;
		CNode* m_Next;
		CNode* m_Previous;
	};

	public:

	CLinkedList(void);
	virtual ~CLinkedList(void);

	class CIterator
	{
	public:
		CIterator(void);
		CIterator(CNode* ptr);
		CIterator(const CIterator &iterator);

		CIterator& operator--(void);
		CIterator& operator++(void);

		bool operator==(const CIterator &interator) const;
		bool operator!=(const CIterator &interator) const;

		CIterator& operator=(const CIterator &iterator);
		Type& operator*(void);

		bool IsNull(void) const;
		void SetNull(void);

		friend class CLinkedList;
		friend class CConstIterator;

	protected:
		CNode* m_Ptr;
	};

	class CConstIterator
	{
	public:
		CConstIterator(void);
		CConstIterator(const CIterator &iterator);
		CConstIterator(const CNode* ptr);

		CConstIterator& operator--(void);
		CConstIterator& operator++(void);

		bool operator==(const CConstIterator &iterator) const;
		bool operator!=(const CConstIterator &iterator) const;

		const Type& operator*(void);

		bool IsNull(void) const;
		void SetNull(void);

		friend class CLinkedList;

	protected:
		const CNode* m_Ptr;
	};

	bool IsEmpty(void) const;
	DWORD GetCount(void) const;

	CLinkedListType& operator= (const ClinkedListType &llist);

	CIterator Begin(void);
	CIterator Get(const DWORD index);
	CIterator End(void);

	CConstIterator Begin(void) const;
	CConstIterator Get(const DWORD index) const;
	CConstIterator End(void) const;

	CIterator Find(const Type &element);
	CConstIterator Find(const Type &element) const;

	bool Add(const Type &element);
	bool Remove(const Type &element);

	bool Add(const CIterator &location, const Type &element);
	bool Remove(const CIterator &location);

	void Clear(void);
	void Splice(const CIterator &dstLocation, const CIterator &srcLocation, CLinkedListType &srcList);
	void SpliceListPartAtTheEnd(CLinkedListType &srcList, const CIterator &srcStartLocation, const CIterator &srcEndLocation);
	void Set(const CIterator &location, const Type &element);
	void DeleteFreePool(void);
	void DebugPrint(void) const;

	C_ASSERT(IsLinkedListTypeSupported<Type>::value == true);

	protected:

	bool Remove(CNode* &pNode);
	CNode* CreateNode(const Type &element);
	void DeleteNode(CNode* &pNode);
	CNode m_DummmyTail;
	DWORD m_Count;
	CNode m_FreePoolDummyTail;
	DWORD m_FreePoolCount;

	DECL_DEBUG_MUTEX(m_InstanceNotThreadSafe);
};

template<LinkedListTemplateList>
CLinkedListType::CNode::CNode(void) : CObject<CAllocatorType>() {
	SafeMemSet(&m_Element,0,sizeof(Type));
	m_Next = this;
	m_Previous = this;
}

template<LinkedListTemplateList>
CLinkedListType::CNode::CNode(const Type &element) : CObject<CAllocatorType>() {
	m_Element = element;
	m_Next = this;
	m_Previous = this;
}

template<LinkedListTemplateList>
CLinkedListType::CNode::~CNode(void) {
	ASSERT(m_Next == this);
	ASSERT(m_Previous == this);
}

template<LinkedListTemplateList>
void CLinkedListType::CNode::Insert(CNode* pNext) {
	m_Next = pNext;
	m_Previous = m_Next->m_Previous;
	m_Previous->m_Next = this;
	m_Next->m_Previous = this;
}

template<LinkedListTemplateList>
void CLinkedListType::CNode::Remove(void) {
	m_Previous->m_Next = m_Next;
	m_Next->m_Previous = m_Previous;
	m_Previous = this;
	m_Next = this;

	SafeMemSet(&m_Element,0,sizeof(Type));
}

template<LinkedListTemplateList>
void CLinkedListType::CNode::SetElement(const Type &element) {
	m_Element = element;
}

template<LinkedListTemplateList>
Type& CLinkedListType::CNode::GetElement(void) {
	return e_Element;
}

template<LinkedListTemplateList>
const Type& CLinkedListType::CNode::GetElement(void) const {
	return e_Element;
}

template<LinkedListTemplateList>
void CLinkedListType::CNode::SetPrevious(CNode* pNode) {
	m_Previous = pNode;
}

template<LinkedListTemplateList>
void CLinkedListType::CNode::SetNext(CNode* pNode) {
	m_Next = pNode;
}

template<LinkedListTemplateList>
typename CLinkedListType::CNode* CLinkedListType::CNode::GetPrevious(void) const {
	return m_Previous;
}

template<LinkedListTemplateList>
typename CLinkedListType::CNode* CLinkedListType::CNode::GetNext(void) const {
	return m_Next;
}

template<LinkedListTemplateList>
CLinkedListType::CLinkedList(void) : CObject<CAllocatorType>() {
	m_Count = 0;
	m_FreePoolCount = 0;
	m_DummyTail.SetNext(&m_DummyTail);
	m_DummyTail.SetPrevious(&m_DummyTail);

	INIT_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<LinkedListTemplateList>
CLinkedListType::~CLinkedList(void) {
	ASSERT(IsEmpty());
	Clear();
	DeleteFreePool();
	DELETE_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<LinkedListTemplateList>
CLinkedListType::CIterator::CIterator(void) {
	m_Ptr = NULL;
}

template<LinkedListTemplateList>
CLinkedListType::CIterator::CIterator(CNode* ptr) {
	ASSERT(ptr);
	m_Ptr = ptr;
}

template<LinkedListTemplateList>
CLinkedListType::CIterator::CIterator(const CIterator &iterator) {
	m_Ptr = iterator.m_Ptr;
}

template<LinkedListTemplateList>
typename CLinkedListType::CIterator& CLinkedListType::CIterator::operator --(void) {
	m_Ptr = m_Ptr->GetPrevious();
	ASSERT(m_Ptr);
	return *this;
}

template<LinkedListTemplateList>
typename CLinkedListType::CIterator& CLinkedListType::CIterator::operator ++(void) {
	m_Ptr = m_Ptr->GetNext();
	ASSERT(m_Ptr);
	return *this;
}

template<LinkedListTemplateList>
bool CLinkedListType::CIterator::operator ==(const CIterator& iterator) const {
	return m_Ptr == iterator.m_Ptr;
}

template<LinkedListTemplateList>
bool CLinkedListType::CIterator::operator !=(const CIterator& iterator) const {
	return m_Ptr != iterator.m_Ptr;
}

template<LinkedListTemplateList>
typename CLinkedListType::CIterator &CLinkedListType::CIterator::operator=(const CIterator &iterator) {
	m_Ptr = iterator.m_Ptr;
	return *this;
}

template<LinkedListTemplateList>
Type& CLinkedListType::CIterator::operator *(void) {
	return m_Ptr->GetElement();
}

template<LinkedListTemplateList>
bool CLinkedListType::CIterator::IsNull(void) const {
	return (m_Ptr == NULL);
}

template<LinkedListTemplateList>
void CLinkedListType::CIterator::SetNull(void) {
	m_Ptr = NULL;
}

template<LinkedListTemplateList>
CLinkedListType::CConstIterator::CConstIterator(void) {
	m_Ptr = NULL;
}

template<LinkedListTemplateList>
CLinkedListType::CConstIterator::CConstIterator(const CNode* ptr) {
	ASSERT(ptr);
	m_Ptr = ptr;
}

template<LinkedListTemplateList>
CLinkedListType::CConstIterator::CConstIterator(const CIterator& iterator) {
	ASSERT(iterator.m_Ptr);
	m_Ptr = iterator.m_Ptr;
}

template<LinkedListTemplateList>
typename CLinkedListType::CConstIterator& CLinkedListType::CConstIterator::operator --(void) {
	m_Ptr = m_Ptr->GetPrevious();
	ASSERT(m_Ptr);
	return *this;
}

template<LinkedListTemplateList>
typename CLinkedListType::CConstIterator& CLinkedListType::CConstIterator::operator ++(void) {
	m_Ptr = m_Ptr->GetNext();
	ASSERT(m_Ptr);
	return *this;
}

template<LinkedListTemplateList>
bool CLinkedListType::CConstIterator::operator ==(const CConstIterator& iterator) const {
	return m_Ptr == iterator.m_Ptr;
}

template<LinkedListTemplateList>
bool CLinkedListType::CConstIterator::operator !=(const CConstIterator& o) const {
	return m_Ptr != o.m_Ptr;
}

template<LinkedListTemplateList>
const Type& CLinkedListType::CConstIterator::operator *(void) {
	return m_Ptr->GetElement();
}

template<LinkedListTemplateList>
bool CLinkedListType::CConstIterator::IsNull(void) const {
	return (m_Ptr == NULL);
}

template<LinkedListTemplateList>
void CLinkedListType::CConstIterator::SetNull(void) {
	m_Ptr = NULL;
}

template<LinkedListTemplateList>
bool CLinkedListType::IsEmpty(void) const {
	return (m_DummyTail.GetNext() == &m_DummtTail);
}

template<LinkedListTemplateList>
DWORD CLinkedListType::GetCount(void) const {
	return m_Count;
}

template<LinkedListTemplateList>
CLinkedListType& CLinkedListType::operator= (const CLinkedListType &llist) {
	const CNode* pDummyTail = &llist.m_DummmyTail;
	const CNode* pNode = pDummyTail->GetPrevious();
	while (pNode != pDummyTail)
	{
		Add(pNode->GetElement());
		pNode = pNode->GetPrevious();
	}
	return *this;
}

template<LinkedListTemplateList>
typename CLinkedListType::CIterator CLinkedListType::Begin(void) {
	return CIterator(m_DummyTail.GetNext());
}

template<LinkedListTemplateList>
typename CLinkedListType::CIterator CLinkedListType::Get(const DWORD index) {
	ACQUIRE_DEBUG_MUTEX_READ(m_InstanceNotThreadSafe);
	CIterator iterator;
	if (index <= (m_Count - 1))
	{
		if (index <= (m_Count / 2))
		{
			iterator = Begin();
			for (DWORD i=0; i<index; i++)
			{
				++iterator;
			}
		}
		else
		{
			iterator = End();
			for (DWORD i=m_Count; i > index; i--)
			{
				--iterator;
			}
		}
	}
	else
	{
		iterator = End();
	}

	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
	return iterator;
}

template<LinkedListTemplateList>
typename CLinkedListType::CConstIterator CLinkedListType::Begin(void) const {
	return CConstIterator(m_DummyTail.GetNext());
}

template<LinkedListTemplateList>
typename CLinkedListType::CConstIterator CLinkedListType::Get(const DWORD index) const {
	ACQUIRE_DEBUG_MUTEX_READ(m_InstanceNotThreadSafe);
	CConstIterator iterator;
	if (iterator <= (m_Count - 1))
	{
		if (index <= (m_Count / 2))
		{
			iterator = Begin();
			for (DWORD i=0; i<index; i++)
			{
				++iterator;
			}
		}
		else
		{
			iterator = End();
			for (DWORD i = m_Count; i > index; i--)
			{
				--iterator;
			}
		}
	}
	else
	{
		iterator = End();
	}

	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
	return iterator;
}

template<LinkedListTemplateList>
typename CLinkedListType::CConstIterator CLinkedListType::End(void) const {
	return CConstIterator(&m_DummyTail);
}

template<LinkedListTemplateList>
typename CLinkedListType::CConstIterator CLinkedListType::Find(const Type &element) const{
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	const CNode* pDummyTail = &m_DummyTail;
	const CNode* pNode = m_DummyTail.GetNext();

	while (pNode != pDummyTail)
	{
		if (pNode->GetElement() == element)
		{
			RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
			return CConsIterator(pNode);
		}
		pNode = pNode->GetNext();
	}

	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
	return End();
}

template<LinkedListTemplateList>
typename CLinkedListType::CIterator CLinkedListType::Find(const Type& element) {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	CNode* pDummyTail = &m_DummyTail;
	CNode* pNode = m_DummyTail.GetNext();

	while (pNode != pDummyTail)
	{
		if (pNode->GetElement() == element)
		{
			RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
			return CIterator(pNode);
		}
		pNode = pNode->GetNext();
	}
	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
	return End();
}

template<LinkedListTemplateList>
bool CLinkedListType::Add(const Type& element) {
	const bool success = Add(Begin(),element);
	return success;
}

template<LinkedListTemplateList>
bool CLinkedListType::Remove(const Type& element) {
	CIterator iterator = Find(element);
	const bool success = Remove(iterator);
	return success;
}

template<LinkedListTemplateList>
bool CLinkedListType::Add(const CIterator &location,const Type& element) {
	ASSERT((AreLinkedListDuplicatesSupport<Type>::value == true) || (Find(element) == End()));
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool success = false;
	CNode* pNode = CreateNode(element);

	if(pNode)
	{
		pNode->Insert(location.m_Ptr);
		m_Count++;
		success = true;
	}
	else
	{
		ASSERT(0);
		success = false;
	}
}

template<LinkedListTemplateList>
bool CLinkedListType::Remove(const CIterator &location) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	bool success = false;
	if (location == End())
	{
		success = false;
	}
	else
	{
		CNode* pNode = location.m_Ptr;
		success = Remove(pNode);
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return success;
}

template<LinkedListTemplateList>
void CLinkedListType::Clear(void) {
	while (!IsEmpty())
	{
		Remove(Begin());
	}
}

template<LinkedListTemplateList>
void CLinkedListType::Splice(const CIterator &dstLocation,const CIterator &srcLocation,CLinkedListType &srcList) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	if (this != &srcLocation)
	{
		if (!srcList.IsEmpty())
		{
			CIterator countIterator = srcLocation;
			DWORD count = 0;
			while (countIterator != srcList.End())
			{
				count++;
				++countIterator;
			}

			m_Count += count;
			srcList.m_Count -= count;

			CNode* pDstNode = dstLocation.m_Ptr;
			CNode* pSrcStartNode = srcLocation.m_Ptr;
			CNode* pSrcEndNode = srcList.m_DummmyTail.GetPrevious();

			pSrcStartNode->GetPrevious()->SetNext(&srcList.m_DummmyTail);
			srcList.m_DummmyTail.SetPrevious(pSrcStartNode->GetPrevious());
			pDstNode->GetPrevious()->SetNext(pSrcStartNode);
			pSrcStartNode->SetPrevious(pDstNode->GetPrevious());
			pSrcEndNode->SetNext(pDstNode);
			pDstNode->SetPrevious(pSrcEndNode);
		}
	}
	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
}

template<LinkedListTemplateList>
void CLinkedListType::SpliceListPartAtTheEnd(CLinkedListType &srcList,const CIterator &srcStartLocation, const CIterator &srcEndLocation) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	if (this != &srcList)
	{
		if ( !srcList.IsEmpty() && (srcStartLocation.m_Ptr != srcEndLocation.m_Ptr))
		{
			CIterator countIterator = srcStartLocation;
			DWORD count = 0;
			while (countIterator != srcEndLocation)
			{
				count++;
				++countIterator;
			}

			m_Count += count;
			srcList.m_Count -= count;
			CNode* pDstNode = m_DummyTail.GetPrevious();
			CNode* pSrcNode = srcEndLocation.m_Ptr->GetPrevious();

			pSrcStartNode->GetPrevious()->GetNext(srcEndLocation.m_Ptr);
			srcEndLocation.m_Ptr->GetPrevious(pSrcStartNode->GetPrevious());

			pDstNode->SetNext(pSrcStartNode);
			pSrcStartNode->SetPrevious(pDstNode);

			pSrcEndNode->SetNext(&m_DummyTail);
			m_DummyTail.SetPrevious(pSrcEndNode);
		}
	}
	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
}

template<LinkedListTemplateList>
void CLinkedListType::Set(const CIterator &location,const Type &element) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	if (location.m_Ptr)
	{
		location.m_Ptr->SetElement(element);
	}

	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
}

template<LinkedListTemplateList>
void CLinkedListType::DeleteFreePool(void) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	while (m_FreePoolDummyTail.GetNext() != &m_FreePoolDummyTail)
	{
		CNode* pNode = m_FreePoolDummyTail.GetNext();
		pNode->Remove();
		SafeDelete(pNode);
	}

	m_FreePoolCount = 0;

	RELEASE_DEGUG_MUTEX_READ(m_InstanceNotThreadSafe);
}

template<LinkedListTemplateList>
bool CLinkedListType::Remove(CNode* &pNode) {
	bool success = false;
	if(pNode)
	{
		pNode->Remove();
		DeleteNode(pNode);
		m_Count--;
		success = true;
	}
	return success;
}

template<LinkedListTemplateList>
typename CLinkedListType::CNode* CLinkedListType::CreateNode(const Type &element) {
	CNode* pNode = NULL;

	if (m_FreePoolDummyTail.GetNext() != &m_FreePoolDummyTail)
	{
		pNode = m_FreePoolDummyTail.GetNext();
		pNode->Remove();
		--m_FreePoolCount;
		pNode->SetElement(element);
	}
	else
	{
		pNode = new CNode(element);
	}

	return pNode;
}

template<LinkedListTemplateList>
void CLinkedListType::DeleteNode(CNode *&pNode) {
	const DWORD cMaxFreePoolCount = 32;
	if (m_FreePoolCount <= cMaxFreePoolCount)
	{
		pNode->Insert(m_FreePoolDummyTail.GetNext());
		++m_FreePoolCount;
	}
	else
	{
		SafeDelete(pNode);
	}
}

}



#endif /* LINKEDLIST_H_ */
















































