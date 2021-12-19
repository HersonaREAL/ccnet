#include <bits/stdint-uintn.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread.h>
#include <log.h>
#include <vector>
#include <unistd.h>

#define thread_cnt 16

uint64_t g_test_sem = 0;
ccnet::Semaphore g_sem(1);

uint64_t g_test_mutex = 0;
ccnet::Mutex g_mutex;

uint64_t g_test_mutex_raii = 0;
ccnet::Mutex g_mutex2;

uint64_t g_test_writeLock = 0;
ccnet::RWMutex g_RWmutex;

uint64_t g_test_readLock = 0;

void func1() {
    LOG_INFO() << "hi,  name: " << ccnet::Thread::GetName() <<", this->name: " <<  ccnet::Thread::GetThis()->getName()
               << ", id: " << ccnet::getThreadId() << ", this->id: " << ccnet::Thread::GetThis()->getId();

    // test sem
    for (size_t i = 0; i < 99999; i++) {
       g_sem.wait();
       g_test_sem++;
       g_sem.notify();
    }

    //test mutex
    for (size_t i = 0; i < 99999; i++) {
        g_mutex.lock();
        g_test_mutex++;
        g_mutex.unlock();
    }

    // test mutex
    for (size_t i = 0; i < 99999; i++) {
        ccnet::Mutex::MutexLock lock(g_mutex2);
        g_test_mutex_raii++;
    }

    // test writelock
    for (size_t i = 0; i < 99999; i++) {
        ccnet::RWMutex::WriteLock lock(g_RWmutex);
        g_test_writeLock++;
    }

    //test readlock
    for (size_t i = 0; i < 99999; i++) {
        ccnet::RWMutex::ReadLock lock(g_RWmutex);
        g_test_readLock++;
    }
}


inline uint64_t res() {
    uint64_t res = 0;
    for (size_t _ = 0; _ < thread_cnt; _++) {
        for (size_t i = 0; i < 99999; i++) {
            res++;
        }
    }
    return res;
}


int main() {
    std::vector<ccnet::Thread::ptr> vec;
    const uint64_t sum = res();
    for (size_t i = 0; i < thread_cnt; i++) {
        vec.push_back(std::make_shared<ccnet::Thread>(func1, "t_" + std::to_string(i)));
    }
    for (size_t i = 0; i < thread_cnt; i++) {
        vec[i]->join();
    }

#define Check(testname, val) \
    do { \
        std::cout << #testname << "[";\
        val == sum ? std::cout << "\033[32mPASS" : std::cout <<"\033[33mFAILD";\
        std::cout << "\033[0m]" << std::endl; \
    }while(0);

    Check(test_sem, g_test_sem)
    Check(test_mutex, g_test_mutex)
    Check(test_mutex_raii, g_test_mutex_raii)
    Check(test_write_lock, g_test_writeLock)
    Check(test_read_lock, g_test_readLock)
    std::cout << "hello, thread~" << std::endl;
    return 0;
}