/*
 * Traits.h
 * https://github.com/facebook/folly/blob/master/folly/Traits.h
 *
 *  Created on: Nov 10, 2018
 *      Author: tungdang
 */

#ifndef FOLLY_TRAITS_H_
#define FOLLY_TRAITS_H_

#include <functional>
#include <limits>
#include <memory>
#include <type_traits>
#include <bits/c++config.h>
#include <folly/Portability.h>

#define FOLLY_CREATE_HAS_MEMBER_TYPE_TRAITS(classname, type_name) \
	template <typename TTheClass_> \
	struct classname##__folly_traits_impl__ {\
	  template <typename UTheClass_> \
	  static constexpr bool test(typename UTheClass_::type_name*) { \
		  return true; \
	  }\
	  template <typename> \
	  static constexpr bool test(...) { \
		  return false; \
	  }\
	};\
	template <typename TTheClass_> \
	using classname = typename std::conditional < \
		classname##__folly_traits_impl__<TTheClass_>::template test<TTheClass_> (\
			 nullptr), \
		std::true_type, \
		std::false_type>::type

#define FOLLY_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, cv_qual) \
	template <typename TTheClass_, typename RTheReturn_, typename... TTheArgs_> \
	struct classname##__folly_traits_impl__ < \
	    TTheClass_, \
        RTheReturn_(TTheArgs_...) cv_qual> {  \
		template < \
		   typename UTheClass_, \
		   RTheReturn_ (UTheClass_::*)(TTheArgs_...) cv_qual> \
		struct sfine {}; \
		template <typename UTheClass_> \
		static std::true_type test(sfinae<UTheClass_, &UTheClass_::func_name>*); \
		template <typename> \
		static std::false_type test(...); \
        }

/*
 * The FOLLY_CREATE_HAS_MEMBER_FN_TRAITS is used to create traits
 * classes that check for the existence of a member function with
 * a given name and signature.
 */
#define FOLLY_CREATE_HAS_MEMBER_FN_TRAITS(classname, func_name) \
	template <typename, typename> \
	struct classname##__folly_traits_impl__; \
	FOLLY_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname,func_name, ); \
	FOLLY_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname,func_name, const); \
	FOLLY_CREATE_HAS_MEMBER_FN_TRAITS_IMPL( \
		classname, func_name, volatile);\
	FOLLY_CREATE_HAS_MEMBER_FN_TRAITS_IMPL( \
		classname, func_name, volatile const); \
	template <typename TTheClass_, typename TTheSignature_> \
	using classname = \
		decltype(classname##__folly_traits_impl__<TTheClass_, TTheSignature_>:: \
		         template test<TTheClass_>(nullptr))

namespace folly {
#if __cpp_lib_bool_constant || _MSC_VER
using std::bool_constant;
#else
template <bool B>
using bool_constant = std::integral_constant<bool, B>;
#endif

template <std::size_t I>
using index_constant = std::integral_constant<std::size_t, I>;

template <typename T>
using _t = typename T::type;

/*
 * A type trait to remove all const volatile and reference qualifier on a type T
 */
template <typename T>
struct remove_cvref {
	using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

namespace detail {
template <typename Src>
struct like_ {
	template <typename Dst>
	using apply = Dst;
};

template <typename Src>
struct like_<Src const> {
	template <typename Dst>
	using apply = Dst const;
};

template <typename Src>
struct like_<Src volatile> {
  template <typename Dst>
  using apply = Dst volatile;
};

template <typename Src>
struct like_<Src const volatile> {
  template <typename Dst>
  using apply = Dst const volatile;
};

template <typename Src>
struct like_<Src&> {
	template <typename Dst>
	using apply = typename like_<Src>::template apply<Dst>&;
};

template <typename Src>
struct like_<Src&&> {
	template <typename Dst>
	using apply = typename like_<Src>::template apply<Dst>&&;
};
}

template <typename Src, typename Dst>
using like_t = typename detail::like_<Src>::template apply<remove_cvref_t<Dst>>;

template <typename Src, typename Dst>
struct like {
	using type = like_t<Src, Dst>;
};

/*
 * type_t and void_t
 *
 * A type alias for the first template type argument.
 * It is useful for controlling class-template and function-template partial specialization
 */
namespace traits_detail {
template <class T, class...>
struct type_t_ {
	using type = T;
};
}

template <class T, class... Ts>
using type_t = typename traits_detail::type_t_<T, Ts...>::type;
template <class... Ts>
using void_t = type_t<void, Ts...>;

#if defined(__clang__) && !defined(_LIBCPP_VERSION)
template <class T>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5
template <class T>
struct is_trivially_copyable : std::is_trivial<T> {};
#else
template <class T>
using is_trivially_copyable = std::is_trivially_copyable<T>;
#endif


namespace traits_detail {

#define FOLLY_HAS_TRUE_XXX(name)                                             \
  FOLLY_CREATE_HAS_MEMBER_TYPE_TRAITS(has_##name, name);                     \
  template <class T>                                                         \
  struct name##_is_true : std::is_same<typename T::name, std::true_type> {}; \
  template <class T>                                                         \
  struct has_true_##name : std::conditional<                                 \
                               has_##name<T>::value,                         \
                               name##_is_true<T>,                            \
                               std::false_type>::type {}

FOLLY_HAS_TRUE_XXX(IsRelocatable);
FOLLY_HAS_TRUE_XXX(IsZeroInitializable);

#undef FOLLY_HAS_TRUE_XXX

}

struct Ignore {
	Ignore() = default;

