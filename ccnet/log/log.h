#pragma once

#include <fstream>
#include <bits/stdint-uintn.h>
#include <cstdarg>
#include <cstdio>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <list>
#include <cstdint>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <set>
#include <utils.h>

#define CCNET_LOG_LEVEL(logger, level) \
		if (logger->getLevel() <= level) \
			ccnet::LogWrap(logger, level,  \
				std::make_shared<ccnet::LogEvent>(__FILE__, __LINE__,  0, \
											ccnet::getThreadId(), \
											ccnet::getFiberId(), time(NULL))).getSs()

#define CCNET_LOG_NAME(name) ccnet::LogMgr::Instance()->getLogger(name)

#define CCNET_LOG_DEBUG(logger) CCNET_LOG_LEVEL(logger, ccnet::LogLevel::DEBUG)
#define CCNET_LOG_INFO(logger) CCNET_LOG_LEVEL(logger, ccnet::LogLevel::INFO)
#define CCNET_LOG_WARN(logger) CCNET_LOG_LEVEL(logger, ccnet::LogLevel::WARN)
#define CCNET_LOG_ERROR(logger) CCNET_LOG_LEVEL(logger, ccnet::LogLevel::ERROR)
#define CCNET_LOG_FATAL(logger) CCNET_LOG_LEVEL(logger, ccnet::LogLevel::FATAL)

