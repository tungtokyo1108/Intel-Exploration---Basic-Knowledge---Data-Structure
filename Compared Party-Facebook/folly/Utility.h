/*
 * Utility.h
 * https://github.com/facebook/folly/blob/master/folly/Utility.h
 *
 *  Created on: Nov 10, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_UTILITY_H_
#define FOLLY_UTILITY_H_

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <folly/CPortability.h>
#include <folly/Traits.h>

namespace folly {

/*
 * copy
 *
 * - Usable when have a function with two overload
 * - The purpose is to make copies and moves explicit without having to
 *   spell out the full type names
 * - in this case, for copies, to invoke copy constructors
 */
template <typename T>
constexpr typename std::decay<T>::type copy(T&& value) noexcept(
	noexcept(typename std::decay<T>::type(std::forward<T>(value)))) {
	return std::forward<T>(value);
}

#if __cpp_lib_as_const || _LIBCPP_STD_VER > 14 || _MSC_VER

using std::as_const;

#else
template <class T>
constexpr T const& as_const(T& t) noexcept {
	return t;
}

template <class T>
void as_const(T const&&) = delete;

#endif

template <typename Src, typename Dst>
constexpr like_t<Src, Dst>&& forward_like(Dst&& dst) noexcept {
	return static_cast<like_t<Src,Dst>&&>(std::forward<Dst>(dst));
}

#if __cpp_lib_exchange_function || _LIBCPP_STD_VER > 11 || _MSC_VER

using std::exchange;

#else
template <class T, class U = T>
T exchange(T& obj, U&& new_value) {
	T old_value = std::move(obj);
	obj = std::forward<U>(new_value);
	return old_value;
}
#endif

namespace utility_detail {
template <typename...>
struct make_seq_cat;
template <template <typename T, T...> class S,
          typename T,
		  T... Ta,
		  T... Tb,
		  T... Tc>
struct make_seq_cat<S<T,Ta...>, S<T,Tb...>, S<T, Tc...>> {
	using type =
	  S<T,
	    Ta...,
		(sizeof...(Ta) + Tb)...,
		(sizeof...(Ta) + sizeof...(Tb) + Tc)...>;
};

template <std::size_t Size>
struct make_seq {
	template <typename S0, typename S1>
	using apply = typename make_seq_cat <
		typename make_seq<Size/2>::template apply<S0,S1>,
		typename make_seq<Size/2>::template apply<S0,S1>,
		typename make_seq<Size%2>::template apply<S0,S1>>::type;
};

template <>
struct make_seq<1> {
	template <typename S0, typename S1>
	using apply = S1;
};
template <>
struct make_seq<0> {
  template <typename S0, typename S1>
  using apply = S0;
};
}

#if __cpp_lib_integer_sequence || _MSC_VER

 using std::index_sequence;
 using std::integer_sequence;

#else
 template <class T, T... Ints>
 struct integer_sequence {
   using value_type = T;

   static constexpr std::size_t size() noexcept {
     return sizeof...(Ints);
   }
 };

 template <std::size_t... Ints>
 using index_sequence = integer_sequence<std::size_t, Ints...>;
#endif


#if FOLLY_HAS_BUILTIN(__make_integer_seq) || _MSC_FULL_VER >= 190023918

template <typename T, std::size_t Size>
using make_integer_sequence = __make_integer_seq<integer_sequence, T, Size>;

#else

template <typename T, std::size_t Size>
using make_integer_sequence = typename utility_detail::make_seq<
    Size>::template apply<integer_sequence<T>, integer_sequence<T, 0>>;

#endif

template <std::size_t Size>
using make_index_sequence = make_integer_sequence<std::size_t, Size>;
template <class... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

struct in_place_tag {};
template <class>
struct in_place_type_tag {};
template <std::size_t>
struct in_place_index_tag {};

using in_place_t = in_place_tag (&)(in_place_tag);
template <class T>
using in_place_type_t = in_place_type_tag<T> (&)(in_place_type_tag<T>);
template <std::size_t I>
using in_place_index_t = in_place_index_tag<I> (&)(in_place_index_tag<I>);

inline in_place_tag in_place(in_place_tag = {}) {
  return {};
}
template <class T>
inline in_place_type_tag<T> in_place_type(in_place_type_tag<T> = {}) {
  return {};
}
template <std::size_t I>
inline in_place_index_tag<I> in_place_index(in_place_index_tag<I> = {}) {
  return {};
}

struct initlist_construct_t {};
constexpr initlist_construct_t initlist_construct{};

struct presorted_t {};
constexpr presorted_t presorted{};

struct unsorted_t {};
constexpr unsorted_t unsorted{};

template <typename T>
struct transparent : T {
  using is_transparent = void;
  using T::T;
};

struct Identity {
	template <class T>
	constexpr T&& operator()(T&&x) const noexcept {
		return static_cast<T&&>(x);
	}
};

namespace moveonly_ {
/*
 * Disallow copy but not move in derived types
 */
class MoveOnly {
protected:
	constexpr MoveOnly() = default;
	~MoveOnly() = default;

	MoveOnly(MoveOnly&&) = default;
	MoveOnly& operator=(MoveOnly&&) = default;
	MoveOnly(const MoveOnly&) = delete;
	MoveOnly& operator=(const MoveOnly&) = delete;
};
}

using MoveOnly = moveonly_::MoveOnly;

template <typename T>
constexpr auto to_signed(T const& t) -> typename std::make_signed<T>::type {
	using S = typename std::make_signed<T>::type;
	return std::numeric_limits<S>::max() < t ? -static_cast<S>(~t) + S{-1}
	                                         : static_cast<S>(t);
}

template <typename T>
constexpr auto to_unsigned(T const& t) -> typename std::make_unsigned<T>::type {
	using U = typename std::make_unsigned<T>::type;
	return static_cast<U>(t);
}

}

#endif /* FOLLY_UTILITY_H_ */
