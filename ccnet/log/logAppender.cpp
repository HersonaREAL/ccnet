#include "log.h"
#include <memory>

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

}