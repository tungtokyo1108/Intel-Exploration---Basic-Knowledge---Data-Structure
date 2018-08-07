/*
 * Queue.h
 *
 *  Created on: Jul 31, 2018
 *      Author: tungdang
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#pragma once
#include "LinkedList.h"
namespace iSTD {

#define QueueTemplateList class Type, class CAllocatorType
#define CQueueType CQueue<Type,CAllocatorType>

template<QueueTemplateList>
class CQueue : public CLinkedListType {
public:
	bool Push(const Type element);
	Type Pop(void);
	Type Top(void) const;
};

template<QueueTemplateList>
bool CQueueType::Push(const Type element) {
	return this->Add(element);
}

template<QueueTemplateList>
Type CQueueType::Pop(void) {
	Type element = {0};
	if (this->IsEmpty())
	{
		ASSERT(0);
	}
	else
	{
		typename CQueue::CIterator end = this->End();
		--end;

		element = *end;
		this->Remove(end);
	}
	return element;
}

template<QueueTemplateList>
Type CQueueType::Top(void) const {
	Type element = {0};
	if (this->IsEmpty())
	{
		ASSERT(0);
	}
	else
	{
		typename CQueueType::CConstIterator end = this->End();
	    --end;
	    element = *end;
	}
	return element;
}

}


#endif /* QUEUE_H_ */
