/*
 * type.h
 *
 *  Created on: Jul 19, 2018
 *      Student (Coder): Tung Dang
 */

#pragma once
#ifndef ISTDLIB_TYPES_H_INCLUDED
#define ISTDLIB_TYPES_H_INCLUDED

#include "cppcompiler.h"
#include "osinlines.h"
#include <stdint.h>

typedef int64_t QWORD;
typedef int64_t UINT_PTR;
typedef unsigned long DWORD;
typedef int64_t INT64;
typedef uint64_t UINT64;
typedef unsigned long ULONG;
typedef unsigned int UINT32;
typedef unsigned int UINT;
typedef int8_t UINT8;
typedef int16_t UINT16;
typedef int INT;
typedef unsigned char BYTE;
typedef unsigned short WORD;

#ifndef _cplusplus
template <class Type>
struct SRange {
	Type Min;
	Type Max;
};

template <class Type>
struct SRangeA {
	Type Min;
	Type Max;
	SRangeA(const Type min, const Type max)
	{
		Min = min;
		Max = max;
	};
};
#endif

#ifndef MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)
#endif

union VALUE8 {
	unsigned char u;
	signed char s;
	struct
	{
		unsigned char l : 4;
		unsigned char h : 4;
	};
};

union VALUE16 {
	unsigned short u;
	signed short s;
	struct
	{
		VALUE8 l;
		VALUE8 h;
	};
};

union VALUE32 {
	unsigned int u;
	signed int s;
	float f;
	DWORD d;

	struct {
		VALUE16 l;
		VALUE16 h;
	};
};

union FLOAT8 {
	struct
	{
		DWORD fraction : 4;
		DWORD exponent : 3;
		DWORD sign : 1;
	};

	VALUE8 value;
	FLOAT8()
	{
		value.u = 0;
	};
	FLOAT8 (unsigned char uVal)
	{
		value.u = uVal;
	};
	FLOAT8 (signed char sVal)
	{
		value.s = sVal;
	};
};

union FLOAT16 {
	struct
	{
		DWORD fraction : 23;
		DWORD exponent : 8;
		DWORD sign : 1;
	};
	VALUE16 value;
};

union FLOAT32 {
	struct
	{
		DWORD fraction : 23;
		DWORD exponent : 8;
		DWORD sign : 1;
	};
	VALUE32 value;
	FLOAT32()
	{
		value.f = 0;
	};
	FLOAT32(unsigned long uVal)
	{
		value.u = uVal;
	};
	FLOAT32(signed long sVal)
	{
		value.s = sVal;
	};
	FLOAT32(float fVal)
	{
		value.f = fVal;
	};
};

const DWORD BITS_PER_BYTE = 8;
const DWORD BITS_PER_WORD = BITS_PER_BYTE * sizeof(WORD);
const DWORD BITS_PER_DWORD = BITS_PER_BYTE * sizeof(DWORD);

#endif /* TYPE_H_ */















































