#pragma once

#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <list>
#include <cstdint>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>

namespace ccnet {

// 日志事件
class LogEvent {
public:
	using ptr = std::shared_ptr<LogEvent>;
	LogEvent();
private:
	const char* m_file = nullptr; 	//file name
	int32_t m_line = 0;           	//行号
	uint32_t m_elapse = 0;			// 程序启动至今耗时毫秒数
	uint32_t m_threadId = 0;	  	// thread id
	uint32_t m_fiberId = 0;	       	// 协程ID
	uint64_t m_time;			  	//时间戳	
	std::string m_content;		  
};

class LogLevel {
public:
	enum Level{
		DEBUG = 1,
		INFO = 2, 
		WARN = 3, 
		ERROR = 4, 
		FATAL = 5
	};
};

// 格式器
class LogFormatter {
public:
	using ptr = std::shared_ptr<LogFormatter>;
	LogFormatter(const std::string &pattern);

	std::string format(LogEvent::ptr event);
private:
	class FormatItem {
	public:
		using ptr = std::shared_ptr<FormatItem>;
		virtual ~FormatItem() {}	
		virtual void format(std::ostream &os, LogEvent::ptr ev) = 0;
	};

	void init();
private:
	std::string m_pattern;
	std::vector<FormatItem> m_items;
};

// 日志输出
class LogAppender {
public:
	using ptr = std::shared_ptr<LogAppender>;
	virtual ~LogAppender() {}

	virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;
	void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
	LogFormatter::ptr getFormatter() const { return m_formatter; }
protected:
	LogLevel::Level m_level;
	LogFormatter::ptr m_formatter;
};

// 日志器
class Logger {
public:
	Logger(const std::string& name = "root");
	void log(LogLevel::Level level, LogEvent::ptr event);

	// log类型输出
	void debug(LogEvent::ptr ev);
	void info(LogEvent::ptr ev);
	void warn(LogEvent::ptr ev);
	void error(LogEvent::ptr ev);
	void fatal(LogEvent::ptr ev);

	void addAppender(LogAppender::ptr appender);
	void delAppender(LogAppender::ptr appender);
	LogLevel::Level getLevel() const { return m_level; }
	void setLevel(LogLevel::Level lv) { m_level = lv; } 
private:
	std::string m_name;				//日志名称
	LogLevel::Level m_level;		//日志级别
	std::list<LogAppender::ptr> m_appenders;//Appender集合
};


// terminal
class StdoutLogAppender : public LogAppender {
public:
	using ptr = std::shared_ptr<StdoutLogAppender>;
	void log(LogLevel::Level level, LogEvent::ptr event) override;
};

//file
class FileLogAppender : public LogAppender {
public:
	using ptr = std::shared_ptr<FileLogAppender>;
	FileLogAppender(const std::string &filename);
	void log(LogLevel::Level level, LogEvent::ptr event) override;
	bool reopen();
private:
	std::string m_filename;
	std::ofstream m_filestream;
};

}
