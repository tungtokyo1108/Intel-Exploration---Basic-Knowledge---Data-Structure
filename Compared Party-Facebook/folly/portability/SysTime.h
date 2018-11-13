/*
 * SysTime.h
 * https://github.com/facebook/folly/blob/master/folly/portability/SysTime.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_SYSTIME_H_
#define FOLLY_PORTABILITY_SYSTIME_H_

#ifndef _WIN32
#include <sys/time.h>
#else
// Someone decided this was a good place to define timeval.....
#include <folly/portability/Windows.h>
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

extern "C" {
int gettimeofday(timeval* tv, struct timezone*);
void timeradd(timeval* a, timeval* b, timeval* res);
void timersub(timeval* a, timeval* b, timeval* res);
}
#endif

#endif /* FOLLY_PORTABILITY_SYSTIME_H_ */
