/*
 * Stack.h
 *
 *  Created on: Aug 4, 2018
 *      Author: tungdang
 */

#ifndef STACK_H_
#define STACK_H_

#pragma once
#include "Macros.h"
#include "Object.h"
#include "utility.h"
#include "Threading.h"
#include "type.h"]

namespace iSTD {

#define CALCULATE_NEXT_NODE_SIZE(preSize,growthFactor) \
	((prevSize * growthFactor + 90)/100)

#define StackTemplateList class ElemType, class CAllocatorType, DWORD BaseSize, DWORD GrowthFactor
#define StackDeclTemplateList class ElemType, class CAllocatorType, DWORD BaseSize = 4, DWORD GrowthFactor = 135
#define CStackType CStack<ElemType, CAllocatorType, BaseSize, GrowthFactor>

template<StackDeclTemplateList>
class CStack : public CObject<CAllocatorType> {
	ISTD_DISALLOW_COPY_AND_ASSIGN(CStack);
	C_ASSERT(CALCULATE_NEXT_NODE_SIZE(BaseSize, GrowFactor) > 0);

public:
	CStack();
	~CStack();

	DWORD GetCount(void) const;
	bool IsEmpty(void) const;

	bool Push(ElemType element);
	ElemType Pop(void);
	ElemType Top(void) const;

	bool Contains(const ElemType& elem) const;

protected:
	class CStackNode : CObject<CAllocatorType>
	{
		ISTD_DISALLOW_COPY_AND_ASSIGN(CStackNode);
	public:
		static bool Create(DWORD size, CStackNode*& pNewNode);
		static bool Create(DWORD size, CStackNode* pParentNode, CStackNode*& pNewNode);
		static void Delete(CStackNode*& pNode);
		bool IsEmpty(void) const;
		bool IsFull(void) const;

		DWORD GetMaxSize(void) const;
		CStackNode* GetParentNode(void) const;
		void Push(ElemType element);
		ElemType Pop(void);
		ElemType Top(void) const;
		bool Contains(const ElemType& elem) const;

	private:
		CStackNode(DWORD size);
		CStackNode(DWORD size, CStackNode* pParentNode);
		~CStackNode();

		bool Initialize(void);
		const DWORD m_cMaxSize;
		DOWRD m_Count;
		ElemType* m_pElements;
		CStackNode* m_pParentNode;
	};

	static const DWORD m_cStaticSize = BaseSize;
	DWORD m_Count;
	CStackNode* m_pTopNode;
	ElemType m_StaticArray[m_cStaticSize];
	DECL_DEBUG_MUTEX(m_InstanceNotThreadSafe);
};

template<StackTemplateList>
CStackType::CStack() : m_count(0), m_pTopNode(NULL) {
	INIT_DEBUG_MUTEX(m_InstanceNotThreadSafe);
}

template<StackTemplateList>
CStackType::~CStack() {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	while (m_pTopNode)
	{
		CStackNode* pParent = m_pTopNode->GetParentNode();
		CStackNode::Delete(m_pTopnode);
		m_pTopNode = pParent;
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	DELETE_DEBUG_MUTEX( m_InstanceNotThreadSafe );
}

template<StackTemplateList>
DWORD CStackType::GetCount(void) const {
	return m_Count;
}

template<StackTemplateList>
bool CStackType::Push(ElemType element) {
	ACQUIRE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );

	bool success = true;
	if (m_Count < m_cStaticSize)
	{
		m_StaticArray[m_Count++] = element;
	}
	else
	{
		if (m_pTopNode == NULL)
		{
			const DWORD newSize = CALCULATE_NEXT_NODE_SIZE(m_cStaticSize, GrowthFactor);
			success = CStackNode::Create(newSize,m_pTopNode);
		}
		else if (m_TopNode->IsFull())
		{
			const DWORD newSize = CALCULATE_NEXT_NODE_SIZE(m_pTopNode->GetMaxSize(), GrowthFactor);
			success = CStackNode::Create(newSize,m_pTopNode,m_pTopNode);
		}
		if (success)
		{
			++m_Count;
			m_pTopNode->Push(element);
		}
	}

	RELEASE_DEBUG_MUTEX_WRITE( m_InstanceNotThreadSafe );
	return success;
}

template<StackTemplateList>
ElemType CStackType::Pop(void) {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	ASSERT(!IsEmpty());
	ElemType elem;
	if (m_Count <= m_cStaticSize)
	{
		elem = mStaticArray[--m_Count];
	}
	else
	{
		if (m_pTopNode->IsEmpty())
		{
			CStackNode* pParentNode = m_pTopNode->GetParentNode();
			CStackNode::Delete(m_pTopNode);
			m_pTopNode = pParentNode;
			ASSERT(m_pTopNode);
		}
		--m_Count;
		elem = m_pTopNode->Pop();
	}

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	return elem;
}

template<StackTemplateList>
ElemType CStackType::Top(void) const {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	ASSERT(!IsEmpty());
	ElemType elem;
	if (m_Count <= m_cStaticSize)
	{
		elem = m_StaticArray[m_Count - 1];
	}
	else
	{
		if (m_pTopNode->IsEmpty())
		{
			ASSERT(m_pTopNode->GetParentNode());
			elem = m_pTopNode->GetParentNode()->Top();
		}
		else
		{
			elem = m_pTopNode->Top();
		}
	}

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	return elem;
}

template<StackTemplateList>
bool CStackType::IsEmpty(void) const {
	return (m_Count == 0);
}

template<StackTemplateList>
bool CStackType::Contains(const ElemType& elem) const {
	ACQUIRE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );

