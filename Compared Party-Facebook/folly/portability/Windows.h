/*
 * Windows.h
 * https://github.com/facebook/folly/blob/master/folly/portability/Windows.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_WINDOWS_H_
#define FOLLY_PORTABILITY_WINDOWS_H_

#ifdef _WIN32
#ifndef __STDC__
/* nolint */
#define __STDC__ 1
#include <direct.h> // @manual nolint
#include <io.h> // @manual nolint
#undef __STDC__
#else
#include <direct.h> // @manual nolint
#include <io.h> // @manual nolint
#endif

#if defined(min) || defined(max)
#error Windows.h needs to be included by this header, or else NOMINMAX needs \
 to be defined before including it yourself.
#endif

// This is needed because, for some absurd reason, one of the windows headers
// tries to define "min" and "max" as macros, which messes up most uses of
// std::numeric_limits.
#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <WinSock2.h> // @manual
#include <Windows.h> // @manual

#ifdef CAL_GREGORIAN
#undef CAL_GREGORIAN
#endif

// Defined in the GDI interface.
#ifdef ERROR
#undef ERROR
#endif

// Defined in minwindef.h
#ifdef IN
#undef IN
#endif

// Defined in winerror.h
#ifdef NO_ERROR
#undef NO_ERROR
#endif

// Defined in minwindef.h
#ifdef OUT
#undef OUT
#endif

// Defined in minwindef.h
#ifdef STRICT
#undef STRICT
#endif

// Defined in Winbase.h
#ifdef Yield
#undef Yield
#endif

#endif

#endif /* FOLLY_PORTABILITY_WINDOWS_H_ */
