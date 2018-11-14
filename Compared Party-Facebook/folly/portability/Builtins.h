/*
 * Builtins.h
 * https://github.com/facebook/folly/blob/master/folly/portability/Builtins.h
 *
 *  Created on: Nov 14, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_BUILTINS_H_
#define FOLLY_PORTABILITY_BUILTINS_H_

#include <assert.h>
#include <folly/Portability.h>
#include <intrin.h>
#include <stdint.h>

namespace folly {
namespace portability {
namespace detail {

void call_flush_instruction_cache_self_pid(void* begin, size_t size);

}
}
}

FOLLY_ALWAYS_INLINE void __builtin___clear_cache(char* begin, char* end) {
	if (folly::kIsArchAmd64)
	{

	}
	else
	{
		folly::portability::detail::call_flush_instruction_cache_self_pid(
		  static_cast<void*>(begin), static_cast<size_t>(end-begin));
	}
}

FOLLY_ALWAYS_INLINE int __builtin_clz(unsigned int x) {
	unsigned long index;
	return int(_BitScanReverse(&index, (unsigned long)x) ? 31 - index : 32);
}

FOLLY_ALWAYS_INLINE int __builtin_clzl(unsigned long x) {
	return __builtin_clz((unsigned int)x);
}

FOLLY_ALWAYS_INLINE int __builtin_clzll(unsigned long long x) {
	unsigned long index;
	return int(_BitScanReverse64(&index, x) ? 63 - index : 64);
}

FOLLY_ALWAYS_INLINE int __builtin_ctz(unsigned int x) {
  unsigned long index;
  return int(_BitScanForward(&index, (unsigned long)x) ? index : 32);
}

FOLLY_ALWAYS_INLINE int __builtin_ctzl(unsigned long x) {
  return __builtin_ctz((unsigned int)x);
}

FOLLY_ALWAYS_INLINE int __builtin_ctzll(unsigned long long x) {
  unsigned long index;
  return int(_BitScanForward64(&index, x) ? index : 64);
}

FOLLY_ALWAYS_INLINE int __builtin_ffs(int x) {
  unsigned long index;
  return int(_BitScanForward(&index, (unsigned long)x) ? index + 1 : 0);
}

FOLLY_ALWAYS_INLINE int __builtin_ffsl(long x) {
  return __builtin_ffs(int(x));
}

FOLLY_ALWAYS_INLINE int __builtin_ffsll(long long x) {
  unsigned long index;
  return int(_BitScanForward64(&index, (unsigned long long)x) ? index + 1 : 0);
}

FOLLY_ALWAYS_INLINE int __builtin_popcount(unsigned int x) {
  return int(__popcnt(x));
}

FOLLY_ALWAYS_INLINE int __builtin_popcountl(unsigned long x) {
  static_assert(sizeof(x) == 4, "");
  return int(__popcnt(x));
}

FOLLY_ALWAYS_INLINE int __builtin_popcountll(unsigned long long x) {
  return int(__popcnt64(x));
}

FOLLY_ALWAYS_INLINE void* __builtin_return_address(unsigned int frame) {
  // I really hope frame is zero...
  (void)frame;
  assert(frame == 0);
  return _ReturnAddress();
}

#endif /* FOLLY_PORTABILITY_BUILTINS_H_ */
