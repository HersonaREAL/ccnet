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

Logger::Logger(const std::string& name)
    : m_name(name) 
{

}

// 输出到每个appender
void Logger::log(LogLevel::Level level, LogEvent::ptr event) 
{
    if (level >= m_level) {
        for (const auto & ap : m_appenders) {
            ap->log(std::shared_ptr<Logger>(this), level, event);
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