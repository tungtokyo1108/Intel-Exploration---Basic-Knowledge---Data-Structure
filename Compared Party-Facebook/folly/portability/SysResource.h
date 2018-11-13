/*
 * SysResource.h
 * https://github.com/facebook/folly/blob/master/folly/portability/SysResource.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_SYSRESOURCE_H_
#define FOLLY_PORTABILITY_SYSRESOURCE_H_

#ifndef _WIN32
#include <sys/resource.h>
#else
#include <cstdint>

#include <folly/portability/SysTime.h>

#define PRIO_PROCESS 1

#define RLIMIT_CORE 0
#define RLIMIT_NOFILE 0
#define RLIMIT_DATA 0
#define RLIMIT_STACK 3
#define RLIM_INFINITY SIZE_MAX

#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN 0
#define RUSAGE_THREAD 0

using rlim_t = size_t;
struct rlimit {
  rlim_t rlim_cur;
  rlim_t rlim_max;
};

struct rusage {
  timeval ru_utime;
  timeval ru_stime;
  long ru_maxrss;
  long ru_ixrss;
  long ru_idrss;
  long ru_isrss;
  long ru_minflt;
  long ru_majflt;
  long ru_nswap;
  long ru_inblock;
  long ru_oublock;
  long ru_msgsnd;
  long ru_msgrcv;
  long ru_nsignals;
  long ru_nvcsw;
  long ru_nivcsw;
};

extern "C" {
int getrlimit(int type, rlimit* dst);
int getrusage(int who, rusage* usage);
int setrlimit(int type, rlimit* src);

int getpriority(int which, int who);
int setpriority(int which, int who, int value);
}
#endif


#endif /* FOLLY_PORTABILITY_SYSRESOURCE_H_ */
