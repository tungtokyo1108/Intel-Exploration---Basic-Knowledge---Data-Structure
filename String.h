/*
 * String.h
 *
 *  Created on: Aug 2, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef STRING_H_
#define STRING_H_

#pragma warning(push)
#pragma warning(disable : 4996)

#include "utility.h"
#include "Debug.h"
#include "Object.h"
#include "MemCopy.h"

#include <stdarg.h>
#include <stdio.h>

namespace iSTD {

template<class CAllocatorType>
class CString : public CObject<CAllocatorType> {
public:

	CString(void);
	CString(size_t growSize);
	CString(const CString& str);
	CString(const char* str);
	virtual ~CString(void);

	operator const char* () const;

	bool operator == (const CString& str);
	bool operator == (const char* str);

	CString& operator = (const CString& str);
	CString& operator = (const char* str);

	CString& operator = (const bool val);
	CString& operator = (const char val);
	CString& operator = (const short val);
	CString& operator = (const int val);
	CString& operator = (const long val);
	CString& operator = (const float val);
	CString& operator = (const unsigned char val);
	CString& operator = (const unsigned short val);
	CString& operator = (const unsigned int val);
	CString& operator = (const unsigned long val);

	const CString operator + (const CString& str) const;
	const CString operator + (const char * str) const;

	const CString operator + (const bool val) const;
	const CString operator + (const char val) const;
	const CString operator + (const short val) const;
	const CString operator + (const int val) const;
	const CString operator + (const long val) const;
	const CString operator + (const float val) const;
	const CString operator + (const unsigned char val) const;
	const CString operator + (const unsigned short val) const;
	const CString operator + (const unsigned int val) const;
	const CString operator + (const unsigned long val) const;

	void operator += (const CString& str);
	void operator += (const char* str);
	void operator += (const bool val);
	void operator += (const char val);
	void operator += (const short val);
	void operator += (const int val);
	void operator += (const long val);
	void operator += (const float val);
	void operator += (const unsigned char val);
	void operator += (const unsigned short val);
	void operator += (const unsigned int val);
	void operator += (const unsigned long val);

	void Set(const char* str);
	void SetFormatted(const char* str, ... );
	void Append(const char* str);
	void AppendFormatted(const char* str, ... );
	size_t Length(void) const;
	bool IsEmpty(void) const;
	void DetachBuffer(void);

protected:
	char* m_pString;
	size_t m_Length;
	size_t m_BufferSize;
	size_t m_GrowSize;
	static const size_t s_cStringGrowSize = 128;
};

template<class CAllocatorType>
inline CString<CAllocatorType>::CString(size_t growSize) : CObject<CAllocatorType>() {
	m_pString = NULL;
	m_Length = 0;
	m_BufferSize = 0;
	m_GrowSize = growSize;
}

template<class CAllocatorType>
inline CString<CAllocatorType>::CString(const CString<CAllocatorType>& str) : CObject<CAllocatorType>() {
	m_pString = NULL;
	m_Length = 0;
	m_BufferSize = 0;
	m_GrowSize = str.m_GrowSize;
	Set(str.m_pString);
}

template<class CAllocatorType>
inline CString<CAllocatorType>::CString(const char* str) : CObject<CAllocatorType>() {
	m_pString = NULL;
	m_Length = 0;
	m_BufferSize = 0;
	m_GrowSize = s_cStringGrowSize;
	Set(str);
}

template<class CAllocatorType>
inline CString<CAllocatorType>::~CString(void) {
	CAllocatorType::Deallocate(m_pString);
	m_pString = NULL;
	m_BufferSize = 0;
}

template<class CAllocatorType>
inline CString<CAllocatorType>::operator const char *() const {
	return m_pString;
}

template<class CAllocatorType>
inline bool CString<CAllocatorType>::operator ==(const CString<CAllocatorType> &str) {
	if (str && m_pString)
	{
		return ( ::strcmp(m_pString,(const char*)str) == 0);
	}
	else
	{
		return false;
	}
}

template<class CAllocatorType>
inline bool CString<CAllocatorType>::operator ==(const char* str) {
	if (str && m_pString)
	{
		return ( ::strcmp(m_pString,str) == 0);
	}
	else
	{
		return false;
	}
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const CString<CAllocatorType>& str) {
	if (str)
	{
		Set((const char*)str);
	}
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const char* str) {
	if (str)
	{
		Set(str);
	}
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const bool val) {
	SetFormatted("%d", val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const char val) {
	SetFormatted("%d",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const short val) {
	SetFormatted("%d",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const int val) {
	SetFormatted("%d",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const long val) {
	SetFormatted("%d",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const float val) {
	double d = val;
	SetFormatted("%f",d);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const unsigned char val) {
	SetFormatted("%u",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const unsigned short val) {
	SetFormatted("%u",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const unsigned int val) {
	SetFormatted("%u",val);
	return *this;
}

template<class CAllocatorType>
inline CString<CAllocatorType>& CString<CAllocatorType>::operator =(const unsigned long val) {
	SetFormatted("%u",val);
	return *this;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const CString<CAllocatorType>& str) const {
	CString<CAllocatorType> res(*this);
	if (str)
	{
		res.Append((const char*)str);
	}
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const char* str) const {
	CString<CAllocatorType> res(*this);
	res.Append(str);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const bool val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%d",val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const char val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%d",val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const short val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%d",val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const int val) const {
	CString<CAllocatorTypes> res(*this);
	res.AppendFormatted("%d",val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const long val) const {
	CString<CAllocatorTypes> res(*this);
	res.AppendFormatted("%d",val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const float val) const {
	CString<CAllocatorTypes> res(*this);
	double d = val;
	res.AppendFormatted("%f",d);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const unsigned char val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%u",val);
	return res;

}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const unsigned short val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%u",val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const unsigned int val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%u", val);
	return res;
}

template<class CAllocatorType>
inline const CString<CAllocatorType> CString<CAllocatorType>::operator +(const unsigned long val) const {
	CString<CAllocatorType> res(*this);
	res.AppendFormatted("%u",val);
	return res;
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const CString<CAllocatorType>& str) {
	if (str)
	{
		Append((const char*)str);
	}
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const char* str) {
	Append(str);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const bool val) {
	AppendFormatted("%d",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const char val) {
	AppendFormamtted("%d",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const short val) {
	AppendFormatted("%d",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const int val) {
	AppendFormatted("%d",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const long val) {
	AppendFormatted("%d",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const float val) {
	double d = val;
	AppendFormatted("%f",d);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const unsigned char val) {
	AppendFormatted("%u",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const unsigned short val) {
	AppendFormatted("%u",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const unsigned int val) {
	AppendFormatted("%u",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::operator +=(const unsigned long val) {
	AppendFormatted("%u",val);
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::Set(const char* str) {
	if (str)
	{
		const size_t length = ::strlen(str) + 1;
		CAllocatorType::Deallocate(m_pString);
		m_pString = NULL;
		m_BufferSize = 0;

		m_pString = (char*)CAllocatorType::Allocate(length);
		ASSERT(m_pString);

		if (m_pString)
		{
			STRCPY(m_pString,length,str);
			m_Length = ::strlen(m_pString);
			m_BufferSize = length;
		}
	}
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::SetFormatted(const char* str, ...) {
	if (str)
	{
		va_list args;
		va_start(args,str);
		// const size_t length = ::_vscprintf( str, args ) + 1;
		const size_t length;
		va_end(args);

		CAllocatorType::Deallocate(m_pString);
		m_pString = NULL;
		m_BufferSize = 0;

		m_pString = (char*)CAllocatorType::Allocate(length);
		ASSERT(m_pString);
		if (m_pString)
		{
			va_start(args,str);
			VSNPRINTF( m_pString, length, length, str, args );
			va_end(args);

			m_Length = ::strlen(m_pString);
			m_BufferSize = length;
		}
	}
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::Append(const char* str) {
	if (str)
	{
		if (m_pString)
		{
			size_t length = m_Length + ::strlen(str) + 1;
			if (length > m_BufferSize)
			{
				length += m_GrowSize;
				char* temp = (char*)CAllocatorType::Allocate(length);
				ASSERT(temp);

				if (temp)
				{
					STRCPY(temp,length,m_pString);
					STRCAT(temp,length,str);
					CAllocatorType::Deallocate(m_pString);
					m_pString = temp;
					m_Length = ::strlen(m_pString);
					m_BufferSize = length;
				}
			}
			else
			{
				STRCAT(m_pString,m_BufferSize,str);
				m_Length = ::strlen(m_pString);
			}
		}
		else
		{
			Set(str);
		}
	}
}

template<class CAllocatorType>
inline void CString<CAllocatorType>::AppendFormatted(const char* str, ...) {
	if (str)
	{
		va_list args;
		va_start(args,str);
		// const size_t length = ::_vscprintf( str, args ) + 1;
		const size_t length;
		va_end(args);

		if (m_pString != NULL)
		{
			if ((m_Length + length) > m_BufferSize)
			{
				char* temp = (char*)CAllocatorType::Allocate(m_lengt + length);
				ASSERT(temp);

				if (temp)
				{
					MemCopy(temp,m_pString,m_Length);
					va_start(args,str);
					VSNPRINTF(temp + m_Length, length, length-m_GrowSize,str,args);
					va_end(args);

					char* oldStr = m_pString;
					m_pString = temp;
					m_BufferSize = m_Length + length;
					m_Length += ::strlen(m_pString + m_Length);
					CAllocatorType::Deallocate(oldStr);
				}
			}
			else
			{
				va_start(args,str);
				VSNPRINTF(m_pString + m_Length, m_BufferSize - m_Length, length, str, args);
				va_end(args);
				m_Length += ::strlen(m_pString + m_Length);
			}
		}
		else
		{
			m_BufferSize = 0;
			m_pString = (char*)CAllocatorType::Allocate(length);
			ASSERT(m_pString != NULL);
			if (m_pString)
			{
				va_start(args,str);
				VSNPRINTF(m_String, length, length, str, args);
				va_end(args);
				m_Length = ::strlen(m_pString);
				m_BufferSize = length;
			}
		}
	}
}

template<class CAllocatorType>
size_t CString<CAllocatorType>::Length(void) const {
	return m_Length;
}

template<class CAllocatorType>
bool CString<CAllocatorType>::IsEmpty(void) const {
	return (m_Length == 0);
}

template<class CAllocatorType>
void CString<CAllocatorType>::DetachBuffer(void) {
	m_pString = NULL;
}


}

#endif /* STRING_H_ */

