	bool found = false;
	if (m_Count >= m_cStaticSize)
	{
		CStackNode* pNode = m_pTopNode;
		while (pNode)
		{
			if (pNode->Contains(elem))
			{
				found = true;
				break;
			}
			pNode = pNode->GetParentNode();
		}
	}

	if (!found)
	{
		DWORD elemNumber = iSTD::Min(m_cStaticSize,m_Count);
		while(elemNumber--)
		{
			if(m_StaticArray[elemNumber] == elem)
			{
				found = true;
				break;
			}
		}
	}

	RELEASE_DEBUG_MUTEX_READ( m_InstanceNotThreadSafe );
	return found;
}

template<StackTemplateList>
CStackType::CStackNode::CStackNode(DWORD size) : m_cMaxSize(size), m_Count(0), m_pElements(NULL), m_pParentNode(NULL)
{
}

template<StackTemplateList>
CStackType::CStackNode::CStackNode(DWORD size, CStackNode* pParentNode) : m_cMaxSize(size), m_Count(0), m_pElement(NULL), m_pParentNode(pParentNode)
{
}

template<StackTemplateList>
CStackType::CStackNode::~CStackNode() {
	CAllocatorType::Deallocate(m_pElements);
}

template<StackTemplateList>
bool CStackType::CStackNode::Initialize(void) {
	m_pElements = (ElemType*)CAllocatorType::Allocate(sizeof(ElemType) * m_cMaxSize);
	return (m_pElements != NULL);
}

template<StackTemplateList>
bool CStackType::CStackNode::IsEmpty(void) const {
	return (m_Count == 0);
}

template<StackTemplateList>
bool CStackType::CStackNode::IsFull(void) const {
	ASSERT(m_Count <= m_cMaxSize);
	return(m_Count == m_cMaxSize);
}

template<StackTemplateList>
DWORD CStackType::CStackNode::GetMaxSize(void) const {
	return m_cMaxSize;
}

template<StackTemplateList>
typename CStackType::CStackNode* CStackType::CStackNode::GetParentNode(void) const {
	return m_pParentType;
}

template<StackTemplateList>
void CStackType::CStackNode::Push(ElemType element) {
	ASSERT(!IsFull());
	m_pElement[m_Count++] = element;
}

template<StackTemplateList>
ElemType CStackType::CStackNode::Pop(void) {
	ASSERT(!IsEmpty());
	return m_pElements[--m_Count];
}

template<StackTemplateList>
ElemType CStackType::CStackNode::Top(void) const {
	ASSERT(!IsEmpty());
	return m_pElements[m_Count - 1];
}

template<StackTemplateList>
bool CStackType::CStackNode::Contains(const ElemType& elem) const {
	DWORD elemNumber = m_Count;
	while(elemNumber--)
	{
		if (m_pElements[elemNumber] == elem)
		{
			return true;
		}
	}
	return false;
}

template<StackTemplateList>
bool CStackType::CStackNode::Create(DWORD size, CStackNode*& pNewNode) {
	bool success = true;
	pNewNode = new CStackNode(size);
	if (pNewNode)
	{
		pNewNode->Acquire();
		success = pNewNode->Initialize();

		if (success == false)
		{
			CStackNode::Delete(pNewNode);
		}
	}
	else
	{
		ASSERT(0);
		success = false;
	}
	return success;
}

template<StackTemplateList>
bool CStackType::CStackNode::Create(DWORD size, CStackNode* pParentNode, CStackNode*& pNewNode) {
	bool success = true;
	pNewNode = new CStackNode(size, pParentNode);
	if (pNewNode)
	{
		pNewNode->Acquire();
		success = pNewNode->Initialize();
		if (success == false)
		{
			CStackNode::Delete(pNewNode);
		}
	}
	else
	{
		ASSERT(0);
		success = false;
	}
	return success;
}

}

#endif /* STACK_H_ */


















































