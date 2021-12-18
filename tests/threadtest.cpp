#include <iostream>
#include <memory>
#include <string>
#include <thread.h>
#include <log.h>
#include <vector>
#include <unistd.h>

void func1() {
   LOG_INFO() << "hi,  name: " << ccnet::Thread::GetName() <<", this->name: " <<  ccnet::Thread::GetThis()->getName()
              << ", id: " << ccnet::getThreadId() << ", this->id: " << ccnet::Thread::GetThis()->getId();
    sleep(22);
}

int main() {
    std::vector<ccnet::Thread::ptr> vec;
    for (size_t i = 0; i < 10; i++) {
        vec.push_back(std::make_shared<ccnet::Thread>(func1, "t_" + std::to_string(i)));
    }
    for (size_t i = 0; i < 10; i++) {
        vec[i]->join();
    }
    std::cout << "hello, thread~" << std::endl;
    return 0;
}