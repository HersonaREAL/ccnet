#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <log.h>
#include <utils.h>
#include "iomanager.h"

namespace ccnet {
IOManager::IOManager(size_t thread_size , bool use_caller , const std::string &name)
    : Scheduler(thread_size, use_caller, name)
{
    m_epfd = epoll_create(8192);
    CCNET_ASSERT_EX(m_epfd > 0, "IOManager: epoll_create error");

    //TODO 
    int ret = pipe(m_tickleFds);
    CCNET_ASSERT_EX(ret == 0, "IOManager: pipe error");

    epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = m_tickleFds[0];

    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &ev);
    CCNET_ASSERT_EX(ret == 0, "IOManager: epoll_ctl error");

    //TODO resize fdContexts
    resizeCtxs(64);

    start();
}

IOManager::~IOManager()
{
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdCtxs.size(); i++) {
        if (m_fdCtxs[i])
            delete m_fdCtxs[i];
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    FdContext *fd_ctx = nullptr;
    LockType::ReadLock rlock(m_mutex);
    if (m_fdCtxs.size() > (size_t)fd) {
        fd_ctx = m_fdCtxs[fd];
        rlock.unlock();
    } else {
        rlock.unlock();
        //扩容
        LockType::WriteLock wlock(m_mutex);
        resizeCtxs(fd * 1.5);
        fd_ctx = m_fdCtxs[fd];
    }

    FdContext::LockType::Lock fdlock(fd_ctx->mutex);
    // 重复添加事件, assert!
    CCNET_ASSERT_EX(!(fd_ctx->events & event), 
        "add event assert! fd=%d, event=0x%x, fd_ctx->event=0x%x\n", 
        fd, event, fd_ctx->events);
    
    //操作epoll
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLET | fd_ctx->events | event;
    ev.data.ptr = fd_ctx;
    int ret = epoll_ctl(m_epfd, op, fd, &ev);
    if (ret) { 
        LOG_FMT_ERROR("addevnet: epoll_ctl error, epfd=%d, fd=%d, event=0x%x, errno=%s",
                m_epfd, fd_ctx->fd, ev.events, std::strerror(errno));
        return -1;
    }

    // 加入回调
    ++m_pendingEvCnt;
    fd_ctx->events = (Event)(fd_ctx->events | event);
    auto &ev_ctx = fd_ctx->getEvContext(event);
    CCNET_ASSERT(!ev_ctx.callback
                && !ev_ctx.fiber
                && !ev_ctx.scheduler);

    ev_ctx.scheduler = Scheduler::GetThis();
    if (cb) {
        ev_ctx.callback.swap(cb);
    } else {
        ev_ctx.fiber = Fiber::GetThis();
        CCNET_ASSERT(ev_ctx.fiber->getState() == Fiber::RUNNING);
    }

    return 0;
}

bool IOManager::delEvent(int fd, Event event)
{
    LockType::ReadLock rlock(m_mutex);
    if (m_fdCtxs.size() <= (size_t)fd) {
        return false;
    }

    FdContext* fd_ctx = m_fdCtxs[fd];
    rlock.unlock();

    FdContext::LockType::Lock fdlock(fd_ctx->mutex);
    if (!(fd_ctx->events & event)) {
        return false;
    }

    //删除event
    Event new_event = (Event)(fd_ctx->events & (~event));
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ev;
    ev.events = EPOLLET | new_event;
    ev.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &ev);
    if (ret) {
        LOG_FMT_ERROR("addevnet: epoll_ctl error, epfd=%d, fd=%d, event=0x%x",
                m_epfd, fd_ctx->fd, ev.events);
        return false;
    }

    --m_pendingEvCnt;
    fd_ctx->events = new_event; // 更新上下文的event, 不触发
    auto &ev_ctx = fd_ctx->getEvContext(event);
    fd_ctx->resetEvContext(ev_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event)
{
    LOG_DEBUG() << "call cancelEvent(" << fd << ", " << event <<")";
    LockType::ReadLock rlock(m_mutex);
    if (m_fdCtxs.size() <= (size_t)fd) {
        return false;
    }

    FdContext* fd_ctx = m_fdCtxs[fd];
    rlock.unlock();
    FdContext::LockType::Lock fdlock(fd_ctx->mutex);
    if (!(fd_ctx->events & event)) {
        return false;
    }
    Event new_event = (Event)(fd_ctx->events & (~event));
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ev;
    ev.events = EPOLLET | new_event;
    ev.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &ev);
    if (ret) {
        LOG_FMT_ERROR("addevnet: epoll_ctl error, epfd=%d, fd=%d, event=0x%x",
                m_epfd, fd_ctx->fd, ev.events);
        return false;
    }

    // 触发取消的事件，不从上下文中删除
    // LOG_DEBUG() << "cancel event, before trigger";
    fd_ctx->triggerEvent(event);
    --m_pendingEvCnt;
    return true;
}


