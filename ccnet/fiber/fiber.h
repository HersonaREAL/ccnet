#pragma once

#include <bits/stdint-uintn.h>
#include <memory>
#include <functional>
#include <sys/ucontext.h>
#include <ucontext.h>

namespace ccnet {

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    using ptr = std::shared_ptr<Fiber>;
    enum State {
        INIT,
        READY,
        RUNNING,
        SUSPEND,
        TERM,
        EXCEPT,
    };
    friend Scheduler;
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    // 重置协程, INIT, TERM
    void reset(std::function<void()> cb);

    void call();
    void ret();

    // 切换到当前协程
    void swapIn();
    // 切出
    void swapOut();

    uint64_t getFiberId() const { return m_id; }

    bool isEnd() const { return m_state == TERM || m_state == EXCEPT; }

    State getState() const { return m_state; }

public:
    static void SetThis(Fiber *f);
    // 返回当前协程
    static Fiber::ptr GetThis();

    static uint64_t GetFiberId();

    // 协程挂起到后台，并设置为ready状态
    static void YieldToReady();
    // 协程挂起到后台，并设置为suspend状态
    static void YieldToSuspend();

    static void Yield();

    static uint64_t TotalFibers();

    static void MainFunc();
    static void MainFuncForSchedule();
    static void SwapToMainFunc();
    static void SwapToScheduleMainFunc();
private:
    Fiber();

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;
    ucontext_t m_ctx;
    void *m_stack = nullptr;
    std::function<void()> m_cb;
};

}