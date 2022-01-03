#include <cstddef>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ccnet.h>

using namespace ccnet;

void test_cb() {
    LOG_INFO() << "hhhhhhhhhhhhhhhhhhi~";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, SOCK_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "182.61.200.7", &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    IOManager::GetThis()->addEvent(sock, IOManager::READ, [](){
        LOG_INFO() << "read cb";
    });
    IOManager::GetThis()->addEvent(sock, IOManager::WRITE, [=](){
        LOG_INFO() << "write cb";
        IOManager::GetThis()->cancelEvent(sock, IOManager::READ);
        close(sock);
    });

}

void test1() {
    IOManager iom(1, true);
    // Scheduler sc(2);
    iom.addTask(test_cb);
    // sc.start();
    // sc.addTask(test_cb);
    // sc.stop();
}


int main() {
    test1();
    std::cout << "hello iomanager~" << std::endl;
    return 0;
}