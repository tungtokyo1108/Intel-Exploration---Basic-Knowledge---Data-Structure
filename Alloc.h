/*
 * Alloc.h
 *
 *  Created on: Jul 22, 2018
 *      Author: tungdang
 */

#ifndef ALLOC_H_
#define ALLOC_H_

#pragma once
#include "Debug.h"
#ifdef __cplusplus

namespace iSTD {

template <class Type>
inline void SafeDelete(Type &ptr) {
	if (ptr)
	{
#if defined( _DEBUG ) && !defined(NO_EXCEPTION_HANDLING)
#if defined ( __GNC__)
		try
		{
			delete ptr;
		}
		catch(...)
		{
			ASSERT(0);
		}
#else
		try
		{
			delete ptr;
		}
		__except(1)
		{
			ASSERT(0);
		}
#endif
	}
#else
	delete ptr;
#endif
	ptr = 0;
}

}

#endif /* ALLOC_H_ */
