/*
 * Invoke.h
 * https://github.com/facebook/folly/blob/master/folly/functional/Invoke.h
 *
 *  Created on: Nov 16, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_FUNCTIONAL_INVOKE_H_
#define FOLLY_FUNCTIONAL_INVOKE_H_

#include <functional>
#include <type_traits>
#include <utility>
#include <iostream>
#include <folly/Preprocessor.h>
#include <folly/Traits.h>

#if __cpp_lib_invoke >= 201411 || _MSC_VER
namespace folly {
using std::invoke;
}
#else
namespace folly {

template <typename F, typename... Args>
constexpr auto invoke(F&& f, Args&&... args) noexcept (
	noexcept(static_cast<F&&>(f)(static_cast<Args&&>(args)...)))
    -> decltype(static_cast<F&&>(f)(static_cast<Args&&>(args)...)) {
	return static_cast<F&&>(f)(static_cast<Args&&>(args)...);
}

template <typename M, typename C, typename... Args>
constexpr auto invoke(M(C::*d), Args&&... args)
    -> decltype(std::mem_fn(d)(static_cast<Args&&>(args)...)) {
	return std::mem_fn(d)(static_cast<Args&&>(args)...);
}
}
#endif

#if __cpp_lib_is_invocable >= 201703 || \
(_MSC_VER >= 1911 && _MSVC_LANG > 201402)

/* using override */ using std::invoke_result;
/* using override */ using std::invoke_result_t;
/* using override */ using std::is_invocable;
/* using override */ using std::is_invocable_r;
/* using override */ using std::is_nothrow_invocable;
/* using override */ using std::is_nothrow_invocable_r;
#else
namespace folly {
namespace invoke_detail {
template <typename F, typename... Args>
using invoke_result_ =
	decltype(invoke(std::declval<F>(), std::declval<Args>()...));

template <typename F, typename... Args>
struct invoke_nothrow_
    : bool_constant<noexcept(
          invoke(std::declval<F>(), std::declval<Args>()...))> {};

template <typename Void, typename F, typename... Args>
struct invoke_result {};

template <typename F, typename... Args>
struct invoke_result<void_t<invoke_result_<F, Args...>>, F, Args> {
	using type = invoke_result_<F, Args...>;
};

template <typename Void, typename F, typename... Args>
struct is_invocable : std::false_type {};

template <typename F, typename... Args>
struct is_invocable<void_t<invoke_result_<F, Args...>>, F, Args...>
    : std::true_type {};

template <typename Void, typename R, typename F, typename... Args>
struct is_invocable_r : std::false_type {};

template <typename R, typename F, typename... Args>
struct is_invocable_r<void_t<invoke_result_<F, Args...>>, R, F, Args...>
    : std::is_convertible<invoke_result_<F, Args...>, R> {};

template <typename Void, typename F, typename... Args>
struct is_nothrow_invocable : std::false_type {};

template <typename F, typename... Args>
struct is_nothrow_invocable<void_t<invoke_result_<F, Args...>>, F, Args...>
    : invoke_nothrow_<F, Args...> {};

template <typename Void, typename R, typename F, typename... Args>
struct is_nothrow_invocable_r : std::false_type {};

template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r<void_t<invoke_result_<F, Args...>>, R, F, Args...>
    : StrictConjunction<
          std::is_convertible<invoke_result_<F, Args...>, R>,
invoke_nothrow_<F, Args...>> {};

}

//  mimic: std::invoke_result, C++17
template <typename F, typename... Args>
struct invoke_result : invoke_detail::invoke_result<void, F, Args...> {};

//  mimic: std::invoke_result_t, C++17
template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

//  mimic: std::is_invocable, C++17
template <typename F, typename... Args>
struct is_invocable : invoke_detail::is_invocable<void, F, Args...> {};

//  mimic: std::is_invocable_r, C++17
template <typename R, typename F, typename... Args>
struct is_invocable_r : invoke_detail::is_invocable_r<void, R, F, Args...> {};

//  mimic: std::is_nothrow_invocable, C++17
template <typename F, typename... Args>
struct is_nothrow_invocable
    : invoke_detail::is_nothrow_invocable<void, F, Args...> {};

//  mimic: std::is_nothrow_invocable_r, C++17
template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r
    : invoke_detail::is_nothrow_invocable_r<void, R, F, Args...> {};
}
#endif

