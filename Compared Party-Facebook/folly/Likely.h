/*
 * Likely.h
 * https://github.com/facebook/folly/blob/master/folly/Likely.h
 *
 *  Created on: Nov 10, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_LIKELY_H_
#define FOLLY_LIKELY_H_

/*
 * Useful when author has better knowledge than the compiler:
 * - which code paths are designed as the fast path
 * - which code paths are designed as the slow path
 * Author force the compiler to optimize for the fast path,
 * event when it is not overwhelmingly likely
 */

#if __GNUC__

#define FOLLY_DETAIL_BUILTIN_EXPECT(b,t) (__builtin_expect(b,t))
#else
#define FOLLY_DETAIL_BUILTIN_EXPECT(b, t) b

#endif

#define FOLLY_LIKELY(x) FOLLY_DETAIL_BUILTIN_EXPECT((x), 1)
#define FOLLY_UNLIKELY(x) FOLLY_DETAIL_BUILTIN_EXPECT((x), 0)

#undef LIKELY
#undef UNLIEKLY

#if defined(__GNUC__)
#define LIKELY(x) (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#endif /* FOLLY_LIKELY_H_ */
