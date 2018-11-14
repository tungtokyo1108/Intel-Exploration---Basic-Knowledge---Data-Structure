/*
 * Assume.h
 * https://github.com/facebook/folly/blob/master/folly/lang/Assume.h
 *
 *  Created on: Nov 14, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_LANG_ASSUME_H_
#define FOLLY_LANG_ASSUME_H_

#include <cstdlib>
#include <folly/Portability.h>

namespace folly {
namespace detail {

extern void assume_check(bool cond);

}

/*
 * Inform the compiler that the argument can be assumed true.
 * It is undefined behavior if the argument is not actually true.
 *
 * Implemented as a function instead of a macro because
 * __builtin_assume does not evaluate its argument at runtime
 */
FOLLY_ALWAYS_INLINE void assume(bool cond) {
	if (kIsDebug)
	{
		detail::assume_check(cond);
	}
	else
	{
#if defined(__clang__)
		__builtin_assume(cond);
#elif defined(__GNUC__)
		if (!cond) {
			__builtin_unreachable();
		}
#elif defined(_MSC_VER)
		__assume(cond);
#endif
	}
}

[[noreturn]] FOLLY_ALWAYS_INLINE void assume_unreachable() {
  assume(false);
  // Do a bit more to get the compiler to understand
  // that this function really will never return.
#if defined(__GNUC__)
  __builtin_unreachable();
#elif defined(_MSC_VER)
  __assume(0);
#else
  // Well, it's better than nothing.
  std::abort();
#endif
}

}

#endif /* FOLLY_LANG_ASSUME_H_ */
