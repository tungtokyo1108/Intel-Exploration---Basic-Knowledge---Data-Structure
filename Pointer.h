/*
 * Pointer.h
 *
 *  Created on: Jul 30, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef POINTER_H_
#define POINTER_H_
#include "Object.h"

#pragma once
namespace iSTD {

template<class Type>
class CPointer {
public:
	CPointer(void);
	virtual ~CPointer(void);

	CPointer<Type>& operator = (Type* rValue);
	CPointer<Type>& operator = (CPointer<Type> rValue);

	Type& operator[] (long index);
	Type& operator* (void);
	Type* operator-> (void);

protected:

	Type* m_Pointer;
};

template<class Type>
CPointer<Type>::CPointer(void) {
	m_Pointer = NULL;
}

template<class Type>
CPointer<Type>::~CPointer(void) {
	m_Pointer = NULL;
}

template<class Type>
CPointer<Type>& CPointer<Type>::operator =( Type* rValue) {
	m_Pointer = rValue;
	return *this;
}

template<class Type>
CPointer<Type>& CPointer<Type>::operator =(CPointer<Type> rValue) {
	m_Pointer = rValue.m_Pointer;
	return *this;
}

template<class Type>
Type& CPointer<Type>::operator[] (long index) {
	return m_Pointer[index];
}

template<class Type>
Type& CPointer<Type>::operator *(void) {
	return *m_Pointer;
}

template<class Type>
Type* CPointer<Type>::operator ->(void) {
	return m_Pointer;
}

}

#endif /* POINTER_H_ */
