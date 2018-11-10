/*
 * Preprocessor.h
 * https://github.com/facebook/folly/blob/master/folly/Preprocessor.h
 *
 *  Created on: Nov 10, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PREPROCESSOR_H_
#define FOLLY_PREPROCESSOR_H_

/*
 * MSVC's preprocessor is a pain, so we have to forcefully
 * expand the VA args in some places
 */
#define FB_VA_GLUE(a, b) a b

/*
 * This macro is used to insert or eliminate text based on the presence of another argument
 */
#define FB_ONE_OR_NONE(a, ...) FB_VA_GLUE(FB_THIRD, (a, ##__VA_ARGS__, a))
#define FB_THIRD(a, b, ...) __VA_ARGS__

/*
 * Helper macro that extracts the first argument out of a list of any number of argument
 */
#define FB_ARG_1(a, ...) a

/*
 * Helper macro that extract the second argument out of a list of any number of arguments
 */
#ifdef _MSC_VER
#define FB_ARG_2_OR_1(...) \
FB_VA_GLUE(FB_ARG_2_OR_1_IMPL, (__VA_ARGS__, __VA_ARGS__))
#else
#define FB_ARG_2_OR_1(...) FB_ARG_2_OR_1_IMPL(__VA_ARGS__, __VA_ARGS__)
#endif
#define FB_ARG_2_OR_1_IMPL(a,b,...) b

/*
 * Helper macro that provides a way to pass argument with commas in it
 * to some other macro whose syntax doesn't allow using extra parentheses
 */
#define FB_SINGLE_ARG(...) __VA_ARGS__
#define FOLLY_PP_DETAIL_APPEND_VA_ARGS(...) , ##__VA_ARGS__

/*
 * Helper macro that just ignores its parameters
 */
#define FOLLY_IGNORE(...)

/*
 * Helper macro that just ignores its parameters and inserts a semicolon
 */
#define FOLLY_SEMICOLON(...) ;

/*
 * Introduce an identifier starting with str and
 * ending with a number that varies with the line
 */
#ifndef FB_ANONYMOUS_VARIABLE
#define FB_CONCATENATE_IMPL(s1, s2) s1##s2
#define FB_CONCATENATE(s1, s2) FB_CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define FB_ANONYMOUS_VARIABLE(str) FB_CONCATENATE(str, __COUNTER__)
#else
#define FB_ANONYMOUS_VARIABLE(str) FB_CONCATENATE(str, __LINE__)
#endif
#endif

#define FB_STRINGIZE(x) #x

#define FOLLY_PP_DETAIL_NARGS_1(dummy, _7, _6, _5, _4, _3, _2, _1, _0, ...) _0
#define FOLLY_PP_DETAIL_NARGS(...) \
  FOLLY_PP_DETAIL_NARGS_1(dummy, ##__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)

#define FOLLY_PP_DETAIL_FOR_EACH_REC_0(fn, ...)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_1(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_0(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_2(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_1(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_3(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_2(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_4(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_3(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_5(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_4(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_6(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_5(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_REC_7(fn, a, ...) \
  fn(a) FOLLY_PP_DETAIL_FOR_EACH_REC_6(fn, __VA_ARGS__)

#define FOLLY_PP_DETAIL_FOR_EACH_2(fn, n, ...) \
  FOLLY_PP_DETAIL_FOR_EACH_REC_##n(fn, __VA_ARGS__)
#define FOLLY_PP_DETAIL_FOR_EACH_1(fn, n, ...) \
  FOLLY_PP_DETAIL_FOR_EACH_2(fn, n, __VA_ARGS__)

/*
 * Used to invoke a preprocessor macro, the name of which is passed as the first argument
 * once for each subsequent variadic argument
 */
#define FOLLY_PP_FOR_EACH(fn, ...) \
  FOLLY_PP_DETAIL_FOR_EACH_1(      \
fn, FOLLY_PP_DETAIL_NARGS(__VA_ARGS__), __VA_ARGS__)

#endif /* FOLLY_PREPROCESSOR_H_ */
