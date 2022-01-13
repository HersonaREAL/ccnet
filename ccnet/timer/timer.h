#pragma once

#include <memory>
#include <functional>
#include <set>
#include <lock.h>
#include <vector>
namespace ccnet {

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>
{
public:
    using ptr = std::shared_ptr<Timer>;
    friend TimerManager;

private:
    // TODO 考虑有无更好的方法
    struct Comparator {
        bool operator() (const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };

private:
    Timer(uint64_t ms, std::function<void()> cb, 
            bool recurring, TimerManager *mgr);

    bool cancel();
    bool refresh();
    bool reset(uint64_t ms, bool from_now);
    
private:
    bool m_recurring = false; //循环定时器
    uint64_t m_ms = 0;          // 周期
    uint64_t m_next = 0;        // 下一次执行时间
    TimerManager *m_mgr = nullptr;
    std::function<void()> m_cb;
};


class TimerManager {
public:
    using LockType = RWMutex; 
    friend Timer;
    TimerManager();
    virtual ~TimerManager();

    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, 
            std::weak_ptr<void> weak_cond, bool recurring = false);

    void delTimer(Timer::ptr timer);

    uint64_t leftTimeGetNextTimer();

    void listExpiredCb(std::vector<std::function<void()>> &cbs);

protected:
    virtual void onInsertFront() = 0;

private:
    LockType m_lock;
    std::set<Timer::ptr, Timer::Comparator> m_timers;
};

}
