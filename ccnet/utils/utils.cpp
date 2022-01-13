#include "utils.h"
#include <fiber.h>
#include <bits/stdint-uintn.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#include <execinfo.h>
#include <assert.h>
#include <sstream>
#include <vector>

namespace ccnet {
pid_t getThreadId() {
    return syscall(SYS_gettid);
}

uint32_t getFiberId() {
    return Fiber::GetFiberId();
}

void getBackTrace(std::vector<std::string> &bt, size_t size, size_t skip )
{
    void **funcAddrs = (void **)malloc(sizeof(void *) * size);
    size_t sz = ::backtrace(funcAddrs, size);

    char **strings = ::backtrace_symbols(funcAddrs, sz);
    if (!strings) {
        perror("backtrace_symbol");
        exit(EXIT_FAILURE);
    }

    for (size_t i  = skip; i < sz; i++) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(funcAddrs);
}

std::string backTraceToString(size_t size, size_t skip , const std::string &prefix )
{
    std::stringstream ss;
    std::vector<std::string> bt;
    getBackTrace(bt, size, skip);

    for (size_t i = 0; i < bt.size(); i++) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

uint64_t getCurrentMS()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000UL + (tv.tv_usec / 1000UL);
}

uint64_t getCurrentUS()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000000UL + tv.tv_usec;
}
}
