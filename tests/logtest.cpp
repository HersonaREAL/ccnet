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

    CCNET_LOG_DEBUG(logger) << "test macro!!!!!!!!!!!!!!!" << 1 << "abc";
    CCNET_LOG_INFO(logger) << "test macro1!!!!!!!!!!!!!!!" << 2 << "def";
    CCNET_LOG_WARN(logger) << "test macro2!!!!!!!!!!!!!!!" << 3 << "ghi";
    CCNET_LOG_ERROR(logger) << "test macro3!!!!!!!!!!!!!!!"<< 4 << "jkl";
    CCNET_LOG_FATAL(logger) << "test macro4!!!!!!!!!!!!!!!"<< 5 << "www";

    std::cout << "oh hi?" << std::endl;
    return 0;
}
