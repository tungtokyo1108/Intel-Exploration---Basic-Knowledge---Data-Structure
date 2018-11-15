/*
 * ConstexprMath.h
 * https://github.com/facebook/folly/blob/master/folly/ConstexprMath.h
 *
 * Microsoft
 * https://github.com/tungtokyo1108/Microsoft-Exploration---Microsoft-Cloud---Cpprestsdk/blob/master/include/cpprest/details/SafeInt3.hpp
 *
 *  Created on: Nov 14, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_CONSTEXPRMATH_H_
#define FOLLY_CONSTEXPRMATH_H_

#include <cstdint>
#include <limits>
#include <type_traits>

namespace folly {

template <typename T>
struct constexpr_equal_to {
	constexpr bool operator() (T const& a, T const& b) const {
		return a == b;
	}
};

template <typename T>
struct constexpr_not_equal_to {
	constexpr bool operator() (T const& a, T const& b) const {
		return a != b;
	}
};

template <typename T>
struct constexpr_less {
	constexpr bool operator() (T const& a, T const& b) const {
		return a < b;
	}
};

template <typename T>
struct constexpr_less_equal {
	constexpr bool operator() (T const& a, T const& b) const {
		return a <= b;
	}
};

template <typename T>
struct constexpr_greater {
	constexpr bool operator() (T const& a, T const& b) const {
		return a > b;
	}
};

template <typename T>
struct constexpr_greater_equal {
	constexpr bool operator() (T const& a, T const& b) const {
		return a >= b;
	}
};

template <typename T>
constexpr T constexpr_max(T a) {
	return a;
}

template <typename T, typename... Ts>
constexpr T constexpr_max(T a, T b, Ts... ts) {
	return b < a ? constexpr_max(a, ts...) : constexpr_max(b, ts...);
}

template <typename T>
constexpr T constexpr_min(T a) {
	return a;
}

template <typename T, typename... Ts>
constexpr T constexpr_min(T a, T b, Ts... ts) {
	return b < a ? constexpr_min(b, ts) : constexpr_min(a, ts);
}

template <typename T, typename Less>
constexpr T const&
constexpr_clamp(T const& v, T const& lo, T const& hi, Less less) {
	return less(v, lo) ? lo : less(hi, v) ? hi : v;
}

template <typename T>
constexpr T const& constexpr_clamp(T const& v, T const& lo, T const& hi) {
	return constexpr_clamp(v, lo, hi, constexpr_less<T>());
}

namespace detail {

template <typename T, typename = void>
struct constexpr_abs_helper {};

template <typename T>
struct constexpr_abs_helper <
       T,
	   typename std::enable_if<std::is_floating_point<T>::value>::type> {
	static constexpr T go (T t) {
		return t < static_cast<T>(0) ? - t : t;
	}
};

template <typename T>
struct constexpr_abs_helper <
       T,
	   typename std::enable_if<
	   std::is_integral<T>::value && !std::is_same<T,bool>::value &&
	   std::is_unsigned<T>::value>::type> {
	static constexpr T go(T t)
	{
		return t;
	}
};

template <typename T>
struct constexpr_abs_helper <
       T,
       typename std::enable_if<
       std::is_integral<T>::value && !std::is_same<T, bool>::value &&
       std::is_signed<T>::value>::type> {
	static constexpr typename std::make_unsigned<T>::type go (T t) {
		return typename std::make_unsigned<T>::type(t < static_cast<T>(0) ? -t : t);
	}
};
}

template <typename T>
constexpr auto constexpr_abs(T t)
  -> decltype(detail::constexpr_abs_helper<T>::go(t)) {
	return detail::constexpr_abs_helper<T>::go(t);
}

namespace detail {

template <typename T>
constexpr T constexpr_log2_(T a, T e) {
	return e == T(1) ? a : constexpr_log2_(a + T(1), e / T(2));
}

template <typename T>
constexpr T constexpr_log2_cell_(T l2, T t) {
	return l2 + T(T(1) << l2 < t ? 1 : 0);
}

template <typename T>
constexpr T constexpr_square_(T t) {
	return t * t;
}
}

template <typename T>
constexpr T constexpr_log2(T t) {
	return detail::constexpr_log2_(T(0), t);
}

template <typename T>
constexpr T constexpr_log2_ceil(T t) {
	return detail::constexpr_log2_cell_(constexpr_log2(t), t);
}

template <typename T>
constexpr T constexpr_ceil(T t, T round) {
	return round == T(0)
	      ? t
	      : ((t + (t < T(0) ? T(0) : round - T(1))) / round) * round;
}

template <typename T>
constexpr T constexpr_pow(T base, std::size_t exp) {
	return exp == 0
		? T(1)
		: exp == 1 ? base
		: detail::constexpr_square_(constexpr_pow(base, exp / 2)) *
		  (exp % 2 ? base : T(1));
}

template <typename T>
constexpr std::size_t constexpr_find_last_set(T const t) {
	using U = std::make_unsigned_t<T>;
	return t == T(0) ? 0 : 1 + constexpr_log2(static_cast<U>(t));
}

namespace detail {
template <typename U>
constexpr std::size_t
constexpr_find_first_set_(std::size_t s, std::size_t a, U const u) {
	return s == 0 ? a
		: constexpr_find_first_set_(
				s/2, a + s * bool((u >> a) % (U(1) << s) == U(0)), u);
}
}

template <typename T>
constexpr std::size_t constexpr_find_first_set(T t) {
	using U = std::make_unsigned_t<T>;
	using size = std::integral_constant<std::size_t, sizeof(T) * 4>;
	return t == T(0)
		? 0
		: 1 + detail::constexpr_find_first_set_(size{}, 0, static_cast<U>(t));
}

template <typename T>
constexpr T constexpr_add_overflow_clamped(T a, T b) {
	using L = std::numeric_limits<T>;
	using M = std::intmax_t;
	static_assert(
	      !std::is_integral<T>::value || sizeof(T) <= sizeof(M),
	"Integral type too large!");

	return
	!std::is_integral<T>::value ? a + b :
	sizeof(T) < sizeof(M)
	? T(constexpr_clamp(M(a) + M(b), M(L::min()), M(L::max()))) :
	! (a < 0) ? a + constexpr_min(b, T(L::max() - a)) :
	! (b < 0) ? a + b :
			a * constexpr_max(b, T(L::min() - a));
}

template <typename T>
constexpr T constexpr_sub_overflow_clamped(T a, T b) {
	using L = std::numeric_limits<T>;
	using M = std::intmax_t;
	static_assert(
	      !std::is_integral<T>::value || sizeof(T) <= sizeof(M),
	"Integral type too large!");

	return
	  !std::is_integral<T>::value ? a - b :
	   std::is_unsigned<T>::value ? (a < b ? 0 : a - b) :
	   sizeof(T) < sizeof(M)
	   ? T(constexpr_clamp(M(a) - M(b), M(L::min()), M(L::max()))) :
		(a < 0) == (b < 0) ? a - b :
		L::min() < b ? constexpr_add_overflow_clamped(a, T(-b)) :
		a < 0 ? a - b :
		L::max();
}

/*
 * clamp_cast<>
 * - provides sane numeric conversions from float point numbers to integral numbers,
 *   and between different types of integral numbers.
 * - helps to avoid unexpected bugs introduced by bad conversions
 *   and undefined behavior like overflow when casting float point numbers to integral numbers
 */