bool IOManager::cancelAll(int fd)
{
    LockType::ReadLock rlock(m_mutex);
    if (m_fdCtxs.size() <= (size_t)fd) {
        return false;
    }

    FdContext* fd_ctx = m_fdCtxs[fd];
    rlock.unlock();

    FdContext::LockType::Lock fdlock(fd_ctx->mutex);

    int op = EPOLL_CTL_DEL;
    epoll_event ev;
    ev.events = 0;
    ev.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &ev);
    if (ret) {
        LOG_FMT_ERROR("addevnet: epoll_ctl error, epfd=%d, fd=%d, event=0x%x",
                m_epfd, fd_ctx->fd, ev.events);
        return false;
    }

    if (fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEvCnt;
    }
    if (fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEvCnt;
    }
    return true;
}

IOManager* IOManager::GetThis()
{
    return static_cast<IOManager*>(Scheduler::GetThis());
}

void IOManager::tickle() 
{
    // TODO
    LOG_DEBUG() << "iomanager::tickle!";
    if (!hasIdleThreads()) {
        return;
    }

    int ret = write(m_tickleFds[1], "?", 1);
    CCNET_ASSERT(ret == 1);
}

bool IOManager::stopping() 
{
    // TODO
    return Scheduler::stopping() && m_pendingEvCnt == 0;
}

void IOManager::onIdle() 
{
    // TODO
    epoll_event *evs = new epoll_event[64];
    std::shared_ptr<epoll_event> evs_ptr(evs, [](epoll_event *ptr) {
        delete[] ptr;
    });

    while (true) {
        int ret = 0;
        if (stopping()) {
            LOG_INFO() << "name = " << getName() <<"idle stop exit";
            break;
        }

        do {
            static const int MAX_TIMEOUT = 5000;
            LOG_DEBUG() << "enter epoll_wait!";
            ret = epoll_wait(m_epfd, evs, 64, MAX_TIMEOUT);
            LOG_DEBUG() << "exit on epoll_wait!";
            if (ret >= 0 || errno != EINTR) break;
        }while(true);

        for (int i = 0; i < ret; i++) {
            epoll_event& ev = evs[i];
            if (ev.data.fd == m_tickleFds[0]) {
                uint8_t dummy[256];
                while(read(m_tickleFds[0], &dummy, sizeof(dummy)) == 1);
                // read(m_tickleFds[0], &dummy, 1);
                continue;
            }

            FdContext* fd_ctx 
                = static_cast<FdContext*>(ev.data.ptr);
            
            FdContext::LockType::Lock lock(fd_ctx->mutex);
            if (ev.events & (EPOLLERR | EPOLLHUP)) {
                ev.events |= EPOLLIN | EPOLLOUT;
            }

            uint32_t ev_real = NONE;
            if (ev.events & EPOLLIN) {
                ev_real |= READ;
            }
            if (ev.events & EPOLLOUT) {
                ev_real |= WRITE;
            }

            if (!(fd_ctx->events & ev_real)) {
                continue;
            }

            uint32_t ev_left = fd_ctx->events & (~ev_real);
            int op = ev_left ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            ev.events = EPOLLET | ev_left;

            int ret2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &ev);
            if (ret2) {
                //TODO LOG
                continue;
            }

            if (ev_real & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEvCnt;
            }

            if (ev_real & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEvCnt;
            }
        }
        Fiber::YieldToSuspend();
    }
}

void IOManager::resizeCtxs (size_t size)
{
    m_fdCtxs.resize(size);
    for (size_t i = 0; i < m_fdCtxs.size(); i++) {
        if (!m_fdCtxs[i]) {
            m_fdCtxs[i] = new FdContext;
            m_fdCtxs[i]->fd = i;
        }
    }
}

IOManager::FdContext::EventContext& 
IOManager::FdContext::getEvContext(Event ev)
{
    switch (ev) {
    case READ:
        return readEv;
    case WRITE:
        return writeEv;
    default:
        CCNET_ASSERT(false);
    }
}

void IOManager::FdContext::resetEvContext(EventContext &ev_ctx)
{
    ev_ctx.callback = nullptr;
    ev_ctx.fiber.reset();
    ev_ctx.scheduler = nullptr;
}

void IOManager::FdContext::triggerEvent(Event ev)
{
    CCNET_ASSERT(events & ev);
    events = (Event)(events & ~ev);
    auto &ev_ctx = getEvContext(ev);
    CCNET_ASSERT(ev_ctx.scheduler);
    if (ev_ctx.callback) {
        ev_ctx.scheduler->addTask(ev_ctx.callback);
    } else {
        ev_ctx.scheduler->addTask(ev_ctx.fiber);
    }

    resetEvContext(ev_ctx);
}


}