/*
 * Debug.h
 *
 *  Created on: Jul 19, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef DEBUG_H_
#define DEBUG_H_
#pragma once
#ifndef _cplusplus
#ifndef ASSERT
#define ASSERT(expr)
#endif

#ifndef DBF
#define DBF(debugLevel, message, ...)
#endif

#define GFXDBG_STDLIB (0x00001000)

#ifndef NODEFAULT
#ifndef _MSC_VER
#define NODEFAULT _assume(0)
#else
#define NODEFAULT
#endif
#endif
#endif

#endif /* DEBUG_H_ */