#define CCNET_LOG_FMT_LEVEL(logger, level, fmt, ...) \
		if (logger->getLevel() <= level) \
			ccnet::LogWrap(logger, level, \
				std::make_shared<ccnet::LogEvent>(__FILE__, __LINE__, 0, \
											ccnet::getThreadId(), \
											ccnet::getFiberId(), time(NULL))).format(fmt,##__VA_ARGS__)

#define CCNET_LOG_FMT_DEBUG(logger, fmt, ...) CCNET_LOG_FMT_LEVEL(logger, ccnet::LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define CCNET_LOG_FMT_INFO(logger, fmt, ...) CCNET_LOG_FMT_LEVEL(logger, ccnet::LogLevel::INFO, fmt, ##__VA_ARGS__)
#define CCNET_LOG_FMT_WARN(logger, fmt, ...) CCNET_LOG_FMT_LEVEL(logger, ccnet::LogLevel::WARN, fmt, ##__VA_ARGS__)
#define CCNET_LOG_FMT_ERROR(logger, fmt, ...) CCNET_LOG_FMT_LEVEL(logger, ccnet::LogLevel::ERROR, fmt, ##__VA_ARGS__)
#define CCNET_LOG_FMT_FATAL(logger, fmt, ...) CCNET_LOG_FMT_LEVEL(logger, ccnet::LogLevel::FATAL, fmt, ##__VA_ARGS__)


//for root log
#define CCNET_LOG_ROOT() ccnet::LogMgr::Instance()->getRoot()
#define LOG_DEBUG() CCNET_LOG_DEBUG(CCNET_LOG_ROOT())
#define LOG_INFO() CCNET_LOG_INFO(CCNET_LOG_ROOT())
#define LOG_WARN() CCNET_LOG_WARN(CCNET_LOG_ROOT())
#define LOG_ERROR() CCNET_LOG_ERROR(CCNET_LOG_ROOT())
#define LOG_FATAL() CCNET_LOG_FATAL(CCNET_LOG_ROOT())
#define LOG_FMT_DEBUG(fmt, ...) CCNET_LOG_FMT_DEBUG(CCNET_LOG_ROOT(), fmt, ##__VA_ARGS__)
#define LOG_FMT_INFO(fmt, ...) CCNET_LOG_FMT_INFO(CCNET_LOG_ROOT(), fmt, ##__VA_ARGS__)
#define LOG_FMT_WARN(fmt, ...) CCNET_LOG_FMT_WARN(CCNET_LOG_ROOT(), fmt, ##__VA_ARGS__)
#define LOG_FMT_ERROR(fmt, ...) CCNET_LOG_FMT_ERROR(CCNET_LOG_ROOT(), fmt, ##__VA_ARGS__)
#define LOG_FMT_FATAL(fmt, ...) CCNET_LOG_FMT_FATAL(CCNET_LOG_ROOT(), fmt, ##__VA_ARGS__)

#define LOG_DEBUG_EX(name) CCNET_LOG_DEBUG(CCNET_LOG_NAME(name))
#define LOG_INFO_EX(name) CCNET_LOG_INFO(CCNET_LOG_NAME(name))
#define LOG_WARN_EX(name) CCNET_LOG_WARN(CCNET_LOG_NAME(name))
#define LOG_ERROR_EX(name) CCNET_LOG_ERROR(CCNET_LOG_NAME(name))
#define LOG_FATAL_EX(name) CCNET_LOG_FATAL(CCNET_LOG_NAME(name))
#define LOG_FMT_DEBUG_EX(name, fmt, ...) CCNET_LOG_FMT_DEBUG(CCNET_LOG_NAME(name), fmt, ##__VA_ARGS__)
#define LOG_FMT_INFO_EX(name, fmt, ...) CCNET_LOG_FMT_INFO(CCNET_LOG_NAME(name), fmt, ##__VA_ARGS__)
#define LOG_FMT_WARN_EX(name, fmt, ...) CCNET_LOG_FMT_WARN(CCNET_LOG_NAME(name), fmt, ##__VA_ARGS__)
#define LOG_FMT_ERROR_EX(name, fmt, ...) CCNET_LOG_FMT_ERROR(CCNET_LOG_NAME(name), fmt, ##__VA_ARGS__)
#define LOG_FMT_FATAL_EX(name, fmt, ...) CCNET_LOG_FMT_FATAL(CCNET_LOG_NAME(name), fmt, ##__VA_ARGS__)


namespace ccnet {

class Logger;

// 日志事件
class LogEvent {
public:
	using ptr = std::shared_ptr<LogEvent>;
	LogEvent(const char *fileName, int32_t line, uint32_t elapse, 
				uint32_t threadId, uint32_t fiberId, uint64_t time);

	const char *getFile() const { return m_file; } 
	int32_t getLine() const { return m_line; }
	uint32_t getElapse() const { return m_elapse; }
	uint32_t getThreadId() const { return m_threadId; }
	uint32_t getFiberId() const { return m_fiberId; }
	uint64_t getTime() const { return m_time; }
	std::string getContent() const { return m_ss.str(); }
	std::stringstream &getSs() { return m_ss; }
private:
	const char* m_file = nullptr; 	//file name
	int32_t m_line = 0;           	//行号
	uint32_t m_elapse = 0;			// 程序启动至今耗时毫秒数
	uint32_t m_threadId = 0;	  	// thread id
	uint32_t m_fiberId = 0;	       	// 协程ID
	uint64_t m_time;			  	//时间戳	
	std::stringstream m_ss;		  
};

class LogLevel {
public:
	enum Level{
		UNKNOW = 0,
		DEBUG = 1,
		INFO = 2, 
		WARN = 3, 
		ERROR = 4, 
		FATAL = 5
	};

	static const char* ToString(LogLevel::Level level);
	static LogLevel::Level ToLevel(const std::string &str);
};


// 格式器
class LogFormatter {
public:
	using ptr = std::shared_ptr<LogFormatter>;
	LogFormatter(const std::string &pattern);

	std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

	const std::string &getPattern() const { return m_pattern; }
public:
	class FormatItem {
	public:
		using ptr = std::shared_ptr<FormatItem>;
		FormatItem (const std::string &fmt = "") {};
		virtual ~FormatItem() {}	
		virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) = 0;
	};

	void init();
private:
	std::string m_pattern;
	std::vector<FormatItem::ptr> m_items;
};

// 日志输出
class LogAppender {
public:
	using ptr = std::shared_ptr<LogAppender>;
	virtual ~LogAppender() {}

	virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
	void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
	void setFormatter(const std::string &str);
	LogFormatter::ptr getFormatter() const { return m_formatter; }
	void setLevel(LogLevel::Level level) { m_level = level; }
	LogLevel::Level getLevel() const { return m_level; }

	virtual std::string toYAML() const = 0;
protected:
	LogLevel::Level m_level = LogLevel::DEBUG;
	LogFormatter::ptr m_formatter;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
public:
	using ptr = std::shared_ptr<Logger>;
	Logger(const std::string& name = "root", const std::string& fmt = "");
	void log(LogLevel::Level level, LogEvent::ptr event);

	// log类型输出
	void debug(LogEvent::ptr ev);
	void info(LogEvent::ptr ev);
	void warn(LogEvent::ptr ev);
	void error(LogEvent::ptr ev);
	void fatal(LogEvent::ptr ev);

	void addAppender(LogAppender::ptr appender);
	void delAppender(LogAppender::ptr appender);
	void clearAppender() { m_appenders.clear(); }
	LogLevel::Level getLevel() const { return m_level; }
	void setLevel(LogLevel::Level lv) { m_level = lv; } 

	LogFormatter::ptr getFormatter() const { return m_formatter; }
	void setFormatter(const std::string &str);
	void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }

	const std::string& getName() const { return m_name; }
	std::string toYAML() const ;
private:
	std::string m_name;				//日志名称
	LogLevel::Level m_level;		//日志级别
	std::list<LogAppender::ptr> m_appenders;//Appender集合
	LogFormatter::ptr m_formatter;   //默认formater
};

