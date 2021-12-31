#include <ccnet.h>
#include <cstdio>
#include <memory>
#include <unistd.h>

using namespace ccnet;


void test_task() {
    int times = 3;
    while (times--) {
        LOG_DEBUG() << "tttttttest!"<< "time="<<times<<" tid = " << getThreadId();
        sleep(1);
        Fiber::YieldToReady();
    }
}



int main() {
    Scheduler sc(3, false, "test");
    // sc.addTask(test_task);
    sc.start();
    sc.addTask(test_task);
    sleep(3);
    sc.stop();
    std::cout << "Hello Schedule~" << std::endl;
}