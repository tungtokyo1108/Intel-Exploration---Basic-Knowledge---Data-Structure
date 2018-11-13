/*
 * Asm.h
 * https://github.com/facebook/folly/blob/master/folly/portability/Asm.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_ASM_H_
#define FOLLY_PORTABILITY_ASM_H_

#include <folly/Portability.h>

#include <chrono>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace folly {
inline void asm_volatile_memory() {
#if defined(__clang__) || defined(__GNUC__)
  asm volatile("" : : : "memory");
#elif defined(_MSC_VER)
  ::_ReadWriteBarrier();
#endif
}

inline void asm_volatile_pause() {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
  ::_mm_pause();
#elif defined(__i386__) || FOLLY_X64
  asm volatile("pause");
#elif FOLLY_AARCH64 || defined(__arm__)
  asm volatile("yield");
#elif FOLLY_PPC64
  asm volatile("or 27,27,27");
#endif
}

inline std::uint64_t asm_rdtsc() {
#if _MSC_VER
  return (uint64_t)__rdtsc();
#elif defined(__i386__) || FOLLY_X64
  // read the timestamp counter on x86
  auto hi = std::uint32_t{};
  auto lo = std::uint32_t{};
  asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return (((std::uint64_t)lo) + (((std::uint64_t)hi) << 32));
#else
  return std::chrono::steady_clock::now().time_since_epoch().count();
#endif
}
}

#endif /* FOLLY_PORTABILITY_ASM_H_ */