// wrap, 用于宏的raii
class LogWrap {
public:
	LogWrap(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
		: m_event(event), m_logger(logger) ,
		  m_level(level)
		  {}	
	std::stringstream &getSs() { return m_event->getSs(); }
	void format(const char *fmt, ...)
	{
		char *buf;
		std::va_list args;
		va_start(args, fmt);
		int len = vasprintf(&buf, fmt, args);
		if (len != -1) {
			m_event->getSs() << std::string(buf, len);
			free(buf);
		}
		va_end(args);
	}  
	~LogWrap() { m_logger->log(m_level, m_event); }
private:
	LogWrap();
	LogEvent::ptr m_event;
	Logger::ptr m_logger;
	LogLevel::Level m_level;
};


// terminal
class StdoutLogAppender : public LogAppender {
public:
	using ptr = std::shared_ptr<StdoutLogAppender>;
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
	std::string toYAML() const override;
};

//file
class FileLogAppender : public LogAppender {
public:
	using ptr = std::shared_ptr<FileLogAppender>;
	FileLogAppender(const std::string &filename);
	void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
	bool reopen();
	std::string toYAML() const override;
private:
	std::string m_filename;
	std::ofstream m_filestream;
};


class LogManager
{
public:
	friend class Singleton<LogManager>;
	Logger::ptr getLogger(const std::string &name) {
		auto it = m_loggerMap.find(name);
		if (it != m_loggerMap.end()) {
			return it->second;
		}
		return m_loggerMap[name] = std::make_shared<Logger>(name);
	}
	Logger::ptr getRoot() const { return m_root; }

	std::string toYAML() const {
		std::stringstream ss;
		YAML::Node node;
		for (const auto &p : m_loggerMap) {
			YAML::Node tmpNode = YAML::Load(p.second->toYAML());
			node.push_back(tmpNode);
		}
		ss << node;
		return ss.str();
	}

private:
	LogManager() 
	{ 
		m_root.reset(new Logger()); 
		m_root->addAppender(StdoutLogAppender::ptr(new StdoutLogAppender())); 
	}
private:
	Logger::ptr m_root;
	std::unordered_map<std::string, Logger::ptr> m_loggerMap;
};


struct LogAppenderConf
{
	int type;
	LogLevel::Level level = LogLevel::UNKNOW;
	std::string file;
	std::string formatter;
	bool operator==(const LogAppenderConf& rhs) const {
		return type == rhs.type &&
			   level == rhs.level &&
			   file == rhs.file &&
			   formatter == rhs.formatter;		
	}
};

struct LogConf
{
	std::string name;
	std::string formatter;
	LogLevel::Level level;
	std::vector<LogAppenderConf> appenders;

	bool operator==(const LogConf& rhs) const {
		return name == rhs.name && 
			   formatter == rhs.formatter &&
			   level == rhs.level &&
			   appenders == rhs.appenders;		
	}
	bool operator<(const LogConf& rhs) const {
		return name < rhs.name;
	}
};


using LogMgr = Singleton<LogManager>;


}
