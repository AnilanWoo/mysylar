#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <sys/syscall.h>
#include <unistd.h>
#include <cstdint>

namespace sylar
{

pid_t GetThreadId();

uint32_t GetFiberId();

}

#endif