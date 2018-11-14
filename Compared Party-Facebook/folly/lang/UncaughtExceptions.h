/*
 * UncaughtExceptions.h
 * https://github.com/facebook/folly/blob/master/folly/lang/UncaughtExceptions.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_LANG_UNCAUGHTEXCEPTIONS_H_
#define FOLLY_LANG_UNCAUGHTEXCEPTIONS_H_

#include <exception>


#if !defined(FOLLY_FORCE_EXCEPTION_COUNT_USE_STD) && \
    (defined(__GNUG__) || defined(__clang__))
#define FOLLY_EXCEPTION_COUNT_USE_CXA_GET_GLOBALS
namespace __cxxabiv1 {
// forward declaration (originally defined in unwind-cxx.h from from libstdc++)
struct __cxa_eh_globals;
// declared in cxxabi.h from libstdc++-v3
extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;
} // namespace __cxxabiv1
#elif defined(_MSC_VER) && (_MSC_VER >= 1400) && \
    (_MSC_VER < 1900) // MSVC++ 8.0 or greater
#define FOLLY_EXCEPTION_COUNT_USE_GETPTD
// forward declaration (originally defined in mtdll.h from MSVCRT)
struct _tiddata;
extern "C" _tiddata* _getptd(); // declared in mtdll.h from MSVCRT
#elif defined(FOLLY_FORCE_EXCEPTION_COUNT_USE_STD) || \
    (defined(_MSC_VER) && (_MSC_VER >= 1900)) // MSVC++ 2015
#define FOLLY_EXCEPTION_COUNT_USE_STD
#else
// Raise an error when trying to use this on unsupported platforms.
#error "Unsupported platform, don't include this header."
#endif

namespace folly {

/*
 * Return the number of uncaught exception
 */
inline int uncaught_exceptions() noexcept {
#if defined(FOLLY_EXCEPTION_COUNT_USE_CXA_GET_GLOBALS)
	return *(reinterpret_cast<unsigned int*>(
			static_cast<char*>(static_cast<void*>(__cxxabiv1::__cxa_get_globals()))
			+ sizeof(void*)));
#elif defined(FOLLY_EXCEPTION_COUNT_USE_GETPTD)
	return *(reinterpret_cast<int*>(
	      static_cast<char*>(static_cast<void*>(_getptd())) + sizeof(void*) * 28 +
	0x4 * 8));
#elif defined(FOLLY_EXCEPTION_COUNT_USE_STD)
return std::uncaught_exceptions();
#endif
}

}

#endif /* FOLLY_LANG_UNCAUGHTEXCEPTIONS_H_ */
