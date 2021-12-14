#pragma once
#include <bits/stdint-uintn.h>
#include <cstdio>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory>
#include <mutex>

namespace ccnet {

template<class T, bool is_thread_safe = true>
class Singleton
{
public:
    static std::shared_ptr<T> Instance() {
        //使用智能指针，程序结束时可析构掉对象
        static std::shared_ptr<T> t; 
        static std::mutex lock;
        if (!is_thread_safe) {
            if (!t)
                t.reset(new T);
            return t;
        }

        if (!t) {
            std::unique_lock<std::mutex> unique_locker(lock);
            if (!t)
                t.reset(new T);
        }
        return t;
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

}