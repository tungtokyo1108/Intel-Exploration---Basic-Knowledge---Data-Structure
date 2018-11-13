/*
 * SysSyscall.h
 * https://github.com/facebook/folly/blob/master/folly/portability/SysSyscall.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_SYSSYSCALL_H_
#define FOLLY_PORTABILITY_SYSSYSCALL_H_

#ifndef _WIN32
#include <sys/syscall.h>

#if defined(__APPLE__)
#define FOLLY_SYS_gettid SYS_thread_selfid
#elif defined(SYS_gettid)
#define FOLLY_SYS_gettid SYS_gettid
#else
#define FOLLY_SYS_gettid __NR_gettid
#endif
#endif

#endif /* FOLLY_PORTABILITY_SYSSYSCALL_H_ */
