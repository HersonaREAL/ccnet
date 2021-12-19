#pragma once
#include <pthread.h>
#include <semaphore.h>
#include <cstdint>
namespace ccnet {
class Semaphore
{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
private:
    Semaphore(const Semaphore &) = delete;
    Semaphore(const Semaphore &&) = delete;
    Semaphore& operator=(const Semaphore &) = delete;
private:
    sem_t m_semaphore;
};

template<class T>
class ScopedLockImp
{
public:
    ScopedLockImp(T& mutex)
        : m_mutex(mutex)
    {
        m_mutex.lock();
        m_islock = true;
    }

    ~ScopedLockImp()
    {
        unlock();
    }

    void lock() {
        if (!m_islock) {
            m_mutex.lock();
            m_islock = true;
        }
    }

    void unlock() {
         if (m_islock) {
            m_mutex.unlock();
        }
    }

private:
    bool m_islock = false;

    //不为引用的话，会执行T析构函数，导致pthread_destory执行多次
    T &m_mutex;
};

template<class T>
class WriteScopedLockImp
{
public:
    WriteScopedLockImp(T& mutex)
        : m_mutex(mutex)
    {
        m_mutex.wrlock();
        m_islock = true;
    }

    ~WriteScopedLockImp()
    {
        unlock();
    }

    void lock() {
        if (!m_islock) {
            m_mutex.wrlock();
            m_islock = true;
        }
    }

    void unlock() {
         if (m_islock) {
            m_mutex.unlock();
        }
    }

private:
    bool m_islock = false;

    //不为引用的话，会执行T析构函数，导致pthread_destory执行多次
    T &m_mutex;
};

template<class T>
class ReadScopedLockImp
{
public:
    ReadScopedLockImp(T& mutex)
        : m_mutex(mutex)
    {
        m_mutex.rdlock();
        m_islock = true;
    }

    ~ReadScopedLockImp()
    {
        unlock();
    }

    void lock() {
        if (!m_islock) {
            m_mutex.rdlock();
            m_islock = true;
        }
    }

    void unlock() {
         if (m_islock) {
            m_mutex.unlock();
        }
    }

private:
    bool m_islock = false;

    //不为引用的话，会执行T析构函数，导致pthread_destory执行多次
    T &m_mutex;
};

class Mutex 
{
public:
    using MutexLock = ScopedLockImp<Mutex>;
    Mutex() {
        pthread_mutex_init(&m_lock, nullptr);
    }

    ~Mutex() {
        pthread_mutex_destroy(&m_lock);
    }

    void lock() {
        pthread_mutex_lock(&m_lock);
    }

    void unlock() {
        pthread_mutex_unlock(&m_lock);
    }
private:
    pthread_mutex_t m_lock;
};


class RWMutex
{
public:
    using WriteLock = WriteScopedLockImp<RWMutex>;
    using ReadLock = ReadScopedLockImp<RWMutex>;

    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

}//ccnet