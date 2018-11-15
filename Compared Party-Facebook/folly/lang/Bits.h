/*
 * Bits.h
 * https://github.com/facebook/folly/blob/master/folly/lang/Bits.h
 *
 * Intel
 * https://github.com/tungtokyo1108/Intel-Exploration---Cloud-Section---CPU-ModernCloud/blob/master/include/qemu/bitops.h
 * https://github.com/tungtokyo1108/Intel-Exploration---Cloud-Section---CPU-ModernCloud/blob/master/include/qemu/bitmap.h
 * https://github.com/tungtokyo1108/Intel-Exploration---Cloud-Section---CPU-ModernCloud/blob/master/include/qemu/bswap.h
 *
 *  Created on: Nov 14, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_LANG_BITS_H_
#define FOLLY_LANG_BITS_H_

#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#include <folly/ConstexprMath.h>
#include <folly/Portability.h>
#include <folly/Utility.h>
#include <folly/lang/Assume.h>
#include <folly/portability/Builtins.h>

namespace folly {
namespace detail {

template <typename Dst, typename Src>
constexpr std::make_signed_t<Dst> bits_to_signed(Src const s) {
	static_assert(std::is_signed<Dst>::value, "unsigned type");
	return to_signed(static_cast<std::make_unsigned_t<Dst>>(to_unsigned(s)));
}

template <typename Dst, typename Src>
constexpr std::make_unsigned_t<Dst> bits_to_unsigned(Src const s) {
	static_assert(std::is_unsigned<Dst>::value, "signed type");
	return static_cast<Dst>(to_unsigned(s));
}
}

template <typename T>
inline constexpr unsigned int findFirstSet(T const v) {
	using S0 = int;
	using S1 = long int;
	using S2 = long long int;
	using detail::bits_to_signed;
	static_assert(sizeof(T) <= sizeof(S2), "over-sized type");
	static_assert(std::is_integral<T>::value, "non-integral type");
	static_assert(!std::is_same<T, bool>::value, "bool type");

	return static_cast<unsigned int> (
			sizeof(T) <= sizeof(S0) ? __builtin_ffs(bits_to_signed<S0>(v)) :
			sizeof(T) <= sizeof(S1) ? __builtin_ffsl(bits_to_signed<S1>(v)) :
			sizeof(T) <= sizeof(S2) ? __builtin_ffsll(bits_to_signed<S2>(v)) :
	0);
}

template <typename T>
inline constexpr unsigned int findLastSet(T const v) {
	using U0 = unsigned int;
	using U1 = unsigned long int;
	using U2 = unsigned long long int;
	using detail::bits_to_unsigned;
	static_assert(sizeof(T) <= sizeof(U2), "over-sized type");
	static_assert(std::is_integral<T>::value, "non-integral type");
	static_assert(!std::is_same<T, bool>::value, "bool type");

	using size = index_constant<constexpr_max(sizeof(T), sizeof(U0))>;
	return v ? 1u + static_cast<unsigned int>((8u * size{} - 1u) ^ (
			sizeof(T) <= sizeof(U0) ? __builtin_clz(bits_to_unsigned<U0>(v)) :
			sizeof(T) <= sizeof(U1) ? __builtin_clzl(bits_to_unsigned<U1>(v)) :
			sizeof(T) <= sizeof(U2) ? __builtin_clzll(bits_to_unsigned<U2>(v)) :
	0)) : 0u;
}

template <typename T>
inline constexpr unsigned int popcount(T const v) {
	using U0 = unsigned int;
	using U1 = unsigned long int;
	using U2 = unsigned long long int;
	using detail::bits_to_unsigned;
	static_assert(sizeof(T) <= sizeof(U2), "over-sized type");
	static_assert(std::is_integral<T>::value, "non-integral type");
	static_assert(!std::is_same<T, bool>::value, "bool type");

	return static_cast<unsigned int> (
			sizeof(T) <= sizeof(U0) ? __builtin_popcount(bits_to_unsigned<U0>(v)) :
			sizeof(T) <= sizeof(U1) ? __builtin_popcountl(bits_to_unsigned<U1>(v)) :
			sizeof(T) <= sizeof(U2) ? __builtin_popcountll(bits_to_unsigned<U2>(v)) :
	0);
}

template <class T>
inline constexpr T nextPowTwo(T const v) {
	static_assert(std::is_unsigned<T>::value, "signed type");
	return v ? (T(1) << findLastSet(v - 1)) : T(1);
}

template <class T>
inline constexpr T prevPowTwo(T const v) {
	static_assert(std::is_unsigned<T>::value, "signed type");
	return v ? (T(1) << (findLastSet(v) - 1)) : T(0);
}

template <class T>
inline constexpr bool isPowTwo(T const v) {
	static_assert(std::is_integral<T>::value, "non-integral type");
	static_assert(std::is_unsigned<T>::value, "signed type");
	static_assert(!std::is_same<T, bool>::value, "bool type");
	return (v != 0) && !(v & (v - 1));
}

namespace detail {

template <size_t Size>
struct uint_types_by_size;

#define FB_GEN(sz, fn) \
	static inline uint##sz##_t byteswap_gen(uint##sz##_t v) { \
	   return fn(v); \
	}\
	template <> \
	struct uint_types_by_size<sz/8> { \
		using type = uint##sz##_t;\
	};\

FB_GEN(8, uint8_t)
#ifdef _MSC_VER
FB_GEN(64, _byteswap_uint64)
FB_GEN(32, _byteswap_ulong)
FB_GEN(16, _byteswap_ushort)
#else
FB_GEN(64, __builtin_bswap64)
FB_GEN(32, __builtin_bswap32)
FB_GEN(16, __builtin_bswap16)
#endif

#undef FB_GEN

template <class T>
struct EndianInt {
	static_assert(
	      (std::is_integral<T>::value && !std::is_same<T, bool>::value) ||
	          std::is_floating_point<T>::value,
	"template type parameter must be non-bool integral or floating point");
	static T swap(T x) {
		/*
		 * Implement swap with memcpy because that is defined behavior in C++
		 * Rely on compiler to optimize away the memcpy calls
		 */
		constexpr auto s = sizeof(T);
		using B = typename uint_types_by_size<s>::type;
		B b;
		std::memcpy(&b, &x, s);
		b = byteswap_gen(b);
		std::memcpy(&x, &b, s);
		return x;
	}

	static T big(T x) {
		return kIsLittleEndian ? EndianInt::swap(x) : x;
	}

	static T little(T x) {
		return kIsBigEndian ? EndianInt::swap(x) : x;
	}
};
}

