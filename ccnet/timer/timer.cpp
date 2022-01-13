#include <algorithm>
#include <memory>
#include <log.h>
#include <utils.h>
#include <vector>
#include "timer.h"


namespace ccnet {

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring)
{
    Timer::ptr timer = std::make_shared<Timer>(ms, cb, recurring, this);
    bool insert_front = false;

    {
        LockType::WriteLock lock(m_lock);
        m_timers.insert(timer);
        insert_front = m_timers.begin()->get() == timer.get();
    }

    if (insert_front) {
        // TODO raise iomanager
        onInsertFront();
    }

    return timer;
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, 
    std::weak_ptr<void> weak_cond, bool recurring )
{
    auto new_cb = [weak_cond, cb]() {
        if (weak_cond.lock()) {
            cb();
        }
    };

   return addTimer(ms, new_cb, recurring);
}

void TimerManager::delTimer(Timer::ptr timer)
{
    // TODO
    CCNET_ASSERT(0);
}

uint64_t TimerManager::leftTimeGetNextTimer()
{
    LockType::ReadLock lock(m_lock);
    if (m_timers.empty()) {
        return -1;
    }

    uint64_t now = getCurrentMS();
    uint64_t firstNext = (*m_timers.begin())->m_next;

    return firstNext > now ? firstNext - now : 0;
}

void TimerManager::listExpiredCb(std::vector<std::function<void()>> &cbs)
{
    std::vector<Timer::ptr> expired_vec;
    {
        LockType::ReadLock lock(m_lock);
        if (m_timers.empty())  {
            return;
        }
    }

    LockType::WriteLock lock(m_lock);
    // 拿出过期timer
    Timer::ptr now_ptr = std::make_shared<Timer>(0, nullptr, false, this);
    auto it = m_timers.lower_bound(now_ptr);
    uint64_t now = now_ptr->m_next;
    while (it != m_timers.end() && (*it)->m_next == now) {
        ++it;
    }

    expired_vec.insert(expired_vec.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);

    // 处理循环定时器
    for (auto &ptr : expired_vec) {
        cbs.push_back(ptr->m_cb);
        if (ptr->m_recurring) {
            ptr->m_next = now + ptr->m_ms;
            m_timers.insert(ptr);
        } else {
            ptr->m_cb = nullptr;
        }
    }
}

bool Timer::Comparator::operator() (const Timer::ptr& lhs, const Timer::ptr& rhs) const
{
    if (!lhs && !rhs) {
        return false;
    }

    if (!lhs) {
        return true;
    }

    if (!rhs) {
        return false;
    }

    if (lhs->m_next < rhs->m_next) {
        return true;
    }

    if (rhs->m_next < lhs->m_next) {
        return false;
    }

    return rhs.get() - lhs.get() > 0;
}

Timer::Timer(uint64_t ms, std::function<void()> cb, 
            bool recurring, TimerManager *mgr)
    : m_recurring(recurring), m_ms(ms), m_mgr(mgr), m_cb(cb) 
{
    m_next = getCurrentMS() + m_ms;
}

bool Timer::cancel()
{
    TimerManager::LockType::WriteLock lock(m_mgr->m_lock);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_mgr->m_timers.find(shared_from_this());
        m_mgr->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::refresh()
{
    TimerManager::LockType::WriteLock lock(m_mgr->m_lock);
    if (!m_cb) {
        return false;
    }

    // delete first
    auto it = m_mgr->m_timers.find(shared_from_this());
    if (it == m_mgr->m_timers.end()) {
        return false;
    }

    // refresh
    m_mgr->m_timers.erase(it);
    m_next = getCurrentMS() + m_ms;

    // insert
    m_mgr->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    // same
    if (ms == m_ms && !from_now) {
        return false;
    }

    TimerManager::LockType::WriteLock lock(m_mgr->m_lock);
    if (!m_cb) {
        return false;
    }
    
    // delete first
    auto it = m_mgr->m_timers.find(shared_from_this());
    if (it == m_mgr->m_timers.end()) {
        return false;
    }

    // change
    m_mgr->m_timers.erase(it);
    m_next = from_now ?
            getCurrentMS() + ms : m_next - m_ms + ms;
    m_ms = ms;

    // insert
    m_mgr->m_timers.insert(shared_from_this());
    return true;

}

}// namespace
