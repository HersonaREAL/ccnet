#pragma once
#include <bits/stdint-uintn.h>
#include <cstdio>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory>
#include <mutex>
#include <vector>

namespace ccnet {

template<class T, bool is_thread_safe = true>
class Singleton
{
public:
    // static std::shared_ptr<T> Instance() {
    //     //使用智能指针，程序结束时可析构掉对象
    //     static std::shared_ptr<T> t; 
    //     static std::mutex lock;
    //     if (!is_thread_safe) {
    //         if (!t)
    //             t.reset(new T);
    //         return t;
    //     }

    //     if (!t) {
    //         std::unique_lock<std::mutex> unique_locker(lock);
    //         if (!t)
    //             t.reset(new T);
    //     }
    //     return t;
    // }

    //c++11 thread safe singleton
    static T* Instance() {
        static T t;
        return &t;
    }

    ~Singleton() = delete;
    Singleton(T&&) = delete;
    Singleton(const T&&) = delete;
    void operator= (const T&) = delete;
protected:
    Singleton() = default;
};

pid_t getThreadId(); 

uint32_t getFiberId(); 

void getBackTrace(std::vector<std::string> &bt, size_t size, size_t skip = 1);

std::string backTraceToString(size_t size, size_t skip = 2, const std::string &prefix = "");

#define CCNET_ASSERT(expr) \
    do {\
        if (expr) break;\
        LOG_ERROR() << "\n\033[31mAssertion\033[0m: " << #expr \
                    << "\n\033[33mbacktrack\033[0m: \n" \
                    << ccnet::backTraceToString(100, 2, "   ");\
        assert(expr);\
    }while(0);

#define CCNET_ASSERT_EX(expr,fmt, ...) \
    do {\
        if (expr) break;\
        LOG_FMT_ERROR("\n\033[31mAssertion\033[0m: %s\n\033[33mbacktrack\033[0m: \n%s\nmessage: "#fmt,\
                      #expr, ccnet::backTraceToString(100, 2, "   ").c_str(), ##__VA_ARGS__);\
        assert(expr);\
    }while(0);

}