/*
 * big* convert between native and big-endian representation
 * little* convert between native and little-endian representation
 * swap* convert between big-endian and little-endian represenation
 */
#define FB_GEN1(fn, t, sz) \
	static t fn##sz(t x) { \
	    return fn<t>(x); \
    }

#define FB_GEN2(t, sz) \
	FB_GEN1(swap,t,sz) \
	FB_GEN1(big, t, sz) \
	FB_GEN1(little, t, sz)

#define FB_GEN(sz)          \
    FB_GEN2(uint##sz##_t, sz) \
    FB_GEN2(int##sz##_t, sz)

class Endian {
public:
	enum class Order : uint8_t {
		LITTLE,
		BIG
	};

	static constexpr Order order = kIsLittleEndian ? Order::LITTLE : Order::BIG;

	template <class T>
	static T swap(T x) {
		return folly::detail::EndianInt<T>::swap(x);
	}
	template <class T>
	static T big(T x) {
		return folly::detail::EndianInt<T>::big(x);
	}
	template <class T>
	static T little(T x) {
		return folly::detail::EndianInt<T>::little(x);
	}


#if !defined(__ANDROID__)
  FB_GEN(64)
  FB_GEN(32)
  FB_GEN(16)
  FB_GEN(8)
#endif

};

#undef FB_GEN
#undef FB_GEN2
#undef FB_GEN1

template <class T, class Enable = void>
struct Unaligned;

FOLLY_PACK_PUSH
template <class T>
struct Unaligned<T, typename std::enable_if<std::is_pod<T>::value>::tpye> {
	Unaligned() = default;
	Unaligned(T v) : value(v) {}
	T value;
} FOLLY_PACK_ATTR;
FOLLY_PACK_POP

template <class T>
inline T loadUnaligned(const void* p) {
	static_assert(sizeof(Unaligned<T>) == sizeof(T), "Invalid unaligned size");
	static_assert(alignof(Unaligned<T>) == 1, "Invalid alignment");
	if (kHasUnalignedAccess)
	{
		return static_cast<const Unaligned<T>*>(p)->value;
	}
	else
	{
		T value;
		memcpy(&value, p, sizeof(T));
		return value;
	}
}

template <class T>
inline T partialLoadUnaligned(const void* p, size_t l) {
	static_assert(
	      std::is_integral<T>::value && std::is_unsigned<T>::value &&
	          sizeof(T) <= 8,
	"Invalid type");
	assume(l < sizeof(T));

	auto cp = static_cast<const char*>(p);
	T value = 0;
	if (!kHasUnalignedAccess || !kIsLittleEndian)
	{
		memcpy(&value, cp, l);
		return value;
	}

	auto avail = l;
	if ( l & 4)
	{
		avail -= 4;
		value = static_cast<T>(loadUnaligned<uint32_t>(cp + avail)) << (avail * 8);
	}
	if ( l & 2)
	{
		avail -= 2;
		value |= static_cast<T>(loadUnaligned<uint16_t>(cp + avail)) << (avail * 8);
	}
	if ( l & 1)
	{
		value |= loadUnaligned<uint8_t>(cp);
	}
	return value;
}

template <class T>
inline void storeUnaligned(void* p, T value) {
	static_assert(sizeof(Unaligned<T>) == sizeof(T), "Invalid unaligned size");
	static_assert(alignof(Unaligned<T>) == 1, "Invalid alignment");
	if (kHasUnalignedAccess)
	{
		assume(p != nullptr);
		new (p) Unaligned<T>(value);
	}
	else
	{
		memcpy(p, &value, sizeof(T));
	}
}

template <typename T>
T bitReverse(T n) {
	auto m = static_cast<typename std::make_unsigned<T>::type>(n);
	m = ((m & 0xAAAAAAAAAAAAAAAA) >> 1) | ((m & 0x5555555555555555) << 1);
    m = ((m & 0xCCCCCCCCCCCCCCCC) >> 2) | ((m & 0x3333333333333333) << 2);
	m = ((m & 0xF0F0F0F0F0F0F0F0) >> 4) | ((m & 0x0F0F0F0F0F0F0F0F) << 4);
	return static_cast<T>(Endian::swap(m));
}

}

#endif /* FOLLY_LANG_BITS_H_ */
