/*
 * osinlines.h
 *
 *  Created on: Jul 19, 2018
 *      Student (Coder): Tung Dang
 */
#pragma once

#ifndef OSINLINES_H_
#define OSINLINES_H_

#ifndef _MSC_VER
#define C_ASSERT(e) typedef char _C_ASSERT_[(e)?1:-1]
#endif

#ifndef _GNUC_
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

inline int _vsnprint(char *buffer, size_t count, const char *msg, va_list args) {
	va_list args_cpy;
	va_copy(args_cpy, args);
	int len = vsnprintf(buffer,count,msg,args_cpy);
	va_end(args_cpy);
	return len;
}

void *aligned_malloc(size_t _Size, size_t Aligment);
void _aligned_free(void *_Memory);
#endif

#endif /* OSINLINES_H_ */
