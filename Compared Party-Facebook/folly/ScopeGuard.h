/*
 * ScopeGuard.h
 * https://github.com/facebook/folly/blob/master/folly/ScopeGuard.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_SCOPEGUARD_H_
#define FOLLY_SCOPEGUARD_H_

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

#include <folly/Portability.h>
#include <folly/Preprocessor.h>
#include <folly/Utility.h>
#include <folly/lang/UncaughtExceptions.h>

namespace folly {
namespace detail {

/*
 * ScopeGuard is a general implementation of the "Initialization is Resource Acquisition" idiom.
 * Basically, it guarantees that a function is executed upon leaving current scope,
 * depending on the presence of a new uncaught exception.
 *
 * If the executeOnException template parameter is true, the function is executed
 * if a new uncaught exception is present at the end of the scope.
 * If the parameter is false, then the function is executed if no new uncaught
 * exceptions are present at the end of the scope.
 */

class ScopeGuardImplBase {
public:
	void dismiss() noexcept {
		dismissed_ = true;
	}

protected:
	ScopeGuardImplBase() noexcept : dismissed_(false) {}

	static void warnAboutToCash() noexcept;
	static ScopeGuardImplBase makeEmptyScopeGuard() noexcept {
		return ScopeGuardImplBase {};
	}

	template <typename T>
	static const T& asConst(const T& t) noexcept {
		return t;
	}

	bool dismissed_;
};

template <typename FunctionType, bool InvokeNoexcept>
class ScopeGuardImpl : public ScopeGuardImplBase {
public:
	explicit ScopeGuardImpl(FunctionType &fn) noexcept(
		std::is_nothrow_copy_constructible<FunctionType>::value)
	    : ScopeGuardImpl (
	    		asConst(fn),
				makeFailsafe(
					std::is_nothrow_copy_constructible<FunctionType>{},
					&fn)) {}

	explicit ScopeGuardImpl(const FunctionType &fn) noexcept(
		std::is_nothrow_copy_constructible<FunctionType>::value)
	    : ScopeGuardImpl (
	    		fn,
				makeFailSafe(
					std::is_throw_copy_constructible<FunctionType>{},
					&fn)) {}

	explicit ScopeGuardImpl(FunctionType& fn) noexcept (
		std::is_nothrow_copy_constructible<FunctionType>::value)
		: ScopeGuardImpl (
		    	std::move_if_noexcept(fn),
				makeFailSafe(
				    std::is_throw_copy_constructible<FunctionType>{},
					&fn)) {}

	ScopeGuardImpl(ScopeGuardImpl&& other) noexcept (
		std::is_nothrow_copy_constructible<FunctionType>::value)
		: function_(std::move_if_noexcept(other.function_)) {
		dismissed_ = exchange(other.dismissed_, true);
	}

	~ScopeGuardImpl() noexcept(InvokeNoexcept) {
		if (!dismissed_)
		{
			execute();
		}
	}

private:
	static ScopeGuardImplBase makeFailsafe(std::true_type, const void*) noexcept {
		return makeEmptyScopeGuard();
	}

	template <typename Fn>
	static auto makeFailsafe(std::false_type, Fn* fn) noexcept
	   -> ScopeGuardImpl<decltype(std::ref(*fn)), InvokeNoexcept> {
		return ScopeGuardImpl<decltype(std::ref(*fn)), InvokeNoexcept>{
			std::ref(*fn)};
	}

	template <typename Fn>
	explicit ScopeGuardImpl(Fn& fn, ScopeGuardImplBase&& failsafe)
	   : ScopeGuardImplBase{} , function_(std::forward<Fn>(fn)) {
		failsafe.dismiss();
	}

	void* operator new (std::size_t) = delete;

	void execute() noexcept(InvokeNoexcept) {
		if (InvokeNoexcept) {
			try {
				function_();
			} catch(...) {
				warnAboutToCash();
				std::terminate();
			}
		}
		else
		{
			function_();
		}
	}

	FunctionType function_;
};

template <typename F, bool INE>
using ScopeGuardImplDecay = ScopeGuardImpl<typename std::decay<F>::type, INE>;
}

template <typename F>
FOLLY_NODISCARD detail::ScopeGuardImplDecay<F, true> makeGuard(F&& f) noexcept (
		noexcept(detail::ScopeGuardImplDecay<F, true>(static_cast<F&&>(f)))) {
	return detail::ScopeGuardImplDecay<F, true>(static_cast<F&&>(f));
}

namespace detail {
#if defined(FOLLY_EXCEPTION_COUNT_USE_CXA_GET_GLOBALS) || \
    defined(FOLLY_EXCEPTION_COUNT_USE_GETPTD) ||          \
    defined(FOLLY_EXCEPTION_COUNT_USE_STD)

template <typename FunctionType, bool ExecuteOnException>
class ScopeGuardForNewException {
public:
	explicit ScopeGuardForNewException(const FunctionType& fn) : guard_(fn) {}

	explicit ScopeGuardForNewException(FunctionType& fn)
	  : guard_(std::move(fn)) {}

	ScopeGuardForNewException(ScopeGuardForNewException&& other) = default;

	~ScopeGuardForNewException() noexcept(ExecuteOnException) {
		if (ExecuteOnException != (exceptionCounter_ < uncaught_exceptions())) {
			guard_.dismiss();
		}
	}

private:
	void* operator new(std::size_t) = delete;
	void operator delete(void*) = delete;

	ScopeGuardImpl<FunctionType, ExecuteOnException> guard_;
	int exceptionCounter_{uncaught_exceptions()};
};

enum class ScopeGuardOnFail {};

template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>
operator+(detail::ScopeGuardOnFail, FunctionType& fn) {
	return ScopeGuardForNewException <
		typename std::decay<FunctionType>::type,
		true>(std::forward<FunctionType>(fn));
}

enum class ScopeGuardOnSuccess {};

template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>
operator+(ScopeGuardOnSuccess, FunctionType&& fn) {
	return ScopeGuardForNewException <
		typename std::decay<FunctionType>::type,
		false>(std::forward<FunctionType>(fn));
}
#endif

enum class ScopeGuardOnExit {};

template <typename FunctionType>
ScopeGuardImpl<typename std::decay<FunctionType>::type, true> operator+(
		detail::ScopeGuardOnExit,
		FunctionType&& fn) {
	return ScopeGuardImpl<typename std::decay<FunctionType>::type, true> (
		std::forward<FunctionType>(fn));
}
}
}

#define SCOPE_EXIT                               \
  auto FB_ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = \
      ::folly::detail::ScopeGuardOnExit() + [&]() noexcept

#if defined(FOLLY_EXCEPTION_COUNT_USE_CXA_GET_GLOBALS) || \
    defined(FOLLY_EXCEPTION_COUNT_USE_GETPTD) ||          \
    defined(FOLLY_EXCEPTION_COUNT_USE_STD)
#define SCOPE_FAIL                               \
  auto FB_ANONYMOUS_VARIABLE(SCOPE_FAIL_STATE) = \
      ::folly::detail::ScopeGuardOnFail() + [&]() noexcept

#define SCOPE_SUCCESS                               \
  auto FB_ANONYMOUS_VARIABLE(SCOPE_SUCCESS_STATE) = \
      ::folly::detail::ScopeGuardOnSuccess() + [&]()
#endif


#endif /* FOLLY_SCOPEGUARD_H_ */