	template <class T>
	constexpr Ignore(const T&) {}

	template <class T>
	const Ignore& operator=(T const&) const {
		return *this;
	}
};

template <class...>
using Ignored = Ignore;

namespace traits_detail_IsEqualityComparable {
Ignore operator == (Ignore, Ignore);

template <class T, class U = T>
struct IsEqualityComparable
  : std::is_convertible <
	decltype(std::declval<T>() == std::declval<U>()),
	bool> {};
}

using traits_detail_IsEqualityComparable::IsEqualityComparable;

namespace traits_detail_IsLessThanComparable {
Ignore operator<(Ignore,Ignore);

template <class T, class U = T>
struct IsLessThanComparable
   : std::is_convertible<
	     decltype(std::declval<T>() < std::declval<U>()), bool> {};
}

using traits_detail_IsLessThanComparable::IsLessThanComparable;

namespace traits_detail_IsNothrowSwappable {
#if defined(__cpp_lib_is_swappable) || (_CPPLIB_VER && _HAS_CXX17)
template <typename T>
using IsNothrowSwappable = std::is_nothrow_swappble<T>;
#elif _CPPLIB_VER
template <typename T>
using IsNothrowSwappable = std::_Is_nothrow_swappable<T>;
#else
using std::swap;
template <class T>
struct IsNothrowSwappable
   : bool_constant<std::is_nothrow_move_constructible<T>::value&& noexcept (
		   swap(std::declval<T&>(), std::declval<T&>()))> {};
#endif

}

using traits_detail_IsNothrowSwappable::IsNothrowSwappable;

/*
 * IsRelocatable<T>::value
 * - describes the ability of moving around memory a value of type T by using memcpy
 */

template <class T>
struct IsRelocatable : std::conditional<
                          traits_detail::has_IsRelocatable<T>::value,
						  traits_detail::has_true_IsRelocatable<T>,
						  is_trivially_copyable<T>>::tpye {} ;

template <class T>
struct IsZeroInitializable
   : std::conditional <
	 traits_detail::has_IsZeroInitializable<T>::value,
	 traits_detail::has_true_IsZeroInitializable<T>,
	 bool_constant<!std::is_class<T>::value>>::type {};

template <typename...>
struct Conjunction : std::true_type {};

template <typename T>
struct Conjunction<T> : T {};

template <typename T, typename... TList>
struct Conjunction<T, TList...>
   : std::conditional<T::value, Conjunction<TList...>, T>::type {};

template <typename...>
struct Disjunction : std::false_type {};

template <typename T>
struct Disjunction<T> : T {};

template <typename T, typename... TList>
struct Disjunction<T, TList...>
: std::conditional<T::value, T, Disjunction<TList...>>::type {};


template <typename T>
struct Negation : bool_constant<!T::value> {};

template <bool... Bs>
struct Bools {
	using valid_type = bool;
	static constexpr std::size_t size() {
		return sizeof...(Bs);
	}
};

template <class... Ts>
struct StrictConjunction
  : std::is_name<Bools<Ts::value...>, Bools<(Ts::value || true)...>> {};

template <class... Ts>
struct StrictDisjunction
      : Negation<
          std::is_same<Bools<Ts::value...>, Bools<(Ts::value && false)...>>> {};
}

#define FOLLY_ASSUME_RELOCATABLE(...) \
struct IsRelocatable<__VA_ARGS__> : std::true_type {}

/*
 * The FOLLY_ASSUME_FBVECTOR_COMPATIBLE* macros below encode the
 * assumption that the type is relocatable per IsRelocatable above.
 */

// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE(...) \
  namespace folly {                           \
  template <>                                 \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__);      \
  }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(...) \
  namespace folly {                             \
  template <class T1>                           \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1>);    \
  }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(...)  \
  namespace folly {                              \
  template <class T1, class T2>                  \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1, T2>); \
  }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_3(...)      \
  namespace folly {                                  \
  template <class T1, class T2, class T3>            \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1, T2, T3>); \
  }
