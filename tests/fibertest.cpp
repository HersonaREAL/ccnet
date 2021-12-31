#include <iostream>
#include <ccnet.h>
#include <memory>
#include <vector>
using namespace ccnet;

#define THREAD_NUM 8

void tiny_test() {
    std::cout << "shit test\n";
}

void FIBER_ROUTINE()
{
    LOG_INFO() << "fiber start! fiber id = " << getFiberId();
    Fiber::Yield();
    LOG_INFO() << "fiber end! fiber id = " << getFiberId();
    Fiber::Yield();
}

void TEST_FIBER()
{
    Fiber::ptr fb = std::make_shared<Fiber>(FIBER_ROUTINE);
    fb->call();
    fb->call();
    fb->call();
    LOG_INFO() << "thread end thread id = " << getThreadId();
}

void test_thread_fiber()
{
    std::vector<Thread::ptr> vec;
    for (size_t i = 0; i < THREAD_NUM; i++) {
        vec.push_back(std::make_shared<ccnet::Thread>(TEST_FIBER, "t_" + std::to_string(i)));
    }

    for (size_t i = 0; i < THREAD_NUM; i++) {
        vec[i]->join();
    }
}

int main() {
    // Fiber::GetThis();
    Thread::SetName("main");
    test_thread_fiber();
    std::cout << "hello fiber~" << std::endl;
    return 0;
}