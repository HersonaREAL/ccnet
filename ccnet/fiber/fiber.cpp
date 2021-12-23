#include <atomic>
#include <config.h>
#include <bits/stdint-uintn.h>
#include <exception>
#include <ucontext.h>

#include "fiber.h"

namespace ccnet {

static std::atomic_uint64_t s_fiber_id{0};
static std::atomic_uint64_t s_fiber_count{0};

static thread_local Fiber *t_fiber = nullptr;
static thread_local Fiber::ptr t_MainFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

class StackAllocator
{
public:
    static void *Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void *vp, size_t size) {
        free(vp);
    }
};

Fiber::Fiber()
{
    m_state = RUNNING;
    SetThis(this);

    if (getcontext(&m_ctx)) {
        CCNET_ASSERT_EX(0, "Fiber::Fiber(), getcontext error");
    }

    ++s_fiber_count;
    LOG_DEBUG() << "Fiber::Fiber";
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    : m_id(++s_fiber_id)
    , m_stacksize(stacksize)
    , m_cb(cb)
{
    //若没有主协程
    if (!t_MainFiber) {
        Fiber::ptr main_fiber(new Fiber());
        // 默认构造会把主协程设进去
        CCNET_ASSERT(t_fiber == main_fiber.get());
        t_MainFiber = main_fiber;
    }

    ++s_fiber_count;
    m_stacksize = stacksize == 0 ? g_fiber_stack_size->getVal() : stacksize;
    m_stack = StackAllocator::Alloc(m_stacksize);

    //得到当前上下文
    if (getcontext(&m_ctx)) {
        CCNET_ASSERT_EX(0, "Fiber::Fiber(cb, stacksize), getcontext error");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    //setcontext时跳转到MainFunc
    makecontext(&m_ctx, Fiber::MainFunc, 0);

    LOG_DEBUG() << "Fiber::Fiber, id = " << m_id;
}

Fiber::~Fiber()
{
    --s_fiber_count;
    if (m_stack) {
        CCNET_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        // 主协程？
        CCNET_ASSERT(!m_cb);
        CCNET_ASSERT(m_state == RUNNING);

        Fiber *cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }

    LOG_DEBUG() << "~fiber(), id = " << m_id;
}

    // 重置协程, 处于INIT, TERM状态才行
void Fiber::reset(std::function<void()> cb)
{
    CCNET_ASSERT(m_stack);
    CCNET_ASSERT(m_state == INIT || m_state == TERM || m_state == EXCEPT);
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        CCNET_ASSERT_EX(0, "Fiber::reset(cb), getcontext error");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, Fiber::MainFunc, 0);
    m_state = INIT;
}
    // 切换到当前协程
void Fiber::swapIn()
{
    SetThis(this);
    CCNET_ASSERT(m_state != RUNNING);
    m_state = RUNNING;

    if (swapcontext(&t_MainFiber->m_ctx, &m_ctx)) {
        CCNET_ASSERT_EX(0, "Fiber::swapIn, swapcontext error");
    }
}
    // 切出
void Fiber::swapOut()
{
    SetThis(t_MainFiber.get());

    if (swapcontext(&m_ctx, &t_MainFiber->m_ctx)) {
        CCNET_ASSERT_EX(0, "Fiber::swapOut, swapcontext error");
    }
}

void Fiber::SetThis(Fiber *f)
{
    t_fiber = f;
}
    // 返回当前协程
Fiber::ptr Fiber::GetThis()
{
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }

    // 创建主协程
    if (!t_MainFiber) {
        Fiber::ptr main_fiber(new Fiber());
        // 默认构造会把主协程设进去
        CCNET_ASSERT(t_fiber == main_fiber.get());
        t_MainFiber = main_fiber;
    }
    return t_fiber->shared_from_this();
}

uint64_t Fiber::GetFiberId()
{
    if (t_fiber)
        return t_fiber->getFiberId();
    return 0;
}

    // 协程挂起到后台，并设置为ready状态
void Fiber::YieldToReady()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

    // 协程挂起到后台，并设置为suspend状态
void Fiber::YieldToSuspend()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = SUSPEND;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers()
{
    return s_fiber_count;
}

//无条件跳转到Main
void Fiber::SwapToMainFunc()
{
    SetThis(t_MainFiber.get());
    if (setcontext(&t_MainFiber->m_ctx)) {
        CCNET_ASSERT_EX(0, "Fiber::SwapToMainFunc, swapcontext error");
    }
}

void Fiber::MainFunc()
{
    Fiber::ptr cur = GetThis();
    CCNET_ASSERT(cur);

    try {
        // cur在swapIn设置过了
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception &e) {
        cur->m_state = EXCEPT;
        LOG_ERROR() << "Fiber Except: "<< e.what();
    } catch (...) {
        cur->m_state = EXCEPT;
        LOG_ERROR() << "Fiber Except";
    }

    //swap出去后栈中还有只能指针,需要释放智能指针引用
    cur.reset();
    SwapToMainFunc();

    CCNET_ASSERT_EX(0, "can not reach here");
}

}