#include <cstddef>
#include <log.h>
#include <memory>
#include <string>
#include <vector>
#include "schedule.h"

namespace ccnet {

static thread_local Scheduler* t_scheduler = nullptr;

static thread_local Fiber* t_sc_fiber = nullptr;

    // 当前协程调度器
Scheduler* Scheduler::GetThis()
{
    return t_scheduler;
}
    // 调度器所在协程
Fiber* Scheduler::GetScFiber()
{
    return t_sc_fiber;
}

Scheduler::Scheduler(size_t thread_size, bool use_caller, const std::string &name)
    : m_name(name)
{
    CCNET_ASSERT(thread_size > 0);
    if (use_caller) {
        --thread_size;
        // Fiber的构建函数会自动创建该线程的主协程
        m_scFiber = std::make_shared<Fiber>(std::bind(&Scheduler::run, this));

        // 设置该线程调度器
        CCNET_ASSERT(GetThis() == nullptr);
        setThis();
        
        Thread::SetName(m_name);
        t_sc_fiber = m_scFiber.get();
        m_scThreadId = getThreadId();
        m_threadIds.push_back(m_scThreadId);
    } else {
        m_scThreadId = -1;
    }

    m_threadCnt = thread_size;
}

Scheduler::~Scheduler()
{
    CCNET_ASSERT(m_autoStop);
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

void Scheduler::start()
{
    LockType::Lock lock(m_mutex);
    if (!m_stopping)
        return;
    m_stopping = false;

    CCNET_ASSERT(m_threads.empty());
    m_threads.resize(m_threadCnt);
    for (size_t i = 0; i < m_threadCnt; i++) {
        m_threads[i] = std::make_shared<Thread>(std::bind(&Scheduler::run, this), 
                                        m_name + "_" + std::to_string(i));
        m_threadIds.push_back(m_threads[i]->getId());
    }
}

void Scheduler::stop()
{
    LOG_DEBUG() << "Scheduler stop tid = " << getThreadId();
    m_autoStop = true;
    // use caller 并且只有一个线程
    if (m_scFiber && m_threadCnt == 0
        && (m_scFiber->isEnd() || m_scFiber->getState() == Fiber::INIT) )
    {
        m_stopping = true;
        if (stopping())
            return;
    }

    m_stopping = true;

    for (size_t i = 0; i < m_threadCnt; i++) {
        tickle();
    }

    if (m_scFiber) {
        tickle();
        if (!isStop()) {
            // 切换到sc协程
            m_scFiber->call();
        }
    }

    // 退出该函数时会自动销毁
    std::vector<Thread::ptr> thr;
    {
        LockType::Lock lock(m_mutex);
        thr.swap(m_threads);
    }

    LOG_DEBUG() << "stop::Join tid = " << getThreadId();
    for (auto &ptr : thr)
        ptr->join();

}

    // 通知调度协程有任务
void Scheduler::tickle()
{
    // LOG_DEBUG() << "tickle! Tid = " << getThreadId() << " Fid = " << getFiberId();
}

    // 协程调度核心函数
void Scheduler::run()
{
    LOG_DEBUG() << "run, Tid = " << getThreadId();
    setThis();
    if (getThreadId() != m_scThreadId) {
        //后面虽然是调swapIn, 其实跟调用call没区别,都是跟MainFiber交换
        t_sc_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fb = std::make_shared<Fiber>(std::bind(&Scheduler::onIdle, this));
    Task task;

    for(;;) {
        task.reset();
        bool tickle_me = false;
        {
            LockType::Lock lock(m_mutex);
            auto it = m_tasks.begin();
            while (it != m_tasks.end()) {
                // -1代表任意进程可执行
                if (it->thread_id != -1 && getThreadId() != it->thread_id) {
                    ++it;
                    tickle_me = true;
                    continue;
                }
                CCNET_ASSERT(it->fiber || it->callback);

                if (it->fiber && it->fiber->getState() == Fiber::RUNNING) {
                    ++it;
                    continue;
                }

                // 取得任务
                task = std::move(*it);
                ++m_activeThreadCnt;
                m_tasks.erase(it);
                break;
            }
        }

        if (tickle_me) {
            tickle();
        }

        if (task.callback) {
            // cb创建协程
            task.fiber = std::make_shared<Fiber>(task.callback);
        } 

        if (task.fiber && !task.fiber->isEnd()) {
            // 是协程且没结束
            task.fiber->swapIn();
            --m_activeThreadCnt;

            if (task.fiber->getState() == Fiber::READY) {
                addTask(task.fiber);
            } else if (!task.fiber->isEnd()) {
                task.fiber->m_state = Fiber::SUSPEND;
            }
            continue;
        } 

        // 取不到task进入空闲状态
        if (idle_fb->isEnd()) break;

        ++m_idleThreadCnt;
        idle_fb->swapIn();
        --m_idleThreadCnt;

        if (!idle_fb->isEnd()) {
            idle_fb->m_state = Fiber::SUSPEND;
        }
    }
}

bool Scheduler::isStop()
{
    return m_stopping && m_autoStop
            && m_tasks.empty() && m_activeThreadCnt == 0;
}

    // 空闲时执行idle协程
void Scheduler::onIdle()
{
    LOG_DEBUG() << "onIdle, Fid = " << getFiberId();
    while(!isStop()) {
        Fiber::YieldToSuspend();
    }
}

    // 设置当前调度器
void Scheduler::setThis()
{
    t_scheduler = this;
}

}