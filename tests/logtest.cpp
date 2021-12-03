#include <iostream>
#include <thread>
#include "../ccnet/log/log.h"
#include "../ccnet/utils/utils.h"


using namespace ccnet;
int main(int argc, char *argv[]) { 
    Logger::ptr logger(new Logger());
    logger->addAppender(LogAppender::ptr(new StdoutLogAppender()));

    LogEvent::ptr ev(new LogEvent(__FILE__, __LINE__, 0, getThreadId(), getFiberId(), time(NULL)));
    ev->getSs() << "test log";
    logger->log(LogLevel::DEBUG, ev);

    std::cout << "oh hi?" << std::endl;
    return 0;
}