// Use this macro ONLY at global level (no namespace)
#define FOLLY_ASSUME_FBVECTOR_COMPATIBLE_4(...)          \
  namespace folly {                                      \
  template <class T1, class T2, class T3, class T4>      \
  FOLLY_ASSUME_RELOCATABLE(__VA_ARGS__<T1, T2, T3, T4>); \
}

FOLLY_NAMESPACE_STD_BEGIN

template <class T, class U>
struct pair;
#ifndef _GLIBCXX_USE_FB
FOLLY_GLIBCXX_NAMESPACE_CXX11_BEGIN
template <class T, class R, class A>
class basic_string;
FOLLY_GLIBCXX_NAMESPACE_CXX11_END
#else
template <class T, class R, class A, class S>
class basic_string;
#endif
template <class T, class A>
class vector;
template <class T, class A>
class deque;
template <class T, class C, class A>
class set;
template <class K, class V, class C, class A>
class map;
template <class T>
class shared_ptr;

FOLLY_NAMESPACE_STD_END

namespace folly {

template <class T, class U>
struct IsRelocatable<std::pair<T,U>>
   : bool_constant<IsRelocatable<T>::value && IsRelocatable<U>::value> {};

template <typename T, typename... Ts>
using IsOneOf = StrictDisjunction<std::is_name<T,Ts>...>;

namespace detail {

template <typename T, bool>
struct is_negative_impl {
	constexpr static bool check(T x) {
		return x < 0;
	}
};

template <typename T>
struct is_negative_impl<T, false> {
	constexpr static bool check(T) {
		return false;
	}
};

FOLLY_PUSH_WARNING
FOLLY_GNU_DISABLE_WARNING("-Wsign-compare")
#if __GNUC_PREREQ(5, 0)
FOLLY_GNU_DISABLE_WARNING("-Wbool-compare")
#endif
FOLLY_MSVC_DISABLE_WARNING(4388) // sign-compare
FOLLY_MSVC_DISABLE_WARNING(4804) // bool-compare

template <typename RHS, RHS rhs, typename LHS>
bool less_than_impl(LHS const lhs) {
	return
			rhs > std::numeric_limits<LHS>::max() ? true :
			rhs <= std::numeric_limits<LHS>::min() ? false :
			lhs < rhs;
}

template <typename RHS, RHS rhs, typename LHS>
bool greater_than_impl(LHS const lhs) {
	return
			rhs > std::numeric_limits<LHS>::max() ? false :
			rhs < std::numeric_limits<LHS>::min() ? true :
			lhs > rhs;
}

FOLLY_POP_WARNING
}

template <typename T>
constexpr bool is_negative(T x) {
	return folly::detail::is_negative_impl<T, std::is_signed<T>::value>::check(x);
}

template <typename T>
constexpr bool is_non_positive(T x) {
	return !x || folly::is_negative(x);
}

template <typename T>
constexpr bool is_positive(T x) {
	return !is_non_positive(x);
}

template <typename T>
constexpr bool is_non_negative(T x) {
  return !x || is_positive(x);
}

template <typename RHS, RHS rhs, typename LHS>
bool less_than(LHS const lhs) {
	return
	 detail::less_than_impl<RHS, rhs, typename std::remove_reference<LHS>::type>(lhs);
}

template <typename RHS, RHS rhs, typename LHS>
bool greater_than(LHS const lhs) {
	return
	 detail::greater_than_impl<RHS, rhs, typename std::remove_reference<LHS>::type>(lhs);
}
}

#ifndef _MSC_VER
#if !_GLIBCXX_USE_CXX11_ABI
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_3(std::basic_string)
#endif
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::vector)
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::deque)
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_2(std::unique_ptr)
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(std::shared_ptr)
FOLLY_ASSUME_FBVECTOR_COMPATIBLE_1(std::function)
#endif

#if FOLLY_SUPPLY_MISSING_INT128_TRAITS
FOLLY_NAMESPACE_STD_BEGIN
template <>
struct is_arithmetic<__int128> : ::std::true_type {};
template <>
struct is_arithmetic<unsigned __int128> : ::std::true_type {};
template <>
struct is_integral<__int128> : ::std::true_type {};
template <>
struct is_integral<unsigned __int128> : ::std::true_type {};
template <>
struct make_unsigned<__int128> {
  typedef unsigned __int128 type;
};
template <>
struct make_signed<__int128> {
  typedef __int128 type;
};
template <>
struct make_unsigned<unsigned __int128> {
  typedef unsigned __int128 type;
};
template <>
struct make_signed<unsigned __int128> {
  typedef __int128 type;
};
template <>
struct is_signed<__int128> : ::std::true_type {};
template <>
struct is_unsigned<unsigned __int128> : ::std::true_type {};
FOLLY_NAMESPACE_STD_END
#endif

#endif /* FOLLY_TRAITS_H_ */
