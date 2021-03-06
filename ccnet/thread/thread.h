#pragma once

#include <cstdint>
#include <bits/stdint-uintn.h>
#include <string>
#include <thread>
#include <functional>
#include <memory>
#include <cassert>
#include <cstdint>
#include <pthread.h>
#include <semaphore.h>

#include "lock.h"

namespace ccnet {



class Thread 
{
public:
    using ptr = std::shared_ptr<Thread>;
    Thread(std::function<void()> cb, const std::string name);
    ~Thread();

    pid_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    static void* run(void *arg);
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb = []() { assert(0); };
    std::string m_name = "UNKNOW";
    Semaphore m_semaphore;
};

}//ccnet