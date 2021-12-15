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



void LogAppender::setFormatter(LogFormatter::ptr val, bool setFlag) {
    m_formatter = val;
    if (setFlag) {
        m_hasFormatter = m_formatter ? true : false;
        // std::cout << "hasFormatter: " << m_hasFormatter << ", formatter: " << m_formatter->getPattern() << std::endl;
    }
}

void LogAppender::setFormatter(const std::string &str, bool setFlag) {
    if (!str.empty()) {
        setFormatter(std::make_shared<LogFormatter>(str), setFlag);
    }
}

std::string StdoutLogAppender::toYAML() const  {
    YAML::Node node;
    std::stringstream ss;
    node["type"] = "StdoutLogAppender";
    node["level"] = LogLevel::ToString(m_level);
    if (m_hasFormatter && !m_formatter->isError()) {
        node["formatter"] = m_formatter->getPattern();
    }
    ss << node;
    return ss.str();
}


std::string FileLogAppender::toYAML() const  {
    YAML::Node node;
    std::stringstream ss;
    node["type"] = "FileLogAppender";
    node["level"] = LogLevel::ToString(m_level);
    node["file"] = m_filename;
    if (m_hasFormatter && !m_formatter->isError()) {
        node["formatter"] = m_formatter->getPattern();
    }
    ss << node;
    return ss.str();
}

}