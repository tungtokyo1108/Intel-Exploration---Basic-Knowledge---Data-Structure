/*
 * Bits.h
 * https://github.com/facebook/folly/blob/master/folly/lang/Bits.h
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
