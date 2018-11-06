/*
 * Config.h
 *
 *  Created on: Nov 6, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FACEBOOK_FOLLY_PORTABILITY_CONFIG_H_
#define FACEBOOK_FOLLY_PORTABILITY_CONFIG_H_


#ifdef FOLLY_PLATFORM_CONFIG
#include FOLLY_PLATFORM_CONFIG
#endif

#if __has_include(<features.h>)
#include <features.h> // @manual
#endif

#ifdef __ANDROID__
#include <android/api-level.h> // @manual
#endif

#ifdef __APPLE__
#include <Availability.h> // @manual
#endif

#endif /* FACEBOOK_FOLLY_PORTABILITY_CONFIG_H_ */
