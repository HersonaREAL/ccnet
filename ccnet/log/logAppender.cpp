#include "log.h"
#include <memory>
#include <sstream>

namespace ccnet {

FileLogAppender::FileLogAppender(const std::string &filename) 
    : m_filename(filename)
{
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level && m_filestream) {
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }

    m_filestream.open(m_filename);
    return !!m_filestream; //?
}


void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level) {
        std::cout << m_formatter->format(logger, level, event);
    }
}




void LogAppender::setFormatter(const std::string &str) {
    if (!str.empty()) {
        m_formatter = std::make_shared<LogFormatter>(str);
    }
}

std::string StdoutLogAppender::toYAML() const  {
    YAML::Node node;
    std::stringstream ss;
    node["type"] = "StdoutLogAppender";
    node["level"] = LogLevel::ToString(m_level);
    ss << node;
    return ss.str();
}


std::string FileLogAppender::toYAML() const  {
    YAML::Node node;
    std::stringstream ss;
    node["type"] = "FileLogAppender";
    node["level"] = LogLevel::ToString(m_level);
    node["file"] = m_filename;
    ss << node;
    return ss.str();
}

}