#include <config.h>
#include <iostream>
#include <memory>
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
LogLevel::Level LogLevel::ToLevel(const std::string &str) 
{
#define Cast2Level(level_str) \
    if (str == #level_str) { \
        return LogLevel::level_str;\
    }
    Cast2Level(DEBUG)
    Cast2Level(INFO)
    Cast2Level(WARN)
    Cast2Level(ERROR)
    Cast2Level(FATAL)
#undef Cast2Level
    return LogLevel::UNKNOW;
}

LogEvent::LogEvent(const char *fileName, int32_t line, uint32_t elapse, 
				uint32_t threadId, uint32_t fiberId, uint64_t time)
        : m_file(fileName), m_line(line), m_elapse(elapse), 
          m_threadId(threadId), m_fiberId(fiberId), m_time(time)
{

}

Logger::Logger(const std::string& name, const std::string& fmt)
    : m_name(name), m_level(LogLevel::DEBUG) 
{
    if (fmt.empty())
        m_formater.reset(new LogFormatter("%d%T%t%T%F%T[%p]%T[%c]%T<%f:%l>: %m%n"));
    else
        m_formater.reset(new LogFormatter(fmt));
}

// 输出到每个appender
void Logger::log(LogLevel::Level level, LogEvent::ptr event) 
{
    if (level >= m_level) {
        /*为什么不使用 shared_ptr<A>(this)?
            会产生两个无关智能指针，导致this被释放两次
        */
        if (!m_appenders.empty()) { 
            auto self = shared_from_this();
            for (const auto & ap : m_appenders) {
                ap->log(self, level, event);
            }
        } else {
            // 没有appender默认使用root
            CCNET_LOG_ROOT()->log(level, event);
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
 
void Logger::setFormatter(const std::string &str) {
    if (!str.empty()) {
        m_formater = std::make_shared<LogFormatter>(str);
    }
}

struct LogIniter
{
	LogIniter() {
        auto g_logs_defs = g_logs_defines();
        g_logs_defs->addListener(0x5555555555555555, [](const std::set<LogConf>& old_val, const std::set<LogConf>& new_val) {
            //增 改
            for (const LogConf& conf : new_val) {
                auto it = old_val.find(conf);

                // 存在且相同， 下一个， 否则重置所有熟悉
                if (it != old_val.end() && (*it) == conf)
                    continue;

                // 不存在时会新增一个
                auto lg = CCNET_LOG_NAME(conf.name);
                lg->setLevel(conf.level);
                if (!conf.formatter.empty()) {
                    lg->setFormatter(conf.formatter);
                }

                lg->clearAppender();
                for (const auto &appender : conf.appenders) {
                    LogAppender::ptr ap;
                    switch(appender.type) {
                    case 0:
                        ap.reset(new StdoutLogAppender());
                        break;
                    case 1:
                        ap.reset(new FileLogAppender(appender.file));
                        break;
                    //etc.
                    default:
                        std::cerr << "g_logs_def listener: unknow appender type" << std::endl;
                        continue;
                    }

                    ap->setLevel(appender.level);
                    ap->setFormatter(appender.formatter);

                    lg->addAppender(ap);
                }
            }

            //TODO删除
        });
	}

	static ConfigVar<std::set<LogConf> >::ptr g_logs_defines() {
		static ConfigVar<std::set<LogConf> >::ptr g_logs_defines = Config::lookup("logs", std::set<LogConf>(), "logs configuration");
		return g_logs_defines;
	}
};

// main启动前初始化日志
static LogIniter __log_init;

}