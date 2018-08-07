/*
 * Vector.h
 *
 *  Created on: Aug 2, 2018
 *      Author: tungdang
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#pragma once
#include <type.h>
#include <Debug.h>

namespace iSTD {

template<class Type>
struct SVector4 {
	Type X;
	Type Y;
	Type Z;
	Type W;

	Type& operator[] (const int i)
	{
		ASSERT(i >= 0);
		ASSERT(i <= 3);
		return (((Type*)this)[i]);
	}

	const Type& operator[] (const int i) const
	{
		ASSERT(i >= 0);
		ASSERT(i <= 3);
		return (((Type*)this)[i]);
	}

	bool operator == (const SVector4&& vector) const {
		return ((X == vector.X) &&
				(Y == vector.Y) &&
				(Z == vector.Z) &&
				(W == vector.W));
	}

	bool operator != (const SVector4&& vector) const {
		return ((X != vector.X) &&
				(Y != vector.Y) &&
				(Z != vector.Z) &&
				(W != vector.W));
	}
};



}


#endif /* VECTOR_H_ */
