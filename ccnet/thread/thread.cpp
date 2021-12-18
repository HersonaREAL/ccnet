#include <functional>
#include <log.h>
#include <pthread.h>
#include <stdexcept>
#include "thread.h"


namespace ccnet {

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

Thread::Thread(std::function<void()> cb, const std::string name)
    : m_cb(cb)
{
    if (!name.empty()) {
        m_name = name;
    }
    int ret = pthread_create(&m_thread, nullptr, Thread::run, this);

    if (ret) {
        LOG_ERROR() << "pthread_create fail, " << "name: " << name;
        throw std::logic_error("pthread_create error");
    }
    
}

Thread::~Thread()
{
    if (m_thread) {
        pthread_detach(m_thread);
    }

}

void Thread::join()
{
    if (m_thread) {
        int ret = pthread_join(m_thread, nullptr);
        if (ret) {
            LOG_ERROR() << "pthread_join fail, "<< "name: " << m_name << ", id: " << m_id; 
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void *arg)
{
    Thread *thread = static_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = getThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    
    //清空对象里的function，防止引用智能指针导致资源不释放
    std::function<void()> cb;

    cb.swap(thread->m_cb);
    cb();

    return nullptr;
}

Thread* Thread::GetThis()
{
    return t_thread;
}

const std::string& Thread::GetName()
{
    return t_thread_name;
}

void Thread::SetName(const std::string &name)
{
    if (t_thread) {
        t_thread->m_name = name;
    }

    t_thread_name = name;
}

}//ccnet;