/*
 * LRUSet.h
 *
 *  Created on: Aug 3, 2018
 *      Author: tungdang
 */

#ifndef LRUSET_H_
#define LRUSET_H_

#pragma once
#include <assert.h>
#include "Object.h"

namespace iSTD {

#define LRUTemplateList class KeyType, class ValueType, class CAllocatorType
#define LRUClassType CLRUSet<KeyType, ValueType, CAllocatorType>

template<LRUTemplateList>
class CLRUSet : public CObject<CAllocatorType> {
public:
	struct SLRUSetItem : public CObject<CAllocatorType>
	{
		SLRUSetItem *pNextItem, *pPreviousItem;
		KeyType key;
		ValueType value;
	};

	class CLRUSetIterator
	{
		friend class CLRUSet;
	public:
		bool HashNext() const;
		const ValueType& Next();
	private:
		CLRUSetIterator() {}
		SLRUSetItem* current;
	};

	CLRUSet();
	CLRUSet(short in_setSize, SLRUSetItem* in_itemArrayLocation);
	~CLRUSet();

	void Initialize(short in_setSize, SLRUSetItem* in_itemArrayLocation);
	bool IsItemInSet(const KeyType& in_key) const;
	bool IsItemInSet(const KeyType& in_key, ValueType& out_value) const;
	bool TouchItem(const KeyType& touch_key, const ValueType& touch_value);
	bool TouchItem(const KeyType& touch_key, const ValueType& touch_value, KeyType& evict_key, ValueType& evict_value);
	short GetOccupancy() const;
	CLRUSetIterator* InitAndReturnIterator();

	protected:
	bool EvicAndAdd(const KeyType& newKey, const ValueType& newValue);
	void MakeMRU(SLRUSetItem* touchedItem);

	SLRUSetItem *m_pNewestItem, *m_pOldestItem;
	SLRUSetItem *m_aItemArray;
	short m_count, m_setSize;
	CLRUSetIterator m_iterator;
};

template<LRUTemplateList>
LRUClassType::CLRUSet() : CObject<CAllocatorType>(), m_setSize(), m_count(0) {
	m_pOldestItem = NULL;
	m_pNewestItem = NULL;
	m_aItemArray = NULL;
}

template<LRUTemplateList>
LRUClassType::CLRUSet(short in_setSize, SLRUSetItem* in_itemArrayLocation) : m_count(0), m_setSize(in_setSize), m_aItemArray(in_itemArrayLocation) {
	if (m_setSize < 1)
	{
		m_setSize = 1;
	}
	m_pOldestItem = NULL;
	m_pNewestItem = NULL;
}

template<LRUTemplateList>
LRUClassType::~CLRUSet() {}

template<LRUTemplateList>
void LRUClassType::Initialize(short in_setSize, SLRUSetItem *in_itemArrayLocation) {
	CLRUSet(in_setSize, in_itemArrayLocation);
}

template<LRUTemplateList>
bool LRUClassType::IsItemInSet(const KeyType& in_key) const {
	SLRUSetItem* currentItem = m_pNewestItem;
	while (NULL != currentItem)
	{
		if (currentItem->key == in_key)
		{
			return true;
		}
		currentItem = currentItem->pPreviousItem;
	}
	return false;
}

template<LRUTemplateList>
bool LRUClassType::IsItemInSet(const KeyType& in_key, ValueType& out_value) const {
	SLRUSetItem* currentItem = m_pNewestItem;
	while(NULL != currentItem)
	{
		if (currentItem->key == in_key)
		{
			out_value = currentItem->value;
			return true;
		}
		currentItem = currentItem->pPreviousItem;
	}
	return false;
}

template<LRUTemplateList>
bool LRUClassType::TouchItem(const KeyType& touch_key, const ValueType& touch_value) {
	SLRUSetItem* currentItem = m_pNewestItem;
	while (currentItem)
	{
		if (currentItem->key == touch_key)
		{
			MakeMRU(currentItem);
			return true;
		}
		currentItem = currentItem->pPreviousItem;
	}
	bool wasItemEvicted = EvicAndAdd(touch_key, touch_value);
	return wasItemEvicted;
}

template<LRUTemplateList>
bool LRUClassType::TouchItem(const KeyType& touch_key,
		                     const ValueType& touch_value,
							 KeyType& evict_key,
							 ValueType& evict_value) {
	SLRUSetItem* currentItem = m_pNewestItem;
	while(currentItem)
	{
		if (currentItem->key == touch_key)
		{
			MakeMRU(currentItem);
			return false;
		}
		currentItem = currentItem->pPreviousItem;
	}
	if (m_count == m_setSize)
	{
		evict_key = m_pOldestItem->key;
		evict_value = m_pOldestItem->value;
	}

	bool wasItemEvicted = EvictAndAdd(touch_key, touch_value);
	return wasItemEvicted;
}

template<LRUTemplateList>
bool LRUClassType::EvicAndAdd(const KeyType& newKey, const ValueType& newValue) {
	if (m_count == 0)
	{
		SLRUSetItem* newItem = &m_aItemArray[0];
		newItem->key = newKey;
		newItem->value = newValue;
		newItem->pNextItem = NULL;
		newItem->pPreviousItem = NULL;
		m_pNewestItem = newItem;
		m_pOldestItem = newItem;
		m_count++;
		return false;
	}
	else
	{
		SLRUSetItem* newOldest = m_pOldestItem->pNextItem;
		SLRUSetItem* newNewest = m_pOldestItem;
		SLRUSetItem* oldNewest = m_pNewestItem;

		newNewest->key = newKey;
		newNewest->value = newValue;
		newNewest->pNextItem = NULL;
		newNewest->pPreviousItem = oldNewest;

		oldNewest->pNextItem = newNewest;
		newOldest->pPreviousItem = NULL;

		m_pNewestItem = newNewest;
		m_pOldestItem = newOldest;

		return true;
	}
}

template<LRUTemplateList>
void LRUClassType::MakeMRU(SLRUSetItem* touchedItem) {
	if (m_pNewestItem == touchedItem)
		return;
	if (touchedItem->pPreviousItem != NULL)
	{
		touchedItem->pPreviousItem->pNextItem = touchedItem->pNextItem;
		touchedItem->pNextItem->pPreviousItem = touchedItem->pPreviousItem;
	}
	else
	{
		m_pOldestItem = touchedItem->pNextItem;
		m_pOldestItem->pPreviousItem = NULL;
	}

	m_pNewestItem->pNextItem = touchedItem;
	touchedItem->pNextItem = NULL;
	touchedItem->pPreviousItem = m_pNewestItem;
	m_pNewestItem = touchedItem;
}

template<LRUTemplateList>
short LRUClassType::GetOccupancy() const {
	return m_count;
}

template<LRUTemplateList>
bool LRUClassType::CLRUSetIterator::HashNext() const {
	return (NULL != current) ? true : false;
}

template<LRUTemplateList>
const ValueType& LRUClassType::CLRUSetIterator::Next() {
	ValueType& val = current->value;
	current = current->pNextItem;
	return val;
}

}


#endif /* LRUSET_H_ */
















































