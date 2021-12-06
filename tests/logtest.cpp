#include <iostream>
#include <thread>
#include <log.h>
#include <utils.h>


using namespace ccnet;
int main(int argc, char *argv[]) { 
    Logger::ptr logger(new Logger());
    logger->addAppender(LogAppender::ptr(new StdoutLogAppender()));

    FileLogAppender::ptr file_ap(new FileLogAppender("./logtest.txt"));
    logger->addAppender(file_ap);
    file_ap->setFormatter(LogFormatter::ptr(new LogFormatter("%d%T[%p]%T[%c]: %m%n")));
    file_ap->setLevel(LogLevel::Level::ERROR);

    LogEvent::ptr ev(new LogEvent(__FILE__, __LINE__, 0, getThreadId(), getFiberId(), time(NULL)));
    ev->getSs() << "test log";
    logger->log(LogLevel::DEBUG, ev);

    CCNET_LOG_DEBUG(logger) << "test macro!!!!!!!!!!!!!!!" << 1 << "abc";
    CCNET_LOG_INFO(logger) << "test macro1!!!!!!!!!!!!!!!" << 2 << "def";
    CCNET_LOG_WARN(logger) << "test macro2!!!!!!!!!!!!!!!" << 3 << "ghi";
    CCNET_LOG_ERROR(logger) << "test macro3!!!!!!!!!!!!!!!"<< 4 << "jkl";
    CCNET_LOG_FATAL(logger) << "test macro4!!!!!!!!!!!!!!!"<< 5 << "www";

    CCNET_LOG_FMT_DEBUG(logger, "test fmt debug");
    CCNET_LOG_FMT_INFO(logger, "test fmt info");
    CCNET_LOG_FMT_WARN(logger, "test fmt warn");
    CCNET_LOG_FMT_ERROR(logger, "test fmt error");
 
    CCNET_LOG_FMT_DEBUG(logger, "test fmt debug,%d,%s,%c",111, "debug", 'd');
    CCNET_LOG_FMT_INFO(logger, "test fmt info,%d,%s,%c", 222, "info", 'i');
    CCNET_LOG_FMT_WARN(logger, "test fmt warn,%d,%s,%c", 333, "warn", 'w');
    CCNET_LOG_FMT_ERROR(logger, "test fmt error,%d,%s,%c", 444, "error", 'e');
    CCNET_LOG_FMT_FATAL(logger, "test fmt fatal,%d,%s,%c", 555, "fatal", 'f');   
    // CCNET_LOG_FMT_LEVEL(logger, LogLevel::DEBUG, "test fmt level");

    auto lg = LogMgr::Instance()->getLogger("test");
    CCNET_LOG_DEBUG(lg) << "test mgr";
    CCNET_LOG_FMT_INFO(lg, "shit mgr%d", 6666);

    CCNET_LOG_ERROR(CCNET_LOG_ROOT()) << "test ccnet log root";
    LOG_DEBUG() << "test root log debug";
    LOG_ERROR() << "error";
    LOG_FATAL() << "fatal";
    LOG_INFO() << "info";
    LOG_WARN() << "warn";

    LOG_FMT_DEBUG("test fmt debug");
    LOG_FMT_INFO("test fmt debug"); 
    LOG_FMT_WARN("test fmt debug");
    LOG_FMT_ERROR("test fmt debug");
    LOG_FMT_FATAL("test fmt debug");
    LOG_FMT_DEBUG("test fmt debug%d",666);
    LOG_FMT_INFO("test fmt debug%d",666); 
    LOG_FMT_WARN("test fmt debug%d",666);  
    LOG_FMT_ERROR("test fmt debug%d",666);   
    LOG_FMT_FATAL("test fmt debug%d",666);
    std::cout << "oh hi?" << std::endl;
    return 0;
}
