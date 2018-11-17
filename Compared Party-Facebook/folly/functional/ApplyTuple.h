/*
 * ApplyTuple.h
 * https://github.com/facebook/folly/blob/master/folly/functional/ApplyTuple.h
 *
 *  Created on: Nov 16, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_FUNCTIONAL_APPLYTUPLE_H_
#define FOLLY_FUNCTIONAL_APPLYTUPLE_H_

#include <functional>
#include <tuple>
#include <utility>

#include <folly/Traits.h>
#include <folly/Utility.h>
#include <folly/functional/Invoke.h>

namespace folly {

template <typename Tuple>
using index_sequence_for_tuple =
		make_index_sequence<std::tuple_size<Tuple>::value>;

namespace detail {
namespace apply_tuple {
namespace adl {

using std::get;

struct ApplyInvoke {
	template <typename T>
	using seq = index_sequence_for_tuple<std::remove_reference_t<T>>;

	template <typename F, typename T, std::size_t... I>
	static constexpr auto invoke_(F&& f, T&& t, index_sequence<I...>) noexcept(
	      is_nothrow_invocable<F&&, decltype(get<I>(std::declval<T>()))...>::value)
	      -> invoke_result_t<F&&, decltype(get<I>(std::declval<T>()))...> {
	    return invoke(static_cast<F&&>(f), get<I>(static_cast<T&&>(t))...);
	}
};

template <
   typename Tuple,
   std::size_t... Indices,
   typename ReturnTuple =
		   std::tuple<decltype(get<Indices>(std::declval<Tuple>()))...>>
	auto forward_tuple(Tuple&& tuple, index_sequence<Indices...>) -> ReturnTuple {
	return ReturnTuple{get<Indices>(std::forward<Tuple>(tuple))...};
}

}
}
}

struct ApplyInvoke : private detail::apply_tuple::adl::ApplyInvoke {
public:
	template <typename F, typename T>
	constexpr auto operator()(F&& f, T&& t) const noexcept (
		noexcept(invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>())))
	-> decltype(invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{})) {
		return invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{});
	}
};

#if __cpp_lib_apply >= 201603

using std::apply;

#else
template <typename F, typename Tuple>
constexpr decltype(auto) apply(F&& func, Tuple&& tuple) {
	return ApplyInvoke{}(static_cast<F&&>(func), static_cast<Tuple&&>(tuple));
}
#endif

template <typename Tuple>
auto forwar_tuple(Tuple&& tuple) noexcept
	-> decltype(detail::apply_tuple::adl::forward_tuple(
		std::declval<Tuple>(),
		std::declval<
		   index_sequence_for_tuple<std::remove_reference_t<Tuple>>>())) {
	return detail::apply_tuple::adl::forward_tuple(
			std::forward<Tuple>(tuple),
			index_sequence_for_tuple<std::remove_reference_t<Tuple>>{});
}

template <typename F, typename Tuple>
struct apply_result : invoke_result<ApplyInvoke, F, Tuple> {};
template <typename F, typename Tuple>
using apply_result_t = invoke_result_t<ApplyInvoke, F, Tuple>;
template <typename F, typename Tuple>
struct is_applicable : is_invocable<ApplyInvoke, F, Tuple> {};
template <typename R, typename F, typename Tuple>
struct is_applicable_r : is_invocable_r<R, ApplyInvoke, F, Tuple> {};
template <typename F, typename Tuple>
struct is_nothrow_applicable : is_nothrow_invocable<ApplyInvoke, F, Tuple> {};
template <typename R, typename F, typename Tuple>
struct is_nothrow_applicable_r
: is_nothrow_invocable_r<R, ApplyInvoke, F, Tuple> {};

namespace detail {
namespace apply_tuple {

template <class F>
class Uncurry {
public:
	explicit Uncurry(F&& func) : func_(std::move(func)) {}
	explicit Uncurry(const F& func) : func_(func) {}

	template <class Tuple>
	auto operator() (Tuple&& tuple) const
	-> decltype(apply(std::declval<F>(), std::forward<Tuple>(tuple))) {
		return apply(func_, std::forward<Tuple>(tuple));
	}
private:
	F func_;
};
}
}


template <class F>
auto uncurry (F&& f)
-> detail::apply_tuple::Uncurry<typename std::decay<F>::type> {
	return detail::apply_tuple::Uncurry<typename std::decay<F>::type> (
			std::forward<F>(f));
}

#if __cpp_lib_make_from_tuple || (_MSC_VER >= 1910 && _MSVC_LANG > 201402)

 using std::make_from_tuple;

#else

namespace detail {
namespace apply_tuple {

template <class T>
struct Construct {
	template <class... Args>
	constexpr T operator() (Args&&... args) const {
		return T(std::forward<Args>(args)...);
	}
};
}
}

template <class T, class Tuple>
constexpr T make_from_tuple(Tuple&& t) {
	return apply(detail::apply_tuple::Construct<T>(), std::forward<Tuple>(t));
}

#endif

}

#endif /* FOLLY_FUNCTIONAL_APPLYTUPLE_H_ */
