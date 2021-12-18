#include <iostream>
#include <thread.h>

void func1() {
    std::cout << "output ssssssssssssssssssssssssssshit" << std::endl;
}

int main() {
    ccnet::Thread t1(func1, "shit thread");
    t1.join();
    std::cout << "hello, thread~" << std::endl;
    return 0;
}