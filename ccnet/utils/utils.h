#pragma once
#include <bits/stdint-uintn.h>
#include <cstdio>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace ccnet {

pid_t getThreadId(); 

uint32_t getFiberId(); 

}