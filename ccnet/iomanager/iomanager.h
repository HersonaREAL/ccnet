#pragma once
#include <sys/epoll.h>
#include <atomic>
#include <functional>
#include <memory>
#include <schedule.h>
#include <vector>

namespace ccnet {

class IOManager : public Scheduler
{
public:
    using ptr = std::shared_ptr<IOManager>;
    using LockType = RWMutex;
    
    enum Event {
        NONE  = 0x0,
        READ  = EPOLLIN,
        WRITE = EPOLLOUT,
    };
private:
    struct FdContext
    {
        using LockType = Mutex;
        struct EventContext
        {
            Scheduler *scheduler = nullptr;
            Fiber::ptr fiber = nullptr;               //事件协程
            std::function<void()> callback; //事件回调
        };
        EventContext& getEvContext(Event ev);
        void resetEvContext(EventContext &ev_ctx);
        void triggerEvent(Event ev);
        int fd;
        EventContext readEv;     //读事件
        EventContext writeEv;    //写事件
        Event events = NONE;  //已注册事件
        LockType mutex;
    };

public:
    IOManager(size_t thread_size = 1, bool use_caller = true, const std::string &name = "");
    ~IOManager();

    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);

    static IOManager* GetThis();

private:
    void resizeCtxs(size_t size);

protected:
    void tickle() override;
    bool stopping() override;
    void onIdle() override;

private:
    int m_epfd = 0;
    int m_tickleFds[2];

    std::atomic_size_t m_pendingEvCnt = {0};
    LockType m_mutex;
    std::vector<FdContext*> m_fdCtxs;
};

}