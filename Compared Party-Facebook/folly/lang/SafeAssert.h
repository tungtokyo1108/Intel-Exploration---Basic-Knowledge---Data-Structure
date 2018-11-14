/*
 * SafeAssert.h
 * https://github.com/facebook/folly/blob/master/folly/lang/SafeAssert.h
 *
 *  Created on: Nov 14, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_LANG_SAFEASSERT_H_
#define FOLLY_LANG_SAFEASSERT_H_

#include <folly/Portability.h>
#include <folly/Preprocessor.h>

#define FOLLY_SAFE_CHECK_IMPL(expr, expr_s, msg) \
	((expr) ? static_cast<void>(0)                 \
          : ::folly::detail::assertionFailure(   \
                FB_STRINGIZE(expr_s),            \
                (msg),                           \
                __FILE__,                        \
                __LINE__,                        \
                __PRETTY_FUNCTION__))
#define FOLLY_SAFE_CHECK(expr,msg) FOLLY_SAFE_CHECK_IMPL((expr), (expr), (msg))

#define FOLLY_SAFE_DCHECK(expr, msg) \
	FOLLY_SAFE_CHECK_IMPL(!folly::kIsDebug || (expr), (expr), (msg))

namespace folly {
namespace detail {
[[noreturn]] void assertionFailure (
		const char* expr,
		const char* msg,
		const char* file,
	    unsigned int line,
		const char* function);
}
}

#endif /* FOLLY_LANG_SAFEASSERT_H_ */
