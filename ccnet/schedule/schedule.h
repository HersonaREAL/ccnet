#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <fiber.h>
#include <lock.h>
#include <thread.h>
#include <vector>
namespace ccnet {

class Scheduler
{
public:
    using ptr = std::shared_ptr<Scheduler>;
    using LockType = Mutex;
private:
    struct Task
    {
        using ptr = std::shared_ptr<Task>;
        Fiber::ptr fiber;
        std::function<void()> callback;
        pid_t thread_id;

        explicit Task(Fiber::ptr fb, pid_t id)
            : fiber(fb), thread_id(id) {}
        Task(std::function<void()> cb, pid_t id)
            : callback(cb), thread_id(id) {}
    };

public:
    // 当前协程调度器
    static Scheduler* GetThis();
    // 调度器所在协程
    static Fiber* GetScFiber();

public:
    Scheduler(size_t thread_size = 1, bool use_caller = true, const std::string &name = "");
    virtual ~Scheduler();

    void start();
    void stop();

    bool isStop();


    // 添加任务，可以是函数，也可以是协程指针
    template<typename ExecType>
    void addTask(ExecType exec, pid_t thread_id = -1)
    {
        bool need_tickle = false;
        {
            LockType::Lock lock(m_mutex);
            need_tickle = addTaskNoBlock(exec, thread_id);
        }

        if (need_tickle) {
            tickle();
        }
    }

    template<typename Iterator>
    void addTask(Iterator begin, Iterator end) {
        bool need_tickle = false;
        {
            LockType::Lock lock(m_mutex);
            while (begin != end) {
                need_tickle = addTaskNoBlock(*begin) || need_tickle;
                ++begin;
            }
        }
        if (need_tickle) {
            tickle();
        }
    }

protected:
    // 通知调度协程有任务
    virtual void tickle();

    // 协程调度核心函数
    void run();

    virtual bool stopping() { return isStop(); }

    // 空闲时执行idle协程
    virtual void onIdle();

    // 设置当前调度器
    void setThis();

    // 是否拥有空闲线程
    bool hasIdleThreads() { return m_idleThreadCnt > 0; }

private:

    // 非阻塞添加任务
    template<typename ExecType>
    bool addTaskNoBlock(ExecType exec, pid_t thread_id)
    {
        bool need_tickle = m_tasks.empty();
        auto task = Task(exec, thread_id);
        if (task.fiber || task.callback) {
           m_tasks.push_back(task);
        }
        return need_tickle;
    }

private:
    LockType m_mutex;
    // 线程池
    std::vector<Thread::ptr> m_threads;

    // 协程队列
    std::list<Task> m_tasks;

    // use_caller为真时的调度协程
    Fiber::ptr m_scFiber;

    std::string m_name;

protected:
    std::vector<pid_t> m_threadIds;
    size_t m_threadCnt = 0;    
    std::atomic_size_t m_activeThreadCnt = {0};
    std::atomic_size_t m_idleThreadCnt = {0};
    bool m_stopping = true;
    bool m_autoStop = false;

    //use_caller调度线程的ID
    pid_t m_scThreadId = 0;
};

}