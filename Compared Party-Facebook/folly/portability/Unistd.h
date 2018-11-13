/*
 * Unistd.h
 * https://github.com/facebook/folly/blob/master/folly/portability/Unistd.h
 *
 *  Created on: Nov 13, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef FOLLY_PORTABILITY_UNISTD_H_
#define FOLLY_PORTABILITY_UNISTD_H_

#ifndef _WIN32

#include <unistd.h>

#else

#include <cstdint>

#include <sys/locking.h> // @manual

#include <folly/portability/SysTypes.h>

#define _SC_PAGESIZE 1
#define _SC_PAGE_SIZE _SC_PAGESIZE
#define _SC_NPROCESSORS_ONLN 2
#define _SC_NPROCESSORS_CONF 2
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define F_OK 0
#define X_OK F_OK
#define W_OK 2
#define R_OK 4
#define RW_OK 6

#define F_LOCK _LK_LOCK
#define F_ULOCK _LK_UNLCK

namespace folly {
namespace portability {
namespace unistd {
int access(char const* fn, int am);
int chdir(const char* path);
int close(int fh);
int dup(int fh);
int dup2(int fhs, int fhd);
int fsync(int fd);
int ftruncate(int fd, off_t len);
char* getcwd(char* buf, int sz);
int getdtablesize();
int getgid();
pid_t getpid();
pid_t getppid();
int getuid();
int isatty(int fh);
int lockf(int fd, int cmd, off_t len);
off_t lseek(int fh, off_t off, int orig);
ssize_t read(int fh, void* buf, size_t mcc);
int rmdir(const char* path);
int pipe(int pth[2]);
ssize_t pread(int fd, void* buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void* buf, size_t count, off_t offset);
ssize_t readlink(const char* path, char* buf, size_t buflen);
void* sbrk(intptr_t i);
unsigned int sleep(unsigned int seconds);
long sysconf(int tp);
int truncate(const char* path, off_t len);
int usleep(unsigned int ms);
ssize_t write(int fh, void const* buf, size_t count);
} // namespace unistd
} // namespace portability
} // namespace folly

/* using override */ using namespace folly::portability::unistd;

#endif

#endif /* FOLLY_PORTABILITY_UNISTD_H_ */
