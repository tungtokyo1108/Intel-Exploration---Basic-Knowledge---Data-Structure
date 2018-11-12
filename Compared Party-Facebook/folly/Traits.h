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

#endif /* FOLLY_TRAITS_H_ */