template <typename Dst, typename Src>
constexpr typename std::enable_if<std::is_integral<Src>::value, Dst>::type
constexpr_clamp_cast(Src src) {
	static_assert(
	      std::is_integral<Dst>::value && sizeof(Dst) <= sizeof(int64_t),
	"constexpr_clamp_cast can only cast into integral type (up to 64bit)");

	using L = std::numeric_limits<Dst>;
	return
			std::is_signed<Src>::value == std::is_signed<Dst>::value
			? (
			  sizeof(Src) <= sizeof(Dst) ? Dst(src) :
			  Dst(constexpr_clamp(src, Src(L::min()), Src(L::max()))))
			: std::is_signed<Src>::value && std::is_unsigned<Dst>::value
			? (
			  src < 0 ? Dst(0) :
			  sizeof(Src) <= sizeof(Dst) ? Dst(src) :
			  Dst(constexpr_min(src, Src(L::max()))))
			: (
			  sizeof(Src) < sizeof(Dst) ? Dst(src) :
			  Dst(constexpr_min(src, Src(L::max()))));
}

namespace detail {

constexpr double kClampCastLowerBoundDoubleToInt64F = -9223372036854774784.0;
constexpr double kClampCastUpperBoundDoubleToInt64F = 9223372036854774784.0;
constexpr double kClampCastUpperBoundDoubleToUInt64F = 18446744073709549568.0;

constexpr float kClampCastLowerBoundFloatToInt32F = -2147483520.0f;
constexpr float kClampCastUpperBoundFloatToInt32F = 2147483520.0f;
constexpr float kClampCastUpperBoundFloatToUInt32F = 4294967040.0f;

template <typename D, typename S>
constexpr D constexpr_clamp_cast_helper(S src, S sl, S su, D dl, D du) {
	return src < sl ? dl : (src > su ? du : D(src));
}
}

template <typename Dst, typename Src>
constexpr typename std::enable_if<std::is_floating_point<Src>::value, Dst>::type
constexpr_clamp_cast(Src src) {
	static_assert(
	      std::is_integral<Dst>::value && sizeof(Dst) <= sizeof(int64_t),
	"constexpr_clamp_cast can only cast into integral type (up to 64bit)");

	using L = std::numeric_limits<Dst>;
	return
			(src != src) ? Dst(0) :
			sizeof(Src) > sizeof(Dst) ?
			detail::constexpr_clamp_cast_helper(
				src, Src(L::min()), Src(L::max()), L::min(), L::max()) :
			sizeof(Src) < sizeof(Dst) ? (
			  src >= 0.0
			  ? constexpr_clamp_cast<Dst>(
					  constexpr_clamp_cast<std::uint64_t>(double(src)))
			  : constexpr_clamp_cast<Dst>(
					  constexpr_clamp_cast<std::int64_t>(double(src)))) :
			std::is_same<Src, double>::value && std::is_same<Dst, int64_t>::value ?
				detail::constexpr_clamp_cast_helper(
					double(src),
					detail::kClampCastLowerBoundDoubleToInt64F,
					detail::kClampCastUpperBoundDoubleToInt64F,
					L::min(),
					L::max()) :
			std::is_same<Src, double>::value && std::is_same<Dst, uint64_t>::value ?
				detail::constexpr_clamp_cast_helper(
					double(src),
					0.0,
					detail::kClampCastUpperBoundDoubleToUInt64F,
					L::min(),
					L::max()) :
			std::is_same<Src, float>::value && std::is_same<Dst, int32_t>::value ?
				detail::constexpr_clamp_cast_helper(
					float(src),
					detail::kClampCastLowerBoundFloatToInt32F,
					detail::kClampCastUpperBoundFloatToInt32F,
					L::min(),
					L::max()) :
				detail::constexpr_clamp_cast_helper(
					float(src),
					0.0f,
					detail::kClampCastUpperBoundFloatToUInt32F,
					L::min(),
					L::max());
}
}

#endif /* FOLLY_CONSTEXPRMATH_H_ */
