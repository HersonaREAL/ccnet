#include <iostream>
#include <ccnet.h>
#include <memory>
using namespace ccnet;

void TEST_FIBER()
{
    LOG_INFO() << "fiber start!";
    Fiber::YieldToSuspend();
    LOG_INFO() << "fiber end!";
    Fiber::YieldToSuspend();
}

int main() {
    // Fiber::GetThis();
    Thread::SetName("main");
    Fiber::ptr fb = std::make_shared<Fiber>(TEST_FIBER);
    fb->swapIn();
    std::cout << "this is shit~" << std::endl;
    fb->swapIn();
    std::cout << "stil shit~" << std::endl;
    fb->swapIn();
    std::cout << "hello fiber~" << std::endl;
    return 0;
}