namespace folly {
namespace detail {

struct invoke_private_overload;

template <typename Invoke>
struct free_invoke_proxy {
 public:
  template <typename... Args>
  struct invoke_result : folly::invoke_result<Invoke, Args...> {};
  template <typename... Args>
  using invoke_result_t = folly::invoke_result_t<Invoke, Args...>;
  template <typename... Args>
  struct is_invocable : folly::is_invocable<Invoke, Args...> {};
  template <typename R, typename... Args>
  struct is_invocable_r : folly::is_invocable_r<R, Invoke, Args...> {};
  template <typename... Args>
  struct is_nothrow_invocable : folly::is_nothrow_invocable<Invoke, Args...> {};
  template <typename R, typename... Args>
  struct is_nothrow_invocable_r
      : folly::is_nothrow_invocable_r<R, Invoke, Args...> {};

  template <typename... Args>
  static constexpr auto invoke(Args&&... args) noexcept(
      noexcept(Invoke{}(static_cast<Args&&>(args)...)))
      -> decltype(Invoke{}(static_cast<Args&&>(args)...)) {
    return Invoke{}(static_cast<Args&&>(args)...);
  }

  using invoke_type = Invoke;
};
}
}

#define FOLLY_CREATE_FREE_INVOKE_TRAITS(classname, funcname, ...)    \
  namespace classname##__folly_detail_invoke_ns {                    \
    namespace classname##__folly_detail_invoke_ns_inline {           \
      FOLLY_PUSH_WARNING                                             \
      FOLLY_CLANG_DISABLE_WARNING("-Wunused-function")               \
      void funcname(::folly::detail::invoke_private_overload&);      \
      FOLLY_POP_WARNING                                              \
    }                                                                \
    using FB_ARG_2_OR_1(                                             \
        classname##__folly_detail_invoke_ns_inline                   \
            FOLLY_PP_DETAIL_APPEND_VA_ARG(__VA_ARGS__))::funcname;   \
    struct classname##__folly_detail_invoke {                        \
      template <typename... Args>                                    \
      constexpr auto operator()(Args&&... args) const                \
          noexcept(noexcept(funcname(static_cast<Args&&>(args)...))) \
              -> decltype(funcname(static_cast<Args&&>(args)...)) {  \
        return funcname(static_cast<Args&&>(args)...);               \
      }                                                              \
    };                                                               \
  }                                                                  \
  struct classname : ::folly::detail::free_invoke_proxy<             \
                         classname##__folly_detail_invoke_ns::       \
classname##__folly_detail_invoke> {}

namespace folly {
namespace detail {

template <typename Invoke>
struct member_invoke_proxy {
 public:
  template <typename O, typename... Args>
  struct invoke_result : folly::invoke_result<Invoke, O, Args...> {};
  template <typename O, typename... Args>
  using invoke_result_t = folly::invoke_result_t<Invoke, O, Args...>;
  template <typename O, typename... Args>
  struct is_invocable : folly::is_invocable<Invoke, O, Args...> {};
  template <typename R, typename O, typename... Args>
  struct is_invocable_r : folly::is_invocable_r<R, Invoke, O, Args...> {};
  template <typename O, typename... Args>
  struct is_nothrow_invocable
      : folly::is_nothrow_invocable<Invoke, O, Args...> {};
  template <typename R, typename O, typename... Args>
  struct is_nothrow_invocable_r
      : folly::is_nothrow_invocable_r<R, Invoke, O, Args...> {};

  template <typename O, typename... Args>
  static constexpr auto invoke(O&& o, Args&&... args) noexcept(
      noexcept(Invoke{}(static_cast<O&&>(o), static_cast<Args&&>(args)...)))
      -> decltype(Invoke{}(static_cast<O&&>(o), static_cast<Args&&>(args)...)) {
    return Invoke{}(static_cast<O&&>(o), static_cast<Args&&>(args)...);
  }

  using invoke_type = Invoke;
};
}
}

#define FOLLY_CREATE_MEMBER_INVOKE_TRAITS(classname, membername)              \
  struct classname##__folly_detail_member_invoke {                            \
    template <typename O, typename... Args>                                   \
    constexpr auto operator()(O&& o, Args&&... args) const noexcept(noexcept( \
        static_cast<O&&>(o).membername(static_cast<Args&&>(args)...)))        \
        -> decltype(                                                          \
            static_cast<O&&>(o).membername(static_cast<Args&&>(args)...)) {   \
      return static_cast<O&&>(o).membername(static_cast<Args&&>(args)...);    \
    }                                                                         \
  };                                                                          \
  struct classname : ::folly::detail::member_invoke_proxy<                    \
classname##__folly_detail_member_invoke> {}

#endif /* FOLLY_FUNCTIONAL_INVOKE_H_ */
