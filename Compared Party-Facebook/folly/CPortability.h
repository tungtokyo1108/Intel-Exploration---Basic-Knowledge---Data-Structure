/*
 * CPortability.h
 * https://github.com/facebook/folly/blob/master/folly/CPortability.h
 *
 *  Created on: Nov 6, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_CPORTABILITY_H_
#define FOLLY_CPORTABILITY_H_

#include <folly/portability/Config.h>

#ifndef __GNUC_PREREQ
#if defined __GNUC__ && defined __GNUC_MINOR__
/* nolint */
#define __GNUC_PREREQ(maj, min) \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
/* nolint */
#define __GNUC_PREREQ(maj, min) 0
#endif
#endif

// portable version check for clang
#ifndef __CLANG_PREREQ
#if defined __clang__ && defined __clang_major__ && defined __clang_minor__
/* nolint */
#define __CLANG_PREREQ(maj, min) \
  ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
#else
/* nolint */
#define __CLANG_PREREQ(maj, min) 0
#endif
#endif

#if defined(__has_builtin)
#define FOLLY_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define FOLLY_HAS_BUILTIN(...) 0
#endif

#if defined(__has_feature)
#define FOLLY_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define FOLLY_HAS_FEATURE(...) 0
#endif

#if FOLLY_HAS_FEATURE(address_sanitizer) || __SANITIZE_ADDRESS__
#define FOLLY_SANITIZE_ADDRESS 1
#endif

#ifdef FOLLY_SANITIZE_ADDRESS
#if defined(__clang__)
#if __has_attribute(__no_sanitize__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_sanitize__("address"), __noinline__))
#elif __has_attribute(__no_address_safety_analysis__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_address_safety_analysis__, __noinline__))
#elif __has_attribute(__no_sanitize_address__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_sanitize_address__, __noinline__))
#endif
#elif defined(__GNUC__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_address_safety_analysis__, __noinline__))
#endif
#endif
#ifndef FOLLY_DISABLE_ADDRESS_SANITIZER
#define FOLLY_DISABLE_ADDRESS_SANITIZER
#endif

#if FOLLY_HAS_FEATURE(thread_sanitizer) || __SANITIZE_THREAD__
#define FOLLY_SANITIZE_THREAD 1
#endif

/**
 * Define a convenience macro to test when ASAN, UBSAN or TSAN sanitizer are
 * being used
 */
#if defined(FOLLY_SANITIZE_ADDRESS) || defined(FOLLY_SANITIZE_THREAD)
#define FOLLY_SANITIZE 1
#endif

#if FOLLY_SANITIZE
#define FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...) \
  __attribute__((no_sanitize(__VA_ARGS__)))
#else
#define FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...)
#endif

#if defined(__GNUC__)
#if __GNUC_PREREQ(4,9)
#define FOLLY_EXPORT [[gnu::visibility("default")]]
#else
#define FOLLY_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define FOLLY_EXPORT
#endif

#ifdef _MSC_VER
#define FOLLY_NOINLINE __declspec(noinline)
#elif defined(__clang__) || defined(__GNUC__)
#define FOLLY_NOINLINE __attribute__((__noinline__))
#else
#define FOLLY_NOINLINE
#endif

#ifdef _MSC_VER
#define FOLLY_ALWAYS_INLINE __forceinline
#elif defined(__clang__) || defined(__GNUC__)
#define FOLLY_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define FOLLY_ALWAY_INLINE inline
#endif

#if FOLLY_HAVE_WEAK_SYMBOLS
#define FOLLY_ATTR_WEAK __attribute__((__weak__))
#else
#define FOLLY_ATTR_WEAK
#endif

#ifndef FOLLY_MICROSOFT_ABI_VER
#ifdef _MSC_VER
#define FOLLY_MICROSOFT_ABI_VER _MSC_VER
#endif
#endif

#endif /* FOLLY_CPORTABILITY_H_ */
