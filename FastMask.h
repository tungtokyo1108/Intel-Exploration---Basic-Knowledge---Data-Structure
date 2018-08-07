/*
 * FastMask.h
 *
 *  Created on: Jul 29, 2018
 *      Author: tungdang
 */

#ifndef FASTMASK_H_
#define FASTMASK_H_

#pragma once
#include "Object.h"

namespace iSTD
{

#define FastMaskTemplateList bool OrderedList
#define CFastMaskSetType CFastMask<OrderedList>

template<FastMaskTemplateList>
class CFastMask {
public:
	CFastMask(unsigned int inSize);
	virtual ~CFastMask();

	inline bool IsValid(void) const;
	inline bool IsSet(unsigned int index) const;
	inline bool IsDirty(void);
	inline const unsigned int* GetMask(unsigned int &ioSetKey);
	inline unsigned int GetSize();
	inline unsigned int GetSetListSize();
	inline void GetUnsortedSetList(unsigned int const** toList, unsigned int &ioSize);
	inline void GetSetList(unsigned int const** ioList, unsigned int &ioSize);
	inline void SetBits(unsigned int index, unsigned int count);
	inline void SetBits(unsigned int index);
	inline void ClearBits(void);
	inline void UnSetBit(unsigned int index);
	inline void Resize(unsigned int in_NewSize);

protected:
	inline void SortSetList(void);
	inline void CollapseSetList(void);
	unsigned int* m_Mask;
	unsigned int* m_SetList;
	unsigned int m_Key;
	unsigned int m_SortIdx;
	unsigned int m_CollapseIdx;
	unsigned int m_SetListSize;
	unsigned int m_Capacity;
	bool m_SortOnGet;
	bool m_CollapseUnsorted;
};

template<FastMaskTemplateList>
CFastMaskSetType::CFastMask( unsigned int inSize )
:   m_Mask(0)
,   m_SetList(0)
,   m_Key(1)
,   m_SortIdx(0)
,   m_CollapseIdx(0)
,   m_SetListSize(0)
,   m_Capacity(inSize)
,	m_SortOnGet(false)
,   m_CollapseUnsorted(false)
{
    ASSERT(inSize > 0);

    if( inSize > 0 )
    {
        m_Mask      = new unsigned int[m_Capacity];
        m_SetList   = new unsigned int[m_Capacity];

        ASSERT(0 != m_Mask);
        ASSERT(0 != m_SetList);

        if( m_Mask && m_SetList )
        {
            // quick run up to next power of 2
            while( (unsigned int)m_Key <= m_Capacity )
            {
                m_Key = m_Key << 1;
            }

            // clear mask
            for( unsigned int i = 0; i < m_Capacity; i++ )
            {
                m_Mask[i] = m_Key;
            }
        }
    }
}

template<FastMaskTemplateList>
CFastMaskSetType::~CFastMask(void){
}

template<FastMaskTemplateList>
bool CFastMaskSetType::IsValid(void) const {
	return ((m_Mask != NULL) && (m_SetList != NULL));
}

template<FastMaskTemplateList>
bool CFastMaskSetType::IsSet(unsigned int index) const {
	ASSERT(index < m_Capacity);
	return (m_Key != m_Mask[index]);
}

template<FastMaskTemplateList>
bool CFastMaskSetType::IsDirty(void) {
	if (true == m_CollapseUnsorted)
	{
		CollapseSetList();
	}
	return (m_SetListSize > 0);
}

template<FastMaskTemplateList>
const unsigned int* CFastMaskSetType::GetMask(unsigned int &ioSetKey) {
	ioSetKey = m_Key;
	return m_Mask;
}

template<FastMaskTemplateList>
unsigned int CFastMaskSetType::GetSize(void) {
	return m_Capacity;
}

template<FastMaskTemplateList>
unsigned int CFastMaskSetType::GetSetListSize(void) {
	if (true == m_CollapseUnsorted)
	{
		CollapseSetList();
	}
	return m_SetListSize;
}

template<FastMaskTemplateList>
void CFastMaskSetType::SetBits(unsigned int index, unsigned int count) {
	ASSERT((index + count) <= m_Capacity);
	for(unsigned int i = index; i < index + count; i++)
	{
		if (m_Key == m_Mask[i])
		{
			if (m_SetListSize >= m_Capacity)
			{
				CollapseSetList();
			}
			ASSERT(m_SetListSize < m_Capacity);
			m_Mask[i] = m_SetListSize;
			m_SetList[m_SetListSize++] = i;

			if(OrderedList)
			{
				m_SortOnGet = true;
			}
		}
	}
}

template<FastMaskTemplateList>
void CFastMaskSetType::SetBits(unsigned int index) {
	ASSERT(index < m_Capacity);
	if (m_Key == m_Mask[index])
	{
		if (m_SetListSize >= m_Capacity)
		{
			CollapseSetList();
		}

		ASSERT(m_SetListSize < m_Capacity);
		m_Mask[index] = m_SetListSize;
		m_SetList[m_SetListSize++] = index;
		if (OrderedList)
		{
			m_SortOnGet = true;
		}
	}
}

template<FastMaskTemplateList>
void CFastMaskSetType::GetUnsortedSetList(unsigned int const** ioList, unsigned int &ioSize) {
	if (true == m_CollapseUnsorted)
	{
		CollapseSetList();
	}
	*ioList = m_SetList;
	ioSize = m_SetListSize;
}

template<FastMaskTemplateList>
void CFastMaskSetType::GetSetList(const unsigned int** ioList, unsigned int &ioSize) {
	if (OrderedList && (true == m_SortOnGet))
	{
		SortSetList();
	}
	else if (true == m_CollapseUnsorted)
	{
		CollapseSetList();
	}
	*ioList = m_SetList;
	ioSize = m_SetListSize;
}

template<FastMaskTemplateList>
void CFastMaskSetType::ClearBits(void) {
	unsigned int index = 0;
	for (unsigned int i=0; i<m_SetListSize; i++)
	{
		index = m_SetList[i];
		if (index != m_Key)
		{
			m_Mask[index] = m_Key;
		}
	}

	m_SetListSize = 0;
	m_SortIdx = 0;
	m_CollapseIdx = 0;
	m_SortOnGet = false;
	m_CollapseUnsorted = false;
}

template<FastMaskTemplateList>
void CFastMaskSetType::UnSetBit(unsigned int index) {
	ASSERT(index < m_Capacity);
	if (m_Key != m_Mask[index])
	{
		m_CollapseUnsorted = true;
		unsigned int setListPos = m_Mask[index];
		if (setListPos < m_CollapseIdx)
		{
			m_CollapseIdx = setListPos;
		}

		if (OrderedList)
		{
			if (setListPos < m_SortIdx)
			{
				m_SortIdx = setListPos;
			}
			m_SortOnGet = true;
		}
		m_SetList[m_Mask[index]] = m_Key;
		m_Mask[index] = m_Key;
	}
}

template<FastMaskTemplateList>
void CFastMaskSetType::Resize(unsigned int in_NewSize) {
	ASSERT(in_NewSize != 0);
	if (in_NewSize == m_Capacity)
	{
		return;
	}
	unsigned int* old_m_SetList = m_SetList;
	unsigned int  old_m_SetListSize = m_SetListSize;

	m_SetListSize = 0;
	m_CollapseIdx = 0;
	m_SortIdx = 0;
	m_Capacity = in_NewSize;
	m_CollapseUnsorted = false;
	m_SortOnGet = false;

	m_Mask = new unsigned int[m_Capacity];
	m_SetList = new unsigned int[m_Capacity];

	ASSERT(0 != m_Mask);
	ASSERT(0 != m_SetList);

	unsigned int old_m_Key = m_Key;
	m_Key = 1;

	while ((unsigned int)m_Key <= m_Capacity)
	{
		m_Key = m_Key << 1;
	}
	for (unsigned int i=0; i < m_Capacity; i++)
	{
		m_Mask[i] = m_Key;
	}
	for (unsigned int i=0; i<old_m_SetListSize; i++)
	{
		if (old_m_SetList[i] != old_m_Key)
		{
			SetBits(old_m_SetList[i]);
		}
	}
}

template<FastMaskTemplateList>
void CFastMaskSetType::SortSetList() {
	unsigned int count = 0;
	unsigned int keyVal = 0;
	int idx;

	for (unsigned int i = m_SortIdx; i < m_SetListSize; i++)
	{
		keyVal = m_SetList[i];
		if (keyVal == m_Key)
		{
			count++;
		}
		idx = i-1;
		while (idx >= 0 && m_SetList[idx] > keyVal)
		{
			m_SetList[idx+1] = m_SetList[idx];
			idx--;
		}
		m_SetList[idx+1] = keyVal;
	}

	m_SetListSize -= count;
	m_SortOnGet = false;
	m_CollapseUnsorted = false;
	m_SortIdx = (m_SetListSize > 0) ? m_SetListSize - 1 : 0;
	m_CollapseIdx = m_SortIdx;

	for (unsigned int i=0; i<m_SetListSize; i++)
	{
		m_Mask[m_SetList[i]] = i;
	}
}

template<FastMaskTemplateList>
void CFastMaskSetType::CollapseSetList() {
	unsigned int count = m_CollapseIdx;
	for (unsigned int i=m_CollapseIdx; i < m_SetListSize; i++)
	{
		if (m_Key != m_SetList[i])
		{
			m_Mask[m_SetList[i]] = count;
			m_SetList[count++] = m_SetList[i];
		}
	}

	m_CollapseUnsorted = false;
	m_SetListSize = false;

	if (0 == m_SetListSize)
	{
		m_SortIdx = 0;
		m_CollapseIdx = 0;
		if (OrderedList)
		{
			m_SortOnGet = false;
		}
	}
	else
	{
		if (m_CollapseIdx < m_SortIdx)
		{
			m_SortIdx = m_CollapseIdx;
			if (OrderedList)
			{
				m_SortOnGet = true;
			}
		}

		m_CollapseIdx = m_SetListSize - 1;
	}
}


}


#endif /* FASTMASK_H_ */























































