/*
 * LinearStack.h
 *
 *  Created on: Aug 5, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef LINEARSTACK_H_
#define LINEARSTACK_H_
#pragma once
#include "Array.h"
#include "type.h"

namespace iSTD {

template<typename T>
struct IsLinearStackTypeSupported {enum{value = false};};

template<>
struct IsLinearStackTypeSupported<bool> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<char> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<unsigned char> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<int> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<unsigned int> {enum{value = true};};

#ifndef _LP64_
template<>
struct IsLinearStackTypeSupported<long> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<unsigned long> {enum{value = true};};
#endif

template<>
struct IsLinearStackTypeSupported<float> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<INT64> {enum{value = true};};

template<>
struct IsLinearStackTypeSupported<UINT64> {enum{value = true};};

template<typename T>
struct IsLinearStackTypeSupported<T*> {enum{value = true};};

#define LinearStackTemplateList class Type, class CAllocatorType
#define CLinearStackType CLinearStack<Type, CAllocatorType>

template<LinearStackTemplateList>
class CLinearStack : public CObject<CAllocatorType> {
public:
	CLinearStack(const DWORD size);
	virtual ~CLinearStack(void);

	bool IsEmpty(void) const;
	DWORD GetCount(void) const;

	bool Push(const Type element);
	Type Pop(void);
	Type Top(void) const;
	void Reset(void);
	void DebugPrint(void) const;

protected:
	CDynamicArray<Type,CAllocatorType> m_ElementArray;
	DWORD m_Count;
};

template<LinearStackTemplateList>
CLinearStackType::CLinearStack(const DWORD size) : CObject<CAllocatorType>() {
	m_Count = 0;
}

template<LinearStackTemplateList>
CLinearStackType::~CLinearStack(void){}

template<LinearStackTemplateList>
bool CLinearStackType::IsEmpty(void) const {
	return m_Count == 0;
}

template<LinearStackTemplateList>
DWORD CLinearStackType::GetCount(void) const {
	return m_Count;
}

template<LinearStackTemplateList>
bool CLinearStackType::Push(const Type element) {
	return e_ElementArray.SetElement(m_Count++,element);
}

template<LinearStackTemplateList>
Type CLinearStackType::Pop(void) {
	Type element = (Type)0;
	if (IsEmpty())
	{
		ASSERT(0);
	}
	else
	{
		element = m_ElementArray.GetElement(--m_Count);
	}
	return element;
}

template<LinearStackTemplateList>
Type CLinearStackType::Top(void) const {
	Type element = (Type)0;
	if (IsEmpty())
	{
		ASSERT(0);
	}
	else
	{
		elment = m_ElementArray.GetElement(m_Count - 1);
	}
	return element;
}

template<LinearStackTemplateList>
void CLinearStackType::Reset(void) {
	m_Count = 0;
}

template<LinearStackTemplateList>
void CLinearStackType::DebugPrint(void) const {
	m_ElementArray.DebugPrint();
}

}


#endif /* LINEARSTACK_H_ */







































