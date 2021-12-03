#include "log.h"

namespace ccnet {

const char* LogLevel::ToString(LogLevel::Level level) 
{
    switch(level) {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARN:
        return "WARN";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::FATAL:
        return "FATAL";
    default:
        return "UNKNOW";          
    }
}

LogEvent::LogEvent(const char *fileName, int32_t line, uint32_t elapse, 
				uint32_t threadId, uint32_t fiberId, uint64_t time)
        : m_file(fileName), m_line(line), m_elapse(elapse), 
          m_threadId(threadId), m_fiberId(fiberId), m_time(time)
{

}

Logger::Logger(const std::string& name)
    : m_name(name), m_level(LogLevel::DEBUG) 
{
    m_formater.reset(new LogFormatter("%d   [%p]    <%f:%l>: %m %n"));
}

// 输出到每个appender
void Logger::log(LogLevel::Level level, LogEvent::ptr event) 
{
    if (level >= m_level) {
        /*为什么不使用 shared_ptr<A>(this)?
            会产生两个无关智能指针，导致this被释放两次
        */
        auto self = shared_from_this();
        for (const auto & ap : m_appenders) {
            ap->log(self, level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr ev) 
{
    log(LogLevel::DEBUG, ev);
}

void Logger::info(LogEvent::ptr ev) 
{
    log(LogLevel::INFO, ev);
}

void Logger::warn(LogEvent::ptr ev) 
{
    log(LogLevel::WARN, ev);
}

void Logger::error(LogEvent::ptr ev) 
{
    log(LogLevel::ERROR, ev);
}

void Logger::fatal(LogEvent::ptr ev) 
{
    log(LogLevel::FATAL, ev);
}

void Logger::addAppender(LogAppender::ptr appender) 
{
    if (!appender->getFormatter()) {
        appender->setFormatter(m_formater);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) 
{
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

}